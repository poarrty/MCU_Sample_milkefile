/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_gpio_in.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: gpio in operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_GPIO_IN_H
#define __BSP_GPIO_IN_H
#include "drv_gpio.h"

/*-------------------------input-----------------------------------------*/
/*-------------------------input end ------------------------------------*/

/*-------------------------output----------------------------------------*/
#ifdef GPIO_HAL_USING
#define PUSH_ROD_MOTOR_1_OC_in   GPIO_R(T1_OC_MCU_GPIO_Port, T1_OC_MCU_Pin)
#define PUSH_ROD_MOTOR_2_OC_in   GPIO_R(T1_OC_MCU_GPIO_Port, T1_OC_MCU_Pin)
#define WATER_DISTRIBUTION_OC_in GPIO_R(T1_OC_MCU_GPIO_Port, T1_OC_MCU_Pin)
#else
#define PUSH_ROD_MOTOR_1_OC_in   (HAL_GPIO_ReadPin(T1_OC_MCU_GPIO_Port, T1_OC_MCU_Pin))
#define PUSH_ROD_MOTOR_2_OC_in   (HAL_GPIO_ReadPin(T2_OC_MCU_GPIO_Port, T2_OC_MCU_Pin))
#define WATER_DISTRIBUTION_OC_in (HAL_GPIO_ReadPin(B_OC_MCU_GPIO_Port, B_OC_MCU_Pin))
#define WATER_VALVE_CLEAN_OC_in  (HAL_GPIO_ReadPin(DC_IO_1_GPIO_Port, DC_IO_1_Pin))
#endif  // !gpio_hal_in

#define gpio_in(id) gpio_in_status_read(id)
/*-------------------------output end-----------------------------------------*/
enum { ID_PUSH_ROD_MOTOR_1_OC = 0, ID_PUSH_ROD_MOTOR_2_OC, ID_WATER_DISTRIBUTION_OC, ID_WATER_VALCE_CLEAN_OC };
int32_t gpio_in_status_read(uint8_t gpio_in_id);
#endif
