/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_i2c.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: i2c operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_i2c.h"
#define LOG_TAG "bsp_i2c"

#ifdef USED_BSP_I2C

// #define hw_i2c2_module
#define hw_i2c2_module

#ifdef hw_i2c1_module
extern I2C_HandleTypeDef hi2c1;
#endif  // hw_i2c1_module

#ifdef hw_i2c2_module
extern I2C_HandleTypeDef hi2c2;
#endif  // hw_i2c2_module

int i2c_read_regs(uint8_t i2c_obj_id, uint8_t reg_addr, uint8_t *buf, uint8_t len) {
    I2C_HandleTypeDef *i2c;
    u8                 DevAddress;

    uint8_t temp;
    temp = reg_addr;

    switch (i2c_obj_id) {
#ifdef hw_i2c1_module

        case hw_i2c1:
            i2c        = &hi2c1;
            DevAddress = i2c1_devAddr;
            break;
#endif  // hw_i2c1_module
#ifdef hw_i2c2_module

        case hw_i2c2:
            i2c        = &hi2c2;
            DevAddress = i2c2_devAddr;
            break;
#endif  // hw_i2c2_module

        default:
            return -HAL_ERROR;
            break;
    }

    // write reg address
    if (HAL_I2C_Master_Transmit(i2c, DevAddress, &temp, 1, 100) != HAL_OK) {
        return -HAL_ERROR;
    }

    if (HAL_I2C_Master_Receive(i2c, DevAddress, buf, len, 100) != HAL_OK) {
        return -HAL_ERROR;
    }

    return len;
}

int i2c_write_regs(uint8_t i2c_obj_id, uint8_t reg_addr, uint8_t *buf, uint8_t len) {
    I2C_HandleTypeDef *i2c;
    u8                 DevAddress;

    uint8_t temp[len + 1];
    temp[0] = reg_addr;
    memcpy(temp + 1, buf, len);

    switch (i2c_obj_id) {
#ifdef hw_i2c1_module

        case hw_i2c1:
            i2c        = &hi2c1;
            DevAddress = i2c1_devAddr;
            break;
#endif  // hw_i2c1_module
#ifdef hw_i2c2_module

        case hw_i2c2:
            i2c        = &hi2c2;
            DevAddress = i2c2_devAddr;
            break;
#endif  // hw_i2c2_module

        default:
            return -HAL_ERROR;
            break;
    }

    // i2c write register
    if (HAL_I2C_Master_Transmit(i2c, DevAddress, temp, (len + 1), 100) != HAL_OK) {
        return -RT_ERROR;
    }

    return len;
}

static void bsp_i2c_read(void) {
    uint8_t buf[30];

    for (int i = 0; i < 30; i++) {
        buf[i] = i;
        log_d("%d ", buf[i]);
        i2c_write_regs(0, i, &buf[i], 1);
        osDelay(2);
    }

    memset(buf, 0, 30);
    log_d("\r\n");
    osDelay(10);

    for (int i = 0; i < 30; i++) {
        osDelay(2);
        i2c_read_regs(0, i, &buf[i], 1);
        log_d("%d ", buf[i]);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), bsp_i2c_read, bsp_i2c_read, i2c test);

#endif
