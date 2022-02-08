/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_pwm.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: pwm operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_iwdg.h"
#define LOG_TAG "bsp_iwdg"
#include "elog.h"
//#define USED_IWDG

#ifdef USED_IWDG
extern IWDG_HandleTypeDef hiwdg;

int iwdg_feed(void) {
    /* feed dog */
    if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK) {
        log_e("watch dog keepalive fail.");
    }

    return HAL_OK;
}

int iwdg_write_timeout(uint16_t time) {
    uint32_t Reload;

    if (!LSI_VALUE) {
        log_e("Please define the value of LSI_VALUE!");
    }

    hiwdg.Instance = IWDG;

    Reload = time * LSI_VALUE / (1ul << (hiwdg.Init.Prescaler + 2)) / 1000;

    if (Reload > 0xFFF) {
        log_e("wdg set timeout parameter too large, please less than %lds", 1000 * 0xFFF * (4 * (1ul << hiwdg.Init.Prescaler)) / LSI_VALUE);
        return -1;
    }

    hiwdg.Init.Reload = Reload;

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        log_e("wdg set timeout failed.");
        return -HAL_ERROR;
    }

    return HAL_OK;
}
#endif
