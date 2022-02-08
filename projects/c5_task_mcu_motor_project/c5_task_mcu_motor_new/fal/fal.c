/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 09:31:03
 * @Description: 功能抽象层接口入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal.h"
#include "FreeRTOS.h"
#include "shell_port.h"
#include "stdio.h"

#define LOG_TAG "fal"
#include "elog.h"
/**
 * @addtogroup Robot-NAV_103
 * @{
 */

/**
 * @defgroup Robot_FAL 功能适配层 - FAL
 *
 * @brief FAL层实现各种通用的功能模块，并提供统一的接口，为上层调用；
 *        为了便于处理不同的协议，降低协议在上下两层相互转换的复杂和提高系统性能，
 *        该层还支持协议透传功能，即允许上层直接将协议数据发送下来，然后指定到特定的协议模块处理\n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern void MX_WWDG_Init(void);

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
int set_local_rtc(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_deinit
 * Description:    释放FAL层资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_deInit(void) {
    return 0;
}

int set_local_rtc(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec) {
    // RTC_TimeTypeDef rtc_time = {0};
    // RTC_DateTypeDef rtc_date = {0};

    // rtc_date.Year = year - 2000;
    // rtc_date.Month = moon;
    // rtc_date.Date = date;

    // rtc_time.Hours = hour;
    // rtc_time.Minutes = min;
    // rtc_time.Seconds = sec;
    // rtc_time.SubSeconds = 1.0 * msec / 1000 * 255;

    // log_w("Set_local_rtc: %d-%d-%d %02d:%02d:%02d.%03d",
    //    rtc_date.Year + 2000, rtc_date.Month, rtc_date.Date, rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, msec);

    // HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
    // HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_103 */

// /***************************/

// /******************************************************************************
//  * @Function: fal_init
//  * @Description: 功能抽象层初始化函数
//  * @Input: void
//  * @Output: None
//  * @Return: void
//  * @Others: None
// *******************************************************************************/
// int fal_init(void)
// {
//     fal_version_init();

//     return 0;
// }

// /******************************************************************************
//  * @Function: fal_DeInit
//  * @Description: 功能抽象层反初始化函数
//  * @Input: void
//  * @Output: None
//  * @Return: void
//  * @Others: None
// *******************************************************************************/
// int fal_DeInit(void)
// {
//     return 0;
// }
