



#include "main.h"
#include "customMain.h"
#include "voltmetro.h"




void voltmetro(void){

				if(GPIOR0[f_volt]){// BTJT   GPIOR0,#f_volt,voltmetro1    ;¿Protección de voltaje activada?
					goto voltmetro1;
				}
				goto fin_voltmetro; // JP     fin_voltmetro    ;NO, Termina el proceso voltmetro

voltmetro1:
				cntvolt++; //		INC    cntvolt          ;Un milisegundo más
				//LD A,cntvolt
				if(cntvolt >= 25){//CP A,#25
					goto adq_muestra; //JRUGE  adq_muestra
				}
				goto verifica_limites; //  jp     verifica_limites ;NO, Realiza comparaciones con el dato anterior



adq_muestra:
				cntvolt = 0; // CLR    cntvolt          ;limpia contador

				wreg = volt_trms;//	mov     wreg,volt_trms   ;Es el voltaje obtenido del proceso de muestreo
	                //ld A,wreg
				if(wreg != 0xFF){//cp A,#$FF
					goto no_255_dat;//jrne no_255_dat
				}


				//ld A,cm_h255
				if(cm_h255 == 2){//cp A,#2
					goto fail_tvolt;//jreq fail_tvolt
				}
				cm_h255++;//inc	cm_h255
				goto verifica_limites;//jra verifica_limites;

no_255_dat:
				cm_h255 = 0;//clr cm_h255
				//ld	A,wreg
				if(wreg > 50){//cp	A,#50;
					goto carga_muestra_V;//jrugt carga_muestra_V;
				}
				//ld A,cm_lo50
				if(cm_lo50 == 2){//cp A,#2;
					goto fail_tvolt;//jreq fail_tvolt
				}
				cm_lo50++;//inc cm_lo50
				goto verifica_limites;//jra verifica_limites;

fail_tvolt:
bad_value:
				trefst2[f_tvolt] = 0;	//bres		trefst2,#f_tvolt;		no hay error en la tarjeta de medición de voltaje
				//ld A,edorefri
				if(edorefri != 1){//cp A,#1
					goto hide_fail_eu;//jrne hide_fail_eu
				}
				trefst2[f_tvolt] = 1;//BitSet(trefst2, f_tvolt);	//bset		trefst2,#f_tvolt;		Indica error en la tarjeta de medición de voltaje


hide_fail_eu:
				cntmuesv = 0; //clr cntmuesv
				voltl = 115;//mov voltl,#115;
				//ld A,protype;
				if(Plantilla[protype] != 0x02){//cp A,#$02;
					goto no_ld_210; // jrne no_ld_210;
				}
				voltl = 210;//mov			voltl,#210;					Carga el promedio de muestras con 210
no_ld_210:
				goto verifica_limites; //jra verifica_limites

carga_muestra_V: //************************************************************
				cm_lo50 = 0; //clr cm_lo50;
				trefst2[f_tvolt] = 0;//bres		trefst2,#f_tvolt;		no hay error en la tarjeta de medición de voltaje
						//LDW X,#voltram
	                    //LD A,XL
	                    //ADD A,cntmuesv
	                	//LD XL,A
	                	//LD A,wreg
	                    //LD (X),A
				voltram[cntmuesv] = wreg;
				cntmuesv++;//  INC    cntmuesv         ;Una muestra más, incrementa contador de muestras de voltaje
	                    //ld A,cntmuesv
				if(cntmuesv >= 8){//cp A,#8
					goto promedio;//JRUGE promedio
				}
				goto verifica_limites;//jra verifica_limites
	                    //***********************************************************

promedio:
				cntmuesv = 0;//clr cntmuesv //****************************
				//LDW X, #voltram
				//call prom8m1b
				voltl = prom8m1b (&voltram[0], 8);//	MOV    voltl,resull     ;Carga el resultado del promedio en su registro

verifica_limites:

verifica_alto:
				if(trefst[f_hv]){//BTJT trefst,#f_hv,hist_vla
					goto hist_vla;
				}
				STM8_A = volt_max;	//	LD     A,volt_max            ;NO, Carga el dato de límite de alto voltaje
				goto comp_vla;//jra comp_vla

hist_vla:		//;-----------------------Falla por ALTO Voltaje activa
				STM8_A = volt_hmax;			 //LD     A,volt_hmax    ;SI, Carga el dato Histeresis Alto para comparar

comp_vla:		//;-----------------Compara para verificar voltaje ALTO
				if(STM8_A > voltl ){ //CP A,voltl
					goto verifica_bajo;//JRUGT verifica_bajo
				}
voltaje_alto:		//;-----------------------------  Voltaje de línea ALTO
					if(valfhvl == 0){//cp A,#0						// ;¿Ha transcurrido el tiempo para validación de falla
						goto falla_VLalto;//JREQ falla_VLalto
					}
					goto fin_voltmetro;//jra fin_voltmetro

falla_VLalto:
				   Bset_Clear_trfst(trefst,trefst,f_hv, f_lv);
//				   BitSet(trefst,f_hv);		//	BSET   trefst,#f_hv       ;Activa bandera por ALTO VOLTAJE
//				   BitClear(trefst,f_lv); 	//	BRES   trefst,#f_lv       ;Cancela bandera por BAJO VOLTAJE
				   goto tiempo_libfalla;// jra     tiempo_libfalla   ;IMPORTANTE: Carga retardo para liberar falla

verifica_bajo:		//;----------------------------  Verifica voltaje BAJO
	               valfhvl =  Plantilla[protval];//MOV    valfhvl,protval
	               if(edorefri != 1){//  ;		/ En autoprueba deja el led como está.
	            	   goto no_auto_00;//jrne no_auto_00
	               }
	               valfhvl = 2;//MOV    valfhvl,#2

no_auto_00:
					if(trefst[f_lv]){//BTJT trefst,#f_lv,hist_vlb
						goto hist_vlb; //
					}
					STM8_A = volt_min;		//LD     A,volt_min         ;NO, Carga el dato de límite de bajo voltaje
					goto comp_vlb;//  jra     comp_vlb           ;Realiza comparación con voltaje de línea bajo

hist_vlb:			//;--------------------  Falla por BAJO Voltaje activa
					STM8_A = volt_hmin;// LD     A,volt_hmin     ;Carga el dato Histeresis Bajo para comparar

comp_vlb:			//;--------------  Compara para verificar voltaje BAJO
	               if(STM8_A < voltl ){//cp A,voltl
	            	   goto verifica_rango; //JRULT  verifica_rango
	               }

voltaje_bajo:		//;----------------------------  Voltaje de línea BAJO
	                //ld A,valflvl;
	                if(valflvl == 0){//cp A,#0
	                	goto falla_VLbajo;//  JREQ   falla_VLbajo       ;SI, Activa Falla por Voltaje de Línea Bajo
	                }
					goto fin_voltmetro;// jra     fin_voltmetro      ;NO, Termina el proceso VOLTMETRO

falla_VLbajo:
					Bset_Clear_trfst(trefst,trefst,f_lv,f_hv);
//					BitSet(trefst,f_lv);// BSET   trefst,#f_lv       ;Activa bandera por BAJO VOLTAJE
//					BitClear(trefst,f_hv);// BRES   trefst,#f_hv       ;Cancela bandera por ALTO VOLTAJE


tiempo_libfalla:		//;-----  15s de retardo para liberar falla por voltaje
					wreg = Plantilla[rangein]; //mov wreg,rangein
					BaBentre10_math();  //call BaBentre10_math
					// retprotl = resull;// mov retprotl,resull
					retproth = (resulh * 256) + resull;//mov retproth,resulh
					goto  fin_voltmetro; //jra  fin_voltmetro

verifica_rango:				//;-----------------------------Voltaje dentro de rango
					valflvl = Plantilla[protval]; // MOV    valflvl,protval   ;Refresaca tiempo para validación de falla BAJO Volt
					if(edorefri != 1){//cp A,#1;  / En autoprueba deja el led como está.
						goto no_auto_01;//jrne no_auto_01
					}
					valflvl = 2;//MOV valflvl,#2
no_auto_01:
	                //ld A,retprotl;
	                if(retproth == 0){//or A,retproth;
	                	goto vl_rango; //JREQ vl_rango
	                }
	                goto fin_voltmetro;//jra     fin_voltmetro     ;NO, Termina el proceso VOLTMETRO
vl_rango:
					Bclear_Clear_trfst(trefst, trefst,f_lv, f_hv);
//					BitClear(trefst, f_lv);// BRES   trefst,#f_lv       ;Voltaje en rango y tiempo transcurrido, cancela
//					BitClear(trefst,f_hv);// BRES   trefst,#f_hv       ;/ banderas de falla por voltaje de línea

fin_voltmetro:
					if(GPIOR0[f_volt]){//BTJT GPIOR0,#f_volt,fin_voltmetro1		//;¿Protección de voltaje activada?
						goto fin_voltmetro1;
					}
					Bclear_Clear_trfst(trefst, trefst,f_lv, f_hv);
//					BitClear(trefst,f_lv);//BRES   trefst,#f_lv			//;NO hay protección de voltaje activada
//					BitClear(trefst, f_hv);//BRES   trefst,#f_hv		//;limpia banderas de falla por voltaje

fin_voltmetro1:
					goto main_voltmetro;//jp main_voltmetro
main_voltmetro:

}
//*************************************************************************************************
