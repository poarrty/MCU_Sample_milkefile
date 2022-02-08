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

#ifndef _TIM_CORE_H
#define _TIM_CORE_H

/*****************************************************************
* 包含头文件
******************************************************************/
#include "main.h"
#include "devices.h"
/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_UART_CORE
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
	TIM_IC_RISING,
	TIM_IC_FALLING,
	TIM_IC_BOTH_EDGE
}TIM_IC_INT_TYPE;
/*****************************************************************
* 结构定义
******************************************************************/

/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/
typedef  void (*tim_callback)(const struct ca_device *dev, void *handler); 
int32_t tim_ic_capture_register_callback(const struct ca_device *dev, void *handler, tim_callback callback);
int32_t tim_period_elapsed_register_callback(const struct ca_device *dev, void *handler, tim_callback callback);

int32_t tim_capture_start(const struct ca_bus *bus, TIM_IC_INT_TYPE polarity);
int32_t tim_capture_stop(const struct ca_bus *bus);
int32_t tim_capture_read(const struct ca_bus *bus);

int32_t tim_pwm_start(const struct ca_bus *bus, float duty_cycle, uint32_t pulsesNum);
int32_t tim_pwm_stop(const struct ca_bus *bus);
int32_t tim_pwm_set_duty_cycle(const struct ca_bus *bus, uint32_t duty_cycle);

int32_t tim_etr_start(const struct ca_bus *bus);
int32_t tim_get_counter(const struct ca_bus *bus);
int32_t tim_set_counter(const struct ca_bus *bus,uint32_t counter);
int32_t tim_get_period(const struct ca_bus *bus);

int32_t tim_period_elapsed_start(const struct ca_bus *bus);
void MX_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */


#endif
