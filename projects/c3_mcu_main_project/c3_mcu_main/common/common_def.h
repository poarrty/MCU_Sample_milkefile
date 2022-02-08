/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: common_def.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-04-30 11:52:10
 * @Description: 公共定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __COMMON_CONFIG_H__
#define __COMMON_CONFIG_H__

#include <stdint.h>
#include <stdio.h>

#ifndef __LOG_EN__
#define __LOG_EN__ 1
#endif

#define SYS_SET_BIT(val, bit) ((val) |= (1 << bit))
#define SYS_GET_BIT(val, bit) ((val >> bit) & 1)
#define SYS_CLR_BIT(val, bit) ((val) &= (~(1 << bit)))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#if __LOG_EN__
typedef enum {
    // LOG打印关闭
    LOG_LEVEL_OFF = 0,
    //打印错误
    LOG_LEVEL_ERROR,
    //打印警告
    LOG_LEVEL_WARNING,
    //打印调试信息
    LOG_LEVEL_DEBUG,
    //打印数组信息
    LOG_LEVEL_DEBUG_ARRAY,
    //打印信息
    LOG_LEVEL_INFO,
} log_level_enum_t;

extern log_level_enum_t log_level;
#endif

void common_def_init(void);

#endif
