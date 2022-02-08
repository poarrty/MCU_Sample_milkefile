/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-05     Xeon Xu      the first version
 * 2019-01-22     YLZ          port from stm324xx-HAL to bsp stm3210x-HAL
 * 2019-01-26     YLZ          redefine `struct stm32_drv_can` add member `Rx1Message`
 * 2019-02-19     YLZ          port to BSP [stm32]
 * 2019-06-17     YLZ          modify struct stm32_drv_can.
 */

#ifndef __DRV_CAN_H__
#define __DRV_CAN_H__

#include <board.h>
#include "can.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS1SHIFT    16
#define BS2SHIFT    20
#define RRESCLSHIFT 0
#define SJWSHIFT    24
#define BS1MASK     ((0x0F) << BS1SHIFT)
#define BS2MASK     ((0x07) << BS2SHIFT)
#define RRESCLMASK  (0x3FF << RRESCLSHIFT)
#define SJWMASK     (0x3 << SJWSHIFT)

struct stm32_baud_rate_tab {
    rt_uint32_t baud_rate;
    rt_uint32_t config_data;
};
#define BAUD_DATA(TYPE, NO) ((can_baud_rate_tab[NO].config_data & TYPE##MASK))

/* stm32 can device */
struct stm32_can {
    char *               name;
    CAN_HandleTypeDef    CanHandle;
    CAN_FilterTypeDef    FilterConfig;
    struct rt_can_device device; /* inherit from can device */
};

extern const struct rt_can_ops _can_ops;
extern struct stm32_can        drv_can1;

int rt_hw_can_init(void);
int _can_int_rx(struct rt_can_device *can, struct rt_can_msg *data, int msgs);

#ifdef __cplusplus
}
#endif

#endif /*__DRV_CAN_H__ */

/************************** end of file ******************/
