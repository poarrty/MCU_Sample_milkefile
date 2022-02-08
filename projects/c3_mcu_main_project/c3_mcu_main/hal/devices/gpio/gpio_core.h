/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-25
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-3-25       sulikang    0.0.1         创建文件
******************************************************************/

#ifndef _GPIO_CORE_H
#define _GPIO_CORE_H

/*****************************************************************
* 包含头文件
******************************************************************/
#include "main.h"
#include "devices.h"
/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_GPIO_CORE
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
typedef enum{
	GPIO_EXTI_RISING,
	GPIO_EXTI_FALLING,
	GPIO_EXTI_BOTH_EDGE
}GPIO_EXTI_TYPE;

/*****************************************************************
* 结构定义
******************************************************************/
struct sw_handler{
	struct ca_bus *gpio_ctrl[3];
};

/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/
typedef  void (*gpio_exti_callback)(const struct ca_device *dev, uint16_t GPIO_Pin); 
int32_t gpio_exti_register_callback(const struct ca_device *dev, uint16_t gpio_pin, gpio_exti_callback callback);
int32_t gpio_exti_start(const struct ca_bus *bus, GPIO_EXTI_TYPE exti_type);
int32_t gpio_exti_stop(const struct ca_bus *bus);
int32_t gpio_read(const struct ca_bus *bus, void *buffer, uint32_t len);
int32_t gpio_write(const struct ca_bus *bus, void *buffer, uint32_t len);
int32_t gpio_ioctl(const struct ca_bus *bus,  uint32_t cmd, void *arg);


/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */


#endif
