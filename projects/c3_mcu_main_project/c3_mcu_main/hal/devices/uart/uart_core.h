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

#ifndef _UART_CORE_H
#define _UART_CORE_H

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
struct rs485_handler{
	struct ca_bus *uart;
	struct ca_bus *gpio_ctrl;
	int8_t gpio_read_status;
};

/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/
typedef  void (*uart_rx_callback)(const struct ca_device *dev, uint8_t rx_buf); 
int32_t uart_rx_register_callback(const struct ca_device *dev, uart_rx_callback callback);

int32_t uart_read(const struct ca_bus *bus, void *buffer, uint32_t len);
int32_t uart_write(const struct ca_bus *bus, void *buffer, uint32_t len);
int32_t rs485_read(const struct ca_bus *bus, void *buffer, uint32_t len);
int32_t rs485_write(const struct ca_bus *bus, void *buffer, uint32_t len);



/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */


#endif
