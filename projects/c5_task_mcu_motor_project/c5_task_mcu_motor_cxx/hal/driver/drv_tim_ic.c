/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim_ic.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: timer imput capture driver
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "drv_tim_ic.h"
#define LOG_TAG "drv_tim_ic"
#ifdef USED_DRV_TIM_IC

#define TIMx_Handler &htim4

extern TIM_HandleTypeDef htim4;

void ic_timx_chx_enable(void) {
    HAL_TIM_IC_Start_IT(TIMx_Handler, IC_TIMER_CHANNAL);  // en tim4 channel1 and IT
}

void ic_timx_chx_disable(void) {
    HAL_TIM_IC_Stop_IT(TIMx_Handler, IC_TIMER_CHANNAL);
}

// timer4 IT handle function
// void IC_TIMER_IRQHandler(void)
// {
//  HAL_TIM_IRQHandler(TIMx_Handler);              //timer it handle interface
// }

#endif