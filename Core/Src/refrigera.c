

#include "main.h"
#include "customMain.h"
#include "board_PFULLDEF.h"
#include "ELTEC_EmulatedEEPROM.h"


void refrigera (void){
				uint8_t estado1_Aux;

				// VAlor de Carga de Prueba
				//tsac_w = -250;
				// VAlor de Carga de Prueba

				for (int i = 0; i < 8; i++) { //clr	GPIOR0 //clr GPIOR1;
					GPIOR0[i] = 0;										// Limpia registro de banderas
					GPIOR1[i] = 0;										// Limpia registro de banderas
				}
				wreg = 	Plantilla [logicos]; //mov wreg,logicos;		;	/ Parámetro A2
				if(!GetRegFlagState(wreg,0)){ //btjf wreg,#0,refrigera_j00;    // Indica función ahorradora
					goto refrigera_j00;
				}
				GPIOR0 [f_ahorra] = 1; //bset GPIOR0,#f_ahorra
refrigera_j00:
				if(!GetRegFlagState(wreg, 1)){ //btjf wreg,#1,refrigera_j01;
					goto refrigera_j01;
				}
				GPIOR0 [f_disable] = 1; //bset GPIOR0,#f_disable			/ Deshabilitado el deshielo al arranque

refrigera_j01:
				wreg = Plantilla [histvol]; //mov wreg,histvol
				BaBentre10_math();		//call BaBentre10_math
                //ld A,protype;
				if(Plantilla [protype] != 0x02){ //cp A,#$02;					//	Protección de voltaje 220v?
					goto ask_protvolt_120; //jrne ask_protvolt_120;			;		No, Pregunta por la de 120v
				}
				//ld a,volt_100
				volt_min = Plantilla [volt_100] + 100; //add a,#100				Carga como voltaje minimo
				//ld volt_min,a;
				volt_hmin = volt_min + waux;//add a,waux //ld volt_hmin,a;		Carga como voltaje minimo con histeresis
				//ld a,volt_200
				volt_max = Plantilla [volt_200] + 200; //add a,#200 //ld volt_max,a;		//Carga como voltaje maximo
				volt_hmax = volt_max - wreg; //sub a,wreg  //ld	volt_hmax,a
				goto set_protvolt; //jra set_protvolt

ask_protvolt_120:
				if(Plantilla [protype] != 0x01){ //cp A,#$01;
					goto refrige05;//jrne refrige05
				}
        //ld	 a,volt_000
				volt_min = Plantilla [volt_000]; //ld volt_min,a;				//Carga como voltaje minimo
				volt_hmin = volt_min + waux; //add a,waux //ld volt_hmin,a;		//Carga como voltaje minimo con histeresis
				volt_max = Plantilla [volt_100] + 100;//ld a,volt_100 //add a,#100		//Carga como voltaje maximo
				volt_hmax = volt_max - wreg; //sub a,wreg //ld volt_hmax,a				//Carga como voltaje maximo con histeresis

set_protvolt:
                GPIOR0[f_volt] = 1;				//;	/ Indica protección de voltaje
refrige05:
				GPIOR1[f_led] = 1; //bset	GPIOR1,#f_led;

				//;*****************************
				//; Para firmware de equipo medico solo se permite deshielo por ventilador, ya que el relevador de deshielo se usa para cerradura
				//;	por lo que se fuerza el parámetro de tipo de deshielo (dhmode) a ser igual a cero
				Plantilla [dhmode] = 0; //clr dhmode

              ///ldw X,spdiur_w
              //ldw	Y,difdiur_w;
              // liminf_w = (uint16_t)(Plantilla[spdiur_H] * 256) + (uint16_t)(Plantilla[spdiur_L]);  //ldw liminf_w,X
				liminf_w = TwoByteInArrayToWord (&Plantilla[spdiur_H]);				// (uint16_t)(Plantilla[spdiur_H] * 256) + (uint16_t)(Plantilla[spdiur_L]);  //ldw liminf_w,X

              //tempo2 = spdiur_w; //ldw tempo2,X
              //waux = difdiur_w; //waux,Y
              //call sumafbcd_math
              //ldw     X,waux
              //ldw   	limsup_w,X
				limsup_w = liminf_w + TwoByteInArrayToWord (&Plantilla[difdiur_H]);       //

// ;// codigo para modos de ahorro 1 y 2
				if(!flagsC[f_ahorro1]){ //btjf flagsC,#f_ahorro1,no_ahorro1
					goto no_ahorro1;
				}
				liminf_w = liminf_w +  TwoByteInArrayToWord (&Plantilla[difAhorro1_H]);
				limsup_w =  limsup_w + TwoByteInArrayToWord (&Plantilla[difAhorro1_H]);

				if(!flagsC[f_ahorro2]){ // btjf	flagsC,#f_ahorro2,no_ahorro1
					goto no_ahorro1;
				}
				liminf_w = liminf_w  + TwoByteInArrayToWord (&Plantilla[difAhorro2_H]);
				limsup_w = limsup_w  + TwoByteInArrayToWord (&Plantilla[difAhorro2_H]);

no_ahorro1:


				if(!GetRegFlagState(edorefri,1)){ //btjf	edorefri,#1,refrigera_j04;
					goto refrigera_j04;
				}
				goto refrige10;
refrigera_j04:
				offdpy();		//call offdpy			/// Apaga el display
				//datled &=  0x80;	//ld A,datled//and A,#$80;	// Apaga los led´s
				for(uint8_t k=0; k<7;k++){
					if(k!=4)
						datled[k] = 0;
				}
				if(GetRegFlagState(edorefri,0)){ //btjt	edorefri,#0,refrigera_j05;
					goto refrigera_j05;
            	}
            	goto	indica;			//jp			indica;			/ edorefri = 0; Indicación de los parámetros de operación
refrigera_j05:
				goto	autopru;		// edorefri = 1; Estado de diagnóstico del equipo
refrige10:
				// Prueba  tsac_w = -14;
				desptdv_math();			//call desptdv_math
				Bclear_Clear_trfst(datled, datled,2, 3);
//				BitClear(datled,2);		//datled[2] = 0;			/ Apaga leds de deshielo y compresor
//				BitClear(datled,3);		//datled[3] = 0;			/ Apaga leds de deshielo y compresor
//;---------------------------------------------------------------------------------------------------------------------------------
//;								CONTROL DE RELE CEERRADURA
//;---------------------------------------------------------------------------------------------------------------------------------
func_lock:
            if(!botonst[b3_f1]){ //btjf botonst,#b3_f1,no_Func_Lock;		/ ¿Función primaria del botón lampara?
            	goto no_Func_Lock;
            }
        	//; indica que hay que mostrar un mensaje
            cntMsgCmd = 250;
            numMsg = 3;
            //BitComplement(estado1,est1LockDr);		 //	||||||||||   bcpl estado1,#est1LockDr; //	/ cambia estado de rele cerradura
            estado1[est1LockDr] ^= 0x1;
            //waux = eeEstado1;
            waux = findLastValue((uint32_t)&eeEstado1);;
            BitClear(waux, est1LockDr);//bres waux,#est1LockDr
            if(!estado1[est1LockDr]){//btjf estado1,#est1LockDr,func_lock_01;
            	goto func_lock_01;
            }
            BitSet(waux, est1LockDr); //bset waux,#est1LockDr
func_lock_01:
			//; carga waux en eeprom
            //ldw X,#eeEstado1;
            //call wreeprom
			wreeprom ( waux,  &eeEstado1);
            //MOV IWDG_KR,#$AA;
no_Func_Lock:
//;---------------------------------------------------------------------------------------------------------------------------------
//;								CONTROL DE LAMPARA
//;---------------------------------------------------------------------------------------------------------------------------------
               if(!botonst[b2_f1]){ //btjf botonst,#b2_f1,refrigera_j06c;   / ¿Función primaria del botón lampara?
            	   goto refrigera_j06c;
               }
               //; indica que hay que mostrar un mensaje
               cntMsgCmd = 250; //mov cntMsgCmd,#250
               numMsg = 2; //mov numMsg,#2
               flagsC[f_lampDoor] ^= 0x1;// BitComplement(flagsC,f_lampDoor);			//	flagsC[f_lampDoor] = 0; //bcpl flagsC,#f_lampDoor
               //waux = eeEstado1;
               waux = findLastValue((uint32_t)&eeEstado1);;
               BitClear(waux, est1Lamp);//bres waux,#est1Lamp
               if(!flagsC[f_lampDoor]){ //btjf flagsC,#f_lampDoor,refrigera_j06d;
            	   goto refrigera_j06d;
               }
               BitSet(waux, est1Lamp);//bset waux,#est1Lamp
refrigera_j06d:
                //ldw X,#eeEstado1;
				//call wreeprom
				wreeprom ( waux,  &eeEstado1);
				//MOV IWDG_KR,#$AA
refrigera_j06c:

                if(flagsC[f_ahorro2]){//btjt flagsC,#f_ahorro2,refrigera_j06;
                	goto refrigera_j06;
                }
                if(flagsa[nocturno]){//btjt flagsa,#nocturno,refrigera_j06
                	goto refrigera_j06;
                }
                if(!flagsb[f_luzb]){ //btjf flagsb,#f_luzb,refrigera_j06
                	goto refrigera_j06;
                }
                if(flagsC[f_lampDoor]){//btjt flagsC,#f_lampDoor,refrigera_j06a
                	goto refrigera_j06a;
                }
                if(retLampOff != 0){ //tnz retLampOff
                	goto refrigera_j06a; //jrne	refrigera_j06a
                }
                if(!flagsC[f_doorOpen]){ //btjf flagsC,#f_doorOpen,refrigera_j06;
                	goto refrigera_j06;
                }
refrigera_j06a:
                 GPIOR0[f_lamp] = 1;// bset		GPIOR0,#f_lamp; 	/ No, enciende bandera de activación de lámpara
refrigera_j06:
//;---------------------------------------------------------------------------------------------------------------------------------
                if(GetRegFlagState(edorefri, 0)){ //btjt edorefri,#0,refrigera_j07;
                	goto refrigera_j07;
                }
                goto	control;				//jp			control ;		/ edorefri = 2; Control de temperatura basado en la temperatura ambiente
refrigera_j07:	goto autodesh;					//jp			autodesh;		/ edorefri = 3; Estado de deshielo de modo automático

toindi:			flagsa[0] = 1;		 	//bset		flagsa,#0;0x01;	/ Indica período de arranque
				cntseg = time_auto - time_ini;
				edorefri = 0;
				goto toap10;					// Limpia contador de parpadeos

toap:     		durautop = 17;				//  Carga el tiempo de duracuón de la autoprueba
          		durautop2 = 8;				//  Carga el tiempo de duracuón de la autoprueba
          		edorefri = 0x01;				/// Carga el estado de Autoprueba
          		cntseg = 0;
toap10:   		// cntblkl = 0;
				cntblkh = 0;					//;		/ Inicia el contador de parpadeos
				goto finref;


toctl_0:
				wreg = Plantilla [dripposd];
				BaBentre10_math();			//call BaBentre10_math
				drp_comph = (uint16_t)(waux * 60);

				drp_fanh = (uint16_t)(wreg * 60);

toctl:
		 	 	 minbrake_load();				// call minbrake_load
		 	 	 edorefri = 0x02;				// Carga el estado de Control
		 	 	 goto todh20;

toctl_inicio:
				retvent = 20;
				//tminstopl = 50;
				//tminstoph = 0;					// Carga el tiempo mínimo de descanso del compresor
				tminstoph = (0*256) + 50;			//;/ Carga el tiempo mínimo de descanso del compresor
				edorefri = 0x02;				// Carga el estado de Control
				goto todh20;


todh:      		cntdpyev = 0;									// Cancela el desplegado de Tevaporador
				ldadaptivo();	//call ldadaptivo				// Carga tiempo total de interdeshielo en segundos
				wreg = Plantilla [timedh];//ld	A,dhmode;		// Toma el tiempo de duración del deshielo en minutos
				if(Plantilla [dhmode] != 0x01 ){//cp A,#$01;		/// ¿Deshielo por gas caliente?
					goto todh1;
				}
				BaBentre10_math();					//call BaBentre10_math
				goto todh2;

todh1:			STM8_16_X =  wreg * 60;			//Plantilla [timedh] *  Número de segundos por minuto
				resulh = highByte(STM8_16_X);
				resull = lowByte(STM8_16_X);		//ldw     resulh,X
todh2:   		//durdhl = resull;
         		//durdhh = resulh;
				durdhh = (resulh*256) + resull;
				edorefri = 0x03;						//;	/ Carga el estado de Autodeshielo
todh20:    	 	minfunc = 0x04;						/// 4 segundos de función activa como mínimo

finref:



refrigera_j08:
//;----------------------------------------------------------------------------------------
//; En equipo medico falla de voltaje no apaga compresor
//;finref_1:				btjt		trefst,#f_hv,refrigera_j09;	/ Hay alarma de voltaje?
//;								btjf		trefst,#f_lv,refrigera_j10;
//;refrigera_j09:	bres		GPIOR0,#f_comp;	/ Sí, Apaga el compresor
//;								call		minbrake_load
//;----------------------------------------------------------------------------------------
refrigera_j10:
                if(GPIOR0[f_comp]){
                	goto refrigera_j11;
                }
				datled[luzC] = 0; 		// bres		datled,#luzC; / Apaga LED de compresor


				//;/ carga estado actual de la cerradura
				//;bres		GPIOR0,#f_dh
refrigera_j11:
				estado1_Aux = findLastValue((uint32_t)&eeEstado1);
                if(!GetRegFlagState(estado1_Aux,est1LockDr)){ //btjf eeEstado1,#est1LockDr,refrigeraLockDrOFF
                	goto refrigeraLockDrOFF;
                }
                GPIOR0[f_dh] = 1; 			//bset		GPIOR0,#f_dh
refrigeraLockDrOFF:

nocturnoFunc:
               if(cntNoct_H == 0){//ld A,cntNoct_H;  or A,cntNoct_L;		/ terminó el tiempo de Nocturno?
            	   goto endNocturno;
               }
               GPIOR0[f_comp] = 0;				//bres		GPIOR0,#f_comp
               GPIOR0[f_dh] = 0;				//bres		GPIOR0,#f_dh
               //;bres		GPIOR0,#f_lamp
               GPIOR1[f_fan] = 0;					//;manten las cargas apagadas
               Bclear_Clear_trfst(datled, datled,luzC, luzF);
               //BitClear(datled,luzC);			//datled[luzC] = 0;		// apaga la luz de compresor
               //BitClear(datled,luzF);			//datled[luzF] = 0;		// Enciende la luz de compresor
               goto endNoct;

endNocturno:
              if(!flagsa[nocturno]){ //btjf flagsa,#nocturno,endNoct
            	  goto endNoct;
              }

              Bset_Clear_trfst(flagsb,flagsa,f_luzb, 3);

//            BitSet(flagsb,f_luzb);				// bset		flagsb,#f_luzb
//            BitClear(flagsa,3);					// bres		flagsa,#3			// Cancela modo nocturno
              Bset_Clear_trfst(flagsC,datled,f_ahorro1, 4);

//            BitSet(flagsC,f_ahorro1);				// bset    flagsC,#f_ahorro1;			/ activa bandera de ahorro1
//            BitClear(datled,4);					// bres		datled,#4;0x10;	/ Apaga el led del modo nocturno

              //t_ahorro1_L = 0;
              t_ahorro1_H = 0;
              //t_ahorro2_L = 0;
              t_ahorro2_H = 0;						//;			/ provoca entrada a modo ahorro2
endNoct:
			estado1_Aux = findLastValue((uint32_t)&eeEstado1);
			if(GetRegFlagState (estado1_Aux, est1Refri)){ //btjt eeEstado1,#est1Refri,refriON
            	 goto refriON;
             }
refriOFF:
		   st_refri();
//           GPIOR0[f_comp] = 0;					//bres		GPIOR0,#f_comp
//           GPIOR0[f_dh] = 0;					//bres		GPIOR0,#f_dh
//           GPIOR0[f_lamp] = 0;					//bres		GPIOR0,#f_lamp
//           //;mov			colorPredef,#7;				/ Si el estado de la lámpara es Off apaga la lámpara vía comunicación
//           GPIOR1[f_fan] = 0;					//bres		GPIOR1,#f_fan		;manten las cargas apagadas
//           minbrake_load();		//call minbrake_load
//           edorefri = 0x02;						/// manten equipo en estado de Control
//           ldadaptivo();		//call ldadaptivo					/// manten cargado el tiempo entre deshielos
      	   //datled = 0;					// manten indicadores de display apagados
           for(uint8_t k=0; k<8; k++){
        	   datled[k] = 0;
           }
           //datdig1 = 0x1F;				// Apaga los dígitos
           //datdig2 = 0x1F;				// Apaga los dígitos
refriON:
         if(!flagsBattery[batON]){//btjf flagsBattery,#batON,ac_OK
        	 goto ac_OK;
         }
         st_refri();
//         GPIOR0[f_comp] = 0;			//bres		GPIOR0,#f_comp
//         GPIOR0[f_dh] = 0;				//bres		GPIOR0,#f_dh
//         GPIOR0[f_lamp] = 0;			//bres		GPIOR0,#f_lamp
//         GPIOR1[f_fan] = 0;				//bres		GPIOR1,#f_fan		;manten las cargas apagadas
//         minbrake_load();				//call minbrake_load
//         edorefri = 0x02;				//;	/ manten equipo en estado de Control
//         ldadaptivo();					//call		ldadaptivo;			/ manten cargado el tiempo entre deshielos
         //	;clr			datled;					/ manten indicadores de display apagados

         timeRstBLE = 1;					// manten modulo wifi en reset
         flagsC[f_spReached] = 1; 			///bset	flagsC,#f_spReached; / indica que el setpoint fue alcanzado para permitir fallas de temperatura

		//	;mov			datdig1,#$0B;		/ Apaga los dígitos
		//	;mov			datdig2,#$0A;		/ Apaga los dígitos

ac_OK:
         goto main_refrigera;

// ;**************************************************************************************************
//        ;Rutinas del proceso Refrigera
//;**************************************************************************************************
indica:
          fauto[3] = 0; 	//bres fauto,#3

          Load_ret1();			// Carga el retardo para entrar a nocturno con luz.
          Load_ret2();			// Carga retardo de tiempo permitido con puerta abierta.
          Bclear_Clear_trfst(trefst, trefst,6, 7);
//        BitClear(trefst,6); 	// bres		trefst,#6;0xC0;	/ Cancela indicaciones de fuera de rango de voltaje
//        BitClear(trefst,7);	// bres		trefst,#7;0xC0;	/ Cancela indicaciones de fuera de rango de voltaje
          cntobscu = 0x02;		// Carga el contador de tiempo de obscuridad con 2 segundos
          wreg = 0x05;

          if(!(fauto[6])){ //btjf fauto,#6,no_change_time
        	  goto no_change_time;
          }
          wreg = 0x01;

no_change_time:
			retvent = wreg;				//mov			retvent,wreg;	/ Carga el retardo de encendido del ventilador con 5 segundos
			interdhh = (uint16_t)wreg;			//mov			interdhl,wreg;	/ Carga 5 segundos para pasar a deshielo
			blink_640();				//call blink_640			/// En wreg quedan los múltiplos de 640 ms

			//ldw	X,cntblkh
			if(cntblkh < 100){//cpw X,#100
				goto indi_off; //jrult	indi_off
			}
			if(cntblkh <= 250){
				goto Indi_int; //jrule	Indi_int
			}
			if(cntblkh < 300){
				goto indi_off; //rult indi_off
			}
			if(cntblkh < 450){
				goto Indi_time;//rult Indi_time
			}
			if(cntblkh < 500){
				goto indi_off; //rult indi_off
			}
			if(cntblkh < 650){
				goto Indi_sp; //rult Indi_sp
			}
			if(cntblkh < 700){
				goto indi_off; //rult indi_off
			}
			if(cntblkh < 850){
				goto Indi_dif; //rult Indi_dif
			}

indi_off:
           goto defindi;

           // MAN_paso 1 de indicacion parametros
Indi_int:	 //mov			tempo2,interdh;	/ Muestra interdeshielo en horas
             //soloent (Plantilla [interdh]);	//call soloent		;	/ Muestra interdeshielo en horas
			soloent (findLastValue((uint32_t) &eePlantilla[interdh]));
             //op_menu (eePlantilla [eeD1_Msg1], eePlantilla [eeD2_Msg1]);
			op_menu (findLastValue((uint32_t) &eePlantilla[eeD1_Msg1]) , findLastValue((uint32_t) &eePlantilla[eeD2_Msg1]));
			//datdig1 = eePlantilla [eeD1_Msg1];		//"U"
             //datdig2 = eePlantilla [eeD2_Msg1];		//"A"
             //datled = 0;			//clr	datled
             for(uint8_t k=0; k<8; k++){
            	 datled[k] = 0;
             }
             goto defindi;

//------------------------------------------------------------------------
Indi_time:
            //tempo2 = Plantilla[timedh];		//mov			tempo2,timedh;	/ Toma el tiempo de duración del deshielo en minutos
            //if(Plantilla[dhmode] != 1 ){  /// ¿El deshielo es por gas caliente?
            if(findLastValue((uint32_t) &eePlantilla[dhmode]) != 1){
				goto indica45;
            }
            //soloent(Plantilla[timedh]);		//call soloent
            soloent( findLastValue((uint32_t) &eePlantilla[timedh]) );
            datled[dp] = 1;					//;	/ Enciende el punto
            goto defindi;
indica45:   // tempo1 = 0;
            convad10(Plantilla[timedh]);			//call convad10

            //;cambia mensajes
            //A,eeprotype
            //if(eePlantilla[eeprotype] == 1){//cp A,#1
            if(findLastValue((uint32_t) &eePlantilla[eeprotype])){
            	goto indica_110v; //jreq indica_110v
            }
            //if(eePlantilla[eeprotype] == 2){
            if(findLastValue((uint32_t) &eePlantilla[eeprotype]) == 2){
            	goto indica_220v; //jreq indica_220v
            }

indica_na:
            wreg = 0x26;				//"-"
            waux = 0x26;				//"-"
            goto indica_volt;

indica_110v:
             wreg = 0x1F;				//" "
             waux = 0x0B;				//"b"
             goto indica_volt;

indica_220v:
              wreg = 0x1F;				//" "
              waux = 0x0A;				//"A"
              goto indica_volt;

indica_volt:
			 op_menu (wreg, waux);
             //datdig1 = wreg;
             //datdig2 = waux;
             //datled = 0;		// clr datled
             for(uint8_t k=0; k<8; k++){
            	 datled[k] = 0;
             }             
             goto defindi;

             // MAN_paso 3 de indicacion parametros
Indi_sp:    sp_dpy();					//call sp_dpy				/ Despliega el Set Point que esta operando
            //tempo2 = versionFirm1;	/// primer dato de version
			soloent(versionFirm1);		//call soloent
			//datled = 0;					// clr datled
			for(uint8_t k=0; k<8; k++){
				datled[k] = 0;
			}
			goto defindi;

Indi_dif:	dif_dpy();		//call dif_dpy		// Despliega el Diferencial que este operando
            //tempo2 = versionFirm2;			// segundo dato de version
            soloent(versionFirm2);				//call		soloent;
            // datled = 0;							// clr datled
            for(uint8_t k=0; k<8; k++){
            	datled[k] = 0;
            }


defindi:   buildmode();					//call buildmode;				//	/ Construye el modo de cambio de estado para la rutina refrigera
           if(!GetRegFlagState(modo,tinic)){//btjf modo,#tinic,indica_j00;
        	   goto indica_j00;
           }
           goto defind10;				/// No, pregunta por otro modo

indica_j00:
            if(Plantilla [numSens] != 1){ 	// Si el número de sensores con el que trabaja es 1, no preguntes por condiciones iniciales del sensor evaporador
            	goto indica_j00b;
            }
            if(GPIOR0[f_disable]){ //btjt GPIOR0,#f_disable,indica_j01  /// Esta deshabilitado el deshielo al arranque? sí, carga interdeshielo
            	goto indica_j01;		//indica_j01;/ Esta deshabilitado el deshielo al arranque? sí, carga interdeshielo
            }
            goto indica_j02;

indica_j00b:
             if(trefst[f_sda]){ //btjt trefst,#f_sda,indica_j01;
            	 goto indica_j01;
             }
            if(trefst[f_sdc]){ //btjt trefst,#f_sdc,indica_j01
            	goto indica_j01;			//indica_j01;	/ ¿Falla en el sensor de evaporador?
            }

            if(GPIOR0[f_disable]){//btjt GPIOR0,#f_disable,indica_j01
            	goto indica_j01;		//indica_j01;/ Esta deshabilitado el deshielo al arranque?
            }
            if(!GetRegFlagState(modo,limech)){ //btjf modo,#limech,indica_j02;
            	goto indica_j02;		//indica_j02;	/ La temperatura del evaporador esta por debajo del límite establecido?
            }
indica_j01:		ldadaptivo();	// call ldadaptivo
indica_j02:   	//;jp			toctl;			/ Cambia a control
			goto	toctl_inicio;			//jp      toctl_inicio     ;RM_20230419  Para retardo al inicio

defind10:
             if(holdseg != 0){  //tnz holdseg; jrne finindi;	/ ¿Ya es más de 1 segundo?
            	 goto finindi;
             }
             if(trefst[f_sac]){ //btjt trefst,#f_sac,indica_j04
            	 goto indica_j04;
             }
             goto finindi;

indica_j04:
             goto toap;			//	jp			toap;			/ Entra a autoprueba con el conector

finindi:     goto finref;

//;--------------------------------------------------------------------------------------------------
autopru:
            wreg = durautop;
            latchtimeh = (uint16_t)(wreg);		// mov			latchtimel,wreg;,	Copia el tiempo que esta corriendo
            blink_640();						//call blink_640			//call		blink_640;				/ En wreg quedan los múltiplos de 640 ms
            if(wreg != 0){						/// ¿Es el primer ciclo?
            	goto version;					//jrne		version;					/ No, ve a mostrar la version
            }
            fauto[7] = 1;						//bset		fauto,#7
            goto defap;							/// Sí, mantiene apagado todo

version:

			if(fauto[1]){//btjt fauto,#1,fail_det		// Ya se detecto una falla? si, no hagas secuencia
				goto fail_det;			//fail_det;/ Ya se detecto una falla? si, no hagas secuencia
			}
			wreg = num_ver;			//;		/ Versión del Firmware
			soloent1();				//call soloent1
			datled[0] = 1;//BitSet(datled,0);			/// Enciende el punto
			fauto[0] = 1;			/// No muestres fallas de sensor ambiente

           if(durautop <= 16){
        	   goto door_det; //jrule door_det
           }
           if(!HAL_GPIO_ReadPin(PFULLDEF_P5_PORT,PFULLDEF_P5_PIN)){
        	   goto no_op_door;//btjf PD_IDR,#swdoor,no_op_door
           }
           				//no_op_door;	/	¿La puerta está abierta?
           trefst[4] = 1; //bset		trefst,#4;				/	Sí, activa alarma de puerta abierta
           goto ask_fail;
no_op_door:
            goto defap;
door_det:
			if(durautop <= 13){ //jrule do_sec ***
            	goto do_sec;
            }
			datled[1] = 0;					//bres		datled,#1;				/ apaga el signo
            if(!HAL_GPIO_ReadPin(PFULLDEF_P5_PORT,PFULLDEF_P5_PIN)){//btjf PD_IDR,#swdoor,no_det_door;
            	goto no_det_door;				//	¿La puerta está abierta?
            }			//	¿La puerta está abierta?
            fauto[2] = 1;					//bset		fauto,#2;					/ Indica que se abrio la puerta
            datled[1] = 1;		 		//bset		datled,#1;				/ prende el signo
            GPIOR1[f_led] = 0;				// Apaga el led inidcador
no_det_door:
			goto defap;

do_sec:
           	   fauto[0] = 0;		//bres		fauto,#0;					/ Ya se pueden mostrar fallas de sensor ambiente
ask_fail:
			for(int k=0;k<8;k++){
				if(trefst[k] != 0){//tnz  trefst					//;						/ Hay alguna falla ?
           		   goto fail_det;//jrne fail_det
           	   }
           	   if(trefst2[k] != 0 ){//tnz trefst2;					;						/ Hay alguna falla ?
           		   goto fail_det; //jrne fail_det
           	   }
			}

           	   goto ask_do_sec;	//;				/	No, continua con la secuencia

fail_det:
			op_menu (0x0A, 0x0A);
			//datdig1 = 0x0A;
			//datdig2 = 0x0A;
			if(GetRegFlagState (lowByte(cntblkh), 6)){ //btjt	cntblkl,#6,blk_aa0;
				goto blk_aa0;							//blk_aa0;		/ Parpadea cada 640 ms
			}
			op_menu (0x1F, 0X1F);
			//datdig1 = 0x1F;							//;		/ Apaga los dígitos
			//datdig2 = 0X1F;							//;		/ Apaga los dígitos

blk_aa0:
           fauto[1] = 1;	//;					/ indica de hubo falla
           fauto[7] = 1;

           if(trefst[f_hv]){//btjt trefst,#f_hv,dpy_volt_f;		;	/ Hay alarma de voltaje?
        	   goto dpy_volt_f;
           }
           if(!trefst[f_lv]){  //btjf trefst,#f_lv,no_dpy_volt_f;
        	   goto no_dpy_volt_f;
           }

dpy_volt_f:
              fauto[3] = 0;					//bres		fauto,#3
              if(GetRegFlagState(durautop2, 0)){//btjt durautop2,#0,no_dpy_f
            	  goto no_dpy_f;
              }
              fauto[3] = 1;					//bset		fauto,#3

no_dpy_f:
				wreg = voltl;
				if(wreg < 100 ){ //ld a,voltl
					goto under_100_f; //jrult under_100_f
				}
				wreg = wreg - 100;

				if(wreg < 100){
					goto under_100_f;
				}
				wreg = wreg - 100;

under_100_f:
              //ld wreg,a
				soloent1();	//call soloent1
				datled[0] = 0;			//bres		datled,#0;				/ apaga el punto

no_dpy_volt_f:

                goto defap;					// Si, no hagas secuencia

ask_do_sec:
				if(fauto[2]){						// se abrio la puerta? Si, realiza la secuencia
					goto on_compres;
				}
				goto fail_det;

on_compres:
				GPIOR0[f_comp] = 1;			// Enciende el compresor
				datled[luzC] = 1;//BitSet(datled,luzC);			// Enciende el compresor
				//ld A,durautop
				if(durautop <= 11 ){//cp A,#11;
            	 	 goto on_desh;
             	 }
             	 goto defap;

on_desh:
				GPIOR0[f_comp] = 0;					// apaga el compresor
				Bset_Clear_trfst(datled, datled,luzD, luzC);
//            	BitSet(datled,luzD);				// Enciende el deshielo
//				BitClear(datled,luzC); 				// apaga el compresor
				GPIOR0[f_dh] = 1;					// Enciende el deshielo
            	datled[luzD] = 1;// BitSet(datled,luzD);				// Enciende el deshielo
            	if(durautop <= 9){
            		goto on_vent;//jrule on_vent
            	}
            	goto defap;

on_vent:
				GPIOR0[f_dh] = 0;						/// apaga el deshielo
				Bset_Clear_trfst(datled, datled,luzF, luzD);
//				BitSet(datled,luzF);					/// Enciende el ventilador
//				BitClear(datled,luzD);				/// apaga el deshielo
				GPIOR1[f_fan] = 1;					/// Enciende el ventilador
				if(durautop <= 7){
					goto on_lamp; //jrule	on_lamp
				}
				goto defap;

on_lamp:
				GPIOR1[f_fan] = 0;						// apaga el ventilador
				Bset_Clear_trfst(datled, datled,luzN, luzF);
//				BitSet(datled,luzN);					// Enciende el lampara
//				BitClear(datled,luzF);					// apaga el ventilador
				GPIOR0[f_lamp] = 1;					// Enciende el lampara
				if(durautop <= 05){
					goto dpy_volt; //jrule	dpy_volt
				}
				goto defap;

dpy_volt:
				GPIOR0[f_lamp] = 0;					// apaga el lampara
				datled[luzN] = 0;// BitClear(datled,luzN); 				// apaga el lampara
				wreg = voltl;
				if(wreg < 100){
					goto under_100; //jrult	under_100
				}
				wreg = wreg - 100;

				if(wreg < 100){
					goto under_100;
				}
				wreg = wreg - 100;

under_100:
				//ld	wreg,a
				soloent1();			//call soloent1
				datled[0]= 0; //BitClear(datled,0);			// apaga el punto

				fauto[6] = 1;
				fauto[7] = 0;

defap:			buildmode();			//call		buildmode;		/ Construye el modo de cambio de estado
				if(!GetRegFlagState(modo,0)){ //btjf modo,#0,finap;
					goto finap;
				}
defap1:			goto toindi;						// Cambia al estado de indicación de parámetros
finap:			goto finref;
//;--------------------------------------------------------------------------------------------------
control:
				datled[luzF] = 0;			//bres		datled,#luzF
				//wreg = lowByte(drp_fanh);		//drp_fanl;
				if(drp_fanh == 0){
					goto ask_door_fan; //jreq ask_door_fan
				}
				goto fan_off;

ask_door_fan:
               if(GetRegFlagState(Plantilla[logicos2],ventDoorOFF)){ //btjt logicos2,#ventDoorOFF,ask_fan_on	/ sí los parámetros de configuración lo indican ignora el control de ventilador por puerta
            	   goto ask_fan_on;
               }

               if(cnt_pta_fan == 0){//tnz cnt_pta_fan;			// El contador de puerta abierta llego a cero?
            	   goto ask_fan_on; //jreq ask_fan_on;			// Si, ignora el control de puerta.
               }

               if(retvent == 0){ //tnz retvent
            	   goto ask_fan_on;//jreq ask_fan_on;			// Si,el ventilador puede ser encendido
               }
               goto fan_off; //jra fan_off						// No, manten apagado el ventilador

ask_fan_on:
				if(!portX[rel_co]){			//	btjf PA_ODR,#rel_co,fan_control
					goto	fan_control;		// ¿Esta encendido el compresor?    RM_20220714 Cambio en pin de compresor pasa de PC a PA
				}
				if(ret_fan_co != 0){ //tnz ret_fan_co
					goto fan_off; //jrne fan_off
				}
				goto fan_on;//jra fan_on			// Si, el ventilador permanece encedido

fan_control:
				wreg = Plantilla [cicloFd];			// Carga ciclo de ventilador diurno
				BaBentre10_math();					//  BaBentre10
				if(waux != 0){						// esta en cero la parte alta???
					goto ciclof_00;					// No, pregunta por parte baja
				}
				goto fan_off;							// Si, manten apagado el ventilador
ciclof_00:		if(wreg != 0){//tnz wreg;					// esta en cero la parte baja ??
					goto ciclof_01; //jrne ciclof_01;		// No, continua con el cicleo
				}
				goto fan_on; //jra	fan_on;					// Si, manten encendido el ventilado

ciclof_01:		if(!portX[rel_fn]){				 //btjf		PA_ODR,#rel_fn,ciclo_off; Manuel 01-MAR-2022 Esta encendido el ventilador????
					goto	ciclo_off; 				//Manuel 01-MAR-2022 Esta encendido el ventilador????
				}
				goto ciclo_on;					/// Si, ciclo de encendido

ciclo_off:
				if(toff_fan == 0){ //tnz toff_fan;	// ya termino ciclo de apagado?
					goto ld_ciclo_on; //jreq ld_ciclo_on;
				}
				goto fan_off;						// No, manten apagado el ventilador

ld_ciclo_on:
				if(ton_fan != 0){ //tnz ton_fan
					goto fan_on; //jrne fan_on
				}
				ton_fan = waux; //mov ton_fan,waux;		// Si, carga tiempo de encendido
				goto fan_on; //jra	fan_on;					// cambia a encendido

ciclo_on:
				if(ton_fan == 0){ //tnz	ton_fan;			// ya termino ciclo de apagado?
					goto ld_ciclo_off; //jreq ld_ciclo_off;
				}
				goto fan_on;//jra fan_on;					// No, manten apagado el ventilador

ld_ciclo_off:
				toff_fan = wreg;//mov	toff_fan,wreg;		// Si, carga tiempo de encendido
				goto fan_off;//jra fan_off;					// cambia a encendido

fan_on:
				GPIOR1[f_fan] = 1; 		//bset		GPIOR1,#f_fan;;	/ Enciende el ventilador
				datled[luzF] = 1;//bset datled,#luzF

fan_off:


ctl10:
               //Manejo a 16bits 	//latchtimel = interdhl;//mov latchtimel,interdhl
               latchtimeh = interdhh;  //mov latchtimeh,interdhh;

               if(trefst[f_sac]){//btjt	trefst,#f_sac,control_j02;		/ Es sensor de ambiente esta en corto?
            	   goto control_j02;
               }
               if(!trefst[f_saa]){ //btjf	trefst,#f_saa,control_j03;	/ No, entonces el sensor de ambiente esta abierto?
            	   goto control_j03;
               }

control_j02:	//manuel reduc...     jp			refrioff;		/ Si, apaga el refrigerador
				goto refrioff; //jra refrioff;    / Si, apaga el refrigerador

control_j03:	//
				if(trefst2[f_co_ex]){ //btjt trefst2,#f_co_ex,defctl
             		 goto defctl;
				}

//------------------------------------------	Hyst Control --------------------------------------------------------------
ctl13:
				asm ("nop");

				//call ld_tdev_to_wreg
				if((int16_t)tdev_to_Word() < (int16_t)limsup_w){		// tdev - limsup ;	/ tdev > ó = limsup?
					goto ctl20; //jrslt	ctl20		// No,Revisa el otro límite
				}
ctl15:			//	ld			A,tminstopl;
				//	or			A,tminstoph;		/ Ya terminó el tiempo mínimo de descanso?
				if(tminstoph != 0){						// Ya terminó el tiempo mínimo de descanso?
					goto defctl;					// No, espera
				}
				// mov			wreg,drp_compl
				// ld			A,drp_comph
				// or			A,wreg
				if(drp_comph != 0){
					goto defctl;
				}
				goto on_comp;						// Enciende el compresor

ctl20:         	//ldw			X,liminf_w  ;// manuel_math_change//	liminfl;
				if((int16_t)liminf_w < (int16_t)tdev_to_Word()){		/// liminf - tdev  ;	/ tdev < ó = liminf?
					goto equal;
         	 	}

				Bset_Clear_trfst(flagsC, datled,f_spReached, 6);

//				BitSet(flagsC,f_spReached);			//bset	flagsC,#f_spReached
//				BitClear(datled,6);			//		bres		datled,#6;0x40		; Desactiva bandera de modo pull down  >> IJG JULIO 2012



rest:			if(portX[rel_co]){	//btjt		PA_ODR,#rel_co,control_j04;	/ Esta encendido el compresor?      RM_20220714 Cambio en pin de compresor pasa de PC a PA
					goto control_j04;
				}
        		goto defctl;			//jra			defctl;			/ No, deja correr el tiempo de descanso

control_j04:
				minbrake_load();	//call minbrake_load
				goto defctl;		// Si, termina con compresor apagado

equal:			if(portX[rel_co]){		//btjt		PA_ODR,#rel_co,control_j05;	/ Deja el compresor como está en ese momentoRM_20220714 Cambio en pin de compresor pasa de PC a PA
					goto control_j05;
				}

        		goto defctl;

control_j05:
on_comp:		GPIOR0[f_comp] = 1;			//	bset		GPIOR0,#f_comp;	/ Enciende el compresor
				datled[2] =1;			//	bset		datled,#2;0x04;	/ Enciende la luz de compresor
				goto defctl;

refrioff:
				goto rest;

defctl:
//;------------------------------------------------------------------------------------------
//;-------------Alarma de deficiencia--------------------------------------------------------

				//; En equipo medico deahbilita función de deficiencia (tercer sensor se usa para vacuna)
				Plantilla [timeDefi] = 0; //clr timeDefi **************************************************

				if(Plantilla[timeDefi] == 0 ){ /// si el tiempo de deficiencia es 0, se cancela la detección de falla "A"
					goto noChkTimeDefi; //jreq	noChkTimeDefi
				}

				if(portX[rel_co]){ //btjt		PA_ODR,#rel_co,chkTimeDefi; / sí el compresor está encendido checa el tiempo para realizar mediciones de deficiencia, si está apagado manten cargado el tiempode deficiencia         RM_20220714 Cambio en pin de compresor pasa de PC a PA
					goto chkTimeDefi;
				}

				//mov			wreg,timeDefi;	/ Toma el tiempo de deficiencia
				//ldw			X,#$003C;				/ Número de segundos por minuto
				//ld      A,wreg
				//mul			X,A;						/ Multiplicalos
				STM8_16_X = Plantilla[timeDefi] *60;

				if( STM8_16_X == cntDefi_H ){	// sí el contador viene con un valor distinto, entonces se apago compresor antes de que el contador llegara a cero
					goto loadDefiCnts;
				}

				if(Plantilla[numSens] == 3){ 	// se está trabajando con 3 sensores?
					goto chkTimeDefi_3s_01;		// Sí, realiza mediciones para 3 sensores unicamente
				}
				goto chkTimeDefi_1s_02;			//jra			chkTimeDefi_1s_02;	/ No, realiza la segunda medición de 1 sensor

loadDefiCnts:
				//ldw     resulh,X
				//mov			cntDefi_L,resull;
				//mov			cntDefi_H,resulh;	/ Carga el tiempo de deficiencia en segundos
				cntDefi_H = STM8_16_X;
				//subw		X,#1
				//ldw			cntDefi1_H,X;		/ establece el tiempo de deficiencia inicial para sensor 1
				cntDefi1_H = STM8_16_X - 1;				// subw		X,#1
				goto noChkTimeDefi;

chkTimeDefi:
				if(Plantilla[numSens] == 3){			/// se está trabajando con 3 sensores?
					goto chkTimeDefi_3s;				/// realiza mediciones para 3 sensores unicamente
				}

chkTimeDefi_1s:
                  if(trefst2[f_s3short]){	//btjt		trefst2,#f_s3short,noAlarmDefi;	Si hay alguna falla con el sensor ambiente cancela falla de deficiencia
                	  goto noAlarmDefi;
                  }
                  if(trefst2[f_s3open]){//btjt trefst2,#f_s3open,noAlarmDefi
                	  goto noAlarmDefi;
                  }

                  //ldw	 X,cntDefi_H
                  if(cntDefi_H != cntDefi1_H){ //cpw			X,cntDefi1_H;			/ es tiempo de realizar primera medición ?
                	  goto chkTimeDefi_1s_01; //jrne chkTimeDefi_1s_01
                  }
                  //ldw			X,tdevl;					/ Sí, carga temperatura ambiente
                  tempDefi1 = tdev_to_Word();				// ldw	tempDefi1,X;
                  goto finChkTimeDefi;						//jra			finChkTimeDefi;		/ continua

chkTimeDefi_1s_01:
                    //ldw X,cntDefi_H
                    if(cntDefi_H != 1){ //cpw X,#1
                    	goto finChkTimeDefi;	//jrne		finChkTimeDefi;		/ es momento de realizar las últimas mediciones ? No,continúa
                    }
chkTimeDefi_1s_02:
					STM8_16_X = tempDefi1;							//ldw			X,tempDefi1;			/ carga temperatura ambiente inicial (inicio del ciclo de compresor)
					STM8_16_X = (int16_t)STM8_16_X - (int16_t)tdev_to_Word();			//subw		X,tdevl;					/ resta temperatura ambiente actual (pasado el tiempo timeDefi de compresor encendido)
                    if((int16_t)STM8_16_X >= (int16_t)TwoByteInArrayToWord (&Plantilla[difDefi_H])){ // cpw X,difDefi		/	el diferencial es mayor al definido para alarma? Sí, sal sin alarma de deficiencia
                    	goto noAlarmDefi; //jrsge noAlarmDefi;
                    }
                    goto alarmDefi;									//jra			alarmDefi;				/ No, activa alarma de deficiencia



chkTimeDefi_3s:
                   if(trefst2[f_s3short]){ //btjt trefst2,#f_s3short,chkTimeDefi_1s;	Si hay alguna falla con el sensor de salida de aire realiza mediciones con un solo sensor
                	   goto chkTimeDefi_1s;
                   }

                   if(trefst2[f_s3open]){ //btjt trefst2,#f_s3open,chkTimeDefi_1s
                	   goto chkTimeDefi_1s;
                   }

                   //ldw X,cntDefi_H
                   if(cntDefi_H != 1){ //cpw X,#1
                	   goto finChkTimeDefi; //jrne finChkTimeDefi			/ realiza mediciones solo en el último segundo del tiempo de deficiencia
                   }

chkTimeDefi_3s_01:
					STM8_16_X = tdev_to_Word();			// carga temperatura ambiente
					STM8_16_X = (int16_t)STM8_16_X - (int16_t)tret_w;		// subw		X,tret_w;				/ resta temperatura de salida de aire
                    if((int16_t)STM8_16_X >= (int16_t)TwoByteInArrayToWord (&Plantilla[difDefi_H])){	//jrsge		noAlarmDefi;		/	el diferencial es mayor al definido para alarma? Sí, sal sin alarma de deficiencia
                    	goto noAlarmDefi;//jrsge noAlarmDefi;
                    }


alarmDefi:
					trefst2[f_defi] = 1; //bset	trefst2,#f_defi;			// activa alarma de deficiencia
                    interdhh = 0;//clr interdhh
                    //interdhl = 0;//clr interdhl;				/;				// limpia tiempo de interdeshielo para forzar un deshielo en caso de que la temperatura lo permita (t9)
                    //latchtimel = interdhl;  //mov latchtimel,interdhl
                    latchtimeh = interdhh; //mov latchtimeh,interdhh
                    goto finChkTimeDefi; //jra	finChkTimeDefi
noAlarmDefi:

					trefst2[f_defi]=0; //bres trefst2,#f_defi;		/// borra alarma de deficiencia

finChkTimeDefi:

noChkTimeDefi:

//;------------------------------------------------------------------------------------------

					//call ld_tdev_to_wreg
					STM8_16_X = tdev_to_Word();	//ldw     X,waux
					if((int16_t)STM8_16_X < (int16_t)TwoByteInArrayToWord (&Plantilla[defrResetTemp_H])){ //cpw X,defrResetTemp;
						goto no_reset_interdh; //jrslt no_reset_interdh;		/ sí, pregunta si ya termino el interdeshielo
					}
					ldadaptivo();		//call ldadaptivo				/// no, carga de nuevo el tiempo para entrar a deshielo (mínimo y máximo)
					goto defct10; //jra defct10;				//	/ omite entrada a deshielo por tiempo o temperatura

no_reset_interdh:

                  //ld A,numSens
                  if(Plantilla[numSens] == 1){ //cp A,#1
                	  goto noDeshXTemp;//jreq noDeshXTemp;   / Si el número de sensores con el que trabaja es 1, no revises temperatura de evaporador para entrar a deshielo
                  }
				//	;------------------------------------------------------------------------------------------------------
				//	;---------- Agrega condición de temperatura ambiente para forzar deshielo por temperatura evaporador
                  //wreg = tdevf;//mov wreg,tdevf;
                  //waux = tdevl;//mov waux,tdevl
                  //ldw     X,waux
                  STM8_16_X = (uint16_t)(tdevl * 256) + (uint16_t)(tdevf);

                  if(trefst[f_saa]){ //btjt	trefst,#f_saa,noDeshXTemp;
                	  goto noDeshXTemp;
                  }
                  if(trefst[f_sac]){ //btjt	trefst,#f_sac,noDeshXTemp;		/ ¿Falla en el sensor ambiente? Sí, no entres a deshielo por temperatura.
                	  goto noDeshXTemp;
                  }

                  if((int16_t)STM8_16_X >= (int16_t)TwoByteInArrayToWord (&Plantilla[defrStartTempAmb_H])){ //cpw	X,defrStartTempAmb;   TEMP_ambiente < defrStartTempAmb programado
                	  goto noDeshXTemp; //jrsge	noDeshXTemp
                  }
                  //;------------------------------------------------------------------------------------------------------

					//mov			wreg,tevaf;
					//mov			waux,teval
					//ldw     X,waux
                  STM8_16_X = (uint16_t)(teval * 256) + (uint16_t)(tevaf);

                  if(trefst[f_sda]){//btjt	trefst,#f_sda,noDeshXTemp;
                	  goto noDeshXTemp;
                  }
                  if(trefst[f_sdc]){//btjt	trefst,#f_sdc,noDeshXTemp		/ ¿Falla en el sensor de evaporador? Sí, no entres a deshielo por temperatura.
                	  goto noDeshXTemp;
                  }

                  if((int16_t)STM8_16_X >= (int16_t)TwoByteInArrayToWord (&Plantilla[defrStartTemp_H])){//	cpw X,defrStartTemp;	TEMP_congelador < defrStartTemp programado
                	  goto noDeshXTemp;//jrsge noDeshXTemp
                  }
                  goto todh; //jp todh



noDeshXTemp:
				buildmode();		//call buildmode	/ Construye el modo de cambio de estado para la rutina refrigera
				if(!GetRegFlagState(modo, 0)){//btjf modo,#0,control_j06;		/ Es el modo xxxxxxx1?
            	   goto control_j06;
				}
				goto todh; //jp			todh;			/ Si, cambia al estado de deshielo automático

control_j06:
defct10:
				// Ya transcurrió el mínimo tiempo de función actva?
				if(minfunc != 0){ //tnz minfunc
					goto finctl; //jrne	finctl;
				}

				if(!flags_accMenu){ //btjf flags_accMenu,#0,control_j07	/ ¿deshielo forzado por selección de menú?
            		goto control_j07;
            	}
				flags_accMenu = 0;//  //bres flags_accMenu,#0
            	goto todh; //	jp			todh;			/ Si, cambia al estado de deshielo

control_j07:

finctl:       	 goto finref;
//;--------------------------------------------------------------------------------------------------
autodesh:
				Bset_Clear_trfst(datled, datled,3,luzF);
//				BitSet(datled,3);				//bset datled,#3					bset		datled,#3;0x08;	/ Enciende la luz de deshielo
//				BitClear(datled,luzF);			//bres datled,#luzF

				////ld A,numSens
				if(Plantilla[numSens] == 1){
					goto deshXvent;				/// Si el número de sensores con el que trabaja es 1, el deshielo siempre es por ventilador (o natural)
				}

				if(Plantilla[dhmode] != 0x01){	// Es deshielo por válvula?
					goto deshie20;				/// No
				}
				GPIOR0[f_comp] = 1;				//bset		GPIOR0,#f_comp;	/ Enciende el compresor para que circule el gas
deshie20:       if(Plantilla[dhmode] != 0){		// Es deshielo por circulación de aire?
					goto deshie25;				//;		/ No, acciona el deshielo
				}

deshXvent:
				if(GetRegFlagState(Plantilla[logicos2], ventDoorOFF)){ //btjt		logicos2,#ventDoorOFF,deshie24; / sí los parámetros de configuración lo indican ignora el control de ventilador por puerta
					goto deshie24;
				}
				if(retvent != 0){ //tnz retvent
					goto deshie30; //jrne		deshie30;		/ No, espera
				}
deshie24:
             	 GPIOR1[f_fan] = 1;				/// bset		GPIOR1,#f_fan;	/ Enciende la Función de ventilador
             	 datled[luzF] = 1;			// 	bset		datled,#luzF
             	 goto deshie30;					//	jra			deshie30

deshie25:        GPIOR0[f_dh] = 1;				//bset		GPIOR0,#f_dh;	/ Enciende la Función de deshielo


deshie30:        holdseg = 0;						//clr			holdseg;		/ Para iniciar desde cero segundos
				 cnthold = Plantilla[timeHold];		// mov			cnthold,timeHold;		/ Carga el contador con tiempo de bloqueo despues de deshielo (15 minutos)
				 //latchtimel = durdhl;				//mov			latchtimel,durdhl;
				 latchtimeh = durdhh;				//mov			latchtimeh,durdhh;/ Copia el tiempo que esta corriendo para el estado vigente
defadh:	         buildmode();				//call		buildmode;		/ Construye el modo de cambio de estado para la rutina refrigera
					//ld			A,#$13;
					//and			A,modo;
				 if((0x13 & modo ) == 0){
					goto defadh10;				//jreq		defadh10;		/ No, revisa otra condición
				 }
				 goto toctl_0;					//jp			toctl_0;			/ Cambia al estado de control

defadh10:	     if(minfunc != 0){
					goto finadh;				//jrne		finadh;			/ No, esperate
             	 }

finadh:		     goto finref;
//;------------------------------------------------------------------------------------------


main_refrigera:


}

void st_refri (void)
{
    GPIOR0[f_comp] = 0;					//bres		GPIOR0,#f_comp
    GPIOR0[f_dh] = 0;					//bres		GPIOR0,#f_dh
    GPIOR0[f_lamp] = 0;					//bres		GPIOR0,#f_lamp
    //;mov			colorPredef,#7;				/ Si el estado de la lámpara es Off apaga la lámpara vía comunicación
    GPIOR1[f_fan] = 0;					//bres		GPIOR1,#f_fan		;manten las cargas apagadas
    minbrake_load();		//call minbrake_load
    edorefri = 0x02;						/// manten equipo en estado de Control
    ldadaptivo();		//call ldadaptivo					/// manten cargado el tiempo entre deshielo
}
