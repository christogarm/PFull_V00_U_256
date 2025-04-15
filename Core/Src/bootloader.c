
#include "main.h"
#include "customMain.h"



void bootloader (void){
	 __disable_irq();

//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
//	HAL_IWDG_Refresh( &hiwdg );
//	asm ("nop");


//	while( HAL_FLASH_Unlock() !=  HAL_OK );
//	while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, direccion, palabra_key) != HAL_OK);
//	//while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, direccion_fw, buffer_recepcion) != HAL_OK);
//	while(HAL_FLASH_Lock()!=  HAL_OK );

    // Desbloquea el controlador FLASH

	void (*app_reset_handler) (void) = (void*) (*(volatile uint32_t *) (0x0801F000 + 4));
    //__set_MSP((*(volatile uint32_t *)(0x0801E800)) );0x801E800
	app_reset_handler();
}


//;=====================================================================
//;	SUBRUTINA QUE GUARDA LOS DATOS DEL PROGRAMA EN EEPROM
//;	Carga inicialmente el programa a la seccion de ram para ejecutarla
//;=====================================================================
//void GRABA_BLOQUE(uint8_t *dataPointer , uint8_t *dirPointer){
//
//
//   	uint32_t AddressDestination;
//  	uint32_t Data ;
//
//  	HAL_FLASHEx_DATAEEPROM_Unlock();
//
//	if (ProgMemCode == 0xAB){
//		asm ("nop");
//
//	}
//	else if (ProgMemCode == 0xAA){
//		asm ("nop");
//
//	}
//	else if (ProgMemCode == 0x55){
//
//		// for(uint8_t i = 0; i < 128 ; i++ ){
//		for(uint8_t i = 0; i < 32 ; i++ ){
//			AddressDestination = dirPointer;
//		   	// Data = (uint32_t)(*dataPointer);
//
//			Data  =  (uint32_t)(*dataPointer);						dataPointer++;
//			Data  += ((uint32_t)(*dataPointer)) <<8;				dataPointer++;
//			Data  += ((uint32_t)(*dataPointer)) <<16;				dataPointer++;
//			Data  += ((uint32_t)(*dataPointer))  <<24;			   	dataPointer++;
//
//			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, AddressDestination, Data);		// HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, AddressDestination, Data);
//		   	dirPointer +=4;
//			HAL_IWDG_Refresh( &hiwdg );				// evita time out watch
//			asm ("nop");
//
//
//		}
//	}
//
//  	HAL_FLASHEx_DATAEEPROM_Lock();
//}
