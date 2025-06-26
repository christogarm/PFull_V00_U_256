


#include "main.h"
#include "board_PFULLDEF.h"
#include "commonMainAsm.h"
#include "customMain.h"
#include "ELTEC_EmulatedEEPROM.h"
#include "stm32u0xx_hal_pwr_ex.h"

// rutina refrigera Adaptada CTOF Completa ..............

uint32_t cnt_veces_muestreo_2 = 0;
//*************************************************************************************************

void muestreo(void){


		if(GetRegFlagState(reePlantilla[eelogicos2],5)){		//;¿Se usará el TRANSDUCTOR de corriente de IMBERA?
				goto	muestreo_00;
		}
		cnt_2ms_corrIMB++;									//inc     cnt_2ms_corrIMB           ;SI, obten una muestra instantanea cada 2ms
		if (cnt_2ms_corrIMB != 4){			//;El proceso muestre se ejecuta cada 250us apróx
			goto	muestreo_00;			//jrne    muestreo_00
		}
		cnt_2ms_corrIMB =0;					//clr     cnt_2ms_corrIMB         ;Han transcurrido 2ms. Limpia el contador

		ADC1->CHSELR = 0;
        ADC1->CHSELR |= ADC_CHSELR_CHSEL0;  		// Canal 0
        capturaAD ();								// muestra es a 10 bits
        ram_h = adcramh;							//;Guarda el resultado de la conversión en RAM TEMPORAL
        											//mov		  ram_l,adcraml           ;/Lee primero el MAYOR
        muestra_corrIMB += ram_h;					//;Realiza la suma acumulada de las muestras
        											//;ADC de 10 bits, 64 muestras máximo de 1,024 (si fueran 20A)

        cnt_muestras_corrIMB++;						// inc     cnt_muestras_corrIMB    ;¿Han trasncurrido 64 muestras?
		if (cnt_muestras_corrIMB != 64){			// ld      A,cnt_muestras_corrIMB  ;/
			goto	muestreo_00;					// cp      A,#64
		}											//	jrne    muestreo_00		          ;/
		cnt_muestras_corrIMB= 0;					// clr     cnt_muestras_corrIMB  ;64 muestras trasncurridas, limpia el contador
		mcorr_ponderada_1 = mcorr_ponderada_2;		//;La última muestra se conviente en la muestra anterior
													//ldw     X,muestra_corrIMB     ;Realiza el promedio de las 64 muestras obtenidas
													//ld      A,#64                 ;/
													//div     X,A                   ;/
		mcorr_ponderada_2 = muestra_corrIMB/64;		//ldw     mcorr_ponderada_2,X   ;El resultado es la muestra actual
		muestra_corrIMB = 0;						//clrw    X                     ;Limpia el acumulado de las muestras
		ram_h = mcorr_ponderada_1/2;				//;Aplica factor de ponderación de 50%
													//;a la primer muestra promediada
		STM8_16_X = mcorr_ponderada_2/2;			//;Aplica factor de ponderación de 50%
													//;a la segunda muestra promediada
		STM8_16_X = STM8_16_X + ram_h;				//;Suma las dos muestras ponderadas
		//;El factor se aplica al final para no desbordar
		fact_mul = 39; 								//mov     fact_mul,#39     ;eecorrIMB_mul  ;Aplica factor, el resultado llega en X
		fact_div = 2;								//mov     fact_div,#2      ;eecorrIMB_div  ;/
		aplica_factor();							//call    aplica_factor         ;Aplica el factor a la muestra adquirida
		corriente_IMB = STM8_16_X;					//ldw     corriente_IMB,X       ;Almacena el resultado en la variable correspondiente

muestreo_00:
		//ldw     X,#t_filtro_flanco;
		decwreg(&t_filtro_flanco);	//call    decwreg;        / Agota el tiempo para filtrar cruces por cero

		cnt_veces_muestreo++; //inc cnt_veces_muestreo/// A,cnt_veces_muestreo
		if(cnt_veces_muestreo <= 200){ 		// 200){ //cp  A,#200
			goto alterna_presente;			//jrule   alterna_presente      ;Hay alterna presente
		}
	    cnt_veces_muestreo = 0;  //clr cnt_veces_muestreo
	    cruze_por_cero[0] = 1;		//bset    cruze_por_cero,#0

//	    if(GetRegFlagState(flagsBattery, batOFF)){ //btjt	flagsBattery,#batOFF,batOFF_OK
//          	goto batOFF_OK;
//        }
//	  	flagsLogger2 [2] = 1;//bset	flagsLogger2,#2
//	  	goto error_muestreo; //jp error_muestreo

//batOFF_OK:
	    flagsLogger2 [0] = 1; //bset	flagsLogger2,#0; /indica un fallo de energía

// ;-------------------------------------------------------------------------------
// ;RM_20240816 Evita grabados y HALT si no se ha calibrado el control


	    if(reef_voltaje == 0x3C){
			goto grabadoEmergencia_00; //		jreq    grabadoEmergencia_00         ;
		}
		goto error_muestreo;							//jp      error_muestreo

//;-------------------------------------------------------------------------------
grabadoEmergencia_00:
				//;------------ Grabado de emergencia por fallo de energía
		if(edorefri >= 2){					// ya pasó por indica o autoprueba ?
			goto grabadoEmergencia; 		// sí, continua
		}
		goto grabadoEmergenciaFin;			//jp		grabadoEmergenciaFin;							/ no, espera


//		;							btjf	flagsLogger2,#0,grabadoEmergenciaFin; / hay fallo de energía ? No, continúa sin grabado de emergencia
//		;							btjt	flagsLogger2,#1,grabadoEmergenciaFin; / ya se ejecuto el grabado de emergencia ? Sí, no vuelvas a grabar.


grabadoEmergencia:
		//;primero guarda lo que aun hay en el buffer .

		cntBlockFlash = cntBlockDATA;//mov		cntBlockFlash,cntBlockDATA;
		cntByteBlock = cntByteBlockDATA; //mov	cntByteBlock,cntByteBlockDATA
		//ldw		X,#data_buffer
		//ldw		dirBuffer,X
		dirBuffer = &data_buffer[0]; //--------------------
		//ldw		X,#dataLogger
		//ldw		dirLogger,X
		dirLogger = &dataLogger[0];// ------------------------

		//;---- Carga en penúltimo byte del buffer el numero de bytes capturados en el actual buffer
	    //	LDW		X,dirBuffer
	    //  resulh = 0; //clr resulh
	    //  resull = 126;//mov resull,#126
//	    dirBuffer = dirBuffer + 126; //addw	X,resulh
        //ld		A,cntByteBlock

		// Se agrega este parche debido a la naturaleza de la memoria
		// CGM 23/04/2025
		if(cntByteBlock == 0){
			dirBuffer[126] = 0xFF;	//ld		(X),A ---------?
		}
		else{
			dirBuffer[126] = cntByteBlock;	//ld		(X),A ---------?
		}



		//;---- Graba buffer en bloque de flash
	    ProgMemCode = 0xAA; //mov ProgMemCode,#$AA;
	                   //ld    A,cntBlockFlash;
	                   //ldw		X,#128;
	    			   //mul		X,A; -----------------?
	    			   //addw	X,dirLogger;--------------?
//	    dirPointer = dirLogger + (128 * cntBlockFlash);//LDW dirPointer,X
	    dirPointer = &dirLogger[128*cntBlockFlash];
	    // cntBlockFlash = dirBuffer;//LDW X,dirBuffer;
	    dataPointer = &dirBuffer[0];//LDW dataPointer,X

	    dirBufferPage = &bufferPageDATA[0];

	    grabadoLoggerBloquesCompletos(dirPointer, dirBufferPage);

	    GRABA_BLOCK();			//call GRABA_BLOCK



	//---------------------------------------------------------------------------------------------
	    // ldw		X,cntRegDATA
	    cntReg = cntRegDATA;//ldw cntReg,X
	    // ldw X,#eeCntRegDATA
	    cntRegPNT = &eeCntRegDATA;//ldw cntRegPNT,X
	    // call save_cntReg
	    save_cntReg();

	    //;primero guarda lo que aun hay en el buffer .
	    cntBlockFlash = cntBlockEVENT;//mov	cntBlockFlash,cntBlockEVENT
	    cntByteBlock = cntByteBlockEVENT;//mov cntByteBlock,cntByteBlockEVENT
	    //ldw X,#event_buffer
	    //ldw dirBuffer,X
	    dirBuffer = &event_buffer[0];
	    //ldw X,#eventLogger
	    //ldw dirLogger,X
	    dirLogger = &eventLogger[0];

	    // ;---- Carga en penúltimo byte del buffer el numero de bytes capturados en el actual buffer
//	    uint8_t *point_X;    		//LDW	X,dirBuffer
	                    							//clr		resulh
	                    							//mov		resull,#126
//	    point_X = dirBuffer + 126; //addw	X,resulh
	    //ld		A,cntByteBlock
//	    *point_X = cntByteBlock;//ld (X),A ----------------?
	    // Se agrega este parche debido a la naturaleza de la memoria
	    // CGM 23/04/2025
	    if(cntByteBlock == 0){
	    	dirBuffer[126] = 0xFF;	//ld		(X),A ---------?
	    }
	    else{
	    	dirBuffer[126] = cntByteBlock;	//ld		(X),A ---------?
	    }

	    //;---- Graba buffer en bloque de flash
	    ProgMemCode = 0xAA; //mov		ProgMemCode,#$AA;
	    //ld    A,cntBlockFlash;
	    //ldw		X,#128;
	    //;//mul		X,A;
	    //addw	X,dirLogger; ------------------?
	    //LDW		dirPointer,X
	    //LDW		X,dirBuffer;
	    dirPointer = &dirLogger[128 * cntBlockFlash]; // -----------------------------------?
	    //LDW		dataPointer,X
	    dataPointer = &dirBuffer[0];//------------------------------------?

	    dirBufferPage = &bufferPageEVENT[0];

	    grabadoLoggerBloquesCompletos(dirPointer, dirBufferPage);

	    GRABA_BLOCK();//call GRABA_BLOCK

	    //ldw		X,cntRegEVENT
	    //ldw		cntReg,X
	    cntReg = cntRegEVENT;
	    //ldw		X,#eeCntRegEVENT
	    //ldw		cntRegPNT,X
	    cntRegPNT = &eeCntRegEVENT;
	    save_cntReg();		//call	save_cntReg
	    save_timeUNIX();	//call	save_timeUNIX

	    for(uint8_t i=0; i<8; i++)
	    	flagsEvent[i] = 0;


grabadoEmergenciaFin:
    // Deshabilitacion de Perifericos Necesarios para Bajo Consumo JTA

	GPIOA->MODER |= 0x3FFFFFFF;
	GPIOB->MODER |= 0xFFFFFFFF;
	GPIOC->MODER |= 0xFFFFFFFF;					//Puertos Colocados en analogico menos GPIOA15 que es la interrupcion para salir de bajo consumo
	GPIOD->MODER |= 0xFFFFFFFF;
	GPIOE->MODER |= 0xFFFFFFFF;
	GPIOF->MODER |= 0xFFFFFFFF;

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();				//Deshabilitacion de CLK para puertos
	__HAL_RCC_GPIOF_CLK_DISABLE();

	while(HAL_I2C_DeInit(&hi2c1) != HAL_OK);   //Deshabilitacion de perifericos por medio de funcion DEInit
	while(HAL_UART_DeInit(&huart2) != HAL_OK);
	while(HAL_UART_DeInit(&huart4) != HAL_OK);
	//while(HAL_ADC_DeInit(&hadc1) != HAL_OK); //HAL DEInit ADC
	ADC_Deinit_Func();						   //Deinicializacion de ADC por medio de acceso directo a registros

	__HAL_RCC_DMA1_CLK_DISABLE();              //Deshabilitacion de CLK para DMA1

	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);         //Habilitacion de interrupcion por medio de PIN PA15
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn); //Deshabilitacion de interrupciones DMA1
	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX_OVR_IRQn);

	HAL_SuspendTick();                         //Suspencion tickSys HAL

	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI); //Entada de modo de bajo consumo STOP2, tener en cuenta en configuration bits para freeze de WatchDOg

	HAL_ResumeTick();							//Regreso de Bajo consumo por medio de corriente Alterna, reestablecimiento de Systick

	HAL_IWDG_Refresh(&hiwdg);                   //Clear Watch
	reconfigura_perif();						//Reconfiguracion de perifericos necesarios

	cnt_veces_muestreo_2 = 0;

	flagsEvent[3] = 1;
	retPowerOn = 10;
	edorefri = 0;
	cntblkh = 0;
	flagsa[7] = 1;// flagsa = 0x81;
	flagsa[0] = 1;
	cntseg = 0;

	timeRstBLE = 1;     // Reinicio de maquina de Estados

	for(uint8_t k=0; k<8; k++)
		flagsBattery[k] = 0;
		//;Apaga Perifericos RGM_07-AGO-2023
ibus_ocupado_halt:


DoSleep:

	goto error_muestreo;//jp error_muestreo

//;===============================================================================
//;El control tiene voltaje de alimentación de CA presente, se pueden realizar
//;mediciones de voltaje y corriente.

alterna_presente:
		if(t_filtro_flanco != 0){		//	ld      A,t_filtro_flanco       ;FILTRO DE TIEMPO PARA DETECIÓN DE FLANCO DE BAJADA
			goto	muestra_0;				// 	jrne    muestra_0               ;continua muestreando antes de detectar FLANCOS
		}
		detecta_flanco();			//call    detecta_flanco        //;¿Se detecto un flanco de bajada?
		if(!s_reloj[2]){			//btjf    s_reloj,#2,muestra_0  ;NO, Continua muestreando
			goto muestra_0;
		}
		t_filtro_flanco = 63;		//mov     t_filtro_flanco,#63   ;VENTANA DE 15.75ms - 16.66ms / .250us = 66.64 SE PIERDEN CICLOS
		cnt_veces_muestreo = 0;		//clr     cnt_veces_muestreo
		cnt_mues_ant = cnt_mues;		//mov     cnt_mues_ant,cnt_mues
		ban_muestreo[0] = 1;	//bset    ban_muestreo,#0     ;Activa bandera que indica el inicio de un nuevo ciclo
		ban_muestreo[1] = 1;	//bset    ban_muestreo,#1     ;Activa bandera que indica que transcurrio un ciclo para tiempos
		//mov     c_sigma_cuad_sampl_4_1,sigma_cuad_sampl_4_1
		//mov     c_sigma_cuad_sampl_3_1,sigma_cuad_sampl_3_1
		//mov     c_sigma_cuad_sampl_2_1,sigma_cuad_sampl_2_1
		//mov     c_sigma_cuad_sampl_1_1,sigma_cuad_sampl_1_1
				  c_sigma_cuad_sampl_1 = sigma_cuad_sampl_1;
		//mov     c_sigma_cuad_sampl_4_2,sigma_cuad_sampl_4_2
		//mov     c_sigma_cuad_sampl_3_2,sigma_cuad_sampl_3_2
		//mov     c_sigma_cuad_sampl_2_2,sigma_cuad_sampl_2_2
		//mov     c_sigma_cuad_sampl_1_2,sigma_cuad_sampl_1_2
				  c_sigma_cuad_sampl_2 = sigma_cuad_sampl_2;
		goto     muestra_0;		//jra     muestra_0


muestra_0:
		muestreo_trms();		//call    muestreo_trms

//;===============================================================================
//;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Proceso CALCULA_TRMS

calcula_trms:   //;Con las muestras adquiridas, calcula los valores trms de i,v

		if(ban_muestreo[1]){			//btjt    ban_muestreo,#1,cant_muestras    ;¿Ha transcurrido un ciclo?
			goto cant_muestras;
		}
		goto	fin_calcula_trms;		//jp     fin_calcula_trms                ;NO, termina calculo de trms


cant_muestras:       // ;-----------------------  ¿Cantidad de muestras adecuadas?
		ban_muestreo[1] = 0;					//bres    ban_muestreo,#1  ;Limpia bandera que indica que transcurrio 1 ciclo

		goto	calcula_itrms;					//jp      calcula_itrms

		if(cnt_mues_ant == 64){			//;¿Cantidad de muestras adecuada? Depende modelo
			goto calcula_itrms;			//;Equipos rectificador de onda completa 64 Muestras
		}								//jreq    calcula_itrms       ;Muestreo válido, calcula itrms
		goto	fin_calcula_trms;	    //jp      fin_calcula_trms  ;Fin proceso sin incluir muestra para promediar


calcula_itrms:	    //;--------  Calcula la corriente trms de 1 ciclo en miliampers
//		mov     c_sigma_cuad_sampl_4,c_sigma_cuad_sampl_4_1   ;CANAL 1 es la corriente
//		mov     c_sigma_cuad_sampl_3,c_sigma_cuad_sampl_3_1   ;/
//		mov     c_sigma_cuad_sampl_2,c_sigma_cuad_sampl_2_1   ;/
//		mov     c_sigma_cuad_sampl_1,c_sigma_cuad_sampl_1_1   ;/
		c_sigma_cuad_sampl = c_sigma_cuad_sampl_1;
		prom_muestras();			// call    prom_muestras   ;Calcula el promedio de las muestras, utiliza datos de la suma acumulada
		calcula_raiz();				// call    calcula_raiz    ;Calcula la raíz cuadrada del promedio y regresa en X el resultado

ajuste_ima:      //;-----  Aplica factor a dato de 2 bytes (X) para convertir a mA
		fact_mul = 55;			//mov     fact_mul,#55    ;eecorr_mulsal     ;Aplica factor, el resultado regresa en X
		fact_div = 2; 			//mov     fact_div,#2     ;eecorr_divsal     ;/
		aplica_factor();  		//call    aplica_factor              ;/


acumula_muestra_i:      //;------------  Suma acumulada de 8 muestras de corriente
//		  mov     waux,acu8m_corriente_1     ;Realiza la suma acumulada de 8 muestras
//		  mov     wreg,acu8m_corriente_2     ;Suma entre X y acu8m_corriente_[1:3]
//			call    suma_acumulada_8m          ;/
//		  mov     acu8m_corriente_1,waux     ;/
//		  mov     acu8m_corriente_2,wreg     ;/
		  acu8m_corriente = acu8m_corriente + STM8_16_X;

calcula_vrms:      //;---------------  Calcula el voltaje trms de 1 ciclo en volts
//		    mov     c_sigma_cuad_sampl_4,c_sigma_cuad_sampl_4_2   ;CANAL 2 es el voltaje
//		    mov     c_sigma_cuad_sampl_3,c_sigma_cuad_sampl_3_2   ;/
//		    mov     c_sigma_cuad_sampl_2,c_sigma_cuad_sampl_2_2   ;/
//		    mov     c_sigma_cuad_sampl_1,c_sigma_cuad_sampl_1_2   ;/
			c_sigma_cuad_sampl = c_sigma_cuad_sampl_2;
			prom_muestras();			// call    prom_muestras   ;Calcula el promedio de las muestras, utiliza datos de la suma acumulada
			calcula_raiz();				// call    calcula_raiz    ;Calcula la raíz cuadrada del promedio y regresa en X el resultado

acumula_muestra_v:      //;------------  Suma acumulada de 8 muestras de corriente
//			  mov     waux,acu8m_voltaje_1       ;Realiza la suma acumulada de 8 muestras
//			  mov     wreg,acu8m_voltaje_2       ;Suma entre X y acu8m_corriente_[1:3]
//			  call    suma_acumulada_8m          ;/
//			  mov     acu8m_voltaje_1,waux       ;/
//			  mov     acu8m_voltaje_2,wreg       ;/
			acu8m_voltaje = acu8m_voltaje + STM8_16_X;



esp_8muestras:       //;------------  Espera 8 muestras para actualizar mediciones
			cnt_mues_ciclos++;		//inc     cnt_mues_ciclos            ;Una muestra más
			if(cnt_mues_ciclos < 8){			//ld      A,cnt_mues_ciclos          ;¿Ya transcurrieron 8 muestras?
				goto fin_calcula_trms;			//cp      A,#8
			}									//jrult   fin_calcula_trms           ;NO, termina proceso

			cnt_mues_ciclos = 0;				// clr     cnt_mues_ciclos          ;Limpia el contador de muestras de ciclos

//			mov     waux,acu8m_voltaje_1     ;SI, calcula el promedio de voltaje (8m)
//			mov     wreg,acu8m_voltaje_2     ;/
//			call    calcula_prom_8m          ;/
//			ldw     voltaje_trms,X           ;Coloca el voltaje real medido en variable
			voltaje_trms = (uint16_t)(acu8m_voltaje/8);
//			;-------------------------------------------------------------------------------
//			;RM_20241206 Ajuste para voltajes mayores a 180Vca
//			clr     waux                     ;Ajusta la medición de voltaje para
//			mov     wreg,#180                ;valores mayores a 180Vca
//			cpw     X,waux                   ;/
//			jrule   coloca_voltaje           ;/
//			decw    X                      ;/
//			decw    X                      ;/
//
//			;RM_20241206 Truncamiento de medición de voltaje a 1 byte para desiciones
//			clr     waux                     ;Valida que el voltaje medido este
//			mov     wreg,#254                ;en un rango de 0-254 para tomar decisiones
//			cpw     X,waux                   ;y enviar por BLE
//			jrule   coloca_voltaje           ;/
//			ldw     X,#254                 ;/
coloca_voltaje:                      		//;/
			//	ld      A,XL                     ;/
			//	ld      volt_trms,A              ;/
			volt_trms = (uint8_t)voltaje_trms;
//;-------------------------------------------------------------------------------
//			clr     acu8m_voltaje_1          ;Limpia la suma acumulada de muestras
//			clr     acu8m_voltaje_2		       ;/
			acu8m_voltaje = 0;


//			mov     waux,acu8m_corriente_1   ;Calcula el promedio de corriente (8m)
//			mov     wreg,acu8m_corriente_2   ;/
//
//			call    calcula_prom_8m          ;/
			STM8_16_X = (uint16_t)(acu8m_corriente/8);
//			subw    X,#0      ;eecorr_ajuste          ;Realiza el ajuste de corriente
//			ldw     corriente_trms,X         ;/
			corriente_trms = STM8_16_X;
//			;ldw     variable_corriente,X
//			clr     acu8m_corriente_1        ;Limpia la suam acumulada de muestras
//			clr     acu8m_corriente_2        ;/
			acu8m_corriente = 0;


voltaje_calibrado:
			/*
			 * SALTOOOOOOO para evistar el calibra Reset
			 */
			goto corriente_calibrada;

			if(reef_voltaje == 0x3C){
				goto corriente_calibrada; //		jreq    grabadoEmergencia_00         ;
			}
			calibra_voltaje();		//call    calibra_voltaje          ;NO, calibra voltaje


corriente_calibrada:
//;ld      A,eecorr_calibra           ;¿Esta calibrado en corriente el equipo?
//;cp      A,#$3C                     ;/
//;jreq    fin_calcula_trms           ;SI, el equipo ya fue calibrado, continua
//	  ;call    calibra_corriente        ;NO, calibra corriente


error_muestreo:
fin_calcula_trms:


			if(GetRegFlagState(Plantilla[numSens],f_senCo)){		//;¿Se usará el TRANSDUCTOR de corriente de IMBERA?
				goto	selecciona_transductor;
			}
			//; con sensor de corriente deshabilitado la corriente es cero
											//clrw		X
			variable_corriente = 0;			//ldw     variable_corriente,X
			goto	fin_calcula_trms_1;		//jra     fin_calcula_trms_1
selecciona_transductor:
			if(GetRegFlagState(reePlantilla[eelogicos2],5)){		//btjt    eelogicos2,#5,transductor_ELTEC
				goto	transductor_ELTEC;
			}
		    										//ldw     X,corriente_IMB
			variable_corriente = corriente_IMB;		//ldw     variable_corriente,X
			goto 	fin_calcula_trms_1;	    		//jra     fin_calcula_trms_1
transductor_ELTEC:
													//ldw     X,corriente_trms
			variable_corriente = corriente_trms;	//ldw     variable_corriente,X



fin_calcula_trms_1:
//;jp      inicio_recurrente

//;>>>>>>>>>>>>>>>>>>>>><>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  FIN Proceso CALCULA_TRMS
//;===============================================================================


}

void ADC_Deinit_Func()
{
 	const uint32_t tmp_adc_is_disable_on_going = ((ADC1->CR & ADC_CR_ADDIS) != 0) ? 1UL : 0UL;

	if (((ADC1->CR & ADC_CR_ADEN) != 0) && (tmp_adc_is_disable_on_going == 0UL))
	{
		if ((ADC1->CR & (ADC_CR_ADSTART | ADC_CR_ADEN)) == ADC_CR_ADEN)
		{
			ADC1->CR = (ADC1->CR & ~ADC_CR_BITS_PROPERTY_RS) | ADC_CR_ADDIS;
			ADC1->ISR = (ADC_FLAG_EOSMP | ADC_FLAG_RDY);
		}

		CLEAR_BIT(ADC1->CR, (ADC_CR_ADVREGEN | ADC_CR_BITS_PROPERTY_RS));

		ADC1->IER &= ~(ADC_IT_AWD3 | ADC_IT_AWD2 | ADC_IT_AWD1 | ADC_IT_OVR |
	               ADC_IT_EOS | ADC_IT_EOC | ADC_IT_EOCAL | ADC_IT_EOSMP | ADC_IT_RDY);

		ADC1->ISR = (ADC_FLAG_AWD3 | ADC_FLAG_AWD2 | ADC_FLAG_AWD1 | ADC_FLAG_OVR |
	             ADC_FLAG_EOS | ADC_FLAG_EOC | ADC_FLAG_EOCAL | ADC_FLAG_EOSMP | ADC_FLAG_RDY);

		ADC1->CFGR1 &= ~(ADC_CFGR1_AWD1CH   | ADC_CFGR1_AWD1EN  | ADC_CFGR1_AWD1SGL | ADC_CFGR1_DISCEN |
	                 ADC_CFGR1_CHSELRMOD | ADC_CFGR1_AUTOFF |
	                 ADC_CFGR1_WAIT | ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD |
	                 ADC_CFGR1_EXTEN   | ADC_CFGR1_EXTSEL | ADC_CFGR1_ALIGN  | ADC_CFGR1_RES    |
	                 ADC_CFGR1_SCANDIR | ADC_CFGR1_DMACFG | ADC_CFGR1_DMAEN);

		ADC1->SMPR &= ~ADC_SMPR_SMP1;
		ADC1->CHSELR &= ~(ADC_CHSELR_SQ_ALL);

	  	ADC1->AWD1TR &= ~(ADC_AWD1TR_HT1 | ADC_AWD1TR_LT1);
		ADC1->AWD2TR &= ~(ADC_AWD2TR_HT2 | ADC_AWD2TR_LT2);
	  	ADC1->AWD3TR &= ~(ADC_AWD3TR_HT3 | ADC_AWD3TR_LT3);

		ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;

		ADC1_COMMON->CCR &= ~(ADC_CCR_VBATEN | ADC_CCR_TSEN | ADC_CCR_VREFEN | ADC_CCR_PRESC);

		__HAL_RCC_ADC_CLK_DISABLE();

	    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4);
	    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

		//ADC1-> ADCGroupRegularSequencerRanks = 0x00000000UL;

		ADC1 -> ISR 	= 0x00000000;
		ADC1 -> IER 	= 0x00000000;
		ADC1 -> CR  	= 0x00000000;
		ADC1 -> CFGR1 	= 0x00000000;
		ADC1 -> CFGR2   = 0x00000000;
		ADC1 -> SMPR    = 0x00000000;
		ADC1 -> AWD1TR  = 0x0FFF0000;
		ADC1 -> CHSELR  = 0x00000000;
		ADC1 -> AWD3TR  = 0x0FFF0000;
		ADC1 -> DR      = 0x00000000;
		ADC1 -> AWD2CR  = 0x00000000;
		ADC1 -> AWD3CR  = 0x00000000;
		ADC1 -> CALFACT = 0x00000000;
		//ADC1 -> CCR     = 0x00000000;
	}

}

