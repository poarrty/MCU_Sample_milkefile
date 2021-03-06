/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS安全管理
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/


#ifndef _PAL_SECURITY_H
#define _PAL_SECURITY_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/

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
typedef enum button_security
{
#ifdef EMERG
    BUTTON_SECURITY_EMERG,         //急停
#endif

#ifdef CRASH_LEFT
    BUTTON_SECURITY_CRASH_LEFT,    //左碰撞
#endif

#ifdef CRASH_RIGHT
    BUTTON_SECURITY_CRASH_RIGHT,   //右碰撞
#endif
    
    SECURITY_BUTTON_MAX

}BUTTON_SECURITY_E;                //安全类清洁组件数量

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/


/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_PAL */

#endif
