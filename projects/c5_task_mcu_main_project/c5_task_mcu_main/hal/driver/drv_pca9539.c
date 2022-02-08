#include "drv_pca9539.h"
#include "bsp_i2c.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "main.h"
#include "log.h"
#include "cmsis_os.h"
#include "shell.h"

#define LOG_TAG "drv_pca9539"
#include "elog.h"

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
extern osMutexId_t mutex_i2c2Handle;
static uint8_t     i2c_error_status = 0;

void clear_i2c_error_status(void) {
    taskENTER_CRITICAL();
    i2c_error_status = 0;
    taskEXIT_CRITICAL();
}

uint8_t get_i2c_error_status(void) {
    return i2c_error_status;
}

void set_i2c_error_stauts(uint8_t status) {
    taskENTER_CRITICAL();
    i2c_error_status = status;
    taskEXIT_CRITICAL();
}

/*写寄存器*/
int pca9539_write_regs(uint8_t reg, uint8_t const *data, uint8_t val_size) {
    uint32_t ret = 0;
    osMutexAcquire(mutex_i2c2Handle, osWaitForever);
    ret = HAL_I2C_Mem_Write(&hi2c2, PCA9539_W, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, val_size, 50);
    if (ret != HAL_OK) {
        set_i2c_error_stauts(1);
    }
    osMutexRelease(mutex_i2c2Handle);
    return ret;
}

/*读寄存器*/
int pca9539_read_regs(uint8_t reg, uint8_t const *data, uint8_t val_size) {
    uint32_t ret = 0;
    osMutexAcquire(mutex_i2c2Handle, osWaitForever);
    ret = HAL_I2C_Mem_Read(&hi2c2, PCA9539_R, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, val_size, 50);
    if (ret != HAL_OK) {
        set_i2c_error_stauts(1);
    }
    osMutexRelease(mutex_i2c2Handle);
    return ret;
}

/*设置pca9539寄存器模式*/
void pca9539_mode(uint8_t port, uint8_t pin, uint8_t mode) {
    uint8_t msg[2]    = {0, 0};
    uint8_t pin_index = PCA9539_GET_PIN(port, pin);
    if (port > PCA9539_PORT1 || pin > PIN7)
        return;
    if (pca9539_read_regs(PORT_0_CONFIG_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        if (mode == PIN_MODE_OUTPUT) {
            msg[pin_index / PORT_PER_PIN] &= ~(1 << (pin % 8));
        } else if (mode == PIN_MODE_INPUT) {
            msg[pin_index / PORT_PER_PIN] &= ~(1 << (pin % 8));
            msg[pin_index / PORT_PER_PIN] |= (1 << (pin % 8));
        }
        if (pca9539_write_regs(PORT_0_CONFIG_CMD, msg, ITEM_NUM(msg)) != HAL_OK) {
            log_e("write config  reg fail!!\r\n");
        }
    } else {
        // log_e("read config reg fail!\r\n");
    }
}

/*设置pca9539 gpio 电平值*/
void pca9539_pin_write(uint8_t port, uint8_t pin, uint8_t pin_val) {
    uint8_t msg[2]    = {0, 0};
    uint8_t pin_index = PCA9539_GET_PIN(port, pin);

    if (port > PCA9539_PORT1 || pin > PIN7)
        return;

    if (pca9539_read_regs(PORT_0_OUTPUT_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        msg[pin_index / PORT_PER_PIN] &= ~(1 << (pin % 8));
        msg[pin_index / PORT_PER_PIN] |= (pin_val << (pin % 8));
        if (pca9539_write_regs(PORT_0_OUTPUT_CMD, msg, ITEM_NUM(msg)) != HAL_OK) {
            log_e("write output reg fail!!\r\n");
        }
    } else {
        // log_e("read output reg fail!!\r\n");
    }
}

/*读取pca9539 gpio 电平值*/
int pca9539_pin_read(uint8_t port, uint8_t pin) {
    uint8_t msg[2]    = {0, 0};
    uint8_t pin_bit   = 0xff;
    uint8_t pin_index = PCA9539_GET_PIN(port, pin);

    if (port > PCA9539_PORT1 || pin > PIN7)
        return pin_bit;

    /* 读取寄存器数据 */
    if (pca9539_read_regs(PORT_0_INPUT_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        pin_bit = (msg[pin_index / PORT_PER_PIN] >> (pin % 8)) & 0x01;
    } else {
        // LOG_ERROR("read input reg fail!!\r\n");
    }
    return pin_bit;
}

/*test read*/
void pca9539_read(uint8_t port, uint8_t pin) {
    uint8_t ret_val[2] = {0};
    if (port) {
        pca9539_read_regs(PORT_1_INPUT_CMD, ret_val, ITEM_NUM(ret_val));
        log_d("retval_p1 = %#x\r\n", ret_val[0]);
    } else {
        pca9539_read_regs(PORT_0_INPUT_CMD, ret_val, ITEM_NUM(ret_val));
        log_d("retval_p0 = %#x\r\n", ret_val[0]);
    }
}

void pca9539_read_reg_value(void) {
    uint8_t msg[2] = {0, 0};
    if (pca9539_read_regs(PORT_0_CONFIG_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        LOG_DEBUG("config reg 1: %x, 2: %x", msg[0], msg[1]);
    } else {
        log_e("read config reg fail!\r\n");
    }
    if (pca9539_read_regs(PORT_0_OUTPUT_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        LOG_DEBUG("write reg 1: %x, 2: %x", msg[0], msg[1]);
    } else {
        log_e("read config reg fail!\r\n");
    }
    if (pca9539_read_regs(PORT_0_INPUT_CMD, msg, ITEM_NUM(msg)) == HAL_OK) {
        LOG_DEBUG("input reg 1: %x, 2: %x", msg[0], msg[1]);
    } else {
        log_e("read config reg fail!\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, pca9539_read_reg_value,
                 pca9539_read_reg_value, read pac9539 reg);
