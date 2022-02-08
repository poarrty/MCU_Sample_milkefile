/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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
#define DL_I_MCU_Pin GPIO_PIN_2
#define DL_I_MCU_GPIO_Port GPIOE
#define DL_I_MCU_EXTI_IRQn EXTI2_IRQn
#define MX_IO_DIRT_BOX_Pin GPIO_PIN_13
#define MX_IO_DIRT_BOX_GPIO_Port GPIOC
#define MX_IO_WATER_BOX_Pin GPIO_PIN_14
#define MX_IO_WATER_BOX_GPIO_Port GPIOC
#define M_R_EN_Pin GPIO_PIN_15
#define M_R_EN_GPIO_Port GPIOF
#define M_L_EN_Pin GPIO_PIN_12
#define M_L_EN_GPIO_Port GPIOE
#define MX_5V_M_EN_Pin GPIO_PIN_11
#define MX_5V_M_EN_GPIO_Port GPIOD
#define PWR_ON_OFF_US_Pin GPIO_PIN_4
#define PWR_ON_OFF_US_GPIO_Port GPIOG
#define MX_USART1_TX_CARPET_Pin GPIO_PIN_9
#define MX_USART1_TX_CARPET_GPIO_Port GPIOA
#define MX_USART1_RX_CARPET_Pin GPIO_PIN_10
#define MX_USART1_RX_CARPET_GPIO_Port GPIOA
#define DCF1_EN_Pin GPIO_PIN_3
#define DCF1_EN_GPIO_Port GPIOD
#define MX_12V_EXT_EN_Pin GPIO_PIN_7
#define MX_12V_EXT_EN_GPIO_Port GPIOD
#define MX_3V3_M_Pin GPIO_PIN_10
#define MX_3V3_M_GPIO_Port GPIOG
#define DL_O_MCU_Pin GPIO_PIN_13
#define DL_O_MCU_GPIO_Port GPIOG
#define DCF2_EN_Pin GPIO_PIN_0
#define DCF2_EN_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
