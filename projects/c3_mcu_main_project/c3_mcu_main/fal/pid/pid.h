


/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/


#ifndef _PID_H
#define _PID_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
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
#define ITERATION_NUM 50
#define SET_VALUE 300.0
#define PID_CONV_REAL 1.0
/*****************************************************************
 * 结构定义
 ******************************************************************/
struct _pid{
        float SetSpeed;                    //设定值
        //float ActualSpeed;                                 //实际值
        float err;                                                 //偏差值
        float err_last;                    //上一次偏差值
        float err_next;                    //下一次偏差值
        float Kp, Ki, Kd;                                  //设定值
        float voltage;                     //实际转换值
        float integral;                                    //积分累积
        float umax;                                                //偏差上限值
        float umin;                                                //偏差下限值
        float output;
        float increment;
        float add_max ;
        float add_min ;
        float limit_error;                // 允许误差
};
/*****************************************************************
 * 全局变量声明
 ******************************************************************/
extern struct _pid pid[2];
/*****************************************************************
 * 函数原型声明
 ******************************************************************/
float PID_Speed_Incr(struct _pid *pid, float curSpeed);
void PID_Init(struct _pid *pid);
/*****************************************************************
 * 函数说明
 ******************************************************************/
// float PID_Speed_Incr(struct _pid *pid, float ActualSpeed);
// void PID_Init(struct _pid *pid);
#ifdef __cplusplus
}
#endif

/* @} Robot_PAL */

#endif