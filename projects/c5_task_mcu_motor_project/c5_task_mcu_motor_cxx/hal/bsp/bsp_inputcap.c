/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_inputcap.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: input capture
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_inputcap.h"
#define LOG_TAG "bsp_inputcap"
#define USED_BSP_INPUTCAP
#ifdef USED_BSP_INPUTCAP

#define us2freq(us)            (1000000 / us)
#define interval_max_us        1000000
#define interval_is_right(val) (val && val < interval_max_us)
#define cal_21edge_freq        (in_freq->state_switch)
#define curr_status            in_freq->state

enum { INPUT_CAP_FIRST_RISING = 0, INPUT_CAP_SECOND_RISING, INPUT_CAP_DEFAULT_MAX };

void input_cap_freq_handler(void *param) {
    volatile struct input_cap_freq *in_freq      = (struct input_cap_freq *) param;
    uint32_t                        current_time = 0;

    current_time = clock_cpu_gettime();

    if (current_time == 0) {
        current_time = 1;
    }

    in_freq->curr_edge_trigger_time = current_time;

    switch (curr_status) {
        case INPUT_CAP_FIRST_RISING:
            in_freq->first_edge_trigger_time = current_time;

            if (cal_21edge_freq == RT_TRUE) {
                // time length between two rising edge
                in_freq->time_interval = clock_cpu_microsecond_diff(in_freq->second_edge_trigger_time, current_time);

                // frequence
                if (interval_is_right(in_freq->time_interval)) {
                    in_freq->input_freq = us2freq(in_freq->time_interval);
                }

                cal_21edge_freq = RT_FALSE;
            }

            curr_status = INPUT_CAP_SECOND_RISING;
            break;

        case INPUT_CAP_SECOND_RISING:
            in_freq->second_edge_trigger_time = current_time;

            // time length between two rising edge
            in_freq->time_interval = clock_cpu_microsecond_diff(in_freq->first_edge_trigger_time, current_time);

            // frequence
            if (interval_is_right(in_freq->time_interval)) {
                in_freq->input_freq = us2freq(in_freq->time_interval);
            }

            curr_status     = INPUT_CAP_FIRST_RISING;
            cal_21edge_freq = RT_TRUE;
            break;

        default:
            curr_status     = INPUT_CAP_FIRST_RISING;
            cal_21edge_freq = RT_FALSE;
            break;
    }
}

#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION
#ifdef SIDE_BRUSH_MOTOR_LETF_TIMX_IC
extern struct input_cap_freq sbm_left_freq_info;
void                         HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == IC_TIMER) {
        input_cap_freq_handler((void *) &sbm_left_freq_info);
    }
}
#endif
#endif

#endif