#ifndef BSP_PWM_H
#define BSP_PWM_H

#include "stdint.h"

#define PWM_CHANNEL1 1
#define PWM_CHANNEL2 2
#define PWM_CHANNEL3 3
#define PWM_CHANNEL4 4

#define PWM_OBJ_FLAG_USED    (1ul << 1)
#define PWM_OBJ_FLAG_NO_USED (1ul << 0)
#define LED_PWM_CYCLE_NS     10000
#define FILTER_PUMP_CYCLE_NS 100
#define LED_PWM_MAX_LEVLE    100

enum { ID_LED_PWM = 0, ID_FILTER_PUMP_PWM };

int pwm_write_pulse(uint8_t pwm_obj_id, uint32_t pulse);
int pwm_enable(uint8_t pwm_obj_id);
int pwm_disable(uint8_t pwm_obj_id);

#endif