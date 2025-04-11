

#include "main.h"
#include "customMain.h"
#include "ELTEC_EmulatedEEPROM.h"





void logger (void){

		uint8_t *point_X;
		uint8_t *point_Y;

		//ld A,edorefri;
		//cp A,#2;
		if(edorefri >= 2){
			goto logger_01;//jruge	logger_01;
		}
		goto fin_logger; //jp fin_logger


logger_01:
//;		;------------ Grabado de emergencia por fallo de energía


//;-------------------------------------------------------
		//;*********** solo para pruebas
		//;jra		voltFlagOK
		//;*********** solo para pruebas
		//
		//; pregunta si hay un voltaje correcto al arranque para comenzar a loggear
		if(flagsLogger[7]){ //btjt flagsLogger,#7,voltFlagOK
			goto voltFlagOK;
		}
		//ld A,protype;
		if(Plantilla[protype] == 0x02){//cp A,#$02; Protección de voltaje 220v?
			goto chk_220;//jreq	chk_220
		}
chk_120:
		//ld A,voltl
		if(voltl >= 70){//cp A,#70; / el voltaje es superior al minimo medible ?
			goto voltArranOK;//jruge voltArranOK
		}
		goto voltArranFAIL;//jra	voltArranFAIL
chk_220:
		//ld A,voltl
		if(voltl >= 170){//cp A,#170;	/ el voltaje es superior al minimo medible ?
			goto voltArranOK;//jruge voltArranOK
		}
		goto voltArranFAIL;//jra voltArranFAIL

voltArranOK:
		flagsLogger[7] = 1;//bset	flagsLogger,#7
voltArranFAIL:
        goto fin_logger; //jp fin_logger
voltFlagOK:
//;-----------------------------------------------------------
//; Unicamente trabajo logger cuando no hay conexión al servidor
//		if(!flagsWIFI[f_serverConnect]){//btjf flagsWIFI,#f_serverConnect,serverDown
//	       	goto serverDown;
//	    }
//	    goto fin_logger; //fin_logger
//serverDown:

//;===========================================================
//;					LOGGER DE EVENTOS
//;===========================================================
//				+++++	EVENTO PUERTA	++++++
event_logg:
		if(flagsWIFI[f_eventLoggerSend])	//btjt		flagsWIFI,#f_eventLoggerSend,event_logg_01; No loggear hasta terminar Tx
			goto event_logg_01;
		if(!flagsLogger[4]){//btjf	flagsLogger,#4,door_event; No loggear hasta terminar Tx
			goto door_event;
		}
event_logg_01:
		goto fin_logger; // jp fin_logger

//				   Evento de apertura de puerta
//;----------------------------------------------------------
door_event:
		//Salto para probar logeo de DATOS tiempo
		//+++++++++++++++++++++++++++++++++++++++
		// goto	data_logg;
		//+++++++++++++++++++++++++++++++++++++++
		//Salto para probar logeo de DATOS tiempo

		if(flagsEvent[0]){//btjt	flagsEvent,#0,ask_DE_end;	/ ya había iniciado evento puerta? Sí, checa si ya se termina evento
			goto ask_DE_end;
		}
ask_DE_start:
		if(flagsC[0]){//btjt	flagsC,#0,ask_DE_start_01; / hay puerta abierta ? Sí, captura inicio de evento
			goto ask_DE_start_01;
		}
		goto comp_event;//jp comp_event;/ No, checa el evento de compresor
ask_DE_start_01:
		BloqEventPuerta[EP_timeInit_4] = highByte(timeSeconds_HW);		//ldw	X,timeSeconds_HW
		BloqEventPuerta[EP_timeInit_3] = lowByte(timeSeconds_HW);		//ldw	EP_timeInit_HW,X
		BloqEventPuerta[EP_timeInit_2] = highByte(timeSeconds_LW);		//ldw	X,timeSeconds_LW
		BloqEventPuerta[EP_timeInit_1] = lowByte(timeSeconds_LW);		//ldw	EP_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventPuerta[EP_eventType] = 1;		// mov	EP_eventType,#1;/ carga el tipo de evento (1 para puerta)
		BloqEventPuerta[EP_tempAmbInit_H] = tdevl;				// ldw	X,tdevl
		BloqEventPuerta[EP_tempAmbInit_L] = tdevf;				// ldw	EP_tempAmbInit,x; / carga temperatura ambiente
		BloqEventPuerta[EP_voltInit] = voltl;					//mov	EP_voltInit,voltl; /carga voltaje
		flagsEvent[0] = 1;//bset	flagsEvent,#0;					/ indica quie el evento puerta ya inició

		//ld	A,#0;
		//ld	xh,A
		//ld	A,tmDoorEvent;					/ carga el tiempo de puerta abierta necesario para considerarlo evento
		//ld	xl,A
		//ldw	temp_doorEvent,X;				/ carga tiempo de duración minima de evento puerta abierta
		temp_doorEvent = (0*256) + Plantilla[tmDoorEvent];
 	    goto comp_event;//jp	comp_event;

ask_DE_end:
		if(flagsC[0]){//btjt	flagsC,#0,comp_event;	 	/ ya se cerró la puerta ?
			goto comp_event;
		}
		//ldw X,temp_doorEvent
		if(temp_doorEvent == 0){//tnzw	X; / se cumplió el tiempo minimo de puerta abierta ?
			goto door_event_end;//jreq	door_event_end;	/ sí, ve a finalizar el evento
		}
		flagsEvent[0] = 0;//bres	flagsEvent,#0;	/ borra inicio de evento puerta
		goto comp_event;//jra comp_event; / continúa sin grabar evento
door_event_end:
		BloqEventPuerta[EP_timeEnd_4] = highByte(timeSeconds_HW);			//ldw	X,timeSeconds_HW
		BloqEventPuerta[EP_timeEnd_3] = lowByte(timeSeconds_HW);			//ldw	EP_timeEnd_HW,X
		BloqEventPuerta[EP_timeEnd_2] = highByte(timeSeconds_LW);			//ldw		X,timeSeconds_LW
		BloqEventPuerta[EP_timeEnd_1] = lowByte(timeSeconds_LW);			//ldw		EP_timeEnd_LW,X;				/ guarda el tiempo final
		BloqEventPuerta[EP_tempEvaEnd_H] = teval;				//ldw	X,teval
		BloqEventPuerta[EP_tempEvaEnd_L] = tevaf;				//ldw	EP_tempEvaEnd,x;	/ copia el dato de temperatura evaporador

//		;prueba
//									;ld		A,cntLogger_H;
//									;or		A,cntLogger_L;					/ Ya es tiempo de tomar datos?
//									;jrne	comp_event;							/ sí, continua
//		;prueba


		dirDataLoad = &BloqEventPuerta[EP_timeInit_4];	//ldw	X,#EP_timeInit_HW
		//ldw dirDataLoad,X;	/ indica el inicio del bloque de datos a cargar (evento puerta)
		load_event();			//call	load_event

		flagsEvent[0] = 0;		//bres flagsEvent,#0;	/ borra inicio de evento puerta
		goto comp_event;		//jra	comp_event;			/ continúa

//;										Evento de encendido de compresor
//;----------------------------------------------------------
comp_event:
		if(flagsEvent[1]){//  btjt flagsEvent,#1,ask_CE_end;	/ ya había iniciado evento compresor? Sí, checa si ya se termina evento
			goto ask_CE_end;
		}
ask_CE_start:
		if(GPIOR0[0]){//btjt GPIOR0,#0,ask_CE_start_01; / compresor encendido ? Sí, captura inicio de evento
			goto ask_CE_start_01;
		}
		goto desh_event;//jp desh_event;			/ No, checa el evento de deshielo
ask_CE_start_01:
		BloqEventComp[EC_timeInit_4] = highByte(timeSeconds_HW);		//ldw		X,timeSeconds_HW
		BloqEventComp[EC_timeInit_3] = lowByte(timeSeconds_HW);		//ldw		EC_timeInit_HW,X
		BloqEventComp[EC_timeInit_2] = highByte(timeSeconds_LW);		//ldw		X,timeSeconds_LW
		BloqEventComp[EC_timeInit_1] = lowByte(timeSeconds_LW);		//ldw		EC_timeInit_LW,X;	/ guarda el tiempo de inicio
		BloqEventComp[EC_eventType] = 2;				//mov		EC_eventType,#2;	/ carga el tipo de evento (2 para compresor)
		BloqEventComp[EC_tempAmbInit_H] = tdevl;		//ldw		X,tdevl
		BloqEventComp[EC_tempAmbInit_L] = tdevf;		//ldw		EC_tempAmbInit,x;				/ carga temperatura ambiente
		BloqEventComp[EC_voltInit] = voltl;	//mov EC_voltInit,voltl; /carga voltaje
        flagsEvent[1] = 1;						//bset	flagsEvent,#1;	/ indica que el evento compresor ya inició
		goto desh_event;						//jp desh_event;	/ continúa
ask_CE_end:
		if(GPIOR0[0]){//btjt	GPIOR0,#0,desh_event;	 	/ ya se apagó el compresor ?
			goto desh_event;
		}
comp_event_end:
		BloqEventComp[EC_timeEnd_4] = highByte(timeSeconds_HW);			//ldw		X,timeSeconds_HW
		BloqEventComp[EC_timeEnd_3] = lowByte(timeSeconds_HW);			//ldw		EC_timeEnd_HW,X
		BloqEventComp[EC_timeEnd_2] = highByte(timeSeconds_LW);			//ldw		X,timeSeconds_LW
		BloqEventComp[EC_timeEnd_1] = lowByte(timeSeconds_LW);			//ldw		EC_timeEnd_LW,X;/ guarda el tiempo final
		BloqEventComp[EC_tempEvaEnd_H] = teval;							//ldw		X,teval
		BloqEventComp[EC_tempEvaEnd_L] = tevaf;							//ldw		EC_tempEvaEnd,x;				/ copia el dato de temperatura evaporador

		dirDataLoad = &BloqEventComp[EC_timeInit_4];					//ldw X,#EC_timeInit_HW
		//ldw	dirDataLoad,X;/ indica el inicio del bloque de datos a cargar (evento compresor)
		load_event();			//call	load_event

		flagsEvent[1] = 0;//bres flagsEvent,#1;	/ borra inicio de evento compresor
		goto desh_event;//jra	desh_event;		/ continúa

//;				Evento de deshielo
//;----------------------------------------------------------
desh_event:
		if(flagsEvent[2]){//  btjt	flagsEvent,#2,ask_DhE_end;	/ ya había iniciado evento deshielo? Sí, checa si ya se termina evento
			goto ask_DhE_end;
		}
ask_DhE_start:
		//ld A,edorefri;	/ Carga el estado del refrigerador
		if(edorefri != 3){//cp	A,#3;/ está en estado de deshielo ?
			goto power_event;//jrne	power_event;/ No, continúa sin revisar evento deshielo
		}
		BloqEventDesh[ED_timeInit_4] = highByte(timeSeconds_HW);	//ldw		X,timeSeconds_HW
		BloqEventDesh[ED_timeInit_3] = lowByte(timeSeconds_HW);		//ldw		ED_timeInit_HW,X
		BloqEventDesh[ED_timeInit_2] = highByte(timeSeconds_LW);	//ldw		X,timeSeconds_LW
		BloqEventDesh[ED_timeInit_1] = lowByte(timeSeconds_LW);		//ldw		ED_timeInit_LW,X;	/ guarda el tiempo de inicio
		BloqEventDesh[ED_eventType] = 3;//mov	ED_eventType,#3;	/ carga el tipo de evento (3 para deshielo)
		BloqEventDesh[ED_tempAmbInit_H] = tdevl;		//ldw	X,tdevl
		BloqEventDesh[ED_tempAmbInit_L] = tdevf;		 //ldw	ED_tempAmbInit,x;	/ carga temperatura ambiente
		BloqEventDesh[ED_voltInit] = voltl;//mov	ED_voltInit,voltl; /carga voltaje
        flagsEvent[2] = 1;//bset flagsEvent,#2;	/ indica que el evento deshielo ya inició
        goto power_event;//jp power_event;	/ continúa
ask_DhE_end:
		//ld A,edorefri;	/ Carga el estado del refrigerador
		if(edorefri == 3){//cp A,#3; / sigue en estado de deshielo ?
			goto power_event;//jreq	power_event	/ Sí, continúa sin terminar evento
		}
desh_event_end:
		BloqEventDesh[ED_timeEnd_4] = highByte(timeSeconds_HW);	//ldw		X,timeSeconds_HW
		BloqEventDesh[ED_timeEnd_3] = lowByte(timeSeconds_HW);	//ldw		ED_timeEnd_HW,X
		BloqEventDesh[ED_timeEnd_2] = highByte(timeSeconds_LW);	//ldw		X,timeSeconds_LW
		BloqEventDesh[ED_timeEnd_1] = lowByte(timeSeconds_LW);	//ldw		ED_timeEnd_LW,X;	/ guarda el tiempo final
		BloqEventDesh[ED_tempEvaEnd_H] = teval;		//ldw		X,teval
		BloqEventDesh[ED_tempEvaEnd_L] = tevaf;		//ldw		ED_tempEvaEnd,x;	/ copia el dato de temperatura evaporador

		dirDataLoad = &BloqEventDesh[ED_timeInit_4];//ldw	X,#ED_timeInit_HW
		//ldw dirDataLoad,X;		/ indica el inicio del bloque de datos a cargar (evento deshielo)
		load_event();			//call	load_event

		flagsEvent[2] = 0;//bres flagsEvent,#2;		/ borra inicio de evento deshielo
		goto power_event;//jra	power_event;	/ continúa

//;		Evento de falla de energía (power-on reset)
//;----------------------------------------------------------
power_event:
		if(retPowerOn != 0 ){//	tnz		retPowerOn
			//goto alarm_event;//	jrne	alarm_event
			goto wifi_event;
		}
		if(flagsEvent[3]){//	btjt flagsEvent,#3,power_event_end; Ya inició evento de power-on ?
			goto power_event_end;
		}
		//goto alarm_event;//	jra		alarm_event
		goto wifi_event;
power_event_end:
//		BloqEventPwrOn[EPo_timeInit_4] = eeTimeUnix1;		//ldw	X,eeTimeUnix1
//		BloqEventPwrOn[EPo_timeInit_3] = eeTimeUnix2;		//ldw	EPo_timeInit_HW,X
//		BloqEventPwrOn[EPo_timeInit_2] = eeTimeUnix3;		//ldw	X,eeTimeUnix3
//		BloqEventPwrOn[EPo_timeInit_1] = eeTimeUnix4;		//ldw	EPo_timeInit_LW,X;	/ guarda el tiempo de inicio

		BloqEventPwrOn[EPo_timeInit_4] = reeTimeUnix1;		//ldw	X,eeTimeUnix1
		BloqEventPwrOn[EPo_timeInit_3] = reeTimeUnix2;		//ldw	EPo_timeInit_HW,X
		BloqEventPwrOn[EPo_timeInit_2] = reeTimeUnix3;		//ldw	X,eeTimeUnix3
		BloqEventPwrOn[EPo_timeInit_1] = reeTimeUnix4;		//ldw	EPo_timeInit_LW,X;	/ guarda el tiempo de inicio

		BloqEventPwrOn[EPo_eventType] = 4;//mov	EPo_eventType,#4;	/ carga el tipo de evento (3 para falla de energía)
		BloqEventPwrOn[EPo_tempAmbInit_H] = tdevl;	//ldw		X,tdevl
		BloqEventPwrOn[EPo_tempAmbInit_L] = tdevf;	//ldw	EPo_tempAmbInit,x;	/ carga temperatura ambiente
		BloqEventPwrOn[EPo_voltInit] = voltl;//mov	EPo_voltInit,voltl; /carga voltaje


		BloqEventPwrOn[EPo_timeEnd_4] = highByte(timeSeconds_HW);	//ldw		X,timeSeconds_HW
		BloqEventPwrOn[EPo_timeEnd_3] = lowByte(timeSeconds_HW);	//ldw		EPo_timeEnd_HW,X
		BloqEventPwrOn[EPo_timeEnd_2] = highByte(timeSeconds_LW);	//ldw		X,timeSeconds_LW
		BloqEventPwrOn[EPo_timeEnd_1] = lowByte(timeSeconds_LW);	//ldw		EPo_timeEnd_LW,X;				/ guarda el tiempo final
		BloqEventPwrOn[EPo_tempEvaEnd_H] = teval;		//ldw X,teval
		BloqEventPwrOn[EPo_tempEvaEnd_L] = tevaf;		//ldw		EPo_tempEvaEnd,x;				/ copia el dato de temperatura evaporador


		dirDataLoad = &BloqEventPwrOn[EPo_timeInit_4];	//ldw X,#EPo_timeInit_HW
		//ldw	dirDataLoad,X;		/ indica el inicio del bloque de datos a cargar (evento deshielo)
		load_event();				//call	load_event

		flagsEvent[3] = 0;//bres flagsEvent,#3;	/ borra inicio de evento power-on
		goto wifi_event;		//jra		wifi_event;

//;----------------------------------------------------------
//;										Evento de wifi
//;----------------------------------------------------------
wifi_event:
		if(flagsEvent[4])		// btjt	flagsEvent,#4,ask_wfE_end;	/ ya había iniciado evento wifi? Sí, checa si ya se termina evento
			goto ask_wfE_end;
ask_wfE_start:
		if(!flagsTxControl[f_statWIFI])// btjf	flagsTxControl,#f_statWIFI,wifi_event_start; Hubo desconexión con servidor Wifi ? Si, inicia evento falla wifi
			goto wifi_event_start;
		goto alarm_event;			// jra		alarm_event;						/ No, continúa sin revisar evento wifi

wifi_event_start:
		//ldw		X,timeSeconds_HW
		BloqEventWiFiEx[WF_timeInit_4] = highByte(timeSeconds_HW);	//ldw		WF_timeInit_HW,X
		BloqEventWiFiEx[WF_timeInit_3] = lowByte(timeSeconds_HW);
		//ldw		X,timeSeconds_LW
		BloqEventWiFiEx[WF_timeInit_2] = highByte(timeSeconds_LW);//ldw		WF_timeInit_LW,X;				/ guarda el tiempo de inicio
		BloqEventWiFiEx[WF_timeInit_1] = lowByte(timeSeconds_LW);

		BloqEventWiFiEx[WF_eventType] = 5;//mov		WF_eventType,#5;				/ carga el tipo de evento (5 para wifi)
		//ldw		X,tdevl
		BloqEventWiFiEx[WF_tempAmbInit_H] = tdevl;		//ldw		WF_tempAmbInit,x;				/ carga temperatura ambiente
		BloqEventWiFiEx[WF_tempAmbInit_L] = tdevf;
		BloqEventWiFiEx[WF_voltInit] = voltl;		//mov		WF_voltInit,voltl; /carga voltaje
		flagsEvent[4] = 1;			//bset	flagsEvent,#4;					/ indica que el evento wifi ya inició
		goto alarm_event;			//jp		alarm_event;						/ continúa
ask_wfE_end:
		if(flagsTxControl[f_statWIFI])	//btjt	flagsTxControl,#f_statWIFI,wifi_event_end; Volvió la conexión wifi ? Sí, termina evento falla de wifi
			goto wifi_event_end;
		goto alarm_event;				//jra	alarm_event;						/ Sí, continúa sin terminar evento
wifi_event_end:
		//ldw		X,timeSeconds_HW
		BloqEventWiFiEx[WF_timeEnd_4] = highByte(timeSeconds_HW);	//ldw		WF_timeEnd_HW,X
		BloqEventWiFiEx[WF_timeEnd_3] = lowByte(timeSeconds_HW);
		//ldw		X,timeSeconds_LW
		BloqEventWiFiEx[WF_timeEnd_2] = highByte(timeSeconds_LW);//ldw		WF_timeEnd_LW,X;				/ guarda el tiempo final
		BloqEventWiFiEx[WF_timeEnd_1] = lowByte(timeSeconds_LW);
		//ldw		X,teval
		BloqEventWiFiEx[WF_tempEvaEnd_H] = teval;//ldw		WF_tempEvaEnd,x;				/ copia el dato de temperatura evaporador
		BloqEventWiFiEx[WF_tempEvaEnd_L] = tevaf;
		//ldw		X,#WF_timeInit_HW
		dirDataLoad = &BloqEventWiFiEx[WF_timeInit_4];//ldw		dirDataLoad,X;					/ indica el inicio del bloque de datos a cargar (evento wifi)
		load_event();//call	load_event

		flagsEvent[4] = 0;//bres	flagsEvent,#4;

		goto alarm_event;//jra alarm_event;	/continúa


//;		Evento de alarma
//;----------------------------------------------------------
alarm_event:
		goto data_logg; //jp data_logg

//;===========================================================
//;										LOGGER DE DATOS
//;===========================================================
data_logg:
		if(flagsWIFI[f_timeLoggerSend])//btjt		flagsWIFI,#f_timeLoggerSend,jmp_fin_logger; No loggear hasta terminar Tx
			goto jmp_fin_logger;
		if(flagsLogger[5]){//btjt	flagsLogger,#5,jmp_fin_logger; No loggear hasta terminar Tx
			goto jmp_fin_logger;
		}
		//ld A,cntLogger_H;
		//or A,cntLogger_L;		/ Ya es tiempo de tomar datos?
		if(cntLogger_H  == 0){
			goto logger_02; //jreq	logger_02;	/ sí, continua
		}
jmp_fin_logger:
        goto fin_logger; //jp fin_logger

logger_02:

		//mov wreg,loggerTime;	/ Toma el tiempo de loggeo en minutos
		//ldw Y,#60;	/ Número de segundos por minuto
		//call	mult1x2;	/ Multiplicalos
		//mov cntLogger_L,resull;
        cntLogger_H = Plantilla[loggerTime] * 60;//mov	cntLogger_H,resulh;		/ Carga el Tiempo total en segundos

        //DEBUG***********DEBUG---------DEBUG
        //cntLogger_H = 2;  // valor de prueba a cada 10 segundos tomar muestras
        //DEBUG***********DEBUG---------DEBUG


        BloqDatalooger[timeLogg_4] = highByte(timeSeconds_HW);
        BloqDatalooger[timeLogg_3] = lowByte(timeSeconds_HW);
        BloqDatalooger[timeLogg_2] = highByte(timeSeconds_LW);
        BloqDatalooger[timeLogg_1] = lowByte(timeSeconds_LW);

        //;---- Prepara datos a loggear
        BloqDatalooger[tempAmbLogg_H] = tdevl;	//ldw	X,tdevl
        BloqDatalooger[tempAmbLogg_L] = tdevf;	//ldw	tempAmbLogg,x;/ copia el dato de temperatura ambiente
        BloqDatalooger[tempEvaLogg_H] = teval;//ldw	X,teval
        BloqDatalooger[tempEvaLogg_L] = tevaf;//ldw	tempEvaLogg,x;/ copia el dato de temperatura evaporador
        BloqDatalooger[voltLogg] = voltl;		//mov	voltLogg,voltl;	/ toma el volatje de la función de medición rms



         //;---- Pasa los datos necesarios a la subrutina
         //ldw		X,#data_buffer
         dirBuffer = &data_buffer[0];	//ldw		dirBuffer,X
         //ldw	X,#timeSeconds_HW
         dirDataLoad = &BloqDatalooger[timeLogg_4];//ldw	dirDataLoad,X
         //ldw	X,#dataLogger
         dirLogger = &dataLogger[0];//ldw	dirLogger,X

         numBlock = 96;//mov numBlock,#96
         numByte = 9;//mov	numByte,#9
         cntBlockFlash = cntBlockDATA;//mov	cntBlockFlash,cntBlockDATA
         cntByteBlock = cntByteBlockDATA;//mov	cntByteBlock,cntByteBlockDATA

         //ld A,numByte;		/ toma el número de bytes por evento/tiempo loggeado
         //ldw X,#128
         //div X,A; / obtén el número de capturas enteras por bloque de flash
         //ld A,numByte;
         //mul X,A;	/	obten el numero de bytes a almacenar por bloque de flash
         //ld A,xl
         //ld wreg,A / wreg pasa como un dato más para la siguiente subrutina
         STM8_A = (128 / numByte);		 // obtén el número de capturas enteras por bloque de flash
         wreg = STM8_A * numByte;

         //ldw		X,cntRegDATA
         if(cntRegDATA>0)
        	 cntRegDATA++;	//incw	X

		//;Se agrega un limite para el contador de registros de datos (1120 registros en 10k de memoria ) (14 registros por cada 128 bytes)
		//cpw		X,#1343
		if(cntRegDATA<1343)//jrult	no_clrREG_01
			goto no_clrREG_01;
		cntRegDATA = 1343;// ldw		X,#1343
no_clrREG_01:


         //ldw cntRegDATA,X
         cntReg = cntRegDATA;//ldw cntReg,X
		 //ldw X,#eeCntRegDATA
 		 cntRegPNT = &eeCntRegDATA;	//ldw cntRegPNT,X
 		 reeCntRegDATA = cntReg;

 		 load_buffer_logger();		//call	load_buffer_logger

         //;---- Recupera datos de la subrutina
         cntBlockDATA = cntBlockFlash;//mov	cntBlockDATA,cntBlockFlash
         cntByteBlockDATA = cntByteBlock;//mov	cntByteBlockDATA,cntByteBlock
         if(!flagsLogger[0]){//btjf	flagsLogger,#0,noFullLoggerDATA; Se reportó que se alcanzó el limite de logger ? (máximo número de bloques)
        	 	 goto noFullLoggerDATA;
         }
		flagsLogger[0] = 0;//bres	flagsLogger,#0
		flagsLogger[1] = 1;//bset	flagsLogger,#1;					/ Indica que se llenó el logger de dato al menos una vez
noFullLoggerDATA:

		goto fin_logger; //jp fin_logger;


fin_logger:

}
void load_event(){
				//;---- Pasa los datos necesarios a la subrutina

		//ldw		X,#event_buffer
		dirBuffer = &event_buffer[0];//ldw		dirBuffer,X;/ indica la dirección del buffer a cargar
		//ldw		X,#eventLogger
		dirLogger = &eventLogger[0]; //ldw		dirLogger,X;	/ indica la dirección del logger a grabar
		numBlock = 160;//mov numBlock,#160;	/ número máximo de bloque a guardar del logger de eventos (96 bloques de  128 bytes = 12kb )
		numByte = 14;//mov	numByte,#14;	/ número de bytes a cargar para logger de eventos = 14
		cntBlockFlash = cntBlockEVENT;//mov	cntBlockFlash,cntBlockEVENT; / pasa el número de bloques de evento grabados al momento
		cntByteBlock = cntByteBlockEVENT;//mov	cntByteBlock,cntByteBlockEVENT; / pasa el número de bytes cargados al buffer de eventos al momento

		//ld A,numByte; / toma el número de bytes por evento/tiempo loggeado
		//ldw X,#128
		//div X,A;	/ obtén el número de capturas enteras por bloque de flash
		//ld A,numByte;	/
		//mul X,A;	/	obten el numero de bytes a almacenar por bloque de flash
		//ld A,xl
		//ld wreg,A; / wreg pasa como un dato más para la siguiente subrutina
		STM8_A = (128/numByte);
		wreg = STM8_A * numByte;								/// wreg pasa como un dato más para la siguiente subrutina


		//ldw	X,cntRegEVENT
		cntRegEVENT++;				//incw	X

//		;Se agrega un limite para el contador de registros de datos (1120 registros en 10k de memoria ) (14 registros por cada 128 bytes)
//		cpw		X,#1439
//		jrult	no_clrREG_02
//		;clrw	X
//		ldw		X,#1439
		if(cntRegEVENT<1439)
			goto no_clrREG_02;
		cntRegEVENT = 1439;
no_clrREG_02:

		//ldw	cntRegEVENT,X
		cntReg = cntRegEVENT;			//ldw	cntReg,X
		//ldw	X,#eeCntRegEVENT
		cntRegPNT = &eeCntRegEVENT;		//ldw	cntRegPNT,X
		reeCntRegEVENT = cntReg;

		load_buffer_logger();		//call	load_buffer_logger

		//;---- Recupera datos de la subrutina
		cntBlockEVENT = cntBlockFlash;//mov	cntBlockEVENT,cntBlockFlash
		cntByteBlockEVENT = cntByteBlock;//mov	cntByteBlockEVENT,cntByteBlock;	guarda los cambios realizados por la rutina
		if(!flagsLogger[0]){//btjf	flagsLogger,#0,noFullLoggerEVENT;	Se reportó que se alcanzó el limite de logger ? (máximo número de bloques)
			goto noFullLoggerEVENT;
		}
		flagsLogger [0] = 0;//bres	flagsLogger,#0
		flagsLogger[2] = 1;//bset	flagsLogger,#2;		/ Indica que se llenó el logger de eventos al menos una vez
noFullLoggerEVENT:
                 //return; //ret
}

void load_buffer_logger(){

		uint8_t *point_X;
		uint8_t *point_Y;
		uint8_t *tempo2;

		//;---- Carga datos a buffer de datos en RAM
		point_Y = dirDataLoad;//LDW	Y,dirDataLoad;		/ Apunta a los datos a loggear
		point_X = dirBuffer;//LDW	X,dirBuffer;	/ Apunta al buffer de datos en RAM
		//clr	resulh
		//mov	resull,cntByteBlock
		//addw	X,resulh;	/	suma al apuntador el número de bytes que han sido grabados
		point_X = &point_X[cntByteBlock];
		cntByteLogg = 0;//clr cntByteLogg;	/ limpia contrador de datos cargados
load_buffer:
		//LD A,(Y) ;	/	Copia los datos al buffer en RAM
		//LD (X),A;
        *point_X = *point_Y;
        point_X++;//INCW X;
        point_Y++;//INCW Y; / Incrementa apuntadores
        cntByteLogg++;//INC cntByteLogg; /	Incrementa el numero de bytes cargados
		cntByteBlock++;//INC	cntByteBlock; /	Incrementa el numero de bytes cargados al buffer

		//ld A,cntByteBlock; / Ya se llenó el buffer ?
		if(cntByteBlock < wreg){//cp A,wreg
				goto load_buffer_01;//jrult	load_buffer_01;	/ No, continua con la carga

		}
		//;---- Graba buffer en bloque de flash

									//LDW	X,dirBuffer; / Apunta al buffer de datos en RAM
		point_X = &dirBuffer[127];	//addw	X,#127;	/ Apunta al último byte del buffer
		STM8_A = *point_X;			//ld A,(X)
		STM8_A++;   //inc	A;	/ incrementa el byte
		*point_X = STM8_A;	 //ld	(X),A;	/ devuelve el dato incrementado al buffer
		//;---- Borra penúltimo byte de buffer
		point_X--;//decw	X
		*point_X = 0;//clr	(X)

		ProgMemCode = 0xAA;//mov	ProgMemCode,#$AA;	/ Indica que se va a grabar bloque de Flash
		//ld   A,cntBlockFlash;/ Toma el número de bloques grabados en Flash
		//ldw  X,#128;	/ Carga el tamaño de los bloques (128 bytes)
		STM8_16_X= 	128 * cntBlockFlash;//mul X,A; / Multiplicalos
		 //addw X,dirLogger;/	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
		dirPointer = &dirLogger[STM8_16_X];//LDW	dirPointer,X
		//LDW	X,dirBuffer;	/ apunta al buffer de datos en RAM
		//LDW	dataPointer,X
		dataPointer = dirBuffer;
		tempo2 = point_Y; // Y Aqui sigue EXISTIENDO, no necesita respaldarse   ------ //LDW	tempo2,Y;	/ almacena temporalmente el apuntador de RAM

		GRABA_BLOCK();		//call	GRABA_BLOCK

		save_timeUNIX();	//call	save_timeUNIX

		save_cntReg();		//call	save_cntReg

		cntBlockFlash++;//inc cntBlockFlash
		//ld A,cntBlockFlash; / Ya se acabo la Flash reservada para datos ?
		if(cntBlockFlash < numBlock){//cp A,numBlock
			goto load_buffer_00;//jrult	load_buffer_00;	/ No, continua con la carga
		}
		cntBlockFlash = 0;//clr	cntBlockFlash;	/ limpia el contador de bloques de Flash grabados

		flagsLogger[0] = 1;//bset	flagsLogger,#0;					/ indica que se llegó al limite del logger
load_buffer_00:
		load_next_buffer();		//call	load_next_buffer; / Después de grabar bloques de Flash, craga buffer de RAM con el siguiente bloque de datos
		point_Y = tempo2;//LDW	Y,tempo2; / recupera el apuntador de RAM
		point_X = dirBuffer;//LDW	X,dirBuffer; / Reinicia el apuntador del buffer de datos en RAM
		cntByteBlock = 0;//clr	cntByteBlock; /	limpia contador de bytes cargados al buffer

load_buffer_01:
		//ld A,cntByteLogg;	/ Ya se cargaron los datos indicados?
		if(cntByteLogg < numByte){//cp A,numByte;
			goto load_buffer;//jrult load_buffer;/ No, continua cargadno
		}
					//ret
		asm ("nop");

}














