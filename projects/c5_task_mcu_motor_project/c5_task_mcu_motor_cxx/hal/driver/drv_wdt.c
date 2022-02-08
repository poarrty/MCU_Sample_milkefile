/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-12-07     balanceTWK        first version
 */
#include "watchdog.h"
#define LOG_TAG "drv_wdt"
#include "elog.h"
#ifdef RT_USING_WDT

struct stm32_wdt_obj {
    rt_watchdog_t      watchdog;
    IWDG_HandleTypeDef hiwdg;
    rt_uint16_t        is_start;
};

struct stm32_wdt_obj   stm32_wdt;
struct rt_watchdog_ops ops;

static rt_err_t wdt_init(rt_watchdog_t *wdt) {
    return RT_EOK;
}

static rt_err_t wdt_control(rt_watchdog_t *wdt, int cmd, void *arg) {
    rt_uint16_t time = *((rt_uint16_t *) arg);

    switch (cmd) {
        /* feed the watchdog */
        case RT_DEVICE_CTRL_WDT_KEEPALIVE:
            if (HAL_IWDG_Refresh(&stm32_wdt.hiwdg) != HAL_OK) {
                log_e("watch dog keepalive fail.");
            }

            break;

        /* set watchdog timeout */
        case RT_DEVICE_CTRL_WDT_SET_TIMEOUT:
#if defined(LSI_VALUE)
            if (LSI_VALUE) {
                stm32_wdt.hiwdg.Init.Reload = time * LSI_VALUE / (1ul << (stm32_wdt.hiwdg.Init.Prescaler + 2)) / 1000;
            } else {
                log_e("Please define the value of LSI_VALUE!");
            }

            if (stm32_wdt.hiwdg.Init.Reload > 0xFFF) {
                log_e("wdg set timeout parameter too large, please less than %ds",
                      1000 * 0xFFF * (4 * (1ul << stm32_wdt.hiwdg.Init.Prescaler)) / LSI_VALUE);
                return -RT_EINVAL;
            }

#else
#error "Please define the value of LSI_VALUE!"
#endif

            if (stm32_wdt.is_start) {
                if (HAL_IWDG_Init(&stm32_wdt.hiwdg) != HAL_OK) {
                    log_e("wdg set timeout failed.");
                    return -RT_ERROR;
                }
            }

            break;

        case RT_DEVICE_CTRL_WDT_GET_TIMEOUT:
#if defined(LSI_VALUE)
            if (LSI_VALUE) {
                (*((rt_uint32_t *) arg)) = 1000 * stm32_wdt.hiwdg.Init.Reload * (4 * (1ul << stm32_wdt.hiwdg.Init.Prescaler)) / LSI_VALUE;
            } else {
                log_e("Please define the value of LSI_VALUE!");
            }

#else
#error "Please define the value of LSI_VALUE!"
#endif
            break;

        case RT_DEVICE_CTRL_WDT_START:
            if (HAL_IWDG_Init(&stm32_wdt.hiwdg) != HAL_OK) {
                log_e("wdt start failed.");
                return -RT_ERROR;
            }

            stm32_wdt.is_start = 1;
            break;

        default:
            log_w("This command is not supported.");
            return -RT_ERROR;
    }

    return RT_EOK;
}

int rt_wdt_init(void) {
#ifdef user_init
//
#if defined(SOC_SERIES_STM32H7)
    stm32_wdt.hiwdg.Instance = IWDG1;
#else
    stm32_wdt.hiwdg.Instance = IWDG;
#endif
    stm32_wdt.hiwdg.Init.Prescaler = IWDG_PRESCALER_256;

    stm32_wdt.hiwdg.Init.Reload = 0x00000FFF;
#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32H7) || \
    defined(SOC_SERIES_STM32L0)
    stm32_wdt.hiwdg.Init.Window = 0x00000FFF;
#endif
    stm32_wdt.is_start = 0;

#else
    /* keep same with stm32cubemx */
    stm32_wdt.hiwdg.Instance       = IWDG;
    stm32_wdt.hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    stm32_wdt.hiwdg.Init.Reload    = 0x00000FFF;
    stm32_wdt.is_start             = 1;
#endif

    ops.init               = &wdt_init;
    ops.control            = &wdt_control;
    stm32_wdt.watchdog.ops = &ops;

    return RT_EOK;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rt_wdt_init, rt_wdt_init, rt_wdt_init);

#endif /* RT_USING_WDT */
