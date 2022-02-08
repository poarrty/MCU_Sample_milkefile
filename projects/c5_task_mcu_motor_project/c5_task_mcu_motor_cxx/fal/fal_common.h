/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_common.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: common called
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __FAL_COMMON_H
#define __FAL_COMMON_H

#include <board.h>

#include "button.h"
#include "clean_ctrl.h"
#include "clean_def.h"
#include "detection.h"
#include "fal_letter_shell.h"
#include "fal_log.h"
#include "fal_misc.h"
#include "fal.h"
#include "fal_version.h"

//#define USED_BUTTON

/* clean_ctrl && detection */
// #define USED_CELAN_DEVICE
// #define USED_CLEAN_DETECTION
#define USED_SIDE_BRUSH_MOTOR_CTRL
#define USED_SIDE_BRUSH_MOTOR_DETECTION
#define SIDE_BRUSH_MOTOR_LETF_TIMX_IC
#define USED_PUSH_ROD_MOTOR_1_CTRL
#define USED_PUSH_ROD_MOTOR_1_DETECTION
//#define USED_PUSH_ROD_MOTOR1_DETECTION_ADC
#define USED_PUSH_ROD_MOTOR_2_CTRL
#define USED_PUSH_ROD_MOTOR2_DETECTION
//#define USED_PUSH_ROD_MOTOR2_DETECTION_ADC
#define USED_WATER_DIST_PUMP_CTRL
#define USED_WATER_DIST_PUMP_DETECTION
#define USED_WATER_VALVE_CLEAN_CTRL
#define USED_WATER_VALVE_CLEAN_DETECTION
//#define USED_SUNCTION_MOTOR_CTRL
//#define USED_SUNCTION_MOTOR_DETECTION
#define USED_ROLL_BRUSH_MOTOR_CTRL
#define USED_ROLL_BRUSH_MOTOR_DETECTION
/* end */

//#define USED_ROLL_BRUSH_MOTOR_CTRL_CAN
//#define USED_SUNCTION_MOTOR_CTRL_CAN

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char          CHAR;

typedef uint16_t USHORT;
typedef int16_t  SHORT;

typedef uint32_t ULONG;
typedef int32_t  LONG;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int      utils_truncate_number_uint(uint32_t *number, uint32_t min, uint32_t max);
void     utils_step_towards_uint(uint32_t *value, uint32_t goal, uint32_t step);
int      utils_truncate_number_int(int *number, int min, int max);
void     utils_step_towards_int(int *value, int goal, int step);
int      utils_truncate_number_float(float *number, float min, float max);
void     utils_step_towards_float(float *value, float goal, float step);
uint16_t average_get(uint16_t *in_data, uint8_t len, uint8_t del_len);
void     bubble_sort(uint16_t *data, uint8_t n);
#endif
