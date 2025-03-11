

#include "main.h"
#include "customMain.h"

void watch (void){


			memodriver();		//	call		memodriver	;		/ Refresca los parámetros de operación del refrigerador uno a la vez cada ms
//;------------------------------------------------------------------------------------------------------------------
//;							supervicion de timeout token wifi - ble
//;------------------------------------------------------------------------------------------------------------------
			//ldw		X,timeoutTBLE;					/ No,checa time out
			//cpw		X,#1;
			if(timeoutTBLE == 1)//jreq	resetComMod;					/
				goto resetComMod;
			//ldw		X,timeoutTWF;					/ No,checa time out
			//cpw		X,#1;
			if(timeoutTWF != 1)//jrne	fallas_temp;					/ No, continúa
				goto fallas_temp;
resetComMod:
			timeTxTBLE = 20;	//mov		timeTxTBLE,#20
			flagsTX2[2] =0;		//bres	flagsTX2,#2;					/ sí, cancela bandera de token recibido
			//ldw		X,#300;
			timeoutTBLE = 300;	//ldw		timeoutTBLE,X;					/ carga time out de Token BLE (5 min)

			timeTxTWF = 30;			//mov		timeTxTWF,#30
			flagsTX2 [1] = 0; 		//bres	flagsTX2,#1;					/ sí, cancela bandera de token recibido
			timeRstBLE = 5;			//mov		timeRstBLE,#5;				/ carga 5 segundos de apagado para BLE (reinicia el modulo wifi)
			//ldw		X,#300;
			timeoutTWF = 300;		//ldw		timeoutTWF,X;					/ carga time out de Token (5 min)
			keyTx = 0;				//clr		keyTx  ;// en caso de desconexion cancela toda transmisión
			for(uint8_t i=0; i<8; i++){
				flagsTX[i] = 0;		//clr		flagsTX
				flagsRxFirm[i]= 0;	//clr		flagsRxFirm
				flagsTxControl[i] = 0; // clr		flagsTxControl
				flagsWIFI[i] = 0; 		//clr		flagsWIFI
			}

			flagsLogger[4] = 0;			//bres	flagsLogger,#4;				// permite loggeo de eventos
			flagsLogger[5] = 0;			//bres	flagsLogger,#5;				// permite loggeo de datos


//;------------------------------------------------------------------------------------------------------------------
fallas_temp:

////;------------------------- Control del Activacion de Cargas
watch50:

				portX[rel_co] = 0;			//		bres		puertoa,#rel_co; /Apaga los relevadores    ;RM_20220623  Cambio de puertos para cargas
				portX[rel_dh] = 0;			//		bres		puertoe,#rel_dh;                           ;RM_20220623  Cambio de puertos para cargas
				portX[rel_lz] = 0;			//		bres		puertoe,#rel_lz;		// Manuel 10-MAR-2022  ;RM_20220623  Cambio de puertos para cargas
				portX[rel_fn] = 0;			//		bres		puertoa,#rel_fn;		// Manuel 10-feb-2022  ;RM_20220623  Cambio de puertos para cargas
				portX[led_pot] = 0;			//		bres		puertoa,#led_pot;

				if(!GPIOR1[f_led]){
					goto	jmp_watch_02;
				}
				portX[led_pot] = 1;			// 		bset		puertoa,#led_pot;
jmp_watch_02:

				if(timerOffManto != 0){
					goto	jmp_watch_12;			//jrne		jmp_watch_12;			/ si hay tiempo de apagado de mantenimiento manten las cargas apagadas.
				}

//;------------------------- Control del retardo Compresor
				if(!GPIOR0[f_comp]){
					goto	jmp_watch_03_00;
				}
				if(retzc_ms_compresor != 0){
					goto	jmp_watch_03;				//jrne    jmp_watch_03                    ;NO, espera a que se cumpla
				}
				portX[rel_co] = 1;					// bset		puertoa,#rel_co; /Enciende el compresor    ;RM_20220623  Cambio de puertos para cargas
				goto jmp_watch_03;					// jra			jmp_watch_03
jmp_watch_03_00:
				retzc_ms_compresor = tms_retcz_compresor;			// 	mov     retzc_ms_compresor,#tms_retcz_compresor       ;Carga retardo para encendido de compresor

jmp_watch_03:

				if(GPIOR0[f_comp]){				//btjt  	GPIOR0,#f_comp,jmp_watch_05;
					goto	jmp_watch_05;
				}
				if(GPIOR1[f_fan]){				//btjt		GPIOR1,#f_fan,jmp_watch_04
					goto	jmp_watch_04;
				}
				ret_fan_co = 15;					//mov			ret_fan_co,#15;	/ Si, el compresor esta apagado carga retard
				goto	jmp_watch_05;				//jra			jmp_watch_05
jmp_watch_04:
				ret_fan_co = 0;						//clr			ret_fan_co
jmp_watch_05:

//;------------------------- Control del retardo deshielo
watch55:		if(!GPIOR0[f_dh]){				//btjf		GPIOR0,#f_dh,jmp_watch_07_00;
					goto	jmp_watch_07_00;
				}
				if(retzc_ms_deshielo != 0){				// tnz			retzc_ms_deshielo              ;¿Ya se cumplió el retardo al cruce por cero para el deshielo?
					goto	jmp_watch_07;				// jrne    jmp_watch_07                    ;NO, espera a que se cumpla
				}
				portX[rel_dh] = 1 ;						// bset		puertoe,#rel_dh; Enciende el deshielo    ;RM_20220623  Cambio de puertos para cargas
				goto jmp_watch_07;			    		// jra			jmp_watch_07

jmp_watch_07_00:
				retzc_ms_deshielo = tms_retcz_deshielo;	 	//mov     retzc_ms_deshielo,#tms_retcz_deshielo       ;Carga retardo para encendido de deshielo

jmp_watch_07:

//;------------------------- Control del retardo Lampara
watch60:		if(!GPIOR0[f_lamp]){				//btjf		GPIOR0,#f_lamp,jmp_watch_11_00;
					goto	jmp_watch_11_00;
				}
				if(retzc_ms_lampara != 0){				//;¿Ya se cumplió el retardo al cruce por cero para lampara?
					goto	jmp_watch_11;				// jrne    jmp_watch_11                    ;NO, espera a que se cumpla
				}
				portX[rel_lz] = 1 ;						// bset		puertoe,#rel_lz; // Manuel 10-Mar-2022	Enciende la lampara   ;RM_20220623  Cambio de puertos para cargas
				goto	jmp_watch_11;					// jra			jmp_watch_11

jmp_watch_11_00:
				retzc_ms_lampara = tms_retcz_lampara;		//	mov     retzc_ms_lampara,#tms_retcz_lampara       ;Carga retardo para encendido de lampara

//;------------------------- Control del retardo Ventilador
jmp_watch_11:
				if(!GPIOR1[f_fan]){
					goto jmp_watch_12_00;
				}
				if(retzc_ms_ventilador != 0){		//tnz			retzc_ms_ventilador              ;¿Ya se cumplió el retardo al cruce por cero para ventilador?
					 goto jmp_watch_12;				//	jrne    jmp_watch_12                    ;NO, espera a que se cumpla
				}
				portX[rel_fn] = 1 ;					// bset		puertoa,#rel_fn; // Manuel 01-feb-2022	Enciende ventilador
				goto jmp_watch_12;					// jra			jmp_watch_12


jmp_watch_12_00:
				retzc_ms_ventilador = tms_retcz_ventilador;			//		mov     retzc_ms_ventilador,#tms_retcz_ventilador       ;Carga retardo para encendido de ventilador

jmp_watch_12:
//;------------------------- Revision de la comunicacion


//;------------------------- Revision de verifica_eeprom:


//;------------------------- Revision de verifica_ram:


//;------------------------- Revision de los Option Bytes




}
