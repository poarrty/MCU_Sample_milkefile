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

#include "math.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>

#include "cmsis_gcc.h"
#include "cmsis_os.h"
#include "task.h"
#include <stm32f1xx.h>
// #include "sys_pubsub.h"

#include "SEGGER_RTT.h"
#include "elog_port.h"
#include "flashdb.h"
#include "log.h"
#include "shell.h"
#include "shell_port.h"

// #include "drv_common.h"
// #include "bsp_common.h"
#include "app.h"
#include "fal_common.h"
// #include "hal.h"
// #include "pal.h"
#include "common.h"

#include "extern_init.h"

#define RT_USING_FINSH

#ifdef __cplusplus
extern "C" {
#endif

#define RT_ASSERT(EX)                                                                                  \
    if (!(EX)) {                                                                                       \
        log_w("(%s) assertion failed at function:%s, line number:%d \n", #EX, __FUNCTION__, __LINE__); \
    }

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
