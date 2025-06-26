



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


void detecta(void);
void  doorDetect(_Bool boolSwitch_);		// Detecta si la puerta esta abierta o cerrad, el parametro enviado es para saber si es normalmente abierto o cerrado

// 26-Ago-2024		uint8_t ADCresult[] = {0x07,0x00,0x00,0xFF,0x00,0x00,0x08};
//*************************************************************************************************
void comunicacion(void){

comu_normal:
		/* Buffer used for transmission */
		//************************************************************************************************************

		// Prueba de trasnmision I2C
		Cnt_EspComu--;
		if(Cnt_EspComu == 0){
			goto comu_01;
		}
		goto fin_comu;

comu_01:
			//Cnt_EspComu = T_Espera;
			Cnt_EspComu = 10;

			if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY){
				if(FlagTx){
					if (HAL_I2C_Master_Receive_DMA(&hi2c1,(uint16_t)I2C_ADDRESS, (uint8_t*)aRxBuffer, sizeRxBuffer)==HAL_OK){

						for(uint8_t k=0; k<8; k++)
							Botones_T[k] = (_Bool) ((aRxBuffer[6] >> k) & 0x1);		// Recepción de botones CGM 22//11/2024
						FlagTx = 0;
					}
					else{
						HAL_I2C_DeInit(&hi2c1);
						HAL_I2C_Init(&hi2c1);
					}
				}
				else{
					countResetI2C = 0;
					// Se agrega una variable extra debido a que aun sigue en uso el chksum para el envio de Logger 08/01/2025 CGM
					uint32_t chksum_32_HW_LW_AUX = chksum_32_HW_LW;
					chksum_32_HW_LW = 0;					// limpia registros de checksum
					for(uint8_t i = 0; i < 7 ; i++ )
						aTxBuffer [i] = 0;

					aTxBuffer [0] = 0x07;
				  	aTxBuffer [1] = sizeRxBuffer; // Para no solicitar datos 0x00
				  	aTxBuffer [2] = Display_1;		//0XFF;
				  	aTxBuffer [3] = Display_2;		//0XFF;

				  	for(uint8_t k=0; k<8; k++){
				  		aTxBuffer [4] |= (uint8_t) (Ind_Par[k] << k);
				  		aTxBuffer [5] |= (uint8_t) (dms_extra[k] << k);
				  	}

				  	buildChksumBloq (&aTxBuffer[0], 6);			// tamaño del bloque a calcular el chksum
				  	aTxBuffer[6] = (uint8_t)(chksum_32_HW_LW);

				  	chksum_32_HW_LW = chksum_32_HW_LW_AUX;// tambien se agrega esta lines para su recuperación del dato 08/01/2025 CGM

				  	if(HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, COUNTOF(aTxBuffer))==HAL_OK){
				  		FlagTx= 1;
				  	}
				  	else{
				  		HAL_I2C_DeInit(&hi2c1);
				  		HAL_I2C_Init(&hi2c1);
				  	}
				}
			}
			else{
				countResetI2C++;
				if(countResetI2C == 10){
					countResetI2C = 0;
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
		goto switch_nc;
	}
	goto switch_no;


switch_nc:

	doorDetect(0);
	if(FlagPuertaX[9]){
		goto opendoor;
	}
detecta_j02:
	goto closedoor;


switch_no:
	//goto closedoor;//  Eliminar cuando quieras ver la activación de la puerta
	doorDetect(1);

	if(FlagPuertaX[9]){
		goto opendoor;
	}
	goto closedoor;

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
	goto opendoor05;
	goto revbotones;

det_j04:
	load_timeNoct();
	durdhh=0;
	//durdhl=0;
	flagsa[3] = 1;// BitSet(flagsa,3); // Revisa esta linea
	datled[4] = 1;// BitSet(datled,4);
	goto disp_new;

opendoor:
	if(debdoor>=0x80){
		goto noinc;
	}
	debdoor++;

noinc:
	if((debdoor)<0x80){
		goto revnocman;
	}
	flagsC[f_doorOpen] = 1;// BitSet(flagsC,f_doorOpen);
	//retLampOff=5;

	retvent= 0x05;
	//if(eePlantilla[eetimepaf] != 0){ // // Se cambio a variable EEPROM porque no hay un refresco pronto en la RAM
	if(reePlantilla[eetimepaf]!= 0){
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
		goto ahorro_off;
		//goto cancelAlarmPA;
	}

	if(Plantilla[timepa]==0){
		//goto revbotones;
		goto ahorro_off;
		//goto cancelAlarmPA;
	}

	trefst[4] = 1;// BitSet(trefst,4);

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
		goto ask_time_ahorro;
	}
	//ld_tdev_to_wreg();
	if( (int16_t) (tdev_to_Word()) >= (int16_t) (TwoByteInArrayToWord(&Plantilla[pulldown_H])) ){
		goto ahorro_off;
	}


ask_ahorro:
	if(cntCiclosCmp==0){
		goto ask_time_ahorro;
	}

	goto ahorro_off;

ask_time_ahorro:

	//;// RGM_21-Nov-2023
	//CLRW    X
	//ld     A,RTC_TR3      ;HORAS
	//ld     XH,A
	//ld     A,RTC_TR2      ;MINUTOS
	//ld     XL,A
	HAL_RTC_GetTime (&hrtc, &hRtcTime, RTC_FORMAT_BCD);
	uint16_t hRtcHourMint_W = (uint16_t) ((hRtcTime.Hours << 8) + hRtcTime.Minutes);
	uint16_t eehoraAhOn_W = (uint16_t) ((reePlantilla[eehoraAhOn_H] << 8) + reePlantilla[eehoraAhOn_L]);
	//CPW    X,eehoraAhOn      ;
	if(eehoraAhOn_W != hRtcHourMint_W)//jrne   CompareEntradaAhorro
		goto CompareEntradaAhorro;
	//; Provoca una entrada fozosa al modo de ahorro
	t_ahorro1_H = 0;		//clr		t_ahorro1_H
	//clr		t_ahorro1_L
	t_ahorro2_H = 0;		//clr		t_ahorro2_H
	//clr		t_ahorro2_L

CompareEntradaAhorro:
	//CPW    X,eehoraAhOff      ;
	uint16_t eehoraAhOff_W = (uint16_t) ((reePlantilla[eehoraAhOff_H] << 8) + reePlantilla[eehoraAhOff_L]);
	if(eehoraAhOff_W != hRtcHourMint_W)		//jrne   OutCompareAhorro
		goto OutCompareAhorro;

	// ; provoca una salida forzosa del modo de ahorro
	if(!flagsC[f_ahorro1])	//btjf		flagsC,#f_ahorro1,ahorro_off_01
		goto ahorro_off_01;

	cntdpysp = 0xF0;		//mov			cntdpysp,#$F0;		/ Despliega Set Point y el diferencial

ahorro_off_01:
	flagsC[f_ahorro1] = 0;		//	bres	flagsC,#f_ahorro1
	flagsC[f_ahorro2] = 0;		//	bres	flagsC,#f_ahorro2
	load_tiempoAhorro1();		//	call	load_tiempoAhorro1;			/ cada que se abre puerta vuelve a cargar tiempos de ahorro
	load_tiempoAhorro2();		//	call	load_tiempoAhorro2;

OutCompareAhorro:
	//	ld			A,t_ahorro1_L;
	//	or			A,t_ahorro1_H;
	if(t_ahorro1_H == 0)//	jreq		modoAhorro1_ON;					/ ya se completo el tiempo para entrar a Ahorro 1? sí activa ahorro 1
		goto modoAhorro1_ON;
	load_tiempoAhorro2();	//	call		load_tiempoAhorro2;			/ no, manten cargado el tiempo para Ahorro2
	flagsC[f_ahorro1] = 0;	//	bres    flagsC,#f_ahorro1;			/ desactiva bandera de ahorro1
	flagsC[f_ahorro2] = 0;	//	bres    flagsC,#f_ahorro2;			/ desactiva bandera de ahorro2

	goto fin_detecta;		//	jra			fin_detecta

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
	if(deb_2btn >= 150 ){
		goto ask_btn1;
		//goto ask_btn2_f3;
	}
	deb_2btn += 3;

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
	if(fbtn_deb[b2_ow])
		goto ask_btn3;

	if(deb2_btn2 != 0)
		goto ask_btn3;

	botonst[b2_f2] = 1;
	fbtn_deb[b2_ow] = 1;

	goto findet;


no_btn2:
	fbtn_deb[b2_ow] = 0;//BitClear(fbtn_deb,b2_ow);
	if(!Botones_COPY[btn_2]){ // Linea 714 - 719
		goto ask_deb2_btn2;
	}
	if(deb2_btn2<125){// jrult
		goto ask_deb2_btn2;
	}
	botonst[b2_f1] = 1;
	timeBuzzOn = 20;

ask_deb2_btn2:
	if(deb2_btn2>=150){// JRUGE
		goto ask_btn3;
	}
	deb2_btn2 += 3;
	//goto ask_btn3;

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
	return;

}

void doorDetect(_Bool boolSwitch_){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	// Esto es para simular en caso de que se abra la puerta cuando apenas se enciende el control

	/*
	 * 19-Jun-2025 RGM:	Prueba de entrada a funcion detecta de puerta
	 */
	//HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	//HAL_GPIO_TogglePin(PFULLDEF_MPx_PORT[1],PFULLDEF_MPx_PIN[1]);


	// Procesa el MUX de las salidas
	countWaitPuerta++;
	if(countWaitPuerta > 4){	// Ya pasaron 8 Milisegundos?
		countWaitPuerta = 0;
		for(uint8_t Px = 0; Px < sizePx; Px++){
			//
			if( !(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px]) ^ boolSwitch_) ){
				if(FlagPuerta10Times[sizePx*countMPx+Px] <4)
					FlagPuerta10Times [sizePx*countMPx+Px]++;
				if(FlagPuerta10Times[sizePx*countMPx+Px] == 4){
					FlagPuertaX[sizePx * countMPx + Px] = 1;
					FlagPuerta10Times [sizePx*countMPx+Px]++;
				}
			}
			else{
				FlagPuerta10Times [sizePx*countMPx+Px] = 0;
				FlagPuertaX[sizePx * countMPx + Px] = 0;
			}
		}
		for(uint8_t MPx = 0; MPx < sizeMPx; MPx++){
			 HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[MPx], PFULLDEF_MPx_PIN[MPx], GPIO_PIN_RESET);

//			  GPIO_InitStruct.Pin = PFULLDEF_MPx_PIN[MPx];
//			  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//			  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//			  HAL_GPIO_Init(PFULLDEF_MPx_PORT[MPx], &GPIO_InitStruct);
		}
		countMPx++;
		if(countMPx >= sizeMPx)
			countMPx = 0;

		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_SET);
		GPIO_InitStruct.Pin = PFULLDEF_MPx_PIN[countMPx];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(PFULLDEF_MPx_PORT[countMPx], &GPIO_InitStruct);
		//HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[MPx],PFULLDEF_MPx_PIN[MPx], GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_SET);

	}
	return;


//	if((countWaitPuerta == 6)){// Ya pasaron 12 Milisegundos?
//		for(uint8_t Px = 0; Px < 5; Px++){
//			if(!(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px]) ^ boolSwitch_) ){
//				FlagPuerta10Times [sizePx*countMPx+Px]++;
//			}
//			else if((HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px]) ^ boolSwitch_)){
//				FlagPuertaX[sizePx*countMPx+Px] = 0;
//				FlagPuerta10Times[sizePx*countMPx+Px] = 0;
//			}
//		}
//		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_RESET);
//
//		if(countMPx < sizeMPx)
//			countMPx++;
//		if(countMPx == sizeMPx)
//			countMPx = 0;
//
//		HAL_GPIO_WritePin(PFULLDEF_MPx_PORT[countMPx],PFULLDEF_MPx_PIN[countMPx], GPIO_PIN_SET);
//		countWaitPuerta = 0;
//	}else if(countWaitPuerta < 6){
//		countWaitPuerta++;
//	}
//
//	for(uint8_t MPx = 0; MPx < sizeMPx; MPx++){
//		for(uint8_t Px = 0; Px < sizePx; Px++){
//
//			if((FlagPuerta10Times [sizePx * MPx + Px] > 0) & (countWaitPuerta < 6)){
//				if(!(HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px]) ^ boolSwitch_)){
//					FlagPuerta10Times [sizePx * MPx + Px]++;
//				}
//				else if((HAL_GPIO_ReadPin(PFULLDEF_Px_PORT[Px],PFULLDEF_Px_PIN[Px])^ boolSwitch_)){
//					FlagPuertaX[sizePx * MPx + Px] = 0;
//					FlagPuerta10Times[sizePx * MPx + Px] = 0;
//				}
//				if(FlagPuerta10Times [sizePx * MPx + Px] == 10){
//					FlagPuertaX[sizePx * MPx + Px] = 1;
//					FlagPuerta10Times[sizePx * MPx + Px] = 0;
//				}
//
//			}
//		}
//	}
}




