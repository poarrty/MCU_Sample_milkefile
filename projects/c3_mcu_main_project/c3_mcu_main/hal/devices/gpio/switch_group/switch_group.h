/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         张博炜
  ** Version:        V0.0.1
  ** Date:           2021-11-5
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-10 张博炜 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-11-5       张博炜	     0.0.1         创建文件
******************************************************************/

#ifndef _switch_group_H
#define _switch_group_H

/*****************************************************************
* 包含头文件
******************************************************************/
#include "main.h"
#include "devices.h"
/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_switch_group
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
#define       SWITCH_NUM                              1          //切换开关数量
#define       SWITCH_MAX_GPIO_NUM                     3          //切换开关群中最多gpio的切换开关的gpio数量 注册切换开关时都以最大的gpio数注册
#define       GPIO_BE_REGISTERED_NUM                  SWITCH_NUM*SWITCH_MAX_GPIO_NUM// 
/*****************************************************************
* 结构定义
******************************************************************/


/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/
int32_t switch_group_open(const struct ca_device *dev, int32_t flags);
int32_t switch_group_close(const struct ca_device *dev);
int32_t switch_group_read(const struct ca_device *dev, void *buffer, uint32_t size);
int32_t switch_group_write(const struct ca_device *dev, void *buffer, uint32_t size);
int32_t switch_group_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg);
int32_t switch_group_init(struct ca_device *dev);
int32_t switch_group_deinit(struct ca_device *dev);


/*****************************************************************
* 函数说明
******************************************************************/
int32_t switch_group_bus_find_name(struct ca_device *dev,struct ca_bus **my_bus);
int32_t switch_group_unit_ioctl(struct ca_bus *ca_bus_array_p,  uint32_t cmd,void *arg);
#ifdef __cplusplus
}
#endif

/* @} Robot_switch_group */


#endif
