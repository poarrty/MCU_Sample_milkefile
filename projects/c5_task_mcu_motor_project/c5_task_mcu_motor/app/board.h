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

#include "drv_common.h"
#include "bsp_common.h"
#include "fal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RT_ASSERT(EX)                                                                                  \
    if (!(EX)) {                                                                                       \
        log_w("(%s) assertion failed at function:%s, line number:%d \n", #EX, __FUNCTION__, __LINE__); \
    }

#define rt_list_entry(node, type, member) rt_container_of(node, type, member)

#define rt_container_of(ptr, type, member) ((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
