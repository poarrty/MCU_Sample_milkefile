/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS清洁组件检测
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <deszc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/


#ifndef _PAL_CLEAN_H
#define _PAL_CLEAN_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include <stdio.h>
#include <string.h>
#include "devices.h"
#include "define.h"
#include "define_button.h"

/**
 * @ingroup Robot_PAL 协议适配层
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
typedef enum button_clean
{
#ifdef CLEAN_WATER_BOX
    BUTTON_CLEAN_CLEAN_WATRE_BOX,        //清水箱在位检测
#endif

#ifdef WATER_POSITION
    BUTTON_CLEAN_WATER_POSITION,         //水位检测
#endif

#ifdef DIRT_BOX
    BUTTON_CLEAN_DIRT_BOX,         //水位检测
#endif
    
    CLEAN_BUTTON_MAX

}BUTTON_CLEAN_E;                         //按键类清洁组件数量

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void emerg_exit_callback(const struct ca_device *dev, uint16_t GPIO_Pin);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_PAL */

#endif
