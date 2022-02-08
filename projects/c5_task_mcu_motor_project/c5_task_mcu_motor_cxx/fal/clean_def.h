/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: clean_def.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: clean define
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef _CLEAN_DEF_H_
#define _CLEAN_DEF_H_

#include "stdint.h"

#define PUSH_ROD_MOTOR_1_NUM     0
#define PUSH_ROD_MOTOR_2_NUM     1
#define WATER_DISTRIBUTION_NUM   0
#define SIDE_BRUSH_MOTOR_NUM     0
#define ROLL_MOTOR_NUM           0
#define SUNCTION_MOTOR_NUM       0
#define FILTER_PUMP_NUM          0
#define SEWAGE_WATER_VALVE_NUM   0
#define CLEAN_WATER_VALVE_NUM    1
#define WASTE_WATER_VALVE_NUM    2
#define SOFT_START_BUTTON_NUM    0
#define START_SUSPEND_BUTTON_NUM 1
#define OPS_MODE_BUTTON_NUM      2
#define WORK_MODE_BUTTON_NUM     3
#define LED_BRIGHTNESS_NUM       0
#define LED_BLINK_NUM            0
#define WASTE_WATER_NUM          0
#define CLEAN_WATER_NUM          1

typedef struct power_start {
    uint8_t start_flag;
} power_start_st;

typedef struct clean_module_ctrl {
    uint8_t  device_num;
    uint16_t error_flag;
    int      set_value;
    int      cur_value;
    float    current;
    float    duty_cycle;
} clean_module_ctrl_st;

#endif
