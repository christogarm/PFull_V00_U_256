


#include "main.h"
#include "board_PFULLDEF.h"
#include "commonMainAsm.h"
#include "customMain.h"
#include "ELTEC_EmulatedEEPROM.h"
#include "stm32u0xx_hal_pwr_ex.h"

uint32_t cnt_veces_muestreo_2 = 0;
//*************************************************************************************************

void muestreo(void){



	     //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);  PIn TEST


		cnt_veces_muestreo++; //inc cnt_veces_muestreo/// A,cnt_veces_muestreo


		if(cnt_veces_muestreo <= 200){ // 200){ //cp  A,#200
			goto detecta_flanco;//jrule detecta_flanco
		}

//		if(cnt_veces_muestreo < 3)
//			cnt_veces_muestreo_2++;
//		else
//			cnt_veces_muestreo_2 = 0;
//
//		if(cnt_veces_muestreo_2 < 200)
//			goto detecta_flanco;

	    cnt_veces_muestreo = 0;  //clr cnt_veces_muestreo
	    cruze_por_cero[0] = 1;		//bset    cruze_por_cero,#0

//	    if(GetRegFlagState(flagsBattery, batOFF)){ //btjt	flagsBattery,#batOFF,batOFF_OK
//          	goto batOFF_OK;
//        }
//	  	flagsLogger2 [2] = 1;//bset	flagsLogger2,#2
//	  	goto error_muestreo; //jp error_muestreo

//batOFF_OK:
	    flagsLogger2 [0] = 1; //bset flagsLogger2,#0


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
	    dirBuffer[126] = cntByteBlock;	//ld		(X),A ---------?


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
	    dirBuffer[126] = cntByteBlock;

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
	save_timeUNIX();
	GPIOA->MODER |= 0x3FFFFFFF;
	GPIOB->MODER |= 0xFFFFFFFF;
	GPIOC->MODER |= 0xFFFDFFFF;
	GPIOD->MODER |= 0xFFFFFFFF;
	GPIOE->MODER |= 0xFFFFFFFF;
	GPIOF->MODER |= 0xFFFFFFFF;

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOF_CLK_DISABLE();

	while(HAL_I2C_DeInit(&hi2c1) != HAL_OK);
	while(HAL_UART_DeInit(&huart2) != HAL_OK);
	while(HAL_UART_DeInit(&huart4) != HAL_OK);
	//ADC_Deinit_Func();
	while(HAL_ADC_DeInit(&hadc1) != HAL_OK);
	__HAL_RCC_DMA1_CLK_DISABLE();

	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2000, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX_OVR_IRQn);

//	IWDG->KR   = 0x00005555;
//	IWDG->PR   = 0X6;
//	IWDG->RLR  = 0X2D3;
//	while((IWDG->SR & IWDG_SR_RVU) != 0x00u)
//	HAL_IWDG_Refresh(&hiwdg);



sleep_rt:

	HAL_SuspendTick();

	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

	HAL_ResumeTick();
	if(bandera_RTC)
	{
		bandera_RTC = 0;
		goto sleep_rt;
	}

	//HAL_IWDG_Refresh(&hiwdg);
	reconfigura_perif();

	//for (int i = 0; i < 10; i++)
	//{
	//	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_10);
	//	HAL_Delay (15);
	//	HAL_IWDG_Refresh(&hiwdg);
	//}
	//HAL_IWDG_Refresh(&hiwdg);

	flagsEvent[3] = 1;
	retPowerOn = 10;
	edorefri = 0;
	cntblkh = 0;
	flagsa[7] = 1;// flagsa = 0x81;
	flagsa[0] = 1;
	cntseg = 0;
	for(uint8_t k=0; k<8; k++)
		flagsBattery[k] = 0;

		//;Apaga Perifericos RGM_07-AGO-2023


ibus_ocupado_halt:


DoSleep:
goto error_muestreo;//jp error_muestreo

//;=========================================  Rutina DETECCIÓN DE FLANCO DE SUBIDA
//;La detección de cruce por cero se realiza por umbral en el pin de MUESTREO.

detecta_flanco:						 //;Más de 50ms sin cruces por cero
		s_reloj[2] = 0;//bres s_reloj,#2

up_anterior:

		if(s_reloj[0]){	//btjt s_reloj,#0,alto   [actual]	   ;¿El valor de la bandera "actual" es 1?
			goto alto;
		}
		s_reloj[1] = 0;   				//bres s_reloj,#1    ;[anterior]  ;NO: Anterior <- 0
		goto end_up_anterior;//jra end_up_anterior

alto:
		s_reloj[1] = 1; 	//bset s_reloj,#1		;[anterior]   ;SI: Anterior <- 1

end_up_anterior:

up_actual:
		//uint16_t   adcram = 0;
		//mov ADC1_CR1,#%00100011   ;
		//mov ADC1_SQR1,#%10000000	;
		//mov ADC1_SQR2,#%00000000	;
		//mov ADC1_SQR3,#%00000010	;
		//mov ADC1_SQR4,#%00000000	;
		//call capturaAD   ********************CAALL
		//waux = adcramh;//mov		  waux,adcramh ;
		//wreg = adcraml;//mov wreg,adcraml ;
		//ldw     Y,waux       ;
			// 19-AGO-2024		ADC_Select_CH5 ();
			// 19-AGO-2024		HAL_ADC_Start(&hadc); // start the adc
			// 19-AGO-2024		HAL_ADC_PollForConversion(&hadc, 1000); // poll for conversion
			// 19-AGO-2024		adcram = HAL_ADC_GetValue(&hadc); // get the adc value
			// 19-AGO-2024		HAL_ADC_Stop(&hadc);
		// ----------------------Codigo Julio Torres
//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
		//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
//		ADC1->CHSELR = ADC_CHSELR_CHSEL5;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL10;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL0;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL18;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL14;  // Canal 0

		ADC1->CHSELR |= ADC_CHSELR_CHSEL9;  // Canal 0
		//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
		//ADC->CCR |= ADC_CCR_VREFEN;
		capturaAD();
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;
		// ----------------------Codigo Julio Torres

		asm ("nop");
		//if(adcramh >= 10){//cpw     Y,#10
		//if(adcramh >= 20){//cpw     Y,#10
		//if(adcramh >= 35){// Con el valor de 35 funciona bien la deteccion del theshold
		if(adcramh >= 35){// Con el valor de 35 funciona bien la deteccion del theshold

			goto pin_cruce_alto;//jruge   pin_cruce_alto
		}
		s_reloj[0] = 0;		// bres    s_reloj,#0;
		goto end_up_actual;//jra     end_up_actual;
pin_cruce_alto:
		s_reloj[0] = 1;	 		//bset    s_reloj,#0;
end_up_actual:


determina_flanco: //---------------------------------------------------------?
		//ld A,s_reloj
		//and A,#$03
		//-----if(!(s_reloj[0] | s_reloj[1])){// if((s_reloj & 0x03) == 0x01){	//cp  A,#$01 **************????
        if((!s_reloj[1]) & (s_reloj[0]) ){// if((s_reloj & 0x03) == 0x01){	//cp  A,#$01 **************????

			goto flanco_subida;			//jreq flanco_subida/ salta si z=1
		}
		goto fin_detecta_flanco;//jra fin_detecta_flanco


flanco_subida:
		s_reloj[2] = 1;	//bset    s_reloj,#2								;[b_flancos]	;Activar bandera de flanco de subida
		//bcpl    PA_ODR,#0					// Test por, prueba
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);	 //28-May-2024: Salida IO7 toogle test
		cruze_por_cero[0] = 1;			//bset    cruze_por_cero,#0			;Activa bandera de cruce por cero

fin_detecta_flanco:

//;=====================================  FIN Rutina DETECCIÓN DE FLANCO DE BAJADA

		if(s_reloj[2]){			//btjt    s_reloj,#2,pregunta_edo_muestreo
			goto pregunta_edo_muestreo;
		}
		//ld    A,edo_muestreo
		if(edo_muestreo == 1){//cp    A,#1 ***************???
			goto muestrea; //jreq  muestrea
		}
		goto fin_voltrms;//jp fin_voltrms


pregunta_edo_muestreo:

		//ld A,cnt_mues
		if(cnt_mues == 0){//cp A,#0
			goto no_error_muestreo;//jreq no_error_muestreo
		}
		goto error_muestreo;//jp error_muestreo


no_error_muestreo:
		edo_muestreo = 1; //mov edo_muestreo,#1


muestrea:
	    //ld A,cnt_mues
		if(cnt_mues != 32 ){ //cp A,#32 ****************
			goto adq_muesn; //jrne    adq_muesn
		}
		cnt_mues = 0;//clr     cnt_mues         ;SI, limpia el contador de muestras
		edo_muestreo = 0;//mov     edo_muestreo,#0
		// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);	 //28-May-2024: Salida IO7 toogle test
		goto voltaje_rms;//jp      voltaje_rms

adq_muesn:
		//mov ADC1_CR1,#%00100011
		//mov ADC1_SQR1,#%10000000
		//mov ADC1_SQR2,#%00000000
		//mov ADC1_SQR3,#%00000010
		//mov ADC1_SQR4,#%00000000
		//CALL capturaAD
// 19-AGO-2024		ADC_Select_CH5 ();
// 19-AGO-2024				HAL_ADC_Start(&hadc); // start the adc
// 19-AGO-2024		HAL_ADC_PollForConversion(&hadc, 1000); // poll for conversion
// 19-AGO-2024				adcram = HAL_ADC_GetValue(&hadc); // get the adc value
// 19-AGO-2024				HAL_ADC_Stop(&hadc);
		// ----------------------Codigo Julio Torres
//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
		//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
//		ADC1->CHSELR = ADC_CHSELR_CHSEL5;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL10;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL0;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL18;
		ADC1->CHSELR &= ADC_CHSELR_CHSEL14;  // Canal 0

		ADC1->CHSELR |= ADC_CHSELR_CHSEL9;  // Canal 0
		//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
		//ADC->CCR |= ADC_CCR_VREFEN;
		capturaAD();
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;
		// ----------------------Codigo Julio Torres
		asm ("nop");


		adcramh >>= 1;    			//srlw    Y             ;Convierte la muestra de 10 bits en 9 bits ****************?????
		adcramh >>= 1; 				//srlw    Y             ;Convierte la muestra de  9 bits en 8 bits ****************???

		//ldw     X,#vl_ram     ;Apunta a la RAM correspondiente de la muestra de Voltaje de Línea
		//ld      A,XL             ;/
		//add     A,cnt_mues       ;/
		//ld      XL,A
		//ld      (X),A         ;/
		vl_ram [cnt_mues] = (uint8_t) adcramh;
		cnt_mues++;			//inc     cnt_mues			;Una muestra más

		// HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);	 //28-May-2024: Salida IO7 toogle test

termina_muestreo:
		goto fin_voltrms;//jp      fin_voltrms

//;===============================================================================
//;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Proceso VRMS
//;Cada muestra de la pila generada con las 32 muestras es multiplicacda por un
//;factor que relaciona el voltaje medido con el voltaje presente en la entrada,
//;posteriormente cada muestra multiplicada por el factor es elevada al cuadrado
//;y es sumada a una varibale que acumula el resultado de la sumatoria para
//;promediarlo posteriormente. Finalmente se calcula la raíz cuadrada del
//;resultado del promedio y se transmite por la UART.


voltaje_rms:
		//sigma_cuad_sampl_1 = 0;			//clr     sigma_cuad_sampl_1
		//sigma_cuad_sampl_2 = 0;			//clr     sigma_cuad_sampl_2
		//sigma_cuad_sampl_3 = 0;			//clr     sigma_cuad_sampl_3
		//sigma_cuad_sampl_4 = 0;			//clr     sigma_cuad_sampl_4
		sigma_cuad_sampl = 0;

suma_cuad:
				// waux = 0; //clr waux
				// ldw X,#vl_ram
				// ld A,XL
				// add A,cnt_mues
				// ld  XL,A
				// ld A,(X)  ////////////?????
				wreg= vl_ram[cnt_mues]; //ld wreg,A
//;=====================================================  Rutina CALCULA SUMATORIA
//;Llega una muetra de 8 bytes en waux:wreg, la muestra es multiplciacda por un
//;factor que relaciona el divisor de voltaje y la resolución del ADC.
//;La muestra se eleva al cuadrado y se acumula en las variables par ala suma de
//;4 bytes.
take_sampling:
//----------------------- Factor
					//ldw     X,waux
					//ld      A,eevolt_mul
	                //eevolt_mul = waux * eevolt_mul;//mul     X,A ***************************????
					uint16_t	foo = 0;
					//foo = (uint16_t)(wreg * eevolt_mul);
					foo = (uint16_t)(wreg * reevolt_mul);
					//resul = (uint16_t)(wreg * 100);
					//ld      A,eevolt_div
					//div     X,A
	                //eevolt_div = waux / eevolt_div; //************************************?????
					//foo = foo / eevolt_div;
					foo = foo / reevolt_div;
					//resul = resul / 64;
					//waux = eevolt_div; //ldw waux,X
					//resulh = waux;//ldw resulh,X

//------------------------Cuad-----------------------------
			   	//level_1st_mult_H  primer byte
	                //ld      A,wreg
	                //ld      XL,A
					//ld      A,resull
					//mul     X,A
					//resull = wreg * resull;
	            	//ldw     level_1st_mult_H,X
	                //level_1st_mult_H = resull;


	            //;level_2st_mult_H  segundo byte
					//ld      A,waux
					//ld      XL,A
					//ld      A,resull
					//mul     X,A
	                // resull = waux * resull;
	                //level_2st_mult_H = resull; //ldw     level_2st_mult_H,X


	            //;level_3st_mult_H  segundo byte
					//ld      A,wreg
					//ld      XL,A
	                //ld      A,resulh
	                //resulh = wreg * resulh; //mul     X,A -------------------------------?
	                //level_3st_mult_H = resulh; //ldw     level_3st_mult_H,X


	             //level_4st_mult_H  segundo byte
					//ld      A,waux
					//ld      XL,A
					//ld      A,resulh //----------------------------------------------?
					//mul     X,A
	                //resulh = waux*resulh;
	                //level_4st_mult_H = resulh;//ldw level_4st_mult_H,X
					level_4st_mult = (uint32_t)(foo * foo);
	 //----------------------------------------------------------------------------------------------????
	               //level_1st_result_L = level_1st_mult_L;//mov level_1st_result_L, level_1st_mult_L -----------?
	                //ld A,level_1st_mult_H
	               //level_2st_mult_L = level_1st_mult_H + level_2st_mult_L;//add A,level_2st_mult_L
	               // wreg = 0;//clr     wreg
	               // goto carry_1;//jrnc    carry_1 --------------------?
	               // wreg++; //inc  wreg


//carry_1:

	             //add A,level_3st_mult_L
	             //ld level_1st_result_H,A
				 //level_1st_result_H = level_1st_mult_H + level_3st_mult_L;
	             //goto carry_2;//jrnc carry_2
	            // wreg++;//inc wreg
	             //
//carry_2:
	          //ld A,wreg
	          //add	A,level_2st_mult_H
	         // level_2st_mult_H = wreg + level_2st_mult_H;
	          //wreg = 0;//clr wreg
	         // goto carry_3;//jrnc    carry_3
	         // wreg++;//inc     wreg

//carry_3:
	         //level_3st_mult_H = level_2st_mult_H + level_3st_mult_H;//add A,level_3st_mult_H -----
	         //goto carry_4;//jrnc carry_4
	         //wreg++;//inc wreg

//carry_4:
	          //add A,level_4st_mult_L
	         //;;;ld level_2st_result_L,A
	         //level_2st_result_L = level_3st_mult_H + level_4st_mult_L;
	         //goto carry_5;//jrnc carry_5
	         //wreg++; //inc wreg
//carry_5:
	          //ld A,wreg
	          //add A,level_4st_mult_H
	          //ld level_2st_result_H,A
	          //level_2st_result_H = wreg + level_4st_mult_H;

	//------------------------sigma----------------------------------------------------------------------------------

	          //ldw	X,level_1st_result_H
	          //addw X,sigma_cuad_sampl_2
	          //ldw  sigma_cuad_sampl_2,X
	          //sigma_cuad_sampl_2 = level_1st_result_H + sigma_cuad_sampl_2;
	          //waux = 0;//clr waux
	          //wreg = 0;//clr wreg
	          //goto not_inc_byte3_2;//jrnc not_inc_byte3_2
	         // wreg++;//inc  wreg

not_inc_byte3_2:
			//ldw X,waux
			//addw X,level_2st_result_H
			//level_2st_result_H = waux + level_2st_result_H;
			//addw X,sigma_cuad_sampl_4
			//ldw sigma_cuad_sampl_4,X
			//sigma_cuad_sampl_4 = waux + sigma_cuad_sampl_4;
              sigma_cuad_sampl +=  level_4st_mult;

	//================================================= FIN Rutina CALCULA SUMATORIA

			cnt_mues++;//inc cnt_mues
			//ld      A,cnt_mues
			if(cnt_mues >= 32){//cp A,#32 -------------------------?
				goto sumatoria_completa;//jreq sumatoria_completa
			}
			goto suma_cuad;//jp suma_cuad

sumatoria_completa:
			cnt_mues = 0;//clr cnt_mues

//;-------------------------------------------------------------------------------
//;ldw    Y,sigma_prom_h   ;Pregunta si la suma acumulada es mayor de 254
//;cpw    Y,#64516
//;jruge  mayor_255
//;Sumatoria para 254 Volts = 2,064,512 [1F8080]


			//ld A,sigma_cuad_sampl_3
			//if(sigma_cuad_sampl_3 <= 0x1F){//cp A,#$1F
			//	goto segundo_dato;//jrule segundo_dato
			//}
			//goto mayor_254;//jp mayor_254
segundo_dato:
			//ld A,sigma_cuad_sampl_3
			//if(sigma_cuad_sampl_3 == 0x1F){//cp A,#$1F
			//	goto segundo_dato2;//jreq segundo_dato2
			//}
			//goto dato_rango;//jp dato_rango

segundo_dato2:
			//ld      A,sigma_cuad_sampl_2
			//if(sigma_cuad_sampl_2  < 0x80){//cp      A,#$80
			//	goto dato_rango;//jrult   dato_rango
			//}
			//jp     mayor_254
mayor_254:
			//ldw    X,#65025          ;255 elevado al cuadrado
			//sigma_prom_h = 65025;//ldw     sigma_prom_h,X -------------------------------????
			//goto fin_voltrms; //jp   fin_voltrms


dato_rango:

prom_32mues:
	        //A = 5;//ld A,#5
taking_prom:
			// sigma_cuad_sampl_4 = sigma_cuad_sampl_4 >> 1;//srl sigma_cuad_sampl_4
			// sigma_cuad_sampl_3 = (sigma_cuad_sampl_3 >> 1) | (sigma_cuad_sampl_3 & 1) << 7;//rrc sigma_cuad_sampl_3
			// sigma_cuad_sampl_2 = (sigma_cuad_sampl_2 >> 1) | (sigma_cuad_sampl_2 & 1) << 7;//rrc sigma_cuad_sampl_2
			// sigma_cuad_sampl_1 = (sigma_cuad_sampl_1 >> 1) | (sigma_cuad_sampl_1 & 1) << 7;//rcc sigma_cuad_sampl_1
			//   A--;//dec  A
			//	   if(A != 0){//tnz A
			//		   goto taking_prom;//jrne taking_prom
			//	   }
				   //ldw X,sigma_cuad_sampl_2
				   //ldw	sigma_prom_h,X
			//	   sigma_prom_h = sigma_cuad_sampl_2;
			sigma_cuad_sampl  >>= 5;


	//=========================================================  Rutina RAIZ CUADRADA
raiz:
root_square:
_raiz:

		//OFST_1; //OFST_1: equ 4 -----------------------?  ************************

	                  //pushw x
	                  //subw	sp,#4

					  //ldw	(OFST_1-1,sp),x
					  //clrw	x
					  //ldw	(OFST_1-3,sp),x
					 //jra	raiz_2




	//raiz_1: //-------------------------------??

	                 //ldw x, (OFST_1-1,sp)
					 //ldw (OFST_1-3,sp),x
				//FST_1 - 1  <= OFST_1;


					 //ldw x, (OFST_1+1,sp)
	                 //ldw y (OFST_1-1,sp)

	                 //divw  x,y;
	                 //addw x,(OFST_1-1,sp)
					 //srlw x
					 //ldw (OFST_1-1,sp),x




	//raiz_2:
	//                ldw x,(OFST_1-3, sp)
	//				cpw	x,(OFST_1-1,sp)
	//				jreq square ready
	//
	//				incw x
	//				cpw x,(OFST_1-1,sp)
	//				jrne raiz_1


square_ready:
	//              ldw x,(OFST_1-1, sp)
	//				addw sp,#6

					//ld A, XL
					//ld volt_trms, A
					//volt_trms = sigma_prom_h;
					volt_trms = (uint8_t)(sqrt(sigma_cuad_sampl));
					cnt_veces_muestreo = 0;//clr cnt_veces_muestreo

					flagsLogger2[0] = 0;//bres flagsLogger2, #0
					flagsLogger2[1] = 0;//bres flagsLogger2, #1
					flagsLogger2[2] = 0;//BRES flagsLogger2, #2

					// SAlto por lo mientras
					goto fin_calibra_voltaje;
					// SAlto por lo mientras

//;-------------------- Falta programa esta parte ---------------------------------------
calibracion_voltaje:
	        //ld A,eef_voltaje
			asm ("nop");
			//if(eef_voltaje != 0x3C ){//cp A, #$3c					¿Esta calibrado en voltaje el equipo?
			if(reef_voltaje != 0x3C){
				goto realiza_cal_volt; //jrne realiza_cal_volt
			}
			goto fin_calibra_voltaje;

realiza_cal_volt:

//;RM_20230908 Mejoras para calibración de voltaje, espera 2 segundos antes de calibrar
			muestras_cal_volt++; //inc     muestras_cal_volt
			if(muestras_cal_volt >= 60){//cp  A,#60
				goto espera_iguales;//jruge   espera_iguales
			}
			goto fin_calibra_voltaje;// jp fin_calibra_voltaje

espera_iguales:

			if(muestras_cal_volt != 60) {//  cp  A,#60
				goto segunda_muestra;//	jrne   segunda_muestra
			}
			voltaje_ant_cal = volt_trms;//	mov voltaje_ant_cal,volt_trms
			goto fin_calibra_voltaje;//jp  fin_calibra_voltaje

segunda_muestra:

			if(muestras_cal_volt != 61){//cp      A,#61
				goto tercera_muestra;////jrne    tercera_muestra
			}
			//	  ld      A,volt_trms
			if(volt_trms != voltaje_ant_cal){//cp  A,voltaje_ant_cal
				//**********************************************
				goto fin_calibra_voltaje;// MAN DEBUG
				//**********************************************
				goto calibra_reset;//jrne  calibra_reset

			}
			goto fin_calibra_voltaje;// jp fin_calibra_voltaje

tercera_muestra:
			//	ld      A,volt_trms
			if(volt_trms == voltaje_ant_cal){//cp  A,voltaje_ant_cal
				goto calibracion_valida;//jreq calibracion_valida
			}
			//**********************************************
			goto fin_calibra_voltaje;// MAN DEBUG
			//**********************************************
			goto calibra_reset;//jp calibra_reset


calibracion_valida:

			//  clrw    X  ------------------------??
			//ld      A,volt_trms
			//ld      XL,A

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
			if(volt_trms > 80 ){//cpw     X,waux, x  ;Mayor o igual al umbral alto, resistores mal soldados o inadecuados
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
	        goto fin_voltrms;// jp fin_voltrms
error_muestreo:
			cnt_mues = 0;//clr cnt_mues
			cnt_veces_muestreo = 0;//clr cnt_veces_muestreo
			edo_muestreo = 0;//mov edo_muestreo,#0
fin_voltrms:


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

