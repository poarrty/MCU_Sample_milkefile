/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-7      SummerGift   first version
 */

#ifndef __DRV_COMMON_H__
#define __DRV_COMMON_H__

#include "board.h"

#include "can.h"
#include "drv_can.h"
#include "drv_gpio.h"
#include "drv_soft_i2c.h"
#include "drv_tim_ic.h"
#include "drv_tim.h"
#include "drv_usart.h"
#include "i2c-bit-ops.h"
#include "i2c.h"
#include "pin.h"
#include "watchdog.h"

#define USED_CPUTIME_CORTEXM
#define BSP_USING_CAN
#define BSP_USING_CAN1
#define RT_USING_PIN
//#define RT_USING_I2C
#define USED_DRV_TIM_IC
//#define USED_DRV_TIM
#define USED_DRV_USART
//#define RT_USING_WDT
// #define RT_USING_I2C_BITOPS

#ifdef __cplusplus
extern "C" {
#endif

#define SCB_CFSR        (*(volatile const unsigned *) 0xE000ED28) /* Configurable Fault Status Register */
#define SCB_HFSR        (*(volatile const unsigned *) 0xE000ED2C) /* HardFault Status Register */
#define SCB_MMAR        (*(volatile const unsigned *) 0xE000ED34) /* MemManage Fault Address register */
#define SCB_BFAR        (*(volatile const unsigned *) 0xE000ED38) /* Bus Fault Address Register */
#define SCB_AIRCR       (*(volatile unsigned long *) 0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                                /* Reset value, write to SCB_AIRCR can reset cpu */

#define SCB_CFSR_MFSR (*(volatile const unsigned char *) 0xE000ED28)  /* Memory-management Fault Status Register */
#define SCB_CFSR_BFSR (*(volatile const unsigned char *) 0xE000ED29)  /* Bus Fault Status Register */
#define SCB_CFSR_UFSR (*(volatile const unsigned short *) 0xE000ED2A) /* Usage Fault Status Register */

void _Error_Handler(char *s, int num);

#ifndef Error_Handler
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#endif

#define DMA_NOT_AVAILABLE ((DMA_INSTANCE_TYPE *) 0xFFFFFFFFU)

#ifndef ITEM_NUM
#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
#endif

int skip_atoh(char *s, rt_uint32_t *hex_data);

#ifdef __cplusplus
}
#endif

#endif
