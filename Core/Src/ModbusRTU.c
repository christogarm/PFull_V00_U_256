



#include "main.h"
#include "board_PFULLDEF.h"
#include "customMain.h"
#include "ModbusRTU.h"


_Bool receivecomplete_RS485 = 0;							// recepcion completa DMA

uint8_t  _u8MBSlave;                                         ///< Modbus slave (1..255) initialized in begin()
uint16_t _u16ReadAddress;                                    ///< slave register from which to read
uint16_t _u16ReadQty;                                        ///< quantity of words to read



uint16_t _u16WriteAddress;                                   ///< slave register to which to write

//LN_194:		#pragma space extern [] @near
uint16_t _u16WriteVal;                                       ///10-Ene-2022	< Valor a escribir
uint16_t _u16TransmitBuffer[ku8MaxBufferSize];               ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()

uint8_t _u8TransmitBufferIndex;
uint16_t u16TransmitBufferLength;



uint8_t u8MBFunctionSlave;




//------------------------------------------------------------------------------------------------------------------
//Completa--------------------------------
//Source_T: SerializeDMA.c
//------------------------------------------------------------------------------------------------------------------
uint8_t serialEvent_RS485 (void)
{
	//------------------------------------------------------------------------------------------------
		uint8_t u8ModbusADUSize =0;

		uint16_t sizeBufferRx = sizeof(RxBuffer_RS485);
		HAL_UART_Receive_DMA ( &huart4, RxBuffer_RS485, sizeBufferRx );

		uint16_t contadorRx = __HAL_DMA_GET_COUNTER(&hdma_usart4_rx );
		if( contadorRx < sizeBufferRx ){						// si cambio esta relacion, es porque ya llego algo al buffer

			//timeoutRx ++;
			//if( timeoutRx >= 17 )
			// 18-oct-2021  if( USART1->SR & USART_SR_IDLE )
			if( USART4->ISR & USART_ISR_IDLE ){
			// if(__HAL_UART_GET_FLAG(&handleUART1,UART_FLAG_IDLE))
				asm ("nop");

				// HAL_UART_DMAStop(&huart4);
				HAL_UART_AbortReceive(&huart4);


				if( (USART4->ISR & USART_ISR_IDLE)  )
					__HAL_UART_CLEAR_IDLEFLAG(&huart4);

				uint8_t i;
				for (i=0;i<(sizeof(RxBuffer_RS485)-contadorRx);i++)
					{
						SerialAnsModBus[i]=RxBuffer_RS485[i];
					}
				SerialAnsModBus[i]= 0;					// Add Null byte
				u8ModbusADUSize = sizeBufferRx - contadorRx;
				receivecomplete_RS485 = 1;
			}

		}
		return  u8ModbusADUSize;
}
//--------------------------------------- Serializacion de Strings -------------------------------------------------------
/*
 * //Source_T: SerializeDMA.c
*/
void   SerializeDMAArray_RS485 (uint8_t *tosend,uint8_t cantidadBytes)
{
		asm ("nop");
	    memcpy(TxBuffer_RS485, tosend, cantidadBytes);
		HAL_UART_Transmit_DMA(&huart4, TxBuffer_RS485, (uint16_t)cantidadBytes );

}
/*
Processor-independent CRC-16 calculation.
Polynomial: x^16 + x^15 + x^2 + 1 (0xA001)<br>
Initial value: 0xFFFF

This CRC is normally used in disk-drive controllers.

@param uint16_t crc (0x0000..0xFFFF)
@param uint8_t a (0x00..0xFF)
@return calculated CRC (0x0000..0xFFFF)
*/
//------------------------------------------------------------------------------------------------------------------
//Completa--------------------------------
// ModbusRTU.c
//------------------------------------------------------------------------------------------------------------------
static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
int i;

crc ^= a;
for (i = 0; i < 8; ++i)
{
if (crc & 1)
  crc = (crc >> 1) ^ 0xA001;
else
  crc = (crc >> 1);
}

return crc;
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
/** Linea: 267
Constructor initialize it using ModbusMaster
*/
void	Modbus_ModbusSalave (void){
  /*
	_idle = 0;
  _preTransmission = 0;
  _postTransmission = 0;
	*/
	timeClearComst;
	timeClearComst1;
	fcom;//???????
}
//------------------------------------------------------------------------------------------------------------------
/** Linea: 281
Initialize class object.

Assigns the Modbus slave ID and serial port.
Call once class has been instantiated, typically within setup().

@param slave Modbus slave ID (1..255)
@param &serial reference to serial port object (Serial, Serial1, ... Serial3)
@ingroup setup
*/
void ModbusMaster_begin(uint8_t slave)
{
//  txBuffer = (uint16_t*) calloc(ku8MaxBufferSize, sizeof(uint16_t));
  _u8MBSlave = slave;
  // Manuel 23-NOv-2021		_serial = &serial;
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;
/*
#if __MODBUSMASTER_DEBUG__
  pinMode(__MODBUSMASTER_DEBUG_PIN_A__, OUTPUT);
  pinMode(__MODBUSMASTER_DEBUG_PIN_B__, OUTPUT);
#endif
*/
}
//------------------------------------------------------------------------------------------------------------------
/** Linea: 306
Place data in transmit buffer.

@see ModbusMaster::clearTransmitBuffer()
@param u8Index index of transmit buffer array (0x00..0x3F)
@param u16Value value to place in position u8Index of transmit buffer (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup buffer
*/
uint8_t ModbusSlave_setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
{
  if (u8Index < ku8MaxBufferSize)
  {
    _u16TransmitBuffer[u8Index] = u16Value;
    return ku8MBSuccess;
  }
  else
  {
    return ku8MBIllegalDataAddress;
  }
}
//------------------------------------------------------------------------------------------------------------------
/** Linea: 328
Modbus transaction engine.
Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)

@param u8MBFunction Modbus function (0x01..0xFF)
@return 0 on success; exception number on failure
*/
void		ModbusSlaveTransaction (uint8_t u8MBFunction)
{

  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
	uint8_t i;
  uint16_t u16CRC;

	//Hablita la trasnmision 17-Dic-2021
    HAL_GPIO_WritePin(GPIOC, PFULLDEF_MAX485_DE_AND_RE_NEG, GPIO_PIN_SET);      //28-May-2024:  Habilita trasnmision por modbus


  // assemble Modbus Request Application Data Unit
  u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;						//Manuel 1-dic-2021, para todas las respuestas inician asi
  u8ModbusADU[u8ModbusADUSize++] = u8MBFunctionSlave;			//Manuel 1-dic-2021, para todas las respuestas inician asi

	switch(u8MBFunction)
  {

    case ku8MBReadInputRegisters: //Manuel 20-MAR-200:	break;
    case ku8MBReadHoldingRegisters:					//Manuel 1-dic-2021, Modbus function 0x03 Read Holding Registers
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty << 1);

      for (i = 0; i < lowByte(_u16ReadQty); i++)
      {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i]);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i]);
      }
		break;
		case ku8MBWriteSingleRegister:													//Manuel 10-Ene-2022 : Escritura de registro Simple
		  for (i = 2; i <= 5; i++){
				u8ModbusADU[u8ModbusADUSize++] = SerialAnsModBus[i];					//Manuel 10-Ene-2022 : para todas las respuestas inician asi
			}
		break;
  }


  // append CRC
  u16CRC = 0xFFFF;
  for (i = 0; i < u8ModbusADUSize; i++)
  {
    u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
  }
  u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize] = 0;       // Manuel, ver si borro este Quantity, me podria servir para el DMA

  // Manuel 06-dic-2021 : Transmite la respuesta al MASTER
	SerializeDMAArray_RS485 (u8ModbusADU,u8ModbusADUSize);

	// _ASM: Semaforo de comunicación verde
	timeClearComst = 70;               //Refresca el contador de tiempo


}
//------------------------------------------------------------------------------------------------------------------
// Respuestas de excepcion para Modbus
void		ModbusSlaveExceptionTransaction (uint8_t ExceptionCode)
{


  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
  uint8_t i;
  uint16_t u16CRC;

   asm ("nop");

	// Semaforo de comunicación amarillo
	timeClearComst1=20;		//		Carga tiempo con 20 segundos


	//Hablita la trasnmision 17-Dic-2021
    HAL_GPIO_WritePin(GPIOC, PFULLDEF_MAX485_DE_AND_RE_NEG, GPIO_PIN_SET);      //28-May-2024:  Habilita trasnmision por modbus

	u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;						//Manuel 1-dic-2021, para todas las respuestas inician asi
	u8ModbusADU[u8ModbusADUSize++] = (u8MBFunctionSlave | 0x80);			//Manuel 1-dic-2021, para todas las respuestas inician asi
	u8ModbusADU[u8ModbusADUSize++] = ExceptionCode;			//Manuel 1-dic-2021, para todas las respuestas inician asi

  // append CRC
  u16CRC = 0xFFFF;
  for (i = 0; i < u8ModbusADUSize; i++)
  {
    u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
  }
  u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize] = 0;       // Manuel, ver si borro este Quantity, me podria servir para el DMA


	// Manuel 06-dic-2021 : Transmite la respuesta al MASTER
	SerializeDMAArray_RS485 (u8ModbusADU,u8ModbusADUSize);

}
//------------------------------------------------------------------------------------------------------------------
//Completa--------------------------------   Comienza ModBus  Slave Pool  --------------------------------------------
//------------------------------------------------------------------------------------------------------------------
uint8_t 	ModbusSlavePool (void){


	uint8_t u8ModbusADU[256];
	uint8_t u8ModbusADUSize = 0;
	uint8_t i;
	uint16_t u16CRC;
	uint8_t u8MBStatus = 0xFF;

	// Leego algo por el Serial ???
	u8ModbusADUSize = serialEvent_RS485 ();
	asm ("nop");
	asm ("nop");

	if (receivecomplete_RS485){
		receivecomplete_RS485 = 0;
		u8MBStatus = ku8MBSuccess;
    // verify response is for correct Modbus slave
    if (SerialAnsModBus[0] == _u8MBSlave){			// Manuel 1-dic-2021, Matchea con el Address

			u8MBFunctionSlave = SerialAnsModBus[1];				//Manuel 30-Nov-2021

			// verify response is for correct Modbus function code (mask exception bit 7)
      if ((SerialAnsModBus[1] & 0x7F) != u8MBFunctionSlave)
      {
        u8MBStatus = ku8MBInvalidFunction;
        // Manuel: Rompia el While	break;
      }

			// calculate CRC
			u16CRC = 0xFFFF;
			for (i = 0; i < (u8ModbusADUSize - 2); i++){
				u16CRC = crc16_update(u16CRC, SerialAnsModBus[i]);
			}

			// verify CRC
			if (!u8MBStatus && (lowByte(u16CRC) != SerialAnsModBus[u8ModbusADUSize - 2] ||
      highByte(u16CRC) != SerialAnsModBus[u8ModbusADUSize - 1]))
			{
				u8MBStatus = ku8MBInvalidCRC;
			}

			// evaluate  Modbus function code
			switch(u8MBFunctionSlave)
			{
				case ku8MBReadCoils:	break;
				case ku8MBReadDiscreteInputs: break;
				case ku8MBReadInputRegisters: 					//Manuel 20-MAR-200:	break;
				case ku8MBReadHoldingRegisters:
					_u16ReadAddress = (SerialAnsModBus[2]*256) + SerialAnsModBus[3];				// Data Address Request
					_u16ReadQty = (SerialAnsModBus[4]*256) + SerialAnsModBus[5];						// Data Address Request
				break;
				case ku8MBWriteSingleRegister:           																	// Manuel 10-Ene-2022 : Nuevo case para la escritura
					_u16WriteAddress = (SerialAnsModBus[2]*256) + SerialAnsModBus[3];				// Data Address Request
					_u16WriteVal = (SerialAnsModBus[4]*256) + SerialAnsModBus[5];
				break;
			}

		}
		else{
			u8MBStatus = ku8MBInvalidSlaveID;

			// Semaforo de comunicación amarillo
			timeClearComst1 =20;		//		Carga tiempo con 20 segundos

		}
	}


return u8MBStatus;
}
//--------------------------------------------------------------------------------------------------


