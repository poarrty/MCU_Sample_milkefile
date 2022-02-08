/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_pwm.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: pwm operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_PWM_H
#define __BSP_PWM_H
#include <board.h>

#define SIDE_BRUSH_MOTOR_SPPED (240)
// old:500 new:208.33f
#define SIDE_BRUSH_MOTOR_CYCLE_NS   ((int32_t)(SIDE_BRUSH_MOTOR_SPPED * 500.0f))  // 208.33f
#define SIDE_BRUSH_MOTOR_DIV        (2.4)
#define WATER_DISTRIBUTION_CYCLE_NS SIDE_BRUSH_MOTOR_CYCLE_NS
#define ROLL_BRUSH_MOTOR_CYCLE_NS   SIDE_BRUSH_MOTOR_CYCLE_NS

#define MAX_PERIOD 65535
#define MIN_PERIOD 3
#define MIN_PULSE  2

enum { ID_WATER_DISTRIBUTION_PWM = 0, ID_SIDE_BRUSH_MOTOR_LFTE_PWM, ID_SIDE_BRUSH_MOTOR_RIGHT_PWM, ID_ROLL_BRUSH_MOTOR_PWM };

int pwm_write_period(uint8_t pwm_obj_id, uint32_t period);
int pwm_write_pulse(uint8_t pwm_obj_id, uint32_t pulse);
int pwm_read_pulse(uint8_t pwm_obj_id, uint32_t *pulse);
int pwm_enable(uint8_t pwm_obj_id);
int pwm_disable(uint8_t pwm_obj_id);

#endif
