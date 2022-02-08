#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stdio.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_def.h"

void              Flash_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count);
int8_t            Flash_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count);
HAL_StatusTypeDef Flash_EraseSector(uint32_t Address);

#endif
