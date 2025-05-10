




#include "main.h"
#include "board_PFULLDEF.h"
#include "customMain.h"
#include "bluetooth.h"


#include "ELTEC_EmulatedEEPROM.h"

_Bool WaitSerAnsw_Ble = 0;  					// bandera, esperando respuesta por el puerto serial
_Bool receivecomplete_Ble = 0;					// recepcion completa DMA


unsigned long 	PrevMillis = 0;



uint8_t CntDebInp1 = 0;				// contador debounce Entrada 1


//********************************************************************************************************************************
//********************************************************************************************************************************
//********************************************************************************************************************************
//------------------------------------------------------------------------------------------------------------------------------------------------
/*
*/
unsigned int AtParsing_Ble (int Caseuno,int Casedos,char *replystr)
{
	unsigned int AnsAtParsing = 0;			// 12-ene-2021, inicializa la respuesta de la funcion
	//  manuel if ((MQTT.TCP_Flag == false) && (receivecomplete))
	if (receivecomplete_Ble)
	{
	  receivecomplete_Ble = 0;

		if (strstr(SerialAnswBLE, replystr) != 0)  //     NULL != NULL  ?
		{
			AnsAtParsing = Caseuno;		// 12-ene-2021,
		}

	}
	return AnsAtParsing;
}
//--------------------------------------- Serializacion de Strings -------------------------------------------------------
/*
*/
void   SerializeString_Ble (char *tosend)
{
		asm ("nop");
		uint16_t localLength = strlen(tosend);
	    memcpy(TxBuffer_Ble, tosend, localLength);
		HAL_UART_Transmit_DMA(&huart2, TxBuffer_Ble, localLength);
}
//---------------------------------------------------------------------------------------------------------------

void   SerializeString2 (uint8_t *tosend, uint8_t tamano)
{
	asm ("nop");
	uint16_t localLength = (uint16_t)tamano;
    memcpy(TxBuffer_Ble, tosend, localLength);
    while(huart2.gState == HAL_UART_STATE_BUSY_TX);
	HAL_UART_Transmit_DMA(&huart2, TxBuffer_Ble, localLength);
}
/*
*/
void serialEvent_Ble ()
{
	//------------------------------------------------------------------------------------------------
		uint16_t sizeBufferRx_1;
		sizeBufferRx_1 = sizeof(RxBuffer_Ble);
		HAL_UART_Receive_DMA ( &huart2, RxBuffer_Ble, sizeBufferRx_1 );

		uint16_t delta;
		uint16_t contadorRx = __HAL_DMA_GET_COUNTER(&hdma_usart2_rx );
		if( contadorRx < sizeBufferRx_1 ){						// si cambio esta relacion, es porque ya llego algo al buffer

			//timeoutRx ++;
			//if( timeoutRx >= 17 )
			// 18-oct-2021  if( USART1->SR & USART_SR_IDLE )
			if( USART2->ISR & USART_ISR_IDLE ){
			// if(__HAL_UART_GET_FLAG(&handleUART1,UART_FLAG_IDLE))
				asm ("nop");

				contadorRx = __HAL_DMA_GET_COUNTER(&hdma_usart2_rx );
				delta = sizeBufferRx_1 - contadorRx;
				// HAL_UART_DMAStop(&huart2);
				// HAL_UART_DMAPause(&huart2);
				// HAL_UART_DMAResume(&huart2);
				HAL_UART_AbortReceive(&huart2);
				//((hdma_usart2_rx).Instance)->CNDTR = sizeBufferRx_1;

			    /* Configure the source, destination address and the data length & clear flags*/
			    // DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);


				if( (USART2->ISR & USART_ISR_IDLE)  )
					__HAL_UART_CLEAR_IDLEFLAG(&huart2);

				uint8_t i;
				for (i=0;i<(sizeof(RxBuffer_Ble)-contadorRx);i++)
					{
						SerialAnswBLE[i]=RxBuffer_Ble[i];
					}
				SerialAnswBLE[i]= 0;					// Add Null byte
				receivecomplete_Ble = 1;
			}

		}
}
//----------------------------------------------------------------------------------------------------------------------
unsigned int SendATreply_Ble(char command[20], char replystr[30],int Caseuno,int Casedos, unsigned long waitms)
{
	unsigned int AnsSendATreply = 0;
	char reply[20];			;//	char reply[10];

	if (!WaitSerAnsw_Ble)
	{
		WaitSerAnsw_Ble = 1;  						// bandera, esperando respuesta por el puerto serial

		PrevMillis = millis();
		// Test Manuel 03-sep-2021 	strcpy(reply, replystr);       //????????? Modificar esta funcion para mandar argunto dentro de la misma

		SerializeString_Ble(command);
	}

	serialEvent_Ble();                  // Manuel, Hace revision si llego algun dato serial

	AnsSendATreply = AtParsing_Ble(Caseuno,Casedos,replystr);  // 12-ene-2021

	//02-SEP-2021		  currentMillis = millis();       // Manuel, se actualizan los ms que estan transcurriendo actualmente
	if ((millis() - PrevMillis) >= waitms)
	{
		WaitSerAnsw_Ble = 0;	// intenta nuevamente el comando
	}

	return	AnsSendATreply;
}

//*************************************************************************************************

static unsigned int  ParamItem = 1;
static unsigned int  BaudRateTest = 1;

void SetUpBluetooth_Ble(){
	//if (findLastValue((uint32_t)&eePlantilla[eeFlagBLE]) == 0){				// Mauel 09-dic-2021:	Si ya fue configurado "EEPROM VAR", jamas vuelve a llamar a configuracion ???
		ConfigBLE_Ble ();
	//}
	//else{
	//	if (tick_1s == 1){
			//BluetoothState = 2;
			HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_RESET);      //28-May-2024:  Enciende Modulo WIFI
			//GPIOC->BSRR = GPIO_BSRR_BR_6;
	//	}
	//}
}

void	ConfigBLE_Ble (){

	if (tick_1s == 1){
		HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_RESET);    // Libera la energia del modulo, despues de 1 segundo del power on
		//GPIOC->BSRR = GPIO_BSRR_BR_6;
	}

	switch (ParamItem){
	  uint8_t i;


		case 1:			//  Manuel,   Baud Rate Scan
			if (SendATreply_Ble("BLE_AT+AT","BLE_OK\r\n",ParamItem,0,1000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble_func(5);
//				WaitSerAnsw_Ble = 0;
//				ParamItem = 5;
			}
			/*
			else if (!WaitSerAnsw_Ble){			// ya expiro el tiempo de haber mandado el comando   "+++"
				BaudRateTest ++;
				switch (BaudRateTest){
					case 1:  ChangeBaud_Ble (1200); break;
					case 2:  ChangeBaud_Ble (2400); break;
					case 3:  ChangeBaud_Ble (4800); break;
					case 4:  ChangeBaud_Ble (9600); break;
					case 5:  ChangeBaud_Ble (19200); break;
					case 6:  ChangeBaud_Ble (38400); break;
					case 7:  ChangeBaud_Ble (57600); break;
					case 8:  ChangeBaud_Ble (115200); break;
					case 9:  ChangeBaud_Ble (230400); break;
					default: BaudRateTest = 0; break;
				}
			}
			*/
			break;
//--------------------------------------
/*		case 2:			//  Manuel,   Retardo tonto por el pin PWRC
			asm ("nop");

			if (SendATreply_Ble("","",ParamItem,0,2000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble_func(3);
//				WaitSerAnsw_Ble = 0;
//				ParamItem = 3;
			}
			else if (!WaitSerAnsw_Ble){			// ya expiro el tiempo de haber mandado el comando   "+++"
				ParamItem = 3;
			}
			break;
//--------------------------------------
		case 3:			//  Manuel,   Cambia el Baud
			asm ("nop");

			if (SendATreply_Ble("AT+BAUD\r\n","+BAUD=8",ParamItem,0,4000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble_func(5);
//				WaitSerAnsw_Ble = 0;
//				ParamItem = 5;
			}
			else if (!WaitSerAnsw_Ble){			// ya expiro el tiempo de haber mandado el comando   "+++"
				ParamItem = 4;
			}
			break;
	  case 4:			//  Manuel,   Cambia el nombre de difusion
			if (SendATreply_Ble("AT+BAUD8\r\n","+BAUD=8",ParamItem,0,3000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble_func(1);
//				WaitSerAnsw_Ble = 0;
//				ParamItem = 1;
				BaudRateTest = 8;
				ChangeBaud_Ble (115200);
			}
			else if (!WaitSerAnsw_Ble){			// ya expiro el tiempo de haber mandado el comando   "+++"
				ParamItem = 1;
			}
			break;
*/
	  case 5:			//  Manuel,   Cambia el nombre de difusion
		  	if (SendATreply_Ble("BLE_AT+NAMEIMBERA-CTOF-F\r\n","BLE_OK\r\n",ParamItem,0,2000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
			//if (SendATreply_Ble("BLE_AT+NAMEIMBERA-HEALTH\r\n\r\n","BLE_OK\r\n",ParamItem,0,2000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble_func(6);
//				WaitSerAnsw_Ble = 0;
//				ParamItem = 6;
			}
			break;
			/*
	  case 6:			//  Manuel,   Establece no hacer ECO
			if (SendATreply_Ble("AT+ENLOG0\r\n","OK\r\n",ParamItem,0,2000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
				WaitSerAnsw_Ble = FALSE;
				BluetoothState = 2;
				ParamItem = 1;
				nop();
				// Termina configuracion, escribe en EEPROM un testigo
				#pragma asm
					#include "setFlagBLE.asm"
				#pragma endasm
			}
			break;
			*/

		default:
			BluetoothState = 2;
			WaitSerAnsw_Ble_func(1);
//			WaitSerAnsw_Ble = 0;
//			ParamItem = 1;
			asm ("nop");
			// Termina configuracion, escribe en EEPROM un testigo

				// _ASM: Pendiente a traducir asm

			break;
	}
}
void ChangeBaud_Ble (uint32_t USART2_BAUDRATE){

  /*
  	  Para Adaptar UART2 en otros modelos de BLE
 */

}
//*************************************************************************************************
void GetParamBluetooth_Ble(){
	uint8_t i =0;
	char *foo;

	HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_RESET);      //28-May-2024:  Enciende Modulo WIFI
	//GPIOC->BSRR = GPIO_BSRR_BR_6;
	switch (ParamItem){
	  case 1:			//  Manuel,   09-nov-2021 Obtiene la Mac Adress
		if (SendATreply_Ble("BLE_AT+MAC","BLE_+MAC=",ParamItem,0,2000) == ParamItem){      //  Manuel, retorno un GSM_Response = 1?
			 foo = strchr(SerialAnswBLE,'=');
			 for (i = 0; i<=11; i++)
				 Bloque_handshake [i+2] = foo[i+1];		//macAdress [i] = foo[i+1];
			 ParamItem = 2;
		 }
		 break;
	  case 2:			//  Manuel,   Obtiene la Mac Adress
		 BluetoothState = 3;
		 ParamItem = 1;
	 break;
	}
}
unsigned long TestMessMilis;
uint8_t PasswordALG = 0 ;       // RGM_8-Dic-2023
uint8_t RndNumber = 0 ;       // RGM_8-Dic-2023      //RM_20240304 Para agregar PASSWORD de seguridad BLE
//uint8_t DevLock = 0 ;       // RGM_8-Dic-2023        //RM_20240304 Para agregar PASSWORD de seguridad BLE

//*************************************************************************************************
void TransmitReceive_Ble(){
    asm ("nop");

    uint8_t i_ = 0;
	HAL_GPIO_WritePin(GPIOC, PFULLDEF_FET_ON_OFF_WIFI, GPIO_PIN_RESET);      //28-May-2024:  Enciende Modulo WIFI
	//GPIOC->BSRR = GPIO_BSRR_BR_6;
	serialEvent_Ble ();
	if (receivecomplete_Ble){
		 receivecomplete_Ble = 0;

		 //		#include "chkRxFlag.asm"
		 if (flagsRxFirm [1]){    			// Ya se sabe cuantos bloques se van a recibir ?
			 flagsRxFirm [2] = 1;				// indica que ya llegó un paquete
		 }

		 // 18-oct-2021  if (strstr(SerialAnswBLE, "\x40\x21")){
		 if (memcmp(SerialAnswBLE, "\x40\x21",(size_t) 2) == 0){	 		// CÓDIGO DE "CONFIRMACIÓN DE CONEXIÓN (HANDSHAKE)"
			 codeTX = 0x21;
		 }
		 if (memcmp(SerialAnswBLE, "\x40\x5C",2) == 0){		 	// CÓDIGO DE TIEMPO BCD "ESCRITURA DEL TIEMPO EN FORMATO BCD"
			 codeTX = 0x5C;
		 }
		 if (memcmp(SerialAnswBLE, "\x40\x80",2) == 0){			// "TOKEN DE FUNCIONAMIENTO AL MÓDULO WIFI"
			 codeTX = 0x80;
		 }
		 if (memcmp(SerialAnswBLE, "\x40\x81",2) == 0){
			 codeTX = 0x81;
		 }
		 if (memcmp(SerialAnswBLE, "\xF1\x3D",2) == 0){			// RESPUESTA ENVÍO DE LOGGER MODULO WIFI
			 codeTX = 0x3D;
		 }


		 if (memcmp(SerialAnswBLE, "\x40\x5E",2) == 0){		   // CÓDIGO DE DESBLOQUEO TEMPORAL
			 //timeUnlockWIFI = 255;	// carga tiempo de desbloqueo de comandos protegidos por password
			 DevLock = 0x72;
			 //	#pragma asm
			 // Carga datos de bloque para transmitir la respuesta
			 grabacion_exitosa_handshake();
//			 Bloque_handshake[comando1] =	0xF1;	//comando1 = 0xF1;
//			 Bloque_handshake[comando2] =  0x3D;	//comando2 = 0x3D;				// indica que la grabación fue exitosa

			//pointTx = (uint16_t)&comando1;
			manda_transmision();
			//pointTx = &Bloque_handshake[comando1];		// &comando1;
			//pointInitTx = &Bloque_handshake[comando1];	// &comando1;
			//pointEndTx = &Bloque_handshake[comando2];	// &comando2;
			//blockSizeTX = 2;

			///flagsTX [3] = 1;				// evita que se Calcule y se mande checksum
			//keyTx = 0x55;					// listo para mandar transmisión
		 }
		// Inicia preambulo para password
		//if (memcmp(SerialAnswBLE, "\x40\x60",2) == 0){
		if (memcmp(SerialAnswBLE, "\x40\x70",2) == 0){
		    asm ("nop");
			PasswordALG = RndNumber;
			ImberaProtocolBuffer[0] = (char)(PasswordALG);   // RGM_29/NOV/2023 se manda sin complemento
			sizeTX = 1;  // 4;
			SerializeString2(ImberaProtocolBuffer, sizeTX);
		 }
		// Step2 Comprobacion MAC para password
		//if (memcmp(SerialAnswBLE, "\x40\x61",2) == 0){
		if (memcmp(SerialAnswBLE, "\x40\x71",2) == 0){
			uint8_t foo,foo2;
		    asm ("nop");
			foo = PasswordALG & (~0xF0);
			if (foo >0x0B)
			  foo = PasswordALG & (~0xF8);
			foo2 = Bloque_handshake [foo+2] + (~PasswordALG);		// foo2 = macAdress [foo] + (~PasswordALG);
			if (SerialAnswBLE [2] == foo2){
				DevLock = 0x72;    // RGM_29/NOV/2023Dispositivo Desbloqueado
				ImberaProtocolBuffer[0] = 0xF1;							ImberaProtocolBuffer[1] = 0x3D;					sizeTX = 2;
				SerializeString2(ImberaProtocolBuffer, sizeTX);
			}
			else{
				ImberaProtocolBuffer[0] = 0xF1;							ImberaProtocolBuffer[1] = 0x3E;  				sizeTX = 2;
				SerializeString2(ImberaProtocolBuffer, sizeTX);
			}
		}

		//if ( (DevLock == 0x72) || (timeUnlockWIFI) ){			// Dispositivo desbloqieado por password ?
		if ( (DevLock == 0x72) || (statComWIFIFlag) ){
		  if (memcmp(SerialAnswBLE, "\x40\x60",2) == 0)			//"Lectura de datos tipo TIEMPO\n\r"
			 codeTX = 0x60;
		  if (memcmp(SerialAnswBLE, "\x40\x61",2) == 0)			//"Lectura de datos tipo EVENTO\n\r"
			 codeTX = 0x61;
		  if (memcmp(SerialAnswBLE, "\x40\x53",2) == 0)			//"Estado en TIEMPO REAL\n\r"
			 codeTX = 0x53;
		  if (memcmp(SerialAnswBLE, "\x40\x50",2) == 0)			//"MODIFICACION de parametros de operacion\n\r"
			 codeTX = 0x50;
		  if (memcmp(SerialAnswBLE, "\x40\x51",2) == 0)			//"LECTURA de parametros de operacion\n\r"
			 codeTX = 0x51;
		  if (memcmp(SerialAnswBLE, "\x40\x46",2) == 0)			//"Modificacion de FIRMWARE\n\r"
			 codeTX = 0x46;
		  if (memcmp(SerialAnswBLE, "\x40\x49",2) == 0)			//"Modificacion de FIRMWARE\n\r"
			 codeTX = 0x49;
		  if (memcmp(SerialAnswBLE, "\x40\x4A",2) == 0)			//"Modificacion de FIRMWARE\n\r"
			 codeTX = 0x4A;
		  if (memcmp(SerialAnswBLE, "\x40\x54",2) == 0)			// código de limpieza de logger
			 codeTX = 0x54;
	   	  if (memcmp(SerialAnswBLE, "\x40\x80",2) == 0)			//"Modificacion de FIRMWARE\n\r"
	   		 codeTX = 0x80;
	   	  if (memcmp(SerialAnswBLE, "\xF1\x3D",2) == 0)			// RESPUESTA ENVÍO DE LOGGER MODULO WIFI
	   		 codeTX = 0x3D;
		  if (memcmp(SerialAnswBLE, "\x40\x55",2) == 0)			// Grabación de parámetros especificos
			 codeTX = 0x55;
	   	  if (memcmp(SerialAnswBLE, "\x40\x56",2) == 0)			// Ejecucuión de estados
	   		 codeTX = 0x56;
	   	  if (memcmp(SerialAnswBLE, "\x40\x57",2) == 0)			// lectura de estados
	   		 codeTX = 0x57;
	   	  if (memcmp(SerialAnswBLE, "\x40\x58",2) == 0)			// CÓDIGO DE TIEMPO UNIX
	   		 codeTX = 0x58;
	   	  if (memcmp(SerialAnswBLE, "\x40\x59",2) == 0)			// CÓDIGO DE ENVÍO DE GEOLOCALIZACIÓN
	   		 codeTX = 0x59;
	      if (memcmp(SerialAnswBLE, "\x40\x5A",2) == 0)			// CÓDIGO DE LECTURA DE GEOLOCALIZACION
	    	 codeTX = 0x5A;
	   	  if (memcmp(SerialAnswBLE, "\x40\x5B",2) == 0)			// CÓDIGO DE LECTURA DE TIEMPO UNIX
	   		 codeTX = 0x5B;
	   	  if (memcmp(SerialAnswBLE, "\x40\x5C",2) == 0)			// CÓDIGO DE TIEMPO BCD
		 	 codeTX = 0x5C;
	   	  if (memcmp(SerialAnswBLE, "\x40\x5F",2) == 0)			// CÓDIGO DE INFORMACION DE RELOJ
			  codeTX = 0x5F;
	   	  if (memcmp(SerialAnswBLE, "\x40\x62",2) == 0)	 		// CÓDIGO DE logger tiempo wifi
	   		  codeTX = 0x62;
	   	  if (memcmp(SerialAnswBLE, "\x40\x63",2) == 0)		 	// CÓDIGO DE logger eventos wifi
	   		  codeTX = 0x63;
	   	}// close  if (DevLock == 0x72)

	} //Close if (receivecomplete_Ble)


	if ( ((millis() - TestMessMilis ) >= 8)){     // ya paso 3 segundos
		TestMessMilis = millis();

		if (keyTx == 0x55){
		    asm ("nop");
    //	#pragma asm
		    tx_buffer_prep ();
    //	#pragma endasm
			}
		if (keyTx == 0x55){
		    asm ("nop");
			SerializeString2(ImberaProtocolBuffer, sizeTX);
			i_++;
		}
	}

}



//------------------------------------------------------------------------------------------------
void	Read_Inpunts_ble()
{
	if (tick_1ms == 1)						//Manuel., checa el tick, cada 1ms
		{
			// Manuel, lee la linea de entrada 1 "STAT"
			if	(HAL_GPIO_ReadPin(GPIOB,PFULLDEF_STAT_VB))
				{
					if (CntDebInp1 <= 50)
						CntDebInp1++;
					if (CntDebInp1==50)
						{
							device_conected = 1;
							CntDebInp1 = 51;
							statComFlag = 255;
						}
				}
			else
			{
//					CntDebInp1 = 0;				// contador debounce Boton 1
//					device_conected = 0;
//					DevLock = 0;    //RGM_29/NOV/2023     //RM_20240304 Para agregar PASSWORD de seguridad BLE
				if (CntDebInp1 >= 1)
					CntDebInp1--;
				if (CntDebInp1==1)
				{
					device_conected = 0;
					CntDebInp1 = 0;
					statComFlag = 0;
				}
			}
		}
}

//--------------------------------------------------------------------------------------------------------
void WaitSerAnsw_Ble_func (uint8_t valParamItem)
{
	WaitSerAnsw_Ble = 0;
	ParamItem = valParamItem;
}

void grabacion_exitosa_handshake (void)
{
	Bloque_handshake[comando1] =	0xF1;	//mov		comando1,#$F1
	Bloque_handshake[comando2] =	0x3D;	//mov		comando2,#$3D;				/ indica que la grabación fue exitosa
}

void manda_transmision (void)
{
	pointTx = &Bloque_handshake[comando1];
	pointInitTx = &Bloque_handshake[comando1];
	pointEndTx = &Bloque_handshake [comando2];
	blockSizeTX = 2;
	flagsTX [3] = 1;
	keyTx = 0x55;						// listo para mandar transmisión
	codeTX = 0x00;

}


