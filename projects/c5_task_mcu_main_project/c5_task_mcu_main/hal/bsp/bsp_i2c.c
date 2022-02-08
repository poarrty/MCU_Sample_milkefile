#include "bsp_i2c.h"
#include "i2c.h"
#include "main.h"
#include "stdlib.h"
#include "log.h"
#include "elog.h"

HAL_StatusTypeDef bsp_i2c1_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c2, DevAddress, MemAddress, MemAddSize, pData, Size, 1000);

    if (status != HAL_OK) {
        log_e("Return val:%d", status);
        return status;
    }

    return status;
}

HAL_StatusTypeDef bsp_i2c1_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c2, DevAddress, MemAddress, MemAddSize, pData, Size, 1000);

    if (status != HAL_OK) {
        log_e("Return val:%d", status);
        return status;
    }

    return status;
}

HAL_StatusTypeDef bsp_i2c2_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c2, DevAddress, MemAddress, MemAddSize, pData, Size, 1000);

    if (status != HAL_OK) {
        log_e("Return val:%d", status);
        return status;
    }

    return status;
}

HAL_StatusTypeDef bsp_i2c2_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c2, DevAddress, MemAddress, MemAddSize, pData, Size, 1000);

    if (status != HAL_OK) {
        log_e("Return val:%d", status);
        return status;
    }

    return status;
}
