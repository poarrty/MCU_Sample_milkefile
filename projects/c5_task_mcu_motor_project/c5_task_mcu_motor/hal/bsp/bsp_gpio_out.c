/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_gpio_out.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: gpio out operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_gpio_out.h"

#define LOG_TAG "bsp_gpio_out"
#include "elog.h"
#define USED_BSP_GPIO_OUT
#ifdef USED_BSP_GPIO_OUT

int gpio_out_on_off(uint8_t gpio_out_id, uint8_t status) {
    switch (gpio_out_id) {
        case ID_SIDE_BRUSH_MOTOR_LEFT_CW:
            SIDE_BRUSH_MOTOR_LEFT_CW_out(status);
            break;

        case ID_SIDE_BRUSH_MOTOR_RIGHT_CW:
            SIDE_BRUSH_MOTOR_RIGHT_CW_out(status);
            break;

        case ID_SUNCTION_MOTOR_CTL:
            SUNCTION_MOTOR_CTL_out(status);
            break;

        case ID_SUNCTION_MOTOR_CW:
            SUNCTION_MOTOR_CW_out(status);
            break;

        case ID_PUSH_ROD_MOTOR_1_TL:
            PUSH_ROD_MOTOR_1_TL_out(status);
            break;

        case ID_PUSH_ROD_MOTOR_1_TH:
            PUSH_ROD_MOTOR_1_TH_out(status);
            break;

        case ID_PUSH_ROD_MOTOR_2_TL:
            PUSH_ROD_MOTOR_2_TL_out(status);
            break;

        case ID_PUSH_ROD_MOTOR_2_TH:
            PUSH_ROD_MOTOR_2_TH_out(status);
            break;

        case ID_WATER_DISTRIBUTION_POWER:
            WATER_DISTRIBUTION_POWER_out(status);
            break;

        case ID_WATER_VALVE_CLEAN_POWER:
            WATER_VALVE_CLEAN_POWER_out(status);
            break;

        case ID_SIDE_BRUSH_POWER:
            SIDE_BRUSH_POWER_out(status);
            break;

        case ID_XS_POWER:
            XS_POWER_out(status);
            break;

        default:
            return -1;
            break;
    }

    return 0;
}

int gpio_out_on(uint8_t gpio_out_id) {
    uint8_t output_level = 0x01;

    return gpio_out_on_off(gpio_out_id, output_level);
}

int gpio_out_off(uint8_t gpio_out_id) {
    uint8_t output_level = 0;

    return gpio_out_on_off(gpio_out_id, output_level);
}

#ifdef RT_USING_FINSH
void gpio_ops(int argc, char **argv) {
    if (argc < 3) {
        log_w("%s parm error", __FUNCTION__);
        return;
    }

    gpio_out_on_off(atoi(argv[1]), atoi(argv[2]));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), gpio_ops, gpio_ops, gpio_ops 0 1);
#endif

#endif