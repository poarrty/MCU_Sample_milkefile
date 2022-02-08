#include "bsp_gpio_in.h"
#include "stm32f1xx.h"
#include "gpio.h"
#include "stdlib.h"
#include "shell.h"
#include "log.h"
#include "drv_pca9539.h"

#define LOG_TAG "bsp_gpio_in"
#include "elog.h"

void gpio_in_init_for_pca9539(void) {
    pca9539_mode(PCA9539_PORT0, PIN4, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT0, PIN5, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT0, PIN6, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT0, PIN7, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT0, PIN2, PIN_MODE_INPUT);
}

int32_t gpio_in_status_read(uint8_t gpio_in_id) {
    uint8_t status = 0xff;

    switch (gpio_in_id) {
        case ID_WATER_LEVEL:
            status = HAL_GPIO_ReadPin(MX_WATER_LEVEL_DET_GPIO_Port, MX_WATER_LEVEL_DET_Pin);
            break;

        case ID_FILTER_PUMP_OC:
            status = HAL_GPIO_ReadPin(MX_FILTER_PUMP_OC_DET_GPIO_Port, MX_FILTER_PUMP_OC_DET_Pin);
            break;

        case ID_SEWAGE_WATER_VALVE_OC:
            status = HAL_GPIO_ReadPin(MX_SEWAGE_WATER_VALVE_DET_GPIO_Port, MX_SEWAGE_WATER_VALVE_DET_Pin);
            break;

        case ID_WASTE_WATER_VALVE_OC:
            status = HAL_GPIO_ReadPin(MX_WASTE_WATER_VALVE_DET_GPIO_Port, MX_WASTE_WATER_VALVE_DET_Pin);
            break;

        case ID_KEY_RETURN_HOME:
            status = pca9539_pin_read(PCA9539_PORT0, PIN4);
            break;

        case ID_KEY_AUTO_MANUAL_MODE:
            status = pca9539_pin_read(PCA9539_PORT0, PIN5);
            break;

        case ID_KEY_WASH_FLOOR:
            status = pca9539_pin_read(PCA9539_PORT0, PIN6);
            break;

        case ID_KEY_DUST_PUSH:
            status = pca9539_pin_read(PCA9539_PORT0, PIN7);
            break;

        case ID_KEY_STRAINER:
            status = pca9539_pin_read(PCA9539_PORT0, PIN2);
            break;

        default:
            break;
    }
    return status;
}

void gpio_read(int argc, char **argv) {
    int temp = 0xff;

    if (argc < 2) {
        log_d("%s parm error", __FUNCTION__);
        return;
    }

    temp = gpio_in_status_read(atoi(argv[1]));
    log_d("read io state :%d", temp);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, gpio_read, gpio_read,
                 gpio read arg1 id arg2 status);
