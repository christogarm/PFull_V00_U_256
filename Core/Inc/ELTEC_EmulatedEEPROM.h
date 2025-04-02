/*
 * ELTEC_EmulatedEEPROM.h
 *
 *  Created on: Feb 21, 2025
 *      Author: Useri3pc3
 */

#include "main.h"


#define SizePage_Bits		16384							// Size page in Bits
#define SizePage_8Bits		(SizePage_Bits/8)				// Size page in Bytes
#define SizePage_32Bits		(SizePage_Bits/32)				// Size page for Variables of the 32 Bits
#define SizePage_64Bits		(SizePage_Bits/64)

#define sizeEEPROM_V			18							//Size of the myBufSectionEEPROM_V
#define sizeEEPROM_P			128							//Size of the myBufSectionEEPROM_P

#define Page_126			0x803F000						// Address of the Page 126; It's a eePlantilla
#define Page_127			0x803F800						// Address of the Page 127; They're difference eeVariables

uint8_t getNumberPage(uint32_t Address_);												// Get Number Page
uint64_t getAddressPage(uint32_t Address_);												// Get Address Page
void erasePage(uint32_t numberPage_);													// Erase a Page
void writeFLASH(uint64_t * Adrress_, uint64_t * arrayData_,uint8_t size_);				// Write N  DoubleWords (64 bits) in memory Flash
void initEEPROMEmulated(void);															// Initialize EEPROM emulated, i.e. modify it to new Flash format.
void restartFlashMemory(void);															// Restart Flash Memory with the Format
uint32_t findLastValue(uint32_t AddressValue_);											// Find the Last Value saved
void pushAddressData(uint64_t * managerPoint_,uint32_t AddressValue_,uint32_t Value_);	// Push Address and Value in Flash memory
_Bool isPageFull(uint32_t * AddressPage_);												// Is Page Full?
uint64_t currentlyPoint(uint64_t * AddressPage_);										// Get current Point
void FlashManager(uint32_t AddressValue_, uint32_t Value_);								// Write Address and Data in memory Flash to an empty space.



