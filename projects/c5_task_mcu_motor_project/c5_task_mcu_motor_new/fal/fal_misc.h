/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

#ifndef _FAL_MISC_H
#define _FAL_MISC_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/
#include "stdint.h"

/**
 * @ingroup Robot_FAL 功能适配层
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
typedef struct {
    float temperature_inner;
    float humidity_inner;
    float temperature_outside;
    float humidity_outside;

} ROBOT_THTB_T, *ROBOT_THTB_P;

typedef struct {
    uint16_t left_adc_val;
    float    left_distance_val;
    uint16_t right_adc_val;
    float    right_distance_val;

} ROBOT_TOUGH_EDGE_T, *ROBOT_TOUGH_EDGE_P;

typedef enum {
    SFU_RESET_UNKNOWN = 0x00U,
    SFU_RESET_WDG_RESET,
    SFU_RESET_LOW_POWER,
    SFU_RESET_HW_RESET,
    SFU_RESET_BOR_RESET,
    SFU_RESET_SW_RESET,
    SFU_RESET_OB_LOADER,

} SFU_RESET_IdTypeDef;

///< 外设通信 状态
typedef struct {
    uint16_t bit0_eeprom : 1;
    uint16_t bit1_ir : 1;
    uint16_t bit2_imu : 1;
    uint16_t bit3_gps : 1;
    uint16_t bit4_batt : 1;
    uint16_t bit5_motor_drv : 1;
    uint16_t bit6_task_secur : 1;
    uint16_t bit7 : 1;
    uint16_t bit8 : 1;
    uint16_t bit9 : 1;
    uint16_t bit10 : 1;
    uint16_t bit11 : 1;
    uint16_t bit12 : 1;
    uint16_t bit13 : 1;
    uint16_t bit14 : 1;
    uint16_t bit15 : 1;

} SELFTEST_STA_T, *SELFTEST_STA_P;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/****************************************************
功能  : 初始化FAL层
返回值: 等于0表示成功，其它值表示失败原因
 ***************************************************/
int fal_init(void);

/****************************************************
功能  ：释放FAL层资源
返回值：等于0表示成功，其它值表示失败原因
***************************************************/
int fal_deInit(void);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_FAL */

#endif
