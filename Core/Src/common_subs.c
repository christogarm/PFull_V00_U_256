

#include "main.h"
#include "customMain.h"
#include "temper.h"
#include "ELTEC_EmulatedEEPROM.h"
//#include "stm32l0xx_hal_flash_ex.h"

uint16_t TwoByteInArrayToWord (uint8_t  *PointArray);
_Bool cambio_rutina;


//;LN 5910 ------------------------- Rutina que promedia 8 muestras de 1 byte
uint16_t prom8m1b (uint8_t *array , uint8_t prom){
	uint16_t   foo = 0 ;
	for(uint8_t i = 0; i < prom ; i++ ){
		foo += array[i];
	}
	foo /=  prom;
	return foo;
}


//;LN 5944 ============================================================
//;///manuel  esta parte se llama desde el proceso  display
void  prog_param (void){

		if(!botonst2[1]){//btjf botonst2,#1,no_set_prog;
			goto no_set_prog;
		}
		flagsb[f_prog] = 1;//bset flagsb,#f_prog
		cnt_prog = 20;//mov cnt_prog,#20
		key = 0;//clr	key
		param = 0; //clr param
		for(uint8_t k=0; k<8; k++){
			flags_menu[k] = 0; //clr flags_menu;
		}
		//ldw X,#eedato_seg1
		//ldw Y,#cdato_seg1 ************** ?
		uint8_t *X = &eePlantilla[eedato_seg1];					// manuel_ apuntador para la eeprom
		uint8_t *Y = &copiaPlantilla[cdato_seg1];								// manuel_ apuntador ram para la compia de parametros

		// manuel_ copia los parametros de eeprom a una ram copia para modificarlos
copy_eeprom: //********************************?
		//call rdeeprom
		//ld A,waux
		*Y = (uint8_t) findLastValue((uint32_t)X);	//ld (Y), A
		X++;//incw X
		Y++;//incw Y
		if(Y <= &copiaPlantilla[cdato_seg3]){//cpw Y,#cdato_seg3
			goto copy_eeprom;//jrule copy_eeprom
		}

no_set_prog:

		if(flagsb[f_prog]){//btjt flagsb,#f_prog,ask_btn_prsd
			goto ask_btn_prsd;
		}
		goto fin_prog_param; //jp fin_prog_param
ask_btn_prsd:
	for(uint8_t k=0; k<8; k++){
		if(!btn_pr[k]){//tnz btn_pr				;// manuel_ no es un contador es copia de los botones, pregunta practicmante si alguna tecla se presiono
			goto no_btn_pres; //jreq no_btn_pres
		}
	}
		cnt_prog = 20;//mov cnt_prog,#20		;// manuel_ cada que se presiona cualquier tecla se recarga el tiempo de programacion a 30s
no_btn_pres:
		if(cnt_prog != 0){//tnz cnt_prog
			goto no_exp_tmp;//jrne	no_exp_tmp
		}
		goto cancel_prog;

		// manuel_ 1er primer paso para ver si ya se introdujo el password correctamente
no_exp_tmp:

		if(!flagsb[f_menu2]){//btjf flagsb,#f_menu2,noMenu2
			goto noMenu2;
		}
		if(flagsb[f_nv2Menu2]){//btjt flagsb,#f_nv2Menu2,jmp_nv2Menu2
			goto jmp_nv2Menu2;
		}
		goto menu2; //jp menu2
jmp_nv2Menu2:
		goto nv2Menu2;
noMenu2:

		if(!flagsb[f_sprm]){//btjf flagsb,#f_sprm,sel_param
			goto sel_param;
		}
		goto mod_param;//jra mod_param;		/ Si ya se selecciono el parámetro ve a la modificación
//;============================================================
//;Selección de parámetro
sel_param:
		btn_pr[b1_f2] = 0;//bres btn_pr,#b1_f2
		btn_pr[b3_f2] = 0;//bres btn_pr,#b3_f2

		STM8_A = param;		//ld a, param
		lmt_up_w  = 9;		//mov lmt_up_w + 1,#9
		lmt_dw_w = 0;		//mov lmt_dw_w + 1,#00
		md_dif_math();	 	//call md_dif_math;					//24-oct-2024 RGM:	Funcion Revisada
		param = wreg;		//mov param,wreg


//		//ld A,#%00001111   ************************
//		//and A,param
		STM8_A = param & 0x0F;			//;/ Para evitar saltos indeseados
//		//ldw X,#$0002
//		//mul X,A
//		//jp (opc_menu_01,X) ****************
opc_menu_01:
		switch (STM8_A)
		{
			case 0: goto opc00m01;
				//break;
			case 1: goto opc01m01;
				//break;
			case 2: goto opc02m01;
				//break;
			case 3: goto opc03m01;
				//break;
			case 4: goto opc04m01;
				//break;
			case 5: goto opc05m01;
				//break;
			case 6: goto opc06m01;
				//break;
			case 7: goto opc07m01;
				//break;
			case 8: goto opc08m01;
				//break;
			case 9: goto opc09m01;
				//break;
			default: goto opcNA;
				//break;

		}
//		if(STM8_A == 0)     goto opc00m01;	// jp opc00m01;		;/ Opción 0 menú 1
//		if(STM8_A == 1)     goto opc01m01;	// jp opc01m01;		;/ Opción 1 menú 1
//		if(STM8_A == 2)     goto opc02m01;	// jp opc02m01		;/ Opción 2 menú 1
//		if(STM8_A == 3)     goto opc03m01;	// jp opc03m01		;/ Opción 3 menú 1
//		if(STM8_A == 4)     goto opc04m01;	// jp opc04m01		;/ Opción 4 menú 1
//		if(STM8_A == 5)     goto opc05m01; 	// jp opc05m01		;/ Opción 5 menú 1
//		if(STM8_A == 6)     goto opc06m01; 	// jp opc06m01		;/ Opción 6 menú 1
//		if(STM8_A == 7)     goto opc07m01; 	// jp opc07m01		;/ Opción 7 menú 1
//		if(STM8_A == 8)     goto opc08m01; 	// jp opc08m01		;/ Opción 8 menú 1
//		if(STM8_A == 9)     goto opc09m01; 	// jp opc09m01		;/ Opción 9 menú 1
//		if(STM8_A == 10)    goto opcNA; 	// jp  opcNA		;/ Opción 10 menú 1
//		if(STM8_A == 11)    goto opcNA; 	// jp opcNA			;/ Opción 11 menú 1
//		if(STM8_A == 12)    goto opcNA; 	// jp opcNA			;/ Opción 12 menú 1
//		if(STM8_A == 13)    goto opcNA; 	// jp opcNA			;/ Opción 13 menú 1
//		if(STM8_A == 14)    goto opcNA; 	// jp opcNA			;/ Opción 14 menú 1
//		if(STM8_A == 15)    goto opcNA; 	// jp opcNA			;/ Opción 15 menú 1

opc00m01:
		op_menu (0x05, 0x25);
		goto ask_enter;//jra ask_enter
		//datdig1 = 0x05;//mov	datdig1,#$05;	"S"
		//datdig2 = 0x25;//mov	datdig2,#$25;	"P"
		//goto ask_enter;//jra ask_enter
opc01m01:
		op_menu (0x0D, 0x0F);
		//datdig1 = 0x0D;//mov	datdig1,#$0D;	"d"
		//datdig2 = 0x0F;//mov	datdig2,#$0F;	"F"
		goto ask_enter; // jra ask_enter
opc02m01:
		op_menu (0x20, 0x0A);
		//datdig1 = 0x20;//mov	datdig1,#$20;	"t"
		//datdig2 = 0x0A;//mov	datdig2,#$0A;	"A"
		goto ask_enter;//jra ask_enter
opc03m01:
		op_menu (0x20, 0x0B);
		//datdig1 = 0x20; //mov	datdig1,#$20;	"t"
		//datdig2 = 0x0B;//mov	datdig2,#$0B;	"B"
		goto ask_enter;//jra ask_enter
opc04m01:
		op_menu (0x00, 0x0F);
		//datdig1 = 0x00;//mov	datdig1,#$00;	"D"
		//datdig2 = 0x0F;//mov	datdig2,#$0F;	"F"
		goto ask_enter;//jra	ask_enter
opc05m01:
		op_menu (0x25, 0x05);
		//datdig1 = 0x25;//mov	datdig1,#$25;	"P"
		//datdig2 = 0x05;//mov	datdig2,#$05;	"S"
		goto ask_enter;//jra ask_enter
opc06m01:
		op_menu (0x0F, 0x27);
		//datdig1 = 0x0F;//mov	datdig1,#$0F;	"F"
		//datdig2 = 0x27;//mov	datdig2,#$27;	"U"
		goto ask_enter;//jra ask_enter
opc07m01:
		op_menu (0x0F, 0x0A);
		//datdig1 = 0x0F;//mov	datdig1,#$0F;	"F"
		//datdig2 = 0x0A;//mov	datdig2,#$0A;	"A"
		goto ask_enter;//jra	ask_enter
opc08m01:
		op_menu (0x05, 0x0E);
		//datdig1 = 0x05;//mov	datdig1,#$05;	"S"
		//datdig2 = 0x0E;//mov	datdig2,#$0E;	"E"
		goto ask_enter;//jra ask_enter
opc09m01:
		op_menu (0x1F, 0x0E);
		//datdig1 = 0x1F;//mov	datdig1,#$1F;	" "
		//datdig2 = 0x0E;//mov	datdig2,#$0E;	"E"
		goto ask_enter;//jra	ask_enter
opcNA:
		op_menu (0x26, 0x26);
		//datdig1 = 0x26;//mov	datdig1,#$26;	"-"
		//datdig2 = 0x26;//mov	datdig2,#$26;	"-"


ask_enter:
		datled_clear();
		//BitClear(datled,0);//bres datled,#0; / apaga el punto
		//BitClear(datled,1);//bres datled,#1; / apaga el signo


		if(!btn_pr[b2_f1]){//btjf btn_pr,#b2_f1,fin_sel_param;		Se acepto la selección del parámetro?
			goto fin_sel_param;
		}
		flagsb[f_sprm] = 1;//bset flagsb,#f_sprm;		indica que sea sseleccionado una parametro
		//ld A,param
		if(param != 9){//cp A,#9;
			goto ask_set_prog;//jrne ask_set_prog
		}
		goto cancel_prog;//jp cancel_prog;
ask_set_prog:
		if(param != 8){//cp A,#8;
			goto fin_sel_param; //jrne fin_sel_param
		}

		if(!flags_menu[0]){//btjf flags_menu,#0,no_desh_forz
			goto no_desh_forz;
		}
		flags_accMenu = 1;		//bset flags_accMenu,#0
no_desh_forz:
		if(!flags_menu[1]){//btjf	flags_menu,#1,no_def_param
			goto no_def_param;
		}
		flags_menu[1] = 0; //bres flags_menu,#1

		//mov cspdiur_w,cspdiur_w_BK
		copiaPlantilla[cspdiur_H] =  copiaPlantilla[cspdiur_BK_H];			//mov	cspdiur_w,cspdiur_w_BK
		copiaPlantilla[cspdiur_L] =  copiaPlantilla[cspdiur_BK_L];			//mov	cspdiur_w+1,cspdiur_w_BK+1
		copiaPlantilla[cdifdiur_H] = copiaPlantilla[cdifdiur_BK_H];			//mov	cdifdiur_w,cdifdiur_w_BK
		copiaPlantilla[cdifdiur_L] = copiaPlantilla[cdifdiur_BK_L];			//mov	cdifdiur_w+1,cdifdiur_w_BK+1
		copiaPlantilla[climsual_H] = copiaPlantilla[climsual_BK_H];			//mov	climsual_w,climsual_w_BK
		copiaPlantilla[climsual_L] = copiaPlantilla[climsual_BK_L];			//mov	climsual_w+1,climsual_w_BK+1
		copiaPlantilla[climinal_H] = copiaPlantilla[climinal_BK_H];			//mov	climinal_w,climinal_w_BK
		copiaPlantilla[climinal_L] = copiaPlantilla[climinal_BK_L];			//mov	climinal_w+1,climinal_w_BK+1
no_def_param:

		cntmemo = 0x00;//mov cntmemo,#$00			//Para grabar desde la primera localidad de EEPROM
		ctlmemo = 0xAA;//mov ctlmemo,#$AA			//Graba los datos en EEPROM
		goto cancel_prog;//jp cancel_prog

fin_sel_param:
		goto fin_prog_param;
//;============================================================
//;Modificación de parámetro
mod_param:
		//	        //ld A,#%00001111
		//			//and A,param
		STM8_A = param & 0x0F;
		//			//ldw X,#$0003
		//			//mul X,A
		//
		//jp (menu_01_nv2,X) ***********************
menu_01_nv2:

		switch (STM8_A)
		{
			case 0: goto opc00m01nv2;
				//break;
			case 1: goto opc01m01nv2;
				//break;
			case 2: goto opc02m01nv2;
				//break;
			case 3: goto opc03m01nv2;
				//break;
			case 4: goto opc04m01nv2;
				//break;
			case 5: goto opc05m01nv2;
				//break;
			case 6: goto opc06m01nv2;
				//break;
			case 7: goto opc07m01nv2;
				//break;
			case 8: goto opc08m01nv2;
				//break;
			case 9: goto opc09m01nv2;
				//break;
			default: goto opcNAnv2;
				//break;
		}
//		if(STM8_A == 0)     goto opc00m01nv2;//jp opc00m01nv2;		  	;/ Opción 0 menú 1
//		if(STM8_A == 1)     goto opc01m01nv2;//jp opc01m01nv2;		  	;/ Opción 1 menú 1
//		if(STM8_A == 2)     goto opc02m01nv2;//jp opc02m01nv2;		  	;/ Opción 2 menú 1
//		if(STM8_A == 3)     goto opc03m01nv2;//jp opc03m01nv2;		  	;/ Opción 3 menú 1
//		if(STM8_A == 4)     goto opc04m01nv2;//jp opc04m01nv2;		  	;/ Opción 4 menú 1
//		if(STM8_A == 5)     goto opc05m01nv2;//jp opc05m01nv2;		  	;/ Opción 5 menú 1
//		if(STM8_A == 6)     goto opc06m01nv2;//jp opc06m01nv2;		  	;/ Opción 6 menú 1
//		if(STM8_A == 7)     goto opc07m01nv2;//jp opc07m01nv2;		  	;/ Opción 7 menú 1
//		if(STM8_A == 8)     goto opc08m01nv2;//jp opc08m01nv2;		  	;/ Opción 8 menú 1
//		if(STM8_A == 9)     goto opc09m01nv2;//jp opc09m01nv2;		  	;/ Opción 9 menú 1
//		if(STM8_A == 10)     goto opcNAnv2;//jp opcNAnv2;		 		 ;/ Opción 10 menú 1
//		if(STM8_A == 11)     goto opcNAnv2;//jp opcNAnv2;			  	;/ Opción 11 menú 1
//		if(STM8_A == 12)     goto opcNAnv2;//jp opcNAnv2;		 	 	;/ Opción 12 menú 1
//		if(STM8_A == 13)     goto opcNAnv2;//jp opcNAnv2;		  		;/ Opción 13 menú 1
//		if(STM8_A == 14)     goto opcNAnv2;//jp opcNAnv2;		  		;/ Opción 14 menú 1
//		if(STM8_A == 15)     goto opcNAnv2;//jp opcNAnv2;		  		;/ Opción 15 menú 1

//;*********************
//; Se cambia ubicación de subrutina md_dif_math  para que compilador
//; no cambie instrucción jp por jrt
//;*********************

//;===================================================================================
//;LN 6338 ============================================================
//;subrrutina para la modificacion del valor porgramado para temperatura
//			..
//		md_dif_math:
//			..
//;LN 6338 ============================================================
//;===================================================================================
opc00m01nv2:
			opc_nv (TwoByteInArrayToWord (&copiaPlantilla[crngmax_H]), TwoByteInArrayToWord (&copiaPlantilla[crngmin_H]),TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H]), cspdiur_H, cspdiur_L);

			//;mov			datdig1,#$05;	"S"
			//;mov			datdig2,#$25;	"P"

			//ldw  X,crngmax_w
			//ldw lmt_up_w,X
//			lmt_up_w = TwoByteInArrayToWord (&copiaPlantilla[crngmax_H]);		// rngmax (paramaetro t7)
			//ldw X,crngmin_w
			//ldw lmt_dw_w,X
//			lmt_dw_w = TwoByteInArrayToWord (&copiaPlantilla[crngmin_H]);		// rngmin (paramaetro t6)

//			STM8_16_X = TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H]);		//ldw X,cspdiur_w
//			md_signfrac_math();		//call md_signfrac_math
//			copiaPlantilla[cspdiur_H] =	  highByte(STM8_16_X);		//ldw cspdiur_w,X
//			copiaPlantilla[cspdiur_L] =   lowByte(STM8_16_X);
//			convadec_math(STM8_16_X);		//call convadec_math

			goto ask_enter_nv2; //jra ask_enter_nv2
opc01m01nv2:
			opc_nv (99, 0, TwoByteInArrayToWord (&copiaPlantilla[cdifdiur_H]), cdifdiur_H, cdifdiur_L);
			//;mov			datdig1,#$0D;	"d"
			//;mov			datdig2,#$0F;	"F"

			//ldw X,#99;
			//lmt_up_w, X
//			lmt_up_w = 99;			// rngmax (9.9)
			//ldw X,#$0000;
			//ldw lmt_dw_w,X
//			lmt_dw_w = 0;			// rngmin (0.0)

//			STM8_16_X = TwoByteInArrayToWord (&copiaPlantilla[cdifdiur_H]);		// ldw X,cdifdiur_w
//			md_signfrac_math();	// call	md_signfrac_math;modificación de parámetros de 2 Bytes
//			copiaPlantilla[cdifdiur_H] =   highByte(STM8_16_X);		//ldw cdifdiur_w,X
//			copiaPlantilla[cdifdiur_L] =   lowByte(STM8_16_X);
//			convadec_math(STM8_16_X);//call convadec_math;/ despliega dato de temperatura

			goto ask_enter_nv2;//jra ask_enter_nv2
opc02m01nv2:
			opc_nv (TwoByteInArrayToWord (&copiaPlantilla[crngmax_H]) + 150, (TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H])) + (TwoByteInArrayToWord (&copiaPlantilla[cdifdiur_H])) + 10 , TwoByteInArrayToWord (&copiaPlantilla[climsual_H]), climsual_H, climsual_L);
			//;mov			datdig1,#$20;	"t"
			//;mov			datdig2,#$0A;	"A"

			// X,crngmax_w
			//addw X,#150
			//ldw lmt_up_w,X
//			lmt_up_w = TwoByteInArrayToWord (&copiaPlantilla[crngmax_H]) + 150;			// rngmax (paramaetro t7) + 15.0
//			STM8_16_X = TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H]);				//ldw X,cspdiur_w
//			STM8_16_X = STM8_16_X + TwoByteInArrayToWord (&copiaPlantilla[cdifdiur_H]);  // addw x, cdifdiur_w
			// addw X,#10
			// ldw lmt_dw_w,X
//			lmt_dw_w = STM8_16_X + 10;		// rngmin (paramaetro t0 set point) + diferencial + 1.0

//			STM8_16_X = TwoByteInArrayToWord (&copiaPlantilla[climsual_H]);  //	ldw	X,climsual_w
//			md_signfrac_math();		//	call md_signfrac_math;modificación de parámetros de 2 Bytes
//			copiaPlantilla[climsual_H] =   highByte(STM8_16_X);//	ldw	climsual_w,X
//			copiaPlantilla[climsual_L] =   lowByte(STM8_16_X);
//			convadec_math(STM8_16_X);//	call convadec_math;/ despliega dato de temperatura

			goto ask_enter_nv2;//	jra ask_enter_nv2
opc03m01nv2:
			opc_nv (TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H]) - 10, TwoByteInArrayToWord (&copiaPlantilla[crngmin_H]) - 150, TwoByteInArrayToWord (&copiaPlantilla[climinal_H]), climinal_H, climinal_L);
			//;mov			datdig1,#$20;	"t"
			//;mov			datdig2,#$0B;	"B"

			//ldw X,cspdiur_w;
			//subw	X,#10
			//ldw lmt_up_w,X
//			lmt_up_w = TwoByteInArrayToWord (&copiaPlantilla[cspdiur_H]) - 10;				// rngmax (paramaetro t0 set point) -1.0
			//ldw  X,crngmin_w;
			//subw X,#150
			//ldw lmt_dw_w,X
//			lmt_dw_w = TwoByteInArrayToWord (&copiaPlantilla[crngmin_H]) - 150;				// rngmin (paramaetro t6) - 15.0

//			STM8_16_X = TwoByteInArrayToWord (&copiaPlantilla[climinal_H]); //ldw X,climinal_w
//			md_signfrac_math();	//call md_signfrac_math;
//			copiaPlantilla[climinal_H] =   highByte(STM8_16_X);  //ldw climinal_w,X
//			copiaPlantilla[climinal_L] =   lowByte(STM8_16_X);
//			convadec_math(STM8_16_X);//call convadec_math;/

			goto ask_enter_nv2;		//jra ask_enter_nv2
opc04m01nv2:
			//;mov			datdig1,#$00;	"D"
			//;mov			datdig2,#$0F;	"F"

			if(btn_pr[b1_f1]){ //btjt btn_pr,#b1_f1,toggle_opc04m01nv2
				goto toggle_opc04m01nv2;
			}
			if(btn_pr[b3_f1]){ //btjt btn_pr,#b3_f1,toggle_opc04m01nv2
				goto toggle_opc04m01nv2;
			}
			goto dpy_opc04m01nv2; //jra dpy_opc04m01nv2
toggle_opc04m01nv2: //toggle_opc04m01nv2
			flags_menu[0] ^= 1; // BitComplement(flags_menu,0);		//bcpl flags_menu,#0

dpy_opc04m01nv2:
			op_menu (0x28, 0x01);
			//datdig1 = 0x28;//mov datdig1,#$28	"r"
			//datdig2 = 0x01;//mov datdig2,#$01;	"1"
			if(!flags_menu[0]){//btjf flags_menu,#0,dpy2_opc04m01nv2
				goto dpy2_opc04m01nv2;
			}
			op_menu (0x00, 0x1D);
			//datdig1 = 0x00;//mov	datdig1,#$00;	"o"
			//datdig2 = 0x1D;//mov	datdig2,#$1D;	"n"
dpy2_opc04m01nv2: //dpy2_opc04m01nv2
			datled_clear();
			//BitClear(datled,0);//bres datled,#0;  	/ apaga el punto
			//BitClear(datled,1);//bres datled,#1;	/ apaga el signo

			goto ask_enter_nv2;//jra ask_enter_nv2
opc05m01nv2:
			op_menu (0x25, 0x05);
			//datdig1 = 0x25;// mov	datdig1,#$25	"P"
			//datdig2 = 0x05; //mov	datdig2,#$05;	"S"

			lmt_up_w = 99;//	mov lmt_up_w + 1,#99
			lmt_dw_w = 0;//	mov	lmt_dw_w + 1,#00
			STM8_A = key;		//	ld	a,key ;			/ Carga el dato apuntado en la dirección del parámetro
			md_dif_math();	//	call md_dif_math;		/ modificacion de un dato sin signo entero
			key = wreg;//	mov	key,wreg

			soloent1();//	call soloent1;				/ Muestra valor del parámetro
			datled_clear();
			//BitClear(datled,0);//	bres datled,#0;		/ apaga el punto
			//BitClear(datled,1);//	bres datled,#1;		/ apaga el signo

			goto ask_enter_nv2;//	jra	 ask_enter_nv2
opc06m01nv2:
			//;mov			datdig1,#$0F;	"F"
			//;mov			datdig2,#$27;	"U"
			//op_menu (eePlantilla[eeversion1], eePlantilla[eeversion2] / 10);
			op_menu  (findLastValue((uint32_t) &eePlantilla[eeversion1]) , findLastValue((uint32_t) &eePlantilla[eeversion2])/10);
//			datdig1 = eePlantilla[eeversion1];//mov datdig1,eeversion1
			//ldw X,eeversion2
			//ld A,#10
			//div X,A
			//ld A,xl
			//ld datdig2,A
//			datdig2 = eePlantilla[eeversion2] / 10;
			goto ask_enter_nv2;//jra ask_enter_nv2
opc07m01nv2:
			//;mov			datdig1,#$0F;	"F"
			//;mov			datdig2,#$0A;	"A"

		   if(btn_pr[b1_f1]){ //btjt btn_pr,#b1_f1,toggle_opc07m01nv2
			   goto toggle_opc07m01nv2;
		   }
		   if(btn_pr[b3_f1]){//btjt btn_pr,#b3_f1,toggle_opc07m01nv2
			   goto toggle_opc07m01nv2;
		   }
		   goto dpy_opc07m01nv2; //jra dpy_opc07m01nv2
toggle_opc07m01nv2:
			flags_menu[1] ^= 1; //bcpl flags_menu,#1

dpy_opc07m01nv2:
			op_menu (0x28, 0x01);
			//datdig1 = 0x28;//	mov	datdig1,#$28;	"r"
			//datdig2 = 0x01;//	mov	datdig2,#$01;	"1"
			if(!flags_menu[1]){	//btjf flags_menu,#1,dpy2_opc07m01nv2
				goto dpy2_opc07m01nv2;
			}
			op_menu (0x00, 0x1D);
			//datdig1 = 0x00;//	mov	datdig1,#$00;	"o"
			//datdig2 = 0x1D;//	mov	datdig2,#$1D;	"n"
dpy2_opc07m01nv2:
			datled_clear();
			//BitClear(datled,0);//	bres datled,#0;		/ apaga el punto
			//BitClear(datled,1);//	bres datled,#1;		/ apaga el signo

			goto ask_enter_nv2;//	jra ask_enter_nv2

opc08m01nv2:
			op_menu (0x05, 0x0E);
			//datdig1 = 0x05;//	mov	datdig1,#$05;	"S"
			//datdig2 = 0x0E;//	mov	datdig2,#$0E;	"E"
			goto ask_enter_nv2;//	jra	ask_enter_nv2
opc09m01nv2:
			op_menu (0x1F, 0X0E);
			//datdig1 = 0x1F;//	mov	datdig1,#$1F;	" "
			//datdig2 = 0X0E;//	mov	datdig2,#$0E;	"E"
			goto ask_enter_nv2;// jra	ask_enter_nv2
opcNAnv2:
			op_menu (0x26, 0x26);
			//datdig1 = 0x26;//	mov	datdig1,#$26;	"-"
			//datdig2 = 0x26;//	mov	datdig2,#$26;	"-"


ask_enter_nv2:

			if(!btn_pr[b2_f1]){//btjf	btn_pr,#b2_f1,no_enter_nv2;
				goto no_enter_nv2;
			}
			flagsb[f_sprm] = 0;//bres	flagsb,#f_sprm;

			//ld A,param
			if(param != 5){//cp A,#5;							/ es opcion PS (password)?
				goto no_enter_nv2;//jrne no_enter_nv2;
			}
			//ld A,key
			if(key != Plantilla [password]){//cp A,password		/ es opcion PS (password)?
				goto no_enter_nv2;//jrne no_enter_nv2
			}
			Bset_Clear_trfst(flagsb, flags_menu, f_menu2, 3);
//			BitSet(flagsb,f_menu2);//bset flagsb,#f_menu2
//			BitClear(flags_menu,3);//bres flags_menu,#3
			param2 = 0;//clr param2

			if(!flagsb[f_luzb]){//btjf flagsb,#f_luzb,noSetFlagLU
				goto noSetFlagLU;
			}
			flags_menu[3] = 1;//bset flags_menu,#3
noSetFlagLU:
			flags_menu[4] = 0;//	bres flags_menu,#4
			//	ld	A,escala
			if(Plantilla[escala] != 0x20){//	cp	A,#$20
				goto noFahrenheitFlag;//	jrne noFahrenheitFlag
			}
			flags_menu[4] = 1;//	bset flags_menu,#4
noFahrenheitFlag:

no_enter_nv2:
		       goto fin_prog_param; //jp fin_prog_param

//;============================================================
menu2:
		Bclear_Clear_trfst(btn_pr, btn_pr,b1_f2, b3_f2);
//		BitClear(btn_pr,b1_f2);//	bres btn_pr,#b1_f2
//		BitClear(btn_pr,b3_f2);//	bres btn_pr,#b3_f2

		STM8_A = param2;	//	ld	a,param2
		lmt_up_w = 4;		//	mov  lmt_up_w + 1,#4
		lmt_dw_w = 0;		//	mov	lmt_dw_w + 1,#00
		md_dif_math	();		//	call md_dif_math;
		param2 = wreg;//	mov	param2,wreg

		//	ld  A,#%00000111
		//	and  A,param2
		STM8_A = param2 & 0x07;			//;/ Para evitar saltos indeseados
		//	ldw  X,#$0002
		//	mul X,A
		//	jp (opc_menu_02,X)
opc_menu_02:

		switch(STM8_A)
		{
			case 0: goto opc00m02;
				//break;
			case 1: goto opc01m02;
				//break;
			case 2: goto opc02m02;
				//break;
			case 3: goto opc03m02;
				//break;
			case 4: goto opc04m02;
				//break;
			default: goto opcNA_m02;
				//break;
		}
//		if(STM8_A == 0)    	goto opc00m02;//	jp	opc00m02;	  	;/ Opción 0 menú 2
//		if(STM8_A == 1)    	goto opc01m02;//	jp	opc01m02;  		;/ Opción 1 menú 2
//		if(STM8_A == 2)    	goto opc02m02;//	jp	opc02m02;		;/ Opción 2 menú 2
//		if(STM8_A == 3)    	goto opc03m02;//	jp	opc03m02;		;/ Opción 3 menú 2
//		if(STM8_A == 4)    	goto opc04m02;//	jp	opc04m02;		;/ Opción 4 menú 2
//		if(STM8_A == 5)    	goto opcNA_m02;//	jp	opcNA_m02;		;/ Opción 5 menú 2
//		if(STM8_A == 6)    	goto opcNA_m02;//	jp	opcNA_m02;		;/ Opción 6 menú 2
//		if(STM8_A == 7)    	goto opcNA_m02;//	jp	opcNA_m02;		;/ Opción 7 menú 2


opc00m02:
		op_menu (0x00, 0x0F);
		//datdig1 = 0x00; //	mov	datdig1,#$00;   "O"
		//datdig2 = 0x0F;//	mov	datdig2,#$0F;   "F"
		goto ask_enter_m02;//	jra	ask_enter_m02
opc01m02:
		op_menu (0x11, 0x27);
		//datdig1 = 0x11;//	mov datdig1,#$11;	"L"
		//datdig2 = 0x27;//	mov	datdig2,#$27;	"U"
		goto ask_enter_m02;//	jra	ask_enter_m02
opc02m02:
		op_menu (0x27, 0x1D);
		//datdig1 = 0x27;//	mov	datdig1,#$27;	"U"
		//datdig2 = 0x1D;//	mov	datdig2,#$1D;	"n"
		goto ask_enter_m02; //	jra ask_enter_m02
opc03m02:
		op_menu (0x05, 0x0E);
		//datdig1 = 0x05;//	mov	datdig1,#$05;	"S"
		//datdig2 = 0x0E;//	mov	datdig2,#$0E;	"E"
		goto ask_enter_m02;//	jra ask_enter_m02
opc04m02:
		op_menu (0x1F, 0X0E);
		//datdig1 = 0x1F;//	mov	datdig1,#$1F;	" "
		//datdig2 = 0X0E;//	mov	datdig2,#$0E;	"E"
		goto ask_enter_m02;//jra	ask_enter_m02
opcNA_m02:
		op_menu (0x26, 0x26);
		//datdig1 = 0x26;// mov	datdig1,#$26;	"-"
		//datdig2 = 0x26;//	mov	datdig2,#$26;	"-"


ask_enter_m02:
		datled_clear();
		//BitClear(datled,0);//	bres datled,#0;		/ apaga el punto
		//BitClear(datled,1);  //	bres datled,#1;		/ apaga el signo

		if(!btn_pr[b2_f1]){// btjf btn_pr,#b2_f1,fin_menu2;  Se acepto la selección del parámetro?
			goto fin_menu2;
		}
		flagsb[f_nv2Menu2]= 1;		//	bset flagsb,#f_nv2Menu2;   indica que sea sseleccionado una parametro
		//	ld A,param2
		if(param2 != 4){//	cp A,#4;    / es opcion E (exit)?
			goto ask_set_prog_m02;//	jrne ask_set_prog_m02
		}
		goto cancel_prog; ////	jp	cancel_prog   / Sí, sal sin realizar ningún cambio
ask_set_prog_m02:
		if(param2 != 3){//	cp	A,#3;
			goto fin_menu2;//	jrne fin_menu2;
		}

		flagsb[f_luzb]= 0;//	bres flagsb,#f_luzb
		if(!flags_menu[3]){//	btjf flags_menu,#3,noOnLuz
			goto noOnLuz;
		}
		flagsb[f_luzb]= 1;// bset flagsb,#f_luzb
noOnLuz:

		if(!flags_menu[2]){//	btjf flags_menu,#2,noOffManto
			goto noOffManto;
		}
		timerOffManto = 120;//	mov	 timerOffManto,#120;	/ carga tiempo de apagado de mantenimiento (minutos)
noOffManto:
		copiaPlantilla [cescala] = 0;//mov cescala,#$00
		if(!flags_menu[4]){//btjf flags_menu,#4,noFahrenheitFlagDpy
			goto noFahrenheitFlagDpy;
		}
		copiaPlantilla [cescala] = 0x20;//mov cescala,#$20
noFahrenheitFlagDpy:
		//	ld			A,cescala;
		//if(copiaPlantilla [cescala] == eePlantilla [eeescala] ){//	cp A,eeescala
		if(copiaPlantilla [cescala] == findLastValue((uint32_t) &eePlantilla[eeescala])){
			goto noCambiaEscala;//	jreq noCambiaEscala
		}
		waux = copiaPlantilla [cescala];//	mov			waux,cescala;
		//	ldw			X,#eeescala;
		wreeprom(waux, &eePlantilla[eeescala]);//	call		wreeprom;
noCambiaEscala:

		 goto cancel_prog; //jp cancel_prog

fin_menu2:
		goto fin_prog_param;  //jp fin_prog_param

//;============================================================
nv2Menu2:
//			//	ld  A,#%00000111
//			//	and  A,param2    	    ;/ Para evitar saltos indeseados
//			// param2 &= 0x07;
			STM8_A = param2 & 0x07;			//;/ Para evitar saltos indeseados
//			//	ldw  X,#$0003
//			//	mul  X,A
//		   //  param2 = param2 * 0x03;
//			//jp (menu_02_nv2,X) ***********************************
menu_02_nv2:
			switch(STM8_A)
			{
				case 0:  goto opc00m02nv2;
					//break;
				case 1:  goto opc01m02nv2;
					//break;
				case 2:  goto opc02m02nv2;
					//break;
				case 3:  goto opc03m02nv2;
					//break;
				case 4:  goto opc04m02nv2;
					//break;
				default: goto opcNAm02nv2;
					//break;
			}

//			if(STM8_A == 0)     goto opc00m02nv2;// jp	opc00m02nv2;  	;/ Opción 0 menú 2
//			if(STM8_A == 1)     goto opc01m02nv2;//	jp	opc01m02nv2;  	;/ Opción 1 menú 2
//			if(STM8_A == 2)     goto opc02m02nv2;//	jp	opc02m02nv2;  	;/ Opción 2 menú 2
//			if(STM8_A == 3)     goto opc03m02nv2;//	jp	opc03m02nv2;	;/ Opción 3 menú 2
//			if(STM8_A == 4)     goto opc04m02nv2;//	jp	opc04m02nv2;	;/ Opción 4 menú 2
//			if(STM8_A == 5)     goto opcNAm02nv2;//	jp	opcNAm02nv2;	;/ Opción 5 menú 2
//			if(STM8_A == 6)     goto opcNAm02nv2;//	jp	opcNAm02nv2;	;/ Opción 6 menú 2
//			if(STM8_A == 7)     goto opcNAm02nv2;//	jp	opcNAm02nv2;	;/ Opción 7 menú 2

//opcion +++++++
opc00m02nv2:
			//;mov			datdig1,#$00;	"O"
			//;mov			datdig2,#$0F;	"F"
			if(btn_pr[b1_f1]){//	btjt btn_pr,#b1_f1,toggle_opc00m02nv2
				goto toggle_opc00m02nv2;
			}
			if(btn_pr[b3_f1]){//	btjt btn_pr,#b3_f1,toggle_opc00m02nv2
				goto toggle_opc00m02nv2;
			}
			goto dpy_opc00m02nv2; //	jra			dpy_opc00m02nv2
toggle_opc00m02nv2:
			flags_menu[2] ^= 0x2;//bcpl flags_menu,#2

dpy_opc00m02nv2:
			op_menu (0x28, 0x01);
			//datdig1 = 0x28;//	mov			datdig1,#$28;	"r"
			//datdig2 = 0x01;//	mov			datdig2,#$01;	"1"
			if(!flags_menu[2]){//	btjf flags_menu,#2,dpy2_opc00m02nv2
				goto dpy2_opc00m02nv2;
			}
			op_menu (0x00, 0x1D);
			//datdig1 = 0x00;//	mov			datdig1,#$00;	"o"
			//datdig2 = 0x1D;//	mov			datdig2,#$1D;	"n"
dpy2_opc00m02nv2:
			datled_clear();
			//BitClear(datled,0);// bres		datled,#0;		/ apaga el punto
			//BitClear(datled,1);// bres		datled,#1;		/ apaga el signo

			goto ask_enter_m02nv2;
//opcion +++++++
opc01m02nv2:
			//;mov			datdig1,#$11;	"L"
			//;mov			datdig2,#$27;	"U"

			if(btn_pr[b1_f1]){//btjt btn_pr,#b1_f1,toggle_opc01m02nv2
				goto toggle_opc01m02nv2;
			}
			if(btn_pr[b3_f1]){ //btjt btn_pr,#b3_f1,toggle_opc01m02nv2
				goto toggle_opc01m02nv2;
			}
			goto dpy_opc01m02nv2; //jra dpy_opc01m02nv2
 toggle_opc01m02nv2:
 	 	 	flags_menu[3] ^= 1;//bcpl flags_menu,#3

 dpy_opc01m02nv2:
 	 	 	op_menu (0x00, 0x0f);
			//datdig1 = 0x00;//mov datdig1,#$00;	"0"
			//datdig2 = 0x0f;//mov datdig2,#$0f;	"f"
			if(!flags_menu[3]){ //btjf flags_menu,#3,dpy2_opc01m02nv2
				goto dpy2_opc01m02nv2;
			}
			op_menu (0x00, 0x1D);
			//datdig1 = 0x00;//mov datdig1,#$00	"O"
			//datdig2 = 0x1D;//mov datdig2,#$1D;	"n"
 dpy2_opc01m02nv2:
 	 	   datled_clear();
 	 	   //BitClear(datled, 0);//bres datled,#0;		// apaga el punto
		   //BitClear(datled, 1);//bres datled,#1;		// apaga el signo

		   goto ask_enter_m02nv2;// jra ask_enter_m02nv2
//opcion +++++++
opc02m02nv2:
			//;mov			datdig1,#$27;	"U"
			//;mov			datdig2,#$1D;	"n"

			if(btn_pr[b1_f1]){//btjt btn_pr,#b1_f1,toggle_opc02m02nv2
			   goto toggle_opc02m02nv2;
			}
			if(btn_pr[b3_f1]){//btjt btn_pr,#b3_f1,toggle_opc02m02nv2
			   goto toggle_opc02m02nv2;
			}
			goto dpy_opc02m02nv2;//jra dpy_opc02m02nv2
toggle_opc02m02nv2:
			flags_menu[4] ^= 1;//bcpl flags_menu,#4

dpy_opc02m02nv2:
			  op_menu (0x29, 0x0C);
			  //datdig1 = 0x29;//mov datdig1,#$29;  	"°"
//			  /datdig2 = 0x0C;//mov datdig2,#$0C;	"C"
			  if(!flags_menu[4]){//btjf flags_menu,#4,dpy2_opc02m02nv2
				  goto dpy2_opc02m02nv2;
			  }
			  op_menu (0x29, 0x0F);
			  //datdig1 = 0x29;//mov datdig1,#$29;	"°"
			  //datdig2 = 0x0F;//mov datdig2,#$0F		"F"
dpy2_opc02m02nv2:
				datled_clear();
				//BitClear (datled,0);//bres datled,#0;				/ apaga el punto
				//BitClear (datled,1); // bres datled,#1;			/ apaga el signo

				goto ask_enter_m02nv2;//jra ask_enter_m02nv2;
//opcion +++++++
opc03m02nv2:
			 op_menu (0x05, 0x0E);
			 //datdig1 = 0x05;//mov datdig1,#$05;		"S"
			 //datdig2 = 0x0E;//mov datdig2,#$0E;		"E"
			 goto ask_enter_m02nv2;//jra ask_enter_m02nv2
//opcion +++++++
opc04m02nv2:
			 op_menu (0x1F, 0x0E);
			 //datdig1 = 0x1F;//mov datdig1,#$1F		" "
			 //datdig2 = 0x0E;//mov datdig2,#$0E		"E"
			 goto ask_enter_m02nv2;//jra ask_enter_m02nv2
//opcion +++++++
opcNAm02nv2:
			 op_menu (0x26, 0x26);
			 //datdig1 = 0x26;//mov datdig1,#$26;		"-"
			 //datdig2 = 0x26;//mov datdig2,#$26	    "-"


ask_enter_m02nv2:
		if(!btn_pr[b2_f1]){//btjf btn_pr,#b2_f1,no_enter_m02nv2
			goto no_enter_m02nv2;
		}
		flagsb[f_nv2Menu2] = 0;//bres flagsb,#f_nv2Menu2;

no_enter_m02nv2:
		goto fin_prog_param; //jp fin_prog_param;
//;============================================================

cancel_prog:
		Bclear_Clear_trfst(flagsb, flagsb,f_prog, f_sprm);
//		BitClear(flagsb,f_prog);//bres flagsb,#f_prog
//		BitClear(flagsb,f_sprm); //bres flagsb,#f_sprm
		//bres		flagsb,#f_sgpo;								/ cancela bandera de programacion
		Bclear_Clear_trfst(flagsb, flagsb,f_ulck, f_menu2);
//		BitClear(flagsb,f_ulck); //bres flagsb,#f_ulck
//		BitClear(flagsb,f_menu2); // bres flagsb,#f_menu2
		flagsb[f_nv2Menu2] = 0; // bres flagsb,#f_nv2Menu2
fin_prog_param:
		//	bres		flagsb,#f_lmt									;//manuel_ Bandera que indica paso por el limite de programacion (segun definiciones de banderas)
		//	ret

}
//;===================================================================================
//;LN 6338 ============================================================
//;subrrutina para la modificacion del valor porgramado para temperatura
void md_dif_math (){
			flagsb[f_nd_temp]=1; 		// bset flagsb,#f_nd_temp			;// enciende bandera indica no es dato de temperatura
			//clrw X
			STM8_16_X = (uint16_t)STM8_A; 	// ld XL,A
			//lmt_up_w = 0;					// clr lmt_up_w ******** 	Se comentaron estas dos instrucciones:
			//lmt_dw_w = 0;					// clr lmt_dw_w	********	En Ensamblador se borran solo 8 bits de una variable de 16 bits
			md_signfrac_math();				//				********	CGM 22/Nov/2024
}
//;				------------------------------------------------------------
void md_signfrac_math(){
	 	 	 if(btn_pr[b1_f1]){//btjt btn_pr,#b1_f1,inc_sf //
	 	 		 goto inc_sf;
	 	 	 }
	 	 	 if(btn_pr[b3_f1]){ //btjt btn_pr,#b3_f1,dec_sf //
	 	 		 goto dec_sf;
	 	 	 }

	 	 	 if(cnt_btn != 0){//tnz cnt_btn
	 	 		 goto fin_md_signfrac2_0;//jrne fin_md_signfrac2_0
	 	 	 }
	 	 	 cnt_btn = 25;//mov cnt_btn,#25
	 	 	 if(cnt_btn_hld != 0){//tnz cnt_btn_hld
	 	 		 goto md_signfrac2;//jrne md_signfrac2
	 	 	 }
	 	 	 cnt_btn = 10;//mov cnt_btn,#10
md_signfrac2:

			if(btn_pr[b1_f2]){ //btjt btn_pr,#b1_f2,inc_sf
				goto inc_sf;
			}
			if(btn_pr[b3_f2]){ //btjt btn_pr,#b3_f2,dec_sf
				goto dec_sf;
			}

			cnt_btn_hld = 5;//mov cnt_btn_hld,#5
fin_md_signfrac2_0:
			goto fin_md_signfrac2;

//;        ---- incremento cuando se presiona la tecla + de la decima  ----------
inc_sf:
		 	 if((int16_t)STM8_16_X < (int16_t)lmt_up_w){//cpw X,lmt_up_w *******************************?
			 	 goto inc_sf1; //jrslt inc_sf1
		 	 }
		 	 STM8_16_X = lmt_dw_w; //ldw X,lmt_dw_w   ; entonces carga el limite minimo para dar la vuelta
		 	 goto fin_md_signfrac;//jra fin_md_signfrac
inc_sf1:
			if(flagsb[f_nd_temp]){//btjt flagsb,#f_nd_temp,inc_simple_ent
				goto inc_simple_ent;
			}
			if((int16_t)STM8_16_X >= (int16_t)100 ){//cpw X,#100
				goto inc_ent;//jrsge inc_ent
			}
			if((int16_t)STM8_16_X < (int16_t)0xFF9C){//cpw X,#$FF9C
				goto inc_ent;//JRSLT inc_ent
			}

inc_simple_ent:
			STM8_16_X += 1;//addw X,#1
			goto fin_md_signfrac; //jra fin_md_signfrac
inc_ent:
			STM8_16_X += 10;//addw X,#10
			goto fin_md_signfrac; //jra fin_md_signfrac

//;        ---- decremento cuando se presiona la tecla + de la decima  ----------
dec_sf:
			if((int16_t)STM8_16_X > (int16_t)lmt_dw_w){//cpw X,lmt_dw_w
				goto dec_sf1;// jrsgt dec_sf1
			}
			STM8_16_X = lmt_up_w;//ldw  X,lmt_up_w ***********************************
			goto fin_md_signfrac;

dec_sf1:
			if(flagsb[f_nd_temp]){// btjt flagsb,#f_nd_temp,dec_simple_ent
				goto dec_simple_ent;
			}
			if((int16_t)STM8_16_X > (int16_t)100){ //cpw X,#100;
				goto dec_ent;//jrsgt dec_ent
			}
			if((int16_t)STM8_16_X <= (int16_t)0xFF9C){
				goto dec_ent; //jrsle dec_ent
			}

dec_simple_ent:
			STM8_16_X -= 1;//subw X,#1;
			goto fin_md_signfrac;

dec_ent:
			STM8_16_X -= 10;//subw x,#10



fin_md_signfrac:
			cnt_prog = 30;//mov cnt_prog, #30
fin_md_signfrac2:
			//ldw tempo2, X
			flagsb[f_nd_temp] = 0;//bres flagsb,#f_nd_temp			;// borra el proceso para dato de temperatura
			wreg = lowByte(STM8_16_X);//mov wreg,tempo1					;// mueve el dato a wreg cuando es simple
			//ret
}
/********************************************************************************************************
 * Convierte uint16_t foo = tdevl:tdevf
********************************************************************************************************/
//;  ----- _Rev STM32	CUBE IDE
uint16_t	tdev_to_Word(){
	uint16_t   foo;
	foo = (uint16_t)(tdevl * 256) + (uint16_t)(tdevf);
	return	foo;
}
//--------------------------------------------------------------------------------------------------
uint16_t	TwoByteInArrayToWord (uint8_t  *PointArray){
	uint16_t   foo;
	foo = (uint16_t)((PointArray[0]) * 256) + (uint16_t)((PointArray [1]));
	return	 foo;
}
//--------------------------------------------------------------------------------------------------
//void ADC_Select_CH5()
//{
//	ADC_ChannelConfTypeDef sConfig = {0};
//
//	hadc.Instance->CHSELR = (uint32_t) 0;
//	sConfig.Channel = ADC_CHANNEL_5;
//	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
//	HAL_ADC_ConfigChannel(&hadc, &sConfig);
//}
//--------------------------------------------------------------------------------------------------
//;Carga retardos para entrar a funcion nocturno.
//Load_ret:
//;							mov			retnoct,#$14	;	/ Carga el retardo para entrar a nocturno con 20 minutos
//;  ----- _Rev STM32	CUBE IDE
void Load_ret1(){
    asm ("nop");
    asm ("nop");

}
//;  ----- _Rev STM32	CUBE IDE
void Load_ret2 (){
	cntpah = Plantilla[timepa] *60;		 // Carga el contador de tiempo de puerta abierta
}
//;LN 5377	--------------------------------------------------------------------------------------------------
//;Rutina que carga el intervalo entre deshielos y el valor de comparación para modo adaptivo
//;  ----- _Rev STM32	CUBE IDE
void ldadaptivo(){
	interdhh = (Plantilla[interdh] * 3600) ;   // Toma el tiempo mínimo de interdeshielo en horas , 3600 Número de segundos por hora

	coontimeh = (Plantilla[timeadap] * (interdhh/8));  		// Toma el parámetro de tiempo adaptivo

	asm ("nop");
												// Interdeshielo completamente adaptivo
}
//;  ----- Manuel_Rev
//;  ----- _Rev STM32	CUBE IDE
void blink_640 (){
	uint16_t  foo = 0;
	foo = cntblkh;
	foo = foo << 1;
	foo = foo << 1;
	waux =  lowByte(foo);
	wreg = 	highByte(foo);				//wreg;			/ En wreg quedan los múltiplos de 640 ms
}
//;LN 5403	--------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void	sp_dpy (){				//sp_dpy:					;// manuel_math_change//	mov			tempo1,liminf_f ;// manuel_math_change//	liminff;
								// manuel_math_change//	mov			tempo2,liminf_i ;// manuel_math_change//	liminfl;
								//	ldw			X,liminf_w
								//			ldw     tempo2,X
	convadec_math(liminf_w);	//			call		convadec_math  ;// manuel_math_change//  convadec;		/ Despliega el Set Point que esta operando
								//			//ret	;
}
//;LN 5410	--------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void	dif_dpy (){					//	dif_dpy:			;// manuel_math_change// mov			tempo1,limsup_f ;// manuel_math_change// limsupf;	/ Toma límite superior
									//  manuel_math_change// mov			tempo2,limsup_i ;// manuel_math_change// limsupl;
	uint16_t  foo = 0;			// ldw     X,limsup_w
									// ldw     tempo2,X
									// manuel_math_change//  mov			wreg,liminf_f  ;// manuel_math_change//	liminff;	/ Toma límite inferior
									// manuel_math_change//  mov			waux,liminf_i  ;// manuel_math_change//	liminfl;
									// ldw     X,liminf_w
									// ldw     waux,X
	foo = limsup_w - liminf_w;	// call		restfbcd_math   ;// manuel_math_change//     restfbcd;		/ Resta
									// manuel_math_change//  call		convad10;		/ Despliega el Diferencial que este operando
	convadec_math(foo);			// call		conv_temper_positiva
									// ret ;
}
//;LN 5423	--------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void	minbrake_load (){			//	;Subrutina agregada para la optimización de código  >> IJG JULIO 2012
		wreg = Plantilla [minbrake];			// 	mov			wreg,minbrake;	/ Carga el tiempo mínimo de descanso del compresor
		BaBentre10_math();			// 	call		BaBentre10_math   ;// manuel_math_change//  BaBentre10;
		//  >>>>>>> ERROR, aqui no carga lo que hay en (waux:wreg)
		// tminstopl = resull;			//	mov			tminstopl,resull;
		// tminstoph = resulh;			//	mov			tminstoph,resulh;/ Carga el tiempo mínimo de descanso del compresor
		tminstoph = (resulh*256) + resull;			//;/ Carga el tiempo mínimo de descanso del compresor
}
//;LN 6429 --------------------------------------------------------------------------------------------------
//;Rutina que despliega el dato de temperatura contenido en tdevdpy
void	desptdv_math (){

			// Valor de display para despliegue de informacion
			// tdevdpy_w =  tsac_w;
			// Valor de display para despliegue de informacion
			uint16_t  foo = 0;

			foo = tdevdpy_w;

offsetdpy:

			//ldw			X,osdiur_w ;	/ Toma la constante de offset de diurno

off_diur:	//ldw     waux,X
			//call		sumafbcd_math   ;// manuel_math_change//    sumafbcd;
			//  waux:wreg = tempo2:tempo1 +  waux:wreg
			foo = 	foo	+ TwoByteInArrayToWord (&Plantilla[osdiur_H]);

			if(Plantilla[escala] != 0x20){
				goto	dpyTempCelcius;
			}
			if(!(foo & 0x8000)){					// / El dato es negativo?
				goto positivo2Fahrenheit;
			}
			foo = (~foo) +1;
			foo = foo/2;
			foo = foo * 18;
			foo = foo/5;
			foo = 320 - foo;
			goto	finConvFahrenheit;			//jra			finConvFahrenheit
positivo2Fahrenheit:
			foo = foo/2;
			foo = foo * 18;
			foo = foo/5;
			foo = foo + 320;
finConvFahrenheit:
			//ldw			tempo2,X

dpyTempCelcius:
			convadec_math(foo);						//;subrrutina para preparar los registros a mostrar temperatura

}


//;¡¡¡¡¡¡ Ojo. No quitar canvadec de aqui, porque es parte de esta rutina !!!!!!
//;--------------------------------------------------------------------------------------------------
//;Rutina que convierte un número signado, normalizado a 128 y lo despliega
//;El dato esta en los registros tempo1 para la fracción y tempo2 para el entero
//;============================================================
//;subrrutina para preparar los registros a mostrar temperatura
//;  ----- _Rev STM32	CUBE IDE
void convadec_math (uint16_t  tempo2){

		offdpy();				//call		offdpy;

		//-------Procesamiento temporal de temperatura-------------------
		uint8_t tempo_2 =0;
		uint8_t tempo_1 =0;

		if(!(tempo2 & 0x8000)){					// / El dato es negativo?
			goto conv_temper_positiva;
		}
      	//;manuel tratamiento para dato  negativo
		tempo2 = (~tempo2) +1;
		datled[sign] = 1;			//bset		datled,#1;0x02;	/ Enciende el signo


	   //   ;manuel tratamiento para la fraccion positiva
conv_temper_positiva:
		tempo_1 = (uint8_t)(tempo2%10);		// guarda la decena
		tempo_2 = (uint8_t)(tempo2/10);		// guarda el entero

		if(!datled[1]){//if(!GetRegFlagState(datled, 1)){						// Enciende el signo
			goto conv_signo_positivo;
		}
		wreg = 0x23;
		if(!flagsb[f_prog]){						// Esta en modo de programación???
			goto cp_25;
		}
		wreg = 0x64;
cp_25:
		if(tempo_2 >= wreg){			// La parte entera es Es mayor a wreg
			goto dpy_oL;			  // Despliega "-oL"
		}
		goto     convad10;
conv_signo_positivo:
		wreg = 0x63;
		if(!flagsb[f_prog]){						// Esta en modo de programación???
			goto cp_99;
		}
		wreg = 0x64;
cp_99:
		if(tempo_2 >= wreg){			// La parte entera es Es mayor a wreg
			goto dpy_oL;			  // Despliega "-oL"
		}
convad10:
		if(tempo_2 >= 10){						//  El entero es mayor a 10 °x
			goto soloent;
		}
fraccion:
		datled[dp]=1;//BitSet(datled,dp);;				// bset		datled,#0;0x01;	/ Enciende el punto decimal
		//	ldw   	X,tempo2;	/ Despliega el entero en el dígito1
		//	jrne		dpy_sign_no_00;
		//	bres		datled,#1;0x02;	/ Apaga el signo
		//	dpy_sign_no_00:	swapw  	X
dpy_sign_no_00:
			op_menu (tempo_2,tempo_1);
			//datdig1 = tempo_2;
			//datdig2 = tempo_1;
			goto	finconvad;

soloent:
		wreg = tempo_2; 		//mov			wreg,tempo2;

soloent1:
		BaBentre10_math();		// call		BaBentre10_math
		op_menu (waux,wreg);
		//datdig1 = waux;			//	tempo_2/10;
		//datdig2 = wreg;			//	tempo_2%10;
		goto	finconvad;

dpy_oL:
		if (GetRegFlagState(lowByte(cntblkh), 5)){
			goto convadec_j02;
		}
		offdpy ();			//jp			offdpy;
		goto	finconvad;
convadec_j02:
		op_menu (0x10,0x11);
		//datdig1 = 0x10;		//mov			datdig1,#$10;  	/ Coloca el dato para desplegar la letra o
		//datdig2 = 0x11;		//mov			datdig2,#$11 ; / Coloca el dato para desplegar la letra L
finconvad:
}
//-------------------------------------------
void convad10(uint8_t tempo_2){

	uint8_t tempo_1 =0;

		if(tempo_2 >= 10){						//  El entero es mayor a 10 °x
			goto soloent;
		}
fraccion:
		datled[0] = 1;//BitSet(datled,0);;				// Enciende punto decimal
		//	ldw   	X,tempo2;	/ Despliega el entero en el dígito1
		//	jrne		dpy_sign_no_00;
		//	bres		datled,#1;0x02;	/ Apaga el signo
		//	dpy_sign_no_00:	swapw  	X
		op_menu (tempo_2,tempo_1);
		//datdig1 = tempo_2;
		//datdig2 = tempo_1;
		goto	finconvad;

soloent:
		wreg = tempo_2; 		//mov			wreg,tempo2;

soloent1:
		BaBentre10_math();		// call		BaBentre10_math
		op_menu (waux,wreg);
		//datdig1 = waux;			//	tempo_2/10;
		//datdig2 = wreg;			//	tempo_2%10;
		goto	finconvad;

dpy_oL:
		if ((lowByte(cntblkh) & (1 << 5)) == 1){
			goto convadec_j02;
		}
		offdpy ();			//jp			offdpy;
		goto	finconvad;
convadec_j02:
		op_menu (0x10,0x11);
		//datdig1 = 0x10;		//mov			datdig1,#$10;  	/ Coloca el dato para desplegar la letra o
		//datdig2 = 0x11;		//mov			datdig2,#$11 ; / Coloca el dato para desplegar la letra L
finconvad:
}
//-------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void soloent (uint8_t  tempo2){

soloent:
		wreg = tempo2; 		//mov			wreg,tempo_2;

soloent1:
		BaBentre10_math();		// call		BaBentre10_math
		op_menu (waux,wreg);
		//datdig1 = waux;			//	tempo_2/10;
		//datdig2 = wreg;			//	tempo_2%10;
		goto	finconvad;

dpy_oL:
		if ((lowByte(cntblkh) & (1 << 5)) == 1){
			goto convadec_j02;
		}
		offdpy ();			//jp			offdpy;
		goto	finconvad;
convadec_j02:
		op_menu (0x10,0x11);
		//datdig1 = 0x10;		//mov			datdig1,#$10;  	/ Coloca el dato para desplegar la letra o
		//datdig2 = 0x11;		//mov			datdig2,#$11 ; / Coloca el dato para desplegar la letra L
finconvad:
}
//-------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void soloent1 (){

		BaBentre10_math();		// call		BaBentre10_math
		op_menu (waux,wreg);
		//datdig1 = waux;			//	tempo_2/10;
		//datdig2 = wreg;			//	tempo_2%10;
		goto	finconvad;

dpy_oL:
		if ((lowByte(cntblkh) & (1 << 5)) == 1){
			goto convadec_j02;
		}
		offdpy ();			//jp			offdpy;
		goto	finconvad;
convadec_j02:
		op_menu (0x10,0x11);
		//datdig1 = 0x10;		//mov			datdig1,#$10;  	/ Coloca el dato para desplegar la letra o
		//datdig2 = 0x11;		//mov			datdig2,#$11 ; / Coloca el dato para desplegar la letra L
finconvad:
}
//;LN 6574 Rutina que convierte un dato de tipo Byte a la forma Byte/10
//;en wreg esta el dato a convertir, y se regresa en waux las decenas y en wreg el residuo
//;--------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void	BaBentre10_math(void){
		waux = 0;						//clr		 waux;    waux:wreg   ; solo trabaja sobre WREG
		STM8_16_X = (uint16_t)wreg;
		wreg =  (uint8_t)STM8_16_X%10;				// wreg el residuo
		waux = (uint8_t)STM8_16_X/10;					// waux las decenas
		conv60_6_math();
}
//;--------------------------------------------------------------------------------------------------
//;Rutina que convierte un dato Byte/10 en formato 60_6 a valor de cuenta
//;en waux_wreg esta el dato a convertir, y se regresa el valor en resulh:resull
//;wreg y waux no sufern cambio
//;¡¡¡¡¡¡OJO!!!!!!   NO QUITAR DE AQUI PORQUE ES CONTINUACIÓN DE LA RUTINA ANTERIOR
//;  ----- Manuel_Rev
void conv60_6_math (void) {
	uint16_t  foo = 0;
	foo = (uint16_t)(6 * wreg); 						// ldw     X,#$0006, Convierte el Byte de residuo a valor de cuenta
	foo = foo + (uint16_t)(60* waux);		// Convierte el Byte de decenas a valor de cuenta
	resulh = highByte(foo);
	resull= lowByte(foo);
}
//;LN 4928 --------------------------------------------------------------------------------------------------
//;Rutina que determina el tipo de fallas a señalizar y desplegar
//;  ----- Manuel_Rev
fallas(){						//fallas:

		flagsBuzzer[0] = 0;		//bres flagsBuzzer,#0

	    if(edorefri != 0){ //tnz edorefri
	      	 goto fallas10; //jrne fallas10
	    }
finfallas_00:
	    goto finfallas; //jp finfallas

fallas10:
	    if(edorefri == 1){//cp A,#1;
	      	   goto fallas11;//jreq fallas11;
	    }
	    datled[5] = 0;			 //bres datled,#5

fallas11:

		for(int k=0; k<8;k++){
			if((trefst[k] ) | (trefst2[k]) | (flagsBattery [k])){//	tnz trefst,	tnz     trefst2,	tnz			flagsBattery
				goto fallas12;//jrne fallas12
			}
		}

finfallas_01:
	    goto finfallas_00; //  jra finfallas_00

fallas12:
	    if(fauto[0]){//btjt fauto,#0,fallas16
	       	goto fallas16;//
	    }
	    datled[5] = 1;			//bset datled,#5
	    flagsBuzzer[0] = 1;//bset flagsBuzzer,#0
	    waux = 0x1F;//mov waux,#$1F;
	    if(!trefst[f_sac]){//btjf trefst,#f_sac,fallas_j00
	       	goto fallas_j00;
	    }
	    goto fallas15; //jra fallas15;

fallas_j00:
	    if(trefst[f_saa]){//btjt trefst,#f_saa,fallas_j01
	       	goto fallas_j01;
	    }
	    goto fallas20;//jra fallas20

fallas_j01:
	    waux = 0x1F;//mov waux,#$1F

fallas15:
	    // tempo1 = waux;//mov tempo1, waux
	    //call Load_ret;		No hace nada en ensamblador
	    load_tiempoAhorro1();	//	call load_tiempoAhorro1;
	    load_tiempoAhorro2();	//call		load_tiempoAhorro2;
	    // waux = tempo1;//mov waux,tempo1;

fallas_j02:
		datled_clear();
		//BitClear(datled,0);			//bres datled,#0
		//BitClear(datled,1);			//bres datled,#1;
		wreg = 0x13;//mov wreg,#$13;
		op_menu (waux,wreg);
		//datdig1 = waux;//mov datdig1,waux;
		//datdig2 = wreg;//mov datdig2,wreg;
fallas16:
		goto finfallas;//jp finfallas

fallas20:
		if(!trefst[f_hv]){//btjf trefst,#f_hv,fallas_j07;
			goto fallas_j07;
		}
		wreg_waux_conf(0x1F, 0x27, 1);
		//wreg = 0x1F;//mov wreg,#$1F;			" "
		//waux = 0X27;//mov waux,#$27;			"U"
		//BitSet(flagsBuzzer,0);//bset flagsBuzzer,#0
fallas_j07:
		if(!trefst[f_lv]){//btjf trefst,#f_lv,fallas_j08
			goto fallas_j08;
		}
		wreg_waux_conf(0x26, 0x27, 1);
		//wreg = 0x26;//mov wreg,#$26;			"-"
		//waux = 0x27;//mov waux,#$27;			"U"
		//BitSet(flagsBuzzer,0);//bset flagsBuzzer,#0
fallas_j08:
		if(!trefst2[f_tvolt]){//btjf trefst2,#f_tvolt,fallas_j08b
			goto fallas_j08b;
		}
		wreg_waux_conf(0x0E, 0x2F, 0);
		//wreg = 0x0E;//mov wreg,#$0E;			"E"
		//waux = 0x2F;//mov waux,#$2F;			"u"
		//BitClear(flagsBuzzer,0);//bres	flagsBuzzer,#0
fallas_j08b:
		if(!flagsBattery[batON]){		//btjf flagsBattery,#batON,fallas_j08c
			goto fallas_j08c;
		}
		wreg_waux_conf(0x0B, 0x0A, 1);
		//wreg = 0x0B;//mov	wreg,#$0B;			"B"
		//waux = 0x0A;//mov	waux,#$0A;			"A"
		//BitSet(flagsBuzzer, 0);//bset flagsBuzzer,#0
fallas_j08c:
		if(trefst[f_s4short]){//btjt trefst,#f_s4short,fallas_j01c
			goto fallas_j01c;
		}
		if(!trefst2[f_s4open]){//btjf trefst2,#f_s4open,fallas_j02c
			goto fallas_j02c;
		}
fallas_j01c:
		wreg_waux_conf(0x1F, 0x13, 1);
        //wreg = 0x1F;//mov wreg,#$1F			" "
        //waux = 0x13;//mov	waux,#$13;		"H"
        //BitSet(flagsBuzzer, 0);//bset flagsBuzzer,#0
fallas_j02c:

		if(trefst2[f_s3short]){//btjt trefst2,#f_s3short,fallas_j01b
	       	  goto fallas_j01b;
	    }
	    if(!trefst2[f_s3open]){//btjf trefst2,#f_s3open,fallas_j02b
	       	  goto fallas_j02b;
	    }
fallas_j01b:
		wreg_waux_conf(0x1F, 0x13, 1);
		//wreg = 0x1F;//mov wreg,#$1F;		" "
	    //waux = 0x13;//mov waux,#$13			"H"
	    //BitSet(flagsBuzzer, 0);//bset flagsBuzzer,#0
fallas_j02b:
		if(trefst[f_sdc]){//btjt trefst,#f_sdc,fallas_j03
			goto fallas_j03;
		}
		if(!trefst[f_sda]){//btjf trefst,#f_sda,fallas_j04
			goto fallas_j04;
		}
fallas_j03:
		 wreg_waux_conf(0x1F, 0X13, 1);
	     //wreg = 0x1F;//			" "
	     //waux = 0X13;//			"H"
	     //BitSet(flagsBuzzer, 0); //bset flagsBuzzer,#0
fallas_j04:

fallas_j05:
		if(!trefst[f_pa]){//btjf trefst,#f_pa,fallas_j06
			goto fallas_j06;
		}
		wreg_waux_conf(0X1F, 0x1E, 1);
	    //wreg = 0X1F;//		" "
	    //waux = 0x1E;//		"P"
	    //BitSet(flagsBuzzer, 0);//bset	flagsBuzzer,#0
fallas_j06:
		if(!trefst2[f_defi]){ //btjf trefst2,#f_defi,fallas_j06a
			goto fallas_j06a;
		}
		wreg_waux_conf(0x1F, 0X0A, 1);
		//wreg = 0x1F;			//" "
		//waux = 0X0A;			//"A"
		//BitSet(flagsBuzzer,0); //bset	flagsBuzzer,#0
fallas_j06a:

		if(!trefst2[f_ambHi]){//btjf trefst2,#f_ambHi,fallas_j04b
			goto fallas_j04b;
		}
		wreg_waux_conf(0x20, 0x0C, 1);
		//wreg = 0x20;//		"t"
		//waux = 0x0C;//		"C"
		//BitSet(flagsBuzzer, 0); //bset	flagsBuzzer,#0
fallas_j04b:
		if(!trefst2[f_ambLo]){//btjf	trefst2,#f_ambLo,fallas_j04c
			goto fallas_j04c;
		}
		wreg_waux_conf(0x20, 0x0B, 1);
		//wreg = 0x20;//			"t"
		//waux = 0x0B;//			"b"
		//BitSet(flagsBuzzer, 0); //bset flagsBuzzer,#0
fallas_j04c:

		if(!trefst2[f_co_ex]){ //btjf trefst2,#f_co_ex,fallas_j06b
			goto fallas_j06b;
		}
		wreg_waux_conf(0x1F, 0X0C, 1);
		//wreg = 0x1F;//		" "
		//waux = 0X0C;//		"C"
		//BitSet(flagsBuzzer, 0); //bset flagsBuzzer,#0

fallas_j06b:
	              //
parpadeo2:
		if(edorefri == 1 ){
			goto parpadeo2a;
		}

		if((highByte(cntblkh) & Plantilla[nivDpyFail]) == 0){
			goto parpadeo2a;
		}
		goto finfallas;

parpadeo2a:

		goto fallas_j09; //jra	fallas_j09;
		if(GetRegFlagState(lowByte(cntblkh),6)){ //btjt cntblkl,#6,fallas_j09;
			goto fallas_j09;
		}
offdpy:   waux = 0x1F;
	      wreg = 0x1F;
fallas_j09:
		op_menu (wreg,waux);
		//datdig1 = wreg;
		//datdig2 = waux;
								// ld      A,datled
								//	and		A,#$FC;0xBC;	/ Apaga el punto y el signo	 >> IJG JULIO 2012
		datled[0] = 0;			//	ld      datled,A
		datled[1] = 0;
finfallas:


}
void offdpy (){				//	offdpy:
							//	mov			waux,#$1F;		/ Apaga los dígitos
							//	mov			wreg,#$1F;		/ Apaga los dígitos
							//	fallas_j09:		;mov			datdig1,waux;
							//					;mov			datdig2,waux;}
	op_menu (0x1F,0x1F);
	//datdig1 = 0x1F;				//mov			datdig1,wreg;
	//datdig2 = 0x1F;				//mov			datdig2,waux;
							//	ld      A,datled
							//	and		A,#$FC;0xBC;	/ Apaga el punto y el signo	 >> IJG JULIO 2012
	//datled &= 0xFC;			//	ld      datled,A
							//	finfallas:		ret ;
	datled[0]=0;
	datled[1]=0;
}


//;=====================================================================
//;	Funcion que graba por Bloques en la EEPROM o en Flash
//;	Para que esta rutina Funcione se debe pasar esta sección del Código a RAM.
//;	Debe trabajar en conjunto con la Funcion GRABA_EEPROM
//;=====================================================================
// Rutina completa Adaptada <<_RGM
//GRABA_FLASH_EEPR:
void ram_reserved(){

	uint64_t * AddressDestination;
  	uint64_t Data ;


		//CLRW			X																		;	Borra los registro indexados
		//CLRW			Y
		//LDW				X,dirPointer												;	Apunta la direccion a grabar
		//LDW				Y,dataPointer												;	Apunta la direccion de ram donde se eencuentran los datos a grabar
sig_blocking:
		//CLR				cntByteLoaded												;	Borra el contador de bytes cargados
		//BSET			FLASH_CR2,#0												;	Habilita el block programming


		if( ProgMemCode != 0xAA){	//LD				A,ProgMemCode												;	Estamos en el modo bootloader??
			goto PROG_eepr_mode;		//JRNE			PROG_eepr_mode											;	No, entonces programa la eeprom
		}

		 //while( HAL_FLASH_Unlock() !=  HAL_OK );
//					;BTJT			ULT_block,PROG_eepr_mode						;	Es el último bloque del archivo??, si, entonces programa en eeprom
//					BTJT			FLASH_IAPSR,#1,GRABA_FLASH					;	Esta protegida la memoria Flash??, si, entonces ve a grabar la memoria
//MEM_FLASH_LOCKED:	LD				A,#$56
//					LD				FLASH_PUKR,A												;	Cambiar este registro a FLASH_DUKR para habilitar la escritura en EEPROM
//					LD				A,#$AE
//					LD				FLASH_PUKR,A												;	Pon la llave para desbloquear la escritura en flash
//					BTJF			FLASH_IAPSR,#1,MEM_FLASH_LOCKED			;	Si la Flash no esta desbloqueada entonces repite la llave
//					;JRA				MEM_UNLOCKED1												;	Si ya esta desbloqueada ve a grabar en la memoria
//


GRABA_FLASH:

			HAL_IWDG_Refresh( &hiwdg );				//MOV				IWDG_KR,#$AA
			// Para realizar una sobreescritura en la Flash, es necesario borrar la pagina antes
			// CGM 04/12/2024

			uint32_t VarAux_= (((uint32_t) dirPointer) - 0x8000000);
			if(VarAux_ % 2048 == 0){
				uint32_t numberPage = VarAux_/2048; 		// Number the Page
				uint32_t Error_ = 0;
				FLASH_EraseInitTypeDef pEraseInit = {0};
				pEraseInit.NbPages = 1;
				pEraseInit.Page = numberPage;
				pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;

				while( HAL_FLASH_Unlock() !=  HAL_OK );
				HAL_FLASHEx_Erase(&pEraseInit,&Error_);		// Erase the Page
				while( HAL_FLASH_Lock() !=  HAL_OK);
			}
			AddressDestination = (uint64_t *) dirPointer;
			//while( HAL_FLASH_Unlock() !=  HAL_OK );
			// Graba 128 Bytes de FLASH
			for(uint8_t i = 0; i < 16 ; i++ ){
			   	// Data = (uint32_t)(*dataPointer);

				Data  	 =  (uint64_t)(*dataPointer);						dataPointer++;
				Data  	+= ((uint64_t)(*dataPointer))	<<8;				dataPointer++;
				Data  	+= ((uint64_t)(*dataPointer)) 	<<16;				dataPointer++;
				Data	+= ((uint64_t)(*dataPointer))  	<<24;			   	dataPointer++;
				Data	+= ((uint64_t)(*dataPointer))  	<<32;			   	dataPointer++;
				Data	+= ((uint64_t)(*dataPointer))  	<<40;			   	dataPointer++;
				Data	+= ((uint64_t)(*dataPointer))  	<<48;			   	dataPointer++;
				Data	+= ((uint64_t)(*dataPointer))  	<<56;			   	dataPointer++;

				while( HAL_FLASH_Unlock() !=  HAL_OK );
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) AddressDestination, Data);
				while( HAL_FLASH_Lock() !=  HAL_OK);
				AddressDestination++;
				//if(AddressDestination>=0x801d000)
				//	break;
//				while( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressDestination, Data) !=  HAL_OK );		// HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
			   	//dirPointer +=4;
				HAL_IWDG_Refresh( &hiwdg );				// evita time out watch


				asm ("nop");


			}
//			LD				A,(Y)																;	Carga el byte a grabar
//			;LD				(X),A
//			LDF				($010000,X),A
//			INCW			X																		;	Incrementa las direcciones y el contador apuntador de grabacion
//			INCW			Y
//			INC				cntByteLoaded
//			LD				A,cntByteLoaded											;	Si el contador apuntador ya es 63 entonces espera a que la grabacion se ejecute
//			CP				A,#127															;	Si no entonces ve a cargar el siguiente dato
//			JRULE			GRABA_FLASH
//			;CP				A,#0															;	Si no entonces ve a cargar el siguiente dato
//			;JRNE			GRABA_FLASH
			asm ("nop");
			goto	WAIT_FOR_GRAB_0;							//			JRA				WAIT_FOR_GRAB_0



PROG_eepr_mode:

			//while( HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK );
//BTJT			FLASH_IAPSR,#3,MEM_UNLOCKED1
//MEM_LOCKED1:						LD				A,#$AE
//												LD				FLASH_DUKR,A												;	Cambiar este registro a FLASH_PUKR para habilitar la escritura en FLASH
//												LD				A,#$56
//												LD				FLASH_DUKR,A												;	Pon la llave para desbloquear la escritura en EEPROM
//check_bit_enable:																											;	Si la EEPROM no esta desbloqueada entonces repite la llave
//												BTJF			FLASH_IAPSR,#3,PROG_eepr_mode
//MEM_UNLOCKED1:

GRABA_SIG:
			HAL_IWDG_Refresh( &hiwdg );				//MOV				IWDG_KR,#$AA

//			LD				A,(Y)																;	Carga el byte a grabar
//			LD				(X),A
//			INCW			X																		;	Incrementa las direcciones y el contador apuntador de grabacion
//			INCW			Y

//			INC				cntByteLoaded
//			LD				A,cntByteLoaded											;	Si el contador apuntador ya es 63 entonces espera a que la grabacion se ejecute
//			;CP				A,#$3F															;	Si no entonces ve a cargar el siguiente dato
//			CP				A,#127															;	Si no entonces ve a cargar el siguiente dato
//			JRULE			GRABA_SIG

			// Graba 128 Bytes  de Data Memory
//			for(uint8_t i = 0; i < 32 ; i++ ){
//				AddressDestination = dirPointer;
//			   	// Data = (uint32_t)(*dataPointer);
//
//				Data  =  (uint32_t)(*dataPointer);						dataPointer++;
//				Data  += ((uint32_t)(*dataPointer)) <<8;				dataPointer++;
//				Data  += ((uint32_t)(*dataPointer)) <<16;				dataPointer++;
//				Data  += ((uint32_t)(*dataPointer))  <<24;			   	dataPointer++;
//
////				while( HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, AddressDestination, Data) !=  HAL_OK );		// HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
//
//				dirPointer +=4;
//				HAL_IWDG_Refresh( &hiwdg );				// evita time out watch
//				asm ("nop");
//			}
			HAL_IWDG_Refresh( &hiwdg );
			for(uint8_t i = 0; i < 128 ; i++){
				FlashManager((uint32_t)dirPointer, (uint32_t)*dataPointer);
				dataPointer++;
				dirPointer++;
				HAL_IWDG_Refresh( &hiwdg );
			}



WAIT_FOR_GRAB_0:

WAIT_FOR_GRAB:
			HAL_IWDG_Refresh( &hiwdg );		//MOV				IWDG_KR,#$AA

//			BTJF	TIM4_SR1,#0,noOvfTIM4;				/ Ya terminó el intervalo
//			BRES	TIM4_SR1,#0;								/ Apaga la bandera de sobreflujo
//			inc		waux


//		btjf 	  puertoa,#rel_fn,put_rel_dh_low_2
//		btjf		waux,#0,put_rel_dh_low_2;		/ El contador de procesos esta en un valor non?
//		bset		PE_ODR,#rel_dh;	/ Borra el puerto para generar tono
//		jra			end_rel_dh_2
//put_rel_dh_low_2:
//		bres		PE_ODR,#rel_dh;	/ Borra el puerto para generar tono
//end_rel_dh_2:


noOvfTIM4:
//		BTJF			FLASH_IAPSR,#2,WAIT_FOR_GRAB				;	Si no ha terminado la grabación entonces espera


//		BRES			FLASH_IAPSR,#1
//		BRES			FLASH_IAPSR,#3
		//HAL_FLASH_Lock();
		//HAL_FLASHEx_DATAEEPROM_Lock();
//		JP				Return_RAM													;	Devuelve la funcion a la ejecucion en el main Program
//GRABA_FLASH_EEPR_00:

}
//----------------------------------------------------------------------------------------------
//;LN 7001 --------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void load_tiempoAhorro1(){
	uint16_t	foo = 0;
	foo= findLastValue((uint32_t) &eePlantilla[eetiempoAhorro1])*360;
	//foo = eePlantilla[eetiempoAhorro1]*360;		//	mov			wreg,eetiempoAhorro1;	/ Toma el tiempo para entrar a Ahorro1
													//	ldw			Y,#360;		/ Número de segundos por hora (entre 10)
													//	;ldw			Y,#30;#60;		/ Número de segundos por minuto (para prueba solamente)
													//	call		mult1x2;		/ Multiplicalos
													//	mov			t_ahorro1_L,resull;
													//	mov			t_ahorro1_H,resulh;/ Carga el Tiempo total en segundos
	t_ahorro1_H = foo;							//	ret
}
//;LN 7020 --------------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void load_tiempoAhorro2(){
		uint16_t	foo = 0;
		foo= findLastValue((uint32_t) &eePlantilla[eetiempoAhorro2])*360;
		//foo = eePlantilla[eetiempoAhorro2]*360;		//	mov			wreg,eetiempoAhorro2;	/ Toma el tiempo para entrar a Ahorro2
														//	ldw			Y,#360;		/ Número de segundos por hora  (entre 10)
														//	;ldw			Y,#30;#60;		/ Número de segundos por minuto (para prueba solamente)
														//	call		mult1x2;		/ Multiplicalos
														//	mov			t_ahorro2_L,resull;
														//	mov			t_ahorro2_H,resulh;/ Carga el Tiempo total en segundos
		t_ahorro2_H = foo;							//	ret ;
}

void load_timeNoct(){
	//cntNoct_H = eePlantilla[eetimeNoct] * 60;
	cntNoct_H = findLastValue((uint32_t) &eePlantilla[eetimeNoct])*60;
}

//;=====================================================================
//;	SUBRUTINA QUE CARGA EL BUFFER DE DATOS CON LOS DATOS DEL SIGUIENTE BLOQUE A GRABAR
//;
//;=====================================================================
//Rev_RGM: 06-NOV-2024_RGM
void load_next_buffer (void){
	uint8_t *point_X;
	uint8_t *point_Y;

	point_Y = dirBuffer;							// apunta al buffer de datos en RAM
	STM8_16_X = cntBlockFlash * 128;
	point_X = &dirLogger[STM8_16_X];	//	apunta al inicio de la Flash resevada para Logger de datos + el número de bloques grabados
	wreg = 0;
load_next_buffer_01:
	(*point_Y) = (*point_X);
	point_X++;
	point_Y++;
	wreg++;
	if(wreg < 128){
		goto load_next_buffer_01;
	}
    asm ("nop");
}
//;=====================================================================
//;	SUBRUTINA QUE GRABA CONTADORES DE REGISTROS LOGGEADOS.
//;  (LogTiempo:9 x N Cantidad)   (LogEventos:14 x N Cantidad)
//  07-Nov-2024 RGM: Valor usado ????
// Rutina completa Adaptada <<_RGM_Funciona
//;=====================================================================
// Rutina completa Adaptada <<_RGM
void	save_cntReg (){
		//		ldw		X,cntReg
		//		ldw		resulh,X

		uint32_t *point_X;
		point_X = (uint32_t *) (cntRegPNT);

		waux = highByte(cntReg);			//		mov		waux,resulh;
		//		ldw		X,cntRegPNT;
		wreeprom (waux, point_X);		//wreeprom (waux, cntRegPNT);			//		call	wreeprom;						/ ejecuta el grabado
		HAL_IWDG_Refresh( &hiwdg );			//  	MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria
		//
		waux = lowByte(cntReg);		// 		mov		waux,resull;
		point_X++;  // cntRegPNT++;						//		incw	X
		wreeprom (waux, point_X);		//wreeprom (waux, cntRegPNT);			//		call	wreeprom;						/ ejecuta el grabado
		HAL_IWDG_Refresh( &hiwdg );			//		MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria
		//
//		ret
}
//;=====================================================================
//;	SUBRUTINA QUE GRABA TIEMPO UNIX EN EEPROM
//;
//;=====================================================================
// Rutina completa Adaptada <<_RGM_Funciona
void save_timeUNIX (){

	//	ldw		X,timeSeconds_HW
	//	ldw		resulh,X

	waux = highByte(timeSeconds_HW);	//	mov		waux,resulh;
	//  ldw		X,#eeTimeUnix1;
	wreeprom (waux, &eeTimeUnix1);		//  call	wreeprom;				/ ejecuta el grabado
	HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

	waux = lowByte(timeSeconds_HW);		// mov		waux,resull;
	//	ldw		X,#eeTimeUnix2;
	wreeprom (waux, &eeTimeUnix2);		//  call	wreeprom;				/ ejecuta el grabado
	HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria


	//	ldw		X,timeSeconds_LW
	//	ldw		resulh,X

	waux = highByte(timeSeconds_LW);	//	mov		waux,resulh;
	//	ldw		X,#eeTimeUnix3;
	wreeprom (waux, &eeTimeUnix3);		// call	wreeprom;					/ ejecuta el grabado
	HAL_IWDG_Refresh( &hiwdg );			// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

	waux = lowByte(timeSeconds_LW);		// mov		waux,resull;
	// ldw		X,#eeTimeUnix4;
	wreeprom (waux, &eeTimeUnix4);   	// call	wreeprom;					/ ejecuta el grabado
	HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

}
/*;=====================================================================
;	SUBRUTINA QUE calcula el checksum de un bloque
;	en X se apunta al inicio del bloque y waux contiene el tamaño del bloque
;=====================================================================*/
void buildChksumBloq (uint8_t *point_X , uint8_t waux){

	uint8_t A_STM8;
	while (waux){
		A_STM8 = *point_X;
		build_chksum(A_STM8);
		*point_X++;
		waux--;
	}
/*  para que cargarlos en "resulh:resull" y "tempo2:tempo2", se usan con otro Fin ???
								ld		a,(X)
								ld		resulh,a
								incw	X
								ld		a,(X)
								ld		resull,a;							/ carga los 16 bits más significativos del checksum transmitido
								incw	X
								ld		a,(X)
								ld		tempo2,a
								incw	X
								ld		a,(X)
								ld		tempo1,a;							/ carga los 16 bits más significativos del checksum transmitido

*
*/
}
//;------ Adaptacion Completa C ---------------------------------------------------------------------------------------
void	ld_alarmDelay (){
	//Plantilla[alarmDelay]			/ Toma el tiempo de silencio de alarma en minutos
	//	ldw			X,#$003C;		/ Número de segundos por minuto
	silencioAlarmH = Plantilla[alarmDelay] *60; 	// Carga el tiempo que durara el deshielo en segundos
}

/*
 * =====================================================================
 *	SUBRUTINA QUE LIMPIA EL BUFFER DE DATOS DE RAM
 * =====================================================================
 * CGM 26/11/2024
*/
void clean_buffer (){
	for(int i = 0; i<128; i++){
		data_buffer[i] = 0 ;
	}
}

//;=====================================================================
//;	SUBRUTINA para copiar vectores X es la direccion de origen Y dirección de destino wreg tamaño del vecto
//;			CGM 26/11/2024
//;=====================================================================
void copyVector(uint8_t *srcX, uint8_t *dstY){
	for(int i=0; i<wreg; i++){
		*dstY = *srcX;
		srcX++;
		dstY++;
	}
	wreg = 0;
}
/*;=====================================================================
;	SUBRUTINA para calcular checksum. el dato debe estar en el acumulador
;
;=====================================================================*/
void build_chksum(uint8_t A_STM8){
    asm ("nop");
	// uint32_t A_STM8 = (uint32_t)(*point_Y);
    chksum_32_HW_LW = chksum_32_HW_LW +  (uint32_t)(A_STM8);
}
//;=====================================================================
//;	SUBRUTINA QUE GUARDA LOS DATOS DEL PROGRAMA EN EEPROM
//;	Carga inicialmente el programa a la seccion de ram para ejecutarla
//;=====================================================================
// Rutina completa Adaptada <<_RGM
void GRABA_BLOCK(){
/*
  *
  * Funcion que copia Funcion de escritura en FLASH a RAM
  *
 */






		ram_reserved();		//JP				ram_reserved // DESCOMENTAR
Return_RAM:

}
//;LN 4676 --------------------------------------------------------------------------------------------------
//;Subrrutina que decrementa una palabra apuntada por x
//;  ----- _Rev STM32	CUBE IDE
void decword(uint16_t *decwreg_Reg){
	if (*decwreg_Reg)
		(*decwreg_Reg)--;
}
//;Subrrutina que decrementa el registro wreg y si ya es cero se sale
//;..................................................completo
//;  ----- _Rev STM32	CUBE IDE
void decwreg(uint8_t *decwreg_Reg){
	if (*decwreg_Reg)
		(*decwreg_Reg)--;
}
//;  ----- Manuel_Rev   ----------------------------------------------------------------------------------------------
//;  ----- _Rev STM32	CUBE IDE
void buildmode(){
			waux = 0;				//clr			waux;

			if(latchtimeh != 0){
				goto build10;			 //build10;		/ Ya terminó el tiempo del estado vigente?
			}
			BitSet(waux,0);				//bset		waux,#0;0x01;		/ Si, indicalo
build10:

			int16_t foo = 0;
			foo = (Plantilla[limambch_H]*256) + Plantilla[limambch_L];	//  manuel_math_change//						/ Temperatura ambiente para terminar deshielo
			STM8_16_X = (tdevl*256) + tdevf;										//  manuel_math_change//
			if( (int16_t)STM8_16_X < (int16_t)foo ){
				goto build20;				//JRSLT		build20;		/ Esta por debajo de la temperatura límite?
			}
			BitSet(waux,1);				//bset		waux,#1;0x02;		/ No, Indicalo

build20:

			if(Plantilla[numSens] == 1){
				goto	build30;				//	jreq		build30; 				 / Si el número de sensores con el que trabaja es 1, no preguntes por temperatura evaporador
			}

			STM8_16_X = (teval*256) + tevaf;										 //  manuel_math_change//
			foo = (Plantilla[limevach_H]*256) + Plantilla[limevach_L];	//  manuel_math_change// / Temperatura de evaporador para terminar deshielo
			if( (int16_t)STM8_16_X < (int16_t)foo ){
				goto build30;				//JRSLT		build30;		/ La temperatura del evaporador es menor que el límite esperado?
			}
			BitSet(waux,4);						//bset 		waux,#4;0x10;		/ No, Indicalo

build30:
			if(!flagsa[arran]){//if(!GetRegFlagState(flagsa, arran)){
				goto	buildmode_j00;			//buildmode_j00;	/ Esta en período de arranque?
			}
			BitSet(waux,7);						//bset		waux,#7;0x80;		/ Si, indícalo
buildmode_j00:
finbuild:	modo = waux;						//mov			modo,waux ;		/ Entrega el nuevo modo de cambio

}
//;LN 4774 --------------------------------------------------------------------------------------------------
//;Rutina que carga los datos contanidos en la EEPROM hacia la RAM, ó graba los datos de la RAM hacia
//;la EEPROM, dependiendo del registro de control cltmemo. Para mandar a grabar se hace ctlmemo=0xAA
//;Hace un movimiento cada 1/64 s
//;  ----- Manuel_Rev
//;..................................................
void memodriver  (){

			uint8_t *point_Y = &Plantilla[dato_seg1];			// Apunta al inicio de la plantilla en RAM
			uint8_t *point_X = &eePlantilla[eedato_seg1];		// Apunta al inicio de la  eeprom

			if(ctlmemo != 0xAA){							// No, carga los datos de la EEPROM a la RAM
				goto  loaddat;
			}
grabmemo:
			if(cntmemo==Plantilla[dato_seg1]){
				goto nextdat;
			}
			wreeprom(copiaPlantilla[cntmemo],&eePlantilla[cntmemo]);
			goto nextdat;

loaddat:
			//call		rdeeprom;								// Toma el dato de la EEPROM
			//point_Y[cntmemo] = point_X[cntmemo]; 				// y cargalo a la RAM
			point_Y[cntmemo] = findLastValue((uint32_t)&eePlantilla[cntmemo]);
nextdat:	cntmemo++;							// Para apuntar al siguiente dato
			if(cntmemo < Fam_ID){
				goto finmemodr;
			}
			ctlmemo = 0;		//clr		 	ctlmemo ;		/ Deten la grabación de datos
firstdat:	cntmemo = 0;			//clr		 	cntmemo;			/ Si, reinicia el contador

finmemodr:

}
//;LN 4822 --------------------------------------------------------------------------------------------------
//;Rutina para escritura de la memoria EEPROM
//;En wreg debe venir la dirección en donde se desea grabar y en waux el dato a grabar
//;  ----- _Rev STM32	CUBE IDE
void wreeprom (uint8_t Data8bit, uint32_t AddressDestination) {
	uint32_t Data;
	Data = (uint32_t) Data8bit;

//	while( HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK);
//	while(HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data) != HAL_OK);
//	HAL_FLASHEx_DATAEEPROM_Lock();
	FlashManager((uint32_t)AddressDestination, (uint32_t)Data);
}

void op_menu (uint8_t dig1, uint8_t dig2)
{
	datdig1 = dig1;
	datdig2 = dig2;
}

void datled_clear()
{
	datled[0] = 0;//BitClear(datled,0);//bres datled,#0; / apaga el punto
	datled[1] = 0;//BitClear(datled,1);//bres datled,#1; / apaga el signo
}

void wreg_waux_conf(uint8_t wreg_1, uint8_t waux_2, _Bool cambio_rutina)
{
	wreg = wreg_1;//mov wreg,#$1F;			" "
	waux = waux_2;//mov waux,#$27;			"U"
	if(cambio_rutina)
		flagsBuzzer[0]=1;	// BitSet(flagsBuzzer,0);//bset flagsBuzzer,#0
	else
		flagsBuzzer[0]=0;	// BitClear(flagsBuzzer,0);//bset flagsBuzzer,#0
}


void opc_nv (uint16_t p_dato, uint16_t s_dato, uint16_t t_dato, uint8_t c_dato, uint8_t q_dato)
{

	lmt_up_w = p_dato;
	//ldw X,crngmin_w
	//ldw lmt_dw_w,X
	lmt_dw_w = s_dato;

	STM8_16_X = t_dato;
	md_signfrac_math();		//call md_signfrac_math
	copiaPlantilla[c_dato] =	  highByte(STM8_16_X);
	copiaPlantilla[q_dato] =      lowByte(STM8_16_X);
	convadec_math(STM8_16_X);		//call convadec_math

}


























