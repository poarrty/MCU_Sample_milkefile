#include "bsp_flash.h"

void Flash_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count) {
    uint32_t i;

    for (i = 0; i < count; i++) {
        buffer[i] = *((uint8_t *) (addr + i));
    }
}

int8_t Flash_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count) {
    HAL_StatusTypeDef Status      = HAL_OK;
    int8_t            Result      = 0;
    uint16_t *        Half_Buffer = (uint16_t *) buffer;
    uint32_t          Half_Count;
    uint32_t          i;

    HAL_FLASH_Unlock();

    if (count & 0x00000001) {
        Half_Count = count / 2 + 1;
    } else {
        Half_Count = count / 2;
    }

    for (i = 0; (i < Half_Count) && (Status == HAL_OK); i++) {
        Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i * 2, Half_Buffer[i]);
    }

    if (Status != HAL_OK) {
        Result = -1;
    }

    HAL_FLASH_Lock();

    return Result;
}

HAL_StatusTypeDef Flash_EraseSector(uint32_t Address) {
    HAL_StatusTypeDef Status = HAL_OK;
    HAL_FLASH_Unlock();

    uint32_t               PageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;

    /* 擦除一页 */
    pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = Address;
    pEraseInit.Banks       = FLASH_BANK_1;
    pEraseInit.NbPages     = 1;
    Status                 = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

    HAL_FLASH_Lock();
    return Status;
}