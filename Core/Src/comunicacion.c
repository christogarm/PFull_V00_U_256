



#include "main.h"
#include "comunicacion.h"
#include "customMain.h"
#include "board_PFULLDEF.h"
#include "ELTEC_EmulatedEEPROM.h"


//*************************************************************************************************
#define sizeRxBuffer	0x09

_Bool FlagTx = 0;

uint8_t aTxBuffer[] = {0x07,0x00,0x00,0xFF,0x00,0x00,0x08};
uint8_t aRxBuffer[sizeRxBuffer]; 		// Recepcion de Datos

// 26-Ago-2024		uint8_t ADCresult[] = {0x07,0x00,0x00,0xFF,0x00,0x00,0x08};
//*************************************************************************************************
void comunicacion(void){

		/* Buffer used for transmission */
		//************************************************************************************************************

		// Prueba de trasnmision I2C
		Cnt_EspComu--;
		if(Cnt_EspComu == 255){
			portX[dp_sw] = 0;		//	HAL_GPIO_WritePin(GPIOC, PFULLDEF_dp_sw, GPIO_PIN_SET);      //02-Jul-2024:  Enciende display
			//MX_I2C1_Init();
			goto fin_comu;
		}
		if(Cnt_EspComu == 254){
			portX[dp_sw] = 1;		//	HAL_GPIO_WritePin(GPIOC, PFULLDEF_dp_sw, GPIO_PIN_SET);      //02-Jul-2024:  Enciende display
			//MX_I2C1_Init();
			goto fin_comu;
		}
		if(Cnt_EspComu == 0){
			goto comu_01;
		}
		goto fin_comu;

comu_01:
			Cnt_EspComu = 5;
			if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY){
				countResetI2C = 0;
    	      //Cnt_EspComu = 5;
			  // Se agrega una variable extra debido a que aun sigue en uso el chksum para el envio de Logger 08/01/2025 CGM
				uint32_t chksum_32_HW_LW_AUX = chksum_32_HW_LW;
    		  chksum_32_HW_LW = 0;					// limpia registros de checksum
			  for(uint8_t i = 0; i < 7 ; i++ )
				  aTxBuffer [i] = 0;

			  aTxBuffer [0] = 0x07;
			  aTxBuffer [1] = sizeRxBuffer; // Para no solicitar datos 0x00
			  aTxBuffer [2] = Display_1;		//0XFF;
			  aTxBuffer [3] = Display_2;		//0XFF;
			  aTxBuffer [4] = 0;
			  //aTxBuffer [4] = Ind_Par;
			  for(uint8_t k=0; k<8; k++){
				  aTxBuffer [4] |= (uint8_t) (Ind_Par[k]<<k);
			  }
			  aTxBuffer [5] = 0x00;
			  uint8_t *point_X = &aTxBuffer[0];			// carga dirección del buffer a calcular chksum
			  buildChksumBloq (point_X, 6);			// tamaño del bloque a calcular el chksum
			  aTxBuffer[6] = (uint8_t)(chksum_32_HW_LW & 0x000000FF);
			  chksum_32_HW_LW = chksum_32_HW_LW_AUX;// tambien se agrega esta lines para su recuperación del dato 08/01/2025 CGM

			  if(FlagTx){
				  /**
				  	* Recepción de Datos
				  			 **/
				  if (HAL_I2C_Master_Receive_DMA(&hi2c1,(uint16_t)I2C_ADDRESS, (uint8_t*)aRxBuffer, sizeRxBuffer)==HAL_OK){
					  for(uint8_t k=0; k<8; k++)
						  Botones_T[k] = (_Bool) ((aRxBuffer[6] >> k) & 0x1);		// Recepción de botones CGM 22//11/2024
				  	//Vfw_tretsaa = aRxBuffer[8];
				  	//chk_sums = aRxBuffer[7];
				  	FlagTx= 0;
				  }
				  else{
					  HAL_I2C_DeInit(&hi2c1);
					  HAL_I2C_Init(&hi2c1);
				  }


			  }
			  else{
				  if (HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, COUNTOF(aTxBuffer))==HAL_OK){
					  FlagTx= 1;
				  }
				  else{
					  HAL_I2C_DeInit(&hi2c1);
					  HAL_I2C_Init(&hi2c1);
				  }
				// HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, COUNTOF(aTxBuffer),I2C_GENERATE_STOP);
				// HAL_I2C_Mem_Write_DMA();
			  }



			}
			else{
				countResetI2C++;
				if(!countResetI2C){
					HAL_I2C_DeInit(&hi2c1);
					HAL_I2C_Init(&hi2c1);
				}
			}
fin_comu:
	detecta();


}

void detecta(void){

	if(edorefri!=0x1){
		goto dete03;
	}
	goto findet;
dete03:
detecta_j00:
	if(!GetRegFlagState(Plantilla[logicos2],doorOFF)){
		goto detectaPuerta;
	}
	load_tiempoAhorro1();
	load_tiempoAhorro2();
	goto closedoor_b;

detectaPuerta:
	if(GetRegFlagState(Plantilla[logicos],3)){
		goto detecta_j01;
	}
	goto switch_no;


detecta_j01:
switch_nc:

	for(uint8_t MPx = 0; MPx < 4; MPx++){
		HAL_GPIO_WritePin(PFULLDEF_Px_PORT[MPx],PFULLDEF_Px_PIN[MPx], GPIO_PIN_SET);
		for(uint8_t Px = 0; Px < 5; Px++){
			if(!HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])){
				goto opendoor;
			}
		}
		HAL_GPIO_WritePin(PFULLDEF_Px_PORT[MPx],PFULLDEF_Px_PIN[MPx], GPIO_PIN_RESET);
	}
	goto detecta_j02;
	/*if(HAL_GPIO_ReadPin(PFULLDEF_P5_PORT,PFULLDEF_P5_PIN)){ // if(GetRegFlagState(PD_IDR,swdoor))
		goto detecta_j02;
	}
	 goto opendoor;*/

detecta_j02:
	goto closedoor;


switch_no:
	//goto closedoor;//  Eliminar cuando quieras ver la activación de la puerta

	if((countWaitPuerta == 0)&(firstFlagPuerta1)){
		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[1],PFULLDEF_MPx_PIN[1], GPIO_PIN_SET);
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[0],PFULLDEF_Px_PIN[0])){
			FlagPuertaX[4] = 1;// Puerta Abierta
		}
		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[1],PFULLDEF_MPx_PIN[1], GPIO_PIN_SET);
		firstFlagPuerta1 = 0;
	}

	if((countWaitPuerta == 10)){// Ya pasaron 10 Milisegundos?
		for(uint8_t Px = 0; Px < 5; Px++){
			if(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])){
				FlagPuerta10Times [sizePx*countMPx+Px]++;
			}
			else if(!HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])){
				FlagPuertaX[sizePx*countMPx+Px] = 0;
				FlagPuerta10Times[sizePx*countMPx+Px] = 0;
			}
		}
		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_RESET);

		if(countMPx < sizeMPx){
			countMPx++;
		}
		if(countMPx == sizeMPx){
			countMPx = 0;
		}
		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_SET);

		countWaitPuerta = 0;
	}else if(countWaitPuerta < 10){
		countWaitPuerta++;
	}

	for(uint8_t MPx = 0; MPx < sizeMPx; MPx++){
		for(uint8_t Px = 0; Px < sizePx; Px++){

			if((FlagPuerta10Times [sizePx * MPx + Px] > 0) & (countWaitPuerta < 10)){
				if(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])){
					FlagPuerta10Times [sizePx * MPx + Px]++;
				}
				else if(!HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])){
					FlagPuertaX[sizePx * MPx + Px] = 0;
					FlagPuerta10Times[sizePx * MPx + Px] = 0;
				}
				if(FlagPuerta10Times [sizePx * MPx + Px] == 10){
					FlagPuertaX[sizePx * MPx + Px] = 1;
					FlagPuerta10Times[sizePx * MPx + Px] = 0;
				}

			}
		}
	}


	if(FlagPuertaX[4]){
		goto opendoor;
	}
	goto detecta_j03;

	/*if(!HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[4],PFULLDEF_Px_PIN[4])){
		goto detecta_j03;
	}
	goto opendoor;*/



detecta_j03:
closedoor:
	decwreg(&debdoor);
	if(debdoor == 0){
		goto closedoor_b;
	}
		goto revbotones;

closedoor_b:
	flagsC[f_doorOpen] = 0;// BitClear(flagsC,f_doorOpen); // Linea 441
	cnt_pta_fan = Plantilla[timepaf];
	trefst[4]=0;//BitClear(trefst,4); // Linea 446
	Load_ret2();
	if(flagsa[noctman]){//if(GetRegFlagState(flagsa,noctman)){
		goto detecta_j04;
	}

	goto revbotones;

detecta_j04:
	flagsa[1] = 0;//BitClear(flagsa,1); // Linea 450
	if(!flagsa[nocturno]){// if(!GetRegFlagState(flagsa,nocturno)){
		goto det_j04;
	}
	// goto opendoor05;
	goto revbotones;

det_j04:
	load_timeNoct();
	durdhh=0;
	durdhl=0;
	flagsa[3] = 1;// BitSet(flagsa,3); // Revisa esta linea
	datled[4] = 1;// BitSet(datled,4);
	goto disp_new;

opendoor:
	if(debdoor>=0x80){
		goto noinc;
	}
	debdoor++;

noinc:
	if((debdoor)<0x80){ // Checar con Manuel
		goto revnocman;
	}
	flagsC[f_doorOpen] = 1;// BitSet(flagsC,f_doorOpen);
	retLampOff=5;
	cntHoldP= Plantilla[LD_b]; // Revisar esto el timeHoldP
	retvent= 0x05;
	//if(eePlantilla[eetimepaf] != 0){ // // Se cambio a variable EEPROM porque no hay un refresco pronto en la RAM
	if(findLastValue((uint32_t *)Page_126, (uint32_t) &eePlantilla[eetimepaf])!= 0){
		goto no_ld_timepaf;
	}
	//cnt_pta_fan= Plantilla[timepaf];
	cnt_pta_fan=99;

no_ld_timepaf:


opendoor05:

	if(flagsa[nocturno]){// if(GetRegFlagState(flagsa,nocturno)){
		goto detecta_j05;
	}
	goto revnocman;

detecta_j05:
cancelnoct:
	flagsb[f_luzb] = 1;// BitSet(flagsb,f_luzb);
	flagsa[3] = 0;// BitClear(flagsa,3);
	datled[4] = 0;//BitClear(datled,4);
	cntNoct_H=0;

disp_new:

	goto ahorro_off;

revnocman:

detecta_j06:


detecta_j07:
opendoor10:
	if(cntpah){
		//goto revbotones;
		goto ahorro_off;
	}
	if(cnt_pta_fan==0){
		//goto ahorro_off;
		goto cancelAlarmPA;
	}

	if(Plantilla[timepa]==0){
		//goto revbotones;
		//goto ahorro_off;
		goto cancelAlarmPA;
	}

	trefst[4] = 1;// BitSet(trefst,4);
	goto ahorro_off;

cancelAlarmPA:
	trefst[4] = 0;// BitClear(trefst,4);

ahorro_off:
	if(!flagsC[f_ahorro1]){// if(!GetRegFlagState(flagsC,f_ahorro1)){
		goto ahorro_off_00;
	}
	//load_descansoAH();
	cntdpysp=0xF0;

ahorro_off_00:
	flagsC[f_ahorro1] = 0;// BitClear(flagsC,f_ahorro1);
	flagsC[f_ahorro2] = 0;// BitClear(flagsC,f_ahorro2);
	load_tiempoAhorro1();
	load_tiempoAhorro2();
	goto fin_detecta;

revbotones:
	if(flagsC[f_ahorro1]){// if(GetRegFlagState(flagsC,f_ahorro1)){
		goto ask_ahorro;
	}
	//ld_tdev_to_wreg();
	if( (int16_t) tdev_to_Word() >= (int16_t) TwoByteInArrayToWord(&Plantilla[pulldown_H]) ){ // Checar con el ing Manuel
		goto ahorro_off;
	}


ask_ahorro:
	/*if(cntCiclosCmp==0){
		goto ask_time_ahorro;
	}*/
	if(flagsC[f_spReached]){// if(GetRegFlagState(flagsC,f_spReached)){
		goto ask_time_ahorro;
	}
	goto ahorro_off;

ask_time_ahorro:

	if((t_ahorro1_H)==0){
		goto modoAhorro1_ON;
	}
	load_tiempoAhorro2();
	flagsC[f_ahorro1] = 0; 	//BitClear(flagsC,f_ahorro1);
	flagsC[f_ahorro2] = 0;	// BitClear(flagsC,f_ahorro2);
	goto fin_detecta;

modoAhorro1_ON:
	if(flagsC[f_ahorro1]){//if(GetRegFlagState(flagsC,f_ahorro1)){
		goto modoAhorro1_ON_2;
	}
	//BitSet(datled,luzN);
	cntdpysp= 0xF0;

modoAhorro1_ON_2:
	flagsC[f_ahorro1]= 1; // BitSet(flagsC,f_ahorro1);
	if(t_ahorro2_H){ // Linea 504
		goto fin_detecta;
	}

	if(flagsC[f_ahorro2]){//if(GetRegFlagState(flagsC,f_ahorro2)){
		goto modoAhorro1_ON_3;
	}
	cntdpysp=0xF0;

modoAhorro1_ON_3:
	flagsC[f_ahorro2] = 1;// BitSet(flagsC,f_ahorro2);

fin_detecta:



revbotones_2:
	//botonst = 0;

	// btn_pr = 0;
	for(uint8_t k=0; k<8; k++){
		btn_pr[k] = 0;
		botonst[k] = 0;
		botonst2[k] = 0;
	}
	// Botones_T &= 0x0F;// Linea 617-619

	if(flagsb[f_prog]){
		goto no2btn;
	}
	if(!Botones_T[btn_1]){//if(!GetRegFlagState(Botones_T,btn_1)){ // No esta presionado el Boton 1 (entra al IF)
		goto no2btn;
	}
	if(!Botones_T[btn_3]){ // No esta presionado el Boton 3 (entra al IF)
		goto no2btn;
	}
	if(deb_2btn!=0){
		goto findet_jp;
	}
	botonst2[1] = 1;//BitSet(botonst2,1);

findet_jp:
	goto findet;

no2btn:
	if(deb_2btn>=150 ){
		//goto ask_btn1;
		goto ask_btn2_f3;
	}
	deb_2btn += 3;

ask_btn2_f3:
	if(flagsb[f_prog]){
		goto noBtn2F3;
	}
	if(!Botones_T[btn_2]){
		goto noBtn2F3;
	}

	if(debBtn2F3!=0){
		//goto findet_jp;
		goto ask_btn1;
	}
	botonst2[2] = 1;	// BitSet(botonst2,2);
	flagsC[f_spReached] = 0;// BitClear(flagsC,f_spReached);
	debBtn2F3=30;
	goto findet_jp;

noBtn2F3:
	if(debBtn2F3>=30 ){ // Revisar si mi logica es corecta. L 633 (jruge)
		goto ask_btn1;
	}
	debBtn2F3 +=1;

ask_btn1:
	if(!Botones_T[btn_1]){
		goto no_btn1;
	}
	if(Botones_COPY[btn_1]){
		goto ask_btn1_sec;
	}
	botonst[b1_f1] = 1; // BitSet(botonst,b1_f1);
	timeBuzzOn=20;

ask_btn1_sec:
	if(!flagsb[f_prog]){
		goto func_sec_norm_btn1;
	}
	if(deb2_btn1>110){
		goto ask_btn2;
	}
	botonst[b1_f2] = 1; // BitSet(botonst,b1_f2);
	goto findet;

func_sec_norm_btn1:
	if(fbtn_deb[b1_ow]){//if(GetRegFlagState(fbtn_deb,b1_ow)){
		goto ask_btn2;
	}
	if(deb2_btn1!=0){
		goto ask_btn2;
	}
	botonst[b1_f2] = 1;// BitSet(botonst,b1_f2);
	fbtn_deb[b1_ow] = 1;// BitSet(fbtn_deb,b1_ow);
	//timeBuzzOn=20;
	goto findet;

no_btn1:
	fbtn_deb[b1_ow] = 0;//BitClear(fbtn_deb,b1_ow);
	/*if(!GetRegFlagState(Botones_COPY,btn_1)){
		goto ask_deb2_btn1;
	}
	if(deb2_btn1<125){
		goto ask_deb2_btn1;
	}
	BitSet(botonst,b1_f1);*/

ask_deb2_btn1:
	if(deb2_btn1>=150){ // JRUGE
		goto ask_btn2;
	}
	deb2_btn1+=3;
	//goto ask_btn2;

ask_btn2:
	if(!Botones_T[btn_2]){
		goto no_btn2;
	}
	if(Botones_COPY[btn_2]){
		goto ask_btn2_sec;
	}
	botonst[b2_f1] = 1;//BitSet(botonst,b2_f1);
	timeBuzzOn=20;

ask_btn2_sec:
	if(fbtn_deb[b2_ow]){// if(GetRegFlagState(fbtn_deb,b2_ow)){
		goto ask_btn3;
	}
	if(deb2_btn2!=0){
		goto ask_btn3;
	}
	botonst[b2_f2] = 1;	//BitSet(botonst,b2_f2);
	// timeBuzzOn=20;
	fbtn_deb[b2_ow] = 1;//BitSet(fbtn_deb,b2_ow);
	goto findet;

no_btn2:
	fbtn_deb[b2_ow] = 0;//BitClear(fbtn_deb,b2_ow);
	/*if(!GetRegFlagState(Botones_COPY,btn_2)){ // Linea 714 - 719
		goto ask_deb2_btn2;
	}
	if(deb2_btn2<125){// jrult
		goto ask_deb2_btn2;
	}
	BitSet(botonst,b2_f1);
	timeBuzzOn=20;*/

ask_deb2_btn2:
	if(deb2_btn2>=150){// JRUGE
		goto ask_btn3;
	}
	deb2_btn2 += 3;
	//goto ask_btn3;

/*ask_btn2:
	if(!GetRegFlagState(Botones_T,btn_2)){
		goto no_btn2;
	}
	if(GetRegFlagState(Botones_COPY,btn_2)){
		goto ask_btn2_sec;
	}
	BitSet(botonst,b2_f1);
	timeBuzzOn=20;

ask_btn2_sec:
	if(!GetRegFlagState(flagsb,f_prog)){
		goto func_sec_norm_btn2;
	}
	if(deb2_btn2>ask_btn3){
		goto ask_btn3;
	}
	BitSet(botonst,b2_f2);
	goto findet;

func_sec_norm_btn2:
	// Linea 742 - 764
*/


ask_btn3:
	if(!Botones_T[btn_3]){
		goto no_btn3;
	}
	if(Botones_COPY[btn_3]){
		goto ask_btn3_sec;
	}
	botonst[b3_f1] = 1;// BitSet(botonst,b3_f1);
	timeBuzzOn=20;

ask_btn3_sec:
	if(!flagsb[f_prog]){
		goto func_sec_norm_btn3;
	}
	if(deb2_btn3>110){ //jrugt
		goto ask_btn4;
	}
	botonst[b3_f2] = 1;//BitSet(botonst,b3_f2);
	goto findet;

func_sec_norm_btn3:
	if(fbtn_deb[b3_ow]){// if(GetRegFlagState(fbtn_deb,b3_ow)){
		goto ask_btn4;
	}
	if(deb2_btn3!=0){
		goto ask_btn4;
	}
	botonst[b3_f2] = 1;// BitSet(botonst,b3_f2);
	fbtn_deb[b3_ow] = 1;//BitSet(fbtn_deb,b3_ow);
	// timeBuzzOn=20;
	goto findet;

no_btn3:
	fbtn_deb[b3_ow] = 0; // BitClear(fbtn_deb,b3_ow);
	/*if(!GetRegFlagState(Botones_COPY,btn_3)){ // Linea 790- 794
		goto ask_deb2_btn3;
	}
	if(deb2_btn3<125){ // jrult
		goto ask_deb2_btn3;
	}
	BitSet(botonst,b3_f1);
	*/

ask_deb2_btn3:
	if(deb2_btn3>=150){
		goto ask_btn4;
	}
	deb2_btn3 += 3;
	// goto ask_btn4;

ask_btn4: // Linea 806
/*
 * Linea 807 - 836
 *
 */

findet:
	//Botones_COPY = Botones_T;
	for(uint8_t k = 0; k<8; k++)
		Botones_COPY[k] = Botones_T[k];
	if(!flagsb[f_prog]){
		goto det_j18;
	}
	//btn_pr=botonst;
	for(uint8_t k=0; k<8; k++){
		btn_pr[k] = botonst[k];
		botonst[k] = 0;
	}

det_j18:
	if(!flagsBattery[batON]){
		goto det_j19;
	}
	botonst[b2_f1] = 0; // BitClear(botonst,b2_f1);
	botonst[b3_f1] = 0;// BitClear(botonst,b3_f1);

det_j19:

}

//--------------------------------
//void MX_I2C1_Init(void)
//{
//
//  /* USER CODE BEGIN I2C1_Init 0 */
//
//  /* USER CODE END I2C1_Init 0 */
//
//  /* USER CODE BEGIN I2C1_Init 1 */
//
//  /* USER CODE END I2C1_Init 1 */
//  hi2c1.Instance = I2C1;
//  hi2c1.Init.Timing = 0x30108DFF;
//  hi2c1.Init.OwnAddress1 = 0;
//  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//  hi2c1.Init.OwnAddress2 = 0;
//  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Analogue filter
//  */
//  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure Digital filter
//  */
//  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN I2C1_Init 2 */
//
//  /* USER CODE END I2C1_Init 2 */
//
//}





