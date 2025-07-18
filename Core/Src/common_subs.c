

#include "main.h"
#include "customMain.h"
#include "temper.h"
#include "ELTEC_EmulatedEEPROM.h"
//#include "stm32l0xx_hal_flash_ex.h"

uint16_t TwoByteInArrayToWord (uint8_t  *PointArray);
_Bool cambio_rutina;
extern uint16_t temperatureRTP;


// Borra una Array de 8 Elementos
void clear_8_ElementArray( uint8_t *array8size){
	for(uint8_t i= 0; i<8 ; i++ )
		array8size[i] =0;
}


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
		//uint8_t *X = &eePlantilla[eedato_seg1];					// manuel_ apuntador para la eeprom
		uint8_t *X = &eePlantilla[eedato_seg1];
		uint8_t *Y = &copiaPlantilla[cdato_seg1];								// manuel_ apuntador ram para la compia de parametros
		uint8_t *Z = &reePlantilla[eedato_seg1];
		// manuel_ copia los parametros de eeprom a una ram copia para modificarlos
copy_eeprom: //********************************?
		//call rdeeprom
		//ld A,waux
		*Y = (uint8_t) findLastValue((uint32_t)X);	//ld (Y), A
		*Z = *Y;
		//*Y = *X;
		X++;//incw X
		Y++;//incw Y
		Z++;
		if(Y <= &copiaPlantilla[cdato_seg3]){//cpw Y,#cdato_seg3
			goto copy_eeprom;//jrule copy_eeprom
		}

no_set_prog:

		if(flagsb[f_prog]){//btjt flagsb,#f_prog,ask_btn_prsd
			goto ask_btn_prsd;
		}
		goto fin_prog_param; //jp fin_prog_param
ask_btn_prsd:
	_Bool bool_btn_pr = 0;
	for(uint8_t k=0; k<8; k++)
		bool_btn_pr |= btn_pr[k];

	if(!bool_btn_pr)//tnz btn_pr				;// manuel_ no es un contador es copia de los botones, pregunta practicmante si alguna tecla se presiono
		goto no_btn_pres; //jreq no_btn_pres

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
			op_menu  (reePlantilla[eeversion1], reePlantilla[eeversion2]/10);
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
		if(copiaPlantilla [cescala] == reePlantilla[eeescala]){
			goto noCambiaEscala;//	jreq noCambiaEscala
		}
		waux = copiaPlantilla [cescala];//	mov			waux,cescala;
		//	ldw			X,#eeescala;
		wreeprom(waux, &eePlantilla[eeescala]);//	call		wreeprom;
		reePlantilla[eeescala] = waux;
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
Load_ret(){				//Load_ret:
//;							mov			retnoct,#$14	;	/ Carga el retardo para entrar a nocturno con 20 minutos
    asm ("nop");
}
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
			/*
			 * Prueba de Debuggger TEMPERATURA CON RTP
			 */
			//foo = temperatureRTP;

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

// rutina refrigera Adaptada CTOF Completa ..............

void fallas(void){						//fallas:

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

//		;mov			wreg,#$05;
//		;mov			waux,#$0C
//		;mov			waux,#$26;					"-"
//		;mov			waux,#$1F;					" "
	    waux = 0x13;			//mov waux,#$13;
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
//		;mov			waux,#$08;		/ Despliega el código de falla "88"
//		;mov			wreg,#$05;					"S"
//		;mov			waux,#$0A;					"A"
//		;mov			waux,#$1F;					" "
		waux = 0x13;			//mov waux,#$13;

fallas15:
	    tempo1 = waux;			//mov tempo1, waux
		Load_ret();				//call Load_ret;		No hace nada en ensamblador
	    load_tiempoAhorro1();	//call load_tiempoAhorro1;
	    load_tiempoAhorro2();	//call		load_tiempoAhorro2;
	    waux = tempo1;			//mov waux,tempo1;

fallas_j02:
		datled_clear();
		//BitClear(datled,0);			//bres datled,#0
		//BitClear(datled,1);			//bres datled,#1;
		//wreg = 0x13;//mov wreg,#$13;
		wreg = 0x01;//mov wreg,#$01;
		op_menu (waux,wreg);
		//datdig1 = waux;//mov datdig1,waux;
		//datdig2 = wreg;//mov datdig2,wreg;
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_h1)){		//btjf		flagsBuzz,#bz_h1,fallas16
			goto	fallas16;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0


fallas16:

		goto finfallas;//jp finfallas

//		;							tnz     trefst2
//		;							jrne		fallas20
//		;
//		;							ld			A,trefst;
//		;							and			A,#$FC;
//		;							;jreq		finfallas;		/ Si no hay otra falla termina.
//		;							jreq		finfallas_01;		/ Si no hay otra falla termina.

fallas20:

		if(trefst2[f_s3short]){			//btjt		trefst2,#f_s3short,fallas_j01b;	/ El sensor del retorno esta en corto?
			goto	fallas_j01b;
		}
		if(!trefst2[f_s3open]){			//btjf		trefst2,#f_s3open,fallas_j02b;	/ El sensor del retorno esta abierto o desconectado?
			goto	fallas_j02b;
		}

fallas_j01b:
//		;mov			wreg,#$05;					"S"
//		;mov			waux,#$03;					"3"
//		;mov			wreg,#$1F;					" "
//		;mov			waux,#$13;					"H"
		wreg = 0x13;	//mov			wreg,#$13;					"H"
		waux = 0x03;	//mov			waux,#$03;					"3"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_h3)){		//btjf		flagsBuzz,#bz_h3,fallas_j02b
			goto	fallas_j02b;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0

fallas_j02b:

		if(trefst[f_sdc]){			//btjt		trefst,#f_sdc,fallas_j03;	/ El sensor del evaporador esta en corto?
			goto	fallas_j03;
		}
		if(!trefst[f_sda]){			//btjf		trefst,#f_sda,fallas_j04;	/ El sensor del evaporador esta abierto o desconectado?
			goto	fallas_j04;
		}
fallas_j03:
//		;mov			waux,#$00;		/ Si, indica la falla '00'
//		;mov			wreg,#$05
//		;mov			waux,#$0E
//		;mov			wreg,#$0D;					"d"
//		;mov			waux,#$0F;					"F"
//		;mov			wreg,#$1F;					" "
//		;mov			waux,#$13;					"H"
		wreg = 0x13;	//mov			wreg,#$13;					"H"
		waux = 0x02;	//mov			waux,#$02;					"2"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_h2)){		//btjf		flagsBuzz,#bz_h2,fallas_j04
			goto	fallas_j04;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j04:
//		;btjf		trefst,#f_ta,fallas_j05;	/ Hay falla detemperatura alta?
//		;;mov			waux,#$07;		/ Si, indica la falla '77'
//		;mov			wreg,#$12
//		;mov			waux,#$0E


		if(!trefst2[f_ambHi]){			//btjf		trefst2,#f_ambHi,fallas_j04b
			goto	fallas_j04b;
		}
		//;mov			wreg,#$20;					"t"
		//;mov			waux,#$0A;					"A"
		wreg = 0x1F;	//mov			wreg,#$1F;					" "
		waux = 0x20;	//mov			waux,#$20;					"t"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_t)){	//btjf		flagsBuzz,#bz_t,fallas_j04b
			goto	fallas_j04b;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0

fallas_j04b:
		if(!trefst2[f_ambLo]){			//btjf		trefst2,#f_ambLo,fallas_j04c
			goto	fallas_j04c;
		}
		//;mov			wreg,#$20;					"t"
		//;mov			waux,#$0B;					"b"
		wreg = 0x1F;	//mov			wreg,#$1F;					" "
		waux = 0x0F;	//mov			waux,#$0F;					"F"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_f)){	//btjf		flagsBuzz,#bz_f,fallas_j04c
			goto	fallas_j04c;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0


fallas_j04c:

fallas_j05:
		if(!trefst[f_pa]){				//btjf		trefst,#f_pa,fallas_j06;	/ Se ha dejado la puerta abierta por mas tiempo del permitido?
			goto	fallas_j06;
		}
		//;mov			wreg,#$1E;					"P"
		//;mov			waux,#$0A;					"A"
		wreg = 0x1F;	//mov			wreg,#$1F;	" "
		waux = 0x1E;	//mov			waux,#$1E;	"P"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_p)){	//btjf		flagsBuzz,#bz_p,fallas_j06
			goto	fallas_j06;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j06:
		if(!trefst2[f_defi]){				//btjf		trefst2,#f_defi,fallas_j06a
			goto	fallas_j06a;
		}
		wreg = 0x1F;	//mov			wreg,#$1F;					" "
		waux = 0x0A;	//mov			waux,#$0A;					"A"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_a)){	//btjf		flagsBuzz,#bz_a,fallas_j06a
			goto	fallas_j06a;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j06a:
		if(!trefst2[f_co_ex]){				//btjf		trefst2,#f_co_ex,fallas_j06b
			goto	fallas_j06b;
		}
		//;mov			wreg,#$0C;					"C"
		//;mov			waux,#$10;					"o"
		wreg = 0x1F;	//mov			wreg,#$1F;					" "
		waux = 0x0C;	//mov			waux,#$0C;					"C"
		if(!GetRegFlagState(Plantilla[flagsBuzz],bz_c)){	//btjf		flagsBuzz,#bz_c,fallas_j06b
			goto	fallas_j06b;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j06b:
		if(!trefst[f_iny]){				//btjf		trefst,#f_iny,fallas_j06c
			goto	fallas_j06c;
		}
		wreg = 0x01;	//mov			wreg,#$01;					"I"
		waux = 0x1D;	//mov			waux,#$1D;					"n"
		if(!GetRegFlagState(Plantilla[flagsBuzz2],bz_in)){	//btjf		flagsBuzz2,#bz_in,fallas_j06c
			goto	fallas_j06c;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j06c:
		if(!trefst2[f_retCo]){				//btjf		trefst2,#f_retCo,fallas_j06d
			goto	fallas_j06d;
		}
		wreg = 0x0C;	//mov			wreg,#$0C;					"C"
		waux = 0x00;	//mov			waux,#$00;					"0"
		if(!GetRegFlagState(Plantilla[flagsBuzz2],bz_co)){	//btjf		flagsBuzz2,#bz_co,fallas_j06d
			goto	fallas_j06d;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j06d:
		if(!trefst[f_hv]){				//btjf		trefst,#f_hv,fallas_j07;	/ El voltaje de línea es alto?
			goto	fallas_j07;
		}
		//;mov			waux,#$01;		/ Si, indica la falla '11'
		//;mov			wreg,#$13;					"H"
		//;mov			waux,#$14;					"i"
		wreg = 0x1F;	//mov			wreg,#$1F;					" "
		waux = 0x27;	//mov			waux,#$27;					"U"
		if(!GetRegFlagState(Plantilla[flagsBuzz2],bz_u)){	//btjf		flagsBuzz2,#bz_u,fallas_j07
			goto	fallas_j07;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j07:
		if(!trefst[f_lv]){		//btjf		trefst,#f_lv,fallas_j08;	/ El voltaje de línea es bajo?
			goto	fallas_j08;
		}
		//;mov			waux,#$02;		/ Si, indica la falla '22'
		//;mov			wreg,#$11;					"L"
		//;mov			waux,#$10;					"o"
		wreg = 0x26;	//mov			wreg,#$26;					"-"
		waux = 0x27;	//mov			waux,#$27;					"U"
		if(!GetRegFlagState(Plantilla[flagsBuzz2],bz_mu)){	//btjf		flagsBuzz2,#bz_mu,fallas_j08
			goto	fallas_j08;
		}
		flagsBuzzer[0] = 1;				//bset		flagsBuzzer,#0
fallas_j08:
		if(!trefst2[f_tvolt]){		//btjf		trefst2,#f_tvolt,fallas_j08b;	/ La tarjeta de voltaje esta fallando?
			goto	fallas_j08b;
		}
		wreg = 0x0E;	//mov			wreg,#$0E;					"E"
		waux = 0x2F;	//mov			waux,#$2F;					"u"
		flagsBuzzer[0] = 0;				//bres		flagsBuzzer,#0
fallas_j08b:



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
void ram_reserved(void){

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

			//HAL_IWDG_Refresh( &hiwdg );				//MOV				IWDG_KR,#$AA
			// Para realizar una sobreescritura en la Flash, es necesario borrar la pagina antes
			// CGM 04/12/2024


			/*
			 * Optimización de código, para que se necesite menos memoria
			 * CGM 21/04/2024
			 */
			//uint32_t VarAux_= (((uint32_t) dirPointer) - 0x8000000);
			//if(VarAux_ % 2048 == 0){
			if(( ((uint32_t) dirPointer) & 0x7FF) == 0){
				// Estamos a inicio de Pagina, es necesario un borrado, debido a que se necesita escribir
				uint32_t numberPage = getNumberPage((uint32_t) dirPointer); 		// Number the Page
				erasePage(numberPage);
			}
			/*
			 * Optimización de código
			 * CGM 21/04/2025
			 */


//			AddressDestination = (uint64_t *) dirPointer;
//			//while( HAL_FLASH_Unlock() !=  HAL_OK );
//			// Graba 128 Bytes de FLASH
//			for(uint8_t i = 0; i < 16 ; i++ ){
//
//				Data  	 =  (uint64_t)(*dataPointer);						dataPointer++;
//				Data  	+= ((uint64_t)(*dataPointer))	<<8;				dataPointer++;
//				Data  	+= ((uint64_t)(*dataPointer)) 	<<16;				dataPointer++;
//				Data	+= ((uint64_t)(*dataPointer))  	<<24;			   	dataPointer++;
//				Data	+= ((uint64_t)(*dataPointer))  	<<32;			   	dataPointer++;
//				Data	+= ((uint64_t)(*dataPointer))  	<<40;			   	dataPointer++;
//				Data	+= ((uint64_t)(*dataPointer))  	<<48;			   	dataPointer++;
//				Data	+= ((uint64_t)(*dataPointer))  	<<56;			   	dataPointer++;
//
//				while( HAL_FLASH_Unlock() !=  HAL_OK );
//				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) AddressDestination, Data);
//				while( HAL_FLASH_Lock() !=  HAL_OK);
//				AddressDestination++;
//				//if(AddressDestination>=0x801d000)
//				//	break;
////				while( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressDestination, Data) !=  HAL_OK );		// HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
//			   	//dirPointer +=4;
//				//HAL_IWDG_Refresh( &hiwdg );				// evita time out watch
//
//				//asm ("nop");
//
//			}

			// Escritura de la FLASH
			// CGM 21/04/2025
			writeFLASH( (uint64_t *) dirPointer, (uint64_t *) dataPointer,16);

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
			//asm ("nop");
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
			//HAL_IWDG_Refresh( &hiwdg );				//MOV				IWDG_KR,#$AA

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
			//HAL_IWDG_Refresh( &hiwdg );
			for(uint8_t i = 0; i < 128 ; i++){
				FlashManager((uint32_t)dirPointer, (uint32_t)*dataPointer);
				reePlantilla[i] = *dataPointer;		// Guardando el respaldo en RAM
				dataPointer++;
				dirPointer++;
				//HAL_IWDG_Refresh( &hiwdg );
			}



WAIT_FOR_GRAB_0:

WAIT_FOR_GRAB:
			//HAL_IWDG_Refresh( &hiwdg );		//MOV				IWDG_KR,#$AA

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
	foo= reePlantilla[eetiempoAhorro1]*360;
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
void load_tiempoAhorro2(void){
		uint16_t	foo = 0;
		foo= reePlantilla[eetiempoAhorro2]*360;
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
	cntNoct_H = reePlantilla[eetimeNoct]*60;
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

//		uint32_t *point_X;
//		point_X = (uint32_t *) (cntRegPNT);

		//waux = highByte(cntReg);			//		mov		waux,resulh;
		//		ldw		X,cntRegPNT;
		//wreeprom (waux, point_X);		//wreeprom (waux, cntRegPNT);			//		call	wreeprom;						/ ejecuta el grabado



		//HAL_IWDG_Refresh( &hiwdg );			//  	MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria
		//
		//waux = lowByte(cntReg);		// 		mov		waux,resull;
		//point_X++;  // cntRegPNT++;						//		incw	X
		//wreeprom (waux, point_X);		//wreeprom (waux, cntRegPNT);			//		call	wreeprom;						/ ejecuta el grabado

		FlashManager(cntRegPNT, cntReg);


		//HAL_IWDG_Refresh( &hiwdg );			//		MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria
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
	reeTimeUnix1 = waux;
	//HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

	waux = lowByte(timeSeconds_HW);		// mov		waux,resull;
	//	ldw		X,#eeTimeUnix2;
	wreeprom (waux, &eeTimeUnix2);		//  call	wreeprom;				/ ejecuta el grabado
	reeTimeUnix2 = waux;
	//HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria


	//	ldw		X,timeSeconds_LW
	//	ldw		resulh,X

	waux = highByte(timeSeconds_LW);	//	mov		waux,resulh;
	//	ldw		X,#eeTimeUnix3;
	wreeprom (waux, &eeTimeUnix3);		// call	wreeprom;					/ ejecuta el grabado
	reeTimeUnix3 = waux;
	//HAL_IWDG_Refresh( &hiwdg );			// MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

	waux = lowByte(timeSeconds_LW);		// mov		waux,resull;
	// ldw		X,#eeTimeUnix4;
	wreeprom (waux, &eeTimeUnix4);   	// call	wreeprom;					/ ejecuta el grabado
	reeTimeUnix4 = waux;
	//HAL_IWDG_Refresh( &hiwdg );			//  MOV		IWDG_KR,#$AA;			/ Refresca el watch Dog mientras se efectua la grabacion de la memoria

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
		data_buffer[i] = 0xFF;
		event_buffer[i] = 0xFF;
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

//;=====================================================================
//;	Subrutina de control función vaho
//;=====================================================================
void vaho_func(){

				if(Plantilla[tOnVh] == 0){
					goto	vaho_end;		//jreq		vaho_end
				}
				if(Plantilla[tOffVh] == 0){
					goto	vaho_end;		//jreq		vaho_end
				}

				if(flagsVaho[0]){
					goto	vaho_ON;						//btjt		flagsVaho,#0,vaho_ON
				}


vaho_OFF:
				GPIOR0[f_dh] = 0;							//bres		GPIOR0,#f_dh
				flagsVaho[0] = 0;							//bres		flagsVaho,#0
				timeOnVaho_w = Plantilla[tOnVh] * 60 ;		//mov		wreg,tOnVh;
				if(timeOffVaho_w == 0){
					goto	vaho_ON;						//jreq		vaho_ON
				}
				goto	vaho_end;							//jra		vaho_end

vaho_ON:

				GPIOR0[f_dh] = 1;							//bset		GPIOR0,#f_dh
				flagsVaho[0] = 1;							//bset		flagsVaho,#0
				timeOffVaho_w = Plantilla[tOffVh] * 60 ;	//mov		wreg,tOffVh;
				if(timeOnVaho_w == 0){
					goto	vaho_OFF;						//jreq		vaho_OFF
				}
				goto	vaho_end;							//jra		vaho_end
vaho_end:

}

//;===============================================================================
//;SUBTURINAS ASOCIADAS A LA MEDICIÓN DE VOLTAJE Y CORRIENTE TRMS
//;===============================================================================

//;=========================================  Rutina DETECCIÓN DE FLANCO DE BAJADA
//;Por las características del hardware no se considera cruce por cero como tal.
//;La señal se obtiene de un circuito aislado que tiene DEFASE.

// Adaptacion revisada 30-MAY-2025 ........
void detecta_flanco(){

			s_reloj[2] = 0;//bres    s_reloj,#2        ;[b_flancob] ;Desactiva bandera de flanco de bajada

up_anterior:
			if(s_reloj[0]){	//btjt s_reloj,#0,alto   [actual]	   ;¿El valor de la bandera "actual" es 1?
				goto alto;
			}
			s_reloj[1] = 0;   				//bres s_reloj,#1    ;[anterior]  ;NO: Anterior <- 0
			goto end_up_anterior;//jra end_up_anterior

alto:
			s_reloj[1] = 1; 	//bset s_reloj,#1		;[anterior]   ;SI: Anterior <- 1
end_up_anterior:

up_actual:	 //;Actualiza la bandera "actual", con el valor actual del pin de flanco
			if	(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)){
				goto pin_cruce_alto;//jruge   pin_cruce_alto
			}
			s_reloj[0] = 0;		// bres    s_reloj,#0       ;[actual] ;NO: actual <- 0
			goto end_up_actual;//;Determina si existió cruce
pin_cruce_alto:
			s_reloj[0] = 1;	 		//bset    s_reloj,#0         ;[actual] ;SI: actual <- 1
end_up_actual:


determina_flanco: //---------------------------------------------------------?
			//ld      A,s_reloj     ;Enmascara variable para determinar flanco
			//and     A,#$03        ;Las banderas son 0 ANT y 1 ACT enmascara con '00000011'
			//cp      A,#$02        ;Estado buscado flanco de bajada ANT = 1 y ACT = 0,'00000010'
	        if((s_reloj[1]) & (!s_reloj[0]) ){// jreq    flanco_bajada ;Coloca bandera de flanco de bajada
				goto flanco_bajada;			//jreq    flanco_bajada ;Coloca bandera de flanco de bajada
			}
			goto fin_detecta_flanco;//jra fin_detecta_flanco


flanco_bajada:
			s_reloj[2] = 1;				//bset    s_reloj,#2     ;[b_flancos]    ;Activar bandera de flanco de subida
			cruze_por_cero[0] = 1;		//bset    cruze_por_cero,#0     ;Activa bandera de cruce por cero

fin_detecta_flanco:

}

//;=====================================  FIN Rutina DETECCIÓN DE FLANCO DE BAJADA
//
//;=============================================  Rutina de calibración de voltaje
//;CONSIDERACIONES PARA CALIBRACIÓN:
//;1. Debieron pasar 3 segundos con el equipo alimentado y muestreando
//;2. En el instante que se va a realizar la calibración se deben tener 3
//;muestras promediadas (8 muestras) IGUALES
//;3. Las muestras deben estar en un rango definido

// Adaptacion revisada 30-MAY-2025 ........
void calibra_voltaje(){

			muestras_cal_volt++; 			//inc  	muestras_cal_volt
			if(muestras_cal_volt >= 23){	//	cp      A,#23                     ;23 muestras (23*8*16.6ms = 3 segundos)
				goto espera_iguales;//jruge   espera_iguales
			}
			goto fin_calibra_voltaje;// jp fin_calibra_voltaje

espera_iguales:
			if(muestras_cal_volt != 23) {	//cp      A,#23
				goto segunda_muestra;		//jrne    segunda_muestra
			}
			voltaje_ant_cal = volt_trms;	//	mov voltaje_ant_cal,volt_trms
			goto fin_calibra_voltaje;		//jp  fin_calibra_voltaje

segunda_muestra:
			//goto calibracion_valida;
			if(muestras_cal_volt != 24){	//cp      A,#24
				goto tercera_muestra;		//jrne    tercera_muestra
			}
			tolerancia_cal_volt();			//call    tolerancia_cal_volt
			//STM8_A viene de  "tolerancia_cal_volt()"
			if(STM8_A != 0x55){					//cp      A,#24
				goto calibra_reset;			//jrne    tercera_muestra
			}
			goto	fin_calibra_voltaje;	//jp      fin_calibra_voltaje


tercera_muestra:
			tolerancia_cal_volt();			//call    tolerancia_cal_volt
			//STM8_A viene de  "tolerancia_cal_volt()"
			if(STM8_A == 0x55){					//cp      A,#55
					goto calibracion_valida;			//jreq    calibracion_valida
			}
			goto	calibra_reset;	//jp      calibra_reset




calibracion_valida:

//				clrw    X
//				ld      A,volt_trms
//				ld      XL,A

v_ubajo_cv:
				//ldw Y,#60
				//ldw waux,Y
				//waux = 60;
				if(volt_trms > 60 ){//cpw X,waux -------x =   ;Compara la medición sin factor con el umbral bajo
					goto v_ualto_cv;//jrugt  v_ualto_cv
				}
				goto calibra_reset;//jp   calibra_reset

v_ualto_cv:
				//ldw     Y,#80
				//ldw     waux,Y
				//waux = 80;
				if(volt_trms >= 80 ){//cpw     X,waux, x  ;Mayor o igual al umbral alto, resistores mal soldados o inadecuados
					goto calibra_reset;//jruge   calibra_reset
				}
				wreg = 100;					//mov  wreg,#100
				waux = volt_trms;			//mov  waux,volt_trms

graba_calvolt:

desbloquea_eeprom:
				uint32_t AddressDestination;
				uint32_t Data ;
	//			HAL_FLASHEx_DATAEEPROM_Unlock();

		                    //btjt FLASH_IAPSR,#3,eeprom_desbloqueada
eeprom_bloqueada:		//----------------------------------registro interno
		                      //mov FLASH_DUKR,#$AE
		                      //mov  FLASH_DUKR,#$56
eeprom_bloqueada_1:
		                     //btjf FLASH_IAPSR,#3,eeprom_bloqueada_1  ------------?
eeprom_desbloqueada:
				//ld  A, wreg
				//ldw  X, #eevolt_mul
				// (eevolt_mul) = wreg; //ld (X), A
				//Data = (uint32_t)wreg;											//;Guarda la variable de multiplicación en EEPROM
				//AddressDestination = &eevolt_mul;
				wreeprom(wreg,&eevolt_mul);
				reevolt_mul = wreg;

	//			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
				//HAL_IWDG_Refresh( &hiwdg );
graba_1:
		         //btjt FLASH_IAPSR,#2,graba_1 ------------------------registro

				//ld  A,waux
				 //ldw  X,#eevolt_div
				 //ld	(X),A
				//Data = (uint32_t)waux;										//;Guarda la variable de división en EEPROM
				//AddressDestination = &eevolt_div;
				wreeprom(waux,&eevolt_div);
				reevolt_div = waux;
	//			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
				//HAL_IWDG_Refresh( &hiwdg );
graba_2:
		         //btjt	FLASH_IAPSR,#2,graba_2  ----------------registro

				// ld  A,#$3C
				//ldw X,#eef_voltaje
				//ld (X),A
				//Data = (uint32_t)0x3C;										//;Indica que ya se realizo calibración de voltaje en EEPROM
				//AddressDestination = &eef_voltaje;
	//			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
				wreeprom(0x3C,&eef_voltaje);
				reef_voltaje = 0x3C;

				//HAL_IWDG_Refresh( &hiwdg );

graba_3: //----------------------------------registro
		        //btjt FLASH_IAPSR,#2,graba_3
		        // mov FLASH_IAPSR,#$00	   ;Bloquea la EEPROM inmediatamente
	//			HAL_FLASHEx_DATAEEPROM_Lock();
		        goto fin_calibra_voltaje;//jp fin_calibra_voltaje

calibra_reset:
				asm ("nop");
				asm ("nop");
		        goto calibra_reset;//jp      calibra_reset

fin_calibra_voltaje:
//		        goto fin_voltrms;// jp fin_voltrms
//error_muestreo:
//				cnt_mues = 0;//clr cnt_mues
//				cnt_veces_muestreo = 0;//clr cnt_veces_muestreo
//				edo_muestreo = 0;//mov edo_muestreo,#0
//fin_voltrms:



}

//;=================================================  FIN Rutina para RESET de MCU

//;==============================================================  Rutina muestreo
//;Adquiere y procesa 64 muestras de voltaje y corriente.

// Adaptacion revisada 30-MAY-2025 ........
void muestreo_trms(){

		if(!ban_muestreo[0]){						//btjf    ban_muestreo,#0,adq_muestra_i ;¿Nuevo ciclo a medir?
			goto adq_muestra_i;
		}
		ban_muestreo[0] = 0;		//bres    ban_muestreo,#0           ;Limpia bandera que indica que es inicio de un nuevo ciclo
		//clr     sigma_cuad_sampl_1_1      ;Limpia las variables de la sumatoria CH1
		//clr     sigma_cuad_sampl_2_1      ;/
		//clr     sigma_cuad_sampl_3_1      ;/
		//clr     sigma_cuad_sampl_4_1      ;/
		sigma_cuad_sampl_1 = 0;
		//clr     sigma_cuad_sampl_1_2      ;Limpia las variables de la sumatoria CH2
		//clr     sigma_cuad_sampl_2_2      ;/
		//clr     sigma_cuad_sampl_3_2      ;/
		//clr     sigma_cuad_sampl_4_2      ;/
	    sigma_cuad_sampl_2 = 0;
		cnt_mues = 0;				//clr     cnt_mues                  ;Limpia el contador de muestras


adq_muestra_i:            //;------------------------------  Adquiere la muestra n
		if(cnt_mues <= 64){						//jrule   adq_muestra_i_1
			goto adq_muestra_i_1;
		}
		goto	fin_muestreo_trms;		//fin_muestreo_trms			//jp      fin_muestreo_trms

adq_muestra_i_1:
		//Captura ADC CANAL de CORRIENTE
		ADC1->CHSELR = 0;
		ADC1->CHSELR |= ADC_CHSELR_CHSEL0;  // "STM32U:PC0_Canal 0_FOTOCELDA"  ----->  "STM8:PE5 ADC_IN 23"
		capturaAD();

		ram_h = adcramh;			// ;Guarda el resultado de la conversión en RAM TEMPORAL

//		;acond_corriente:
//		;  ldw     X,ram_h
//		;  cpw     X,#512
//		;  jruge   positivo
//		;    ldw     X,#512
//		;    subw    X,ram_h
//		;    ldw     ram_h,X
//		;    jra     fin_acond_corriente
//
//		;:positivo:
//		;  subw    X,#512
//		;  ldw     ram_h,X

fin_acond_corriente:

		STM8_16_X = ram_h;  		  //ldw     X,ram_h        ;Almacena la muestra en X para calculos en RAM
		  	  	  	  	  	  	  	  //ldw     Y,ram_h        ;Almacena la muestra en Y para colocarlo en RAM

		fact_mul = 1;  				//mov     fact_mul,#1     ;eecorr_mul    ;Aplica factor, el resultado llega en X
		fact_div = 1;  				//mov     fact_div,#1     ;eecorr_div    ;/
		aplica_factor();			//call    aplica_factor          ;Aplica el factor a la muestra adquirida

		//  mov     sigma_cuad_sampl_4,sigma_cuad_sampl_4_1   ;CANAL 1 es la corriente
		//  mov     sigma_cuad_sampl_3,sigma_cuad_sampl_3_1   ;/
		//  mov     sigma_cuad_sampl_2,sigma_cuad_sampl_2_1   ;/
		//  mov     sigma_cuad_sampl_1,sigma_cuad_sampl_1_1   ;/
		sigma_cuad_sampl = sigma_cuad_sampl_1;
		cuadrado_suma();			//call    cuadrado_suma  ;Eleva la muestra al cuadrado y realiza suma acumulada
//		  mov     sigma_cuad_sampl_4_1,sigma_cuad_sampl_4   ;CANAL 1 es la corriente
//		  mov     sigma_cuad_sampl_3_1,sigma_cuad_sampl_3   ;/
//		  mov     sigma_cuad_sampl_2_1,sigma_cuad_sampl_2   ;/
//		  mov     sigma_cuad_sampl_1_1,sigma_cuad_sampl_1   ;/
		sigma_cuad_sampl_1 = sigma_cuad_sampl;

adq_muestra_v:            //;------------------------------  Adquiere la muestra n
		//Captura ADC CANAL de Voltaje
		ADC1->CHSELR = 0;
		ADC1->CHSELR |= ADC_CHSELR_CHSEL9;  // "STM32U:PC0_Canal 0_FOTOCELDA"  ----->  "STM8:PE5 ADC_IN 23"
		capturaAD();

		ram_h = adcramh;		//;Guarda el resultado de la conversión en RAM TEMPORAL

		ram_h >>= 1;    			//srlw    X              ;Convierte la muestra de 10 bits en 9 bits
		ram_h >>= 1; 				//srlw    X              ;Convierte la muestra de  9 bits en 8 bits

		STM8_16_X = ram_h;

		fact_mul = reevolt_mul;  	//mov     fact_mul,eevolt_mul    ;Aplica factor, el resultado llega en X
		fact_div = reevolt_div;  	//mov     fact_div,eevolt_div    ;/
		aplica_factor();			//call    aplica_factor          ;Aplica el factor a la muestra adquirida

//		  mov     sigma_cuad_sampl_4,sigma_cuad_sampl_4_2   ;CANAL 2 es el voltaje
//		  mov     sigma_cuad_sampl_3,sigma_cuad_sampl_3_2   ;/
//		  mov     sigma_cuad_sampl_2,sigma_cuad_sampl_2_2   ;/
//		  mov     sigma_cuad_sampl_1,sigma_cuad_sampl_1_2   ;/
		sigma_cuad_sampl =  sigma_cuad_sampl_2;
		cuadrado_suma();			//call    cuadrado_suma  ;Eleva la muestra al cuadrado y realiza suma acumulada
//		  mov     sigma_cuad_sampl_4_2,sigma_cuad_sampl_4   ;CANAL 1 es la corriente
//		  mov     sigma_cuad_sampl_3_2,sigma_cuad_sampl_3   ;/
//		  mov     sigma_cuad_sampl_2_2,sigma_cuad_sampl_2   ;/
//		  mov     sigma_cuad_sampl_1_2,sigma_cuad_sampl_1   ;/
		 sigma_cuad_sampl_2 = sigma_cuad_sampl;

		 cnt_mues++;			//inc     cnt_mues       ;Una muestra más

fin_muestreo_trms:

}


//;=========================================================  Rutina aplica factor
//;Llega un dato de 2 BYTES en X, el dato es multiplicado por un factor que la
//;apróxima al valor calculado (revisar tablas de diseño).

// Adaptacion revisada 30-MAY-2025 ........
void aplica_factor(){
											//;*fact_mul límitado a 64 para resultado 2 BYTES
	uint32_t foo = STM8_16_X * fact_mul;		//call    mul_16x16
											//;Resultado de 2 BYTES en level_1st_result_H
	STM8_16_X = (uint16_t)(foo / fact_div);  		//;Divide entre factor

}

//;=====================================================  FIN Rutina aplica factor

//;============================================== Rutina cuadrado y suma acumulada
//;La muestra con el factor aplicado se eleva al cuadrado y se guarda en las
//;variables para la suma acumulada de las muestras elevadas al cuadrado, el
//;resultado es de 4 BYTES.
//;Antes de llamar la rutina y al regreso se deben indicar las variables del CANAL

// Adaptacion revisada 30-MAY-2025 ........
void cuadrado_suma(){
		//	  ldw     waux,X            ;waux.. wreg     mult 1 X Muestra con factor 2 BYTES
		//	  ldw     resulh,X          ;resulh:resull.. mult 2 X Muestra con factor 2 BYTES
		//	  call    mul_16x16         ;El resultado es un dato de 4B (En la aplicación 3B)
//;waux:wreg      mult 1
//;resulh:resull  mult 2
//;El resultado lo almacena en [level_2st_result_H:level_1st_result_L]
		uint32_t	foo = STM8_16_X * STM8_16_X;
//		;---------------------------------------------------------------  Suma acumulada
//		;Suma 2 variables de 4 BYTES despreciando el acarreo del MSB
//		;Considerando la limitante (511) del algoritmo la suma acumulada de las muestras
//		;elevadas al cuadrado será máximo: 16,711,744d - FF0040h 3B
//		;Por lo anterior, despreciar el acarreo no tiene efecto, pues nunca existirá

suma_acumulada:
//ldw     X,level_1st_result_H   ;Suma acumulada PARTE BAJA
//addw    X,sigma_cuad_sampl_2   ;/
//ldw     sigma_cuad_sampl_2,X   ;sigma_cuad_sampl_X auxiliar suma acumulada
//clr     waux                   ;Para sumar si existe acarreo
//clr     wreg                   ;/
//jrnc    not_inc_byte3_2        ;/
//  inc  		wreg                 ;/
//not_inc_byte3_2:
//ldw     X,waux                 ;/
//addw    X,level_2st_result_H   ;Suma acumulada PARTE ALTA
//addw    X,sigma_cuad_sampl_4   ;/
//ldw     sigma_cuad_sampl_4,X   ;sigma_cuad_sampl_X auxiliar suma acumulada
		sigma_cuad_sampl = sigma_cuad_sampl + foo;

}

//;===========  Rutina promedio de suma acumulada de muestras elevadas al cuadrado
//;Rutina que promedia la sumatoria de las muestras elevadas al cuadrado esta en
//;4 bytes c_sigma_cuad_sampl[4:1], promedio se guardada en las mismas variables.
//
//;Para realizar el promedio de 64 muestras se realizan 6 corrimientos 2^6 = 64
//;Para realizar el promedio de 32 muestras se realizan 5 corrimientos 2^5 = 32

// Adaptacion revisada 30-MAY-2025 ........
void prom_muestras(){

//      ld      A,#5                 ;5 corrimientos para promediar 32
//      ;ld      A,#6                 ;6 corrimientos para promediar 64
//
//taking_prom:
//  	  srl     c_sigma_cuad_sampl_4   ;Divide un dato de 4 BYTES entre 2
//  	  rrc     c_sigma_cuad_sampl_3   ;/
//  	  rrc     c_sigma_cuad_sampl_2   ;/
//  	  rrc     c_sigma_cuad_sampl_1   ;/
//  	  dec     A                      ;¿"A" corrimientos realizados?
//  	  tnz     A                      ;/
//  	  jrne    taking_prom            ;/

	c_sigma_cuad_sampl = c_sigma_cuad_sampl / 32;

}

//;========================================  Rutina para calcular la raíz cuadrada
//;Cálcula la raíz cuadrada de un número de 2 BYTES con el MÉTODO BABILÓNICO
//;Valor a calcular c_sigma_cuad_sampl_x (promedio de la sumatoria de las muestras
//;elevadas al cuadrado), regresa en X el valor de la raíz.
//;TRUNCAMIENTO: 255 al cuadrado es 65025d FE01h, FFFFh equivale a 255, si el dato
//;promediado es de 3 BYTES (pero no 4, es imposible*), trunca calculo de la raíz:
//;Dato a calcular / 4 -> calcula raíz y el resultado es multiplicado por 2
//;Antes de llamar la rutina se debe indicar a que CANAL se le calculará la RAÍZ
//;*Revisar archivo de diseño medidor TRMS

// Adaptacion revisada 30-MAY-2025 ........
void calcula_raiz(){

	STM8_16_X = sqrt(c_sigma_cuad_sampl);


}

//;===============  FIN Rutina promedio de suma acumulada de 8 muestras de 2 BYTES
//
//;=====================================  Rutina tolerancia calibración de voltaje
//;Debido al glitter para la señal de cruce por cero y que no se tiene rectificación
//;de onda completa se deja una toelrancia de +-1V en las 3 señales que son
//;comparadas al realizar la calibración.
//;Se usan las variables volt_trms y voltaje_ant_cal, si estan en tolerancia
//;en A se carga el valor 55 decimal

// Adaptacion revisada 30-MAY-2025 ........
void	tolerancia_cal_volt(){

		STM8_A = volt_trms;					//ld      A,volt_trms
		if(STM8_A == voltaje_ant_cal){
			goto med_en_tolerancia;			//	cp      A,voltaje_ant_cal
		}									//	jreq    med_en_tolerancia
			STM8_A--;							//dec     A
			if(STM8_A == voltaje_ant_cal){		//
				goto med_en_tolerancia;			//	cp      A,voltaje_ant_cal
			}									//jreq    med_en_tolerancia
				STM8_A++;							//inc A;
				STM8_A++;							//inc A;
				if(STM8_A == voltaje_ant_cal){		//
					goto med_en_tolerancia;			//	cp      A,voltaje_ant_cal
				}
				STM8_A = 0;							//clr     A
				goto	fin_tolerancia_cal_volt;	//jra     fin_tolerancia_cal_volt


med_en_tolerancia:
		STM8_A = 0x55;						//ld      A,#55
		goto fin_tolerancia_cal_volt;		//jra     fin_tolerancia_cal_volt

fin_tolerancia_cal_volt:
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

			if(!GetRegFlagState(Plantilla[numSens],f_sen2)){
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
			reePlantilla[cntmemo] = copiaPlantilla[cntmemo];
			goto nextdat;

loaddat:
			//call		rdeeprom;								// Toma el dato de la EEPROM
			//point_Y[cntmemo] = point_X[cntmemo]; 				// y cargalo a la RAM
			point_Y[cntmemo] = reePlantilla[cntmemo];
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

void grabadoLoggerBloquesCompletos(uint64_t * pointX_, uint64_t * pointBuffer_){

	uint64_t * pointInitPage_ = getAddressPage((uint32_t) pointX_); 		// Apuntador Inicio de Pagina
	uint32_t sizeCopy = ( ((uint32_t) cntBlockFlash & 0xF)) << 4;			// Cantidad de Bloques de 128 Bytes Completos
	uint8_t * pointX_126 = (uint8_t *) pointX_;								// Puntero para dirección 126 del Bloque de 128 Bytes

	if(pointX_126[126] != 0 && pointX_126[126] != 0xFF){// Revisa si existe un Bloque de 128 Bytes esta incompleto
		// Lectura de la Pagina en FLASH, hasta los bloques que estan completos
		for(uint32_t i=0; i<sizeCopy; i++){
			pointBuffer_[i] = pointInitPage_[i];// Copia de los bloques de 128 bytes completos
		}

		// Realiza El borrado de la Pagina en caso de encontrar un Bloque de 128 Bytes Incompleto
		uint32_t numberPage_ = getNumberPage((uint32_t) pointX_);
		erasePage(numberPage_);

		// Grabado de Bloques de 128 Bytes Completos
		writeFLASH(pointInitPage_, pointBuffer_,sizeCopy);
	}

}

























