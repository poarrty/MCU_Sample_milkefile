#include "main.h"
#include "bsp_gpio_out.h"
#include "shell.h"
#include "stm32f1xx.h"
#include "gpio.h"
#include "log.h"
#include "stdlib.h"
#include "drv_pca9539.h"

#define LOG_TAG "bsp_gpio_out"
#include "elog.h"

void gpio_out_init_for_pca9539(void) {
    pca9539_mode(PCA9539_PORT1, PIN3, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT0, PIN0, PIN_MODE_INPUT);
    pca9539_mode(PCA9539_PORT1, PIN0, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN2, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN3, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN4, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN5, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN6, PIN_MODE_OUTPUT);
    pca9539_mode(PCA9539_PORT1, PIN7, PIN_MODE_OUTPUT);
}

void gpio_out_on(uint8_t gpio_out_id) {
    switch (gpio_out_id) {
        case ID_FLITER_PUMP:
            break;

        case ID_CLEAN_WATER_VALVE:
            break;

        case ID_WASTE_WATER_VALVE:
            HAL_GPIO_WritePin(MX_WASTE_WATER_VALVE_EN_GPIO_Port, MX_WASTE_WATER_VALVE_EN_Pin, GPIO_PIN_SET);
            break;

        case ID_IO_HUB_CTL_0:
            HAL_GPIO_WritePin(MX_WATER_LEVEL_SEL_00_GPIO_Port, MX_WATER_LEVEL_SEL_00_Pin, GPIO_PIN_SET);
            break;

        case ID_IO_HUB_CTL_1:
            HAL_GPIO_WritePin(MX_WATER_LEVEL_SEL_01_GPIO_Port, MX_WATER_LEVEL_SEL_01_Pin, GPIO_PIN_SET);
            break;

        case ID_RK3399_POWER_CTRL:
            HAL_GPIO_WritePin(MX_RK_POWER_EN_GPIO_Port, MX_RK_POWER_EN_Pin, GPIO_PIN_SET);
            break;

        case ID_SEWAGE_WATER_VALVE:
            HAL_GPIO_WritePin(MX_SEWAGE_WATER_VALVE_EN_GPIO_Port, MX_SEWAGE_WATER_VALVE_EN_Pin, GPIO_PIN_SET);
            break;

        case ID_RK_DEBUG_SWITCH_1:
            HAL_GPIO_WritePin(MX_RKUART_SWTICH_1_GPIO_Port, MX_RKUART_SWTICH_1_Pin, GPIO_PIN_SET);
            break;

        case ID_RK_DEBUG_SWITCH_2:
            HAL_GPIO_WritePin(MX_RKUART_SWTICH_2_GPIO_Port, MX_RKUART_SWTICH_2_Pin, GPIO_PIN_SET);
            break;

        case ID_LED_POWER:
            break;

        case ID_MEG_LED:
            break;

        case ID_EEPROM_RW:
            break;

        case ID_COMM_RS485_DE:
            HAL_GPIO_WritePin(MX_UART4_DATA_EN_GPIO_Port, MX_UART4_DATA_EN_Pin, GPIO_PIN_SET);
            break;

        case ID_XS_POWER:
            break;

        case ID_24V_POWER:
            HAL_GPIO_WritePin(MX_24V_STB_EN_GPIO_Port, MX_24V_STB_EN_Pin, GPIO_PIN_SET);
            break;

        case ID_12V_ULT_POWER:
            break;

        case ID_ULT_CTRL_0:
            break;

        case ID_ULT_CTRL_1:
            break;

        case ID_ULT_CTRL_2:
            break;

        case ID_CLEAN_WATER_LEVEL_ERROR_LED:
            pca9539_mode(PCA9539_PORT0, PIN3, PIN_MODE_OUTPUT);
            pca9539_pin_write(PCA9539_PORT0, PIN3, PIN_LOW);
            break;

        case ID_WASTE_WATER_LEVEL_ERROR_LED:
            pca9539_mode(PCA9539_PORT0, PIN0, PIN_MODE_OUTPUT);
            pca9539_pin_write(PCA9539_PORT0, PIN0, PIN_LOW);
            break;

        /*新按键板指示灯*/
        case ID_KEY_RETURN_HOME_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN0, PIN_LOW);
            break;
        case ID_KEY_AUTO_MODE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN2, PIN_LOW);
            break;
        case ID_KEY_MANUAL_MODE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN3, PIN_LOW);
            break;
        case ID_KEY_WASH_FLOOR_BLUE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN4, PIN_LOW);
            break;
        case ID_KEY_WASH_FLOOR_RED_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN5, PIN_LOW);
            break;
        case ID_KEY_DUST_PUSH_BLUE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN6, PIN_LOW);
            break;
        case ID_KEY_DUST_PUSH_RED_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN7, PIN_LOW);
            break;
        case ID_KEY_BOARD_POWER_EN:
            HAL_GPIO_WritePin(MX_KEY_BOARD_POWER_EN_GPIO_Port, MX_KEY_BOARD_POWER_EN_Pin, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

void gpio_out_off(uint8_t gpio_out_id) {
    switch (gpio_out_id) {
        case ID_WASTE_WATER_VALVE:
            HAL_GPIO_WritePin(MX_WASTE_WATER_VALVE_EN_GPIO_Port, MX_WASTE_WATER_VALVE_EN_Pin, GPIO_PIN_RESET);
            break;

        case ID_IO_HUB_CTL_0:
            HAL_GPIO_WritePin(MX_WATER_LEVEL_SEL_00_GPIO_Port, MX_WATER_LEVEL_SEL_00_Pin, GPIO_PIN_RESET);
            break;

        case ID_IO_HUB_CTL_1:
            HAL_GPIO_WritePin(MX_WATER_LEVEL_SEL_01_GPIO_Port, MX_WATER_LEVEL_SEL_01_Pin, GPIO_PIN_RESET);
            break;

        case ID_RK3399_POWER_CTRL:
            HAL_GPIO_WritePin(MX_RK_POWER_EN_GPIO_Port, MX_RK_POWER_EN_Pin, GPIO_PIN_RESET);
            break;

        case ID_SEWAGE_WATER_VALVE:
            HAL_GPIO_WritePin(MX_SEWAGE_WATER_VALVE_EN_GPIO_Port, MX_SEWAGE_WATER_VALVE_EN_Pin, GPIO_PIN_RESET);
            break;

        case ID_RK_DEBUG_SWITCH_1:
            HAL_GPIO_WritePin(MX_RKUART_SWTICH_1_GPIO_Port, MX_RKUART_SWTICH_1_Pin, GPIO_PIN_RESET);
            break;

        case ID_RK_DEBUG_SWITCH_2:
            HAL_GPIO_WritePin(MX_RKUART_SWTICH_2_GPIO_Port, MX_RKUART_SWTICH_2_Pin, GPIO_PIN_RESET);
            break;

        case ID_COMM_RS485_DE:
            HAL_GPIO_WritePin(MX_UART4_DATA_EN_GPIO_Port, MX_UART4_DATA_EN_Pin, GPIO_PIN_RESET);
            break;

        case ID_XS_POWER:
            break;

        case ID_24V_POWER:
            HAL_GPIO_WritePin(MX_24V_STB_EN_GPIO_Port, MX_24V_STB_EN_Pin, GPIO_PIN_RESET);
            break;

        case ID_CLEAN_WATER_LEVEL_ERROR_LED:
            pca9539_mode(PCA9539_PORT0, PIN3, PIN_MODE_INPUT);
            break;

        case ID_WASTE_WATER_LEVEL_ERROR_LED:
            pca9539_mode(PCA9539_PORT0, PIN0, PIN_MODE_INPUT);
            break;

        /*新按键板指示灯*/
        case ID_KEY_RETURN_HOME_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN0, PIN_HIGH);
            break;
        case ID_KEY_AUTO_MODE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN2, PIN_HIGH);
            break;
        case ID_KEY_MANUAL_MODE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN3, PIN_HIGH);
            break;
        case ID_KEY_WASH_FLOOR_BLUE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN4, PIN_HIGH);
            break;
        case ID_KEY_WASH_FLOOR_RED_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN5, PIN_HIGH);
            break;
        case ID_KEY_DUST_PUSH_BLUE_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN6, PIN_HIGH);
            break;
        case ID_KEY_DUST_PUSH_RED_LED:
            pca9539_pin_write(PCA9539_PORT1, PIN7, PIN_HIGH);
            break;
        case ID_KEY_BOARD_POWER_EN:
            HAL_GPIO_WritePin(MX_KEY_BOARD_POWER_EN_GPIO_Port, MX_KEY_BOARD_POWER_EN_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void gpio_out(int argc, char **argv) {
    if (argc < 3) {
        log_d("%s parm error", __FUNCTION__);
        return;
    }

    if (atoi(argv[2])) {
        gpio_out_on(atoi(argv[1]));
    } else {
        gpio_out_off(atoi(argv[1]));
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, gpio_out, gpio_out,
                 gpio out arg1 id arg2 status);
