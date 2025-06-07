

#include "main.h"
#include "board_PFULLDEF.h"
#include "commonMainAsm.h"
#include "customMain.h"


// rutina refrigera Adaptada CTOF Completa ..............


//***************************** Traduccion Completa *******************************************
void main10(void){
		//Operacion exlusiva para el control SW Display i2c
		if(!portX[dp_sw]){
			goto	put_dp_sw_low;
		}
		HAL_GPIO_WritePin(PFULLDEF_dp_sw, GPIO_PIN_SET);        //28-May-2024:  Enciende DPY I2C
		goto	end_dp_sw;			//jra		end_dp_sw
put_dp_sw_low:
		HAL_GPIO_WritePin(PFULLDEF_dp_sw, GPIO_PIN_RESET);      //28-May-2024:  Apaga DPY I2C
end_dp_sw:


//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//;			Relevador 1 40A			J7:2....PA5: Compresor
//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//(1)Operacion exlusiva para el compresor
		if(!GetRegFlagState(Plantilla[logicos2],modLogic)){
				goto	noModlog_00;
		}
		if(!portX[rel_dh]){
			goto	put_rel_co_low;
		}
		goto	put_rel_co_high;		//jra		put_rel_co_high
noModlog_00:
		if(!portX[rel_co]){				//;RM_20220622 Cambio de puerto para compresor en PCB V3
			goto	put_rel_co_low;
		}
put_rel_co_high:
		  HAL_GPIO_WritePin(PFULLDEF_rel_co, GPIO_PIN_SET);        //28-May-2024:  Enciende compresor
		  goto		end_rel_co;		//jra		end_rel_co
put_rel_co_low:
		  HAL_GPIO_WritePin(PFULLDEF_rel_co, GPIO_PIN_RESET);      //28-May-2024:  Apaga compresor
end_rel_co:


//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//;			Relevador 4 10A				J7:4....PE6: Aux./Luz
//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//;(2)Operacion exlusiva para  Aux.Luz				-----Manuel 10-Mar-2022: Para ADaptar pines
		if(!GetRegFlagState(Plantilla[logicos2],modLogic)){
			goto	noModlog_01;
		}
		if(!portX[rel_co]){
			goto	put_rel_lz_low;
		}
		goto	put_rel_lz_high;		//jra		put_rel_co_high
noModlog_01:
		if(!portX[rel_lz]){				//;RM_20220622 Cambio de puerto para compresor en PCB V3
			goto	put_rel_lz_low;
		}
put_rel_lz_high:
		HAL_GPIO_WritePin(PFULLDEF_rel_lz, GPIO_PIN_SET);        //28-May-2024:  Enciende  Aux.Luz
		goto		end_rel_lz;		//jra		end_rel_co
put_rel_lz_low:
		HAL_GPIO_WritePin(PFULLDEF_rel_lz, GPIO_PIN_RESET);      //28-May-2024:  Apaga  Aux.Luz
end_rel_lz:


//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//;			Relevador 2 40A     J7:1....PA2: Ventilador o segundo Compresor
//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//;(3)Operacion exlusiva para Ventilador			-----Manuel 10-Mar-2022: Para ADaptar pines
		if(!portX[rel_fn]){
			goto	put_rel_fn_low;
		}
		HAL_GPIO_WritePin(PFULLDEF_rel_fn, GPIO_PIN_SET);        //28-May-2024:  Enciende Ventilador
		goto		end_rel_fn;		//jra		end_rel_fn
put_rel_fn_low:
		HAL_GPIO_WritePin(PFULLDEF_rel_fn, GPIO_PIN_RESET);      //28-May-2024:  Apaga Ventilador
end_rel_fn:

//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//;			Relevador 3 20A		J7:3....PE7: Deshielo
//;-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//;(4)Operacion exlusiva para Deshielo
		cntproc++;
		if(!GetRegFlagState(Plantilla[logicos2],modLogic)){
			goto	noModlog_02;
		}
		if(!portX[rel_lz]){
			goto	put_rel_dh_low;
		}
		goto	put_rel_dh_high;		//jra		put_rel_co_high
noModlog_02:
		if(!portX[rel_dh]){				//;RM_20220622 Cambio de puerto para compresor en PCB V3
			goto	put_rel_dh_low;
		}
put_rel_dh_high:
//		if(!GetRegFlagState(cntproc,0)){			//btjf		cntproc,#0,put_rel_dh_low;		/ El contador de procesos esta en un valor non?
//			goto	put_rel_dh_low;
//		}
		HAL_TIM_PWM_Start (&htim1,TIM_CHANNEL_3);
//		HAL_GPIO_WritePin(PFULLDEF_rel_dh, GPIO_PIN_SET);        //bset		PE_ODR,#rel_dh;	/ Borra el puerto para generar tono   ;RM_20220622 Cambio de puerto para deshielo en PCB V3   RM_20230208 Equipo peñafiel relevador de deshielo es de lámpara
		goto		end_rel_dh;		//jra		end_rel_co
put_rel_dh_low:
		HAL_TIM_PWM_Stop (&htim1,TIM_CHANNEL_3);
//		HAL_GPIO_WritePin(PFULLDEF_rel_dh, GPIO_PIN_RESET);      /// Borra el puerto para generar tono  ;RM_20220622 Cambio de puerto para deshielo en PCB V3    RM_20230208 Equipo peñafiel relevador de deshielo es de lámpara
end_rel_dh:


	//;----------------------------
	cntbase++;			// Incrementa contador base
    if (cntbase >= 40){
        asm ("nop");
        asm ("nop");
    	cntbase =0;			// Inicia una nueva centÃ©sima//Manuel 03-Mar-2022	 80;				// Ya contÃ³ 80 octavos de milÃ©sima?
    }
	//;----------
}

//***************************** Traduccion Completa *******************************************
void retardoDeActuadores(void){
		//Revisa el retardo para encender COMPRESOR por cruece por cero
			if(!GPIOR0[f_comp]){						//btjf  	GPIOR0,#f_comp,revisa_ret_comp_00          ;//¿Debe prenderse el COMPRESOR?
				goto 	revisa_ret_comp_00;
			}
			if(cruze_por_cero[1]){			//btjt   cruze_por_cero,#1,dec_retcz_comp          ;//SI, Si ya arranco el cruce solo decrementa
				goto	dec_retcz_comp;
			}
			if(!cruze_por_cero[0]){		//btjf   cruze_por_cero,#0,revisa_ret_deshielo   ;//Espera a detectar cruce por cero
				goto	revisa_ret_deshielo;
			}
			cruze_por_cero[1] = 1;			//bset    cruze_por_cero,#1                    ;//Ya se detecto el cruce por cero, ya solo decrementa
dec_retcz_comp:
			decwreg(&retzc_ms_compresor);		//	ldw			X,#retzc_ms_compresor   ;//Decrementa cada 1ms los retardos
			goto	revisa_ret_deshielo;		//	jra     revisa_ret_deshielo

revisa_ret_comp_00:
			cruze_por_cero[1] = 0;			//	;bres    cruze_por_cero,#1                        ;//Limpia bandera

//------------------------------------------------------------------------------------------
revisa_ret_deshielo:			//Revisa el retardo para encender DESHILEO por cruece por cero
			if(!GPIOR0[f_dh]){						//btjf  	GPIOR0,#f_dh,revisa_ret_desh_00            ;//¿Debe prenderse el DESHIELO?
				goto 	revisa_ret_desh_00;
			}
			if(cruze_por_cero[2]){			//btjt   cruze_por_cero,#2,dec_retcz_desh          ;//SI, Si ya arranco el cruce solo decrementa
				goto	dec_retcz_desh;
			}
			if(!cruze_por_cero[0]){		//btjf   cruze_por_cero,#0,revisa_ret_ventilador ;//Espera a detectar cruce por cero
				goto	revisa_ret_ventilador;
			}
			cruze_por_cero[2] = 1;			//		bset    cruze_por_cero,#2                    ;//Ya se detecto el cruce por cero, ya solo decrementa
dec_retcz_desh:
			decwreg(&retzc_ms_deshielo);		//	ldw			X,#retzc_ms_deshielo   ;//Decrementa cada 1ms los retardos
			goto	revisa_ret_ventilador;		//

revisa_ret_desh_00:
			cruze_por_cero[2] = 0;			//		bres    cruze_por_cero,#2                        ;//Limpia bandera

//------------------------------------------------------------------------------------------
revisa_ret_ventilador:      ;//Revisa el retardo para encender VENTILADOR por cruece por cero
			if(!GPIOR1[f_fan]){						// btjf  	GPIOR1,#f_fan,revisa_ret_vent_00            ;//¿Debe prenderse el DESHIELO?
				goto 	revisa_ret_vent_00;
			}
			if(cruze_por_cero[3]){			// btjt   cruze_por_cero,#3,dec_retcz_vent          ;//SI, Si ya arranco el cruce solo decrementa
				goto	dec_retcz_vent;
			}
			if(!cruze_por_cero[0]){		// btjf   cruze_por_cero,#0,revisa_ret_lampara ;//Espera a detectar cruce por cero
				goto	revisa_ret_lampara;
			}
			cruze_por_cero[3] = 1;			//		bset    cruze_por_cero,#3                    ;//Ya se detecto el cruce por cero, ya solo decrementa
dec_retcz_vent:
			decwreg(&retzc_ms_ventilador);		//  ldw			X,#retzc_ms_ventilador   ;//Decrementa cada 1ms los retardos
			goto	revisa_ret_lampara;		//

revisa_ret_vent_00:
			cruze_por_cero[3] = 0;			//bres    cruze_por_cero,#3                        ;//Limpia bandera

//------------------------------------------------------------------------------------------
revisa_ret_lampara:            ;//Revisa el retardo para encender LAMPARA por cruece por cero
			if(!GPIOR0[f_lamp]){						//btjf  	GPIOR0,#f_lamp,revisa_ret_lamp_00            ;//¿Debe prenderse la LAMPARA?
				goto 	revisa_ret_lamp_00;
			}
			if(cruze_por_cero[4]){			//btjt   cruze_por_cero,#4,dec_retcz_lamp          ;//SI, Si ya arranco el cruce solo decrementa
				goto	dec_retcz_lamp;
			}
			if(!cruze_por_cero[0]){		//btjf   cruze_por_cero,#0,fin_retardos_cruce    ;//Espera a detectar cruce por cero
				goto	fin_retardos_cruce;					//
			}
			cruze_por_cero[4] = 1;						//	bset    cruze_por_cero,#4                    ;//Ya se detecto el cruce por cero, ya solo decrementa
dec_retcz_lamp:
			decwreg(&retzc_ms_lampara);		//	ldw			X,#retzc_ms_lampara   ;//Decrementa cada 1ms los retardos
			goto	fin_retardos_cruce;		//	jra     fin_retardos_cruce
revisa_ret_lamp_00:
			cruze_por_cero[4] = 0;			//	bres    cruze_por_cero,#4                        ;//Limpia bandera

//------------------------------------------------------------------------------------------
fin_retardos_cruce:
			cruze_por_cero[0] = 0;			//bres    cruze_por_cero,#0     ;//Apaga bandera de cruce por cero
}

//------------------------------------------------------------------------------------------
//		calculando tiempo UNIX
//------------------------------------------------------------------------------------------
void calculando_tiempo_UNIX (void){
    asm ("nop");

    //#pragma asm
		//Restablece el Tiempo RTC
		/*		...
				...
				...
			Falta codigo de comandos WIFI por BLE
				...
				...
				...			*/
    //#pragma endasm

	timeUNIX = 0;

	// Si es año bisiesto y ya pasó febrero añade un día (segundo por día =86400)
	if ( (!(time_year%4))  && (time_month>2) ) {
		timeUNIX += 86400;
	}
	// Toma el número de días que han transcurrido hasta el mes actual para añadir los segundos correspondientes
	// el número días trnscurrido está almacenado en un array por lo que para direccionar el array
	// es necesario decrementar la variable de mes
	/*
	time_month--;
	while(time_month){
		time_month--;
		timeUNIX += (daysToMonth[time_month])*86400;
	}
	*/
	if(time_month){
		timeUNIX += (daysToMonth[(time_month-1)])*86400;
	}

	// Añade el numero de días transcurridos des del año 1970, incluye los años bisiestos transcurridos
	timeUNIX += ( ((time_year-1970)*365)*86400 );   // + (((time_year-1970)/4)*86400) );
	leapYears = ( ((time_year-1)-1968)/4 );
	timeUNIX += ( leapYears*86400);

	// Añade el número de días transcurridos
	timeUNIX += ((time_day - 1)*86400);

	// Añade el número de horas transcurridos
	timeUNIX += ((time_hour)*(unsigned long)3600);

	// Añade el número de minutos transcurridos
	timeUNIX += ((time_min)*60);

	// Añade el número de segundos transcurridos
	timeUNIX += time_sec;

	timeUNIX_copy = timeUNIX;

    //#pragma asm
		//Restablece el Tiempo RTC
		/*		...
				...
				...
			Falta codigo de comandos WIFI por BLE
				...
				...
				...			*/
    //#pragma endasm

}





















