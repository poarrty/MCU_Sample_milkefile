/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_pwm.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: pwm operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_pwm.h"
#define LOG_TAG "bsp_pwm"
#include "elog.h"
#define USED_BSP_PWM
#ifdef USED_BSP_PWM

extern TIM_HandleTypeDef htim8;
#define handle                         &htim8
#define WATER_DISTRIBUTION_CHANNEL     TIM_CHANNEL_1
#define SIDE_BRUSH_MOTOR_LFTE_CHANNEL  TIM_CHANNEL_2
#define SIDE_BRUSH_MOTOR_RIGHT_CHANNEL TIM_CHANNEL_3
#define ROLL_BRUSH_MOTOR_CHANNEL       TIM_CHANNEL_2

#define pwm_obj_index(id, htim, channel, complementary)     \
    do {                                                    \
        if (id == ID_WATER_DISTRIBUTION_PWM) {              \
            htim          = handle;                         \
            channel       = WATER_DISTRIBUTION_CHANNEL;     \
            complementary = FALSE;                          \
        } else if (id == ID_SIDE_BRUSH_MOTOR_LFTE_PWM) {    \
            htim          = handle;                         \
            channel       = SIDE_BRUSH_MOTOR_LFTE_CHANNEL;  \
            complementary = FALSE;                          \
        } else if (id == ID_SIDE_BRUSH_MOTOR_RIGHT_PWM) {   \
            htim          = handle;                         \
            channel       = SIDE_BRUSH_MOTOR_RIGHT_CHANNEL; \
            complementary = FALSE;                          \
        } else if (id == ID_ROLL_BRUSH_MOTOR_PWM) {         \
            htim          = handle;                         \
            channel       = ROLL_BRUSH_MOTOR_CHANNEL;       \
            complementary = FALSE;                          \
        }                                                   \
    } while (0)

int pwm_get_tim_clock(TIM_HandleTypeDef *htim, uint32_t *value);

int pwm_get_tim_clock(TIM_HandleTypeDef *htim, uint32_t *value) {
    uint32_t APB2CLKDivider;
    uint32_t pclk2_doubler;

    APB2CLKDivider = (uint32_t)((RCC->CFGR & RCC_CFGR_PPRE2) >> 3);

    if (APB2CLKDivider == RCC_HCLK_DIV1) {
        pclk2_doubler = 1;
    } else {
        pclk2_doubler = 2;
    }

    *value = (uint32_t)((HAL_RCC_GetPCLK2Freq() / 1000000UL) * pclk2_doubler);

    return HAL_OK;
}

// period,unit:ns
int pwm_write_period(uint8_t pwm_obj_id, uint32_t period) {
    TIM_HandleTypeDef *htim = 0;

    uint32_t channel; /* 0-n */
    int      complementary = FALSE;
    (void) complementary;
    uint32_t T;     /* period unit:ns 1ns~4.29s:1Ghz~0.23hz */
    uint32_t pulse; /* unit:ns (pulse<=period) */
    uint32_t tim_clock, psc;

    pwm_obj_index(pwm_obj_id, htim, channel, complementary);

    psc   = htim->Instance->PSC + 1;
    pulse = __HAL_TIM_GET_COMPARE(htim, channel) + 1;

    pwm_get_tim_clock(htim, &tim_clock);

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    T = (uint32_t)(period * tim_clock / psc / 1000UL);

    if (T > MAX_PERIOD) {
        T   = (uint32_t)(period * tim_clock / 1000UL);
        psc = T / MAX_PERIOD + 1;
        T   = T / psc;
        __HAL_TIM_SET_PRESCALER(htim, psc - 1);
    }

    if (T < MIN_PERIOD) {
        T = MIN_PERIOD;
    }

    __HAL_TIM_SET_AUTORELOAD(htim, T - 1);

    pulse = (uint32_t)(pulse * tim_clock / psc / 1000UL);

    if (pulse < MIN_PULSE) {
        pulse = MIN_PULSE;
    } else if (pulse > period) {
        pulse = period;
    }

    __HAL_TIM_SET_COMPARE(htim, channel, pulse - 1);
    __HAL_TIM_SET_COUNTER(htim, 0);

    /* Update frequency value */
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);

    HAL_TIM_PWM_Start(htim, channel);

    return HAL_OK;
}

// period,unit:ns
int pwm_write_pulse(uint8_t pwm_obj_id, uint32_t pulse) {
    TIM_HandleTypeDef *htim        = 0;
    uint32_t           compare_val = pulse;
    uint32_t           period;
    uint32_t           channel       = 0; /* 0-n */
    int                complementary = FALSE;
    (void) complementary;
    uint32_t tim_clock, psc;

    pwm_obj_index(pwm_obj_id, htim, channel, complementary);

    psc = htim->Instance->PSC + 1;
    pwm_get_tim_clock(htim, &tim_clock);

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    // log_d("pulse:%d", pulse);
    compare_val = (uint32_t)(pulse * tim_clock / psc / 1000UL);
    // log_d("compare_val:%d", compare_val);
    period = __HAL_TIM_GET_AUTORELOAD(htim);
    // log_d("period:%d", period);

    if (compare_val < MIN_PULSE) {
        compare_val = MIN_PULSE;
    }

    if (compare_val > period) {
        compare_val = period;
    }

    __HAL_TIM_SET_COMPARE(htim, channel, compare_val - 1);
    __HAL_TIM_SET_COUNTER(htim, 0);

    /* Update frequency value */
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);

    HAL_TIM_PWM_Start(htim, channel);

    return HAL_OK;
}

int pwm_read_pulse(uint8_t pwm_obj_id, uint32_t *pulse) {
    uint32_t           value;
    TIM_HandleTypeDef *htim          = NULL;
    uint32_t           channel       = 0; /* 0-n */
    int                complementary = FALSE;
    (void) complementary;
    uint32_t tim_clock;

    pwm_obj_index(pwm_obj_id, htim, channel, complementary);

    pwm_get_tim_clock(htim, &tim_clock);

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    // period = (__HAL_TIM_GET_AUTORELOAD(htim) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;
    value = (__HAL_TIM_GET_COMPARE(htim, channel) + 1) * (htim->Instance->PSC + 1) * 1000UL / tim_clock;

    *pulse = value;
    return value;
}

int pwm_enable(uint8_t pwm_obj_id) {
    TIM_HandleTypeDef *htim          = NULL;
    uint32_t           channel       = 0; /* 0-n */
    int                complementary = FALSE;
    (void) complementary;

    pwm_obj_index(pwm_obj_id, htim, channel, complementary);

    if (!complementary) {
        HAL_TIM_PWM_Start(htim, channel);
    } else {
        HAL_TIMEx_PWMN_Start(htim, channel);
    }

    return HAL_OK;
}

int pwm_disable(uint8_t pwm_obj_id) {
    TIM_HandleTypeDef *htim          = NULL;
    uint32_t           channel       = 0; /* 0-n */
    int                complementary = FALSE;
    (void) complementary;

    pwm_obj_index(pwm_obj_id, htim, channel, complementary);

    if (!complementary) {
        HAL_TIM_PWM_Stop(htim, channel);
    } else {
        HAL_TIMEx_PWMN_Stop(htim, channel);
    }

    return HAL_OK;
}

static void pwm_test(int argc, char **argv) {
    if (argc < 3) {
        log_w("%s input parm error\r\n", __FUNCTION__);
        return;
    }

    pwm_write_pulse(atoi(argv[1]), atoi(argv[2]));
    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), pwm_test, pwm_test, pwm_test 0 300);

#endif
