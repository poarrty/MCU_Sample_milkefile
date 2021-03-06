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
#ifndef _BSP_RTC_H
#define _BSP_RTC_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/

/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup Robot_BSP
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
功  能: 初始化 bsp
返回值: 等于0表示成功，其它值表示失败原因
 *****************************************************************/
int bsp_init(void);

/*****************************************************************
功  能: 释放 bsp 资源
返回值: 等于0表示成功，其它值表示失败原因
******************************************************************/
int bsp_deInit(void);

int bsp_rtc_set(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_BSP */

#endif
