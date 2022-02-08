﻿/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      hal.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-01-
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-01 robot创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/

#include "hal.h"
#include "devices.h"

/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_HAL 硬件适配层 - HAL
 *
 * @brief
 * HAL层屏蔽了硬件平台的差异，向上层提供了统一的调用函数接口，支持不同的硬件平台
 * \n \n
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
 * 全局变量定义
 ******************************************************************/
int led_handle = -1;

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
sensor_obj_st gpio_objs[] = {
	{
		.bus_name = "gpioA",
		.dev_name = "led",
		.dev_addr = 15,
		.handle = &led_handle
	}
};

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       hal_init
 * Description:    初始化HAL层
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
int  bsp_gpio_init(void)
{
	int index = 0;
	int ret = -1;

	for(index = 0; index < ITEM_NUM(gpio_objs); index++)
	{
		ret = device_register(gpio_objs[index].bus_name, gpio_objs[index].dev_name, 
													   gpio_objs[index].dev_addr, gpio_objs[index].args);
		*gpio_objs[index].handle = ret;
	}
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       hal_deinit
 * Description:    释放HAL层资源
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
int  bsp_gpio_deInit(void)
{
	/*释放资源*/
	return 0;
}

#ifdef __cplusplus
}
#endif

/* @} Robot_HAL */
/* @} Robot-NAV_407 */

/**************************** CMSIS ********************************/
/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup H_CMSIS 微控制器软件接口标准 - CMSIS
 *
 * @brief (CMSIS) 是 Cortex-M 处理器系列的与供应商无关的硬件抽象层。\n
 * CMSIS
 * 可实现与处理器和外设之间的一致且简单的软件接口，从而简化软件的重用，缩短微控制器开发人员新手的学习过程，并缩短新设备的上市时间。\n
 * \n
 * @{
 */

/* @} H_CMSIS */
/* @} Robot_HAL */

/**************************** Driver ********************************/
/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup H_DRIVER 外围驱动接口API - DRIVER
 *
 * @brief 包含处理器的外围驱动接口API。\n
 * \n
 * @{
 */

/* @} H_DRIVER */
/* @} Robot_HAL */