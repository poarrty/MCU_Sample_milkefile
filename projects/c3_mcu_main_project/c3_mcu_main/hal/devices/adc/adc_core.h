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
  ** <adce>          <author>    <version >    <desc>
  ** 2021-3-25       sulikang    0.0.1         创建文件
******************************************************************/

#ifndef _adc_CORE_H
#define _adc_CORE_H

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

/*****************************************************************
* 结构定义
******************************************************************/

/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/

uint16_t TIM_ADC_GET(const struct ca_bus *bus);
uint16_t adc_read(const struct ca_bus *bus, uint32_t channel);

/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */


#endif
