/*************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:      bsp_gpio.h
  ** Author:
  ** Version:
  ** Date:
  ** Description:
  ** Others:
  ** Function List:
  ** History:

  ** <time>   <author>    <version >   <desc>
  **
*************************************************/
#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "devices.h"
#include "hal.h"

/**
 * @ingroup Robot_HAL 硬件适配层
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

/****************************************************
    功能：初始化FAL层
    返回值:等于0表示成功，其它值表示失败原因
 ***************************************************/
int bsp_gpio_init(void);

/****************************************************
    功能：释放FAL层资源
    返回值： 等于0表示成功，其它值表示失败原因
***************************************************/
int bsp_gpio_deInit(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_HAL */

#endif
