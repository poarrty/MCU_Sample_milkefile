/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_gpio_in.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: gpio in operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_gpio_in.h"
#define LOG_TAG "bsp_gpio_in"

#define USED_BSP_GPIO_IN
#ifdef USED_BSP_GPIO_IN

int32_t gpio_in_status_read(uint8_t gpio_in_id) {
    GPIO_PinState pin_status;

    // check id
    switch (gpio_in_id) {
        case ID_PUSH_ROD_MOTOR_1_OC:
            pin_status = PUSH_ROD_MOTOR_1_OC_in;
            break;

        case ID_PUSH_ROD_MOTOR_2_OC:
            pin_status = PUSH_ROD_MOTOR_2_OC_in;
            break;

        case ID_WATER_DISTRIBUTION_OC:
            pin_status = WATER_DISTRIBUTION_OC_in;
            break;

        case ID_WATER_VALCE_CLEAN_OC:
            pin_status = WATER_VALVE_CLEAN_OC_in;
            break;

        default:
            pin_status = GPIO_READ_ERROR;
            break;
    }

    return pin_status;
}

#endif