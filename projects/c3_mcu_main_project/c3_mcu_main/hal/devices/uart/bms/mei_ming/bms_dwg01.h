/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         liuchunyang
 ** Version:        V0.0.1
 ** Date:           2021-9-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-09 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

#ifndef _BMS_DWG01_H
#define _BMS_DWG01_H

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "main.h"
#include "devices.h"
#include "lwrb.h"
#include "define.h"
/*****************************************************************
 * 宏定义
 ******************************************************************/

/**
 * @ingroup Robot_BMS_DWG01
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
int32_t bms_dwg01_open(const struct ca_device *dev, int32_t flags);
int32_t bms_dwg01_close(const struct ca_device *dev);
int32_t bms_dwg01_read(const struct ca_device *dev, void *buffer, uint32_t size);
int32_t bms_dwg01_write(const struct ca_device *dev, void *buffer, uint32_t size);
int32_t bms_dwg01_ioctl(const struct ca_device *dev, uint32_t cmd, void *arg);
int32_t bms_dwg01_init(struct ca_device *dev);
int32_t bms_dwg01_deinit(struct ca_device *dev);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_BMS_DWG01 */

#endif
