
/*
 * Resumen de comandos implementados
 0x4021					tx_control_handshake		..Completo Funciona Ok (Handshake)
 0x4053					tx_control_realTimeState	..Completo Funciona Ok (Estatus)
 0x4051					tx_control_parameters		..Completo Funciona Ok (Plantilla)
 0x4050					tx_control_writeParam		..

 */


#include "main.h"
#include "customMain.h"
#include "ModbusRTU.h"
#include "temper.h"
#include "ELTEC_EmulatedEEPROM.h"

uint32_t cksum_aux = 0;
uint32_t cksum_gral_aux = 0;

uint32_t pagina_borrado = 64;
uint32_t direccion_fw = 0x8020000;

uint32_t   chksum_to_compare = 0;
uint8_t    contador_bloques_fw = 16;


void tx_control(void){


	uint8_t *point_X;
	uint8_t *point_Y;


	//; Funciones de está seccción no se ejecutan hasta que la maquina de estados BLE esté en transmit/recieve
	//ld			A,BluetoothState
	//cp			A,#3
	//jrne		end_tx_control_b ; directamente sal de tx_control
	if(BluetoothState != 3)
		goto end_tx_control_b;

	//; / define el estado actual de la conexion correspondiente
	//tnz			delayComStat
	//jrne		end_tx_control
	if(delayComStat)
		goto end_tx_control;
statDef:
	//tnz			statComFlag
	//jreq		statDef_clr
	if(statComFlag==0)
		goto statDef_clr;
	//;si el pin de estado está en 1 , pregunta por que comunicacion está activa
	if(flagsTxControl[f_select])//btjt		flagsTxControl,#f_select,statDef_WIFI
		goto statDef_WIFI;
statDef_BLE:
	flagsTxControl[f_statBLE] = 1;	//bset		flagsTxControl,#f_statBLE
	//ldw		X,#300;
	timeoutTWF = 300;	//ldw		timeoutTWF,X;					/ manten carga time out de Token (5 min)
	for(uint8_t i = 0; i<8; i++)		//clr		flagsWIFI;/				limpia banderas para permitir logger mientras hay conexión BLE
		flagsWIFI[i] = 0;
	goto end_statDef;//jra			end_statDef
statDef_WIFI:
	flagsTxControl[f_statWIFI] = 1;	//bset		flagsTxControl,#f_statWIFI
	statComWIFIFlag = 255;			// mov			statComWIFIFlag,#255
	goto end_statDef;				// jra			end_statDef
statDef_clr:
	//;pregunta por que comunicacion está inactiva
	//btjt		flagsTxControl,#f_select,statDef_clrWIFI
	if(flagsTxControl[f_select])
		goto statDef_clrWIFI;
statDef_clrBLE:

	//btjf	flagsTxControl,#f_statBLE,noCancelTx ;// sólo si viene de una desconexión cancela la transmisión que estuviera en progreso
	if(!flagsTxControl[f_statBLE])
		goto noCancelTx;

	keyTx = 0;				//clr		keyTx  ;// en caso de desconexion cancela toda transmisión
	// clr		flagsTX
	//clr		flagsRxFirm
	for(uint8_t i=0 ; i<8 ; i++){
		flagsTX[i] = 0;
		flagsRxFirm[i] = 0;
	}

	flagsLogger[4] = 0;			//bres	flagsLogger,#4;				// permite loggeo de eventos
	flagsLogger[5] = 0;			//bres	flagsLogger,#5;				// permite loggeo de datos
noCancelTx:

	flagsTxControl[f_statBLE] = 0;		//bres		flagsTxControl,#f_statBLE
	DevLock = 0;						//clr			DevLock;			sí se pierde conexión BLE el candado vuelve a estar activo
	goto end_statDef;					//jra			end_statDef
statDef_clrWIFI:
	flagsTxControl[f_statWIFI] = 0;		//bres		flagsTxControl,#f_statWIFI
	statComWIFIFlag = 0;				//clr			statComWIFIFlag
end_statDef:


//;===========================================================
//;										TOKEN BLE
//;===========================================================
tokenBLE:
	//;Token solo se manda cuando está seleccionada la comunicación BLE
	if(!flagsTxControl[f_select])//btjf	flagsTxControl,#f_select,tokenBLE_01
		goto tokenBLE_01;
	goto finTokenBLE;//jra		finTokenBLE
tokenBLE_01:

//	btjt	flagsRxFirm,#0,tokenBLE_02; Empezó recepción de Firmware ?
//	btjt	flagsLogger,#5,tokenBLE_02; Empezó Tx logger datos
//	btjt	flagsLogger,#4,tokenBLE_02; Empezó Tx logger eventos
	if(flagsRxFirm[0] || flagsLogger[4] || flagsLogger[5])
		goto tokenBLE_02;

	//tnz		codeTX
	if(codeTX == 0)//jreq	tx_tokenBLE;			/se recibió algun comando valido? No, checa sí hay que mandar token
		goto tx_tokenBLE;
	//;Interpreta comando

	//ld		A,codeTX
	//cp		A,#$80;						/ se recibió respuesta WIFI ?
	if(codeTX == 0x80)		//jreq	finTokenBLE
		goto finTokenBLE;
	//cp		A,#$81;						/ se recibió respuesta BLE ?
	if(codeTX != 0x81)		//jrne	tokenBLE_03;			/ no, es otro comando, deja correr la ejecución
		goto tokenBLE_03;
	codeTX = 0;				// clr		codeTX;						/ sí, limpia código de Tx
tokenBLE_02:
	flagsTX2[2] = 1;		//bset	flagsTX2,#2;			/ sí se recibió repuesta, levanta bandera de token recibido

tokenBLE_03:
	//; la recepccion de cualquier comando tomala como una respuesta de  modulo de counicación
	timeTxTBLE = 20;			//mov		timeTxTBLE,#20;		/ vuelve a cargar tiempo para enviar Token (cada 30s)
	//ldw		X,#300;
	timeoutTBLE = 300;			//ldw		timeoutTBLE,X;					/ carga time out de Token BLE (5 min)
	goto finTokenBLE;			//jra		finTokenBLE

tx_tokenBLE:
	//tnz		keyTx;									/ se está atendiendo alguna transmisión?
	if(keyTx)//jrne	finTokenBLE;						/ espera a que terminé
		goto finTokenBLE;
	//tnz		timeTxTBLE;						/ ya se cumplió tiempo para enviar Token Wifi ?
	if(timeTxTBLE)//jrne	finTokenBLE;					/ no, continua
		goto finTokenBLE;

	timeTxTBLE = 20;//mov		timeTxTBLE,#20;				/ vuelve a cargar tiempo para enviar Token (cada 30s)

	//ldw		X,#$40FA
	//ldw		bufferTxControl,X
	bufferTxControl[0] = 0x40;
	bufferTxControl[1] = 0xFA;
	flagsTX[2] = 0;				//bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

	//ldw		X,#bufferTxControl
	//ldw		pointTx,X
	//ldw		pointInitTx,X
	//incw	X
	//ldw		pointEndTx,X
	//mov		blockSizeTX,#2
	pointTx = &bufferTxControl[0];
	pointInitTx = &bufferTxControl[0];
	pointEndTx = &bufferTxControl[2];
	blockSizeTX = 2;

	//clrw	X
	//ldw		chksum_HW,X
	//ldw		chksum_LW,X;					/ limpia registros de checksum
	chksum_32_HW_LW = 0;
	flagsTX[3] = 1;					// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
	keyTx = 0x55;					//mov		keyTx,#$55;						/ listo para mandar transmisión
	codeTX = 0;// clr		codeTX;								/ limpia código de Tx

	goto finTokenBLE;//jra		finTokenBLE

finTokenBLE:


/*
 Nota: codeTX se escribio en bluetooth.c
 tnz		codeTX
 jrne	rx_tokenWiFi_02b;			/ si llego algún comando no mandes token y consideralo como OK

*/
//;===========================================================
//;										TOKEN WiFi
//;===========================================================
tokenWiFi:

	//;Token solo se manda cuando está seleccionada la comunicación WIFI
	if(flagsTxControl[f_select])	//btjt	flagsTxControl,#f_select,tokenWiFi_01
		goto tokenWiFi_01;
	goto finTokenWiFi;				//jra		finTokenWiFi
tokenWiFi_01:

	//btjt	flagsWIFI,#f_timeLoggerCmd,tokenWiFi_02; si se está transmitiendo logger Wifi no mandes nombre de difusion
	//btjt	flagsWIFI,#f_eventLoggerCmd,tokenWiFi_02; si se está transmitiendo logger Wifi no mandes nombre de difusion
	if(flagsWIFI[f_timeLoggerCmd] || flagsWIFI[f_eventLoggerCmd])
		goto tokenWiFi_02;

	//ldw		X,cntSetName
	//tnzw	X
	if(cntSetName)//jrne	tokenWiFi_02
		goto tokenWiFi_02;

	//ldw			X,#600
	cntSetName = 600;	//ldw			cntSetName,X

	//;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
	//;copia los datos al buffer de tx
	//ldw		X,#difName
	//ldw		Y,#bufferTxControl
	wreg = 50;//mov		wreg,#50
	copyVector(&difName[0],&bufferTxControl[0]);//call	copyVector


//	ldw		X,#bufferTxControl
//	ldw		pointTx,X
//	ldw		pointInitTx,X
//	ldw		X,#(bufferTxControl+50)
//	ldw		pointEndTx,X
//	mov		blockSizeTX,#50
	pointTx = &bufferTxControl[0];
	pointInitTx = &bufferTxControl[0];
	pointEndTx = &bufferTxControl[50];
	blockSizeTX = 50;

	flagsTX[2] = 0;		//bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

	//clrw	X
	//ldw		chksum_HW,X
	chksum_32_HW_LW = 0;		//ldw		chksum_LW,X;					/ limpia registros de checksum
	flagsTX[3] = 1;				//bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
	keyTx = 0x55;				//mov		keyTx,#$55;						/ listo para mandar transmisión
	codeTX= 0;					//clr		codeTX;								/ limpia código de Tx

	goto finTokenWiFi;//jra		finTokenWiFi

tokenWiFi_02:

	//;comunicaciones que no admiten interrupción
		/*if(flagsRxFirm[0]){//btjt	flagsRxFirm,#0,rx_tokenWiFi_02 / Empezó recepción de Firmware ?
			goto rx_tokenWiFi_02;
		}
		if(flagsLogger[5]){//btjt	flagsLogger,#5,rx_tokenWiFi_02 /  Empezó Tx logger datos
			goto rx_tokenWiFi_02;
		}
		if(flagsLogger[4]){//btjt	flagsLogger,#4,rx_tokenWiFi_02 / Empezó Tx logger eventos
			goto rx_tokenWiFi_02;
		}
		if(flagsWIFI[f_timeLoggerCmd]){//btjt	flagsWIFI,#f_timeLoggerCmd,rx_tokenWiFi_02 / si se está transmitiendo logger Wifi no mandes token y consideralo como OK
			goto rx_tokenWiFi_02;
		}
		if(flagsWIFI[f_eventLoggerCmd]){//btjt	flagsWIFI,#f_eventLoggerCmd,rx_tokenWiFi_02 / si se está transmitiendo logger Wifi no mandes token y consideralo como OK
			goto rx_tokenWiFi_02;
		}*/
		// CGM 06/01/2024
		if(flagsRxFirm[0] | flagsLogger[5] | flagsLogger[4] | flagsWIFI[f_timeLoggerCmd] | flagsWIFI[f_eventLoggerCmd]){//btjt	flagsRxFirm,#0,rx_tokenWiFi_02 / Empezó recepción de Firmware ?
			goto rx_tokenWiFi_02;
		}

		if (codeTX != 0){
			goto	rx_tokenWiFi_02b;			//jrne	rx_tokenWiFi_02b;			/ si llego algún comando no mandes token y consideralo como OK
		}

tx_tokenWiFi:
		if(keyTx != 0){//tnz keyTx;	/ se está atendiendo alguna transmisión?
			goto finTokenWiFi; /// jrne	finTokenWiFi; / espera a que terminé
		}
		if(timeTxTWF != 0){//tnz timeTxTWF; / ya se cumplió tiempo para enviar Token Wifi ?
			goto rx_tokenWiFi; //jrne rx_tokenWiFi; / no, continua
		}

		timeTxTWF = 30; //mov timeTxTWF,#30;  / vuelve a cargar tiempo para enviar Token (cada 30s)

		bufferTxControl[0] = 0x40;		//ldw X,#$40F8
		bufferTxControl[1] = 0xF8;		//ldw bufferTxControl,X
//		bufferTxControl[2] = eeLat1;	//ldw X,eeLat1
//		bufferTxControl[3] = eeLat2;	//ldw bufferTxControl+2,X
//		bufferTxControl[4] = eeLat3;	//ldw X,eeLat3
//		bufferTxControl[5] = eeLat4;	//ldw bufferTxControl+4,X
//		bufferTxControl[6] = eeLong1;	//ldw X,eeLong1
//		bufferTxControl[7] = eeLong2;	//ldw bufferTxControl+6,X
//		bufferTxControl[8] = eeLong3;	//ldw X,eeLong3
//		bufferTxControl[9] = eeLong4;	//ldw bufferTxControl+8,X

		bufferTxControl[2] = reeLat1;	//ldw X,eeLat1
		bufferTxControl[3] = reeLat2;	//ldw bufferTxControl+2,X
		bufferTxControl[4] = reeLat3;	//ldw X,eeLat3
		bufferTxControl[5] = reeLat4;	//ldw bufferTxControl+4,X
		bufferTxControl[6] = reeLong1;	//ldw X,eeLong1
		bufferTxControl[7] = reeLong2;	//ldw bufferTxControl+6,X
		bufferTxControl[8] = reeLong3;	//ldw X,eeLong3
		bufferTxControl[9] = reeLong4;	//ldw bufferTxControl+8,X
		bufferTxControl[10] = versionFirm1;
		bufferTxControl[11] = versionFirm2;
		bufferTxControl[12] = fm_hardware;

		flagsTX[2] = 0; //bres flagsTX,#2;	/ Indica que no hay que transmitir Header

		point_X = &bufferTxControl[0];  // ldw	X,#bufferTxControl
		pointTx = point_X;				// ldw		pointTx,X
		pointInitTx = point_X;			// ldw		pointInitTx,X

		//pointEndTx = &bufferTxControl[10];		// ldw		X,#(bufferTxControl+10)
    										// ldw		pointEndTx,X
		pointEndTx = &bufferTxControl[13];
		//blockSizeTX = 10;						// mov		blockSizeTX,#10
		blockSizeTX = 13;
		//clrw	X
		//ldw	chksum_HW,X
		chksum_32_HW_LW = 0;		// ldw	chksum_LW,X;					/ limpia registros de checksum
		flagsTX [3] = 1;			//bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0X55;				//mov	keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;					//clr		codeTX;								/ limpia código de Tx

		goto finTokenWiFi;			//		jra		finTokenWiFi

rx_tokenWiFi:
		//ld A,codeTX
		if(codeTX == 0x80){ // cp	A,#$80;	/ se recibió respuesta del modulo WiFi ?
			goto rx_tokenWiFi_01;//jreq rx_tokenWiFi_01; / sí
		}
		//ldw	X,timeoutTWF;	/ No,checa time out
		if(timeoutTWF != 1){//cpw X,#1  // ya se ciumplió time out ?
			goto finTokenWiFi;//jrne  inTokenWiFi;   / No, continúa
		}
		flagsTX2[1] = 0;//bres flagsTX2,#1; / sí, cancela bandera de token recibido
		timeRstBLE = 5; //mov timeRstBLE,#5; / carga 5 segundos de apagado para BLE (reinicia el modulo wifi)
		//ldw	X,#300;
		timeoutTWF = 300;//ldw timeoutTWF,X;/ carga time out de Token (5 min)
		goto finTokenWiFi;//jra finTokenWiFi

rx_tokenWiFi_01:
		codeTX = 0;//clr codeTX / / limpia código de Tx

rx_tokenWiFi_02b:

		//	ld	A,codeTX
		if(codeTX == 0x80){//	cp	A,#$80;			/ se recibió respuesta del modulo WiFi ?
			goto rx_tokenWiFi_01;//	jreq rx_tokenWiFi_01;  / Sí
		}
		timeoutTWF = 300;

rx_tokenWiFi_02:

		timeTxTWF = 30;			// mov		timeTxTWF,#30;				/ vuelve a cargar tiempo para enviar Token (cada 30s) (carga de nuevo delay para evitar que el token se envíe al instante despues de ciertas transmisiones)

		flagsTX2[1]=1; //bset	flagsTX2,#1; // sí se recibió repuesta, levanta bandera de token recibido
		//ldw	X,#300;
		//timeoutTWF = 300;		//ldw	timeoutTWF,X;	/ carga time out de Token (5 min)
		goto finTokenWiFi;		//jra	finTokenWiFi;

finTokenWiFi:
		if(flagsTX2[1]){//	btjt flagsTX2,#1,tokenWiFi_OK; / si ya se recibió Token Wifi permite Rx/Tx de comandos
			goto tokenWiFi_OK;
		}
		goto end_tx_control;//	jp end_tx_control

tokenWiFi_OK:


//;===========================================================
		if(!flagsWIFI[f_timeLoggerCmd]){//	btjf flagsWIFI,#f_timeLoggerCmd,tx_control_00; si se está transmitiendo logger Wifi no revisa comando BLE
			goto tx_control_00;
		}
		goto jmp_tx_wifi;//  jp jmp_tx_wifi;
tx_control_00:
		if(!flagsWIFI[f_eventLoggerCmd]){//btjf flagsWIFI,#f_eventLoggerCmd,tx_control_00b /  si se está transmitiendo logger Wifi no revisa comando BLE
			goto tx_control_00b;
		}
		goto jmp_tx_wifi;//jp jmp_tx_wifi / no, continua
tx_control_00b:
//-----------------------------------------------------------

		//	ld	A,timeOutRst
		// código inecesario CGM 06/01/2024
		//if(timeOutRst != 1 ){			//	cp	A,#1; / se cumplió time out de reset ?
		//		goto noLoadResetBLE;		//	jrne noLoadResetBLE;	/ no, continúa
		//}

noLoadResetBLE:

		if(!flagsRxFirm[0]){//	btjf	flagsRxFirm,#0,ask_tx_control_01; Empezó recepción de Firmware ?
			goto ask_tx_control_01;
		}
		timeOutRst = 241;//	mov timeOutRst,#241; / carga time out de resetcon 60 segundos

		//ldw		X,#300;
		timeoutTWF = 300;//ldw		timeoutTWF,X;	/ carga time out de Token (5 min)
		timeoutTBLE = 300;
		if(timeOutRx != 0){//tnz timeOutRx
			goto noTimeOutRx;//jrne	noTimeOutRx;	/ se cumplió el time out de recepción ?
		}
		for(uint8_t i=0; i<8; i++){//clr flagsRxFirm;	/ borra banderas de Rx de firmware para cancelar proceso
			flagsRxFirm [i]= 0;
		}
		codeTX = 0;//clr	codeTX;		/ ignora comandos si es que se producieron
		goto ask_tx_control_01;//jp	ask_tx_control_01;	/ no entres a proceso de Rx firmware

noTimeOutRx:
		goto 	rx_firmware;			//jp		rx_firmware
		//; Los comandos son permisibles si hay una conexión activa ya sea BLE o WIFI
		//btjf	flagsTxControl,#f_select,tx_control_askStatBLE
tx_control_askStatWIFI:
		if(flagsTxControl[f_statWIFI])//btjt	flagsTxControl,#f_statWIFI,ask_tx_control_01
			goto ask_tx_control_01;
		goto jmp_tx_wifi;			//jp		jmp_tx_wifi
tx_control_askStatBLE:
		if(flagsTxControl[f_statBLE])// btjt	flagsTxControl,#f_statBLE,ask_tx_control_01
			goto ask_tx_control_01;
		goto jmp_tx_wifi;			//jp		jmp_tx_wifi



ask_tx_control_01:
		if(codeTX != 0x21){					//	cp	A,#$21
			goto ask_tx_control_02;			//	jrne ask_tx_control_02
		}
		timeOutRst = 241;					//	mov	 timeOutRst,#241;	/ carga time out de resetcon 60 segundos
		goto tx_control_handshake;//		jp	tx_control_handshake
ask_tx_control_02:
//;-----------------------------------------------------------
//; Unicamente permite comandos BLE cuando no hay conexión al servidor
		if(flagsWIFI[f_serverConnect]){//	btjt flagsWIFI,#f_serverConnect,ask_tx_control_04; / Sí hay conexión con servidor no permitas comando de petiución de logger
			goto ask_tx_control_04;
		}
		//ld	A,codeTX
		if(codeTX != 0x60){//	cp	A,#$60
			goto ask_tx_control_03;//	jrne ask_tx_control_03
		}
		timeOutRst = 241;//	mov	timeOutRst,#241; / carga time out de resetcon 60 segundos
		goto tx_control_dataLogger;//	jp	tx_control_dataLogger
ask_tx_control_03:
		if(codeTX != 0x61){//	cp		A,#$61
			goto ask_tx_control_04;//	jrne	ask_tx_control_04
		}
		timeOutRst = 241;//	mov		timeOutRst,#241;		/ carga time out de resetcon 60 segundos
		goto tx_control_eventLogger;//	jp		tx_control_eventLogger
ask_tx_control_04:
		if(codeTX != 0x53){//	cp	A,#$53
			goto ask_tx_control_05;//	jrne ask_tx_control_05
		}
		timeOutRst = 241;//	mov	timeOutRst,#241;	/ carga time out de resetcon 60 segundos
		goto tx_control_realTimeState;//	jp	tx_control_realTimeState
ask_tx_control_05:
		if(codeTX != 0x51){ //cp	A,#$51
			goto ask_tx_control_06;//jrne	ask_tx_control_06
		}
		timeOutRst = 241;//timeOutRst,#241;;  /carga time out de resetcon 60 segundos
		goto tx_control_parameters; //jp tx_control_parameters
ask_tx_control_06:
	    if(codeTX != 0x50){ //cp A,#$50
		goto	ask_tx_control_07;	// jrne	ask_tx_control_07
	    }
		timeOutRst = 241;				// carga time out de resetcon 60 segundos
		goto tx_control_writeParam;  //jp	tx_control_writeParam
ask_tx_control_07:
					//	ld		A,codeTX
			if(codeTX != 0x46){//	cp	A,#$46
					goto ask_tx_control_08;//	jrne	ask_tx_control_08
			}
			timeOutRst = 241;//	mov	timeOutRst,#241;				/ carga time out de resetcon 60 segundos
			goto tx_control_writeFirm;//jp tx_control_writeFirm
ask_tx_control_08:
					//	ld		A,codeTX
			if(codeTX != 0x54){//	cp	A,#$54
				goto ask_tx_control_09;//	jrne ask_tx_control_09
			}
			timeOutRst = 241;//	mov		timeOutRst,#241;				/ carga time out de resetcon 60 segundos
			goto tx_clean_logger;//jp tx_clean_logger
ask_tx_control_09:
					//	ld	A,codeTX
			if(codeTX != 0x55){//	cp	A,#$55
				goto ask_tx_control_10;//	jrne ask_tx_control_10
			}
			timeOutRst = 240;//	mov	timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_modParam;//	jp tx_modParam
ask_tx_control_10:
					//ld A,codeTX
			if(codeTX != 0x56){//cp A,#$56
				goto ask_tx_control_11;//jrne	ask_tx_control_11
			}
			timeOutRst = 240;//mov	timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_write_status;//jp	tx_write_status
ask_tx_control_11:
					//	ld	A,codeTX
			if(codeTX != 0x57){//	cp	A,#$57
				goto ask_tx_control_12;//	jrne ask_tx_control_12
			}
			timeOutRst = 240;//	mov	timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_read_status;//jp	tx_read_status
ask_tx_control_12:
			//	ld	A,codeTX
			if(codeTX != 0x58){//	cp	A,#$58
				goto ask_tx_control_13;//	jrne ask_tx_control_13
			}
			timeOutRst = 240;//	mov	timeOutRst,#240;	// carga time out de resetcon 240 segundos
			goto tx_timeUNIX;//	jp	tx_timeUNIX
ask_tx_control_13:
			//	ld		A,codeTX
			if(codeTX != 0x59){//	cp		A,#$59
				goto ask_tx_control_14;//	jrne	ask_tx_control_14
			}
			timeOutRst = 240;//	mov		timeOutRst,#240; // carga time out de resetcon 240 segundos
			goto tx_write_GEO;//jp tx_write_GEO
ask_tx_control_14:
			//ld	A,codeTX
			if(codeTX != 0x5A){//cp	A,#$5A
				goto ask_tx_control_15;//jrne	ask_tx_control_15
			}
			timeOutRst = 240;//mov		timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_read_GEO;//jp tx_read_GEO
ask_tx_control_15:
			//	ld	A,codeTX
			if(codeTX != 0x5B){//	cp	A,#$5B
				goto ask_tx_control_16;//	jrne ask_tx_control_16
			}
			timeOutRst = 240;//	mov	timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_read_timeUNIX;//	jp	tx_read_timeUNIX
ask_tx_control_16:
			//ld A,codeTX
			if(codeTX != 0x5C){//cp A,#$5C
				goto ask_tx_control_17;//jrne	ask_tx_control_17
			}
			timeOutRst = 240;//mov timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_timeBCD;//jp tx_timeBCD
ask_tx_control_17:
			//ld A,codeTX
			if(codeTX != 0x5F){//cp		A,#$5F
				goto ask_tx_control_18;//jrne	ask_tx_control_17
			}
			timeOutRst = 240;//mov timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_infoReloj;//jp		tx_infoReloj
ask_tx_control_18:
			//ld		A,codeTX
			//cp		A,#$62
			if(codeTX!= 0x62)//jrne	ask_tx_control_19
				goto ask_tx_control_19;
			timeOutRst = 240; //mov		timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_wifi_datalogger;//jp		tx_wifi_datalogger
ask_tx_control_19:
			//ld		A,codeTX
			//cp		A,#$63
			if(codeTX!= 0x63)//jrne	ask_tx_control_20
				goto ask_tx_control_20;
			timeOutRst = 240;		//mov		timeOutRst,#240;				// carga time out de resetcon 240 segundos
			goto tx_wifi_eventlogger;//jp		tx_wifi_eventlogger
ask_tx_control_20:


//;---------------------------------------------------------------------------------------------------------
//;								Sección agregada para Transmisión de datos a WiFi
//;---------------------------------------------------------------------------------------------------------
//							; Después de revisar comandos Bluetooth para a revisar Tx de WiFi
jmp_tx_wifi:
			goto	tx_wifi;			//jp		tx_wifi

//;---------------------------------------------------------------------------------------------------------
			// Linea no se ejecuta
			//goto	end_tx_control;			//jp		end_tx_control

//;								Handshake
//;----------------------------------------------------------
// Rutina completa Adaptada <<_RGM
tx_control_handshake:

		//28-oct-2024 RGM:		uint8_t *point_X;
		//28-oct-2024 RGM:		uint8_t *point_Y;

		// Carga datos de bloque de handshake
		Bloque_handshake[comando1] = 0xF1; 				// comando1 = 0xF1;
		Bloque_handshake[comando2] = 0x3F;   		 	//comando2 = 0x3F;
		// MAC adress ya debe estar cargada
		// Bloque_handshake[2]
	    //---------
		// Bloque_handshake[13]
		Bloque_handshake[model] = fm_modelo;   			// model = fm_modelo;
		Bloque_handshake[versionSoft1] = versionFirm1;	// versionSoft1 = versionFirm1;
		Bloque_handshake[versionSoft2] = versionFirm2;	// versionSoft2 = versionFirm2;
		Bloque_handshake[plantParam1] = Plantilla[plantilla1];		// plantParam1 = plantilla1;
		Bloque_handshake[plantParam2] = Plantilla[plantilla2];		// plantParam2 = plantilla2;
		Bloque_handshake[checkList1] = fm_modelo0;		// checkList1 = fm_modelo0;
		Bloque_handshake[checkList2] = fm_hardware;		// checkList2 = fm_hardware;


	//	;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
	//	;copia los datos al buffer de tx
		point_X = &Bloque_handshake[0];	 // Apunta a los datos
		point_Y = &bufferTxControl[0];	 // Apunta al buffer de datos en RAM
		for(uint8_t i = 0; i < size_handshake ; i++ )
	     	point_Y[i] = point_X[i];


		chksum_32_HW_LW = 0;					// limpia registros de checksum

		point_X = &bufferTxControl[0];
		buildChksumBloq (point_X, size_handshake);

		asm ("nop");

		bufferTxControl [size_handshake+0] = (uint8_t)((chksum_32_HW_LW & 0xFF000000)>> 24);
		bufferTxControl [size_handshake+1] = (uint8_t)((chksum_32_HW_LW & 0x00FF0000)>> 16);
		bufferTxControl [size_handshake+2] = (uint8_t)((chksum_32_HW_LW & 0x0000FF00)>>  8);
		bufferTxControl [size_handshake+3] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);

	    pointTx = &bufferTxControl[0];
		pointInitTx = &bufferTxControl[0];
		pointEndTx = &bufferTxControl [size_handshake + 4];
	    blockSizeTX = size_handshake + 4;   //4 bytes Checksum

		flagsTX [4] = 1;					// Indica que ya hubo un handshake
		flagsTX [2] = 0;						// Indica que no hay que transmitir Header

		chksum_32_HW_LW = 0;					// limpia registros de checksum

		flagsTX [3] = 1;					// indica que no se tiene que mandar checksum
		keyTx = 0x55;						// listo para mandar transmisión
		codeTX = 0x00;						// limpia código de Tx

		goto	end_tx_control;			//jp		end_tx_control

//-----------------------------------------------------------------------------------------------------------------------------------------
// Rutina completa Adaptada <<_RGM
tx_control_dataLogger:
		//;---- pasa los datos para la función
		cntBlockFlash = cntBlockDATA;//mov cntBlockFlash,cntBlockDATA
		cntByteBlock = cntByteBlockDATA; //mov	cntByteBlock,cntByteBlockDATA
		offsetInitTX = 3; //mov	offsetInitTX,#3; // Indica el offset a tomar en cuenta en caso de que el Loogger se haya llenado alguna vez
		//ldw X,#data_buffer
		//ldw dirBuffer,X
		dirBuffer = &data_buffer[0];
		//ldw X,#dataLogger
		//ldw dirLogger,X
		dirLogger = &dataLogger[0];

		// CGM 16/04/2025;  se agrega la direccion del buffer de la Pagina del logger de Datos.
		dirBufferPage = &bufferPageDATA[0];

		if(!flagsLogger[1]){//btjf	flagsLogger,#1,noFullFlagTXDATA;	/ se llenó al menos una vez el DATA logger ?
			goto noFullFlagTXDATA;
		}
		flagsTX [0] = 1;//bset	flagsTX,#0;						/ toma en cuenta que la memoria ya se llenó al menos una vez
noFullFlagTXDATA:

		//Prepara datos para Tx (guarda los últimos datos del lloger y cálcula punteros de Tx)
		blockSizeTX = 8;//	mov	blockSizeTX,#8;				/ Indica que el tamaño de los bloques de transmisión
		flagsLogger[5] = 1;//	bset flagsLogger,#5;				/ cancela loggeo de datos hasta que terminé la Tx
		//	ldw	X,#dataLogger
		loggerStart = &dataLogger[0]; //	ldw	loggerStart,X
		point_X = &dataLoggerFin; //	ldw	X,#dataLoggerFin
		point_X++;		//incw X ******************************************
		loggerEnd = point_X;	//	ldw	loggerEnd,X


		// 96 * 128Bytes = 12,288 = 12 K
		numBlock = 96; //mov	numBlock,#96

		//ldw	X,cntRegDATA
		cntReg = cntRegDATA;	//ldw	cntReg,X
		//ldw	X,#eeCntRegDATA
		cntRegPNT = &eeCntRegDATA;//ldw	cntRegPNT,X
		reeCntRegDATA = cntReg;



		//Prepara datos para Tx (cálcula punteros de Tx)
		prepTXlogg_2();	//call	prepTXlogg_2;

		/*
		 * CGM 16/04/2025 (EL SEÑOR DE LA FE)
		 * cntBlockDATA++;			Se incrimenta para Realizar la escritura en el siguiente bloque de 128 dentro de la FLASH.
		 * cntByteBlockDATA = 0;	Se iguala a 0, para comenzar a iniciar la escritura al inicio de buffer
		 * data_buffer[i] = 0xFF;	Se hace un borrado del buffer, y se realiza con 0xFF por hacer un simil con la memoria FLASH, donde es un borrado con 0xFF
		 */
		//cntBlockDATA++;
		//cntByteBlockDATA = 0;
		//for(uint8_t i=0; i<128; i++)
		//	data_buffer[i] = 0xFF;

		//Carga datos de Header
		Bloque_Header [softVersion1] = Plantilla[version1];		// mov		softVersion1,version1
		Bloque_Header [softVersion2] = Plantilla[version2];//mov softVersion2,version2;	/ Carga versión del firmware

		//ldw	X,#0
		//ldw	bufferSize_HW,X
		Bloque_Header [bufferSize_4] = highByte(0);//ldw	bufferSize_HW,X
		Bloque_Header [bufferSize_3] = lowByte(0);

		//ldw	X,#1344
		//ldw	bufferSize_LW,X;	/ carga el número de registros en caso de memoria llena
		Bloque_Header [bufferSize_2] = highByte(1344);	// bufferSize_LW
		Bloque_Header [bufferSize_1] = lowByte(1344);

		Bloque_Header [dataType] = 1; //mov	dataType,#01
		Bloque_Header [dataSize] = 9; //mov	dataSize,#9
		flagsTX[2] = 1; //bset flagsTX,#2;	// Indica que hay que transmitir Header

		//clrw	X
		//ldw chksum_HW,X
		//ldw chksum_LW,X;		/ limpia registros de checksum
		chksum_32_HW_LW = 0;
		flagsTX[3] = 0;			//bres	flagsTX,#3;			/ indica que no se ha enciado el checksum
		keyTx = 0x55;			//mov keyTx,#$55;		/ listo para mandar transmisión
		codeTX = 0;				//clr codeTX;		/ limpia código de Tx
		goto end_tx_control;//jp end_tx_control
//-----------------------------------------------------------------------------------------------------------------------------------------
tx_control_eventLogger:

		//;---- pasa los datos para la función
		cntBlockFlash = cntBlockEVENT;//mov cntBlockFlash,cntBlockEVENT; /pasa el número de bloques de datos grabados
		cntByteBlock = cntByteBlockEVENT;//mov cntByteBlock,cntByteBlockEVENT
		offsetInitTX = 10;//mov offsetInitTX,#10;			/ Indica el offset a tomar en cuenta en caso de que el Loogger se haya llenado alguna vez
		//ldw X,#event_buffer
		//ldw dirBuffer,X
		dirBuffer = &event_buffer[0];
		//ldw X,#eventLogger
		//ldw dirLogger,X
		dirLogger = &eventLogger[0];

		// CGM 16/04/2025;  se agrega la direccion del buffer de la Pagina del logger de EVENTOS.
		dirBufferPage = &bufferPageEVENT[0];

		if(!flagsLogger[2]){//btjf	flagsLogger,#2,noFullFlagTXEVENT;	/ se llenó al menos una vez el EVENT logger ?
			goto noFullFlagTXEVENT;
		}
		flagsTX[0] = 1;//bset	flagsTX,#0;		/ toma en cuenta que la memoria ya se llenó al menos una vez
noFullFlagTXEVENT:

//;							; Prepara datos para Tx (guarda los últimos datos del lloger y cálcula punteros de Tx)
//;							call	prepTXlogg;

		blockSizeTX = 8;//mov	blockSizeTX,#8;				/ Indica que el tamaño de los bloques de transmisión
		flagsLogger[4] = 1;//bset	flagsLogger,#4;				/ cancela loggeo de eventos hasta que terminé la Tx
		//ldw	X,#eventLogger
		//ldw	loggerStart,X
		loggerStart = &eventLogger[0];
		point_X = &eventLoggerFin;		//ldw	X,#eventLoggerFin
		point_X++;						//incw	X ******************
		loggerEnd = point_X;			//ldw	loggerEnd,X


		// 160 * 128Bytes = 20,480 = 20 K
		numBlock = 160;//mov numBlock,#160;	/ número máximo de bloque a guardar del logger de eventos (96 bloques de  128 bytes = 12kb )

		//ldw	X,cntRegEVENT
		cntReg = cntRegEVENT;			//ldw	cntReg,X
		//ldw	X,#eeCntRegEVENT
		cntRegPNT = &eeCntRegEVENT;		//ldw	cntRegPNT,X
		reeCntRegEVENT = cntReg;

		// Prepara datos para Tx (cálcula punteros de Tx)
		prepTXlogg_2();		//call	prepTXlogg_2;

		/*
		 * CGM 16/04/2025 (EL SEÑOR DE LA FE)
		 * cntBlockEVENT++;			Se incrimenta para Realizar la escritura en el siguiente bloque de 128 dentro de la FLASH.
		 * cntByteBlockDATA = 0;	Se iguala a 0, para comenzar a iniciar la escritura al inicio de buffer
		 * data_buffer[i] = 0xFF;	Se hace un borrado del buffer, y se realiza con 0xFF por hacer un simil con la memoria FLASH, donde es un borrado con 0xFF
		*/
		//cntBlockEVENT++;
		//cntByteBlockEVENT = 0;
		//for(uint8_t i=0; i<128; i++)
		//	event_buffer[i] = 0xFF;

		//Carga datos de Header
		Bloque_Header [softVersion1] = Plantilla[version1];		// mov		softVersion1,version1
		Bloque_Header [softVersion2] = Plantilla[version2];	////mov	softVersion2,version2;	/ Carga versión del firmware

		//ldw		X,#0
		//ldw		bufferSize_HW,X
		Bloque_Header [bufferSize_4] = highByte(0);	//*************** ????
		Bloque_Header [bufferSize_3] = lowByte(0);

		//ldw	X,#1440
 	 	 Bloque_Header [bufferSize_2] = highByte(1440); //ldw	bufferSize_LW,X; / carga el número de registros en caso de memoria llena
 	 	 Bloque_Header [bufferSize_1] = lowByte(1440);

 	 	 Bloque_Header [dataType] = 2;//mov dataType,#02
 	 	 Bloque_Header [dataSize] = 14; //mov	dataSize,#14
 	 	 flagsTX[2] = 1;			//bset	flagsTX,#2; / Indica que hay que transmitir Header

 	 	 //clrw	X
 	 	 //ldw		chksum_HW,X
 	 	 //ldw		chksum_LW,X;					/ limpia registros de checksum
 	 	 chksum_32_HW_LW = 0;
 	 	 flagsTX[3] = 0;//bres	flagsTX,#3;						/ indica que no se ha enciado el checksum
 	 	 keyTx = 0x55;//mov		keyTx,#$55;						/ listo para mandar transmisión
 	 	 codeTX = 0;//clr		codeTX;								/ limpia código de Tx

 	 	 goto end_tx_control;//jp		end_tx_control
//-----------------------------------------------------------------------------------------------------------------------------------------
tx_control_realTimeState:


		//28-oct-2024 RGM:		uint8_t *point_X;
		//28-oct-2024 RGM:		uint8_t *point_Y;

		/*  Valores de prueba para comando  "0x4053"
		tdevl= 0;	tdevf = 158;
		teval= 0;	tevaf = 228;
		voltl = 170;
		 */

		// Valores 19-jul-2024	temporales para mostrar en la App
		// tdevl = highByte(tsac_w);							// copia el dato de temperatura ambiente
		// tdevf = lowByte(tsac_w);							// copia el dato de temperatura ambiente

		// teval = highByte(tret_w);// highByte(252);								// copia el dato de temperatura evaporador
		// tevaf = lowByte(tret_w);// lowByte(252);								// copia el dato de temperatura evaporador

		// tret_w = 255;										// copia el dato de temperatura de salida

		// 19-Ago-2024		voltl = 130;
		// Valores temporales para mostrar en la App


		Bloque_TiempoReal [tempAmb_RT_H] = tdevl;								// copia el dato de temperatura ambiente
		Bloque_TiempoReal [tempAmb_RT_L] = tdevf;								// copia el dato de temperatura ambiente
		Bloque_TiempoReal [tempEva_RT_H] = teval;								// copia el dato de temperatura evaporador
		Bloque_TiempoReal [tempEva_RT_L] = tevaf;								// copia el dato de temperatura evaporador
		Bloque_TiempoReal [tempSal_RT_H] = highByte(tret_w);	// copia el dato de temperatura de salida
		Bloque_TiempoReal [tempSal_RT_L] = lowByte(tret_w);			// copia el dato de temperatura de salida

		Bloque_TiempoReal [volt_RT]= voltl;				//volt_RT,voltl;	/ toma el volatje de la función de medición rms

		Bloque_TiempoReal [actuadores_RT] = 0;
		if (GPIOR0 [f_comp])											// compresor activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x1;// BitSet(Bloque_TiempoReal[actuadores_RT], 0);				// carga estado de compresor
		if (GPIOR0 [f_dh])												// deshielo activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x2;//BitSet(Bloque_TiempoReal[actuadores_RT], 1);				// carga estado de deshielo
		if (flagsC[0])													// puerta abierta ?
			Bloque_TiempoReal[actuadores_RT] |= 0x4;////BitSet(Bloque_TiempoReal[actuadores_RT], 2);				// sí, indica puerta abierta
		if (flagsC[1])													// Modo ahorro 1 activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x8;////BitSet(Bloque_TiempoReal[actuadores_RT], 3);				// Modo ahorro 1 activo ?
		if (flagsC[2])													// Modo ahorro 1 activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x10;//BitSet(Bloque_TiempoReal[actuadores_RT], 4);				//	sí, indicalo
		if (flagsa[nocturno])//if (!GetRegFlagState(flagsa, nocturno))											//  Modo nocturno activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x20;//BitSet(Bloque_TiempoReal[actuadores_RT], 5);				//	sí, indicalo
		if (GPIOR1 [f_fan])												// ventilador activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x40;//BitSet(Bloque_TiempoReal[actuadores_RT], 6);				//	sí, indicalo
		if (GPIOR0 [f_lamp])											//  rele auxiliar activo ?
			Bloque_TiempoReal[actuadores_RT] |= 0x80;//BitSet(Bloque_TiempoReal[actuadores_RT], 7);				//  sí, indicalo

		// Bloque_TiempoReal[alarmas2_RT] = trefst2;
		Bloque_TiempoReal[alarmas2_RT] =0;
		for(int k=0;k<8;k++){
			Bloque_TiempoReal[alarmas2_RT] |=(uint8_t) (trefst2[k]<<k);
		}

		Bloque_TiempoReal[alarmas2_RT] &= 0xFE;//BitClear(Bloque_TiempoReal[alarmas2_RT],0);
		//Bloque_TiempoReal [alarmas_RT] = trefst;
		for(uint8_t k=0; k<8; k++){
			Bloque_TiempoReal [alarmas_RT] |= (uint8_t) trefst[k]<<k;
		}
								// copia el resgistro de alarmas trefst

		blockSizeTX =size_TiempoReal;				// tamaño del bloque

		//; Carga datos de Header
		Bloque_Header [softVersion1] = Plantilla[version1];		// mov		softVersion1,version1
		Bloque_Header [softVersion2] = Plantilla[version2];		// mov		softVersion2,version2;	/ Carga versión del firmware
		Bloque_Header [bufferSize_4] = 0;				// bufferSize_HW
		Bloque_Header [bufferSize_3] = 0;				// bufferSize_HW
		Bloque_Header [bufferSize_2] = 0;				// bufferSize_LW
		Bloque_Header [bufferSize_1] = 1;				// bufferSize_LW
		Bloque_Header [dataType] = 3;				//mov		dataType,#03
		Bloque_Header [dataSize] = blockSizeTX;	//mov		dataSize,blockSizeTX
		flagsTX [2] = 1;					// Indica que hay que transmitir Header


		//; carga header al buffer a transmitir
		//;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
		//;copia los datos al buffer de tx
		point_X = &Bloque_Header[0];	 // ldw		X,#softVersion1
		point_Y = &bufferTxControl[0];	 //
		for(uint8_t i = 0; i < 8 ; i++ )
			point_Y[i] = point_X[i];

		//; carga estado en tiempo real al buffer a transmitir
		//;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
		//;copia los datos al buffer de tx
		point_X = &Bloque_TiempoReal[0];	 // ldw		X,#tempAmb_RT
		point_Y = &bufferTxControl[8];	 //
		for(uint8_t i = 0; i < 10 ; i++ )
			point_Y[i] = point_X[i];

		chksum_32_HW_LW = 0;					// limpia registros de checksum

		point_X = &bufferTxControl[0];			// carga dirección del buffer a calcular chksum
		buildChksumBloq (point_X, 18);			// tamaño del bloque a calcular el chksum

		bufferTxControl [18] = (uint8_t)((chksum_32_HW_LW & 0xFF000000)>> 24);
		bufferTxControl [19] = (uint8_t)((chksum_32_HW_LW & 0x00FF0000)>> 16);
		bufferTxControl [20] = (uint8_t)((chksum_32_HW_LW & 0x0000FF00)>>  8);
		bufferTxControl [21] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);

	    pointTx = &bufferTxControl[0];
		pointInitTx = &bufferTxControl[0];
		pointEndTx = &bufferTxControl [22];
	    blockSizeTX = 22;   //4 bytes ChecksumblockSizeTX = 25;   //4 bytes Checksum

		flagsTX [2] = 0;					// Indica que no hay que transmitir Header
		flagsTX [3] = 1;					// evita enviar chksum

		keyTx = 0x55;						// listo para mandar transmisión
		codeTX = 0x00;

		goto end_tx_control;		//jp		end_tx_control

//-----------------------------------------------------------------------------------------------------------------------------------------
tx_control_parameters:

		//28-oct-2024 RGM:		uint8_t *point_X;
		//28-oct-2024 RGM:		uint8_t *point_Y;

		//; Carga datos de Header
		Bloque_Header [softVersion1] = version1;		// mov		softVersion1,version1
		Bloque_Header [softVersion2] = version2;		// mov		softVersion2,version2;	/ Carga versión del firmware
		Bloque_Header [bufferSize_4] = 0;				// bufferSize_HW
		Bloque_Header [bufferSize_3] = 0;				// bufferSize_HW
		Bloque_Header [bufferSize_2] = 0;				// bufferSize_LW
		Bloque_Header [bufferSize_1] = 1;				// bufferSize_LW
		Bloque_Header [dataType] = 04;					//mov		dataType,#04
		Bloque_Header [dataSize] = 128;					//mov		dataSize,#128

	//	; carga header al buffer a transmitir
	//	;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
	//	;copia los datos al buffer de tx
		point_X = &Bloque_Header[softVersion1];	 // ldw		X,#softVersion1
		point_Y = &bufferTxControl[0];	 //
		for(uint8_t i = 0; i < 8 ; i++ )
			point_Y[i] = point_X[i];

	//	; carga parametross eeprom al buffer a transmitir
	//	;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
	//	;copia los datos al buffer de tx
		//point_X = &eePlantilla[eedato_seg1];	 // ldw		X,#softVersion1
		point_Y = &bufferTxControl[8];	 //
		for(uint8_t i = 0; i < 128 ; i++ )
			//point_Y[i] = point_X[i];
			point_Y[i] = reePlantilla[i];

		chksum_32_HW_LW = 0;					// limpia registros de checksum

		//;Añade chksum al buffer a transmiir
		point_X = &bufferTxControl[0];			// carga dirección del buffer a calcular chksum
		buildChksumBloq (point_X, 136);			// tamaño del bloque a calcular el chksum

		bufferTxControl [136] = (uint8_t)((chksum_32_HW_LW & 0xFF000000)>> 24);
		bufferTxControl [137] = (uint8_t)((chksum_32_HW_LW & 0x00FF0000)>> 16);
		bufferTxControl [138] = (uint8_t)((chksum_32_HW_LW & 0x0000FF00)>>  8);
		bufferTxControl [139] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);

	    pointTx = &bufferTxControl[0];
		pointInitTx = &bufferTxControl[0];
		pointEndTx = &bufferTxControl [140];
	    blockSizeTX = 140;   //4 bytes ChecksumblockSizeTX = 25;   //4 bytes Checksum

		flagsTX [2] = 0;						// Indica que no hay que transmitir Header
		flagsTX [3] = 1;						// evita enviar chksum

		keyTx = 0x55;						// listo para mandar transmisión
		codeTX = 0x00;

		goto end_tx_control;		//jp		end_tx_control


//-----------------------------------------------------------------------------------------------------------------------------------------
tx_control_writeParam:

		//28-oct-2024 RGM:		uint8_t *point_X;
		//28-oct-2024 RGM:		uint8_t *point_Y;

verifica_version1:
			//if (RxBuffer_Ble[125] == eePlantilla[eeversion1]){
			if (RxBuffer_Ble[125] == reePlantilla[eeversion1]){
				goto 	verifica_version2;		//jreq	verifica_version2
			}
			goto	no_writeParam;				//jp		no_writeParam
verifica_version2:
			//if(RxBuffer_Ble[126] == eePlantilla[eeversion2]){
			if (RxBuffer_Ble[126] == reePlantilla[eeversion2]){
				goto 	verifica_version3;		//jreq	verifica_version3
			}
			goto	no_writeParam;				//jp		no_writeParam
verifica_version3:

			if(RxBuffer_Ble[2] != 0xAA ){		// agrega offset para primer dato de seguridad
				goto	no_writeParam;			//jrne	no_writeParam
			}
			if(RxBuffer_Ble[67] != 0x66){		// agrega offset para segundo dato de seguridad
				goto	no_writeParam;			//jrne	no_writeParam
			}
			if(RxBuffer_Ble[129] != 0xCC){		// agrega offset para tercer dato de seguridad
				goto	no_writeParam;			//jrne	no_writeParam
			}

			chksum_32_HW_LW = 0;					// limpia registros de checksum

			point_X = &RxBuffer_Ble[0];			// carga dirección del buffer a calcular chksum
			buildChksumBloq (point_X, 130);			// tamaño del bloque a calcular el chksum

			uint32_t chksum_to_compare = 0;
			chksum_to_compare = ((uint32_t)RxBuffer_Ble[130])  <<24;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[131]) <<16;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[132]) <<8;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[133]);
			//((uint32_t)RxBuffer_Ble[130]) <<32 + ((uint32_t)RxBuffer_Ble[131]) <<16 + ((uint32_t)RxBuffer_Ble[132]) <<8  + 	(uint32_t)RxBuffer_Ble[133];
			if (chksum_32_HW_LW != chksum_to_compare){
				goto	no_writeParam;		//jrne	no_writeParam;				/ si no iguales sal si grabar parámetros
			}

write_param:
			ProgMemCode = 0x55;							//	mov		ProgMemCode,#$55;			/ Indica que se va a grabar bloque de EEPROM
			point_X = &RxBuffer_Ble[0];					// apunta al buffer de datos RECIBIDOS
			point_X++;
			point_X++;
			dataPointer =	point_X;	// LDW		dataPointer,X
			// LDW		X,#eedato_seg1;				/	apunta al inicio de la EEPROM
			dirPointer =  &eePlantilla [eedato_seg1];	// LDW		dirPointer,X
			GRABA_BLOCK();			//	GRABA_BLOQUE(point_X , point_Y);  , Cambia para ser Compatible con ASM

			// Carga datos de bloque de handshake para transmitir la respuesta
			Bloque_handshake[comando1] =	0xF1;	//mov		comando1,#$F1
			Bloque_handshake[comando2] =	0x3D;	//mov		comando2,#$3D;				/ indica que la grabación fue exitosa
			flagsTX[5]=1;						// bset 	flagsTX,#5;						/ inidca que hay que reiniciar el control.

			goto	ok_writeParam;				//jp		ok_writeParam

no_writeParam:
			// Carga datos de bloque de handshake para transmitir la respuesta
			Bloque_handshake[0] =  0xF1;	//mov		comando1,#$F1
			Bloque_handshake[1] =  0x3E;	//mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos

ok_writeParam:
			pointTx = &Bloque_handshake[comando1];
			pointInitTx = &Bloque_handshake[comando1];
			pointEndTx = &Bloque_handshake [comando2];
			blockSizeTX = 2;   //4 bytes ChecksumblockSizeTX = 25;   //4 bytes Checksum

			flagsTX [3] = 1;						// bset	flagsTX,#3;						// evita que se mande checksum
			keyTx = 0x55;						// listo para mandar transmisión
			codeTX = 0x00;

			goto end_tx_control;		//jp		end_tx_control

//-----------------------------------------------------------------------------------------------------------------------------------------
tx_control_writeFirm:
			bandera_act_fw_j = 1;   //Parche para parar loggeo

            Bloque_handshake [comando1] = 0xF1;//	mov	comando1,#$F1
            Bloque_handshake [comando2] = 0X03;//	mov	comando2,#$03;	/ indica que está listo para la recepción del firmware
            //Bloque_handshake [comando2] = 0X04;//	mov	comando2,#$03;	/ indica que está listo para la recepción del firmware
            flagsRxFirm[0] = 1;//	bset flagsRxFirm,#0;	/ indica que se comienza con recepción de fimware

			//	ldw	X,#comando1
            pointTx = &Bloque_handshake [comando1];//ldw	pointTx,X
            pointInitTx = &Bloque_handshake [comando1];//	ldw	pointInitTx,X
			//	ldw	X,#comando2
            pointEndTx =  &Bloque_handshake [comando2];//	ldw	pointEndTx,X
            blockSizeTX = 2;//	mov	blockSizeTX,#2

            flagsTX [3] = 1; //	bset flagsTX,#3;	/ evita que se mande checksum
            keyTx = 0x55;//	mov	 keyTx,#$55;	/ listo para mandar transmisión
            codeTX = 0x00;//	clr	codeTX;

            timeOutRx = 5;//	mov	timeOutRx,#5;	/ carga time out para recibir respuestas

			goto end_tx_control;//	jp	end_tx_control

			//<<<<<<<<<<<< Aqui me quede  <<<<<<<<<<<<<<<
			//<<<<<<<<<<<< Aqui me quede  <<<<<<<<<<<<<<<
			//<<<<<<<<<<<< Aqui me quede  <<<<<<<<<<<<<<<
//;----------------------------------------------------------
//;										Rx Firmware(Número de Bloques)
//;----------------------------------------------------------
rx_firmware:
			bandera_act_fw_j = 1;
			flagsLogger[4]=1;		// bset	flagsLogger,#4;				/ cancela loggeo de eventos
			flagsLogger[5]=1;		// bset	flagsLogger,#5;				/ cancela loggeo de datos
			if(!flagsRxFirm[3]){ // btjf	flagsRxFirm,#3,rx_firmware01; Ya se completo la recepción de bloques de firmware ?
				goto rx_firmware01;
			}
			goto rxChkFirm;				// jp		rxChkFirm;

rx_firmware01:
			if(flagsRxFirm[1]){		// btjt	flagsRxFirm,#1,rxBloqFirm; / Ya se sabe cuantos bloques se van a recibir ? Sí, ve a recibir bloques
				goto rxBloqFirm;
			}

rx_numBloqFirm:
			/*ld		A,codeTX
				cp		A,#$49
				;jrne	end_rx_firmware
				jreq	rx_numBloqFirm_01*/
			if(codeTX==0x49){
				goto rx_numBloqFirm_01;
			}
			goto end_rx_firmware;	// jp		end_rx_firmware
rx_numBloqFirm_01:

			/*ldw		X,#RxBuffer_Ble;					/ carga dirección del buffer de recepción
			incw	X
			incw	X;										/ omite las primeras dos localidades del array
			ld		A,(X);								/ carga número de bloques a recibir
			ld		numBloqFirm,A
			*/
			numBloqFirm = (RxBuffer_Ble[2] << 8) | RxBuffer_Ble[3] ;
			cntBloqFirm = 0;		//clr		cntBloqFirm;					/ reinicia el contador de bloques recibidos
			BloqDatalooger[comando1] = 0xF1;		//mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x07;		// mov		comando2,#$07;				/ indica que está listo para la recepción de los bloques del firmware
			flagsRxFirm[1]=1;					// bset	flagsRxFirm,#1;				/ indica que se recibieron el número de paquetes
			//flagsRxFirm[4]=1;					// ;bset	flagsLogger,#4;				/ cancela loggeo de eventos
			//flagsRxFirm[5]=1;					// ;bset	flagsLogger,#5;				/ cancela loggeo de datos
			// clrw	X
			//ldw		chksumFirm_HW,X
			chksumFirm_HW_LW=0;						// ldw		chksumFirm_LW,X;			/ limpia registros de checksum general del Firmware
			/*ldw		X,#comando1
			ldw		pointTx,X*/
			pointTx = &BloqDatalooger[comando1];
			pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
			/* ldw		X,#comando2
			ldw		pointEndTx,X */
			pointEndTx = &BloqDatalooger[comando2];
			blockSizeTX = 2;						// mov		blockSizeTX,#2
			flagsTX[3]=1;						// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55;							// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;								// clr		codeTX;
			timeOutRx = 5;
			goto end_tx_control; 					//jp		end_tx_control
//;----------------------------------------------------------
rxBloqFirm:
			if (flagsRxFirm[2]){// btjt	flagsRxFirm,#2,rxBloqFirm_01
				goto rxBloqFirm_01;
			}
			goto end_tx_control;		//jp		end_tx_control

rxBloqFirm_01:

			//GPIOR1[f_fan] = 0;
			//GPIOR0[f_comp] = 0;  //Parche, desactivacion de relevadores


			flagsRxFirm[2]=0; // bres	flagsRxFirm,#2;				/ borra bandera de paquete recibido
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW=0;		// ldw		chksum_LW,X;					/ limpia registros de checksum
			// ldw		X,#RxBuffer_Ble;					/ carga dirección del buffer de recepción
			wreg = 0;// clr		wreg
			cksum_aux = 0;

load_bloqFirm:
			//uint16_t v16Aux;
			// ld A,(X);	/ carga el dato del buffer
			build_chksum(RxBuffer_Ble[wreg]); // call	build_chksum;	/ ve calculando el checksum
			// incw	X / Aumenta la siguiente direeción de RxBuffer_Ble[1]
			wreg++;			// inc		wreg
			/*
			 * 	ld		A,wreg;								/ Ya se cargaron los 128 bytes ?
			 * 	cp		A,#128
			 * 	jrult	load_bloqFirm;				/ No, continua con la carga
			 */
			if(wreg<128){
				goto load_bloqFirm;
			}

			cksum_aux = (RxBuffer_Ble[128] << 24) | (RxBuffer_Ble[129] << 16) |(RxBuffer_Ble[130] << 8) | RxBuffer_Ble[131];
//			// ld		a,(X)
//			resulh = RxBuffer_Ble[1]; // ld		resulh,a
//			// incw	X / Aumenta la siguiente direeción de RxBuffer_Ble[2]
//			// ld		a,(X)
//			resull = RxBuffer_Ble[2]; // ld		resull,a;							/ carga los 16 bits más significativos del checksum transmitido
//			// incw	X
//			// ld		a,(X)
//			tempo2 = RxBuffer_Ble[3];// ld		tempo2,a
//			//incw	X
//			//ld		a,(X)
//			tempo1 = RxBuffer_Ble[4];// ld		tempo1,a;							/ carga los 16 bits más significativos del checksum transmitido
//			// ldw		X,tempo2;
//			//cpw		X,chksum_LW;					/ compara la parte baja del checksum

//			if(((uint16_t)((tempo2<<8)|tempo1))!=((uint16_t)(chksum_32_HW_LW & 0x0000FFFF))){// jrne	no_writeBloqFirm;			/ si no iguales sal si grabar bloque de firmware
//				goto no_writeBloqFirm;
//			}
//			// ldw		X,resulh;
//			// cpw		X,chksum_HW;					/ compara la parte baja del checksum
//			if(((uint16_t)((resulh<<8)|resull)) != ((uint16_t)((chksum_32_HW_LW & 0xFFFF0000)>>16))){ //jrne	no_writeBloqFirm;			/ si no iguales sal si grabar bloque de firmware
//				goto no_writeBloqFirm;
			if(cksum_aux != chksum_32_HW_LW){
				goto no_writeBloqFirm;
			}
//			}// ; *** Ve calculando el checksum general del Firmware

			// ldw		Y,chksumFirm_HW
			// ldw		X,chksumFirm_LW;			/ Carga el Checksum general del firmware
			// addw	X,chksum_LW;					/ suma la parte baja del checksum del bloque
//			if(((chksumFirm_HW_LW & 0x0000FFFF)+(chksum_32_HW_LW & 0x0000FFFF))>0xFFFF){	// JRNC	no_inc_chksumFirm_HW;	/ Hubo carry ?
//				goto no_inc_chksumFirm_HW;
//			}
//				// INCW	Y;										/ Sí, Incrementa parte alta de checksum
//no_inc_chksumFirm_HW:
			// addw	Y,chksum_HW
			//  ldw		chksumFirm_HW,Y
			chksumFirm_HW_LW += chksum_32_HW_LW; // 							ldw		chksumFirm_LW,X;			/ fuarda el checksum general calculado hasta ahora

			if(direccion_fw >= 0x803F000)
				NVIC_SystemReset();

			if(contador_bloques_fw == 16)
			{
				borra_pagina_logger(pagina_borrado);
				contador_bloques_fw = 0;

			}
			contador_bloques_fw++;
			graba_bloque_fw();

//			ProgMemCode = 0xAA; // mov		ProgMemCode,#$AA;			/ Indica que se va a grabar bloque de FLASH
//			// LDW		X,#RxBuffer_Ble;					/ apunta al buffer de datos RECIBIDOS
//			dataPointer = &RxBuffer_Ble[0];// LDW		dataPointer,X
//			// ld    A,cntBloqFirm;				/ Toma el número de bloques recibidos al momento
//			// ldw		X,#128;								/ Carga el tamaño de los bloques (128 bytes)
//			// mul		X,A;									/ Multiplicalos
//			// addw	X,#eventLogger;				/	apunta al inicio de la Flash resevada para Logger + el número de bloques grabados
//			dirPointer = (128 * cntBloqFirm + eventLogger);// LDW		dirPointer,X
//			GRABA_BLOCK();// call	GRABA_BLOCK;					/ graba bloque de 128bytes en FLASH
			cntBloqFirm++;// inc		cntBloqFirm;					/ incrmenta el contador de datos recibidos
			// ld		A,cntBloqFirm
			// cp		A,numBloqFirm;				/ ya se recibieron todos los paquetes ?
			if(cntBloqFirm != numBloqFirm){//jrne	rxFirm_noComplete
				goto rxFirm_noComplete ;
			}
			flagsRxFirm[3]=1;// bset 	flagsRxFirm,#3;				/ indica que ya se recibieron todos los paquetes
			direccion_fw = 0x8020000;
			pagina_borrado = 64;
			contador_bloques_fw = 16;


rxFirm_noComplete:
// Carga datos de bloque de handshake para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1;	// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;	// mov		comando2,#$3D;				/ indica que la grabación fue exitosa
			timeOutRx=5;				// mov		timeOutRx,#5;					/ carga time out para recibir respuestas
			goto ok_writeBloqFirm; 							// jp		ok_writeBloqFirm

no_writeBloqFirm:
							//;// Carga datos de bloque de handshake para transmitir la respuesta
			BloqDatalooger[comando1]=0xF1;	//mov		comando1,#$F1
			BloqDatalooger[comando2]=0x3E;	// mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos
			for(int i=0; i<8; i++)				// clr		flagsRxFirm;					/ borra banderas de Rx de firmware para cancelar proceso
				flagsRxFirm[i]=0;
			codeTX = 0;						// clr		codeTX;								/ ignora comandos si es que se producieron

			bandera_act_fw_j = 0;
			direccion_fw = 0x8020000;
			pagina_borrado = 64;
			contador_bloques_fw = 16;

ok_writeBloqFirm:
			// ldw		X,#comando1
			pointTx = &BloqDatalooger[comando1]; 	// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2];	// ldw		pointEndTx,X
			blockSizeTX = 2;				// mov		blockSizeTX,#2

			flagsTX[3]=1;	// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55;	// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;		// clr		codeTX;

			goto end_tx_control;


//			;										Rx Firmware(checksuum TOTAL del firmware)
//			;----------------------------------------------------------
rxChkFirm:
			cksum_gral_aux = 0;
			if(flagsRxFirm[4]){//	btjt	flagsRxFirm,#4,end_rx_firmware; se recibió fimware con éxito, no realices nada más
				goto end_rx_firmware;
			}
			// ld		A,codeTX
			// cp		A,#$4A
			if(codeTX != 0x4A){//				jrne	end_rx_firmware
					goto end_rx_firmware;
			}
			cksum_gral_aux = (RxBuffer_Ble[2] << 24) | (RxBuffer_Ble[3] << 16) | (RxBuffer_Ble[4] << 8) | RxBuffer_Ble[5];
//			//ldw		X,#RxBuffer_Ble;					/ carga dirección del buffer de recepción
//			// incw	X
//			// incw	X;										/ omite las primeras dos localidades del array
//			// ld a,(X)
//			resulh = RxBuffer_Ble[2];	// ld		resulh,a
//			// incw	X
//			// ld		a,(X)
//			resull = RxBuffer_Ble[3];	// ld		resull,a;							/ carga los 16 bits más significativos del checksum transmitido
//			// incw	X
//			// ld		a,(X)
//			tempo2 = RxBuffer_Ble[4];// ld		tempo2,a
//			// incw	X
//			// ld		a,(X)
//			tempo1 = RxBuffer_Ble[5]; // ld		tempo1,a;							/ carga los 16 bits más significativos del checksum transmitido
//			// ldw		X,tempo2;
//			// cpw		X,chksumFirm_LW;			/ compara la parte baja del checksum
//			if(((uint16_t)((tempo2<<8)|tempo1)) != ((uint16_t)(chksumFirm_HW_LW & 0x0000FFFF))){		// jrne	rxFirmErr;						/ si no iguales sal si grabar bloque de firmware
//				goto rxFirmErr;
//			}
//			// ldw		X,resulh;
//			// cpw		X,chksumFirm_HW;			/ compara la parte baja del checksum
//			if(((uint16_t)((resulh<<8)|resull)) != ((uint16_t)((chksumFirm_HW_LW & 0xFFFF0000)>>16))){//		jrne	rxFirmErr;						/ si no iguales sal si grabar bloque de firmware
//				goto rxFirmErr;
//			}
			if(cksum_gral_aux != chksumFirm_HW_LW){
				goto rxFirmErr;
			}
			// ;// Carga datos de bloque de handshake para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1; //	mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;//	mov		comando2,#$3D;
			flagsRxFirm[4] = 1;				// bset	flagsRxFirm,#4;				/ inidca que que el Fimware recibido es correcto
			//;mov		keyWrFirm,#$AA;				/ indica que se tiene que grabar Firmware

			timeOutRx=5;	//	mov		timeOutRx,#5;					/ carga time out para recibir respuestas

			goto rxFirmOK;//	jp		rxFirmOK

rxFirmErr:
			BloqDatalooger[comando1] = 0xF1; //	mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3E; // mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos
			for (int i = 0; i<8; i++)// clr		flagsRxFirm;					/ borra banderas de Rx de firmware para cancelar proceso
				flagsRxFirm[i]=0;
			codeTX = 0;// clr		codeTX;								/ ignora comandos si es que se producieron
			bandera_act_fw_j = 0;
			direccion_fw = 0x8020000;
			pagina_borrado = 64;
			contador_bloques_fw = 16;
rxFirmOK:
			// ldw		X,#comando1
			pointTx =  &BloqDatalooger[comando1]; 	// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2];	// ldw		pointEndTx,X
			blockSizeTX = 2; 						//mov		blockSizeTX,#2

			flagsTX[3] = 1;		// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55 ;					// 	mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;	// clr		codeTX;


			goto end_tx_control;// jp		end_tx_control

end_rx_firmware:
			goto end_tx_control; 			//jp		end_tx_control//----------------------------------------------------------

//			;										Clean logger
//			;----------------------------------------------------------
tx_clean_logger:

			clean_buffer();	//	call	clean_buffer
			cntBloqFirm = 0;//	clr		cntBloqFirm


tx_clean_logger_loop:
			ProgMemCode = 0xAA; // mov		ProgMemCode,#$AA;			/ Indica que se va a grabar bloque de FLASH
			// ldw		X,#data_buffer
			dataPointer = &data_buffer[0];		// LDW		dataPointer,X

			// ld    A,cntBloqFirm;				/ Toma el número de bloques recibidos al momento
			// ldw		X,#128;								/ Carga el tamaño de los bloques (128 bytes)
			// mul		X,A;									/ Multiplicalos
			// addw	X,#eventLogger;						/	apunta al inicio de la Memoria de logger + el número de bloques grabados
			dirPointer  = (cntBloqFirm*128+&eventLogger[0]);		// LDW		dirPointer,X

			 // call	GRABA_BLOCK;					/ graba bloque de 128bytes en FLASH
			//while( HAL_FLASH_Unlock() !=  HAL_OK );
			//FLASH_PageErase(dirPointer);
			//HAL_FLASH_Lock();

			/*
			 * CGM 10/04/2025
			 * Borrado Total de todo el Logger, es decir de la pagina 110 a la pagina 125 serán borradas.
			 * Logger de Eventos:			110 - 119 pagina
			 * Logger de Datos o Tiempo:	120 - 125 pagina
			 */
			for(uint32_t i = 110; i<126; i++){
				erasePage(i);
			}

			//cntBloqFirm++; // inc		cntBloqFirm;					/ incrmenta el contador de datos recibidos

			// ld		A,cntBloqFirm
			// cp		A,#0
			// CGM 10/04/2025
			// Se comenta debido a que ya se realiza rl borrado total, en las anteriores lineas.
			//if(cntBloqFirm!= 0){ // jrne	tx_clean_logger_loop
			//	goto tx_clean_logger_loop;
			//}

			BloqDatalooger[comando1] = 0xF1; // mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D; // mov		comando2,#$3D;				/ indica que la grabación fue exitosa
			// ; omite el reinicio del control y en su lugar inicializa los apuntadores de loggeo y carga los buffer de loggeo
			cntByteBlockDATA = 0; 	// clr		cntByteBlockDATA
			cntBlockDATA = 0; 		// clr		cntBlockDATA
			cntByteBlockEVENT = 0; // clr		cntByteBlockEVENT
			cntBlockEVENT = 0;		// clr		cntBlockEVENT
			// ldw		X,#data_buffer
			dirBuffer = &data_buffer[0];	// ldw		dirBuffer,X
			// ldw		X,#dataLogger
			dirLogger = &dataLogger[0];		// ldw		dirLogger,X
			cntBlockFlash = cntBlockDATA; 	// mov		cntBlockFlash,cntBlockDATA
			load_next_buffer();				// call	load_next_buffer	; carga buffer de RAM con el bloque de datos

			// ldw		X,#event_buffer
			dirBuffer = &event_buffer[0];  // ldw		dirBuffer,X
			// ldw		X,#eventLogger
			dirLogger = & eventLogger[0];// ldw		dirLogger,X
			cntBlockFlash = cntBlockEVENT;// mov		cntBlockFlash,cntBlockEVENT
			load_next_buffer(); // call	load_next_buffer	; carga buffer de RAM con el bloque de datos
			for(int i = 0; i<8; i++)
				flagsEvent[i] = 0;// clr		flagsEvent;		Cancela cualquier inicio de eventos

			goto ok_clean_logger; //jp		ok_clean_logger

no_clean_logger:
// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] =	0xF1; // mov		comando1,#$F1
			BloqDatalooger[comando2] = 	0x3E; // mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos

ok_clean_logger:
			// ldw		X,#comando1
			pointTx = &BloqDatalooger[comando1];	// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1];// ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2];	// ldw		pointEndTx,X
			blockSizeTX = 2;						// mov		blockSizeTX,#2

			flagsTX[3] = 1;		// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55; 		// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX=0;			//clr		codeTX;

			goto end_tx_control;// jp		end_tx_control

//----------------------------------------------------------
tx_modParam:

fin_tx_modParam:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_write_status:
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum

			//ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			wreg = 0;	// clr		wreg
			waux = 6;	// mov 	waux,#6;							/ tamaño del bloque a calcular el chksum
			buildChksumBloq(&RxBuffer_Ble[0],6);		// call	buildChksumBloq
			chksum_to_compare = 0;
			chksum_to_compare = ((uint32_t)RxBuffer_Ble[6])  <<24;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[7]) <<16;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[8]) <<8;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[9]);

			if(chksum_to_compare!= chksum_32_HW_LW){// jrne	tx_timeBCD_error;			/ si no iguales sal sin modificar Geolocalización
				goto tx_write_status_error;
			}

			// toma los datos de estado
			// ldw		Y,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			// addw	Y,#2;									/ agrega offset para cargar bytes del tiempo UNIX

			// ; Analiza el Registro de cambios 1
			// ld		A,(Y);								/
			wreg = RxBuffer_Ble[2];  // ld		wreg,A;
			//waux = eeEstado1; // mov		waux,eeEstado1;				/ carga estados actuales
			waux = reeEstado1;
chk_est1_b0:
			if(!GetRegFlagState(wreg, est1Refri)) {// btjf	wreg,#est1Refri,chk_est1_b1; / hay cambio de estado refrigerador on/off ?
				goto chk_est1_b1;
			}
			BitComplement(waux,est1Refri);// bcpl	waux,#est1Refri; 			/ cambia el estado del refri
			// indica que hay que mostrar un mensaje
			cntMsgCmd = 250;	// mov		cntMsgCmd,#250
			numMsg = 1; 		// mov		numMsg,#1

chk_est1_b1:
			if(!GetRegFlagState(wreg, est1Noct)){// btjf	wreg,#est1Noct,chk_est1_b2; / hay cambio de modo nocturno ?
				goto chk_est1_b2;
			}
chk_est1_b2:
			if(!GetRegFlagState(wreg, est1Lamp)){// btjf	wreg,#est1Lamp,chk_est1_b3; / hay cambio de estado de lampara ?
				goto chk_est1_b3;
			}
			flagsC[f_lampDoor] ^= 1;	 // bcpl	flagsC,#f_lampDoor;				/ cambia el estado de la bandera de control de lámpara
			waux &= 0xFB;// BitClear(waux,est1Lamp); // bres	waux,#est1Lamp
			if(!flagsC[f_lampDoor]){// btjf	flagsC,#f_lampDoor,chk_est1_b2_01;	actualiza estado de lampara para guardarlo en eeprom
				goto chk_est1_b2_01;
			}
			BitSet(waux,est1Lamp);// bset	waux,#est1Lamp

chk_est1_b2_01:
			cntMsgCmd = 250; 	// mov		cntMsgCmd,#250
			numMsg = 2; 		// mov		numMsg,#2

chk_est1_b3:
			if(!GetRegFlagState(wreg, est1Ahorro2)){ 	// btjf	wreg,#est1Ahorro2,chk_est1_b4; / hay cambio de estado modo ahorro 2 ?
				goto chk_est1_b4;
			}
			if(flagsC[f_ahorro2]){	// btjt	flagsC,#f_ahorro2,chk_est1_b3_01
				goto chk_est1_b3_01;
			}
			t_ahorro1_H = 0; 	// clr		t_ahorro1_H
			// clr		t_ahorro1_L

			t_ahorro2_H= 0;		// clr		t_ahorro2_H
			// clr		t_ahorro2_L

			goto chk_est1_b4;	// jra		chk_est1_b4

chk_est1_b3_01:
			cntdpysp = 0xF0; // mov			cntdpysp,#$F0;		/ Despliega Set Point y el diferencial
			flagsC[f_ahorro1] = 0;	// bres	flagsC,#f_ahorro1
			flagsC[f_ahorro2] = 0;		// bres	flagsC,#f_ahorro2
			load_tiempoAhorro1();			// call	load_tiempoAhorro1;			/ cada que se abre puerta vuelve a cargar tiempos de ahorro
			load_tiempoAhorro2();			// call	load_tiempoAhorro2;

chk_est1_b4:
			if(!GetRegFlagState(wreg, est1Reset)){	//btjf	wreg,#est1Reset,chk_est1_b5; / hay solicitud de reset ?
				goto chk_est1_b5;
			}
			estado1[est1Reset]=1;	//bset	estado1,#est1Reset

chk_est1_b5:
			if(!GetRegFlagState(wreg, est1Snooze)){		//btjf	wreg,#est1Snooze,chk_est1_b6; / hay cambio de estado buzzer ?
				goto chk_est1_b6;
			}
			ld_alarmDelay(); // call		ld_alarmDelay;				/ carga tiempo de Snooze LC

chk_est1_b6:
			if(GetRegFlagState(Plantilla[logicos2],funReleDesh)){
				goto deshTypeAct_05;
			}
			if(!GetRegFlagState(wreg, est1LockDr)){		// btjf	wreg,#est1LockDr,chk_est1_b7; / hay cambio de estado en cerradura ?
				goto chk_est1_b7;
			}
			BitComplement(waux,est1LockDr);	// bcpl	waux,#est1LockDr;	/ cambia estado de rele cerradura

			// indica que hay que mostrar un mensaje
			cntMsgCmd = 250;	// mov		cntMsgCmd,#250
			numMsg = 3;	// mov		numMsg,#3

chk_est1_b7:
deshTypeAct_05:

			// carga waux en eeprom
			// ldw		X,#eeEstado1;
			wreeprom(waux,&eeEstado1);	// call	wreeprom;							/ ejecuta el grabado de estado 1
			reeEstado1 = waux;
			//HAL_IWDG_Refresh( &hiwdg ); 			// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1; 	// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;	// mov		comando2,#$3D;				/ indica que la grabación fue exitosa

			goto tx_write_status_ok;// jra		tx_write_status_ok

tx_write_status_error:
			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1; 	// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;	// mov		comando2,#$3D;				/ indica que la grabación fue exitosa


tx_write_status_ok:
			// ldw		X,#comando1
			pointTx = &BloqDatalooger[comando1]; 	// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2];	// ldw		pointEndTx,X
			blockSizeTX = 2;						// mov		blockSizeTX,#2

			flagsTX[3]= 1;						// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55; 							// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0; 							// clr		codeTX;


fin_tx_write_status:
			goto end_tx_control;					// jp		end_tx_control
// ----------------------------------------------------------//----------------------------------------------------------

tx_read_status:
		// carga información a enviar.
		// mov		estado1,eeEstado1
		uint8_t estado1_Aux = reeEstado1;
		for(int k=0;k<8;k++)
			estado1[k] = ((estado1_Aux>>k) & 0x1);


estado1_b1:
		estado1[est1Noct] = 0;		// bres	estado1,#est1Noct
		if (!flagsa[nocturno]){ 			// btjf	flagsa,#nocturno,estado1_b2; / estado nocturno activado ?
			goto estado1_b2;
		}
		estado1[est1Noct] = 1; // bset	estado1,#est1Noct;			/ sí, indicalo

estado1_b2:
		estado1[est1Lamp] = 0;// bres	estado1,#est1Lamp
		if (!GPIOR0[f_lamp]){			// btjf	GPIOR0,#f_lamp,estado1_b3; / lampara encendida ?
			goto estado1_b3;
		}
		estado1[est1Lamp] = 1;// bset	estado1,#est1Lamp;			/ sí, indicalo

estado1_b3:
		estado1[est1Ahorro2] = 0;// bres	estado1,#est1Ahorro2
		if(!flagsC[f_ahorro2]){		// btjf	flagsC,#f_ahorro2,estado1_b4; / modo ahorro 2 activo  ?
			goto estado1_b4;
		}
		estado1[est1Ahorro2] = 1;// bset	estado1,#est1Ahorro2;			/ sí, indicalo

estado1_b4:

estado1_b5:
		estado1[est1Snooze] = 0;// bres	estado1,#est1Snooze
		if(!flagsBuzzer[0]){	// btjf	flagsBuzzer,#0,estado1_b6;
			goto estado1_b6;
		}
		estado1[est1Snooze] = 1;// bset	estado1,#est1Snooze

estado1_b6:
		estado1[est1LockDr] = 0;// bres	estado1,#est1LockDr
		if(!GPIOR0[f_dh]){ // btjf	GPIOR0,#f_dh,estado1_b7;
			goto estado1_b7;
		}
		estado1[est1LockDr] = 1; // bset	estado1,#est1LockDr

estado1_b7:

		blockSizeTX = 4;// mov		blockSizeTX,#4;				/ tamaño del bloque

		// Carga datos de Header
		Bloque_Header[softVersion1] = Plantilla[version1]; // mov		softVersion1,version1
		Bloque_Header[softVersion2] = Plantilla[version2]; // mov		softVersion2,version2;	/ Carga versión del firmware
		// ldw		X,#0
		Bloque_Header[bufferSize_4] = highByte(0); // ldw		bufferSize_HW,X
		Bloque_Header[bufferSize_3] = lowByte(0);
		// ldw		X,#1
		// ldw		bufferSize_LW,X;			/ carga el número de registros
		Bloque_Header[bufferSize_2] = highByte(1);
		Bloque_Header[bufferSize_1] = lowByte(1);

		Bloque_Header[dataType] = 5; 		// mov		dataType,#05
		Bloque_Header[dataSize] = blockSizeTX; // mov		dataSize,blockSizeTX
		flagsTX[2] = 1; // bset	flagsTX,#2;						/ Indica que hay que transmitir Header

/*
 * 	carga header al buffer a transmitir
 *	Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
 *	copia los datos al buffer de tx
*/

		// ldw		X,#softVersion1
		// ldw		Y,#bufferTxControl

		wreg = 8; // mov		wreg,#8
		copyVector(&Bloque_Header[softVersion1],&bufferTxControl[0]);	// call	copyVector

//	Carga información de estados en el buffer a tranmitir
		// ldw		X,estado1
		// ldw		bufferTxControl+8,X
		// ldw		X,estado3
		// ldw		bufferTxControl+10,X
		bufferTxControl[8] = 0;
		bufferTxControl[9] = 0;
		bufferTxControl[10] = 0;
		bufferTxControl[11] = 0;
		for(int k=0;k<8;k++){
			bufferTxControl[8] |= (uint8_t)(estado1[k]<<k);
			bufferTxControl[9] |= (uint8_t)(estado2[k]<<k);;
			bufferTxControl[10] |= (uint8_t)(estado3[k]<<k);;
			bufferTxControl[11] |= (uint8_t)(estado4[k]<<k);;
		}


// Añade chksum al buffer a transmiir
		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum

		// ldw		X,#bufferTxControl;			/ carga dirección del buffer a calcular chksum
		wreg = 0;	// clr		wreg
		waux = 12;	// mov 	waux,#12;							/ tamaño del bloque a calcular el chksum
		buildChksumBloq(&bufferTxControl[0],waux); // call	buildChksumBloq

		// ldw		X,chksum_HW
		// ldw		bufferTxControl+12,X
		// ldw		X,chksum_LW
		// ldw		bufferTxControl+14,X
		bufferTxControl[12] = (uint8_t) ((chksum_32_HW_LW & 0xFF000000)>>24);
		bufferTxControl[13] = (uint8_t) ((chksum_32_HW_LW & 0x00FF0000)>>16);
		bufferTxControl[14] = (uint8_t) ((chksum_32_HW_LW & 0x0000FF00)>>8);
		bufferTxControl[15] = (uint8_t) ((chksum_32_HW_LW & 0x000000FF));

		//ldw		X,#bufferTxControl
		pointTx = &bufferTxControl[0];		//ldw		pointTx,X

		pointInitTx = &bufferTxControl[0];	// ldw		pointInitTx,X
		//ldw		X,#(bufferTxControl+16)
		pointEndTx = &bufferTxControl[16];	//ldw		pointEndTx,X
		blockSizeTX=16;						// mov		blockSizeTX,#16

		flagsTX[2] = 0;				// bres	flagsTX,#2;						/ Indica que no hay que transmitir Header
		flagsTX[3] = 1;				// bset	flagsTX,#3;						/ evita enviar chksum

		keyTx = 0x55;				// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;					// clr		codeTX;

fin_tx_read_status:
		goto end_tx_control;		// jp		end_tx_control

//----------------------------------------------------------
tx_timeUNIX:
		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0; // ldw		chksum_LW,X;					/ limpia registros de checksum

		// ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
		wreg = 0;// clr		wreg
		waux = 6;// mov 	waux,#6;							/ tamaño del bloque a calcular el chksum

		buildChksumBloq(&RxBuffer_Ble[0],6);		// call	buildChksumBloq
		chksum_to_compare = 0;
		chksum_to_compare = ((uint32_t)RxBuffer_Ble[6])  <<24;
		chksum_to_compare += ((uint32_t)RxBuffer_Ble[7]) <<16;
		chksum_to_compare += ((uint32_t)RxBuffer_Ble[8]) <<8;
		chksum_to_compare += ((uint32_t)RxBuffer_Ble[9]);

		if(chksum_to_compare!= chksum_32_HW_LW){// jrne	tx_timeBCD_error;			/ si no iguales sal sin modificar Geolocalización
			goto tx_write_status_error;
		}


// Carga datos de bloque para transmitir la respuesta
		BloqDatalooger[comando1] = 0xF1; 	// mov		comando1,#$F1
		BloqDatalooger[comando2] = 0x3D;	// mov		comando2,#$3D;				/ indica que la grabación fue exitosa

		goto tx_timeUNIX_ok;								//jra		tx_timeUNIX_ok

tx_timeUNIX_error:
// Carga datos de bloque para transmitir la respuesta
		BloqDatalooger[comando1] = 0xF1;		// mov		comando1,#$F1
		BloqDatalooger[comando2] = 0x3E;		// mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos

tx_timeUNIX_ok:
		//ldw		X,#comando1
		pointTx = &BloqDatalooger[comando1];		//ldw		pointTx,X
		pointInitTx = &BloqDatalooger[comando1];	//ldw		pointInitTx,X
		//ldw		X,#comando2
		pointEndTx = &BloqDatalooger[comando2];		//ldw		pointEndTx,X
		blockSizeTX = 2;							//mov		blockSizeTX,#2

		flagsTX[3] = 1;				// bset	flagsTX,#3;						/ evita que se mande checksum
		keyTx = 0x55;				// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;					// clr		codeTX;


fin_tx_timeUNIX:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_write_GEO:
// checa el chksum
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW = 0;		// ldw		chksum_LW,X;					/ limpia registros de checksum

			// ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			wreg = 0;					// clr		wreg
			waux = 10;					// mov 	waux,#10;							/ tamaño del bloque a calcular el chksum

			// call	buildChksumBloq

			// ldw		X,tempo2;
			// cpw		X,chksum_LW;					/ compara la parte baja del checksum
			// ldw		X,resulh;
			// cpw		X,chksum_HW;					/ compara la parte baja del checksum

			buildChksumBloq(&RxBuffer_Ble[0],10);		// call	buildChksumBloq
			chksum_to_compare = 0;
			chksum_to_compare = ((uint32_t)RxBuffer_Ble[10])  <<24;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[11]) <<16;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[12]) <<8;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[13]);

			if(chksum_to_compare!= chksum_32_HW_LW){// jrne	tx_timeBCD_error;			/ si no iguales sal sin modificar Geolocalización
				goto tx_write_GEO_error;
			}
			// toma los datos de Geolocalización
			// ldw		Y,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			// addw	Y,#2;							/ agrega offset para cargar bytes del tiempo UNIX

			// ld		A,(Y);								/
			// waux = RxBuffer_Ble[2];		// ld		waux,A;
			// ldw		X,#eeLat1;
			wreeprom(RxBuffer_Ble[2], &eeLat1);					//call	wreeprom;							/ ejecuta el grabado Latitud 1
			reeLat1 = RxBuffer_Ble[2];
			//HAL_IWDG_Refresh( &hiwdg );  // MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

			// incw	Y
			// ld		A,(Y);								/
			// ld		waux,A;
			// ldw		X,#eeLat2;
			wreeprom(RxBuffer_Ble[3], &eeLat2);				// call	wreeprom;							/ ejecuta el grabado Latitud 2
			reeLat2 = RxBuffer_Ble[3];
			//HAL_IWDG_Refresh( &hiwdg );						// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

			wreeprom(RxBuffer_Ble[4], &eeLat3);				// call	wreeprom;							/ ejecuta el grabado Latitud 3
			reeLat3 = RxBuffer_Ble[4];
			//HAL_IWDG_Refresh( &hiwdg );						// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

			wreeprom(RxBuffer_Ble[5], &eeLat4);				// call	wreeprom;							/ ejecuta el grabado Latitud 4
			reeLat4 = RxBuffer_Ble[5];
			//HAL_IWDG_Refresh( &hiwdg );						// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

			wreeprom(RxBuffer_Ble[6], &eeLong1);
			reeLong1 = RxBuffer_Ble[6];
			//HAL_IWDG_Refresh( &hiwdg );

			wreeprom(RxBuffer_Ble[7], &eeLong2);
			reeLong2 = RxBuffer_Ble[7];
			//HAL_IWDG_Refresh( &hiwdg );

			wreeprom(RxBuffer_Ble[8], &eeLong3);
			reeLong3 = RxBuffer_Ble[8];
			//HAL_IWDG_Refresh( &hiwdg );

			wreeprom(RxBuffer_Ble[9], &eeLong4);
			reeLong4 = RxBuffer_Ble[9];
			//HAL_IWDG_Refresh( &hiwdg );


			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1;	// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;	// mov		comando2,#$3D;				/ indica que la grabación fue exitosa

			goto tx_write_GEO_ok;				// jra		tx_write_GEO_ok

tx_write_GEO_error:
			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1;	// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3E;	// mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos

tx_write_GEO_ok:
			// ldw		X,#comando1
			pointTx = &BloqDatalooger[comando1];		// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1];	//ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2];		// ldw		pointEndTx,X
			blockSizeTX = 2;							// mov		blockSizeTX,#2

			flagsTX[3] = 1;								// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55; 								// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;									// clr		codeTX;

fin_tx_write_GEO:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_read_GEO:
			blockSizeTX = 8; // mov		blockSizeTX,#8;				/ tamaño del bloque

			// Carga datos de Header
			// mov		softVersion1,version1
			Bloque_Header[softVersion1] = Plantilla[version2]; // mov		softVersion2,version2;	/ Carga versión del firmware

			//ldw		X,#0
			//ldw		bufferSize_HW,X
			Bloque_Header [bufferSize_4] = highByte(0);	//*************** ????
			Bloque_Header [bufferSize_3] = lowByte(0);

			//ldw	X,#1
			Bloque_Header [bufferSize_2] = highByte(1); //ldw	bufferSize_LW,X; / carga el número de registros en caso de memoria llena
			Bloque_Header [bufferSize_1] = lowByte(1);

			Bloque_Header[dataType] = 6;// mov		dataType,#06
			Bloque_Header[dataSize] = blockSizeTX;				// mov		dataSize,blockSizeTX

			// ; carga header al buffer a transmitir
			// ;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
			// ;copia los datos al buffer de tx

			// ldw		X,#softVersion1
			// ldw		Y,#bufferTxControl
			wreg = 8;// mov		wreg,#8
			copyVector(&Bloque_Header[softVersion1],&bufferTxControl);// call	copyVector

			// Carga información de geolocalización en el buffer a tranmitir
			/*ldw		X,eeLat1
			ldw		bufferTxControl+8,X
			ldw		X,eeLat3
			ldw		bufferTxControl+10,X
			ldw		X,eeLong1
			ldw		bufferTxControl+12,X
			ldw		X,eeLong3
			ldw		bufferTxControl+14,X
			*/

//			bufferTxControl[8] = eeLat1;
//			bufferTxControl[9] = eeLat2;
//			bufferTxControl[10] = eeLat3;
//			bufferTxControl[11] = eeLat4;
//			bufferTxControl[12]	= eeLong1;
//			bufferTxControl[13]	= eeLong2;
//			bufferTxControl[14] = eeLong3;
//			bufferTxControl[15] = eeLong4;

			bufferTxControl[8] = reeLat1;
			bufferTxControl[9] = reeLat2;
			bufferTxControl[10] = reeLat3;
			bufferTxControl[11] = reeLat4;
			bufferTxControl[12]	= reeLong1;
			bufferTxControl[13]	= reeLong2;
			bufferTxControl[14] = reeLong3;
			bufferTxControl[15] = reeLong4;

			// Añade chksum al buffer a transmiir
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum

			// ldw		X,#bufferTxControl;			/ carga dirección del buffer a calcular chksum
			wreg = 0;		// clr		wreg
			waux = 16;		// mov 	waux,#16;							/ tamaño del bloque a calcular el chksum
			buildChksumBloq(&bufferTxControl[0],waux);// call	buildChksumBloq

			// ldw		X,chksum_HW
			// ldw		bufferTxControl+16,X
			// ldw		X,chksum_LW
			// ldw		bufferTxControl+18,X
			bufferTxControl[16] = ((chksum_32_HW_LW & 0xFF000000)>>24);
			bufferTxControl[17] = ((chksum_32_HW_LW & 0x00FF0000)>>16);
			bufferTxControl[18] = ((chksum_32_HW_LW & 0x0000FF00)>>8);
			bufferTxControl[19] = (chksum_32_HW_LW & 0x000000FF);

			// ldw		X,#bufferTxControl
			pointTx = &bufferTxControl[0]; 			// ldw		pointTx,X
			pointInitTx = &bufferTxControl[0];		// ldw		pointInitTx,X
			// ldw		X,#(bufferTxControl+20)
			pointEndTx = &bufferTxControl[20];// ldw		pointEndTx,X
			blockSizeTX = 20;// mov		blockSizeTX,#20

			flagsTX[2] = 0;		// bres	flagsTX,#2;						/ Indica que no hay que transmitir Header
			flagsTX[3] = 1;		// bset	flagsTX,#3;						/ evita enviar chksum

			keyTx = 0X55;		// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;			// clr		codeTX;

fin_tx_read_GEO:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_read_timeUNIX:
			blockSizeTX = 4 ; // mov		blockSizeTX,#4;				/ tamaño del bloque

			// Carga datos de Header
			Bloque_Header[softVersion1] = Plantilla[version1]; 	// mov		softVersion1,version1
			Bloque_Header[softVersion2] = Plantilla[version2];	// mov		softVersion2,version2;	/ Carga versión del firmware

			//ldw		X,#0
			//ldw		bufferSize_HW,X
			Bloque_Header [bufferSize_4] = highByte(0);	//*************** ????
			Bloque_Header [bufferSize_3] = lowByte(0);

			//ldw	X,#1
			Bloque_Header [bufferSize_2] = highByte(1); //ldw	bufferSize_LW,X; / carga el número de registros en caso de memoria llena
			Bloque_Header [bufferSize_1] = lowByte(1);

			Bloque_Header[dataType]  = 7;				// mov		dataType,#07
			Bloque_Header[dataSize] = blockSizeTX;		// mov		dataSize,blockSizeTX
			flagsTX[2] = 1;								// bset	flagsTX,#2;						/ Indica que hay que transmitir Header

			// carga header al buffer a transmitir
			//Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
			//copia los datos al buffer de tx

			//ldw		X,#softVersion1
			//ldw		Y,#bufferTxControl
			wreg = 8;// mov		wreg,#8
			copyVector(&Bloque_Header[softVersion1],&bufferTxControl[0]);// call	copyVector

			// Carga información de tiempo UNIX en el buffer a tranmitir
			// ldw		X,timeSeconds_HW
			bufferTxControl[8] = (uint8_t) ((timeSeconds_HW )>>8);	// ldw		bufferTxControl+8,X
			bufferTxControl[9] = (uint8_t) (timeSeconds_HW & 0x00FF);
			// ldw		X,timeSeconds_LW
			bufferTxControl[10] = (uint8_t) ((timeSeconds_LW )>>8);	// ldw		bufferTxControl+10,X
			bufferTxControl[11] = (uint8_t) (timeSeconds_LW & 0x00FF);

			// Añade chksum al buffer a transmiir
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW=0;	// ldw		chksum_LW,X;					/ limpia registros de checksum

			// ldw		X,#bufferTxControl;			/ carga dirección del buffer a calcular chksum
			wreg=0;				// clr		wreg
			waux=12;			// mov 	waux,#12;							/ tamaño del bloque a calcular el chksum
			buildChksumBloq(&bufferTxControl[0],waux);	// call	buildChksumBloq

			// ldw		X,chksum_HW
			// ldw		bufferTxControl+12,X
			// ldw		X,chksum_LW
			// ldw		bufferTxControl+14,X
			bufferTxControl[12] = (uint8_t) ((chksum_32_HW_LW & 0xFF000000)>>24);
			bufferTxControl[13] = (uint8_t) ((chksum_32_HW_LW & 0x00FF0000)>>16);
			bufferTxControl[14] = (uint8_t) ((chksum_32_HW_LW & 0x0000FF00)>>8);
			bufferTxControl[15] = (uint8_t) (chksum_32_HW_LW & 0x000000FF);

			// ldw		X,#bufferTxControl
			pointTx = &bufferTxControl[0];			// ldw		pointTx,X
			pointInitTx = &bufferTxControl[0];		// ldw		pointInitTx,X
			// ldw		X,#(bufferTxControl+16)
			pointEndTx = &bufferTxControl[16];		// ldw		pointEndTx,X
			blockSizeTX = 16; 						// mov		blockSizeTX,#16

			flagsTX[2] = 0; 						// bres	flagsTX,#2;						/ Indica que no hay que transmitir Header
			flagsTX[3] = 1;							// bset	flagsTX,#3;						/ evita enviar chksum

			keyTx = 0x55; 							// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0; 							// clr		codeTX;

fin_tx_read_timeUNIX:
			goto end_tx_control;				//jp		end_tx_control
//----------------------------------------------------------
tx_timeBCD:
// checa el chksum
			// clrw	X
			// ldw		chksum_HW,X
			chksum_32_HW_LW = 0;	// ldw		chksum_LW,X;					/ limpia registros de checksum

			// ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			wreg = 0; 				// clr		wreg
			waux = 11;				// mov 	waux,#11;							/ tamaño del bloque a calcular el chksum

			// ldw		X,resulh;
			// cpw		X,chksum_HW;					/ compara la parte baja del checksum
			// toma los datos de tiempo
			// ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			// addw	X,#2;
			// ldw		X,tempo2;
			// cpw		X,chksum_LW;
			buildChksumBloq(&RxBuffer_Ble[0],11);		// call	buildChksumBloq
			chksum_to_compare = 0;
			chksum_to_compare = ((uint32_t)RxBuffer_Ble[11])  <<24;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[12]) <<16;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[13]) <<8;
			chksum_to_compare += ((uint32_t)RxBuffer_Ble[14]);

			if(chksum_to_compare!= chksum_32_HW_LW){// jrne	tx_timeBCD_error;			/ si no iguales sal sin modificar Geolocalización
				goto tx_timeBCD_error;
			}
			// ldw		X,resulh;
			// cpw		X,chksum_HW;					/ compara la parte baja del checksum
			// toma los datos de tiempo
			// ldw		X,#RxBuffer_Ble;			/ carga dirección del buffer de recepción
			// addw	X,#2;									/ agrega offset para cargar bytes del tiempo UNIX

			// toma el dato de año
			// ld		A,(X);
			timeBCD_year = RxBuffer_Ble[2];// ld		timeBCD_year,A;
			//incw	X

			// toma el dato de mes
			//ld		A,(X);
			// ld		timeBCD_month,A;
			timeBCD_month = RxBuffer_Ble[3];
			//incw	X

			// ;toma el dato de dia
			// ld		A,(X);
			timeBCD_day = RxBuffer_Ble[4];// ld		timeBCD_day,A;
			//incw	X

			// toma el dato de hora
			// ld		A,(X);
			timeBCD_hour = RxBuffer_Ble[5];// ld		timeBCD_hour,A;
			// incw	X

			// ;toma el dato de minutos
			// ld		A,(X);
			timeBCD_min = RxBuffer_Ble[6];// ld		timeBCD_min,A;
			// incw	X

			//;toma el dato de segundos
			// ld		A,(X);
			timeBCD_sec = RxBuffer_Ble[7];// ld		timeBCD_sec,A;
			// incw	X

			// ;toma el dato de día de la semana
			//ld		A,(X);
			waux = RxBuffer_Ble[8];//ld		waux,A;
			//incw	X
			// rota cuatro posiciones (considera el carry) para dejar día de la semana en los bit 7,6 y 5
			/*rrc		waux
			rrc		waux
			rrc		waux
			rrc		waux*/
			waux = waux<<5;
			// enmascara  los bits 7,6 y 5
			// ld		A,#%11100000
			// and		A,waux
			// ;mezcla información de día de la semana con la de mes
			// or		A,timeBCD_month
			timeBCD_month |= (waux & 0xE0); // ld		timeBCD_month,A

			//;toma el dato de formato de hora
			//ld		A,(X);
			waux = RxBuffer_Ble[9];//ld		waux,A;
			// incw	X
			//; no proceses el dato únicamente asegura el formato de 24 horas
			//bres	timeBCD_hour,#6
			timeBCD_hour &= 0xBF;// BitClear(timeBCD_hour,6);
			// tampoco se porcesa el ultimo byte que corresponde a los millares y centenas de año

// -------------------------------------------------------------------------------
// Tiempo correctamente adquirido de la APP, carga los registros al RTC    RM_20230724
			/*mov     RTC_WPR,#$CA   ;Desbloquea la protección contra escritura
			mov     RTC_WPR,#$53   ;
			// Esta sección del código la Reliza la Funciones de HAL. Por ejemplo: HAL_StatusTypeDef HAL_RTC_SetTime
			bset    RTC_ISR1,#7    ;Modo de inicialización RTC_ISR1[INIT] = 1
			 */
espera_ini_rtc_1:          // Espera 2 RTCCLK (1/38000Hz * 2 = 50 us)
			/*btjf    RTC_ISR1,#6,espera_ini_rtc_1    ;Bandera RTC_ISR1[INITF] == 1

			bres    RTC_CR1,#6        ;Formato de 24 horas
			mov     RTC_DR3,timeBCD_year      ;AÑO
			mov     RTC_DR2,timeBCD_month     ;MES
			mov     RTC_DR1,timeBCD_day       ;DÍA
			mov     RTC_TR3,timeBCD_hour      ;HORAS
			mov     RTC_TR2,timeBCD_min       ;MINUTOS
			mov     RTC_TR1,timeBCD_sec       ;SEGUNDOS
			*/
			hRtcDate.Year = timeBCD_year ;
			hRtcDate.Month = (timeBCD_month & 0x1F);
			hRtcDate.Date = timeBCD_day;
			hRtcDate.WeekDay = ((timeBCD_month & 0xE0)>>5);

			HAL_RTC_SetDate(&hrtc, &hRtcDate, RTC_FORMAT_BCD);

			hRtcTime.Hours = timeBCD_hour;
			hRtcTime.Minutes = timeBCD_min;
			hRtcTime.Seconds = timeBCD_sec;
			hRtcTime.TimeFormat = RxBuffer_Ble[9];
			hRtcTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
			hRtcTime.StoreOperation = RTC_STOREOPERATION_RESET;
			HAL_RTC_SetTime(&hrtc, &hRtcTime, RTC_FORMAT_BCD);

			timeBCD_sec_ANT = timeBCD_sec;// mov     timeBCD_sec_ANT,timeBCD_sec       ;SEGUNDOS

			//bres    RTC_ISR1,#7   ;Termina modo de inicialización RTC_ISR1[INIT] = 0
			//mov     RTC_WPR,#$0   ;Bloquea la protección contra escritura con dato erroneo
			// -------------------------------------------------------------------------------
			flagsTime[f_timeConfigRTC]=0;// bset	flagsTime,#f_timeConfigRTC; Ididca que el RTC fue configurado con los datos recibidos .

			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1; // mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3D;// mov		comando2,#$3D;				/ indica que la grabación fue exitosa

			goto tx_timeBCD_ok;// jra		tx_timeBCD_ok

tx_timeBCD_error:
			// Carga datos de bloque para transmitir la respuesta
			BloqDatalooger[comando1] = 0xF1;// mov		comando1,#$F1
			BloqDatalooger[comando2] = 0x3E;// mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos

tx_timeBCD_ok:
			// ldw		X,#comando1
			pointTx = &BloqDatalooger[comando1];// ldw		pointTx,X
			pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
			// ldw		X,#comando2
			pointEndTx = &BloqDatalooger[comando2]; // ldw		pointEndTx,X
			blockSizeTX = 2;		// mov		blockSizeTX,#2

			flagsTX[3] = 1;			// bset	flagsTX,#3;						/ evita que se mande checksum
			keyTx = 0x55;			// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;				// clr		codeTX;

fin_tx_timeBCD:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_infoReloj:
			//typeClock = 0;					// mov		typeClock,#0;				Reloj interno
			// ;mov		typeClock,#1;				Reloj con señal de CA
			//if(!flagsTime[f_timeConfigRTC]){// btjf		flagsTime,#f_timeConfigRTC,noCristal;// Ya se configuro RTC ? no, no actualices tiempos
			//	goto noCristal;
			//}
			typeClock = 2;					// mov		typeClock,#2;				Reloj cristal
//noCristal:

			// ldw		X,#typeClock;				/ inicio del bloque
			pointTx = &typeClock ;				// ldw		pointTx,X
			pointInitTx = &typeClock;			// ldw		pointInitTx,X
			// ldw		X,#(typeClock + 1);				/ fin del bloque
			pointEndTx = pointInitTx + 1;// ldw		pointEndTx,X
			blockSizeTX = 1;// mov		blockSizeTX,#1;				/ tamaño del bloque

			//; Carga datos de Header
			Bloque_Header [softVersion1] = Plantilla[version1];	 // mov		softVersion1,version1
			Bloque_Header [softVersion2] = Plantilla[version2];	// mov		softVersion2,version2;	/ Carga versión del firmware

			//ldw		X,#0
			//ldw		bufferSize_HW,X
			Bloque_Header [bufferSize_4] = highByte(0);	//*************** ????
			Bloque_Header [bufferSize_3] = lowByte(0);

			//ldw	X,#1
			Bloque_Header [bufferSize_2] = highByte(1); //ldw	bufferSize_LW,X; / carga el número de registros en caso de memoria llena
			Bloque_Header [bufferSize_1] = lowByte(1);

			Bloque_Header[dataType] = 9;				// mov		dataType,#09
			Bloque_Header[dataSize] = blockSizeTX;		// mov		dataSize,blockSizeTX
			flagsTX[2]=1;								// bset	flagsTX,#2;						/ Indica que hay que transmitir Header

			//clrw	X
			//ldw		chksum_HW,X
			chksum_32_HW_LW = 0; 	//ldw		chksum_LW,X;					/ limpia registros de checksum
			flagsTX[3]=0;			// bres	flagsTX,#3;						/ indica que no se ha enviado el checksum
			keyTx = 0x55;			// mov		keyTx,#$55;						/ listo para mandar transmisión
			codeTX = 0;				// clr		codeTX;

fin_tx_infoReloj:
			goto end_tx_control;		//jp		end_tx_control
//----------------------------------------------------------
tx_wifi_datalogger:

			flagsWIFI[f_timeLoggerSend] = 1;	//bset		flagsWIFI,#f_timeLoggerSend;
			codeTX = 0;							//clr		codeTX;								/ limpia código de Tx

			goto end_tx_control;				//jp		end_tx_control

//;----------------------------------------------------------
tx_wifi_eventlogger:
			flagsWIFI[f_eventLoggerSend] = 1;	//bset		flagsWIFI,#f_eventLoggerSend;
			codeTX = 0;							//clr		codeTX;								/ limpia código de Tx

			goto end_tx_control;				//jp		end_tx_control


//----------------------------------------------------------
// prepTXlogg_2:     // 29-Oct-2024	RGM:		Se cambaia de lugar porque es una Subrrutina



end_tx_control:
		if (keyTx)	{						//tnz		keyTx;									/ no entres al loop hasta terminar transmisión de respuesta
			goto	no_resetLoopTx;			//	jrne	no_resetLoopTx
		}
		if(!flagsRxFirm[4]){ //btjf	flagsRxFirm,#4,ask_resetLoopTx
			goto ask_resetLoopTx;
		}
		keyWrFirm=0xAA;// mov		keyWrFirm,#$AA



ask_resetLoopTx:
		if (estado1[est1Reset]){							//btjt	estado1,#est1Reset,resetLoopTx_2;	/ Se necesita reset ?  Entra al loop hasta esperar el reset
			goto	resetLoopTx_2;
		}


resetLoopTx:
		//;nop
		//;btjt	flagsTX,#5,resetLoopTx;	/ Se necesita reset ?  Entra al loop hasta esperar el reset
		if (!flagsTX[5]){				/// Se necesita reset ?  Entra al loop hasta esperar el reset
			goto no_resetLoopTx;
		}
		asm ("nop");
		goto	resetLoopTx;		//jra		resetLoopTx


resetLoopTx_2:
		asm ("nop");
		goto	resetLoopTx_2;


no_resetLoopTx:
		goto	end_tx_control_b;				//jp		end_tx_control_b


tx_wifi:

		//if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)){ // btjt 		PE_IDR,#wifi_connect,tx_wifi_OK
		//	goto tx_wifi_OK;
		//}
		//; logger, telemetria y eventos por servidor solo están permitidos con comunicación WiFi seleccionada
		if(flagsTxControl[f_select])//btjt		flagsTxControl,#f_select,tx_wifi_01
			goto tx_wifi_01;
		goto end_tx_wifi;//jp			end_tx_wifi
tx_wifi_01:
		//;  Pregunta por el estado de la conexión a Servidor
		if(flagsTxControl[f_statWIFI])//btjt		flagsTxControl,#f_statWIFI,tx_wifi_OK
			goto tx_wifi_OK;
		flagsWIFI[f_serverConnect] = 0;// bres		flagsWIFI,#f_serverConnect; / baja bandera de conexión con servidor
		// en desconexión borra banderas de logger enviado para que se envíen a la reconexión
		flagsWIFI[f_timeLoggerSend] = 0;// bres		flagsWIFI,#f_timeLoggerSend;
		flagsWIFI[f_eventLoggerSend] = 0;// bres		flagsWIFI,#f_eventLoggerSend;
		flagsWIFI[f_timeLoggerCmd] = 0;// bres		flagsWIFI,#f_timeLoggerCmd;
		flagsWIFI[f_eventLoggerCmd] = 0;// bres		flagsWIFI,#f_eventLoggerCmd;
		cntRegTxWifi = 0;
		blockLoggWifi = 0;
		byteLoggWifi = 0;
		goto end_tx_wifi;// jp			end_tx_wifi

tx_wifi_OK:
		flagsWIFI[f_serverConnect] = 1;// bset		flagsWIFI,#f_serverConnect;/ Indica que modulo Wifi avisó conexxión con servidor

		// btjf	flagsRxFirm,#0,tx_wifi_OK_01; Empezó recepción de Firmware ?
		if(!flagsRxFirm[0]){
			goto tx_wifi_OK_01;
		}
		goto end_tx_wifi;// jp			end_tx_wifi;  Si ya empezó el intercambio de paquetes de firmware no se ejecutan los procesos wifi
tx_wifi_OK_01:

tx_wifi_OK_02:

		// tnz		keyTx;									/ se está atendiendo alguna transmisión?
	if(keyTx!=0){
		goto end_tx_wifi;// jrne	end_tx_wifi;						/ espera a que termine
	}

//;===========================================================
//;										Logger tiempo
//;===========================================================
tx_wifi_timeLoggerAsk:
		//; Pregunta si ya se envío el logger de tiempo
		//if(!flagsWIFI[f_timeLoggerSend]){
		if(flagsWIFI[f_timeLoggerSend]){
			goto tx_wifi_timeLogger;// btjf		flagsWIFI,#f_timeLoggerSend,tx_wifi_timeLogger
		}
		goto tx_wifi_eventLoggerAsk;	//jp			tx_wifi_eventLoggerAsk

tx_wifi_timeLogger:

// ;------------------------------------------------------------
// ;------------- Envío de comando y tiempo actual
// ; Ya se envió el comando con la estampa de tiempo ?
// ; si ya se mandó, espera respuesta de modulo WiFi.
		if(flagsWIFI[f_timeLoggerCmd]){// btjt		flagsWIFI,#f_timeLoggerCmd,tx_wifi_timeLogger_01
			goto tx_wifi_timeLogger_01;
		}
		// ; en caso contrario carga información para transmitir comando
		flagsWIFI[f_timeLoggerCmd] = 1;	//bset		flagsWIFI,#f_timeLoggerCmd
		//;Sí el contador no viene en cero no grabes datos
		//ldw		X,cntRegTxWifi
		//tnzw	X
		//jrne	tx_wifi_timeLogger_cmd
		if(cntRegTxWifi)
			goto tx_wifi_timeLogger_cmd;

		// ;primero guarda lo que aun hay en el buffer .
		cntBlockFlash = cntBlockDATA;// mov		cntBlockFlash,cntBlockDATA; /pasa el número de bloques de datos grabados
		cntByteBlock = cntByteBlockDATA;// mov		cntByteBlock,cntByteBlockDATA
		// ldw		X,#data_buffer
		dirBuffer = &data_buffer[0];// ldw		dirBuffer,X
		// ldw		X,#dataLogger
		dirLogger = &dataLogger[0]; //ldw		dirLogger,X

// ;---- Carga en penúltimo byte del buffer el numero de bytes capturados en el actual buffer
		// LDW		X,dirBuffer
		// clr		resulh
		// mov		resull,#126
		// addw	X,resulh
		// ld		A,cntByteBlock
		dirBuffer[126] = cntByteBlock;// ld		(X),A

//;---- Graba buffer en bloque de flash
		ProgMemCode = 0xAA;// mov		ProgMemCode,#$AA;				/ Indica que se va a grabar bloque de Flash
		// ld    A,cntBlockFlash;				/ Toma el número de bloques grabados en Flash
		// ldw		X,#128;									/ Carga el tamaño de los bloques (128 bytes)
		// mul		X,A;										/ Multiplicalos
		// addw	X,dirLogger;						/	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
		dirPointer = dirLogger + 128*cntBlockFlash;// LDW		dirPointer,X
		// LDW		X,dirBuffer;						/ apunta al buffer de datos en RAM
		dataPointer = dirBuffer;// LDW		dataPointer,X
		GRABA_BLOCK();// call	GRABA_BLOCK

		// ldw		X,cntRegDATA
		cntReg = cntRegDATA;// ldw		cntReg,X
		// ldw		X,#eeCntRegDATA
		cntRegPNT = &eeCntRegDATA;// ldw		cntRegPNT,X
		reeCntRegDATA = cntReg;

		save_cntReg();// call	save_cntReg

		save_timeUNIX();// call	save_timeUNIX

tx_wifi_timeLogger_cmd:
		//; carga comando
		// ldw		X,#$4085
		bufferWifiTx[0] = 0x40;// ldw		bufferWifiTx,X
		bufferWifiTx[1] = 0x85;
		// ; carga versión de firmware
		// ldw		X,eeversion1
//		bufferWifiTx[2] = eePlantilla[eeversion1];// ldw		bufferWifiTx+2,X
//		bufferWifiTx[3] = eePlantilla[eeversion2];
		bufferWifiTx[2] = reePlantilla[eeversion1];
		bufferWifiTx[3] = reePlantilla[eeversion2];
		// ; carga hora actual
		// ldw		X,timeSeconds_HW
		bufferWifiTx[4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8);// ldw		bufferWifiTx+4,X
		bufferWifiTx[5] = (uint8_t) ((timeSeconds_HW & 0x00FF));
		// ldw		X,timeSeconds_LW
		bufferWifiTx[6] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8);// ldw		bufferWifiTx+6,X
		bufferWifiTx[7] = (uint8_t) ((timeSeconds_LW & 0x00FF));
		// ;carga también datos de geolocalización
		// ldw		X,eeLat1
		// ldw		bufferWifiTx+8,X
		// ldw		X,eeLat3
		// ldw		bufferWifiTx+10,X
		// ldw		X,eeLong1
		//ldw		bufferWifiTx+12,X
		//ldw		X,eeLong3
		//ldw		bufferWifiTx+14,X


//		bufferWifiTx[8] = eeLat1;
//		bufferWifiTx[9] = eeLat2;
//		bufferWifiTx[10] = eeLat3;
//		bufferWifiTx[11] = eeLat4;
//		bufferWifiTx[12] = eeLong1;
//		bufferWifiTx[13] = eeLong2;
//		bufferWifiTx[14] = eeLong3;
//		bufferWifiTx[15] = eeLong4;
		bufferWifiTx[8] = reeLat1;
		bufferWifiTx[9] = reeLat2;
		bufferWifiTx[10] = reeLat3;
		bufferWifiTx[11] = reeLat4;
		bufferWifiTx[12] = reeLong1;
		bufferWifiTx[13] = reeLong2;
		bufferWifiTx[14] = reeLong3;
		bufferWifiTx[15] = reeLong4;

		// ;define inicio, fin y tamaño de bloque de tx
		// ldw		X,#bufferWifiTx
		pointTx = &bufferWifiTx[0];// ldw		pointTx,X
		pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
		// ldw		X,#(bufferWifiTx+16)
		pointEndTx= &bufferWifiTx[16];	// ldw		pointEndTx,X
		blockSizeTX = 16;// mov		blockSizeTX,#16

		flagsTX[2]=0; // bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum
		flagsTX[3]=1;//bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0x55;// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;// clr		codeTX;								/ limpia código de Tx

		//;Sí el contador no viene en cero continua utilizando los punteros sin iniciarlos
		//ldw		X,cntRegTxWifi
		//tnzw	X
		//jrne	tx_wifi_timeLogger_noInit
		if(cntRegTxWifi)
			goto tx_wifi_timeLogger_noInit;

/*;Establece punteros iniciales y finales del envío de datos de tiempo
; pasa el número de bloques y bytes de datos de tiempo grabados
; limpia el contador de registros transmitidos
;	carga timeout en segundos*/

		blockLoggWifi = cntBlockDATA;// mov		blockLoggWifi,cntBlockDATA
		byteLoggWifi = cntByteBlockDATA;// mov		byteLoggWifi,cntByteBlockDATA
		// clrw	X
		cntRegTxWifi = 0;// ldw		cntRegTxWifi,X
tx_wifi_timeLogger_noInit:
		timeoutTxWifi = 3;// mov		timeoutTxWifi,#3

		//; Indica cuantos registros se loggearon para mandar unicamente esa cantidad de registros
		// ldw		X,eeCntRegDATA
		// tnzw	X;										/ se logearon registros ?
		//if(eeCntRegDATA==0){
		//if(findLastValue((uint32_t)&eeCntRegDATA) == 0){
		//	goto tx_wifi_timeLogger_END;// jreq	tx_wifi_timeLogger_END; / no, finaliza envío de logger de datos
		//}
		//numRegTx = eeCntRegDATA;// ldw		numRegTx,X
		//numRegTx = findLastValue((uint32_t)&eeCntRegDATA);
		numRegTx = 1343;
		goto end_tx_wifi;// jp		end_tx_wifi
/*;------------------------------------------------------------
;------------- Validación de respuesta, time out y envío de logger por Registro
*/
tx_wifi_timeLogger_01:
		// ld		A,codeTX
		// cp		A,#$3D
		if(codeTX==0x3D){
			goto tx_wifi_timeLogger_03; // jreq	tx_wifi_timeLogger_03
		}
		codeTX = 0;
		//; checa timeout
		// tnz		timeoutTxWifi
		if(timeoutTxWifi != 0){
			goto tx_wifi_timeLogger_02;// jrne	tx_wifi_timeLogger_02
		}
		flagsWIFI[f_timeLoggerCmd]=0;// bres	flagsWIFI,#f_timeLoggerCmd

tx_wifi_timeLogger_02:
		goto end_tx_wifi;// jp		end_tx_wifi

tx_wifi_timeLogger_03:
		//; Mientras exista comunicacion exitosa en este proceso mante timeout cargado
		//ldw		X,#300;
		timeoutTWF = 300;//ldw		timeoutTWF,X;					/ carga time out de Token (5 min)
		//ldw		X,#300;
		timeoutTBLE = 300;//ldw		timeoutTBLE,X;					/ carga time out de Token (5 min)

		//;	carga timeout en segundos
		timeoutTxWifi = 3;// mov		timeoutTxWifi,#3
		//; checa si ya se terminaron de enviar todos los registros del logger

		// ldw		X,numRegTx
		// ldw		resulh,X
		// ldw		X,cntRegTxWifi
		// cpw		X,resulh
		 if(numRegTx < cntRegTxWifi){
			 goto tx_wifi_timeLogger_04;//jrult	tx_wifi_timeLogger_04
		 }

		// clrw	X
		//cntRegDATA = 0;// ldw		cntRegDATA,X
		//cntReg = 0;// ldw		cntReg,X
		// ldw		X,#eeCntRegDATA
		//cntRegPNT = &eeCntRegDATA;// ldw		cntRegPNT,X
		//save_cntReg();// call	save_cntReg

tx_wifi_timeLogger_END:
		//; indica que ya es enviaron todos los paquetes
		//flagsWIFI[f_timeLoggerSend] = 1;// bset	flagsWIFI,#f_timeLoggerSend
		flagsWIFI[f_timeLoggerSend] = 0;
		flagsWIFI[f_timeLoggerCmd] = 0;	// bres	flagsWIFI,#f_timeLoggerCmd; // borra bandera de comando para liberar envío de token
		delayTxLoggWifi = 10;// mov		delayTxLoggWifi,#10; / carga un retardo para comenzar a envíar el siguiente logger en segundos

		blockLoggWifi = 0; 		//clr	blockLoggWifi
		byteLoggWifi = 0;		//clr		byteLoggWifi
		//clrw	X
		cntRegTxWifi = 0;//ldw		cntRegTxWifi,X

		goto end_tx_wifi;// jp		end_tx_wifi

tx_wifi_timeLogger_04:

		//;manda logger registro por registro
		// ldw		X,#$4083
		bufferWifiTx [0] = 0x40;	// ldw		bufferWifiTx,X
		bufferWifiTx [1] = 0x83;
		//; manda cantidad de datos de 2 bytes en la Tx
		bufferWifiTx[2] = 3;// mov		bufferWifiTx+2,#3
		// ; carga versión de firmware
		// ldw		X,eeversion1
		//bufferWifiTx[3] = eePlantilla[eeversion1];	// ldw		bufferWifiTx+3,X
		//bufferWifiTx[4] = eePlantilla[eeversion2];
		bufferWifiTx[3] = reePlantilla[eeversion1];
		bufferWifiTx[4] = reePlantilla[eeversion2];
tx_wifi_timeLogger_loadLogger_01:

		/* ; Si el contador de bytes loggeado viene en cero quiere decir que ya no hay información en el bloque actual de 128 bytes
		; por lo tanto hay que decrementar el contador de bloques y reubicar el contado de bytes*/
		// tnz		byteLoggWifi
		if(byteLoggWifi != 0){
			goto tx_wifi_timeLogger_loadLogger_03;// jrne	tx_wifi_timeLogger_loadLogger_03
		}
		byteLoggWifi = offsetTimeLogg;// mov		byteLoggWifi,#offsetTimeLogg
		//; Si el contador de bloques llego a cero, hay que cargar el contador de bloques con su valor maximo
		// tnz		blockLoggWifi
		if(blockLoggWifi != 0){
			goto tx_wifi_timeLogger_loadLogger_02;// jrne	tx_wifi_timeLogger_loadLogger_02
		}
		blockLoggWifi = numBlockDATA;// mov		blockLoggWifi,#numBlockDATA

tx_wifi_timeLogger_loadLogger_02:
		blockLoggWifi--;// dec		blockLoggWifi; Ajuste

tx_wifi_timeLogger_loadLogger_03:

		point_Y = &bufferWifiTx[5 + sizeRegTimeLogg];// LDW		Y,#(bufferWifiTx + 5 + sizeRegTimeLogg);	/ apunta al buffer de datos en RAM
		// ld    A,blockLoggWifi;				/ Toma el número de bloques grabados en Flash
		// ldw		X,#128;									/ Carga el tamaño de los bloques (128 bytes)
		// mul		X,A;										/ Multiplicalos
		// addw	X,#dataLogger;				/	apunta al inicio de la Flash resevada para Logger de datos de tiempo + el número de bloques grabados
		// clr		resulh
		// mov		resull,byteLoggWifi
		point_X = &dataLogger[128*blockLoggWifi+byteLoggWifi];// addw	X,resulh;							/ suma la posición de bytes
		wreg = 0;// clr		wreg

tx_wifi_timeLogger_loadLogger_04:
		point_X--;// decw	X;
		point_Y--;// decw	Y;											/ decrementa apuntadores
		byteLoggWifi--;// dec		byteLoggWifi;						/ al mismo tiempo decrementa el contador de bytes de logger
		// LDF		A,($010000,X)
		*point_Y = *(point_X+0x010000);// LD		(Y),A;
		wreg++;//inc		wreg
		// ld		A,wreg
		// cp		A,#sizeRegTimeLogg
		if(wreg < sizeRegTimeLogg){
			goto tx_wifi_timeLogger_loadLogger_04;//jrult tx_wifi_timeLogger_loadLogger_04
		}

		//;Ajuste del dato de voltaje
		waux = bufferWifiTx[13];// mov		waux,bufferWifiTx+13
		bufferWifiTx[13] = 0;// clr		bufferWifiTx+13
		bufferWifiTx[14] = waux;	//mov		bufferWifiTx+14,waux

		// ;define inicio, fin y tamaño de bloque de tx
		// ldw		X,#bufferWifiTx
		pointTx = &bufferWifiTx[0];// ldw		pointTx,X
		pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
		// ldw		X,#(bufferWifiTx+15)
		pointEndTx = &bufferWifiTx[15];// ldw		pointEndTx,X
		blockSizeTX = 15; // mov		blockSizeTX,#15

		flagsTX[2]=0;// bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum
		flagsTX[3] =1;	// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0x55;	// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;// clr		codeTX;								/ limpia código de Tx

		// incrementa el contador de registros enviados
		// ldw		X,cntRegTxWifi
		// incw	X
		cntRegTxWifi++; //ldw		cntRegTxWifi,X

		goto end_tx_wifi;// jp			end_tx_wifi
		// ;===========================================================
		// ;										Logger eventos
		// ;===========================================================
tx_wifi_eventLoggerAsk:
		// ; Pregunta si ya se envío el logger de eventos
		//if(!flagsWIFI[f_eventLoggerSend]){
		if(flagsWIFI[f_eventLoggerSend]){
			goto tx_wifi_eventLogger;// btjf		flagsWIFI,#f_eventLoggerSend,tx_wifi_eventLogger
		}
		goto tx_wifi_eventData;// jp			tx_wifi_eventData

tx_wifi_eventLogger:
		//;------------------------------------------------------------
		//;------------- Espera retardo para comenzar con rutina Tx logger eventos
		//;Verifica si ya se consumio el retardo para comenzar la transmisión
		// tnz		delayTxLoggWifi
//		if(delayTxLoggWifi==0){
//			goto tx_wifi_eventDelayAsk;// jreq	tx_wifi_eventDelayAsk
//		}
//		goto end_tx_wifi;// jp		end_tx_wifi

//tx_wifi_eventDelayAsk:
		//;------------------------------------------------------------
		//;------------- Envío de comando y tiempo actual
		//; Ya se envió el comando con la estampa de tiempo ?
		//; si ya se mandó, espera respuesta de modulo WiFi.
		if(flagsWIFI[f_eventLoggerCmd]){
			goto tx_wifi_eventLogger_01;// btjt		flagsWIFI,#f_eventLoggerCmd,tx_wifi_eventLogger_01
		}

		//; en caso contrario carga información para transmitir comando
		flagsWIFI[f_eventLoggerCmd]=1;// bset		flagsWIFI,#f_eventLoggerCmd

		//;Sí el contador no viene en cero no grabes datos
		//ldw		X,cntRegTxWifi
		//tnzw	X
		//jrne	tx_wifi_eventLogger_cmd
		if(cntRegTxWifi)
			goto tx_wifi_eventLogger_cmd;
		//;primero guarda lo que aun hay en el buffer .
		cntBlockFlash = cntBlockEVENT;// mov		cntBlockFlash,cntBlockEVENT; /pasa el número de bloques de datos grabados
		cntByteBlock = cntByteBlockEVENT;// mov		cntByteBlock,cntByteBlockEVENT
		// ldw		X,#event_buffer
		dirBuffer = &event_buffer[0];// ldw		dirBuffer,X
		// ldw		X,#eventLogger
		dirLogger = &eventLogger[0];// ldw		dirLogger,X

		//;---- Carga en penúltimo byte del buffer el numero de bytes capturados en el actual buffer
		// LDW		X,dirBuffer
		// clr		resulh
		// mov		resull,#126
		// addw	X,resulh
		// ld		A,cntByteBlock
		event_buffer[126]=cntByteBlock;// ld		(X),A

		//;---- Graba buffer en bloque de flash
		ProgMemCode = 0xAA;// mov		ProgMemCode,#$AA;				/ Indica que se va a grabar bloque de Flash
		// ld    A,cntBlockFlash;				/ Toma el número de bloques grabados en Flash
		// ldw		X,#128;									/ Carga el tamaño de los bloques (128 bytes)
		//mul		X,A;										/ Multiplicalos
		//addw	X,dirLogger;						/	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
		dirPointer = &dirLogger[128*cntBlockFlash];// LDW		dirPointer,X
		// LDW		X,dirBuffer;						/ apunta al buffer de datos en RAM
		dataPointer = dirBuffer;// LDW		dataPointer,X
		GRABA_BLOCK();// call	GRABA_BLOCK

		// ldw		X,cntRegEVENT
		cntReg = cntRegEVENT;// ldw		cntReg,X
		//ldw		X,#eeCntRegEVENT
		cntRegPNT = &eeCntRegEVENT;// ldw		cntRegPNT,X
		reeCntRegEVENT = cntReg;

		save_cntReg();		// call	save_cntReg

		save_timeUNIX();	// call	save_timeUNIX

tx_wifi_eventLogger_cmd:
		//; carga comando
		// ldw		X,#$4086
		bufferWifiTx[0] = 0x40;// ldw		bufferWifiTx,X
		bufferWifiTx[1] = 0x86;
		//; carga versión de firmware
		// ldw		X,eeversion1
		//bufferWifiTx[2] = eePlantilla[eeversion1];// ldw		bufferWifiTx+2,X
		//bufferWifiTx[3] = eePlantilla[eeversion2];
		bufferWifiTx[2] = reePlantilla[eeversion1];
		bufferWifiTx[3] = reePlantilla[eeversion2];
		//; carga hora actual
		// ldw		X,timeSeconds_HW
		bufferWifiTx[4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8); // ldw		bufferWifiTx+4,X
		bufferWifiTx[5] = (uint8_t) ((timeSeconds_HW & 0x00FF));
		// ldw		X,timeSeconds_LW
		bufferWifiTx[6] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8); // ldw		bufferWifiTx+6,X
		bufferWifiTx[7] = (uint8_t) ((timeSeconds_LW & 0x00FF));
		//;carga también datos de geolocalización
		/*ldw		X,eeLat1
		ldw		bufferWifiTx+8,X
		ldw		X,eeLat3
		ldw		bufferWifiTx+10,X
		ldw		X,eeLong1
		ldw		bufferWifiTx+12,X
		ldw		X,eeLong3
		ldw		bufferWifiTx+14,X*/
//		bufferWifiTx[8] = eeLat1;
//		bufferWifiTx[9] = eeLat2;
//		bufferWifiTx[10] = eeLat3;
//		bufferWifiTx[11] = eeLat4;
//		bufferWifiTx[12] = eeLong1;
//		bufferWifiTx[13] = eeLong2;
//		bufferWifiTx[14] = eeLong3;
//		bufferWifiTx[15] = eeLong4;

		bufferWifiTx[8] = reeLat1;
		bufferWifiTx[9] = reeLat2;
		bufferWifiTx[10] = reeLat3;
		bufferWifiTx[11] = reeLat4;
		bufferWifiTx[12] = reeLong1;
		bufferWifiTx[13] = reeLong2;
		bufferWifiTx[14] = reeLong3;
		bufferWifiTx[15] = reeLong4;


		// ;define inicio, fin y tamaño de bloque de tx
		// ldw		X,#bufferWifiTx
		pointTx = &bufferWifiTx[0];// ldw		pointTx,X
		pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
		// ldw		X,#(bufferWifiTx+16)
		pointEndTx= &bufferWifiTx[16];	// ldw		pointEndTx,X
		blockSizeTX = 16;// mov		blockSizeTX,#16

		flagsTX[2] = 0; // bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum
		flagsTX[3] = 1;// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0X55; 	// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;//	clr		codeTX;								/ limpia código de Tx

//		;Sí el contador no viene en cero continua utilizando los punteros sin iniciarlos
//		ldw		X,cntRegTxWifi
//		tnzw	X
//		jrne	tx_wifi_eventLogger_noInit
		if(cntRegTxWifi)
			goto tx_wifi_eventLogger_noInit;


		/*;Establece punteros iniciales y finales del envío de eventos
		; pasa el número de bloques y bytes de datos de tiempo grabados
		; limpia el contador de registros transmitidos
		;	carga timeout en segundos*/
		blockLoggWifi = cntBlockEVENT;// mov		blockLoggWifi,cntBlockEVENT
		byteLoggWifi = cntByteBlockEVENT;// mov		byteLoggWifi,cntByteBlockEVENT
		// clrw	X
		cntRegTxWifi = 0;	// ldw		cntRegTxWifi,X

tx_wifi_eventLogger_noInit:
		timeoutTxWifi = 3;	// mov		timeoutTxWifi,#3

		//; Indica cuantos registros se loggearon para mandar unicamente esa cantidad de registros
		// ldw		X,eeCntRegEVENT
		// tnzw	X;										/ se logearon registros ?
		//if(eeCntRegEVENT==0){
		//if(findLastValue((uint32_t)&eeCntRegEVENT)==0){
		//	goto tx_wifi_eventLogger_END;// jreq	tx_wifi_eventLogger_END; / no, finaliza envío de logger de datos
		//}
		//numRegTx = eeCntRegEVENT;// ldw		numRegTx,X
		//numRegTx = findLastValue((uint32_t)&eeCntRegEVENT);
		numRegTx = 1439;
		goto end_tx_wifi;// jp		end_tx_wifi
//;------------------------------------------------------------
//;------------- Validación de respuesta, time out y envío de logger por Registro
tx_wifi_eventLogger_01:
		// ld		A,codeTX
		// cp		A,#$3D
		if(codeTX == 0x3D){
			goto tx_wifi_eventLogger_03;// jreq	tx_wifi_eventLogger_03
		}
		codeTX = 0;
		//; checa timeout
		// tnz		timeoutTxWifi
		if(timeoutTxWifi != 0){
			goto tx_wifi_eventLogger_02;// jrne	tx_wifi_eventLogger_02
		}
		flagsWIFI[f_eventLoggerCmd] = 0;// bres	flagsWIFI,#f_eventLoggerCmd

tx_wifi_eventLogger_02:
		goto end_tx_wifi;// jp		end_tx_wifi

tx_wifi_eventLogger_03:
//		; Mientras exista comunicacion exitosa en este proceso mante timeout cargado
//		ldw		X,#300;
//		ldw		timeoutTWF,X;					/ carga time out de Token (5 min)
//		ldw		X,#300;
//		ldw		timeoutTBLE,X;					/ carga time out de Token (5 min)
		timeoutTWF = 300;
		timeoutTBLE = 300;
		//;	carga timeout en segundos
		timeoutTxWifi = 3;// mov		timeoutTxWifi,#3
		//; checa si ya se terminaron de enviar todos los registros del logger
		// ldw		X,numRegTx
		// ldw		resulh,X
		// ldw		X,cntRegTxWifi
		// cpw		X,resulh
		if(numRegTx < cntRegTxWifi){
			goto tx_wifi_eventLogger_04;// jrult	tx_wifi_eventLogger_04
		}

		// clrw	X
//		cntRegEVENT = 0;// ldw		cntRegEVENT,X
//		cntReg = 0;// ldw		cntReg,X
//		// ldw		X,#eeCntRegEVENT
//		cntRegPNT = &eeCntRegEVENT;// ldw		cntRegPNT,X
//		save_cntReg();// call	save_cntReg

tx_wifi_eventLogger_END:
		//; indica que ya es enviaron todos los paquetes
		//flagsWIFI[f_eventLoggerSend] = 1; 	// bset	flagsWIFI,#f_eventLoggerSend
		flagsWIFI[f_eventLoggerSend] = 0;
		flagsWIFI[f_eventLoggerCmd] = 0;	// bres	flagsWIFI,#f_eventLoggerCmd; // borra bandera de comando para liberar envío de token
		delayTxLoggWifi = 10;				// mov		delayTxLoggWifi,#10; / carga un retardo para comenzar a envíar el siguiente logger en segundos
		blockLoggWifi = 0;//clr		blockLoggWifi
		byteLoggWifi = 0;//clr		byteLoggWifi
		//clrw	X
		cntRegTxWifi = 0;//ldw		cntRegTxWifi,X

		goto end_tx_wifi;//jp		end_tx_wifi
tx_wifi_eventLogger_04:

		//;manda logger registro por registro
		// ldw		X,#$4084
		bufferWifiTx[0] = 0x40;// ldw		bufferWifiTx,X
		bufferWifiTx[1] = 0x84;
		//; manda cantidad de datos de 2 bytes en la Tx
		bufferWifiTx[2] = 3;// mov		bufferWifiTx+2,#3
		//; carga versión de firmware
		// ldw		X,eeversion1
		//bufferWifiTx[3] = eePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
		//bufferWifiTx[4] = eePlantilla[eeversion2];
		bufferWifiTx[3] = reePlantilla[eeversion1];
		bufferWifiTx[4] = reePlantilla[eeversion2];
tx_wifi_eventLogger_loadLogger_01:

		//; Si el contador de bytes loggeado viene en cero quiere decir que ya no hay información en el bloque actual de 128 bytes
		//; por lo tanto hay que decrementar el contador de bloques y reubicar el contado de bytes
		// tnz		byteLoggWifi
		if(byteLoggWifi!=0){
			goto tx_wifi_eventLogger_loadLogger_03;// jrne	tx_wifi_eventLogger_loadLogger_03
		}
		byteLoggWifi = offsetEventLogg;// mov		byteLoggWifi,#offsetEventLogg
		//; Si el contador de bloques llego a cero, hay que cargar el contador de bloques con su valor maximo
		// tnz		blockLoggWifi
		if(blockLoggWifi!=0){
			goto tx_wifi_eventLogger_loadLogger_02;// jrne	tx_wifi_eventLogger_loadLogger_02
		}
		blockLoggWifi = numBlockEVENT;// mov		blockLoggWifi,#numBlockEVENT

tx_wifi_eventLogger_loadLogger_02:
		blockLoggWifi--;// dec		blockLoggWifi; Ajuste

tx_wifi_eventLogger_loadLogger_03:

		point_Y = &bufferWifiTx[5 + sizeRegEventLogg];// LDW		Y,#(bufferWifiTx + 5 + sizeRegEventLogg);	/ apunta al buffer de datos en RAM
		/*ld    A,blockLoggWifi;				/ Toma el número de bloques grabados en Flash
		ldw		X,#128;									/ Carga el tamaño de los bloques (128 bytes)
		mul		X,A;										/ Multiplicalos
		addw	X,#eventLogger;				/	apunta al inicio de la Flash resevada para Logger de eventos + el número de bloques grabados
		clr		resulh
		mov		resull,byteLoggWifi
		addw	X,resulh;							/ suma la posición de bytes*/
		point_X = &eventLogger[byteLoggWifi+128*blockLoggWifi];
		wreg = 0;// clr		wreg

tx_wifi_eventLogger_loadLogger_04:
		point_X--;// decw	X;
		point_Y--;// decw	Y;											/ decrementa apuntadores
		byteLoggWifi--;//dec		byteLoggWifi;						/ al mismo tiempo decrementa el contador de bytes de logger
		// LDF		A,($010000,X)
		*point_Y = *(point_X+0x010000);// LD		(Y),A;
		wreg++;//inc		wreg
		// ld		A,wreg
		// cp		A,#sizeRegEventLogg
		if(wreg<sizeRegEventLogg){
			goto tx_wifi_eventLogger_loadLogger_04;// jrult tx_wifi_eventLogger_loadLogger_04
		}

		//;Ajuste del dato de voltaje
		waux = bufferWifiTx[18];// mov		waux,bufferWifiTx+18
		bufferWifiTx[18] = 0;// clr		bufferWifiTx+18
		bufferWifiTx[19] = waux;// mov		bufferWifiTx+19,waux

		//;define inicio, fin y tamaño de bloque de tx

		// incrementa el contador de registros enviados


		pointTx = &bufferWifiTx[0];// ldw		pointTx,X
		pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
				// ldw		X,#(bufferWifiTx+20)
		pointEndTx= &bufferWifiTx[20];	// ldw		pointEndTx,X
		blockSizeTX = 20;// mov		blockSizeTX,#20

		flagsTX[2] = 0; // bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW = 0;// ldw		chksum_LW,X;					/ limpia registros de checksum
		flagsTX[3] = 1;// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0x55; 	// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;//	clr		codeTX;								/ limpia código de Tx

		// ldw		X,cntRegTxWifi
		// incw	X

		cntRegTxWifi++;// ldw		cntRegTxWifi,X
		goto end_tx_wifi; // jp			end_tx_wifi
//;===========================================================
//;										EVENTOS
//;===========================================================
tx_wifi_eventData:

//;------------------------------------------------------------
//;------------- Espera retardo para comenzar con rutina Tx logger eventos
//;Verifica si ya se consumio el retardo para comenzar la transmisión
		// tnz		delayTxLoggWifi
		if(delayTxLoggWifi == 0){
			goto tx_wifi_eventData_01;// jreq	tx_wifi_eventData_01
		}
		goto end_tx_wifi;// jp		end_tx_wifi
tx_wifi_eventData_01:

		// tnz		keyTx;									/ se está atendiendo alguna transmisión?
		if(keyTx!=0){
			goto end_tx_wifi;// jrne	end_tx_wifi;						/ espera a que termine
		}

//;----------------------------------------------------------
//;										Evento de apertura de puerta
//;----------------------------------------------------------
door_event_WF:

		if(flagsEventWF[0]){
			goto ask_DE_end_WF;// btjt	flagsEventWF,#0,ask_DE_end_WF;	/ ya había iniciado evento puerta? Sí, checa si ya se termina evento
		}
ask_DE_start_WF:
		if(flagsC[0]){
			goto ask_DE_start_01_WF;// btjt	flagsC,#0,ask_DE_start_01_WF; / hay puerta abierta ? Sí, captura inicio de evento
		}
		goto comp_event_WF;// jp		comp_event_WF;							/ No, checa el evento de compresor
ask_DE_start_01_WF:
		// ldw		X,#$4082
		BloqEventPuerta[comandoEP_2] = 0x40;// ldw		comandoEP,X
		BloqEventPuerta[comandoEP_1] = 0x82;
		//BloqEventPuerta[softVersion1EP] = eePlantilla[eeversion1];// mov		softVersion1EP,eeversion1
		//BloqEventPuerta[softVersion2EP] = eePlantilla[eeversion2];
		BloqEventPuerta[softVersion1EP] = reePlantilla[eeversion1];
		BloqEventPuerta[softVersion2EP] = reePlantilla[eeversion2];
		// mov		softVersion2EP,eeversion2

		// ldw		X,timeSeconds_HW
		BloqEventPuerta[EP_timeInit_4] = (uint8_t) ((timeSeconds_HW & 0xFF00) >> 8);// ldw		EP_timeInit_HW,X
		BloqEventPuerta[EP_timeInit_3] = (uint8_t) (timeSeconds_HW & 0x00FF);
		// ldw		X,timeSeconds_LW
		BloqEventPuerta[EP_timeInit_2] = (uint8_t) ((timeSeconds_LW & 0xFF00) >> 8);// ldw		EP_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventPuerta[EP_timeInit_1] = (uint8_t) (timeSeconds_LW & 0x00FF);

		BloqEventPuerta[EP_eventType] = 1; // mov		EP_eventType,#1;				/ carga el tipo de evento (1 para puerta)
		//ldw		X,tdevl
		BloqEventPuerta[EP_tempAmbInit_H] = tdevl;//ldw		EP_tempAmbInit,x;				/ carga temperatura ambiente
		BloqEventPuerta[EP_tempAmbInit_L] = tdevf;
		BloqEventPuerta[EP_voltInit] = voltl;		// mov		EP_voltInit,voltl; /carga voltaje
		flagsEventWF[0] = 1;						// bset	flagsEventWF,#0;					/ indica quie el evento puerta ya inició
		// ld		A,#0;
		// ld		xh,A
		// ld		A,tmDoorEvent;					/ carga el tiempo de puerta abierta necesario para considerarlo evento
		// ld		xl,A
		temp_doorEvent = Plantilla[tmDoorEvent];// ldw		temp_doorEvent,X;				/ carga tiempo de duración minima de evento puerta abierta
		goto comp_event_WF;// jp		comp_event_WF;							/ continúa
ask_DE_end_WF:
		if(flagsC[0]){
			goto comp_event_WF;// btjt	flagsC,#0,comp_event_WF;	 	/ ya se cerró la puerta ?
		}
		// ldw		X,temp_doorEvent
		// tnzw	X;											/ se cumplió el tiempo minimo de puerta abierta ?
		if(temp_doorEvent==0){
			goto door_event_end_WF;//jreq	door_event_end_WF;					/ sí, ve a finalizar el evento
		}
		flagsEventWF[0] = 0;// bres	flagsEventWF,#0;					/ borra inicio de evento puerta
		goto comp_event_WF;// jra		comp_event_WF;							/ continúa sin grabar evento
door_event_end_WF:
		// ldw		X,timeSeconds_HW
		BloqEventPuerta[EP_timeEnd_4]=(uint8_t) ((timeSeconds_HW & 0xFF00)>>8);// ldw		EP_timeEnd_HW,X
		BloqEventPuerta[EP_timeEnd_3]=(uint8_t) (timeSeconds_HW & 0xFF);
		BloqEventPuerta[EP_timeEnd_2]=(uint8_t) ((timeSeconds_LW & 0xFF00)>>8);// ldw		X,timeSeconds_LW
		BloqEventPuerta[EP_timeEnd_1]=(uint8_t) (timeSeconds_LW & 0xFF);// ldw		EP_timeEnd_LW,X;				/ guarda el tiempo final
		// ldw		X,teval
		BloqEventPuerta[EP_tempEvaEnd_H]= teval;// ldw		EP_tempEvaEnd,x;				/ copia el dato de temperatura evaporador
		BloqEventPuerta[EP_tempEvaEnd_L]= tevaf;
		flagsEventWF[0]= 0;		// bres	flagsEventWF,#0;					/ borra inicio de evento puerta
		// ldw		X,#comandoEP
		pointTx = &BloqEventPwrOn[comandoEPo_2];// ldw		pointTx,X
		pointInitTx = &BloqEventPwrOn[comandoEPo_2];// ldw		pointInitTx,X
		// ldw		X,#EP_voltInit
		pointEndTx = &BloqEventPwrOn[EP_voltInit];// ldw		pointEndTx,X
		blockSizeTX = 18;// mov		blockSizeTX,#18

		goto tx_wifiEvent;// jp		tx_wifiEvent

// ;										Evento de encendido de compresor
// ;----------------------------------------------------------
comp_event_WF:

		if(flagsEventWF[1]){
			goto ask_CE_end_WF;// btjt	flagsEventWF,#1,ask_CE_end_WF;	/ ya había iniciado evento compresor? Sí, checa si ya se termina evento
		}

ask_CE_start_WF:
		if(GPIOR0[0]){
			goto ask_CE_start_01_WF;	// btjt	GPIOR0,#0,ask_CE_start_01_WF; / compresor encendido ? Sí, captura inicio de evento
		}
		goto desh_event_WF;				//jp		desh_event_WF;							/ No, checa el evento de deshielo
ask_CE_start_01_WF:
		// ldw		X,#$4082
		BloqEventComp[comandoEC_2] = 0x40;// ldw		comandoEC,X
		BloqEventComp[comandoEC_1] = 0x82;
//		BloqEventComp[softVersion1EC] = eePlantilla[eeversion1];// mov		softVersion1EC,eeversion1
//		BloqEventComp[softVersion2EC] = eePlantilla[eeversion2];// mov		softVersion2EC,eeversion2
		BloqEventComp[softVersion1EC] = reePlantilla[eeversion1];
		BloqEventComp[softVersion2EC] = reePlantilla[eeversion2];

		// ldw		X,timeSeconds_HW
		// ldw		EC_timeInit_HW,X
		BloqEventComp[EC_timeInit_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>> 8);
		BloqEventComp[EC_timeInit_3] = (uint8_t) (timeSeconds_HW & 0xFF);
		// ldw		X,timeSeconds_LW
		BloqEventComp[EC_timeInit_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>> 8);// ldw		EC_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventComp[EC_timeInit_1] = (uint8_t) (timeSeconds_LW & 0xFF);

		BloqEventComp[EC_eventType] = 2;// mov		EC_eventType,#2;				/ carga el tipo de evento (2 para compresor)
		// ldw		X,tdevl
		BloqEventComp[EC_tempAmbInit_H] = tdevl; // ldw		EC_tempAmbInit,x;				/ carga temperatura ambiente
		BloqEventComp[EC_tempAmbInit_L] = tdevf;
		BloqEventComp[EC_voltInit] = voltl;// mov		EC_voltInit,voltl; 			/carga voltaje
		flagsEventWF[1] = 1;// bset	flagsEventWF,#1;					/ indica que el evento compresor ya inició
		goto desh_event_WF;	// jp		desh_event_WF;							/ continúa
ask_CE_end_WF:
		if(GPIOR0[0]){
			goto desh_event_WF;// btjt	GPIOR0,#0,desh_event_WF;	 	/ ya se apagó el compresor ?
		}
comp_event_end_WF:
		// ldw		X,timeSeconds_HW
		// ldw		EC_timeEnd_HW,X
		// ldw		X,timeSeconds_LW
		// ldw		EC_timeEnd_LW,X;				/ guarda el tiempo final
		BloqEventComp[EC_timeEnd_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>> 8);
		BloqEventComp[EC_timeEnd_3] = (uint8_t) (timeSeconds_HW & 0xFF);
		BloqEventComp[EC_timeEnd_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>> 8);
		BloqEventComp[EC_timeEnd_1] = (uint8_t) (timeSeconds_LW & 0xFF);

		// ldw		X,teval
		// ldw		EC_tempEvaEnd,x;				/ copia el dato de temperatura evaporador
		BloqEventComp[EC_tempEvaEnd_H] = teval;
		BloqEventComp[EC_tempEvaEnd_L] = tevaf;

		flagsEventWF[1] = 0;		// bres	flagsEventWF,#1;					/ borra inicio de evento compresor

		// ldw		X,#comandoEC
		// ldw		pointTx,X
		// ldw		pointInitTx,X
		// ldw		X,#EC_voltInit
		// ldw		pointEndTx,X
		// mov		blockSizeTX,#18

		pointTx = &BloqEventComp[comandoEC_2];
		pointInitTx = &BloqEventComp[comandoEC_2];
		pointEndTx =  &BloqEventComp[EC_voltInit];
		blockSizeTX = 18;


		goto tx_wifiEvent; //jp		tx_wifiEvent

// ;										Evento de deshielo
// ;----------------------------------------------------------
desh_event_WF:
		if(flagsEventWF[2]){
			goto ask_DhE_end_WF;// btjt	flagsEventWF,#2,ask_DhE_end_WF;	/ ya había iniciado evento deshielo? Sí, checa si ya se termina evento
		}
ask_DhE_start_WF:
		// ld		A,edorefri;					/ Carga el estado del refrigerador
		// cp		A,#3;										/ está en estado de deshielo ?
		if(edorefri != 3){
			goto power_event_WF;//jrne	power_event_WF;						/ No, continúa sin revisar evento deshielo
		}

		// ldw		X,#$4082
		// ldw		comandoED,X
		//mov		softVersion1ED,eeversion1
		// mov		softVersion2ED,eeversion2
//		BloqEventDesh[softVersion1ED] = eePlantilla[eeversion1];
//		BloqEventDesh[softVersion2ED] = eePlantilla[eeversion2];
		BloqEventDesh[softVersion1ED] = reePlantilla[eeversion1];
		BloqEventDesh[softVersion2ED] = reePlantilla[eeversion2];
		// ldw		X,timeSeconds_HW
		// ldw		ED_timeInit_HW,X
		// ldw		X,timeSeconds_LW
		//ldw		ED_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventDesh[ED_timeInit_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>> 8);
		BloqEventDesh[ED_timeInit_3] = (uint8_t) (timeSeconds_HW & 0xFF);
		BloqEventDesh[ED_timeInit_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>> 8);
		BloqEventDesh[ED_timeInit_1] = (uint8_t) (timeSeconds_LW & 0xFF);

		BloqEventDesh[ED_eventType] = 3;// mov		ED_eventType,#3;				/ carga el tipo de evento (3 para deshielo)
		//ldw		X,tdevl
		//ldw		ED_tempAmbInit,x;				/ carga temperatura ambiente
		BloqEventDesh[ED_tempAmbInit_H] = tdevl;
		BloqEventDesh[ED_tempAmbInit_L] = tdevf;

		BloqEventDesh[ED_voltInit] = voltl;// mov		ED_voltInit,voltl; /carga voltaje
		flagsEventWF[2] = 1; // bset	flagsEventWF,#2;					/ indica que el evento deshielo ya inició
		goto power_event_WF; // jp		power_event_WF;						/ continúa
ask_DhE_end_WF:
		/*ld		A,edorefri;					/ Carga el estado del refrigerador
		cp		A,#3;										/ sigue en estado de deshielo ?
		jreq	power_event_WF;						/ Sí, continúa sin terminar evento*/
		if(edorefri==3){
			goto power_event_WF;
		}
desh_event_end_WF:
		/*ldw		X,timeSeconds_HW
		ldw		ED_timeEnd_HW,X
		ldw		X,timeSeconds_LW
		ldw		ED_timeEnd_LW,X;				/ guarda el tiempo final*/
		BloqEventDesh[ED_timeEnd_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>> 8);
		BloqEventDesh[ED_timeEnd_3] = (uint8_t) (timeSeconds_HW & 0xFF);
		BloqEventDesh[ED_timeEnd_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>> 8);
		BloqEventDesh[ED_timeEnd_1] = (uint8_t) (timeSeconds_LW & 0xFF);
		// ldw		X,teval
		// ldw		ED_tempEvaEnd,x;				/ copia el dato de temperatura evaporador
		BloqEventDesh[ED_tempEvaEnd_H] = teval;
		BloqEventDesh[ED_tempEvaEnd_L] = tevaf;


		flagsEventWF[2] = 0;	//	bres	flagsEventWF,#2;					/ borra inicio de evento deshielo

		/*ldw		X,#comandoED
		ldw		pointTx,X
		ldw		pointInitTx,X
		ldw		X,#ED_voltInit
		ldw		pointEndTx,X
		mov		blockSizeTX,#18*/
		pointTx = &BloqEventDesh[comandoED_2];
		pointInitTx = &BloqEventDesh[comandoED_2];
		pointEndTx =  &BloqEventDesh[ED_voltInit];
		blockSizeTX = 18;


		goto tx_wifiEvent;// jp		tx_wifiEvent

// ;										Evento de falla de energía (power-on reset)
// ;----------------------------------------------------------
power_event_WF:
		if(flagsEventWF[3]){
			goto power_event_end_WF;// btjt	flagsEventWF,#3,power_event_end_WF; Ya inició evento de power-on ?
		}

		goto alarm_event_WF;// jra		alarm_event_WF

power_event_end_WF:

power_event_end_01_WF:

		/*ldw		X,#$4082
		ldw		comandoEPo,X
		mov		softVersion1EPo,eeversion1
		mov		softVersion2EPo,eeversion2*/
		BloqEventPwrOn[comandoEPo_2] = 0x40;
		BloqEventPwrOn[comandoEPo_1] = 0x82;

//		BloqEventPwrOn[softVersion1EPo] = eePlantilla[eeversion1];
//		BloqEventPwrOn[softVersion2EPo] = eePlantilla[eeversion2];

		BloqEventPwrOn[softVersion1EPo] = reePlantilla[eeversion1];
		BloqEventPwrOn[softVersion2EPo] = reePlantilla[eeversion2];

		// ldw		X,timeSeconds_HW
		// ldw		EPo_timeInit_HW,X

		// ldw		X,timeSeconds_LW
		// ldw		EPo_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventPwrOn[EPo_timeInit_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8);
		BloqEventPwrOn[EPo_timeInit_3] = (uint8_t) (timeSeconds_HW & 0x00FF);
		BloqEventPwrOn[EPo_timeInit_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8);
		BloqEventPwrOn[EPo_timeInit_1] = (uint8_t) (timeSeconds_LW & 0x00FF);

		BloqEventPwrOn[EPo_eventType] = 4;// mov		EPo_eventType,#4;				/ carga el tipo de evento (3 para falla de energía)
		// ldw		X,tdevl
		// ldw 		EPo_tempAmbInit,x;			/ carga temperatura ambiente
		BloqEventPwrOn[EPo_tempAmbInit_H] = tdevl;
		BloqEventPwrOn[EPo_tempAmbInit_L] = tdevf;
		// mov		EPo_voltInit,voltl; 		/carga voltaje
		BloqEventPwrOn[EPo_voltInit] = voltl;

		// ldw		X,timeSeconds_HW
		// ldw		EPo_timeEnd_HW,X
		// ldw		X,timeSeconds_LW
		// ldw		EPo_timeEnd_LW,X;				/ guarda el tiempo final
		BloqEventPwrOn[EPo_timeEnd_4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8);
		BloqEventPwrOn[EPo_timeEnd_3] = (uint8_t) (timeSeconds_HW & 0x00FF);
		BloqEventPwrOn[EPo_timeEnd_2] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8);
		BloqEventPwrOn[EPo_timeEnd_1] = (uint8_t) (timeSeconds_LW & 0x00FF);


		// ldw		X,teval
		// ldw		EPo_tempEvaEnd,x;				/ copia el dato de temperatura evaporador
		BloqEventPwrOn[EPo_tempEvaEnd_H] = teval;
		BloqEventPwrOn[EPo_tempEvaEnd_L] = tevaf;

		flagsEventWF[3] = 0;// bres	flagsEventWF,#3;					/ borra inicio de evento power-on

		// ldw		X,#comandoEPo
		// ldw		pointTx,X
		// ldw		pointInitTx,X
		// ldw		X,#EPo_voltInit
		// ldw		pointEndTx,X
		// mov		blockSizeTX,#18

		pointTx = &BloqEventPwrOn[comandoEPo_2];
		pointInitTx = &BloqEventPwrOn[comandoEPo_2];
		pointEndTx =  &BloqEventPwrOn[EPo_voltInit];
		blockSizeTX = 18;

		goto tx_wifiEvent;// jp		tx_wifiEvent

// ;										Evento de alarma
// ;----------------------------------------------------------
alarm_event_WF:

// ;no hubo evento tansmitir
		goto tx_wifi_timeData;// jra		tx_wifi_timeData
tx_wifiEvent:

		// ;Carga de datos
		// ldw		X,#$4082
		// ldw		bufferWifiTx,X
		bufferWifiTx[0] = 0x40;
		bufferWifiTx[1] = 0x82;
		// ; manda cantidad de datos de 2 bytes en la Tx
		bufferWifiTx[2] = 3;// mov		bufferWifiTx+2,#3
		// ; carga versión de firmware
		// ldw		X,eeversion1
//		bufferWifiTx[3] = eePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
//		bufferWifiTx[4] = eePlantilla[eeversion2];
		bufferWifiTx[3] = reePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
		bufferWifiTx[4] = reePlantilla[eeversion2];
		// ;Indica direcciones iniciales de datos a copiar y cantidad de datos (X origen, Y destino, wreg tamaño)
		// ;copia los datos al buffer de tx
		// ldw		X,pointTx
		// addw	X,#4
		// ldw		Y,#bufferWifiTx+5
		wreg = 14;// mov		wreg,#14
		copyVector(pointTx+4,&bufferWifiTx[5]);// call	copyVector

		// ;Ajuste del dato de voltaje
		waux = bufferWifiTx[18]; // mov		waux,bufferWifiTx+18
		bufferWifiTx[18] = 0; // clr		bufferWifiTx+18
		bufferWifiTx[19] = waux; //mov		bufferWifiTx+19,waux

		// ldw		X,#bufferWifiTx
		// ldw		pointTx,X
		// ldw		pointInitTx,X
		// ldw		X,#(bufferWifiTx+20)
		// ldw		pointEndTx,X
		// mov		blockSizeTX,#20

		pointTx = &bufferWifiTx[0];
		pointInitTx = &bufferWifiTx[0];
		pointEndTx = &bufferWifiTx[20];
		blockSizeTX = 20;

		flagsTX[2] = 0;// bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		// ldw		chksum_LW,X;					/ limpia registros de checksum
		chksum_32_HW_LW = 0;
		flagsTX[3] = 1;	// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0x55;	// mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;		// clr		codeTX;								/ limpia código de Tx

		goto end_tx_wifi;// jp		end_tx_wifi

//;===========================================================
//;										DATOS
//;===========================================================
tx_wifi_timeData:
		// tnz		keyTx;									/ se está atendiendo alguna transmisión?
		if(keyTx !=0){
			goto end_tx_wifi; // jrne	end_tx_wifi;						/ espera a que terminé
		}

		// ld		A,timeDataWF_H;
		// or		A,timeDataWF_L;					/ Ya es tiempo de tx datos ?
		if(timeDataWF_H == 0){
			goto tx_wifi_timeData_01;// jreq	tx_wifi_timeData_01;		/ sí, continua
		}
		goto end_tx_wifi;// jp		end_tx_wifi;			/ checa si hay que transmitir algún eevento

tx_wifi_timeData_01:
		wreg = Plantilla[loggerTime]; // mov			wreg,loggerTime;			/ Toma el tiempo de loggeo en minutos
		// ldw			Y,#60;								/ Número de segundos por minuto
		// call		mult1x2;							/ Multiplicalos
		 // mov			timeDataWF_L,resull;
		timeDataWF_H = 60 * wreg;//mov			timeDataWF_H,resulh;		/ Carga el Tiempo total en segundos

		// ld		A,GPIOR0
		// and		A,#$03;								/ enmascara bits 0 y 1 (banderas de compresor y deshielo)
		// ld		actuadores_TD,A;			/	carga estado de compresor y deshielo, (al mismo tiempo limpia las otras banderas)
		actuadores_TD[0] = GPIOR0[0];
		actuadores_TD[1] = GPIOR0[1];
		actuadores_TD[2] = 0;
		actuadores_TD[3] = 0;
		actuadores_TD[4] = 0;
		actuadores_TD[5] = 0;
		actuadores_TD[6] = 0;
		actuadores_TD[7] = 0;

		if(!flagsC[0]){
			goto no_doorOPEN_TD;	// btjf	flagsC,#0,no_doorOPEN_TD; 	/ puerta abierta ?
		}
		actuadores_TD[2] = 1;		// bset	actuadores_TD,#2;			/ sí, indica puerta abierta
no_doorOPEN_TD:
		if(!flagsC[1]){
			goto noFlag_ahorro1_TD;// btjf	flagsC,#1,noFlag_ahorro1_TD; / Modo ahorro 1 activo ?
		}
		actuadores_TD[3] = 1;		//bset	actuadores_TD,#3;			/ sí, indicalo
noFlag_ahorro1_TD:
		/*btjf	flagsC,#2,noFlag_ahorro2_TD; / Modo ahorro 1 activo ?
		bset	actuadores_TD,#4;			/ sí, indicalo*/
		if(!flagsC[2]){
			goto noFlag_ahorro2_TD;
		}
		actuadores_TD[4] = 1;
noFlag_ahorro2_TD:
		/*btjf	flagsa,#nocturno,noFlag_nocturno_TD; / Modo nocturno activo ?
		bset	actuadores_TD,#5;			/ sí, indicalo*/
		if(!flagsa[nocturno]){
			goto noFlag_nocturno_TD;
		}
		actuadores_TD[5] = 1;
noFlag_nocturno_TD:
		/*btjf	GPIOR1,#f_fan,noFlag_vent_TD; / ventilador activo ?
		bset	actuadores_TD,#6;			/ sí, indicalo*/
		if(!GPIOR1[f_fan]){
			goto noFlag_vent_TD;
		}
		actuadores_TD[6] = 1;
noFlag_vent_TD:
		// btjf		GPIOR0,#f_lamp,noFlag_Aux_TD; / rele auxiliar activo ?
		// bset	actuadores_TD,#7;			/ sí, indicalo
		if(!GPIOR0[f_lamp]){
			goto noFlag_Aux_TD;
		}
		actuadores_TD[7] = 1;
noFlag_Aux_TD:

		/*mov		alarmas2_TD,trefst2
		bres	alarmas2_TD,#0
		mov		alarmas_TD,trefst;		/ copia el resgistro de alarmas trefst*/
		//alarmas2_TD = 0;
		for(uint8_t k=0; k<8;k++){
			alarmas2_TD[k] = (trefst2[k]);
		}
		alarmas2_TD[0] = 0;//BitClear(alarmas2_TD,0);
		alarmas_TD = 0;
		for(int k=0; k<8;k++){
			alarmas_TD |= (trefst[k] << k);
		}
		//alarmas_TD = trefst;
		//;Carga de datos
		// ldw		X,#$4081
		// ldw		bufferWifiTx,X
		bufferWifiTx[0] = 0x40;
		bufferWifiTx[1] = 0x81;

		// ; manda cantidad de datos de 2 bytes en la Tx
		bufferWifiTx[2] = 7;// mov		bufferWifiTx+2,#7
		//; carga versión de firmware
//		// ldw		X,eeversion1
//		bufferWifiTx[3] = eePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
//		bufferWifiTx[4] = eePlantilla[eeveTx[3] = findLastValue((uint32_t) &eePlantilla[eeversion1]);// ldw		bufferWifiTx+3,X
		bufferWifiTx[3] = reePlantilla[eeversion1];
		bufferWifiTx[4] = reePlantilla[eeversion2];
		//; carga tiempo
		/*ldw		X,timeSeconds_HW
		ldw		bufferWifiTx+5,X
		ldw		X,timeSeconds_LW
		ldw		bufferWifiTx+7,X*/
		bufferWifiTx[5] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8);
		bufferWifiTx[6] = (uint8_t) (timeSeconds_HW & 0xFF);
		bufferWifiTx[7] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8);
		bufferWifiTx[8] = (uint8_t) (timeSeconds_LW & 0xFF);
		//; carga temperatura 1 (ambiente)
		// ldw		X,tdevl
		// ldw		bufferWifiTx+9,X
		bufferWifiTx[9] = tdevl;
		bufferWifiTx[10] = tdevf;

		//; carga temperatura 2 (evaporador)
		/*ldw		X,teval
		ldw		bufferWifiTx+11,X*/
		bufferWifiTx[11] = teval;
		bufferWifiTx[12] = tevaf;
		//; carga voltaje
		// clr		bufferWifiTx+13
		// mov		bufferWifiTx+14,voltl
		bufferWifiTx[13] = 0;
		bufferWifiTx[14] = voltl;
		//;carga estado de actuadores
		// ldw		X,actuadores_TD
		//bufferWifiTx[15] = actuadores_TD;// ldw		bufferWifiTx+15,X
		bufferWifiTx[15]=0;
		for(int k=0; k<8;k++){
			bufferWifiTx[15] |= (uint8_t)(actuadores_TD[k]<<k);
		}
		bufferWifiTx[16] =estados_TD;
		// ;carga estado de alarmas
		// ldw		X,alarmas2_TD
		bufferWifiTx[17]=0;
		for(uint8_t k=0; k<8; k++){
			bufferWifiTx[17] |= (uint8_t) (alarmas2_TD[k]<<k);//bufferWifiTx[17] = alarmas2_TD;// ldw		bufferWifiTx+17,X
		}

		bufferWifiTx[18] = alarmas_TD;
		//;carga sensor vacuna
		// ldw		X,tret_w
		bufferWifiTx[19] =(uint8_t) ((tret_w&0xFF00)>>8);// ldw		bufferWifiTx+19,X
		bufferWifiTx[20] =(uint8_t) (tret_w&0xFF);
		//;carga cuarto sensor
		// ldw		X,tsac_w
		bufferWifiTx[21] = (uint8_t) ((tret_w&0xFF00)>>8);// ldw		bufferWifiTx+21,X
		bufferWifiTx[22] = (uint8_t) (tret_w&0xFF);

		// ldw		X,#bufferWifiTx
		pointTx = &bufferWifiTx[0];// ldw		pointTx,X
		pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
		// ldw		X,#(bufferWifiTx+23)
		pointEndTx = &bufferWifiTx[23];// ldw		pointEndTx,X
		blockSizeTX = 23;// mov		blockSizeTX,#23

		flagsTX[2] = 0; // bres	flagsTX,#2;						/ Indica que no hay que transmitir Header

		// clrw	X
		// ldw		chksum_HW,X
		chksum_32_HW_LW=0;// ldw		chksum_LW,X;					/ limpia registros de checksum
		flagsTX[3] = 1;// bset	flagsTX,#3;						/ indica que ya se ha enviado el checksum
		keyTx = 0x55; // mov		keyTx,#$55;						/ listo para mandar transmisión
		codeTX = 0;// clr		codeTX;								/ limpia código de Tx

		goto end_tx_wifi;// jp		end_tx_wifi


end_tx_wifi:
		goto end_tx_control;//jp		end_tx_control

end_tx_control_b:
		asm ("nop");

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rutina completa Adaptada <<_RGM
void prepTXlogg_2(){

			uint8_t *point_X;
			uint8_t *point_Y;

			//;---- Carga en penúltimo byte del buffer el numero de bytes capturados en el actual buffer
			point_X	=	dirBuffer;		//LDW	X,dirBuffer
			//clr resulh
			//mov	resull,#126
			//addw	X,resulh
			//ld	A,cntByteBlock

			// Se agrega este parche debido a la naturaleza de la memoria
			// CGM 23/04/2025
			if(cntByteBlock == 0){
				point_X[126] = 0xFF;	//ld		(X),A ---------?
			}
			else{
				point_X[126] = cntByteBlock;	//ld		(X),A ---------?
			}

			//point_X[126] = cntByteBlock;		//ld	(X),A

			/*
			 * CGM 16/04/2025
			 *
			 */
			//point_X[126] = 0;
			//point_X[127] = 1;

			//;---- Graba buffer en bloque de flash
			ProgMemCode = 0xAA;//mov		ProgMemCode,#$AA;				/ Indica que se va a grabar bloque de Flash
			//ld   A,cntBlockFlash;				/ Toma el número de bloques grabados en Flash
			//ldw		X,#128;									/ Carga el tamaño de los bloques (128 bytes)
			STM8_16_X = cntBlockFlash *128;//mul		X,A;										/ Multiplicalos
			//addw	X,dirLogger;						/	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
			dirPointer = &dirLogger[STM8_16_X];		//LDW		dirPointer,X

			/*
			 * CGM 16/04/2025
			 * Se realiza una copia de la pagina actual en RAM y si existe un grabado de un bloquq de 128 bytes incompletos, se realizará un borrado de pagina y solo se escribiran  los bloques de 128 bytes que estan completos
			 */
			grabadoLoggerBloquesCompletos(dirPointer, dirBufferPage);

			//LDW		X,dirBuffer;						/ apunta al buffer de datos en RAM
			dataPointer = dirBuffer;					//LDW		dataPointer,X
			GRABA_BLOCK();			//	call	GRABA_BLOCK

			save_timeUNIX();			//	call	save_timeUNIX

			save_cntReg();				//	call	save_cntReg

			STM8_A = cntBlockFlash;		//ld    A,cntBlockFlash;				/ Toma el número de bloques grabados en Flash

			/////  En caso de que no se haya grabado el bloque completo considera un bloque más para el inicio de la Tx
			if( cntByteBlock == 0){//tnz		cntByteBlock
				goto noIncBlock;//jreq	noIncBlock
			}
			STM8_A++;//inc A
noIncBlock:

			//ldw		X,#128;			/ Carga el tamaño de los bloques (128 bytes)
			//mul		X,A;			/ Multiplicalos
			//addw	X,dirLogger;	/	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
			STM8_16_X = 128 * STM8_A;
			point_X = &dirLogger[STM8_16_X];//ldw	 pointInitTx,X;	/ carga puntero inicial para Tx
			pointInitTx = point_X;
			pointTx = point_X;//ldw	pointTx,X

			flagsTX[0] = 1;//bset	flagsTX,#0;


			if(pointTx != loggerStart){//cpw X,loggerStart;					/ es menor al inicio del logger?
				goto loadPointEnd_0;//jrne	loadPointEnd_0
			}

			point_X = loggerEnd;	//ldw X,loggerEnd;						/ el puntero final es el fin del logger
			flagsTX[0] = 0;			//bres	flagsTX,#0;	/ no esperes a pasar por fin de logger durante transmisión

			goto loadPointEnd;		//jra		loadPointEnd
loadPointEnd_0:
			point_X--;				//decw X
loadPointEnd:
			pointEndTx = point_X;//ldw pointEndTx,X;	/ carga puntero final para Tx
			//ret
}

void bloc_fin_comu (uint8_t blockSizeTX_control, uint32_t chksum_32_HW_LW_control, _Bool flagsTX_4 , _Bool flagsTX_3, _Bool flagsTX_2)
{
	blockSizeTX = blockSizeTX_control;
	chksum_32_HW_LW = chksum_32_HW_LW_control;

	flagsTX[4] = flagsTX_4;
	flagsTX[3] = flagsTX_3;
	flagsTX[2] = flagsTX_2;

	keyTx = 0x55;
	codeTX = 0;

}

void bloq_transmi_header (uint8_t V1, uint8_t V2, uint8_t V3, uint8_t V4, uint8_t V5, uint8_t V6, uint8_t V7, uint8_t V8)
{
	Bloque_Header [softVersion1] = V1;		// mov		softVersion1,version1
	Bloque_Header [softVersion2] = V2;//mov softVersion2,version2;	/ Carga versión del firmware

	//ldw	X,#0
	//ldw	bufferSize_HW,X
	Bloque_Header [bufferSize_4] = V3;//ldw	bufferSize_HW,X
	Bloque_Header [bufferSize_3] = V4;

	//ldw	X,#1344
	//ldw	bufferSize_LW,X;	/ carga el número de registros en caso de memoria llena
	Bloque_Header [bufferSize_2] = V5;	// bufferSize_LW
	Bloque_Header [bufferSize_1] = V6;

	Bloque_Header [dataType] = V7; //mov	dataType,#01
	Bloque_Header [dataSize] = V8; //mov	dataSize,#9

}

void buffer_Tx_control_copy(uint8_t Array_V1, uint8_t Array_V2, uint8_t Array_V3, uint8_t Array_V4, uint8_t pointEndtx_Array)
{
	bufferTxControl [Array_V1] = (uint8_t)((chksum_32_HW_LW & 0xFF000000)>> 24);
	bufferTxControl [Array_V2] = (uint8_t)((chksum_32_HW_LW & 0x00FF0000)>> 16);
	bufferTxControl [Array_V3] = (uint8_t)((chksum_32_HW_LW & 0x0000FF00)>>  8);
	bufferTxControl [Array_V4] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);


	pointTx = &bufferTxControl[0];
	pointInitTx = &bufferTxControl[0];
	pointEndTx = &bufferTxControl [pointEndtx_Array];

}


void Bloq_logger_correct (void)
{
	pointTx = &BloqDatalooger[comando1];// ldw		pointTx,X
	pointInitTx = &BloqDatalooger[comando1]; // ldw		pointInitTx,X
	// ldw		X,#comando2
	pointEndTx = &BloqDatalooger[comando2]; // ldw		pointEndTx,X
}
void Bloq_logger_correct_tx (void)
{
	BloqDatalooger[comando1] = 0xF1; // mov		comando1,#$F1
	BloqDatalooger[comando2] = 0x3D; // mov		comando2,#$3D;				/ indica que la grabación fue exitosa
}

void Termination_WIFICOM (uint8_t pointEndTxWIFI)
{
	pointTx = &bufferWifiTx[0];// ldw		pointTx,X
	pointInitTx = &bufferWifiTx[0];// ldw		pointInitTx,X
	// ldw		X,#(bufferWifiTx+15)
	pointEndTx = &bufferWifiTx[pointEndTxWIFI];// ldw		pointEndTx,X
}

void Wifi_Buffer_Trans(uint8_t Pos1, uint8_t Pos2)
{
	bufferWifiTx [0] = 0x40;	// ldw		bufferWifiTx,X
	bufferWifiTx [1] = Pos1;
	//; manda cantidad de datos de 2 bytes en la Tx
	bufferWifiTx[2] = Pos2;// mov		bufferWifiTx+2,#3
	// ; carga versión de firmware
	// ldw		X,eeversion1
//	bufferWifiTx[3] = eePlantilla[eeversion1];	// ldw		bufferWifiTx+3,X
//	bufferWifiTx[4] = eePlantilla[eeversion2];
	bufferWifiTx[3] = reePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
	bufferWifiTx[4] = reePlantilla[eeversion2];

}

void Wifi_Buffer_Trans2(uint8_t Pos2_2)
{
	save_cntReg();		// call	save_cntReg

	save_timeUNIX();	// call	save_timeUNIX

	bufferWifiTx[0] = 0x40;// ldw		bufferWifiTx,X
	bufferWifiTx[1] = Pos2_2;
	// ; carga versión de firmware
	// ldw		X,eeversion1
//	bufferWifiTx[2] = eePlantilla[eeversion1];// ldw		bufferWifiTx+2,X
//	bufferWifiTx[3] = eePlantilla[eeversion2];
	bufferWifiTx[2] = reePlantilla[eeversion1];// ldw		bufferWifiTx+3,X
	bufferWifiTx[3] = reePlantilla[eeversion2];
	// ; carga hora actual
	// ldw		X,timeSeconds_HW
	bufferWifiTx[4] = (uint8_t) ((timeSeconds_HW & 0xFF00)>>8);// ldw		bufferWifiTx+4,X
	bufferWifiTx[5] = (uint8_t) ((timeSeconds_HW & 0x00FF));
			// ldw		X,timeSeconds_LW
	bufferWifiTx[6] = (uint8_t) ((timeSeconds_LW & 0xFF00)>>8);// ldw		bufferWifiTx+6,X
	bufferWifiTx[7] = (uint8_t) ((timeSeconds_LW & 0x00FF));
			// ;carga también datos de geolocalización
			// ldw		X,eeLat1
			// ldw		bufferWifiTx+8,X
			// ldw		X,eeLat3
			// ldw		bufferWifiTx+10,X
			// ldw		X,eeLong1
			//ldw		bufferWifiTx+12,X
			//ldw		X,eeLong3
			//ldw		bufferWifiTx+14,X

//	bufferWifiTx[8] = eeLat1;
//	bufferWifiTx[9] = eeLat2;
//	bufferWifiTx[10] = eeLat3;
//	bufferWifiTx[11] = eeLat4;
//	bufferWifiTx[12] = eeLong1;
//	bufferWifiTx[13] = eeLong2;
//	bufferWifiTx[14] = eeLong3;
//	bufferWifiTx[15] = eeLong4;
	bufferWifiTx[8] = eeLat1;
	bufferWifiTx[9] = eeLat2;
	bufferWifiTx[10] = eeLat3;
	bufferWifiTx[11] = eeLat4;
	bufferWifiTx[12] = eeLong1;
	bufferWifiTx[13] = eeLong2;
	bufferWifiTx[14] = eeLong3;
	bufferWifiTx[15] = eeLong4;

}



void compare_chksum_func(uint8_t Array_RXBuffer1, uint8_t Array_RXBuffer2, uint8_t Array_RXBuffer3, uint8_t Array_RXBuffer4)
{
	chksum_to_compare = 0;
	chksum_to_compare = ((uint32_t)RxBuffer_Ble[Array_RXBuffer1])  <<24;
	chksum_to_compare += ((uint32_t)RxBuffer_Ble[Array_RXBuffer2]) <<16;
	chksum_to_compare += ((uint32_t)RxBuffer_Ble[Array_RXBuffer3]) <<8;
	chksum_to_compare += ((uint32_t)RxBuffer_Ble[Array_RXBuffer4]);
}

void BloqDatalooger_Incorrect(void)
{
	BloqDatalooger[comando1] =	0xF1; // mov		comando1,#$F1
	BloqDatalooger[comando2] = 	0x3E; // mov		comando2,#$3E;				/ indica que hubo un error y no se programaron los datos
}

void borra_pagina_logger(uint32_t pagina)
{
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError = 0;

    while (HAL_FLASH_Unlock() != HAL_OK);

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // Borrado por páginas
    eraseInitStruct.Page = pagina;                   // Página inicial a borrar
    eraseInitStruct.NbPages = 1;

    while (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK);
    while (HAL_FLASH_Lock() != HAL_OK);

    pagina_borrado++;
}

void graba_bloque_fw (void)
{
	uint8_t i = 0;
	uint8_t j = 0;

	uint8_t a = 0;
	uint8_t b = 1;
	uint8_t c = 2;
	uint8_t d = 3;
	uint8_t e = 4;
	uint8_t f = 5;
	uint8_t g = 6;
	uint8_t h = 7;

//	uint32_t buffer_recepcion = 0;

	uint64_t buffer_recepcion = 0;

	uint64_t contenido1 = 0;
	uint64_t contenido2 = 0;
	uint64_t contenido3 = 0;
	uint64_t contenido4 = 0;
	uint64_t contenido5 = 0;
	uint64_t contenido6 = 0;
	uint64_t contenido7 = 0;
	uint64_t contenido8 = 0;

	while( HAL_FLASH_Unlock() !=  HAL_OK );
	for(i = 0; i < 16; i++)
	{
		switch (j)
		{
			case 0:
				contenido1 =  RxBuffer_Ble[a];
				j++;
			case 1:

				contenido2 =  RxBuffer_Ble[b];
				contenido2 = contenido2 << 8;
				j++;

			case 2:
				contenido3 =  RxBuffer_Ble[c];
				contenido3 = contenido3 << 16;
				j++;

			case 3:
				contenido4 =  RxBuffer_Ble[d];
				contenido4 = contenido4 << 24;
				j++;

			case 4:
				contenido5 =  RxBuffer_Ble[e];
				contenido5 = contenido5 << 32;
				j++;

			case 5:
				contenido6 = RxBuffer_Ble[f];
				contenido6 = contenido6 << 40;
				j++;

			case 6:
				contenido7 = RxBuffer_Ble[g];
				contenido7 = contenido7 << 48;
				j++;

			case 7:
				contenido8 = RxBuffer_Ble[h];
				contenido8 = contenido8 << 56;

				buffer_recepcion = contenido1 | contenido2 | contenido3 | contenido4 | contenido5 |contenido6 | contenido7 |contenido8;
				while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, direccion_fw, buffer_recepcion) != HAL_OK);
				//while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion_fw, buffer_recepcion)!=  HAL_OK);
				//HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion_fw, buffer_recepcion);
				direccion_fw = direccion_fw + 8;
				j = 0;
				a = a + 8;
				b = b + 8;
				c = c + 8;
				d = d + 8;
				e = e + 8;
				f = f + 8;
				g = g + 8;
				h = h + 8;
		}


	}
	while(HAL_FLASH_Lock() != HAL_OK );
}

















