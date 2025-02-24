
#include "main.h"
#include "board_PFULLDEF.h"
#include "customMain.h"
#include "bluetooth.h"
//#include "math.h"

//uint16_t    tsac_w = 0;
//tret_w
//teval
//tevaf
//tdevl
//tdevf


uint16_t	tsacram [8] = {0};
uint16_t    tretram [8] = {0};
uint16_t    tambram [8] = {0};
uint16_t    tevaram [8] = {0};


uint16_t   curva = 0;
uint8_t    A = 0;
uint16_t   Y = 0;
uint16_t   X = 0;
//uint8_t    wreg = 0;
//uint8_t    waux = 0;
uint16_t   resul =  0;
uint16_t   tempo = 0;
uint16_t   raux = 0;
uint16_t   promant = 0;
uint16_t   ultimoprm = 0;
//uint16_t   tdev = 0;
//uint16_t   tDisplay_w = 0;
//uint16_t   tdevdpy_w = 0;
//aux
//uint16_t  limsup_w = 0;
//aux
//	uint8_t retcncfa = 0;
//	uint8_t retcncfe = 0;
//	uint8_t retcncfr = 0;
//	uint8_t retcncfc = 0;
//  uint8_t cntsame = 0;
//  uint8_t ret_up = 0;

//aux
//uint8_t trefst_aux = 0;
//uint8_t trefst2_aux = 0;
//aux
//////////////////////////////////////////////////////////////////////////////////////////////////7
//uint16_t adcram = 0;
//*************************************************************************************************
void temper(void){


	cnttemp++;
	A = cnttemp;

	if(A >= 50) 	   //Manuel 07-Mar-2022	$64;		/ cntemp es menor
		goto tempe05;  // Sí no tomes muestras
	goto fintemp;      // No, sincroniza la lectura con el dígito 1

tempe05:
		decwreg(&retcncfa); // Decrementa tiempo para liberar fallas
		decwreg(&retcncfe);
		decwreg(&retcncfr);
		decwreg(&retcncfc);
//temper_j00:
		cnttemp = 0;   	   // Inicia el contador de 100 ms


//-------------------------------------------------------------------------------------------------
		//-------------Cuarto sensor de temperatura--------------------------------------------------------
		//ADC Temperatura Ambiental o Condensador (temperaturas hasta 50ºC)

				//ADC_ChannelConfTypeDef sConfig = {0};
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
				//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
				//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
				//ADC1->CHSELR = ADC_CHSELR_CHSEL0;
				//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
				//ADC->CCR |= ADC_CCR_VREFEN;
//				capturaAD();
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
				//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;
				/*hadc.Instance->CHSELR = (uint32_t) 0;
				sConfig.Channel = ADC_CHANNEL_10;
				sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
				HAL_ADC_ConfigChannel(&hadc, &sConfig);
				adcram = capturaAD();  //Convierte la señal*/
								        // Habilitar el canal 0 en la secuencia de conversión usando la directiva definida
				        ADC1->CHSELR &= ADC_CHSELR_CHSEL14;
				        ADC1->CHSELR &= ADC_CHSELR_CHSEL10;
				        ADC1->CHSELR &= ADC_CHSELR_CHSEL18;

				        ADC1->CHSELR |= ADC_CHSELR_CHSEL0;  // Canal 0

				        capturaAD ();
//lookshort_S4:
		Y_A();
		//Y = adcramh;
		//A = edorefri;

		if(A == 0x01)
			goto cmp_man_01_s04;        // En autoprueba deja estos límites
		if(Y < 942)						// Límite superior 942 (4.6v (rt = 27.4k rs = 55))
			goto lookopen_S04;          // No, revise límite inferior
		goto sens_short_S04;

cmp_man_01_s04:
		if(Y < 0x01C4)                  // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
			goto lookopen_S04;          // No, revise límite inferior

sens_short_S04:
		Bset_Clear_trfst(&trefst[0], &trefst2[0],5, 7);
//		BitSet(trefst,5);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
		//BitSet(trefst_aux,5);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
//		BitClear(trefst2,7); //#f_s4open   // Cancela la otras falla del sensor de salida
		//BitClear(trefst2_aux,7); //#f_s4open   // Cancela la otras falla del sensor de salida

		goto loadret_S04;  /// Carga el retardo de cancelación

lookopen_S04:
		A = edorefri;
		if(A == 1)
			goto cmp_man_02_S04;  // En autoprueba deja estos límites
		if(Y >= 0x002B)           // Ajuste en comportamiento de la Falla del sensor evaporador IJG 30/07/14
			goto sens_ok_S04;     // Sí, indica sensor OK
		goto sens_open_S04;

cmp_man_02_S04:
		if(Y >= 0x01AE)           // Límite inferior 0x1B4 /  con tolerancia del 1% en el
			goto sens_ok_S04;     // Sí, indica sensor OK

sens_open_S04:
		Bset_Clear_trfst(&trefst2[0], &trefst[0],7, 5);
//		BitSet(trefst2,7);  //#f_s4open   // Indica la falla (Sensor de salida abierto)
		//BitSet(trefst2_aux,7);  //#f_s4open   // Indica la falla (Sensor de salida abierto)
//		BitClear(trefst,5); //#f_s4short  // Cancela la otras falla del sensor de salida
		//BitClear(trefst_aux,5); //#f_s4short  // Cancela la otras falla del sensor de salida

loadret_S04:
		retcncfc = 0x0F;    // Carga retardo de cancelación de falla; 1.5 segundos
		goto tempeLoad_s04;

sens_ok_S04:
		A = retcncfc;
		if(A != 0)         // Ya se agotó el retardo?
			goto tempeLoad_s04;
		Bclear_Clear_trfst(&trefst[0], &trefst2[0],5, 7);
//		BitClear(trefst,5);  //#f_s4short  // Cancela banderas de falla de sensor de salida
		//BitClear(trefst_aux,5);  //#f_s4short  // Cancela banderas de falla de sensor de salida
//		BitClear(trefst2,7);  //#f_s4open  // Cancela banderas de falla de sensor de salida
		//BitClear(trefst2_aux,7);  //#f_s4open  // Cancela banderas de falla de sensor de salida

tempeLoad_s04:
		tsacram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra

//-------------------------------------------------------------------------------------------------
//-------------Tercer sensor de temperatura--------------------------------------------------------

											//ADC Temperatura Salida de aire
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
			//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
//			ADC1->CHSELR = ADC_CHSELR_CHSEL18;
			//f ((ADC1->CR & ADC_CR_ADSTART) == 0)
			//{
		    // Verificar si el modo de secuencia es fijo o en reversa
				//if ((ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED ||
						//(ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED_BACKWARD)
				//{
		        // Habilitar el canal 0 en la secuencia de conversión usando la directiva definida
			        ADC1->CHSELR &= ADC_CHSELR_CHSEL10;
			        ADC1->CHSELR &= ADC_CHSELR_CHSEL0;
			        ADC1->CHSELR &= ADC_CHSELR_CHSEL18;

			        ADC1->CHSELR |= ADC_CHSELR_CHSEL18;  // Canal 0
					capturaAD();
				//}

		    // Limpiar los bits del canal 0 en el registro SMPR para asegurar configuración correcta
	//	    ADC1->SMPR &= ~ADC_SMPR_SMPSEL0;  // Limpiar los bits del canal 0
		    // Establecer el tiempo de muestreo para el canal 0 (equivalente a 1.5 ciclos)
	//	    ADC1->SMPR |= (ADC_SMPR_SMPSEL0_0 | ADC_SMPR_SMPSEL0_1);  // 1.5 ciclos de muestreo para el canal 0
			//}

			//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
//			capturaAD();
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;
			/*hadc.Instance->CHSELR = (uint32_t) 0;
			sConfig.Channel = ADC_CHANNEL_9; ///////////////////////////////////////////Cambiar canales
			sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
			HAL_ADC_ConfigChannel(&hadc, &sConfig);
			adcram = capturaAD();  // Convierte la señal*/


	Y_A();
	//Y = adcramh;
	//A = edorefri;

	if(A == 1)
		goto cmp_man_01_s03;    // En autoprueba deja estos límites
	if(Y < 950)                 // Límite superior 942 (4.6v (rt = 27.4k rs = 55))
		goto lookopen_S03;      // No, revise límite inferior
	goto sens_short_S03;

cmp_man_01_s03:
		if(Y < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
			goto lookopen_S03; // No, revise límite inferior

sens_short_S03:
		Bset_Clear_trfst(&trefst2[0], &trefst2[0],4, 5);
//		BitSet(trefst2,4);     // Indica la falla (Sensor de salida en corto)
		//BitSet(trefst2_aux,4);     // Indica la falla (Sensor de salida en corto)
//		BitClear(trefst2,5);   // Cancela la otras falla del sensor de salida
		//BitClear(trefst2_aux,5);   // Cancela la otras falla del sensor de salida
		goto loadret_S03;      // Carga el retardo de cancelación

lookopen_S03:
		A = edorefri;
		if(A == 1)
			goto cmp_man_02_S03;   // En autoprueba deja estos límites

		if(Y >= 0x002B)            // Ajuste en comportamiento de la Falla del sensor evaporador
			goto sens_ok_S03;      // Sí, indica sensor OK
		goto sens_open_S03;

cmp_man_02_S03:
		if(Y >= 0x01AE)            // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
			goto sens_ok_S03;      // Sí, indica sensor OK

sens_open_S03:
		Bset_Clear_trfst(&trefst2[0], &trefst2[0],5, 4);
//		BitSet(trefst2,5);         // Indica la falla (Sensor de salida abierto)
		//BitSet(trefst2_aux,5);         // Indica la falla (Sensor de salida abierto)
//		BitClear(trefst2,4);       // Cancela la otras falla del sensor de salida
		//BitClear(trefst2_aux,4);       // Cancela la otras falla del sensor de salida

loadret_S03:
		retcncfr = 0x0F;           // Carga retardo de cancelación de falla; 1.5 segundos
		goto tempeLoad_s03;

sens_ok_S03:
		A = retcncfr;
		if(A != 0)                 // Ya se agotó el retardo?
			goto tempeLoad_s03;
		Bclear_Clear_trfst(&trefst2[0], &trefst2[0],4, 5);
//		BitClear(trefst2,4);       // Cancela banderas de falla de sensor de salida
		//BitClear(trefst2_aux,4);       // Cancela banderas de falla de sensor de salida
//		BitClear(trefst2,5);	   // Cancela banderas de falla de sensor de salida
		//BitClear(trefst2_aux,5);	   // Cancela banderas de falla de sensor de salida

tempeLoad_s03:
		tretram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra
//-------------------------------------------------------------------------------------------------
									//ADC Temperatura Enfriador
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
		//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
//		ADC1->CHSELR = ADC_CHSELR_CHSEL10;
//		if ((ADC1->CR & ADC_CR_ADSTART) == 0)
//		{
//		    // Verificar si el modo de secuencia es fijo o en reversa
		    //if ((ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED ||
		        //(ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED_BACKWARD)
		    //{
		        // Habilitar el canal 0 en la secuencia de conversión usando la directiva definida

		        ADC1->CHSELR &= ADC_CHSELR_CHSEL14;
		        ADC1->CHSELR &= ADC_CHSELR_CHSEL0;
		        ADC1->CHSELR &= ADC_CHSELR_CHSEL18;

		        ADC1->CHSELR |= ADC_CHSELR_CHSEL10;  // Canal 0
		        capturaAD ();
		    //}

		    // Limpiar los bits del canal 0 en el registro SMPR para asegurar configuración correcta
	//	    ADC1->SMPR &= ~ADC_SMPR_SMPSEL0;  // Limpiar los bits del canal 0
		    // Establecer el tiempo de muestreo para el canal 0 (equivalente a 1.5 ciclos)
	//	    ADC1->SMPR |= (ADC_SMPR_SMPSEL0_0 | ADC_SMPR_SMPSEL0_1);  // 1.5 ciclos de muestreo para el canal 0
		//}
		//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
//		capturaAD();
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;

		/*hadc.Instance->CHSELR = (uint32_t) 0;
		sConfig.Channel = ADC_CHANNEL_6; ///////////////////////////////////////////Cambiar canales
		sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
		HAL_ADC_ConfigChannel(&hadc, &sConfig);
		adcram = capturaAD(); // Convierte la señal*/


		Y_A();
		//Y = adcramh;
		//A = edorefri;

		if(A == 1)
			goto cmp_man_01;   // En autoprueba deja estos límites
		if(Y < 950)            // Límite superior 942 (4.6v (rt = 27.4k rs = 55))
			goto lookopen;     // No, revise límite inferior
		goto sens_short;

cmp_man_01:
		if(Y < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
			goto lookopen;     // No, revise límite inferior

sens_short:
		Bset_Clear_trfst(&trefst[0], &trefst[0],0, 1);

//		BitSet(trefst,0);       //0x01;	/ Indica la falla (Sensor de ambiente en corto)
		//BitSet(trefst_aux,0);       //0x01;	/ Indica la falla (Sensor de ambiente en corto)
//		BitClear(trefst,1);     //0x02;	/ Cancela la otras falla del sensor de ambiente interno
		//BitClear(trefst_aux,1);     //0x02;	/ Cancela la otras falla del sensor de ambiente interno
		goto loadret;           // Carga el retardo de cancelación

lookopen:
		A = edorefri;
		if(A == 1)
			goto cmp_man_02;    // En autoprueba deja estos límites
		if(Y >= 0x002B)         // Ajuste en comportamiento de la Falla del sensor evaporador IJG 30/07/14
			goto sens_ok;       // Sí, indica sensor OK
		goto sens_open;

cmp_man_02:
		if(Y >= 0x01AE)         // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
			goto sens_ok;       // Sí, indica sensor OK

sens_open:
		Bset_Clear_trfst(&trefst[0], &trefst[0],1, 0);
//		BitSet(trefst,1);      //0x02;	/ Indica la falla (Sensor de ambiente abierto)
		//BitSet(trefst_aux,1);      //0x02;	/ Indica la falla (Sensor de ambiente abierto)
//		BitClear(trefst,0);    //0x01;	/ Cancela la otras falla del sensor de ambiente interno
		//BitClear(trefst_aux,0);    //0x01;	/ Cancela la otras falla del sensor de ambiente interno

loadret:
		retcncfa = 0x0F;   	   // Carga retardo de cancelación de falla; 1.5 segundos
		cnthold = 0;           // Cancela el retardo de retención de display
		goto tempe13;

sens_ok:
		A = retcncfa;
		if(A != 0)            // Ya se agotó el retardo?
			goto tempe13;

		//BitClear(trefst_aux,0);   // Cancela banderas de falla de sensor de ambiente
		Bclear_Clear_trfst(&trefst[0], &trefst[0],0, 1);
//		BitClear(trefst,0);   // Cancela banderas de falla de sensor de ambiente
		//BitClear(trefst_aux,1);   // Cancela banderas de falla de sensor de ambiente
//		BitClear(trefst,1);   // Cancela banderas de falla de sensor de ambiente

tempe13:
	tambram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra
//-------------------------------------------------------------------------------------------------
	//; ADC Temperatura Deshielo

	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	//---Toogle  GPIOA->BSRR = GPIO_BSRR_BS_11;
	//ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
//	ADC1->CHSELR = ADC_CHSELR_CHSEL14;
	//if ((ADC1->CR & ADC_CR_ADSTART) == 0)
	//{
	    // Verificar si el modo de secuencia es fijo o en reversa
	    //if ((ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED ||
	        //(ADC1->CFGR1 & ADC_CFGR1_SCANDIR) == ADC_SCAN_SEQ_FIXED_BACKWARD)
	   // {
	        // Habilitar el canal 0 en la secuencia de conversión usando la directiva definida
	        ADC1->CHSELR &= ADC_CHSELR_CHSEL10;
	        ADC1->CHSELR &= ADC_CHSELR_CHSEL0;
	        ADC1->CHSELR &= ADC_CHSELR_CHSEL18;

	        ADC1->CHSELR |= ADC_CHSELR_CHSEL14;  // Canal 0
	        capturaAD ();
	    //}

	    // Limpiar los bits del canal 0 en el registro SMPR para asegurar configuración correcta
//	    ADC1->SMPR &= ~ADC_SMPR_SMPSEL0;  // Limpiar los bits del canal 0
	    // Establecer el tiempo de muestreo para el canal 0 (equivalente a 1.5 ciclos)
//	    ADC1->SMPR |= (ADC_SMPR_SMPSEL0_0 | ADC_SMPR_SMPSEL0_1);  // 1.5 ciclos de muestreo para el canal 0
	//}
//	while ((ADC1->ISR & ADC_ISR_CCRDY) == 0)//((ADC1->ISR & ADC_ISR_EOC) == 0)
//	{
//
//	}
	//ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
	//ADC->CCR |= ADC_CCR_VREFEN;
//	capturaAD();
	//---Toogle  GPIOA->BSRR = GPIO_BSRR_BR_11;
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	/*hadc.Instance->CHSELR = (uint32_t) 0;
	sConfig.Channel = ADC_CHANNEL_7; ///////////////////////////////////////////Cambiar canales
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
	adcram = capturaAD();  //Convierte la señal*/

	Y_A();
	//Y = adcramh;
	//A = edorefri;

	if(A == 1)
		goto cmp_man_03;   // En autoprueba deja estos límites
	if(Y < 950)            // Límite superior 942 (4.6v (rt = 27.4k rs = 55))
		goto lookopene;    // No, revise límite inferior
	goto senshorte;

cmp_man_03:
	if(Y < 0x01C4)         // Límite superior 0x1BE \  Límites para autoprueba | Para una resistencia de 22 [Kohm]
		goto lookopene;    // No, revise límite inferior

senshorte:
	Bset_Clear_trfst(&trefst[0], &trefst[0],2, 3);

//	BitSet(trefst,2);      //0x04   / Indica sensor del evaporador en corto
	//BitSet(trefst_aux,2);      //0x04   / Indica sensor del evaporador en corto
//	BitClear(trefst,3);     //0x28;	/ Cancela las otras dos fallas del sensor de evaporador
	//BitClear(trefst_aux,3);     //0x28;	/ Cancela las otras dos fallas del sensor de evaporador
	goto loadret2;         // Carga retardo de cancelación de falla

lookopene:
	A = edorefri;
	if(A == 1)
		goto cmp_man_04;   // En autoprueba deja estos límites
	if(Y >= 0x002B)        // Ajuste en comportamiento de la Falla del sensor evaporador IJG 30/07/14
		goto sens_oke;     // Si, entrega el dato medido y cancela las banderas de falla
	goto sens_opene;

cmp_man_04:
	if(Y >= 0x01AE)        // Límite inferior 0x1B4 /  con tolerancia del 1% en el probador.
		goto sens_oke;     // Si, entrega el dato medido y cancela las banderas de falla

sens_opene:
	Bset_Clear_trfst(&trefst[0], &trefst[0],3, 2);
//	BitSet(trefst,3);   //0x08;	/ Indica sensor del evaporador abierto
	//BitSet(trefst_aux,3);   //0x08;	/ Indica sensor del evaporador abierto
//	BitClear(trefst,2); //0x24;	/ Cancela las otras dos fallas del sensor de evaporador
	//BitClear(trefst_aux,2); //0x24;	/ Cancela las otras dos fallas del sensor de evaporador

loadret2:
	retcncfe = 0x14;    // Carga retardo de cancelación de falla; 2 segundos
	goto tempe16;

sens_oke:
	A = retcncfe;
	if(A != 0)          // Ya se agotó el retardo?
		goto tempe16;
	Bclear_Clear_trfst(&trefst[0], &trefst[0],2, 3);
//	BitClear(trefst,2); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador
	//BitClear(trefst_aux,2); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador
//	BitClear(trefst,3); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador
	//BitClear(trefst_aux,3); //0x0C;	/ Cancela indicaciones de falla del sensor del evaporador

tempe16:
	if(trefst[2]) //#f_sdc // El sensor del evaporador esta en corto?
	//if(GetRegFlagState(trefst_aux, 2)) //#f_sdc // El sensor del evaporador esta en corto?
		goto temper_j01;
	goto tempe16a;                 // No, revisa temperatura alta

temper_j01:
		adcramh = 0x000A;           // Carga el dato de temperatura del evaporador con < -50°C
tempe16a:
		tevaram[cntmues] = adcramh;//////////////////////////////////////////////////////Reduccion de obtencion de muestra
//;---------------------------------------------------------------------------------------------
		//; ADC Medición de voltaje de alimentación en 12Vcd

		/*MOV				ADC1_CR1,#%00100011   ; [6:5]10-bit resolution=01, 1:Conversion start=1, 0:A/D converter ON / OFF=1
	MOV				ADC1_SQR1,#%10000000	; 7:DMA Disabled = 1 , [3:0] CHSEL_S[27:24]
	MOV				ADC1_SQR2,#%00000000	; [7:0] CHSEL_S[23:16]
	MOV				ADC1_SQR3,#%00000100	; [7:0] CHSEL_S[15:8]    , PD4/ADC1_IN10: Selecciona (Voltaje 12Vcd)
	MOV				ADC1_SQR4,#%00000000	; [7:0] CHSEL_S[7:0]
	call		capturaAD;		/ Convierte la señal

	ldw			X,#volt_12vcd;
	mov			wreg,cntmues;
	call		cargamues;*/

		cntmues++;         //Incrementa el contador de muestra
		A = cntmues;
		if(A >= 8)         // Ya fueron 8 muestras?
			goto tempe17;  // Si, obten el promedio
		else
			goto fintemp;

tempe17:

//;---------------------------------------------------------------------------------------------
/* BATERIA
              btjt		flagsLogger2,#2,fuente_off;			se detecto falla de AC?
							;No , manten display encendido y funcionamiento normal
							mov     edo_display,#$AA    ;El display debe estar ENCENDIDO
							clr			flagsBattery
							ldw			X,#2160;					/ 36 horas
							;ldw			X,#5
							ldw			timeBatLow,X;			/carga tiempo para indicar bateria baja en minutos
              jra     fin_revisa_v12v
fuente_off:

							ldw			X,#volt_12vcd
							call		prom8mues          ;Calcula el promedio
							ldw			vdc_ADC,Y
							 cpw     Y,#330   ;		/ Equivale a 6.0 Vcd de alimentación
							 jrule   bateria_baja
							;mientras el voltaje sea mayor a 6v con falla de AC inidica que se está alimentando por batería
							btjt		flagsBattery,#batON,bateria_on
							;mov			cntblkh,#$0F
							mov			cntblkh,#$1F
							mov			cntblkl,#$FF
bateria_on:
							 bset		 flagsBattery,#batON
							 bres		 flagsBattery,#batLOW
							 bres		 flagsBattery,#batOFF
							 ;jra     fin_revisa_v12v
							 jra		askTimeBatLow;				siempre pregunta por condición de bateria baja
bateria_baja:
							 cpw     Y,#300   ;		/ Equivale a 5.5 Vcd de alimentación
							 jrule   bateria_descargada; Voltaje menos a  5.5 volts se considera batería descargada
askTimeBatLow:
							 ldw		X,timeBatLow
							 tnzw		X
							 jrne		fin_revisa_v12v
							 bset		 flagsBattery,#batLOW
							 bres		 flagsBattery,#batOFF
							 jra     fin_revisa_v12v

bateria_descargada:
							 bset		 flagsBattery,#batOFF
							 ;mov     edo_display,#$00    ;El display debe estar APAGADO

fin_revisa_v12v:
*/  resul = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul = resul + tsacram[i];    // Calcula el promedio
	}
	resul = resul / 8;
	linealiza();
	X = tempo;
	tsac_w = X;        //Entrega el dato de temperatura de salida linealizada del sensor 4 ambiente o condensador


	resul = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul = resul + tretram[i];   // Calcula el promedio
	}
	resul = resul / 8;
	linealiza();
	X = tempo;
	tret_w = X;    /// Entrega el dato de temperatura de salida linealizada


	resul = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul = resul + tevaram[i];      // Calcula el promedio
	}
	resul = resul / 8;
	linealiza();
	tevaf = lowByte(tempo);      // Entrega el dato de temperatura del evaporador a 10 bits
	teval = highByte(tempo);

	resul = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul = resul + tambram[i];    // Calcula el promedio
	}
	resul = resul / 8;

	Y = promant;                  // Toma el valor anterior del promedio
	Y = Y - resul;			      // w = promant - Resul
	if (Y == 0)
		goto tempe27;                //nota1: checar comparaciones
	else if ((GetRegFlagState(Y, 15)) == 0)
		goto tempe25b;            //La diferencia de promedios es positiva?

	Y = Y - 0xFFFF;               // La diferencia es igual a -1?
	if (Y == 0)
		goto vesiestab;  		  // Si, revisa estabilidad
	Y = resul;
	Y = Y - 0x0001;              // Decrementa el promedio actual
	resul = Y;
	goto tempe26;

tempe25b:
		Y = Y - 0x0001;           // La diferencia es igual a 1?
		if (Y == 0)
			goto vesiestab;       // Si, revisa estabilidad
		Y = resul;
		Y = Y + 0x0001;          // Decrementa el promedio actual
		resul = Y;
		goto tempe26;

vesiestab:
		Y = ultimoprm;     // Toma el valor del último promedio
		Y = Y - resul;     //w = prominter - Resul
		if (Y == 0)
			goto estable;  // El nuevo dato es igual a último?
		cntsame = 0;       // No, inicia el contador de promedios iguales
		goto noestab;

estable:
		cntsame++;         // Si, incrementa el contador

noestab:
		Y = resul;
		ultimoprm = Y;     // Almacena el último dato para la siguiente comparación
		A = cntsame;
		if (A < 0x0F) // Ya esta estable por lo menos por 12 segundos?
			goto tempe29;

tempe26:
		X = resul;          // Almacena el dato anteriorpara la siguiente comparación
		promant = X;
		linealiza();
		tdevf = lowByte(tempo);  // Entrega el dato de temperatura del ambiente en °C con fracción
		tdevl = highByte(tempo);

tempe27:
		ultimoprm = promant;  // Actualiza el dato del último promedio
		cntsame = 0;          // Inicia el contador de promedios iguales hacia arriba

tempe29:
		X = tdev_to_Word();	// tdev;
		tDisplay_w = X;

		A = Plantilla [numSens];
		if (A < 3)
			goto tempe26a;   // si el numero de sensores no es tres, siempre muestra en display sensor 1
		if(!GetRegFlagState(Plantilla [logicos], 7))// if(GetRegFlagState(Plantilla [logicos], 7) == 0) // Sí está seleccionada la función visualiza sensor 3 en display
			goto tempe26a;
		if(trefst2[4]) //#f_s3short
		//if(GetRegFlagState(trefst2_aux, 4)) //#f_s3short
			goto desptdv_failSens3;   // El sensor 3 esta en corto?
		if(!trefst2[5]) //#f_s3open
		//if(GetRegFlagState(trefst2_aux, 5) == 0) //#f_s3open
			goto desptdv_OKSens3;     // No, entonces el sensor 3 esta abierto?
desptdv_failSens3:
		goto tempe26a;                // Si el sensor 3 tiene alguna falla , muestra temperatura de sensor 1
desptdv_OKSens3:
		X = tret_w;
		tDisplay_w = X;

tempe26a:
		if (retcncfa == 0x00)          // Ya llegó a cero el retardo
			goto tempe30;              // Si
		else if(retcncfa < 0x09)       // Ya se obtuvo un promedio correcto?
			goto tempe39;              // Si, cargalo al display
		X = 0xFE34;
		// tdev = X;
		tdevl = highByte(X);
		tdevf = lowByte(X);
		goto tempe40;


tempe30:
		if(!flagsa[0]) //#arran   // Estas en período de arranque?     nota 2: Cambiar flagsa a registro
			goto temper_j02;      // Si, toma el dato de temperatura medido
		goto tempe39;

temper_j02:
		Y = tdevdpy_w;         // manuel_math_change//   tdevdpyl;
		Y = Y - tDisplay_w;    // w = TdeVdpy - TdeV
		if (Y == 0)
			goto tempe40;
		else if (!(GetRegFlagState(Y, 15)))//else if ((GetRegFlagState(Y, 15)) == 0)                               //nota3 checar combinacion con JRSGT
			goto tempe39;      // La diferencia es positiva?

		A =  Plantilla [filtro];
		if(A == 0)
			goto tempe39;
		A = A << 1;
		ret_up++;              // 800 ms más de retardo

		if(A > ret_up)
			goto tempe50;

		Y = (Y ^ 0xFFFF) + 1;
		if (Y < 11)           // Se compara con 0xFE si se quieren dos decrementos abajo
			goto tempe37b;

		X = tDisplay_w;
		X = X - 10;            //w = tdev - 1.0
		//waux = X;
		raux = X;
		goto tempe39a;

tempe37b:
		X = tdevdpy_w;     //manuel_math_change//  tdevdpyf;
		X = X + 1;         //manuel_math_change//  tdevdpyl;
		//waux = X;
		raux = X;
		goto tempe39a;

tempe39:
		X = tDisplay_w;
		//waux = X;
		raux = X;
		cnthold = 0;                  // Cancela el retardo

// manuel_math_change ............................. filtro digital
tempe39a:
		A = cnthold;
		if (A == 0)                   // Ya terminó el tiempo?
			goto tempe39b;            // Si, carga el dato a tdevdpy
		Y = limsup_w;                 // manuel_math_change//   limsupl;/ Compara contra el límite superior para que congele en ese valor
		//Y = Y - waux;
		Y = Y - raux;                 // tdev esta por arriba de limsup?
		//if(Y < 0)   //////////////////JRSLT              nota4: checar combinacion con JRSLT
		if(GetRegFlagState(Y, 15))
			goto tempe40;             // Si, congela el display en limsup
tempe39b:
//tdevdpyl,waux;	/ Almacena el dato anterior para la siguiente comparación
		// X = waux;
		X = raux;
		tdevdpy_w = X;
tempe40:
		ret_up = 0;  // Inicia el retardo hacia arriba
tempe50:
		cntmues = 0;
		if (!trefst[3]) //#f_sda
		//if (GetRegFlagState(trefst_aux, 3) == 0) //#f_sda
			goto tempe52;
		X = 0xFE34;
		tevaf = lowByte(X);      // Entrega el dato de temperatura del evaporador a 10 bits
		teval = highByte(X);

tempe52:
		if (!trefst2[5]) //#f_s3open
		//if (GetRegFlagState(trefst2_aux, 5) == 0) //#f_s3open
			goto tempe53;
		X = 0xFE34;
		tret_w = X;

tempe53:
		if (!trefst2[7]) //#f_s4open
		//if (GetRegFlagState(trefst2_aux, 7) == 0) //#f_s4open
			goto fintemp;
		X = 0xFE34;
		tsac_w = X;

fintemp:

		A = edorefri;

		if (A == 1)
			goto noClrSensFail;  // sí estás en autoprueba permite fallas
		A = Plantilla [numSens];
		if (A == 4)
			goto noClrSensFail;  // sí hay tres sensores no preguntes si hay que borrar fallas
		if (A == 3)
			goto clrSacFail;     // sí hay tres sensores no preguntes si hay que borrar fallas
		if (A == 2)
			goto clrRetFail;
		if (A != 1)
			goto noClrSensFail;  // Si el número de sensores con el que trabaja es 1, borra fallas de evaporador

clrEvaFail:
		Bclear_Clear_trfst(trefst, trefst,2, 3);
//		BitClear(trefst,2);      // Cancela la fallas del sensor de evaporador
		//BitClear(trefst_aux,2);      // Cancela la fallas del sensor de evaporador
//		BitClear(trefst,3);
		//BitClear(trefst_aux,3);

clrRetFail:
		Bclear_Clear_trfst(trefst2, trefst2,4, 5);
//		BitClear(trefst2,4);     // Cancela la fallas del sensor de retorno
		//BitClear(trefst2_aux,4);     // Cancela la fallas del sensor de retorno
//		BitClear(trefst2,5);
		//BitClear(trefst2_aux,5);

clrSacFail:
		Bclear_Clear_trfst(trefst, trefst2,5, 7);
//		BitClear(trefst,5); //#f_s4short   // Cancela la fallas del sensor de retorno
		//BitClear(trefst_aux,5); //#f_s4short   // Cancela la fallas del sensor de retorno
//		BitClear(trefst2,7); //#f_s4open
		//BitClear(trefst2_aux,7); //#f_s4open
noClrSensFail:
		asm ("nop");




}

//*************************************************************************************************

void linealiza (void)
{
	tempo = 0;
	Y = resul;
	curva = Y;                  // Almacena el dato para ajuste de curva

	//  verifica en que rango de la curva cae
	if(curva >= 910)
		goto wise_hot_3;
	else if(curva >= 826)
		goto wise_hot_2;
	else if(curva >= 683)
		goto wise_hot_1;
	else if(curva >= 203)
		goto lil_principal;
	else
		goto curva_fria;


wise_hot_3:

	wise_hot_rep (83, 0xFFFF);
//	wreg = 83;              // Multiplica el dato por el factor
//	Y = curva;
//
//	mult1x2();              // Multiplica
//
//	X = resul;              // En tempo2 quedó el entero del dato de temperatura
//	X = X - 0xFFFF;         // ordenada al origen en °C  (-255.996)
//	tempo = X;
	goto fincurva;

wise_hot_2:
	wise_hot_rep (52, 0x93CC);

//	wreg = 52;              // Multiplica el dato por el factor
//	Y = curva;
//	mult1x2();              // Multiplica
//
//	X = resul;
//	X = X - 0x93CC;         // ordenada al origen en °C  (-147.8)
//	tempo = X;
	goto fincurva;


wise_hot_1:

	wise_hot_rep (29, 0x4980);
//	wreg = 29;              // Multiplica el dato por el factor
//	Y = curva;
//	mult1x2();              // Multiplica
//
//	X = resul;
//	X = X - 0x4980;         // ordenada al origen en °C  (-77.0)
//	tempo = X;
	goto fincurva;

lil_principal:

	wise_hot_rep (19, 0x2ECC);
//	wreg = 19;              // Multiplica el dato por el factor
//	Y = curva;
//	mult1x2();              // Multiplica
//
//	X = resul;
//	X = X - 0x2ECC;         // ordenada al origen en °C  (-46.8)
//	tempo = X;
	goto fincurva;


curva_fria:

	wise_hot_rep (30, 0x3780);

//	wreg = 30;              // Multiplica el dato por el factor
//	Y = curva;
//	mult1x2();              // Multiplica
//
//	X = resul;
//	X = X - 0x3780;         // ordenada al origen en °C  (-55.5)
//	tempo = X;

//goto fincurva;

fincurva:

	X = tempo;
	Y = X;                   //guarda una copia en Y
	if(GetRegFlagState(Y, 15))  //es dato positivo o negativo ?
		X = (X ^ 0xFFFF) + 1;


	A = highByte(X);          //guarda el valor de la parte alta
	waux = A;
	A = 10;

	X = lowByte(X) * A;
	A = highByte(X);
	X = 0;                    //borra la parte baja no interesa ya que son centesimas y milesimas etc.....
	X = A;

	tempo = X;                //esta es la parte de la decena

	//procesa la parte entera
	A = waux;                 //guarda el valor de l parte alta
	X = A;

	A = 10;
	X = lowByte(X) * A;

	X = X + tempo;

	if(GetRegFlagState(Y, 15))
	X = (X ^ 0xFFFF) + 1;

	tempo = X;              //esta es la parte de la decena
}

void mult1x2(void)
{
	resul = Y;                   // salva el valor a multiplicar

	A = highByte(resul);
	Y = A;
	A = wreg;                    //multiplicador

	Y = Y * A;                   // Realiza la multiplicación de las dieciseisenas

	raux = Y;                    //Y Guardalo

	A = lowByte(resul);
	Y = A;
	A = wreg;                    //multiplicador
	Y = Y*A;                     // Realiza la multiplicación de las unidades

	A = lowByte(Y);
	resul = A;
	A = highByte(Y);
	A = A + lowByte(raux);      //Suma dieciseisenas

	resul = (A << 8) + resul;
}

void capturaAD (void)
{
	ADC1->CR |= ADC_CR_ADEN;
	ADC1->ISR |= (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR);
	ADC1->CR |= ADC_CR_ADSTART;

	while ((ADC1->ISR & ADC_FLAG_EOC) == 0)
	{
	}
	ADC1->CR &= ~(ADC_IER_EOCIE | ADC_IER_EOSIE);

	for(uint8_t i=0; i < 250; i++)

	adcramh = ADC1->DR;
}

void wise_hot_rep (uint8_t wreg_p, uint16_t X_p)
{
	wreg = wreg_p;              // Multiplica el dato por el factor
	Y = curva;

	mult1x2();              // Multiplica

	X = resul;              // En tempo2 quedó el entero del dato de temperatura
	X = X - X_p;         // ordenada al origen en °C  (-255.996)
	tempo = X;
}

void Y_A (void)
{
	Y = adcramh;
	A = edorefri;
}

void Bset_Clear_trfst(uint8_t * trfst_1, uint8_t * trfst_2,uint8_t X, uint8_t Y)
{
	*(trfst_1 + X) = 1; // BitSet(trfst_1,X);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
	//BitSet(trefst_aux,5);   //#f_s4short   // Indica la falla (Sensor de salida en corto)
	*(trfst_2 + Y) = 0; // BitClear(trfst_2,Y); //#f_s4open   // Cancela la otras falla del sensor de salida
}

void Bclear_Clear_trfst(uint8_t * trfst_3, uint8_t * trfst_4,uint8_t V, uint8_t W)
{
	*(trfst_3 + V) = 0;// BitClear(trfst_3,V);      // Cancela la fallas del sensor de evaporador
	//BitClear(trefst_aux,2);      // Cancela la fallas del sensor de evaporador
	*(trfst_4 + W) = 0;// BitClear(trfst_4,W);
}



/*
 * NOTAS:   Primero encontrar los canales de ADC para definir bien las conexiones
 * 			Bateria Pendiente
 * 			Definicion de tdev
 * 			Uso de flagsa
 * 			subw y jreq en conjunto
 */

/*
 * #include "main.h"
#include "temper.h"
#include "customMain.h"
#include "math.h"


float       prueba1 = 0.0;
float       adc_f = 0.0;
int         tsac_w_t = 0;
int16_t     tsac_w = 0;

uint16_t    resul = 0;
uint16_t	tsacram [8] = {0};


*************************************************************************************************
uint16_t capturaAD (void)
{
	//Aqui se envian los valores de ADC a las variables correspondientes
	uint16_t   adcram = 0;
	asm("nop");
	//adcram++;
	ADC_Select_CH6 ();
	HAL_ADC_Start(&hadc); // start the adc
	HAL_ADC_PollForConversion(&hadc, 1000); // poll for conversion
	adcram = HAL_ADC_GetValue(&hadc); // get the adc value
	HAL_ADC_Stop(&hadc);
	//Reemplazar por el llamado a funcion o valor de ADC
	//adcram = 0x0FFF;
	return adcram;
}
*************************************************************************************************


void temper(void){
	asm ("nop");

	cnttemp++;
	if(cnttemp == 50)
	{
		//tempe05:
		//ldw			X,#retcncfa;/ Decrementa tiempo para liberar fallas
		//call		decwreg;
		//ldw			X,#retcncfe;
		//call		decwreg;
		//ldw			X,#retcncfr;
		//call		decwreg;
		//ldw			X,#retcncfc;
		//call		decwreg
		cnttemp = 0;
		ADC_Select_CH6();
		// RGM_16/JUL/24:	 capturaAD();
		tsacram[cntmues] = capturaAD();		//	adcram;
		cntmues++;
		if(cntmues == 8)
		tempe17();
	}

}




	//promedio_muestras = promedio_muestras + adcram; ;
	//if(muestras == 8)
	//{
		//curva = promedio_muestras / 8;

		//muestras = 0;
		//promedio_muestras = 0;
		//linealiza();
	//}

void tempe17 (void)
{
	cntmues = 0;
	prom8mues();
	linealiza();

}
void prom8mues(void)
{
	uint16_t resul_aux = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		resul_aux = resul_aux + tsacram[i];
	}
	resul = resul_aux / 8;

}

void linealiza(void)
{
	adc_f = (float) resul;

	if(resul <= 610)
	{
		prueba1 = (0.729727807*adc_f)-462.65612753;
		//prueba1 = (0.726602342*adc_f)-461.468794122;
		tsac_w_t = prueba1;
		tsac_w = tsac_w_t;
		asm ("nop");



	}
	else if(resul > 610)
	{
		prueba1 = (429.8275063742 + (-2.009354582 * adc_f) + (0.002112554* adc_f * adc_f));
		//prueba1 = (289.9866632992 + (-1.643205579 * adc_f) + (0.001875605* adc_f * adc_f));
		tsac_w_t = prueba1;
		tsac_w = tsac_w_t;
		asm ("nop");

	}

}

void ADC_Select_CH6()
{
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc.Instance->CHSELR = (uint32_t) 0;
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

int calcula_temperatura (int m, int b)
{
	int32_t temperatura = 0;

	temperatura = resul;
	temperatura = (m * temperatura) / 100;

	temperatura = temperatura - b;
	temperatura = temperatura /10;

	return temperatura;

 */
