#include "bsp_pwm.h"
#include "tim.h"
#include "shell.h"
#include "log.h"
#include "stdlib.h"

int pwm_write_pulse(uint8_t pwm_obj_id, uint32_t pulse) {
    switch (pwm_obj_id) {
        case ID_LED_PWM:
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, pulse);
            __HAL_TIM_SET_COUNTER(&htim8, 0);
            break;

        case ID_FILTER_PUMP_PWM:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pulse);
            __HAL_TIM_SET_COUNTER(&htim3, 0);
            break;

        default:
            break;
    }

    return 0;
}

int pwm_enable(uint8_t pwm_obj_id) {
    switch (pwm_obj_id) {
        case ID_LED_PWM:
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
            break;

        case ID_FILTER_PUMP_PWM:
            HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
            break;

        default:
            break;
    }

    return 0;
}

int pwm_disable(uint8_t pwm_obj_id) {
    switch (pwm_obj_id) {
        case ID_LED_PWM:
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
            break;

        case ID_FILTER_PUMP_PWM:
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
            break;

        default:
            break;
    }

    return 0;
}

// static void pwm_test(int argc, char **argv)
// {
// 	if (argc < 3)
// 	{
// 		LOG_W("%s input parm error\r\n", __FUNCTION__);
// 		return;
// 	}

// 	pwm_write_pulse(atoi(argv[1]), atoi(argv[2]));
// 	return;
// }
