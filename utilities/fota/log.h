/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: log.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:18:30
 * @Description: LOG接口定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <string.h>
#include <stdio.h>

#if 1

char log_buff[512];

#define COLOR_END "\033[0m"
#define INFO_COLOR "\033[36m"
#define DEBUG_COLOR "\033[35m"
#define DEBUG_ARRAY_COLOR "\033[35m"
#define WARNING_COLOR "\033[33m"
#define ERROR_COLOR "\033[31m"

#define filename(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x

#define LOG_INFO(format, ...)                                        \
    do {                                                             \
            printf(INFO_COLOR "[log_i]:%s[%04d]: " format        \
                                  "\r\n" COLOR_END,                  \
                       filename(__FILE__), __LINE__, ##__VA_ARGS__); \
    } while (0)

#define LOG_DEBUG(format, ...)                                       \
    do {                                                             \
            printf(DEBUG_COLOR "[log_d]:%s[%04d]: " format       \
                                   "\r\n" COLOR_END,                 \
                       filename(__FILE__), __LINE__, ##__VA_ARGS__); \
    } while (0)

#define LOG_CUSTOM(format, ...) printf(format, ##__VA_ARGS__);

#else

#define LOG_INFO(format, ...) \
    do {                      \
    } while (0)
#define LOG_DEBUG(format, ...) \
    do {                       \
    } while (0)
#define LOG_DEBUG_ARRAY(array, num) \
    do {                            \
    } while (0)
#define LOG_WARNING(format, ...) \
    do {                         \
    } while (0)
#define LOG_ERROR(format, ...) \
    do {                       \
    } while (0)
#define LOG_CUSTOM(format, ...) \
    do {                        \
    } while (0)

#endif

#define Error_Check_Callbake(status)            \
    do {                                        \
        if (status != 0) {                      \
            LOG_ERROR("Return val:%d", status); \
        }                                       \
    } while (0);

#endif
