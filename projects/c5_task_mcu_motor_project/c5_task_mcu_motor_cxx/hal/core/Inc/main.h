/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DC_IO_1_Pin             GPIO_PIN_13
#define DC_IO_1_GPIO_Port       GPIOC
#define CW_F_MCU_Pin            GPIO_PIN_14
#define CW_F_MCU_GPIO_Port      GPIOC
#define CW_YBS_MCU_Pin          GPIO_PIN_15
#define CW_YBS_MCU_GPIO_Port    GPIOC
#define OSC_IN_Pin              GPIO_PIN_0
#define OSC_IN_GPIO_Port        GPIOD
#define OSC_OUT_Pin             GPIO_PIN_1
#define OSC_OUT_GPIO_Port       GPIOD
#define T2_L_Pin                GPIO_PIN_0
#define T2_L_GPIO_Port          GPIOC
#define T2_H_Pin                GPIO_PIN_1
#define T2_H_GPIO_Port          GPIOC
#define BS_POWER_ON_Pin         GPIO_PIN_2
#define BS_POWER_ON_GPIO_Port   GPIOC
#define T1_OC_MCU_Pin           GPIO_PIN_3
#define T1_OC_MCU_GPIO_Port     GPIOC
#define Ia_Pin                  GPIO_PIN_0
#define Ia_GPIO_Port            GPIOA
#define Ib_Pin                  GPIO_PIN_1
#define Ib_GPIO_Port            GPIOA
#define MCU_UART1_TX_Pin        GPIO_PIN_2
#define MCU_UART1_TX_GPIO_Port  GPIOA
#define MCU_UART1_RX_Pin        GPIO_PIN_3
#define MCU_UART1_RX_GPIO_Port  GPIOA
#define Ic_Pin                  GPIO_PIN_4
#define Ic_GPIO_Port            GPIOA
#define Phase_A_Pin             GPIO_PIN_5
#define Phase_A_GPIO_Port       GPIOA
#define Phase_B_Pin             GPIO_PIN_6
#define Phase_B_GPIO_Port       GPIOA
#define Phase_C_Pin             GPIO_PIN_7
#define Phase_C_GPIO_Port       GPIOA
#define V_POWER_Pin             GPIO_PIN_4
#define V_POWER_GPIO_Port       GPIOC
#define V_BATTERY_Pin           GPIO_PIN_5
#define V_BATTERY_GPIO_Port     GPIOC
#define T2_OC_MCU_Pin           GPIO_PIN_0
#define T2_OC_MCU_GPIO_Port     GPIOB
#define B_OC_MCU_Pin            GPIO_PIN_1
#define B_OC_MCU_GPIO_Port      GPIOB
#define Precharge_Pin           GPIO_PIN_2
#define Precharge_GPIO_Port     GPIOB
#define DC_OC_1_MCU_Pin         GPIO_PIN_10
#define DC_OC_1_MCU_GPIO_Port   GPIOB
#define FG_FENG_MCU_Pin         GPIO_PIN_11
#define FG_FENG_MCU_GPIO_Port   GPIOB
#define I_BUS_OVER_Pin          GPIO_PIN_12
#define I_BUS_OVER_GPIO_Port    GPIOB
#define PWM_AL_Pin              GPIO_PIN_13
#define PWM_AL_GPIO_Port        GPIOB
#define PWM_BL_Pin              GPIO_PIN_14
#define PWM_BL_GPIO_Port        GPIOB
#define PWM_CL_Pin              GPIO_PIN_15
#define PWM_CL_GPIO_Port        GPIOB
#define PWM_B_MCU_Pin           GPIO_PIN_6
#define PWM_B_MCU_GPIO_Port     GPIOC
#define PWM_F_MCU_Pin           GPIO_PIN_7
#define PWM_F_MCU_GPIO_Port     GPIOC
#define PWM_YBS_MCU_Pin         GPIO_PIN_8
#define PWM_YBS_MCU_GPIO_Port   GPIOC
#define FG_YBS_MCU_Pin          GPIO_PIN_9
#define FG_YBS_MCU_GPIO_Port    GPIOC
#define PWM_AH_Pin              GPIO_PIN_8
#define PWM_AH_GPIO_Port        GPIOA
#define PWM_BH_Pin              GPIO_PIN_9
#define PWM_BH_GPIO_Port        GPIOA
#define PWM_CH_Pin              GPIO_PIN_10
#define PWM_CH_GPIO_Port        GPIOA
#define CAN_RX_Pin              GPIO_PIN_11
#define CAN_RX_GPIO_Port        GPIOA
#define CAN_TX_Pin              GPIO_PIN_12
#define CAN_TX_GPIO_Port        GPIOA
#define SWDIO_Pin               GPIO_PIN_13
#define SWDIO_GPIO_Port         GPIOA
#define SWCLK_Pin               GPIO_PIN_14
#define SWCLK_GPIO_Port         GPIOA
#define CW_ZBS_MCU_Pin          GPIO_PIN_15
#define CW_ZBS_MCU_GPIO_Port    GPIOA
#define T1_L_Pin                GPIO_PIN_10
#define T1_L_GPIO_Port          GPIOC
#define T1_H_Pin                GPIO_PIN_11
#define T1_H_GPIO_Port          GPIOC
#define M_UART4_TX_Pin          GPIO_PIN_12
#define M_UART4_TX_GPIO_Port    GPIOC
#define M_UART4_RX_Pin          GPIO_PIN_2
#define M_UART4_RX_GPIO_Port    GPIOD
#define JTDO_3_Pin              GPIO_PIN_3
#define JTDO_3_GPIO_Port        GPIOB
#define B_IO_Pin                GPIO_PIN_4
#define B_IO_GPIO_Port          GPIOB
#define Relay_control_Pin       GPIO_PIN_5
#define Relay_control_GPIO_Port GPIOB
#define EEPROM_SCL_M_Pin        GPIO_PIN_6
#define EEPROM_SCL_M_GPIO_Port  GPIOB
#define EEPROM_SDA_M_Pin        GPIO_PIN_7
#define EEPROM_SDA_M_GPIO_Port  GPIOB
#define XS_MCU_Pin              GPIO_PIN_8
#define XS_MCU_GPIO_Port        GPIOB
#define FG_ZBS_MCU_Pin          GPIO_PIN_9
#define FG_ZBS_MCU_GPIO_Port    GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
