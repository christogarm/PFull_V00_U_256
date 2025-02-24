


//*********************************************************************************************************************
//*********************************************** Macros de la Clase  ************************************************
//*********************************************************************************************************************
#define		ku8MBIllegalFunction            (uint8_t)0x01
#define		ku8MBIllegalDataAddress         (uint8_t)0x02
#define		ku8MBIllegalDataValue           (uint8_t)0x03
#define		ku8MBSlaveDeviceFailure         (uint8_t)0x04
#define		ku8MBInvalidSlaveID             (uint8_t)0xE0
#define		ku8MBInvalidFunction            (uint8_t)0xE1
#define		ku8MBResponseTimedOut           (uint8_t)0xE2
#define		ku8MBInvalidCRC                 (uint8_t)0xE3
//*****************************************************
#define 	ku8MaxBufferSize  								32     // Manuel con 64: se pasa    		///< size of response/transmit buffers    

#define 	ku8MBReadCoils    						0x01 		///< Modbus function 0x01 Read Coils
#define 	ku8MBReadDiscreteInputs   				0x02	 	///< Modbus function 0x02 Read Discrete Inputs
#define 	ku8MBReadInputRegisters   				0x04		///< Modbus function 0x04 Read Input Registers
#define 	ku8MBReadHoldingRegisters   			0x03		///< Modbus function 0x03 Read Holding Registers
#define 	ku8MBReadWriteMultipleRegisters			0x17		///< Modbus function 0x17 Read Write Multiple Registers

#define 	ku8MBWriteSingleCoil            	0x05		///< Modbus function 0x05 Write Single Coil
#define 	ku8MBMaskWriteRegister          	0x16		///< Modbus function 0x16 Mask Write Register
#define 	ku8MBWriteMultipleCoils         	0x0F		///< Modbus function 0x0F Write Multiple Coils
#define 	ku8MBWriteSingleRegister        	0x06		///< Modbus function 0x06 Write Single Register
#define 	ku8MBWriteMultipleRegisters     	0x10		///< Modbus function 0x10 Write Multiple Registers


#define		ku8MBSuccess                    (uint8_t)0x00

// MODBUS Exception Codes
#define 			ILLEGAL_FUNCTION				0x01
#define 			ILLEGAL_DATA_ADDRESS		0x02
#define 			ILLEGAL_DATA_VALUE			0x03





//*********************************************************************************************************************
//*********************************************************************************************************************
//LN194:	#pragma space extern [] @near
extern		uint16_t _u16ReadAddress;                                    ///< slave register from which to read
extern		uint16_t _u16ReadQty;																					///< quantity of words to read


extern 		uint16_t _u16WriteAddress;                                   ///< slave register to which to write
extern 		uint16_t _u16WriteVal;                                       ///10-Ene-2022	< Valor a escribir


//*********************************************************************************************************************
extern		uint8_t u8MBFunctionSlave;





