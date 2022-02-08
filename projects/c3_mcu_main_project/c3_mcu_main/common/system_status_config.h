#ifndef _SYSTEM_STATUS_CONFIG_H
#define _SYSTEM_STATUS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *系统时钟定时器,暂不支持systick
 */
#define SYS_HTIM htim6

/*
 *自定义RAM空间分配,不定义则默认分配2048内部RAM
 *需自定义char     cStringBuffer[X];
 */
// #define STRING_RAM_EXT

/*
 *默认htop功能查询时间,单位为秒
 *查询时间为0时,关闭htop功能
 */
#define DEFAULT_HTOP_INTERVAL   60

/*
 *系统状态打印接口,默认为printf
 */

#define SYS_PRINT(format, ...)  printf(format, ##__VA_ARGS__)
 
#ifdef __cplusplus
}
#endif

#endif
