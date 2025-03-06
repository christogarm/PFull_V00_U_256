

#include "main.h"
#include "customMain.h"
#include "temper.h"
#include "ELTEC_EmulatedEEPROM.h"

//;************************************************************************************
//;Número desplegado:	|  0 |   1 |   2 |   3 |   4 |   5 |   6  |  7 |   8 |   9 |   A |   b |   C |   d |   E |   F |
uint8_t	dpytab[]  =		{0x81, 0xBD, 0x92, 0x98, 0xAC, 0xC8, 0xC0, 0x9D, 0x80, 0x88, 0x84, 0xE0, 0xC3, 0xB0, 0xC2, 0xC6,
//;Número desplegado:	|  o |   L |   t |   H |   i |     |     |     |     |     |     |     |     |   n |   P |  -- |
						 0xF0, 0xE3, 0xE2, 0xA4, 0xF7, 0x80, 0xAF, 0xF5, 0xDA, 0xF4, 0xAE, 0xDA, 0xDA, 0xF4, 0x86, 0xFF,
//;Número desplegado:	|  t |   A |   L |   C |   F |   P |   - |   U |   r |   ° |     |     |     |     |     |   u | Letras para grupos de parámetros
						 0xE2, 0x84, 0xE3, 0xC3, 0xC6, 0x86, 0xFE, 0xA1, 0xF6, 0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1,
//;Número desplegado:	| t  |  A  |  L  |  C  |  F  |     |     |     |     |     |     |     |     |     |     |     | Letras para grupos de parámetros
						 0xE2, 0x84, 0xE3, 0xC3, 0xC6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};





void display (void){
	uint8_t estado1_Aux = findLastValue((uint32_t)&eeEstado1); // Agrego para no realizar tantas llamadas; CGM 25/02/2025
    asm ("nop");

	if(edorefri == 0x00){
		goto display_02;
	}

	//;================= INICIO RM_20231106 Agrega mensaje de despliegue bL fijo
	//;Despliega el mensaje bL si esta conectado un dispositivo bL
	//;tiene prioridad sobre el mensaje de nocturno.

	if(flagsBattery[batON]){
		goto display_00;
	}
	if(flagsTX [7] == 0){
		goto display_00;
	}
	op_menu (0x0B, 0x11);
	//datdig1 = 0x0B;			//	mov			datdig1,#$0B         ;Despliega bL
	//datdig2 = 0x11;			//	mov			datdig2,#$11         ;/
	datled_clear();
	//BitClear(datled,0);			// bres		datled,#0						;	/ Apaga el signo y punto decimal
	//BitClear(datled,1);			// bres		datled,#1
	goto display_01;

display_00:
//;================= FIN RM_20231106 Agrega mensaje de despliegue bL fijo
	if (GetRegFlagState(estado1_Aux, est1Refri)){									//btjt   eeEstado1,#est1Refri,display_00b   ;¿Bandera on/off activada? O, no despliegues nada
		goto display_00b;
	}
	op_menu (0x11, 0x10);
	//datdig1 = 0x11;		// 	mov			datdig1,#$11         ;Despliega Lo = Off
	//datdig2 = 0x10;		//  mov			datdig2,#$10         ;/
	datled_clear();
	//BitClear(datled,0);		//	bres		datled,#0						;	/ Apaga el signo y punto decimal
	//BitClear(datled,1);		//	bres		datled,#1

display_00b:
	if (!flagsa[3]){				//btjf    flagsa,#3,display_01   ;¿Bandera de NOCTURNO activada?
		goto display_01;
	}
	op_menu (0x1F, 0x1D);
	//datdig1 = 0x1F;		// mov			datdig1,#$1F         ;Despliega n
	//datdig2 = 0x1D;		// mov			datdig2,#$1D         ;/
	datled_clear();
	//BitClear(datled,0);		// bres		datled,#0						;	/ Apaga el signo y punto decimal
	//BitClear(datled,1);		// bres		datled,#1


display_01:

//;-----------------------------------------------------------------------------------------------
//;										 Mensajes temporales de comandos
//;-----------------------------------------------------------------------------------------------

	//;salto solo para pruebas
	//;jra			display_02

	if(cntMsgCmd == 0 ){  //ld	A,cntMsgCmd;//cp A,#$00 ------ if(cntMsgCmd ==  0 )******???
		goto display_02;//jreq		display_02;
	}
	if(cntMsgCmd >= 200){//cp			A,#200; jruge		dpy03; cp	A,#50;---- if(cntMsgCmd <= 200)
		goto dpy03; //************** ?
	}
	if(cntMsgCmd < 50){   //cp	A,#50; //jrult dpy03;
		goto dpy03;
	}

	//; Mensajes a mostrar
	datled_clear();
	//BitClear(datled,0);			//bres		datled,#0						;	/ Apaga el signo y punto decimal
	//BitClear(datled,1);			//bres		datled,#1


	if (numMsg != 1){  //cp numMsg,#1
		goto askMsg2; //jrne		askMsg2
	}

msg1:
	op_menu (0x11, 0x10);
	//datdig1 = 0x11;//mov			datdig1,#$11
	//datdig2 = 0x10;//mov			datdig2,#$10         ;/
	if(!GetRegFlagState(estado1_Aux, est1Refri)){//btjf		eeEstado1,#est1Refri,msg1_01 ***********
		goto msg1_01;
	}
	datled[sign] = 1;		   //datled = sign; //bset		datled,#sign;

msg1_01:
	goto dpy07; //jra			dpy07

askMsg2:
	if(numMsg != 2){ //cp  a,#2 *******************?
		goto askMsg3; //jrne		askMsg3
	}

msg2:
	op_menu (0x11, 0x01);
	//datdig1 = 0x11; //mov  datdig1,#$11
	//datdig2 = 0x01;//mov			datdig2,#$01
	if(!GetRegFlagState(estado1_Aux, est1Lamp)){//btjf		eeEstado1,#est1Lamp,msg2_01
		goto	msg2_01;
	}
	datdig2 = 0x02;//mov datdig2,#$02

msg2_01:
	goto dpy07;		// jra dpy07


askMsg3:
	if(numMsg  != 3){//a,#3
		goto askMsg4;  //jrne askMsg4
	}

msg3:
	op_menu (0x11, 0x00);
	//datdig1 = 0x11;//mov			datdig1,#$11
	//datdig2 = 0x00;//mov			datdig2,#$00
	if(GetRegFlagState(estado1_Aux, est1LockDr)){//btjt eeEstado1,#est1LockDr,msg3_01 ***************************
		goto msg3_01;
	}
	datled[sign] = 1;			//bset		datled,#sign;

msg3_01:
	goto dpy07;//jra dpy07

askMsg4:
	if(numMsg != 4){ //cp			a,#4
		goto askMsg5;  //jrne		askMsg5
	}

msg4:
	op_menu (0x1F, 0x1D);
	//datdig1 = 0x1F;  //mov datdig1,#$1F
	//datdig2 = 0x1D;  // mov	datdig2,#$1D

	goto dpy07;//jra			dpy07

askMsg5:     ///
	goto dpy07;


display_02:
//-------------------------------------------------------------------------------------------------

	prog_param();

//-----------------------------*************
    if(edorefri == 1){
    	goto display_norm3; //jreq		display_norm3;
    }
    if(fauto[7] == 0 ){//btjf		fauto,#7,display_norm3 ********************
    	goto display_norm3;
    }
    op_menu (0x0A, 0x0A);
    //datdig1 = 0x0A;		//mov			datdig1,#$0A
    //datdig2 = 0x0A;		//mov			datdig2,#$0A
    if (GetRegFlagState(lowByte(cntblkh), 6)){//btjt		cntblkl,#6,blk_aa;  ******************
    	goto blk_aa;
    }
    op_menu (0x1F, 0x1F);
    //datdig1 = 0x1F; //mov			datdig1,#$1F;
    //datdig2 = 0x1F;  //mov			datdig2,#$1F;

blk_aa:
   goto dpy07; //jra dpy07

display_norm3:

      if(!flagsa[arran]){ //btjf		flagsa,#arran,display_j00 ***************************
    	  goto display_j00;
      }//
      goto dpy06;//jra dpy06;

display_j00:

      if(cntdpysp == 0){//A,cntdpysp; cp A,#$00;
    	  goto dpy02;//jreq		dpy02;
      }
      if(cntdpysp >= 200){//cp			A,#200;
    	  goto dpy03;//jruge		dpy03;
      }
      if(cntdpysp < 150){//cp			A,#150;
    	  goto dpydif;//jrult dpydif
      }
          sp_dpy();				//	call sp_dpy;     ************CAAAAALLLLLLL**************************
      	  goto dpy07;
dpydif:
     if(cntdpysp >= 100){//A,#100;
    	 goto dpy03;//jruge		dpy03;
     }
     if(cntdpysp < 50 ){
    	 goto dpy03;//jrult		dpy03;
     }
     dif_dpy();	 		/// Despliega el Diferencial que este operando
     datled[1] = 0;		//bres		datled,#1
     goto dpy07;//jra			dpy07;

dpy02:
	goto dpy06;//jra dpy06



dpy03:

	offdpy();		//call offdpy ********CAAAAALLLLLL****************************

display_j02:
    goto dpy07;//jra dpy07

dpy06:
   if(fauto[3] == 1){//btjt		fauto,#3,dpy07  *******************************
	   goto dpy07;
   }
   if(flagsb[f_prog]){//btjt		flagsb,#f_prog,dpy07; *******************************
	   goto dpy07;
   }

   fallas();	/// *** Revisa si hay que indicar fallas

dpy07:
		// prueba de matematica 02-ago-2024
		// asm ("nop");
		 //convadec_math((uint16_t)(tsac_w));
		 // convadec_math((uint16_t)(-350));
		// asm ("nop");

		// sp_dpy();					// Prueba, Despliega solo SP     ... Ok Funciona
		// dif_dpy();					//call dif_dpy      ... Ok Funciona
		// soloent(versionFirm1);		// Prueba, primer dato de version     ... Ok Funciona
		//wreg = num_ver;			// Versión del Firmware
		//soloent1();				//call soloent1   ... Ok Funciona
		//soloent (Plantilla [interdh]);	//call soloent   ... Ok Funciona
		// prueba de matematica 02-ago-2024


		Display_1 = dpytab[datdig1];		// Apunta al dato correspondiente
		Display_2 = dpytab[datdig2];		// Apunta al dato correspondiente

		//********************  Desactiva Buzzer por pruebas
		   // HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);			// Apaga PWM
			//goto realiza_multiplexeo;//    SALTO para que no suene el Buzzer
		  //********************  Desactiva Buzzer por pruebas

//;-------------------------------------------------------------------------------
//;--------------------------------  Acciones a realizar con el BUZZER
buzzer:
		//++++++++++++++++++++++++++++++++++++++++++++++
		for(uint8_t k=0; k<8; k++){
			if(trefst[k] ){ //tnz  trefst ***************??
				goto ask_snooze; //jrne	ask_snooze
			}
			if(trefst2[k] ){ //tnz  trefst2  *************** ???
				goto ask_snooze;  //jrne	ask_snooze
			}
		}

        // silencioAlarmL = 0;//clr	silencioAlarmL
        silencioAlarmH = 0;//clr	silencioAlarmH
        goto noLoadSnooze;//jra	noLoadSnooze

ask_snooze:
		if(!botonst[b1_f1]) { //btjf botonst,#b1_f1,noLoadSnooze ///???????????
			goto noLoadSnooze;
		}
		ld_alarmDelay();	  //call  ld_alarmDelay  ************************CAAAAAALLLLL**********************

noLoadSnooze:
		if(silencioAlarmH == 0){      // Ya terminó el tiempo de snooze?
			goto no_snooze;//jreq no_snooze
		}
		flagsBuzzer[0] = 1; 	 //bres	flagsBuzzer,#0;

no_snooze:

		if(timeBuzzOn == 0){  //tnz timeBuzzOn*******************************
			goto noBtnBuzz; //jreq noBtnBuzz
		}
		if( flagsBuzzer[1] ){//btjt flagsBuzzer,#1,jp_termina_buzzer  ******?
			goto jp_termina_buzzer;
		}
		// waux = 227;//mov     waux,#%11100011
		//mov     BEEP_CSR2,waux *****************************************BEEP_CSR2 no esta********
		HAL_TIM_PWM_Start (&htim3,TIM_CHANNEL_2);			// Enciende PWM
		flagsBuzzer[1] = 1;		//bset		flagsBuzzer,#1
jp_termina_buzzer:
		goto termina_buzzer;//jra termina_buzzer

noBtnBuzz:
		if(!flagsBuzzer[1]){//btjf flagsBuzzer,#1,noBtnBuzz_01
			goto noBtnBuzz_01;
		}
		ban_buzzer[0] = 0; //bres    ban_buzzer,#0
		edo_buzzer = 0;//mov     edo_buzzer,#0
		//mov   	BEEP_CSR2,#%00000011	;BEEP Control/Status Register DESACTIVADO
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);					// Apaga Sonido de Buzzer por Pruebas
		//ldw     X,cnt_gen_ms
		//addw    X,#5000
		//ldw     t_buzzer,X
		t_buzzer = cnt_gen_ms + 5000;

noBtnBuzz_01:
		flagsBuzzer[1] = 0;//bres flagsBuzzer,#1

		if(flagsBuzzer[0]){//btjt flagsBuzzer,#0,buzzer_0
			goto buzzer_0;
		}
		ban_buzzer[0] = 0;//bres    ban_buzzer,#0
		edo_buzzer = 0;//mov     edo_buzzer,#0
		//mov   	BEEP_CSR2,#%00000011 ****************************BEEP_CSR2 no esta******
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);					// Apaga Sonido de Buzzer por Pruebas
		//ldw     X,cnt_gen_ms
		//addw    X,#100
		//ldw     t_buzzer,X
		t_buzzer = cnt_gen_ms + 100;

		goto realiza_multiplexeo;//jp      realiza_multiplexeo

//                	 goto buzzer_0;
buzzer_0:
		edo_buzzer = 1;//edo_buzzer = 1;

		if(!trefst[f_hv]){// btjf		trefst,#f_hv,buzzer_activado_04
			goto buzzer_activado_04;
		}
		edo_buzzer = 2;//mov     edo_buzzer,#2

buzzer_activado_04:
		if(!trefst[f_lv]){//btjf		trefst,#f_lv,buzzer_activado_05
			goto buzzer_activado_05;
		}
		edo_buzzer = 2;//mov     edo_buzzer,#2


buzzer_activado_05:
		if(!flagsBattery[batON]){//btjf		flagsBattery,#batON,buzzer_activado_08
			goto buzzer_activado_08;
		}
		edo_buzzer = 2;//mov     edo_buzzer,#2

buzzer_activado_08:
		if(!flagsTC[f_TC1]){//btjf	flagsTC,#f_TC1,buzzer_activado_01
			goto buzzer_activado_01;
		}
		edo_buzzer = 2;//mov     edo_buzzer,#2

buzzer_activado_01:
		if(!flagsTC[f_TC2]){//btjf		flagsTC,#f_TC2,buzzer_activado_02
			goto buzzer_activado_02;
		}
		edo_buzzer = 3;//mov     edo_buzzer,#3

buzzer_activado_02:
		if(!flagsTC[f_TC3]){//btjf		flagsTC,#f_TC3,buzzer_activado_03
			goto buzzer_activado_03;
		}
		edo_buzzer = 1;//mov     edo_buzzer,#1

buzzer_activado_03:
		if(!flagsBattery[batLOW]){//btjf flagsBattery,#batLOW,buzzer_activado_09
			goto buzzer_activado_09;		//; ---------------------------------------------------------------------------------------------------
		}
		edo_buzzer = 1;//mov     edo_buzzer,#1

buzzer_activado_09:

		//ld			A,trefst
		//and			A,#%00111111
		//cp			A,#0
		uint8_t trefst_aux_ = 0;
		for(uint8_t k=0; k<8; k++){
			trefst_aux_ |= (uint8_t) (trefst[k]<<k);
		}

		if((trefst_aux_ & 0x3F) == 0x00){//jreq buzzer_activado_06  **********************?comparacion con and
			goto buzzer_activado_06;
		}
		edo_buzzer = 1;//mov     edo_buzzer,#1

buzzer_activado_06:
		//ld			A,trefst2
		trefst_aux_ = 0;
		for(int k=0; k<8;k++){
			trefst_aux_ |= trefst2[k]<<k;
		}
		if(trefst_aux_ == 0x00){//cp A,#0 // //jreq		buzzer_activado_07
			goto buzzer_activado_07;
		}
		edo_buzzer = 1;//mov     edo_buzzer,#1

buzzer_activado_07:

		//ld			A,edo_buzzer
		//cp			A,#0

		switch(edo_buzzer)
		{
			case 0: goto buzzer_activado;
				//break;
			case 1: goto buzzer_etapa_1;
				//break;
			case 2: goto buzzer_etapa_2;
				//break;
			case 3: goto buzzer_etapa_3;
				//break;
			case 4: goto buzzer_etapa_4;
				//break;
			default: goto buzzer_disponible;//jra			buzzer_disponible
				//break;
		}

//		if( edo_buzzer == 0){			//jreq		buzzer_activado
//			goto buzzer_activado;
//		}
//		//cp			A,#1
//		if(edo_buzzer == 1){//jreq		buzzer_etapa_1
//			goto buzzer_etapa_1;
//		}
//		//cp			A,#2
//		if(edo_buzzer == 2){//jreq		buzzer_etapa_2
//			goto buzzer_etapa_2;
//		}
//		//cp			A,#3
//		if(edo_buzzer == 3){//jreq		buzzer_etapa_3
//			goto buzzer_etapa_3;
//		}
//		//cp			A,#4
//		if(edo_buzzer == 4){//jreq		buzzer_etapa_4
//			goto buzzer_etapa_4;
//		}
//		goto buzzer_disponible;//jra			buzzer_disponible



buzzer_activado:  //;------------------------------------------------------------

inicializa_tiempo_bz:

		//ldw     X,cnt_gen_ms
		//addw    X,#100
		//ldw     t_buzzer,X
		t_buzzer = cnt_gen_ms + 100;

		goto realiza_multiplexeo; 		//jp      realiza_multiplexeo



buzzer_etapa_1:
		//ldw			X,#500
		buzzer_ton = 500;//ldw buzzer_ton,X
		//ldw			X,#500
		buzzer_toff = 500;  //ldw	buzzer_toff,X
		//waux = 0xE3;                 //mov waux,#%11100011
		goto activa_buzzer;//jp      activa_buzzer


buzzer_etapa_2:
		//ldw			X,#500
		buzzer_ton = 500;//ldw			buzzer_ton,X
		//ldw X,#30000
		buzzer_toff =  30000; //ldw			buzzer_toff,X
		//waux =  0xE3;   //mov     waux,#%11100011
		goto activa_buzzer;//jp      activa_buzzer

buzzer_etapa_3:

		//ldw			X,#500
		buzzer_ton = 500;//ldw			buzzer_ton,X
		//ldw			X,#15000
		buzzer_toff = 15000;//ldw			buzzer_toff,X
		//waux = 0xE3;//mov     waux,#%11100011
		goto activa_buzzer;//jp      activa_buzzer

buzzer_etapa_4:
		// waux = 0xE3;//mov waux,#%11100011



activa_buzzer:
		//ldw     X,cnt_gen_ms
		if(cnt_gen_ms != t_buzzer){//cpw     X,t_buzzer
			goto termina_buzzer;//jrne    termina_buzzer
		}
		ban_buzzer[0] ^= 1;// BitComplement(ban_buzzer,0);		//ban_buzzer	^= 	(1 << 0);					///ban_buzzer[0] = //bcpl    ban_buzzer,#0 ******************************************BCPL
		if(ban_buzzer[0]){//btjt    ban_buzzer,#0,buzzer_on
			goto buzzer_on;
		}
buzzer_off:
		//BEEP_CSR2 //mov       BEEP_CSR2,#%00000011 *************BEEP_CSR2  no esta****************
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);					// Apaga Sonido de Buzzer por Pruebas
		//ldw     X,cnt_gen_ms
		//addw    X,buzzer_toff
		//ldw     t_buzzer,X
		t_buzzer = cnt_gen_ms + buzzer_toff;
		goto termina_buzzer; //jra     termina_buzzer

buzzer_on:
		//mov     BEEP_CSR2,waux ******************BEEP_CSR2 no esta*****
		HAL_TIM_PWM_Start (&htim3,TIM_CHANNEL_2);			// Enciende PWM
		//ldw     X,cnt_gen_ms
		//addw    X,buzzer_ton
		//ldw     t_buzzer,X
		t_buzzer = cnt_gen_ms + buzzer_ton;

termina_buzzer:

buzzer_disponible:


//;-----------------------------------------------------------------------------------------------

realiza_multiplexeo:
//;-------------------------------------------------------------------------------
	for(uint8_t k=0;k<8;k++){
		Ind_Par[k] = 1;//	mov			Ind_Par,#$FF
	}

	if(!datled[dp])				//	btjt		datled,#dp,display_j05;		/ Debe encenderse el punto decimal?
		Ind_Par[1] = 0;		//	bres		Ind_Par,#1;/ No, apagalo.
	if(!datled[sign])			//	btjt		datled,#sign,display_j07;	/ Debe encenderse el signo menos?
		Ind_Par[0] = 0;		//	bres		Ind_Par,#0;/ No, apagalo.
	if(!datled[luzC])			//	btjt		datled,#luzC,trefD_01;		/ Debe encenderse el punto decimal?
		Ind_Par[4] = 0;		//	bres		Ind_Par,#4;/ No, apagalo.
	if(!datled[luzD])			//	btjt		datled,#luzD,trefD_02;	/ Debe encenderse el signo menos?
		Ind_Par[5] = 0;		//	bres		Ind_Par,#5;/ No, apagalo.
	if(!datled[luzF])			//	btjt		datled,#luzF,trefD_03;		/ Debe encenderse el punto decimal?
		Ind_Par[6] = 0;		//	bres		Ind_Par,#6;/ No, apagalo.
	if(!datled[luzN])			//	btjt		datled,#luzN,trefD_04;	/ Debe encenderse el signo menos?
		Ind_Par[7] = 0;		//	bres		Ind_Par,#7;/ No, apagalo.



	if(edorefri >=2){
		goto	clt_ledInd;
	}
	Ind_Par[2] = 0;		//	bres		Ind_Par,#2
	goto	no_blk;

clt_ledInd:

	Ind_Par[2] = 1;			//bset		Ind_Par,#2;
	if(datled[luzD]){
		goto	blink_dia;
	}

	if(cnt_pta_fan == 0){				// / El contador de puerta abierta llego a cero?
		goto	no_blk;
	}
	if(!flagsC[f_doorOpen]){// if(!GetRegFlagState(flagsC, f_doorOpen)){
		goto	no_blk;
	}

	if(GetRegFlagState(lowByte(cntblkh), 5)){				// cntblkl,#5,display_j11;		Parpadea cada 320 ms
	    asm ("nop");
		goto	display_j11;
	}
	Ind_Par[2] = 0;					//bres		Ind_Par,#2
	goto	no_blk;

blink_dia:
	if(GetRegFlagState(lowByte(cntblkh), 7)){			  // cntblkl,#7,display_j11; 		Parpadea cada 1280 ms
		goto	display_j11;
	}
	Ind_Par[2] = 0;

display_j11:
no_blk:

	asm ("nop");
//;---------------------------------------------------------------------------
//;							Con refri OFF manten led puerta apagado.
//;---------------------------------------------------------------------------

	if (estado1_Aux & (1 << est1Refri)){									//btjt   eeEstado1,#est1Refri,display_00b   ;¿Bandera on/off activada? O, no despliegues nada
		goto	refriON;
	}
refriOFF_2:
	Ind_Par[2] = 1;
refriON:

findpy:

		goto	main_display;	//jp			main_display			;Manuel 16-FEB-2022:jp			main;
main_display:


}






























