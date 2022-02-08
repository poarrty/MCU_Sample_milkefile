/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_gpio_out.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: gpio out operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_GPIO_OUT_H
#define __BSP_GPIO_OUT_H

#include "drv_gpio.h"
/*-------------------------input-----------------------------------------*/
/*-------------------------input end ------------------------------------*/

/*-------------------------output----------------------------------------*/
#ifdef GPIO_HAL_USING
#define SIDE_BRUSH_MOTOR_LEFT_CW_out(status)  GPIO_W(CW_ZBS_MCU_GPIO_Port, CW_ZBS_MCU_Pin, status)
#define SIDE_BRUSH_MOTOR_RIGHT_CW_out(status) GPIO_W(CW_YBS_MCU_GPIO_Port, CW_YBS_MCU_Pin, status)
#define SUNCTION_MOTOR_CTL_out(status)        GPIO_W(PWM_F_MCU_GPIO_Port, PWM_F_MCU_Pin, status)
#define SUNCTION_MOTOR_CW_out(status)         GPIO_W(CW_F_MCU_GPIO_Port, CW_F_MCU_Pin, status)
#define PUSH_ROD_MOTOR_1_TL_out(status)       GPIO_W(T1_L_GPIO_Port, T1_L_Pin, status)
#define PUSH_ROD_MOTOR_1_TH_out(status)       GPIO_W(T1_H_GPIO_Port, T1_H_Pin, status)
#define PUSH_ROD_MOTOR_2_TL_out(status)       GPIO_W(T2_L_GPIO_Port, T2_L_Pin, status)
#define PUSH_ROD_MOTOR_2_TH_out(status)       GPIO_W(T2_H_GPIO_Port, T2_H_Pin, status)
#define WATER_DISTRIBUTION_POWER_out(status)  GPIO_W(B_IO_GPIO_Port, B_IO_Pin, status)
#define SIDE_BRUSH_POWER_out(status)          GPIO_W(BS_POWER_ON_GPIO_Port, BS_POWER_ON_Pin, status)
#define EEPROM_RW_out(status)                 GPIO_W(BL24_WP_PC2_M_GPIO_Port, BL24_WP_PC2_M_Pin, status)
#define XS_POWER_out(status)                  GPIO_W(XS_MCU_GPIO_Port, XS_MCU_Pin, status)
#else
#define SIDE_BRUSH_MOTOR_LEFT_CW_out(status)  HAL_GPIO_WritePin(CW_ZBS_MCU_GPIO_Port, CW_ZBS_MCU_Pin, status)
#define SIDE_BRUSH_MOTOR_RIGHT_CW_out(status) HAL_GPIO_WritePin(CW_YBS_MCU_GPIO_Port, CW_YBS_MCU_Pin, status)
#define SUNCTION_MOTOR_CTL_out(status)        HAL_GPIO_WritePin(PWM_F_MCU_GPIO_Port, PWM_F_MCU_Pin, status)
#define SUNCTION_MOTOR_CW_out(status)         HAL_GPIO_WritePin(CW_F_MCU_GPIO_Port, CW_F_MCU_Pin, status)
#define PUSH_ROD_MOTOR_1_TL_out(status)       HAL_GPIO_WritePin(T1_L_GPIO_Port, T1_L_Pin, status)
#define PUSH_ROD_MOTOR_1_TH_out(status)       HAL_GPIO_WritePin(T1_H_GPIO_Port, T1_H_Pin, status)
#define PUSH_ROD_MOTOR_2_TL_out(status)       HAL_GPIO_WritePin(T2_L_GPIO_Port, T2_L_Pin, status)
#define PUSH_ROD_MOTOR_2_TH_out(status)       HAL_GPIO_WritePin(T2_H_GPIO_Port, T2_H_Pin, status)
#define WATER_DISTRIBUTION_POWER_out(status)  HAL_GPIO_WritePin(B_IO_GPIO_Port, B_IO_Pin, status)
#define WATER_VALVE_CLEAN_POWER_out(status)   HAL_GPIO_WritePin(DC_IO_1_GPIO_Port, DC_IO_1_Pin, status)
#define SIDE_BRUSH_POWER_out(status)          HAL_GPIO_WritePin(BS_POWER_ON_GPIO_Port, BS_POWER_ON_Pin, status)
#define XS_POWER_out(status)                  HAL_GPIO_WritePin(XS_MCU_GPIO_Port, XS_MCU_Pin, status)
#endif  // !gipo_hal

#define gpio_out_set(id, sta) gpio_out_on_off(id, sta)
#define gpio_set_on(id)       gpio_out_on(id)
#define gpio_set_off(id)      gpio_out_off(id)
/*-------------------------output end-----------------------------------------*/

enum {
    ID_SIDE_BRUSH_MOTOR_LEFT_CW = 0,  // 0
    ID_SIDE_BRUSH_MOTOR_RIGHT_CW,
    ID_SUNCTION_MOTOR_CTL,
    ID_SUNCTION_MOTOR_CW,
    ID_PUSH_ROD_MOTOR_1_TL,
    ID_PUSH_ROD_MOTOR_1_TH,
    ID_PUSH_ROD_MOTOR_2_TL,
    ID_PUSH_ROD_MOTOR_2_TH,
    ID_WATER_DISTRIBUTION_POWER,
    ID_WATER_VALVE_CLEAN_POWER,
    ID_SIDE_BRUSH_POWER,
    ID_EEPROM_RW,
    ID_XS_POWER
};

int gpio_out_on_off(uint8_t gpio_out_id, uint8_t status);
int gpio_out_on(uint8_t gpio_out_id);
int gpio_out_off(uint8_t gpio_out_id);
#endif
