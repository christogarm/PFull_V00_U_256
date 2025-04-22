

#include "main.h"
#include "customMain.h"
#include "ELTEC_EmulatedEEPROM.h"

uint8_t BCDtoByte(uint8_t wreg_);

void inicio (void){

	uint8_t *point_X;

	//call	mcuset1;			/ Refresca los valores de los registros IO que no deben cambiar
	//call	mcuset;			/ Refresca los valores de los registros IO que no deben cambiar
	//;call	mcuset1;			/ Refresca los valores de los registros IO que no deben cambiar
	//call   config_RTC      ;RM_20230724 Configura el RTC

	flagsa[7] = 1;				//	mov		flagsa,#$81;	/ Indica que esta en período de arranque
	flagsa[0] = 1;

	cntseg =0;					//	mov		cntseg,#0;

	luminos = 0x81;				//	mov		luminos,wreg;

	cnthitemp = time_ht;		//mov		cnthitemp,#time_ht;;	/ Inicia el tiempo de temperatura alta

inicializa_comu:

		/* 01-AGO-2024
		 * Falta Codigo de inicializacion I2C
		 */


	retzc_ms_deshielo = tms_retcz_deshielo;			// mov     retzc_ms_deshielo,#tms_retcz_deshielo       ;Carga retardo para encendido de deshielo
	retzc_ms_ventilador = tms_retcz_ventilador;		// mov     retzc_ms_ventilador,#tms_retcz_ventilador       ;Carga retardo para encendido de ventilador
	retzc_ms_compresor = tms_retcz_compresor;		// mov     retzc_ms_compresor,#tms_retcz_compresor       ;Carga retardo para encendido de compresor
	retzc_ms_lampara = tms_retcz_lampara;			// mov     retzc_ms_lampara,#tms_retcz_lampara       ;Carga retardo para encendido de lampara
	for(uint8_t k=0;k<5; k++){								// clr     cruze_por_cero
		cruze_por_cero[k] = 0;
	}
	muestras_cal_volt = 0;							// clr     muestras_cal_volt         ;RM_20230908 Variables para mejorar la calibración de voltaje
	voltaje_ant_cal = 0;							// clr     voltaje_ant_cal           ;RM_20230908 Variables para mejorar la calibración de voltaje

														//	ldw		X,#eeprotype				;// manuel_ apuntador para la eeprom
														//	call	rdeeprom
	//Plantilla[protype] = eePlantilla[eeprotype];		//	mov    	protype,waux
	Plantilla[protype] = reePlantilla[eeprotype];
	voltl = 110;
	if(Plantilla[protype] != 0x02){					// Protección de voltaje 220v?
		goto no_ini_210;
	}
	voltl = 210;							//  Carga el promedio de muestras con 210
no_ini_210:

	flagsb[f_luzb]= 1;					//bset		flagsb,#f_luzb
	flagsb[f_prog]= 0;					//bres		flagsb,#f_prog
	flagsb[f_sprm]= 0;					//bres		flagsb,#f_sprm;								/ cancela bandera de programacion
	//;bres		flagsb,#f_sgpo;								/ cancela bandera de programacion

	deb2_btn1 = 150;		//mov			deb2_btn1,#150;#175
	deb2_btn2 = 150;		//mov			deb2_btn2,#150;#175
	deb2_btn3 = 150;		//mov			deb2_btn3,#150;#175
	deb2_btn4 = 150;		//mov			deb2_btn4,#150;#175

	Plantilla[dato_seg1] = 0xAA;		//mov			dato_seg1,#$AA
	Plantilla[dato_seg2] = 0x66;		//mov			dato_seg2,#$66
	Plantilla[dato_seg3] = 0xCC;		//mov			dato_seg3,#$CC

	//cnt_pta_fan = eePlantilla[eetimepaf];			//mov			cnt_pta_fan,eetimepaf
	cnt_pta_fan = reePlantilla[eetimepaf];

	load_tiempoAhorro1();		//call	load_tiempoAhorro1;				/ cada que se abre puerta vuelve a cargar tiempos de ahorro
	load_tiempoAhorro2();		//call	load_tiempoAhorro2;

//	;/ Inicializa los registros de tiempo UNIX

	//timeSeconds_HW = (uint16_t)(eeTimeUnix1 * 256) + (uint16_t)(eeTimeUnix2);		//	ldw		X,eeTimeUnix1
	//timeSeconds_HW = (uint16_t)(findLastValue((uint32_t) &eeTimeUnix1) * 256) + (uint16_t)(findLastValue((uint32_t) &eeTimeUnix2));		//	ldw		X,eeTimeUnix1
																									//	ldw		timeSeconds_HW,X
	//timeSeconds_LW = (uint16_t)(eeTimeUnix3 * 256) + (uint16_t)(eeTimeUnix4);		//	ldw		X,eeTimeUnix3
	//timeSeconds_LW = (uint16_t)(findLastValue((uint32_t) &eeTimeUnix3) * 256) + (uint16_t)(findLastValue((uint32_t) &eeTimeUnix4));		//	ldw		X,eeTimeUnix3
																										//	ldw		timeSeconds_LW,X
	cntLogger_H	= 0;			//	clr		cntLogger_H				;
	//	clr		cntLogger_L				;	contador en segundos para loggear datos
	cntBlockFlash =	0;			//	clr		cntBlockFlash			;	contador de bloques de Flash grabados (con bloques de 128bytes se pueden grabar hasta 32k de memoria)
	cntByteBlock = 0;			//	clr		cntByteBlock			; contador de bytes grabados en buffer de datos
	cntByteLogg = 0;			//	clr		cntByteLogg				; contador de bytes grabados en cada loggeo

	flagsEvent[3] = 1;			// bset 	flagsEvent,#3			; Indica inicio de evento power-on


	//timeTxTWF = 15;			//	mov		timeTxTWF,#15;					/ carga tiempo de envio de token Wifi
							//	ldw		X,#300;
	timeTxTWF = 20;
	timeoutTWF = 300;		//	ldw		timeoutTWF,X
	// carga tiempo de envio de token BLE
	//	mov		timeTxTBLE,#10;
	//	ldw		X,#300;
	//	ldw		timeoutTBLE,X
	timeTxTBLE = 10;
	timeoutTBLE = 300;
//	;---  Obteniendo dirección de la última muestra tomada en el logger de datos


	point_X = &dataLogger[127];			// ; carga inicio de logger de datos
	STM8_A = *point_X;						// ; toma el último byte del primer bloque de Flash
	wreg = STM8_A;						//
	waux = 0;

checkNextDataBlock:
	if(point_X != &dataLoggerFin){	//; se llegó al fin del logger (sin encontrar datos diferentes) ?
		goto checkNextDataBlock_01;		//; No, continúa la busqueda
	}
	waux = 0;							//; si todos son iguales considera que el bloque final fue el último en ser grabado
	goto  lastDataBlockFound;
checkNextDataBlock_01:
	waux++;								//; incrementa contador de bloques revisados
	point_X += 128;
	STM8_A = (*point_X);				//; toma el último byte del siguiente bloque de Flash
	if(STM8_A == wreg){
		goto checkNextDataBlock;		//; sí, continua buscando hasta encontrar diferencia
	}
lastDataBlockFound:
	cntBlockDATA = waux;				//; inicializa contador con el bloque encontrado.

	point_X = (cntBlockDATA * 128) + &dataLogger[0] ;	//;	Carga el tamaño de los bloques (128 bytes) y Multiplicalos
	point_X += 126; 									//; apunta al penúltimo byte de ese bloque

	cntByteBlockDATA = (*point_X);						//; inicializa el contador de bytes grabados en el bloque
	//cntByteBlockDATA = 0;
	dirBuffer = &data_buffer[0];
	dirLogger = &dataLogger[0];
	cntBlockFlash = cntBlockDATA;
	load_next_buffer();						//call	load_next_buffer	; carga buffer de RAM con el bloque de datos


//	;---  Obteniendo dirección de la última muestra tomada en el logger de eventos

	point_X = &eventLogger[127];			// ; carga inicio de logger de eventos
	STM8_A = *point_X;						// ; toma el último byte del primer bloque de Flash
	wreg = STM8_A;					//
	waux = 0;

checkNextEventBlock:
		if(point_X != &eventLoggerFin){	//; se llegó al fin del logger (sin encontrar datos diferentes) ?
			goto checkNextEventBlock_01;		//; No, continúa la busqueda
		}
		waux = 0;							//; si todos son iguales considera que el bloque final fue el último en ser grabado
		goto  lastEventBlockFound;
checkNextEventBlock_01:
		waux++;								//; incrementa contador de bloques revisados
		point_X += 128;
		STM8_A = (*point_X);				//; toma el último byte del siguiente bloque de Flash
		if(STM8_A == wreg){					// ;	son iguales ?
			goto checkNextEventBlock;		//; sí, continua buscando hasta encontrar diferencia
		}
lastEventBlockFound:
		cntBlockEVENT = waux;				//; inicializa contador con el bloque encontrado.

		point_X = (cntBlockEVENT * 128) + &eventLogger[0];	//;	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
		point_X += 126; 									//; apunta al penúltimo byte de ese bloque

		cntByteBlockEVENT = (*point_X);						//; inicializa el contador de bytes grabados en el bloque
		//cntByteBlockEVENT = 0;
		dirBuffer = &event_buffer[0];
		dirLogger = &eventLogger[0];
		cntBlockFlash = cntBlockEVENT;
		load_next_buffer();						//; carga buffer de RAM con el bloque de datos

		flagsC[f_spReached]=0;			// bres	flagsC,#f_spReached

	//;/ carga estado inicial de la lampara
		flagsC[f_lampDoor] = 0;			//	bres		flagsC,#f_lampDoor
		uint8_t estado1_Aux = reeEstado1; // Agrego para no realizar tantas llamadas; CGM 25/02/2025
		if(!GetRegFlagState(estado1_Aux, est1Lamp)){
			goto initLampOFF;
		}
		flagsC[f_lampDoor] = 1;			//	bset		flagsC,#f_lampDoor
initLampOFF:
		if(GetRegFlagState(Plantilla[logicos2], funReleDesh))//btjt		logicos2,#funReleDesh,deshTypeAct_02; omite estado Lock sí se eligio función deshielo para relevador
			goto deshTypeAct_02;
		//;/ carga estado inicial de la cerradura
		GPIOR0[f_dh] = 0;			//	bres		GPIOR0,#f_dh
		if(!GetRegFlagState(estado1_Aux, est1LockDr)){
			goto initLockDrOFF;
		}
		GPIOR0[f_dh] = 1;			//	bset		GPIOR0,#f_dh
initLockDrOFF:
deshTypeAct_02:
//		ldw			X,#90
//		ldw			cntSetName,X
		cntSetName = 90;
	//jp	end_init
}

void timeBCD_to_UNIX(void){
//timeBCD_to_UNIX:
	// ; convierte el año bcd y suma 2000 para obtener el año
	// clrw		X
	// mov			wreg,timeBCD_year
	// call		BCDtoByte
	// ld			A,waux
	// ld			xl,A
	// addw		X,#2000
	time_year = (uint16_t) (2000 + BCDtoByte(timeBCD_year));// ldw			time_year,X

	// ; convierte el mes bcd (elimina los bytes que contiene el día de la semana)
	// ld			A,timeBCD_month
	// and			A,#$1F
	// ld			wreg,A
	// call		BCDtoByte
	// mov			time_month,waux
	time_month = BCDtoByte(timeBCD_month & 0x1F);

	// ; convierte el día bcd
	// mov			wreg,timeBCD_day
	// call		BCDtoByte
	// mov			time_day,waux
	time_day = BCDtoByte(timeBCD_day);


	// ; convierte las horas bcd
	// mov			wreg,timeBCD_hour
	// call		BCDtoByte
	// mov			time_hour,waux
	time_hour = BCDtoByte(timeBCD_hour);

	// ; convierte los minutos bcd
	// mov			wreg,timeBCD_min
	// mov			time_min,waux
	time_min=BCDtoByte(timeBCD_min);

	// ; convierte los segundos bcd
	//mov			wreg,timeBCD_sec
	//call		BCDtoByte
	//mov			time_sec,waux
	time_sec=BCDtoByte(timeBCD_sec);
}


uint8_t BCDtoByte(uint8_t wreg_){
	// clr		waux;								/ limpia el byte salida
	// ld		A,wreg;							/ carga dato BCD
	// and		A,#$0F;							/ deja solo el nibble bajo
	// add		A,waux;							/ suma el byte de salida
	// ld		waux,A;							/ guarda byte de salida
	// ld		A,wreg;							/ carga dato BCD de nuevo
	// swap	A;									/ intercambia nibble
	// and		A,#$0F;							/ deja solo el nibble alto original
	// ldw		X,#10
	// mul		X,A;								/ multiplica por 10
	// ld		A,xl
	// add		A,waux;							/ suma el byte de salida
	return (uint8_t)( (wreg_ & 0x0F)+ ((wreg_ & 0xF0)>>4)*10 );// ld		waux,A;							/ guarda byte de salida

}




































