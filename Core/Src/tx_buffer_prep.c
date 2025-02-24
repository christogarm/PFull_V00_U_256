


#include "main.h"
#include "customMain.h"


void tx_buffer_prep (void){

	uint8_t *point_X;
	uint8_t *point_Y;

tx_buffer_pr:
		// ((Paso:1)) Nota, se envia primero el header y (Paso:2) Despues la informacion y (Paso:3) checksum
		if(flagsTX [2]){							// Hay que transmitir Header ??
			goto	tx_header;
		}
		goto	tx_buffer;		//	jp		tx_buffer
//;----------------------------------------------------------------------------------------------------------------
tx_header:
									;//---- Carga datos a array de transmisión
	    point_Y = &Bloque_Header [softVersion1];	//&softVersion1;
	    point_X = &ImberaProtocolBuffer [0];
	    //uint8_t A_STM8;

load_header_tx:

		    STM8_A = *point_Y;          			// *ImberaProtocolBuffer = *softVersion1
	    	*point_X = STM8_A;
	    	build_chksum(STM8_A);
	    	point_X++;
	    	point_Y++;
	   if( point_Y <= &Bloque_Header [dataSize]){
		   goto load_header_tx;		//jrule load_header_tx
	   }

	    sizeTX = size_Header;					// Indica tamaño de header
		flagsTX [2] = 0;						// indica que ya se envió header
		goto	endTx;  //jp		endTx

//;----------------------------------------------------------------------------------------------------------------
tx_buffer:
		// (Paso:3) checksum
		if(!flagsTX [0]){					//btjf	flagsTX,#0,notest;		// Hay que tomar en cuenta que viene de memoria llena ?
			goto	notest;
		}
		if(!flagsTX [1]){					//btjf	flagsTX,#1,load_buffer_tx_prep
			goto	load_buffer_tx_prep;
		}
notest:
		//notest: : poner aqui el codigo de "notest"
		if(pointTx < pointEndTx){
			goto	load_buffer_tx_prep;					//jrult load_buffer_tx_prep
		}

//	;							btjt	flagsLogger,#4,testLoop
//	;							btjt	flagsLogger,#5,testLoop
//	;							jra		noTestLoop
//	;testLoop:
//	;							;btjf	flagsTX,#0,noTestLoop
//	;							nop
//	;							jra		testLoop
//	;noTestLoop:

chksum_tx:
		if(flagsTX [3]){			//btjt flagsTX,#3,prep_endTx; // ya se envió chksum ?
			goto	prep_endTx;
		}
		// LDW		Y,#chksum_HW;					// Apunta a bloque de chksum
		point_X = &ImberaProtocolBuffer[0];
		cntByteTx = 0;						// limpia contrador de datos cargados

		ImberaProtocolBuffer [0] = (uint8_t)((chksum_32_HW_LW & 0xFF000000)>> 24);
		ImberaProtocolBuffer [1] = (uint8_t)((chksum_32_HW_LW & 0x00FF0000)>> 16);
		ImberaProtocolBuffer [2] = (uint8_t)((chksum_32_HW_LW & 0x0000FF00)>>  8);
		ImberaProtocolBuffer [3] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);

		sizeTX = 4;					// Indica tamaño de chksum
		flagsTX [3] = 1;			// indica que ya se envió header
		goto	endTx;		//jp		endTx

prep_endTx:
		keyTx = 0;								// termina Tx
		flagsTX [1] = 0;
		flagsTX [0] = 0;
		flagsLogger [4] = 0;				// permite loggeo de eventos
		flagsLogger [5] = 0;				// permite loggeo de datos
		goto  endTx;  //			jra		endTx

load_buffer_tx_prep:
		//;//---- Carga datos a array de transmisión
		point_Y = pointTx;					// Apunta a los datos a loggear
		point_X = &ImberaProtocolBuffer[0];	// Apunta al buffer de datos en RAM
		cntByteTx = 0;								// limpia contrador de datos cargados
load_buffer_tx:
		STM8_A = *point_Y;



		if (flagsRxFirm [0]){
			goto  noLoadLoggerDir;		// btjt	flagsRxFirm,#0,noLoadLoggerDir
		}
		//; Sí se está transmitiendo logger considera su dirección
		if (flagsLogger [4]){
			goto  loadLoggerDir;		// btjt	flagsLogger,#4,loadLoggerDir
		}
		if (!flagsLogger [5]){
			goto  noLoadLoggerDir;		// btjf	flagsLogger,#5,noLoadLoggerDir
		}
loadLoggerDir:
		STM8_A = *point_Y;		//	LDF		A,($010000,Y)
noLoadLoggerDir:

		*point_X = STM8_A;
		*point_X++;
		*point_Y++;
		cntByteTx++;

		build_chksum(STM8_A);

		if(!flagsTX [0]){
			goto  no_clr_pointer;		// 	btjf	flagsTX,#0,no_clr_pointer; / no reinicies puntero si no hay que pasar por fin de logger
		}

		if(point_Y != loggerEnd){
			goto  no_clr_pointer;		// 	btjf	flagsTX,#0,no_clr_pointer; / no reinicies puntero si no hay que pasar por fin de logger

		}
		point_Y = loggerStart; 			// ldw		Y,loggerStart;							// si se desborda vuelve al inicio de la flash reservada
		flagsTX [1] = 1;				//bset	flagsTX,#1;				// indica que ya dió la vuelta
no_clr_pointer:

		//ld		A,cntByteTx;					// Ya se cargaron los 10 datos ?

		if(cntByteTx < blockSizeTX){
			goto	load_buffer_tx;			//jrult	load_buffer_tx;						// No, continua cargadno
		}
// cierra el : while(cntByteTx < blockSizeTX)

		pointTx = point_Y;						// guarda el nuevo puntero de transmisión
		sizeTX = blockSizeTX;
		goto	endTx;		 //jp		endTx



endTx:
}


