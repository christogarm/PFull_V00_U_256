/*
Traduccion hecha por : MGHJ__12/07/2024
 */

#include "main.h"
#include "customMain.h"


void tiempo (void){

    asm ("nop");



		if(cntbase < 8){				//  Esta iniciando la centésima?
			goto tiempo10;				//	jrult		tiempo10;
		}  //compara a cntbase
		goto fintiem;					// jp			fintiem;		/ No, termina

	////    Variables que se decrenetan cada centesima
tiempo10:
				cntblkh++;						/// Cada 10 ms

			   decwreg(&delayComStat);
			   decwreg(&cntMsgCmd);			//	Decrementa tiempo de desplegado de mensajes de comando
			   decwreg(&timeBuzzOn);		//	Decrementa tiempo de encedido del buzzer

			   decwreg(&cntdpysp);			//	Decrementa tiempo de desplegado de nuevo setpoint
		       decwreg(&retcapfc);			//	Agota el retardo para capturar señal de Fotocelda

		       decwreg(&cnt_ecom);			//	Agota el retardo
		       decwreg(&cnt_dpyrst);		//	Agota el retardo
		       decwreg(&cnt_btn);			//	Agota el retardo
		       decwreg(&cnt_blk_btn);		//	Agota el retardo

		       cntcent++;					//	Una centésima más
		       if (!GetRegFlagState(cntcent, 0)){  		//	btjf		cntcent,#0,no_dec_deb
			        goto no_dec_deb;
		       }

		       decwreg(&deb2_btn1);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb2_btn2);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb2_btn3);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb2_btn4);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb1_btn2);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb1_btn3);			// Agota el retardo para función secundaria de botones
		       decwreg(&deb_2btn);			// Agota el retardo para función secundaria de botones
no_dec_deb:

		        if(cntcent >= 100){ 		// cp			A,#$64;	/ ¿Se completo un segundo?
		        	goto tiempo12;			//;jruge		tiempo12
		        }
		        goto fintiem;

tiempo12:
				cntcent = 0;				//clr			cntcent;		/ Si, inicia desde 0 centésimas
//;***********************************************************************************
//;-----------------------------------------------------------------------------------

				//; Funciones de está seccción no se ejecutan hasta que la maquina de estados BLE esté en transmit/recieve
				//ld			A,BluetoothState
				//cp			A,#3
				if(BluetoothState!=3)//jrne		endSelect
					goto endSelect;
				//ldw			X,#cntSetName;
				decword(&cntSetName);//call		decword

				//; Cambia canal de comunicacion BLE/WIFI
selectCOM:
				//btjt	flagsTxControl,#f_statBLE,no_toggleCOM;  Con conexión BLE activa no cambies el canal de comunicacion
				//; no cambies de canal de comunicación si se está transmitiendo algun logger via wifi
				//btjt	flagsWIFI,#f_timeLoggerCmd,no_toggleCOM;
				//btjt	flagsWIFI,#f_eventLoggerCmd,no_toggleCOM;

				if(flagsTxControl[f_statBLE] || flagsWIFI[f_timeLoggerCmd] || flagsWIFI[f_eventLoggerCmd])
					goto no_toggleCOM;

toggleCOM:
				//; no cambies el canal de comunicación si hay una respuesta en transmisión en progreso
				//tnz		keyTx
				if(keyTx)//jrne	no_toggleCOM
					goto no_toggleCOM;
				flagsTxControl[f_select] ^= 0x1;// bcpl		flagsTxControl,#f_select
				delayComStat = 10;//mov			delayComStat,#10
no_toggleCOM:

				if(!flagsTxControl[f_select])//btjf		flagsTxControl,#f_select,BLEselect
					goto BLEselect;
WIFIselect:
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);//bset		PE_ODR,#2
				goto endSelect;//jra			endSelect
BLEselect:
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);//bres		PE_ODR,#2
endSelect:

//;-----------------------------------------------------------------------------------
//;***********************************************************************************

//				if (flagsTime [f_timeConfigRTC]){ //btjt salta si es = 1--- flagsTime,#f_timeConfigRTC,noIncTime; ***********************
//					goto	noIncTime;
//				}
//				timeSeconds_LW++;				// Incrementa parte baja del tiempo UNIX
//    		    if(timeSeconds_LW){  			//JRNE		no_inc_timeH;			/ Hubo overflow ?
//    		    	goto no_inc_timeH;			//
//    		    }
//    		    timeSeconds_HW++;				// Sí, Incrementa parte alta del tiempo UNIX
//no_inc_timeH:
//
//noIncTime:

	            decword(&temp_doorEvent);		// decremnta registro de duración evento puerta
	            decwreg(&timeTxTBLE);
				decword(&timeoutTBLE);
	            decwreg(&timeTxTWF);
		    	decword(&timeoutTWF);			// decrementa timeout token wifi
		    	decwreg(&timeoutTxWifi);		// decrementa timeout respuesta de wifi
		    	decwreg(&delayTxLoggWifi);		// decrementa tiempo entre envíos de logger
		    	decwreg(&retLampOff);			// decrementa tiempo de retardo para apagado de lampara
		    	decwreg(&retPowerOn);			// decrementa tiempo de retardo para apagado de lampara
		    	decwreg(&timeUnlockWIFI);		// decrementa tiempo de desbloqueo de comandos protegidos por password
		    	decwreg(&debBtn2F3);			// Agota el retardo para función 3 boton 2


		    	decword(&silencioAlarmH);		// decremnta registro de duración evento puerta
		    	decword(&timePreDh_h);



		    	////    Variables que se decrenetan cada segundo

		    	//;IJG codigo innecesario										ldw			X,#{waitrunco};Variables que se decrenetan cada segundo
		    	//ldw			X,#retvent;Variables que se decrenetan cada segundo
tiempo16:		//call		decwreg;
				//incw		X;
				//cpw			X,#End_sec;
				//jrult		tiempo16;

		    	decwreg(&retvent);//	Retardo para el encendido del ventilor
		    	decwreg(&minfunc);//		Tiempo mínimo de función activa
		    	decwreg(&durautop);//	Duración de autoprueba
		    	decwreg(&cntdpyev);//	Tiempo desplegado temperatura de evaporador
		    	decwreg(&cntobscu);//	Contador de tiempo de obscuridad en la fotocelda
		    	decwreg(&cntfail);//		Contador para indicar fallas
		    	decwreg(&valflvl);//
		    	decwreg(&valfhvl);//
		    	decwreg(&cnt_prog);//
		    	decwreg(&cnt_btn_hld);//
		    	decwreg(&ret_fan_co);//	Retardo para el encendido del ventilor por compresor recien encendido
		    	decwreg(&End_sec);//



		    	decword(&cntDefi_H); // decrementa tiempo de desficiencia

		    	decwreg(&timeOutRst);
		    	decwreg(&timeOutRx);
		    	decwreg(&timeRstBLE);
		    	decword(&cntLogger_H);			// decrementra contador de logger
		    	decwreg(&cntInitTx);
		    	decword(&timeDataWF_H);			// decrementra contador de envio de datos por tiempo WIFI

		    	decword(&t_ahorro1_H);
		    	decword(&t_ahorro2_H);
		    	decword(&cntNoct_H);

		    	if(GetRegFlagState(durautop, 0)){   //btjt durautop,#0,no_dec2
		    		goto no_dec2;
		    	}
	    		decwreg(&durautop2);
		    	  //btjt		durautop,#0,no_dec2

no_dec2:

		    	decword(&retproth);
		    	decword(&tminstoph);			// Decrementa tiempo de descanso del compresor
		    	decword(&cntpah);				// Decremetna tiempo permitido de puerta abierta.
		    	decword(&drp_fanh);
		    	decword(&drp_comph);


		    	if(interdhh >= coontimeh ){  //jruge tiempo 17, salta si c = 0  *************************
		    		goto tiempo_17;
		    	}

		        if(portX[rel_co]){  //tiempo_j00 = tiempo_17
		        	goto	tiempo_j00;		//tiempo_j00;	/ Esta encendido el compresor?    ;RM_20220623  Cambio de puerto para compresor
		        }
	        	goto tiempo20; //jra tiempo20

tiempo_j00:
tiempo_17:       decword(&interdhh);			// Agota el tiempo de interdeshielo

tiempo20:		 decword(&durdhh);				// Agota la duración del deshielo

				holdseg++;						// Contador de segundos para retencion de display
    	    	if(holdseg < 60){
    	    		 goto tiempo40;				//jrult		tiempo40;		/ No, continua
		    	 }
	 			holdseg = 0;					//clr			holdseg;		/ Sí, reinicia contador de segundos
		        decwreg(&cnthold);

tiempo40:
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);	 //28-May-2024: Salida IO7 toogle test
				if(GetRegFlagState(cntseg, 3)){
					asm ("nop");
					// GPIOR0[f_comp] = 0;				// Apaga el compresor 	..   Toogle FUNIONA OK
					// GPIOR0[f_dh]  = 0;				// Apaga DH       		..   Toogle FUNIONA OK
					// GPIOR1[f_fan] = 0;					// Apaga FAN		  	..   Toogle FUNIONA OK
					//GPIOR0[f_lamp] = 0;				// Apaga  lampara 		..   Toogle FUNIONA OK
				}
				else{
					asm ("nop");
					// GPIOR0[f_comp] = 1;				// Enciende el compresor
					// GPIOR0[f_dh] = 1;				// Enciende DH
					// GPIOR1[f_fan] = 1;					// Enciende FAN
					// GPIOR0[f_lamp] = 1;				// Enciende lampara
				}
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				cntseg++;							//inc			cntseg;			/ Un segundo más
				if(cntseg < 9){ //cp cntseg, #9
		    		goto tiempo50;				//jrult		tiempo50;		/ No, espera otro segundo
		    	}
				flagsa[0]= 0;		 // bres		flagsa,#0;0x01;	/ Si, termina arranque
tiempo50: 		if(!GetRegFlagState(cntseg, 4)){ // btjf		//cntseg,#4,tiempo_j01; **********************************
					goto	tiempo_j01;		// Ya son 16 segundos?
		    	}
				flagsa[7] = 0;			 //bres		flagsa,#7    / Si, termina inicalización
tiempo_j01:		if(cntseg < 60){  				//cp			A,#$3C;	/ Ya se completó un minuto?
					goto fintiem;				//jrult fintiem
		    	 }
		    	 cntseg = 0;

		    	 fauto [7] = 0; //bres		fauto,#7 ********************************* esta declarado como 8 bits

 ////    Variables que se decrementan cada minuto

		    	 decwreg(&cnt_pta_fan); // Agota tiempo de encendido de ventilador
	    		 decwreg(&ton_fan);  //Agota tiempo de encendido de ventilador
	    	     decwreg(&toff_fan); //Agota tiempo de apagado de ventilador
	    		 decwreg(&retnoct); // Agota el retardo para entrar a modo nocturno de forma automática
	    		 decwreg(&cnthitemp); //Agota el tiempo temperatura alta
	    		 decword(&retnocth); //Agota el retardo para entrar a modo nocturno con luz
	    		 decword(&cntworkh); //Agota el tiempo maximo de trabajo de compresor
	    		 decword(&cntexhah); //Agota el tiempo de descanso de compresor por compresor exhausto

	    		 decwreg(&timerOffManto); // Agota tiempo de apagado de mantenimiento
	    		 goto fintiem;


fintiem:
tiempo_j02:
	             goto main_tiempo;
main_tiempo:

}
