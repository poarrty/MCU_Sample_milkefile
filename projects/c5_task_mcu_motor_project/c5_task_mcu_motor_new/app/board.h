/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: rtconfig.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: global define
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include "rtconfig.h"
#include "rtdef.h"

#include "stdbool.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <unistd.h>

#include <stm32f1xx.h>
#include "cmsis_os.h"
#include "cmsis_gcc.h"
#include "task.h"
#include "sys_pubsub.h"

#include "log.h"
#include "elog_port.h"
#include "shell.h"
#include "shell_port.h"
#include "flashdb.h"

#include "drv_common.h"
#include "bsp_common.h"
#include "fal_common.h"
#include "app.h"
#include "hal.h"
#include "pal.h"
#include "common.h"

#define RT_USING_FINSH

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*init_fn_t)(void);

#define RT_ASSERT(EX)                                                                                  \
    if (!(EX)) {                                                                                       \
        log_w("(%s) assertion failed at function:%s, line number:%d \n", #EX, __FUNCTION__, __LINE__); \
    }

/* board init routines will be called in board_init() function */
#define INIT_BOARD__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("board"))) = fn

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define INIT_PREV__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("prev"))) = fn
/* device initialization */
#define INIT_DEVICE__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("device"))) = fn
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("component"))) = fn
/* environment initialization (mount disk, ...) */
#define INIT_ENV__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("env"))) = fn
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("app"))) = fn

#define INIT__EXPORT(s, e)             \
    do {                               \
        uint32_t *p = (uint32_t *) &s; \
        while (p != (uint32_t *) &e)   \
            ((init_fn_t) *p++)();      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
