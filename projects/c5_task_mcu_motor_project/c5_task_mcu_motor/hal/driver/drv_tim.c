/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_tim.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:58:20
 * @Description: TIM中断回调接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "common_def.h"
#include "log.h"
#include "tim.h"
#include <string.h>
#define LOG_TAG "drv_tim"
#ifdef USED_DRV_TIM

/******************************************************************************
 * @Function: drv_tim_init
 * @Description: TIM底层驱动接口初始化
 * @Input: void
 * @Output: None
 * @Return: HAL_StatusTypeDef
 * @Others: None
 *******************************************************************************/
HAL_StatusTypeDef drv_tim_init(void) {
    HAL_StatusTypeDef status;

    status = HAL_TIM_Base_Start_IT(&htim1);

    if (status != HAL_OK) {
        log_e("Return val:%d", status);
        return status;
    }

    return status;
}

/******************************************************************************
 * @Function: drv_tim1_update_irq_callbake
 * @Description: TIM1更新中断回调函数
 * @Input: 传入的TIM句柄
 * @Output: None
 * @Return: void
 * @Others: 定时周期5ms，用于基本定时
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void drv_tim1_update_irq_callbake(TIM_HandleTypeDef *htim) {}

/******************************************************************************
 * @Function: HAL_TIM_PeriodElapsedCallback
 * @Description: TIM更新中断回调函数
 * @Input: 传入的TIM句柄
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {TIM_HandleTypeDef} *htim
 *******************************************************************************/
void MX_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        drv_tim1_update_irq_callbake(htim);
    }
}

#endif