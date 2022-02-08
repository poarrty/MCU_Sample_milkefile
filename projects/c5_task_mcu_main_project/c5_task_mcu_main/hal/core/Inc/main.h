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
#define MX_3V3_STB_2EN_Pin                     GPIO_PIN_1
#define MX_3V3_STB_2EN_GPIO_Port               GPIOC
#define MX_RKUART_SWTICH_1_Pin                 GPIO_PIN_2
#define MX_RKUART_SWTICH_1_GPIO_Port           GPIOC
#define MX_RKUART_SWTICH_2_Pin                 GPIO_PIN_3
#define MX_RKUART_SWTICH_2_GPIO_Port           GPIOC
#define MX_UART2_TX_Pin                        GPIO_PIN_2
#define MX_UART2_TX_GPIO_Port                  GPIOA
#define MX_UART2_RX_Pin                        GPIO_PIN_3
#define MX_UART2_RX_GPIO_Port                  GPIOA
#define MX_SEWAGE_WATER_VALVE_DET_Pin          GPIO_PIN_6
#define MX_SEWAGE_WATER_VALVE_DET_GPIO_Port    GPIOA
#define MX_WASTE_WATER_VALVE_EN_Pin            GPIO_PIN_7
#define MX_WASTE_WATER_VALVE_EN_GPIO_Port      GPIOA
#define MX_WATER_LEVEL_DET_Pin                 GPIO_PIN_4
#define MX_WATER_LEVEL_DET_GPIO_Port           GPIOC
#define MX_WATER_LEVEL_SEL_01_Pin              GPIO_PIN_5
#define MX_WATER_LEVEL_SEL_01_GPIO_Port        GPIOC
#define MX_STRAINER_DET_Pin                    GPIO_PIN_0
#define MX_STRAINER_DET_GPIO_Port              GPIOB
#define MX_TM3_CH4_FILTER_PUMP_POWER_Pin       GPIO_PIN_1
#define MX_TM3_CH4_FILTER_PUMP_POWER_GPIO_Port GPIOB
#define MX_I2C2_SCL_Pin                        GPIO_PIN_10
#define MX_I2C2_SCL_GPIO_Port                  GPIOB
#define MX_I2C2_SDA_Pin                        GPIO_PIN_11
#define MX_I2C2_SDA_GPIO_Port                  GPIOB
#define MX_UART4_DATA_EN_Pin                   GPIO_PIN_12
#define MX_UART4_DATA_EN_GPIO_Port             GPIOB
#define MX_WATER_LEVEL_SEL_00_Pin              GPIO_PIN_14
#define MX_WATER_LEVEL_SEL_00_GPIO_Port        GPIOB
#define MX_3V3_STB_1EN_Pin                     GPIO_PIN_6
#define MX_3V3_STB_1EN_GPIO_Port               GPIOC
#define MX_24V_STB_EN_Pin                      GPIO_PIN_7
#define MX_24V_STB_EN_GPIO_Port                GPIOC
#define MX_TM8_CH3_LED_POWER_Pin               GPIO_PIN_8
#define MX_TM8_CH3_LED_POWER_GPIO_Port         GPIOC
#define MX_RK_POWER_EN_Pin                     GPIO_PIN_8
#define MX_RK_POWER_EN_GPIO_Port               GPIOA
#define MX_WASTE_WATER_VALVE_DET_Pin           GPIO_PIN_9
#define MX_WASTE_WATER_VALVE_DET_GPIO_Port     GPIOA
#define MX_KEY_BOARD_POWER_EN_Pin              GPIO_PIN_10
#define MX_KEY_BOARD_POWER_EN_GPIO_Port        GPIOA
#define MX_TOF_POWER_EN_Pin                    GPIO_PIN_15
#define MX_TOF_POWER_EN_GPIO_Port              GPIOA
#define MX_UART4_TX_Pin                        GPIO_PIN_10
#define MX_UART4_TX_GPIO_Port                  GPIOC
#define MX_UART4_RX_Pin                        GPIO_PIN_11
#define MX_UART4_RX_GPIO_Port                  GPIOC
#define MX_AURT5_TX_Pin                        GPIO_PIN_12
#define MX_AURT5_TX_GPIO_Port                  GPIOC
#define MX_UART5_RX_Pin                        GPIO_PIN_2
#define MX_UART5_RX_GPIO_Port                  GPIOD
#define MX_FILTER_PUMP_OC_DET_Pin              GPIO_PIN_4
#define MX_FILTER_PUMP_OC_DET_GPIO_Port        GPIOB
#define MX_SEWAGE_WATER_VALVE_EN_Pin           GPIO_PIN_5
#define MX_SEWAGE_WATER_VALVE_EN_GPIO_Port     GPIOB
/* USER CODE BEGIN Private defines */

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_UART5
//#define BSP_UART5_RX_USING_DMA
#define BSP_USING_UART2
//#define BSP_USING_UART3
//#define BSP_UART3_RX_USING_DMA
#define BSP_USING_UART4
#define BSP_UART4_RX_USING_DMA
#define BSP_USING_HW_I2C
//#define BSP_USING_SOFT_I2C
#define BSP_USING_I2C1
#define BSP_I2C1_SCL_PIN GET_PIN(B, 6)
#define BSP_I2C1_SDA_PIN GET_PIN(B, 7)
#define BSP_USING_I2C2
#define BSP_I2C2_SCL_PIN GET_PIN(B, 10)
#define BSP_I2C2_SDA_PIN GET_PIN(B, 11)
#define BSP_USING_PWM8
#define BSP_USING_PWM8_CH3
#define BSP_USING_PWM3
#define BSP_USING_PWM3_CH4
#define BSP_USING_CAN
#define BSP_USING_CAN1
#define BSP_USING_ON_CHIP_FLASH
/* Board extended module Drivers */

/*User defined macros*/
#define APP_MAJOR_VERSION  0
#define APP_MINOR_VERSION  0
#define APP_REVISE_VERSION 11
#define HW_MAJOR_VERSION   0
#define HW_MINOR_VERSION   1
#define HW_REVISE_VERSION  0

//#define USED_AUTO_POWER_ON
//#define USED_DEVICE_OUTPUT_TEST
//#define USED_IO_OUTPUT_TEST
#define USED_IO_WATER_LEVEL_SENSOR
#define USED_FUNCTION_KEY
#define USED_CELAN_DEVICE
#define USED_CLEAN_DETECTION
#define USED_FLITER_PUMP_CTRL
#define USED_FILTER_PUMP_DETECTION
#define USED_SEWAGE_WATER_CTRL
#define USED_SEWAGE_WATER_DETECTION
#define USED_CLEAN_WATER_CTRL
#define USED_CLEAN_WATER_DETECTION
#define USED_WASTE_WATER_CTRL
#define USED_WASTE_WATER_DETECTION
#define USED_LED_CTRL
#define USED_IWDG

#define RT_EOK      0 /**< There is no error */
#define RT_ERROR    1 /**< A generic error happens */
#define RT_ETIMEOUT 2 /**< Timed out */
#define RT_EFULL    3 /**< The resource is full */
#define RT_EEMPTY   4 /**< The resource is empty */
#define RT_ENOMEM   5 /**< No memory */
#define RT_ENOSYS   6 /**< No system */
#define RT_EBUSY    7 /**< Busy */
#define RT_EIO      8 /**< IO error */
#define RT_EINTR    9 /**< Interrupted system call */
#define RT_EINVAL   10
#define RT_NULL     (0)

#define PIN_LOW  0x00
#define PIN_HIGH 0x01

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
