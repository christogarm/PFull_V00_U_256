

#include "main.h"
#include "board_PFULLDEF.h"
#include "commonMainAsm.h"
#include "customMain.h"


//***************************** Traduccion Completa *******************************************
void main10(void){
	//Operacion exlusiva para el control SW Display i2c
		if(portX[dp_sw])
		  HAL_GPIO_WritePin(PFULLDEF_dp_sw, GPIO_PIN_SET);        //28-May-2024:  Enciende DPY I2C
		  //GPIOC->BSRR = GPIO_BSRR_BS_3;
		else
		  HAL_GPIO_WritePin(PFULLDEF_dp_sw, GPIO_PIN_RESET);      //28-May-2024:  Apaga DPY I2C
		 //GPIOC->BSRR = GPIO_BSRR_BR_3;

		//(1)Operacion exlusiva para el compresor
		if(portX[rel_co])
		  HAL_GPIO_WritePin(PFULLDEF_rel_co, GPIO_PIN_SET);        //28-May-2024:  Enciende compresor
		  //GPIOA->BSRR = GPIO_BSRR_BS_9;
		else
		  HAL_GPIO_WritePin(PFULLDEF_rel_co, GPIO_PIN_RESET);      //28-May-2024:  Apaga compresor
		  //GPIOA->BSRR = GPIO_BSRR_BR_9;

		//(2)Operacion exlusiva para  Aux.Luz				-----Manuel 10-Mar-2022: Para ADaptar pines
		if(portX[rel_lz])
		  HAL_GPIO_WritePin(PFULLDEF_rel_lz, GPIO_PIN_SET);        //28-May-2024:  Enciende  Aux.Luz
		  //GPIOC->BSRR = GPIO_BSRR_BS_9;
		else
		  HAL_GPIO_WritePin(PFULLDEF_rel_lz, GPIO_PIN_RESET);      //28-May-2024:  Apaga  Aux.Luz
		  //GPIOC->BSRR = GPIO_BSRR_BR_9;

		//;(3)Operacion exlusiva para Ventilador			-----Manuel 10-Mar-2022: Para ADaptar pines
		if(portX[rel_dh])
		  HAL_GPIO_WritePin(PFULLDEF_rel_fn, GPIO_PIN_SET);        //28-May-2024:  Enciende Ventilador
		  //GPIOA->BSRR = GPIO_BSRR_BS_8;
		else
		  HAL_GPIO_WritePin(PFULLDEF_rel_fn, GPIO_PIN_RESET);      //28-May-2024:  Apaga Ventilador
		  //GPIOA->BSRR = GPIO_BSRR_BR_8;
		//;(4)Operacion exlusiva para Deshielo
		cntproc++;
		if(portX[rel_fn] && (cntproc & 0x01))
		  HAL_GPIO_WritePin(PFULLDEF_rel_dh, GPIO_PIN_SET);        //28-May-2024:  Enciende  Deshielo
		  //GPIOA->BSRR = GPIO_BSRR_BS_10;
		else
		  HAL_GPIO_WritePin(PFULLDEF_rel_dh, GPIO_PIN_RESET);      //28-May-2024:  Apaga  Deshielo
		  //GPIOA->BSRR = GPIO_BSRR_BR_10;
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
			if(GPIOR0[f_comp] == 0){						//btjf  	GPIOR0,#f_comp,revisa_ret_comp_00          ;//¿Debe prenderse el COMPRESOR?
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
			if(GPIOR0[f_dh] == 0){						//btjf  	GPIOR0,#f_dh,revisa_ret_desh_00            ;//¿Debe prenderse el DESHIELO?
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
			if(GPIOR1[f_fan] == 0){						// btjf  	GPIOR1,#f_fan,revisa_ret_vent_00            ;//¿Debe prenderse el DESHIELO?
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
			if(GPIOR0[f_lamp] == 0){						//btjf  	GPIOR0,#f_lamp,revisa_ret_lamp_00            ;//¿Debe prenderse la LAMPARA?
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
	if ( (!(time_year%4)) && (time_month>2) ) {
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





















