


#include "main.h"
#include "customMain.h"
#include "board_PFULLDEF.h"
#include "ELTEC_EmulatedEEPROM.h"

// rutina refrigera Adaptada CTOF Completa ..............


void noctar (void){



noct05:		if(edorefri < 2){
				goto noct12;		// Estas en arranque ó autoprueba?
	 	 	 }


			if(!flagsb[f_excom]){ 	//btjf		flagsb,#f_excom,si_noct
				goto si_noct;
			}
			goto	alumbrado;			//jp			alumbrado
si_noct:

			if(!botonst[b2_f1]){ //	btjf		botonst,#b2_f1,noct10
				goto noct10;
			}
			if(!GetRegFlagState(Plantilla[logicos2],modLogic) ){//btjt		logicos,#f_lampAH,noAskAH2
				goto noModlog_03;
			}
			//;activa visualización del sensor 3 (12 segundos)
			timeDpyS3 = 1400;			//ldw			X,#1400
			goto  noct10;				//jra			noct10

noModlog_03:

			flagsa[1] = 1;		//	bset		flagsa,#1;0x02;	/ No, activa modo nocturno manual

noct10:

			if(Plantilla[sensit] != 0 ){	//btjt		logicos,#f_lampAH,noAskAH2
				goto noct12;
			}
			goto	alumbrado;			//jp			alumbrado;		/ Sí

noct12:



















alumbrado:

finprec:	goto			main_noctar;		//	jp			main_noctar			;Manuel 14-FEB-2022:jp			main;

main_noctar:
}
