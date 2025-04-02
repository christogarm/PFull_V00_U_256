



#include "main.h"
#include "board_PFULLDEF.h"
#include "customMain.h"
#include "ModbusMap.h"
#include "ModbusRTU.h"
#include "ELTEC_EmulatedEEPROM.h"

//-------------------------------------------------------
static void MacStringtoHex(){
	int i;
	uint8_t tempregister =0;
	uint8_t octeto =0;
	uint16_t tempregister2 =0;

	for (i=0; i<=11;i++){
		if (Bloque_handshake[i+2] == '0')  //if (macAdress[i] == '0')
			tempregister = 0x00;
		if (Bloque_handshake[i+2] == '1')		 //if (macAdress[i] == '1')
			tempregister = 0x01;
		if (Bloque_handshake[i+2] == '2')		 //if (macAdress[i] == '2')
			tempregister = 0x02;
		if (Bloque_handshake[i+2] == '3')		//if (macAdress[i] == '3')
			tempregister = 0x03;
		if (Bloque_handshake[i+2] == '4')		//if (macAdress[i] == '4')
			tempregister = 0x04;
		if (Bloque_handshake[i+2] == '5')		//if (macAdress[i] == '5')
			tempregister = 0x05;
		if (Bloque_handshake[i+2] == '6')		//if (macAdress[i] == '6')
			tempregister = 0x06;
		if (Bloque_handshake[i+2] == '7')		//if (macAdress[i] == '7')
			tempregister = 0x07;
		if (Bloque_handshake[i+2] == '8')		//if (macAdress[i] == '8')
			tempregister = 0x08;
		if (Bloque_handshake[i+2] == '9')		//if (macAdress[i] == '9')
			tempregister = 0x09;
		if (Bloque_handshake[i+2] == 'A')		//if (macAdress[i] == 'A')
			tempregister = 0x0A;
		if (Bloque_handshake[i+2] == 'B')		//if (macAdress[i] == 'B')
			tempregister = 0x0B;
		if (Bloque_handshake[i+2] == 'C')		//if (macAdress[i] == 'C')
			tempregister = 0x0C;
		if (Bloque_handshake[i+2] == 'D')		//if (macAdress[i] == 'D')
			tempregister = 0x0D;
		if (Bloque_handshake[i+2] == 'E')		//if (macAdress[i] == 'E')
			tempregister = 0x0E;
		if (Bloque_handshake[i+2] == 'F')		//if (macAdress[i] == 'F')
			tempregister = 0x0F;

    if ((i % 2) == 0)
		octeto =  tempregister << 4;
	else{
		octeto = octeto | tempregister;
			if( i == 1 ||  i == 5 || i == 9)
				tempregister2 = octeto << 8;
			if (i == 3 || i == 7 || i == 11)
				tempregister2 = tempregister2 | (uint16_t) octeto;

			if( i == 3 )
				DatosMAC [0] = tempregister2;
			if( i == 7 )
				DatosMAC [1] = tempregister2;
			if( i == 11 )
				DatosMAC [2] = tempregister2;
		}
	}
}
//*********************************************************************************************
_Bool  ReadHoldingRegistersFunctionRev (uint16_t  ReadHoldingRegisters_AddressBegin	,uint16_t ReadHoldingRegisters_AddressEnd, uint16_t *ArrydHolding){

	uint8_t	GroupQtyRequestCapacity = 0;      //Manuel 06-dic-2021: Capacidad de la Cantidad de registros Requeridos
	_Bool responseOk = 0;
	if(_u16ReadAddress >= ReadHoldingRegisters_AddressBegin && _u16ReadAddress <= ReadHoldingRegisters_AddressEnd){    // Peticion, para "Eventos"
	  responseOk = 1;
		GroupQtyRequestCapacity = (uint8_t)(ReadHoldingRegisters_AddressEnd - _u16ReadAddress + 1);
		// Manuel 06-dic-2021: La cantidad pedida es menor o igual que la cantidad del grupo en funcion del Address del primer Registro
		if (_u16ReadQty <= GroupQtyRequestCapacity){
				uint8_t	i = 0;      //Manuel 06-dic-2021: iterator
				for (i = 0; i < _u16ReadQty; i++ )
					ModbusSlave_setTransmitBuffer (i, ArrydHolding[_u16ReadAddress - ReadHoldingRegisters_AddressBegin + i]);
				ModbusSlaveTransaction (u8MBFunctionSlave);
		}else
			ModbusSlaveExceptionTransaction(ILLEGAL_DATA_VALUE);				// manda el codigo de excepcion 2			ModbusSlaveExceptionTransaction(0x02);				// manda el codigo de excepcion 2

	}
 return responseOk;
}
//*********************************************************************************************
//*********************************************************************************************

void ModbusMap(void){

 uint8_t	modbusSlaveState = 0;					// Estado del Esclavo Modbus

//*********************************************************************************************
//****************  Registros Imbera Control
//*********************************************************************************************

	// Plantilla Dummy de escritura de registros Eventos
 	 if(!((flagsa[3]) | (flagsC[1]) | (flagsC[2]))){// if(!(((flagsa & 0x08) >> 3) | ((flagsC & 0x02) >> 1) | ((flagsC & 0x04) >> 2))){					//	if(!(((flagsa & 0x08)>> 3) || ((flagsC & 0x02)>> 1) || ((flagsC & 0x04)>> 2))){
		Eventos[0]  = (uint16_t) 0;       //14-Ene/2022		Estado de operacion : Normal = 0
	}
 	else if(flagsC[1])// else if(GetRegFlagState(flagsC, 1))		// ES1 ?			//	else if(((flagsC & 0x02)>> 1) == 1)		// ES1 ?
		Eventos[0]  = (uint16_t) 1;
	else if(flagsC[1])// else if(GetRegFlagState(flagsC, 1))		// ES2 ?					//  else if(((flagsC & 0x02)>> 1) == 1)		// ES2 ?
		Eventos[0]  = (uint16_t) 2;
	else if(flagsa[3])		// Nocturno ?				// else if(((flagsa & 0x08)>> 3) == 1)		// Nocturno ?
		Eventos[0]  = (uint16_t) 6;
    Eventos[1]  = (uint16_t) (GPIOR0[1]);			//((GPIOR0 & 0x02)>> 1);       //14-Ene/2022		Deshielo
	Eventos[2]  = (uint16_t) (GPIOR1[0]);			//(GPIOR1 & 0x01);       //14-Ene/2022		Ventilador
    Eventos[3]  = (uint16_t) (GPIOR0[2]); 			//((GPIOR0 & 0x04)>> 2);       //14-Ene/2022		Iluminacion
	Eventos[4]  = (uint16_t) (GPIOR0[0]);			//(GPIOR0 & 0x01);			  //14-Ene/2022		Compresor
	Eventos[5]  = (uint16_t) (flagsC[1]);// Eventos[5]  = (uint16_t) (flagsC & 0x01);      	//14-Ene/2022		Estado de puerta : CERRADA=0, ABIERTA=1				//		Eventos[5]  = (uint16_t) (flagsC & 0x01);      	//14-Ene/2022		Estado de puerta : CERRADA=0, ABIERTA=1
    Eventos[6]  = (uint16_t) 0;				//14-Ene/2022		Relevador sensor humedad
	if (Eventos[0]  == 0)
		Eventos[7]  = (uint16_t) 1;				//14-Ene/2022		Modo Normal
	else
		Eventos[7]  = (uint16_t) 0;				//14-Ene/2022		Modo Normal
	Eventos[8]  = (uint16_t) ((flagsC[1]));// Eventos[8]  = (uint16_t) ((flagsC & 0x02)>> 1);				//14-Ene/2022		Modo ES1 = Modo de ahorro 1		//	Eventos[8]  = (uint16_t) ((flagsC & 0x02)>> 1);				//14-Ene/2022		Modo ES1 = Modo de ahorro 1
	Eventos[9]  = (uint16_t) ((flagsC[2]));// Eventos[9]  = (uint16_t) ((flagsC & 0x04)>> 2);				//14-Ene/2022		Modo ES2 = Modo de ahorro 2		//	Eventos[9]  = (uint16_t) ((flagsC & 0x04)>> 2);				//14-Ene/2022		Modo ES2 = Modo de ahorro 2
	Eventos[10]  = (uint16_t) 0;			//14-Ene/2022		Modo OFF
	Eventos[11]  = (uint16_t) 0;			//14-Ene/2022		Modo Alarma
	Eventos[12]  = (uint16_t) flagsa[3];	// Eventos[12]  = (uint16_t) ((flagsa & 0x08)>> 3);			//14-Ene/2022		Modo Nocturno			//	Eventos[12]  = (uint16_t) ((flagsa & 0x08)>> 3);			//14-Ene/2022		Modo Nocturno
	Eventos[13]  = (uint16_t) 0;			//14-Ene/2022		Modo Vault
	// Plantilla Dummy de escritura de registros Mediciones
	Mediciones[0] = (tdevl *256) + tdevf;		//14-Ene/2022		Temp interna
	Mediciones[1] = (teval *256) + tevaf;		//14-Ene/2022		Temp evaporador
	Mediciones[2] = (uint16_t) voltl*10;  //1200;	//14-Ene/2022		Voltaje
	Mediciones[3] = (uint16_t) 220;		//14-Ene/2022		Temp externa 220/10= 22 grados
	// Plantilla Dummy de escritura de registros Alarmas
	Alarmas[0]  = (uint16_t)  trefst[7];       								//14-Ene/2022		Alarma U						//  Alarmas[0]  = (uint16_t) ((trefst & 0x80)>> 7);       //14-Ene/2022		Alarma U
	Alarmas[1]  = (uint16_t)  trefst[6];       								//14-Ene/2022		Alarma -U						//	Alarmas[1]  = (uint16_t) ((trefst & 0x40)>> 6);       //14-Ene/2022		Alarma -U
	Alarmas[2]  = (uint16_t)  trefst[4];       								//14-Ene/2022		Alarma P						//	Alarmas[2]  = (uint16_t) ((trefst & 0x10)>> 4);       //14-Ene/2022		Alarma P
	Alarmas[3]  = (uint16_t) (trefst[2] | trefst[3]);   //Alarma H Evaporador								//	Alarmas[3]  = (uint16_t) (((trefst & 0x04)>> 2) || ((trefst & 0x08)>> 3));
	Alarmas[4]  = (uint16_t) 0;			 														//14-Ene/2022		Alarma Congelamiento
	Alarmas[5]  = (uint16_t) (trefst[0] | trefst[1]);      								//14-Ene/2022		Alarma H Interna				//  Alarmas[5]  = (uint16_t) ((trefst & 0x01) || ((trefst & 0x02)>> 1));      	//14-Ene/2022		Alarma H Interna
	Alarmas[6]  = (uint16_t) trefst2[1];														//14-Ene/2022		Alarma Compresor  RM_20220711	//	Alarmas[6]  = (uint16_t) ((trefst2 & 0x02)>> 1);				//14-Ene/2022		Alarma Compresor  RM_20220711
	Alarmas[7]  = (uint16_t) trefst2[6];														//14-Ene/2022		Alarma A  RM_20220711			//  Alarmas[7]  = (uint16_t) ((trefst2 & 0x40)>> 6);				//14-Ene/2022		Alarma A  RM_20220711
	Alarmas[8]  = (uint16_t) (trefst[0] | trefst[1]);										//14-Ene/2022		Alarma H Externa				//  Alarmas[8]  = (uint16_t) ((trefst & 0x01) || ((trefst & 0x02)>> 1));				//14-Ene/2022		Alarma H Externa
	// Plantilla Dummy de escritura de registros Datos MAC
	DatosMAC[0] =  0xB4A2;		//14-Ene/2022		MAC B4:A2
	DatosMAC[1] =  0xEB4F;		//14-Ene/2022		MAC EB:4F
	DatosMAC[2] =  0x00FF;		//14-Ene/2022		MAC 00:FF
	MacStringtoHex();


    //DatosFirmware[0] =(uint16_t) ((eePlantilla[eeversion1]*256) + eePlantilla[eeversion2]);
	DatosFirmware[0] = reePlantilla[eeversion1]*256 + reePlantilla[eeversion2];

    //*********************************************************************************************
    //****************  Registros OXXO Control demo RW
    //*********************************************************************************************


    modbusSlaveState = ModbusSlavePool();

	if (modbusSlaveState == ku8MBSuccess){
	    asm ("nop");
		// OpcTestModbus =1;
		switch (u8MBFunctionSlave)
		{
			uint8_t	GroupQtyRequestCapacity = 0;      //Manuel 06-dic-2021: Capacidad de la Cantidad de registros Requeridos

			// 21-feb-2023	Manuel:  case ku8MBReadCoils:	break;
			// 21-feb-2023	Manuel:  case ku8MBReadDiscreteInputs: break;
			// 21-feb-2023	Manuel:  case ku8MBReadInputRegisters: break;  //Manuel 22-MAR-2002:
			//****************************  (0x03) Function Read Holding Registers  *****************************************
			//****************************  (0x03) Function Read Holding Registers  *****************************************
			case ku8MBReadHoldingRegisters:{
			    asm ("nop");

					PNU_0x3002 = 0;
					//if (eePlantilla[eeescala] == 32)									//Manuel 23-Mar-2023
					if(reePlantilla[eeescala] == 32)
					PNU_0x3002 = 1;

					//PNU_0x3004 = eePlantilla[eespdiur_H] * 256 + eePlantilla[eespdiur_L];    // eespdiur_w;       //Manuel 22-Mar-2022
					PNU_0x3004 = findLastValue((uint32_t) &eePlantilla[eespdiur_H]) * 256 + findLastValue((uint32_t) &eePlantilla[eespdiur_L]);

					//PNU_0x3006 = eePlantilla[eedifdiur_H] * 256 + eePlantilla[eedifdiur_L];	//eedifdiur_w;       //Manuel 23-Mar-2022
					PNU_0x3006 = reePlantilla[eedifdiur_H] * 256 + reePlantilla[eedifdiur_L];
					//PNU_0x301E  = (uint16_t) eePlantilla[eeAddModBus];       //Manuel 23-MAR-2022
					PNU_0x301E  = reePlantilla[eeAddModBus];
				  //PNU_0x3104 = (uint16_t) eePlantilla[eetimepa];     // Manuel 23/MAR/2022
					PNU_0x3104 = reePlantilla[eetimepa];

				//---------------      Imbera Control solo de Lectura  --------------------------------------------
				//---------------      Imbera Control solo de Lectura  --------------------------------------------
				// El Address se encuentra dentro del rango del Grupo
				// El Address se encuentra dentro del rango del Grupo  "Eventos"
				if (ReadHoldingRegistersFunctionRev (Eventos_AddressBegin	,Eventos_AddressEnd, Eventos) == 1 )
				  break;
				// El Address se encuentra dentro del rango del Grupo "Mediciones"
				else if(ReadHoldingRegistersFunctionRev (Mediciones_AddressBegin	,Mediciones_AddressEnd, Mediciones) == 1 )
					break;
				// El Address se encuentra dentro del rango del Grupo "Alarmas"
				else if(ReadHoldingRegistersFunctionRev (Alarmas_AddressBegin	,Alarmas_AddressEnd, Alarmas) == 1 )
					break;
				// El Address se encuentra dentro del rango del Grupo "Datos MAC"
				else if(ReadHoldingRegistersFunctionRev (DatosMAC_AddressBegin	,DatosMAC_AddressEnd, DatosMAC) == 1 )
					break;
				// El Address se encuentra dentro del rango del Grupo "DatosFirmware"
				else if(ReadHoldingRegistersFunctionRev (DatosFirmware_AddressBegin	,DatosFirmware_AddressEnd, DatosFirmware) == 1 )
					break;
				// El Address test, contador de Prueba ***********
				else if(ReadHoldingRegistersFunctionRev (TestAddress_AddressBegin	,TestAddress_AddressEnd, &Count_Test) == 1 )
					break;
				//------------------------------------      OXXO Control demo RW						------------------------------------
				//------------------------------------      OXXO Control demo RW						------------------------------------
				// ID del cliente del refrigerador   "ID del cliente del refrigerador"
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3000_AddressBegin	,PNU_0x3000_AddressEnd, &PNU_0x3000) == 1 )
					break;
				// Unidad de temperatura  "Unidad de temperatura"
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3002_AddressBegin	,PNU_0x3002_AddressEnd, &PNU_0x3002) == 1 ){
			    //PNU_0x3002 = 0;
					//if (eeescala == 32)									//Manuel 23-Mar-2023
					//PNU_0x3002 = 1;
					break;
				}
				// Temperatura Setpoint MODO NORMAL           ******* Parametro de lectura completo
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3004_AddressBegin	,PNU_0x3004_AddressEnd, &PNU_0x3004) == 1 ){
					//PNU_0x3004 = eespdiur_w;       //Manuel 22-Mar-2022
					break;
				}
				// Temperatura Diferencial  MODO NORMAL
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3006_AddressBegin	,PNU_0x3006_AddressEnd, &PNU_0x3006) == 1 ){
					//PNU_0x3006 = eedifdiur_w;       //Manuel 23-Mar-2022
					break;
				}
				// MODO NOCTURNO: duración en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3008_AddressBegin	,PNU_0x3008_AddressEnd, &PNU_0x3008) == 1 ){
					break;
				}
				// Consumo por variable
				else if(ReadHoldingRegistersFunctionRev (PNU_0x300A_AddressBegin	,PNU_0x300A_AddressEnd, &PNU_0x300A) == 1 ){
					break;
				}
				// Compresor: tiempo minimo de apagado en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x300C_AddressBegin	,PNU_0x300C_AddressEnd, &PNU_0x300C) == 1 ){
					break;
				}
				// Compresor: tiempo minimo de encendido en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x300E_AddressBegin	,PNU_0x300E_AddressEnd, &PNU_0x300E) == 1 ){
					break;
				}
				// Deshielo Programa 1: habilitación de 3 o 5 fases
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3010_AddressBegin	,PNU_0x3010_AddressEnd, &PNU_0x3010) == 1 ){
					break;
				}
				// Deshielo Programa 1: duración fase 1 en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3012_AddressBegin	,PNU_0x3012_AddressEnd, &PNU_0x3012) == 1 ){
					break;
				}
				// Deshielo Programa 1: duración fase 2 en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3014_AddressBegin	,PNU_0x3014_AddressEnd, &PNU_0x3014) == 1 ){
					break;
				}
				// Deshielo Programa 1: duración fase 3 en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3016_AddressBegin	,PNU_0x3016_AddressEnd, &PNU_0x3016) == 1 ){
					break;
				}
				// Deshielo Programa 1: periodo en mins para iniciar deshielo
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3018_AddressBegin	,PNU_0x3018_AddressEnd, &PNU_0x3018) == 1 ){
					break;
				}
				// Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo
				else if(ReadHoldingRegistersFunctionRev (PNU_0x301A_AddressBegin	,PNU_0x301A_AddressEnd, &PNU_0x301A) == 1 ){
					break;
				}
				// Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo
				else if(ReadHoldingRegistersFunctionRev (PNU_0x301C_AddressBegin	,PNU_0x301C_AddressEnd, &PNU_0x301C) == 1 ){
					break;
				}
				// Modbus: dirección de la CIR
				else if(ReadHoldingRegistersFunctionRev (PNU_0x301E_AddressBegin	,PNU_0x301E_AddressEnd, &PNU_0x301E) == 1 ){
					//PNU_0x301E  = (uint16_t) eeAddModBus;       //Manuel 23-MAR-2022
					break;
				}
				// Alarma puerta abierta: tiempo de activación en mins
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3104_AddressBegin	,PNU_0x3104_AddressEnd, &PNU_0x3104) == 1 ){
					//PNU_0x3104 = (uint16_t) eetimepa;     // Manuel 23/MAR/2022
					break;
				}
				// Alarma Congelamiento Programa 1: temperatura para entrar en alarma
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3106_AddressBegin	,PNU_0x3106_AddressEnd, &PNU_0x3106) == 1 ){
					break;
				}
				// Alarma U 110V: voltaje para entrar en alarma
				else if(ReadHoldingRegistersFunctionRev (PNU_0x3108_AddressBegin	,PNU_0x3108_AddressEnd, &PNU_0x3108) == 1 ){
					break;
				}
				// Alarma U 110V: voltaje para salir de alarma
				else if(ReadHoldingRegistersFunctionRev (PNU_0x310A_AddressBegin	,PNU_0x310A_AddressEnd, &PNU_0x310A) == 1 ){
					break;
				}
				// Alarma -U 110V: voltaje para entrar en alarma
				else if(ReadHoldingRegistersFunctionRev (PNU_0x310C_AddressBegin	,PNU_0x310C_AddressEnd, &PNU_0x310C) == 1 ){
					break;
				}
				// Alarma -U 110V: voltaje para salir de alarma
				else if(ReadHoldingRegistersFunctionRev (PNU_0x310E_AddressBegin	,PNU_0x310E_AddressEnd, &PNU_0x310E) == 1 ){
					break;
				}
				else{
					ModbusSlaveExceptionTransaction(ILLEGAL_DATA_ADDRESS);				// manda el codigo de excepcion 2			ModbusSlaveExceptionTransaction(0x02);				// manda el codigo de excepcion 2
				}
				break;
		  }	// cierra el case
			//****************************  (0x06) Function Write Single Register  *****************************************
			//****************************  (0x06) Function Write Single Register  *****************************************
			case	ku8MBWriteSingleRegister:{
				// ID del cliente del refrigerador
				if(_u16WriteAddress >= PNU_0x3000_AddressBegin && _u16WriteAddress <= PNU_0x3000_AddressEnd){    // Contador de prueba
					PNU_0x3000 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Unidad de temperatura
				if(_u16WriteAddress >= PNU_0x3002_AddressBegin && _u16WriteAddress <= PNU_0x3002_AddressEnd){    // Contador de prueba
					PNU_0x3002 = lowByte(_u16WriteVal);
					if (PNU_0x3002 == 1){
						//if((uint16_t) (eePlantilla[eeescala] == 32)){
						if(reePlantilla[eeescala] == 32){
							asm ("nop");
						}
						else{
							//Cambia a C
							waux = 32;	// mov			waux,#32;	/ Carga
							// ldw			X,#eeescala; / Escribe en EEPROM
							wreeprom(waux , &eePlantilla[eeescala]);// call
							reePlantilla[eeescala] = waux;
			//reset_escala1:
							while(1);//goto reset_escala1;// jra    reset_escala1
						}
					}
					if (PNU_0x3002 == 0){
						//if((uint16_t) (eePlantilla[eeescala] == 32)){
						if(reePlantilla[eeescala]== 32){
							waux = 0;//mov			waux,#0;	/ Carga
							// ldw			X,#eeescala; / Escribe en EEPROM
							wreeprom(waux , &eePlantilla[eeescala]);// call		wreeprom
							reePlantilla[eeescala] = waux;
			//reset_escala:
							while(1);//goto reset_escala; //jra    reset_escala
						}
						else{
							asm ("nop");
						}
					}
					//reset_param = TRUE;                      //RM_20230425 Responder antes de RESET
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Temperatura Setpoint MODO NORMAL
				if(_u16WriteAddress >= PNU_0x3004_AddressBegin && _u16WriteAddress <= PNU_0x3004_AddressEnd){    //
					PNU_0x3004 = _u16WriteVal;

					waux = highByte (PNU_0x3004);
					wreg = lowByte(PNU_0x3004);

					//ldw			X,#eespdiur_w; / Escribe en EEPROM
					//call		wreeprom
					//mov			waux,wreg;	/ Carga
					//incw    X
					//call		wreeprom
					wreeprom(waux,&eePlantilla[eespdiur_H]);
					wreeprom(wreg,&eePlantilla[eespdiur_L]);

					reePlantilla[eespdiur_H] = waux;
					reePlantilla[eespdiur_L] = wreg;

			//reset_escala_0x3004:
					while(1);//jra    reset_escala_0x3004 Descomentar cuando ya se tenga el watchDog

				  //reset_param = TRUE;      //RM_20230425 Responder antes de RESET
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Temperatura Diferencial  MODO NORMAL
				if(_u16WriteAddress >= PNU_0x3006_AddressBegin && _u16WriteAddress <= PNU_0x3006_AddressEnd){    //
					PNU_0x3006 = _u16WriteVal;
					waux = highByte (PNU_0x3006);
					wreg = lowByte(PNU_0x3006);
					// ldw			X,#eedifdiur_w; / Escribe en EEPROM
					// call		wreeprom
					//mov			waux,wreg;	/ Carga
					//incw    X
					//call		wreeprom
					wreeprom(waux,&eePlantilla[eedifdiur_H]);
					wreeprom(wreg,&eePlantilla[eedifdiur_L]);

					reePlantilla[eedifdiur_H] = waux;
					reePlantilla[eedifdiur_L] = wreg;

			//reset_escala_0x3006:
					while(1);//jra    reset_escala_0x3006

					//reset_param = TRUE;      //RM_20230425 Responder antes de RESET
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// MODO NOCTURNO: duración en mins
				if(_u16WriteAddress >= PNU_0x3008_AddressBegin && _u16WriteAddress <= PNU_0x3008_AddressEnd){    //
					PNU_0x3008 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Consumo por variable
				if(_u16WriteAddress >= PNU_0x300A_AddressBegin && _u16WriteAddress <= PNU_0x300A_AddressEnd){    //
					PNU_0x300A = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Compresor: tiempo minimo de apagado en mins
				if(_u16WriteAddress >= PNU_0x300C_AddressBegin && _u16WriteAddress <= PNU_0x300C_AddressEnd){    //
					PNU_0x300C = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Compresor: tiempo minimo de encendido en mins
				if(_u16WriteAddress >= PNU_0x300E_AddressBegin && _u16WriteAddress <= PNU_0x300E_AddressEnd){    //
					PNU_0x300E = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: habilitación de 3 o 5 fases
				if(_u16WriteAddress >= PNU_0x3010_AddressBegin && _u16WriteAddress <= PNU_0x3010_AddressEnd){    // Contador de prueba
					PNU_0x3010 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: duración fase 1 en mins
				if(_u16WriteAddress >= PNU_0x3012_AddressBegin && _u16WriteAddress <= PNU_0x3012_AddressEnd){    // Contador de prueba
					PNU_0x3012 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: duración fase 2 en mins
				if(_u16WriteAddress >= PNU_0x3014_AddressBegin && _u16WriteAddress <= PNU_0x3014_AddressEnd){    // Contador de prueba
					PNU_0x3014 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: duración fase 3 en mins
				if(_u16WriteAddress >= PNU_0x3016_AddressBegin && _u16WriteAddress <= PNU_0x3016_AddressEnd){    // Contador de prueba
					PNU_0x3016 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: periodo en mins para iniciar deshielo
				if(_u16WriteAddress >= PNU_0x3018_AddressBegin && _u16WriteAddress <= PNU_0x3018_AddressEnd){    //
					PNU_0x3018 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: temperatura para inicio de periodo de deshieloDeshielo Programa 1: temperatura para inicio de periodo de deshielo
				if(_u16WriteAddress >= PNU_0x301A_AddressBegin && _u16WriteAddress <= PNU_0x301A_AddressEnd){    // Contador de prueba
					PNU_0x301A = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Deshielo Programa 1: temperatura para salir de deshielo  y verifica si entra a deshielo
				if(_u16WriteAddress >= PNU_0x301C_AddressBegin && _u16WriteAddress <= PNU_0x301C_AddressEnd){    // Contador de prueba
					PNU_0x301C = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Modbus: dirección de la CIR
				if(_u16WriteAddress >= PNU_0x301E_AddressBegin && _u16WriteAddress <= PNU_0x301E_AddressEnd){    // Contador de prueba
					PNU_0x301E = lowByte(_u16WriteVal);

					waux = (uint8_t) PNU_0x301E;

					//ldw			X,#eeAddModBus; / Escribe en EEPROM
					wreeprom(waux,&eePlantilla[eeAddModBus]);//call		wreeprom
					reePlantilla[eeAddModBus] = waux;

			//reset_escala_0x301E:
					while(1);//jra    reset_escala_0x301E


					//reset_param = TRUE;      //RM_20230425 Responder antes de RESET
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma puerta abierta: tiempo de activación en mins
				if(_u16WriteAddress >= PNU_0x3104_AddressBegin && _u16WriteAddress <= PNU_0x3104_AddressEnd){    //
					PNU_0x3104 = lowByte(_u16WriteVal);

					waux = (uint8_t) PNU_0x3104;

					//ldw			X,#eetimepa; / Escribe en EEPROM
					wreeprom(waux,&eePlantilla[eetimepa]);//call		wreeprom
					reePlantilla[eetimepa] = waux;
			//reset_escala_0x3104:
					while(1);// jra    reset_escala_0x3104


					//reset_param	= TRUE;      //RM_20230425 Responder antes de RESET
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma Congelamiento Programa 1: temperatura para entrar en alarma
				if(_u16WriteAddress >= PNU_0x3106_AddressBegin && _u16WriteAddress <= PNU_0x3106_AddressEnd){    //
					PNU_0x3106 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma U 110V: voltaje para entrar en alarma
				if(_u16WriteAddress >= PNU_0x3108_AddressBegin && _u16WriteAddress <= PNU_0x3108_AddressEnd){    //
					PNU_0x3108 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma U 110V: voltaje para salir de alarma
				if(_u16WriteAddress >= PNU_0x310A_AddressBegin && _u16WriteAddress <= PNU_0x310A_AddressEnd){    //
					PNU_0x310A = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma -U 110V: voltaje para entrar en alarma
				if(_u16WriteAddress >= PNU_0x310C_AddressBegin && _u16WriteAddress <= PNU_0x310C_AddressEnd){    //
					PNU_0x310C = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Alarma -U 110V: voltaje para salir de alarma
				if(_u16WriteAddress >= PNU_0x310E_AddressBegin && _u16WriteAddress <= PNU_0x310E_AddressEnd){    //
					PNU_0x310E = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando activar Modo Nocturno
				if(_u16WriteAddress >= PNU_0x3200_AddressBegin && _u16WriteAddress <= PNU_0x3200_AddressEnd){    //
					PNU_0x3200 = lowByte(_u16WriteVal);
					//08/FEB/2022		DeshieloH_T =lowByte(_u16WriteVal);   // 24/ENE/2022
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando activar MODO DE AHORRO ES2
				if(_u16WriteAddress >= PNU_0x3202_AddressBegin && _u16WriteAddress <= PNU_0x3202_AddressEnd){    //
					PNU_0x3202 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando activar MODO VAULT
				if(_u16WriteAddress >= PNU_0x3204_AddressBegin && _u16WriteAddress <= PNU_0x3204_AddressEnd){    //
					PNU_0x3204 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando desactivar MODO VAULT
				if(_u16WriteAddress >= PNU_0x3206_AddressBegin && _u16WriteAddress <= PNU_0x3206_AddressEnd){    //
					PNU_0x3206 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando activar MODO Servicio
				if(_u16WriteAddress >= PNU_0x3208_AddressBegin && _u16WriteAddress <= PNU_0x3208_AddressEnd){    //
					PNU_0x3208 = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando Ventilador siempre ON con compresor OFF
				if(_u16WriteAddress >= PNU_0x320A_AddressBegin && _u16WriteAddress <= PNU_0x320A_AddressEnd){    //
					PNU_0x320A = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando Ventilador activar ciclos ON-OFF con compresor OFF
				if(_u16WriteAddress >= PNU_0x320C_AddressBegin && _u16WriteAddress <= PNU_0x320C_AddressEnd){    //
					PNU_0x320C = lowByte(_u16WriteVal);
					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}


				// Comando  Manuel  07-MAR-2022: Activacion/Desactivacion Nocturno
				if(_u16WriteAddress >= AndyWRegisters_0x30_AddressBegin && _u16WriteAddress <= AndyWRegisters_0x30_AddressEnd){    //

					if (_u16WriteVal == 1){
						if(flagsa[3]){
							asm ("nop");
						}
						else{

						}
					}
					if (_u16WriteVal == 0){
						if(flagsa[3]){

						}
						else{
							asm ("nop");
						}
					}

					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}

				// Comando Manuel  07-MAR-2022: Activacion/Desactivacion Deshielo
				if(_u16WriteAddress >= AndyWRegisters_0x31_AddressBegin && _u16WriteAddress <= AndyWRegisters_0x31_AddressEnd){    // Contador de prueba
					//PNU_0x320C = lowByte(_u16WriteVal);

					if (_u16WriteVal == 1){
						if(GPIOR0[1]){
							asm ("nop");
						}
						else{
							flags_accMenu |=0x01;
							//botonst = botonst | 0x20;
							//	bset		botonst,#b3_f2
						}
					}
					if (_u16WriteVal == 0){
						if(GPIOR0[1]){
						}
						else{
							asm ("nop");
						}
					}

					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}
				// Comando Manuel  07-MAR-2022: Activacion/Desactivacion Lampara
				if(_u16WriteAddress >= AndyWRegisters_0x32_AddressBegin && _u16WriteAddress <= AndyWRegisters_0x32_AddressEnd){    // Contador de prueba

					if (_u16WriteVal == 1){
						if(GPIOR0[2]){
							asm ("nop");
						}
						else{
							flagsb[0] = 1;
							//	bset		botonst,#b3_f2
						}
					}
					if (_u16WriteVal == 0){
						if(GPIOR0[2]){
							asm ("nop");
							flagsb[0] = 0;// bres   flagsb,#0

						}
						else{
							asm ("nop");
						}
					}

					ModbusSlaveTransaction (u8MBFunctionSlave);		// Esto solo Gestiona La respuesta
				}
			break;
		  }

		  default:{                  // No entendio ninguna funcion
				ModbusSlaveExceptionTransaction(ILLEGAL_FUNCTION);      // manda el codigo de excepcion 1			ModbusSlaveExceptionTransaction(0x01);      // manda el codigo de excepcion 1
			}
		}

	}
}
//*************************************************************************************************


