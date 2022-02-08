/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim_ic.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: timer imput capture driver
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __DRV_TIM_IC_H
#define __DRV_TIM_IC_H

#include <board.h>

#define USE_IC_TIMER 4

#if USE_IC_TIMER == 4
#define IC_TIMER              TIM4
#define IC_TIMER_IRQ          TIM4_IRQn
#define IC_TIMER_CLK_ENABLE() __HAL_RCC_TIM4_CLK_ENABLE()
#define IC_TIMER_IRQHandler   TIM4_IRQHandler
#define IC_TIMER_CHANNAL      TIM_CHANNEL_4
#endif

void ic_timx_chx_enable(void);
void ic_timx_chx_disable(void);
#endif
