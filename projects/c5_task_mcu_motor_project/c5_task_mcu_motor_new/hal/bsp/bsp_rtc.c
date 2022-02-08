/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-9-1
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-09 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-9-1        sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "string.h"
#include "stdio.h"
#include "rtc.h"

/**
 * @addtogroup Robot_BSP
 * @{
 */

/**
 * @defgroup BSP_RTC  - RTC
 *
 * @brief  \n
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
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       bsp_rtc_init
 * Description:    初始化 rtc
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
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
int bsp_rtc_init(void) {
    /*添加模块处理函数*/

    return 0;
}

/*****************************************************************/
/**
 * Function:       bsp_rtc_deInit
 * Description:    释放 rtc 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
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
int bsp_rtc_deInit(void) {
    return 0;
}

int bsp_rtc_set(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec) {
    RTC_TimeTypeDef rtc_time = {0};
    RTC_DateTypeDef rtc_date = {0};

    rtc_date.Year  = year - 2000;
    rtc_date.Month = moon;
    rtc_date.Date  = date;

    rtc_time.Hours   = hour;
    rtc_time.Minutes = min;
    rtc_time.Seconds = sec;

    ///< RTC 设置精度是秒
    // rtc_time.SubSeconds = 1023 - msec;
    if (msec > 500) {
        rtc_time.Seconds += 1;
    }

    // log_i("bsp_rtc_set: %d-%d-%d %02d:%02d:%02d.%03d",
    //    rtc_date.Year + 2000, rtc_date.Month, rtc_date.Date, rtc_time.Hours,
    //    rtc_time.Minutes, rtc_time.Seconds, msec);

    HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

    return 0;
}

#ifdef __cplusplus
}
#endif

/* @} BSP_RTC */
/* @} Robot_BSP */
