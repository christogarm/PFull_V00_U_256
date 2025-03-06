/*
 * ELTEC_EmulatedEEPROM.c
 *
 *  Created on: Feb 21, 2025
 *      Author: Christogarm
 */

#include "ELTEC_EmulatedEEPROM.h"
#include "customMain.h"
#include <stdlib.h>

/**
  * @brief  Get Page
  * @param  Adrress_: 	any Address in the Flash memory
  * @retval Address the Page
  */
uint64_t getAddressPage(uint32_t Address_){
	return ( (uint64_t) (Address_ & 0xFFFFF800) );
}


/**
  * @brief  Erase a Page
  * @param  numberPage_: 	Select the Page number (0,1,2,3 .. 127)
  * @retval None
  */
void erasePage(uint32_t numberPage_){
	while( HAL_FLASH_Unlock() !=  HAL_OK );

	FLASH_EraseInitTypeDef pEraseInit = {0};
	pEraseInit.NbPages = 1;
	pEraseInit.Page = numberPage_;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;

	uint32_t Error_=0;
	HAL_FLASHEx_Erase(&pEraseInit,&Error_);		// Erase the Page

	while( HAL_FLASH_Lock() !=  HAL_OK);
}

/**
  * @brief  Write in Flash
  * @param  Adrress_: 		Select the Page number (0,1,2,3 .. 127)
  * @param  arrayData_:		Data save in Flash
  * @param  size_:			Amount of data to save
  * @retval None
  */
void writeFLASH(uint64_t * Address_, uint64_t * arrayData_,uint8_t size_){
	while( HAL_FLASH_Unlock() !=  HAL_OK );
	for(uint8_t i =0; i<size_; i++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) Address_, arrayData_[i]);
		Address_++;
	}
	while( HAL_FLASH_Lock() !=  HAL_OK);
}

/**
  * @brief  Init the EEPROM Emulated
  * @retval None
  */
void initEEPROMEmulated(void){
	uint8_t * varInit = (uint8_t *) Page_126;
	uint8_t size_ = sizeEEPROM_P;
	uint64_t * pointArray_;
	_Bool flagVar_ = 1;
	uint8_t Page_ = 126;

	// Init eePlantilla
initEEPROM_P:
	uint32_t * Flag_EEPROM = ((uint32_t *) Page_126)+1 ; //
	if(*Flag_EEPROM == (uint32_t) Page_126){	// Verify Format Flash is correct
		goto initEEPROM_V;
	}
	flagVar_ = 0;
	uint64_t arrayDataP_[sizeEEPROM_P] = {0};
	pointArray_ = arrayDataP_;
	goto newFormatFlash;

	// Init eeVariables
initEEPROM_V:
	Flag_EEPROM = ((uint32_t *) Page_127)+1 ;
	if(*Flag_EEPROM == (uint32_t) Page_127){ // Verify Format Flash is correct
		return;
	}
	varInit = (uint8_t *) Page_127;
	uint64_t arrayDataV[sizeEEPROM_V] = {0};
	size_ = sizeEEPROM_V;
	pointArray_ = arrayDataV;
	Page_ = 127;

newFormatFlash:
	// Get data
	for(uint8_t i =0; i<size_; i++){
		pointArray_[i]  = (uint64_t) (*varInit);
		pointArray_[i] |= ((uint64_t )(varInit) << 32);
		if( (varInit >= ((uint8_t * ) &eeCntRegDATA)) && flagVar_){
			varInit++;
			pointArray_[i]  |= ((uint64_t) (*varInit))<<8;
		}
		varInit++;
	}
	erasePage(Page_);
	writeFLASH((uint64_t *) (Flag_EEPROM-1),pointArray_,size_);
	if(!flagVar_){
		flagVar_ = 1;
		goto initEEPROM_V;
	}
}

/**
  * @brief  Restart Flash Memory
  * @param
  * @retval
  */
void restartFlashMemory(void){
	uint64_t arrayDataP_[sizeEEPROM_P] = {0};
	uint64_t arrayDataV_[sizeEEPROM_V] = {0};

	uint8_t * varInit = (uint8_t *) Page_126;
	for(uint8_t i =0; i<sizeEEPROM_P; i++){
		arrayDataP_[i] = ((uint64_t )(varInit) << 32);
		varInit ++;
	}

	arrayDataP_[eedato_seg1] |= 0xAA;
	arrayDataP_[eedato_seg2] |= 0x66;
	arrayDataP_[eedato_seg3] |= 0xCC;
	arrayDataP_[eeversion1] |= (uint32_t) versionFirm1;
	arrayDataP_[eeversion2] |= (uint32_t) versionFirm2;

	varInit = (uint8_t *) Page_127;
	for(uint8_t i =0; i<sizeEEPROM_V; i++){
		arrayDataV_[i] |= ((uint64_t )(varInit) << 32);
		if( (varInit >= ((uint8_t * ) &eeCntRegDATA))){
			varInit++;
		}
		varInit++;
	}
	erasePage(126);
	erasePage(127);
	writeFLASH((uint64_t *) (Page_126),&arrayDataP_[0],sizeEEPROM_P);
	writeFLASH((uint64_t *) (Page_127),&arrayDataV_[0],sizeEEPROM_V);
}

/**
  * @brief  Find the Last saved Value
  * @param	AddressPage_:		Address Page
  * @param	AddressValue_:		Address Variable
  * @retval Return AddressValue_'s Data
  */
uint32_t findLastValue(uint32_t AddressValue_){
	uint32_t * AddressPage_ = (uint32_t *) getAddressPage(AddressValue_);
	uint32_t *pointValuex = AddressPage_ + SizePage_32Bits -1;
	if(AddressPage_ < ((uint32_t *) 0x803F000) || AddressPage_ >= ((uint32_t *) 0x8040000)){ // Invalid Direction
		return 0xFFFFFFFF;
	}
	while(*pointValuex != AddressValue_){
		pointValuex -= 2;
		if(pointValuex < AddressPage_){
			restartFlashMemory();
			return 0xFFFFFFFF;
		}
	}
	return *(pointValuex-1);
}

/**
  * @brief  Save Address and Data in Flash
  * @param	managerPoint_:		Address in Flash
  * @param	AddressValue_:		Address Variable
  * @param	Value_:				Data
  * @retval None
  */
void pushAddressData(uint64_t * managerPoint_,uint32_t AddressValue_,uint32_t Value_){
	uint64_t Data_ = 0;
	Data_ = ((uint64_t) AddressValue_) << 32;
	Data_ |= ((uint64_t) Value_);
	writeFLASH(managerPoint_,&Data_,1);
}

/**
  * @brief  Save Address and Data in Flash
  * @param	AddressPage_:		Address Page
  * @retval Return the Address then it is empty
  */
uint64_t currentlyPoint(uint64_t * AddressPage_){
	uint64_t * Pointx =  AddressPage_ + SizePage_64Bits -1;
	while(*Pointx == 0xFFFFFFFFFFFFFFFF){
		Pointx --;
	}
	Pointx++;
	return Pointx; // Return the direction Init
}

/**
  * @brief  FLASH handling
  * @param	AddressValue_:		Address Variable
  * @param	Value_:				Data
  * @retval None
  */
void FlashManager(uint32_t AddressValue_, uint32_t Value_){
	_Bool flag_Page127 = 1;
	uint8_t size_ = sizeEEPROM_V;
	uint64_t * AddressPage_ = (uint64_t *) getAddressPage(AddressValue_);
	if(AddressPage_ == (uint64_t *) (Page_126)){	// Is here Page 126?
		flag_Page127 = 0;
		size_ = sizeEEPROM_P;
	}

	// Manager Characteristics
	uint64_t * managerPointInit = AddressPage_;									// Start Page
	uint64_t * managerPoint =  (uint64_t *) currentlyPoint(AddressPage_);		// Current Point
	uint64_t * managerPointEnd = managerPointInit + SizePage_64Bits -1;			// End Page

	if((managerPoint - 1)== managerPointEnd){	// Is here the End Page?
		// Find the Last Values
		uint32_t * arrayTemp = malloc(size_);
		uint8_t * varInit = (uint8_t *) AddressPage_;
		for(uint8_t i=0; i<size_; i++){
			arrayTemp[i] = findLastValue((uint32_t) varInit);
			varInit++;
			if( (varInit > ((uint8_t * ) &eeCntRegDATA))&flag_Page127){
				varInit++;
			}
		}
		// Erase the Page
		uint32_t VarAux_= (((uint32_t) AddressPage_) - 0x8000000);
		uint32_t numberPage = VarAux_/2048; 		// Number the Page

		erasePage(numberPage);

		// Return the begin Page in current Point

		managerPoint = AddressPage_;
		varInit =  (uint8_t *) AddressPage_;

		// Write the new Values and its Addresses
		for(uint8_t i=0; i<size_; i++){
			pushAddressData(managerPoint,(uint32_t)varInit,arrayTemp[i]);
			varInit++;
			if( varInit > ((uint8_t * ) &eeCntRegDATA)){
				varInit++;
			}
			managerPoint ++;
		}
		pushAddressData(managerPoint,AddressValue_,Value_);
		free(arrayTemp);

	}
	else{

		// Write de new Values and its Addresses
		pushAddressData(managerPoint,AddressValue_,Value_);
	}
}


