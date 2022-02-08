/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-7      SummerGift   first version
 */

#include "drv_common.h"
#define LOG_TAG "drv_common"
#include "elog.h"

static void reboot_sys(uint8_t argc, char **argv) {
    SCB_AIRCR = SCB_RESET_VALUE;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), reboot_sys, reboot_sys, Reboot System);

static int stm32_reg_ops(uint8_t argc, char **argv) {
    uint32_t reg_addr = 0;
    uint32_t data     = 0;

    if (argc < 2) {
        log_d("input param error\r\n");
        return -RT_ERROR;
    }

    if (argc == 2) {
        if (skip_atoh(argv[1], (rt_uint32_t *) &reg_addr) < 0) {
            log_d("read reg addr error\r\n");
            return -RT_ERROR;
        }
    } else {
        if (skip_atoh(argv[1], (rt_uint32_t *) &reg_addr) < 0) {
            log_d("write reg addr error\r\n");
            return -RT_ERROR;
        }

        if (skip_atoh(argv[2], (rt_uint32_t *) &data) < 0) {
            log_d("write reg data error\r\n");
            return -RT_ERROR;
        }

        *((rt_uint32_t *) reg_addr) = data;
    }

    log_d("0x%08x: 0x%08x\r\n", (unsigned int) reg_addr, (unsigned int) *(rt_uint32_t *) reg_addr);
    return RT_EOK;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), stm32_reg_ops, stm32_reg_ops, reg 8000000);

int skip_atoh(char *s, rt_uint32_t *hex_data) {
    rt_uint8_t str_len = 0;
    rt_int32_t temp    = 0;
    char *     str     = s;

    if (s == RT_NULL || hex_data == RT_NULL) {
        return -RT_ERROR;
    }

    str_len = rt_strlen(s);

    // remove 0x
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str_len -= 2;
        str += 2;
    }

    // Only 32-bit addresses are supported
    if (str_len > 8) {
        return -RT_ERROR;
    }

    for (int i = str_len - 1; i >= 0; i--) {
        if (str[i] >= '0' && str[i] <= '9') {
            temp += ((str[i] - '0') << (4 * (str_len - 1 - i)));
        } else if (str[i] >= 'A' && str[i] <= 'F') {
            temp += ((str[i] - 'A' + 10) << (4 * (str_len - 1 - i)));
        } else if (str[i] >= 'a' && str[i] <= 'f') {
            temp += ((str[i] - 'a' + 10) << (4 * (str_len - 1 - i)));
        } else {
            return -RT_ERROR;
        }
    }

    *hex_data = temp;
    return RT_EOK;
}

static __inline int32_t bit2bitno(uint32_t bit) {
    int i;

    for (i = 0; i < 32; i++) {
        if ((0x01 << i) == bit) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char *s, int num) {
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
    }

    /* USER CODE END Error_Handler */
}

/**
 * This function will delay for some us.
 *
 * @param us the delay time of us
 */
void rt_hw_us_delay(rt_uint32_t us) {
    rt_uint32_t start, now, delta, reload, us_tick;
    start   = SysTick->VAL;
    reload  = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;

    do {
        now   = SysTick->VAL;
        delta = start >= now ? start - now : reload + start - now;
    } while (delta < us_tick * us);
}