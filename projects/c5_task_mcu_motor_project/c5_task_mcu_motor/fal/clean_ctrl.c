/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: clean_ctrl.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: clean control operation(IO or PWM)
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "clean_ctrl.h"

#define LOG_TAG "clean_ctrl"
#include "elog.h"
#define SIDE_BRUSH_MOTOR_STEP       (10 * SIDE_BRUSH_MOTOR_CYCLE_NS / SIDE_BRUSH_MOTOR_SPPED)
#define SIDE_BRUSH_MOTOR_STEP_DELAY 10
#define WATER_DIST_PUMP_STEP        WATER_DISTRIBUTION_CYCLE_NS / 50
#define WATER_DIST_PUMP_STEP_DELAY  10
#ifdef USED_SUNCTION_MOTOR_CTRL
#define SUNCTION_MOTOR_PWM_CYCLE 100
#define SUNCTION_MOTOR_DIV       1
#endif
#ifdef USED_ROLL_BRUSH_MOTOR_CTRL
#define ROLL_BRUSH_MOTOR_PWM_CYCLE  10
#define ROLL_BRUSH_MOTOR_DIV        10
#define ROLL_BRUSH_MOTOR_STEP       (ROLL_BRUSH_MOTOR_CYCLE_NS / 5)
#define ROLL_BRUSH_MOTOR_STEP_DELAY 10
#endif
#define HWTIMER_DEV_NAME              "timer3"
#define THREAD_YIELD_TIME             2
#define PUSH_ROD_MOTOR1_DELAY_TIME    12500
#define PUSH_ROD_MOTOR2_DELAY_TIME    12500
#define PUSH_ROD_MOTOR1_ST_DELAY_TIME 500
#define PUSH_ROD_MOTOR2_ST_DELAY_TIME 500

#define WATER_VALVE_CLEAN_DELAY_TIME 10000

/* motor logic */
#define delay_time(time, delayTime)          (++time % delayTime == 0)
#define math_climb_curve_k(Delta_x, y, goal) utils_step_towards_uint(&y, goal, Delta_x);
#define is_climb_done(x, goal)               (x == goal)
#define limit_val(val, max)                  (val > max ? val = max : val)
#define keep_val_range(min, max, val)        (val > max ? val = max : val < min ? val = min : val)
/* side motor logic */
#define side_motor_val2speed(val)                 (val *= SIDE_BRUSH_MOTOR_DIV)
#define side_motor_speed_ValRight(speed_val, max) limit_val(speed_val, max)

#define side_motor_speed_ratio(speed) (((float) speed) / SIDE_BRUSH_MOTOR_SPPED)
#define side_motor_pwnVal(speed)      ((uint32_t)(int32_t)(side_motor_speed_ratio(speed) * SIDE_BRUSH_MOTOR_CYCLE_NS))
#define side_motor_opsDelay(time)     delay_time(time, SIDE_BRUSH_MOTOR_STEP_DELAY)

#define side_motor_power(sta)              clean_io_ctrl(ID_SIDE_BRUSH_POWER, sta)
#define side_motor_speed_climb_up(y, goal) math_climb_curve_k(SIDE_BRUSH_MOTOR_STEP, y, goal)

#define side_motor_dir_ops                                    \
    do {                                                      \
        clean_io_ctrl(ID_SIDE_BRUSH_MOTOR_LEFT_CW, ENABLE);   \
        clean_io_ctrl(ID_SIDE_BRUSH_MOTOR_RIGHT_CW, DISABLE); \
    } while (0)

#define side_motor_spd_ops(val)                              \
    do {                                                     \
        pwm_write_pulse(ID_SIDE_BRUSH_MOTOR_RIGHT_PWM, val); \
    } while (0)

#define side_motor_ctrl(spd_val)     \
    do {                             \
        side_motor_dir_ops;          \
        side_motor_spd_ops(spd_val); \
    } while (0)
#define side_motor_pwm_on                          \
    do {                                           \
        pwm_enable(ID_SIDE_BRUSH_MOTOR_LFTE_PWM);  \
        pwm_enable(ID_SIDE_BRUSH_MOTOR_RIGHT_PWM); \
    } while (0)
#define side_motor_pwm_off                          \
    do {                                            \
        pwm_disable(ID_SIDE_BRUSH_MOTOR_LFTE_PWM);  \
        pwm_disable(ID_SIDE_BRUSH_MOTOR_RIGHT_PWM); \
    } while (0)
#define side_motor_on             \
    do {                          \
        side_motor_power(ENABLE); \
        side_motor_pwm_on;        \
    } while (0)
#define side_motor_off             \
    do {                           \
        side_motor_power(DISABLE); \
        side_motor_pwm_off;        \
    } while (0)

#define is_side_motor_speed_climb_done(speed, goal) is_climb_done(speed, goal)
/* push rod motor logic  */
#define rod_motor_delay_time(length, step) (length / step)
#define rod_motor1_ops_delay(time)         (time++ >= rod_motor_delay_time(PUSH_ROD_MOTOR1_DELAY_TIME, THREAD_YIELD_TIME))
#define rod_motor2_ops_delay(time)         (time++ >= rod_motor_delay_time(PUSH_ROD_MOTOR2_DELAY_TIME, THREAD_YIELD_TIME))
#define rod_motor1_st_delay(time)          (time++ >= rod_motor_delay_time(PUSH_ROD_MOTOR1_ST_DELAY_TIME, THREAD_YIELD_TIME))
#define rod_motor2_st_delay(time)          (time++ >= rod_motor_delay_time(PUSH_ROD_MOTOR2_ST_DELAY_TIME, THREAD_YIELD_TIME))
#define rod_motor_stop(id)                             \
    do {                                               \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TH, 0); \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TL, 0); \
    } while (0)
#define rod_motor1_stop rod_motor_stop(1)
#define rod_motor2_stop rod_motor_stop(2)
#define rod_motor_run(id, status)                            \
    do {                                                     \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TH, !status); \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TL, status);  \
    } while (0)
#define rod_motor1_run(val) rod_motor_run(1, val)
#define rod_motor2_run(val) rod_motor_run(2, val)
/* water dist pump logic */
#define keep_water_dist_pump_ValRight(val) keep_val_range(20, 100, val)
#define get_water_dist_pump_pwnVal(val)    ((100 - val) * WATER_DISTRIBUTION_CYCLE_NS / 100)

#define water_dist_pump_opsDelay(time) delay_time(time, WATER_DIST_PUMP_STEP_DELAY)

#define water_dist_pump_speed_climb_up(y, max) math_climb_curve_k(WATER_DIST_PUMP_STEP, y, max)

#define is_water_dist_pump_speed_climb_done(speed, goal) is_climb_done(speed, goal)

#define water_dist_pump_power(sta)        clean_io_ctrl(ID_WATER_DISTRIBUTION_POWER, sta);
#define water_dist_pump_adjust_speed(val) pwm_write_pulse(ID_WATER_DISTRIBUTION_PWM, val)
#define water_dist_pump_ctrl(pwr, speed_val)     \
    do {                                         \
        water_dist_pump_power(pwr);              \
        water_dist_pump_adjust_speed(speed_val); \
    } while (0)
/* water valve clean logic */
#define water_valve_clean_delay_time(length, step) (length / step)
#define water_valve_clean_delay(time)              (time++ >= water_valve_clean_delay_time(WATER_VALVE_CLEAN_DELAY_TIME, THREAD_YIELD_TIME))
#define water_valve_clean_power(sta)               clean_io_ctrl(ID_WATER_VALVE_CLEAN_POWER, sta);
#define water_valve_clean_ctrl(on)   \
    do {                             \
        water_valve_clean_power(on); \
    } while (0)
/* roll brush motor logic */
#define roll_brush_motor_dir_ops(dir)                                                              \
    do {                                                                                           \
        dir < 0 ? clean_io_ctrl(ID_SUNCTION_MOTOR_CW, 1) : clean_io_ctrl(ID_SUNCTION_MOTOR_CW, 0); \
    } while (0)
#define roll_brush_motor_spd_ops(val)                       \
    do {                                                    \
        pwm_write_pulse(ID_SIDE_BRUSH_MOTOR_LFTE_PWM, val); \
    } while (0)
#define roll_brush_motor_spd_io_ops(status)           \
    do {                                              \
        clean_io_ctrl(ID_SUNCTION_MOTOR_CTL, status); \
    } while (0)
#define roll_brush_motor_ctrl(val)     \
    do {                               \
        roll_brush_motor_spd_ops(val); \
    } while (0)  // roll_brush_motor_spd_io_ops(!val);

#define roll_brush_motor_ValRight(val, min, max) \
    do {                                         \
        val = abs(val);                          \
        keep_val_range(min, max, val);           \
    } while (0)
#define roll_brush_motor_pwnVal(percentVal)         ((uint32_t)(int32_t)((percentVal / 100.0) * ROLL_BRUSH_MOTOR_CYCLE_NS))
#define roll_brush_motor_opsDelay(time)             delay_time(time, ROLL_BRUSH_MOTOR_STEP_DELAY)
#define roll_brush_motor_speed_climb_up(y, goal)    math_climb_curve_k(ROLL_BRUSH_MOTOR_STEP, y, goal)
#define is_roll_motor_speed_climb_done(speed, goal) is_climb_done(speed, goal)

#ifdef USED_SUNCTION_MOTOR_CTRL
static uint8_t sunction_motor_timer_count   = 0;
static uint8_t sunction_motor_pwm_value     = 0;
static uint8_t sunction_motor_output_enable = 0;

void sunction_motor_timeout_handle(void *param) {
    uint8_t        pwm_cycle      = 0;
    static uint8_t last_pwm_cycle = 0;

    if (param == NULL) {
        return;
    }

    if (sunction_motor_output_enable == 0) {
        clean_io_ctrl(ID_SUNCTION_MOTOR_CTL, 0);
        return;
    }

    pwm_cycle = *(uint8_t *) param;
    pwm_cycle = pwm_cycle / SUNCTION_MOTOR_DIV;

    if (last_pwm_cycle != pwm_cycle) {
        sunction_motor_timer_count = 0;
    }

    if (pwm_cycle > SUNCTION_MOTOR_PWM_CYCLE) {
        pwm_cycle = SUNCTION_MOTOR_PWM_CYCLE;
    }

    if (++sunction_motor_timer_count <= pwm_cycle) {
        clean_io_ctrl(ID_SUNCTION_MOTOR_CTL, 1);
    } else {
        clean_io_ctrl(ID_SUNCTION_MOTOR_CTL, 0);
    }

    if (sunction_motor_timer_count >= SUNCTION_MOTOR_PWM_CYCLE) {
        sunction_motor_timer_count = 0;
    }

    last_pwm_cycle = pwm_cycle;
}

rt_err_t sunction_motor_soft_pwm(rt_device_t dev, rt_size_t size) {
    sunction_motor_timeout_handle((void *) &sunction_motor_pwm_value);
    return RT_EOK;
}
#endif

#ifdef USED_ROLL_BRUSH_MOTOR_CTRL
static uint8_t roll_brush_motor_count         = 0;
static uint8_t roll_brush_motor_pwm_value     = 0;
static uint8_t roll_brush_motor_output_enable = 0;

void roll_brush_motor_pwm_handle(void *param) {
    uint8_t        pwm_cycle      = 0;
    static uint8_t last_pwm_cycle = 0;

    if (param == NULL) {
        return;
    }

    if (roll_brush_motor_output_enable == 0) {
        roll_brush_motor_ctrl(0);
        return;
    }

    pwm_cycle = *(uint8_t *) param;
    pwm_cycle = pwm_cycle / ROLL_BRUSH_MOTOR_DIV;

    if (last_pwm_cycle != pwm_cycle) {
        roll_brush_motor_count = 0;
    }

    if (pwm_cycle > ROLL_BRUSH_MOTOR_PWM_CYCLE) {
        pwm_cycle = ROLL_BRUSH_MOTOR_PWM_CYCLE;
    }

    /* pwm1 */
    if (++roll_brush_motor_count <= pwm_cycle) {
        roll_brush_motor_ctrl(1);
    } else {
        roll_brush_motor_ctrl(0);
    }

    if (roll_brush_motor_count >= ROLL_BRUSH_MOTOR_PWM_CYCLE) {
        roll_brush_motor_count = 0;
    }

    last_pwm_cycle = pwm_cycle;
}

rt_err_t roll_brush_motor_soft_pwm(rt_device_t dev, rt_size_t size) {
    roll_brush_motor_pwm_handle((void *) &roll_brush_motor_pwm_value);
    // log_d("TIM6 UPDATE");
    return RT_EOK;
}

#endif

static bool dev_thread_utime;

void device_run_thread_entry(void *param) {
    uint32_t side_brush_motor_div = 0;
#ifdef USED_SIDE_BRUSH_MOTOR_CTRL
    // uint32_t side_brush_motor_sub = 0;
    uint32_t             side_brush_motor_enable         = FALSE;
    uint32_t             side_brush_motor_current_value  = 0;
    uint32_t             side_brush_motor_set_value      = 0;
    uint32_t             last_side_brush_motor_set_value = 0;
    clean_module_ctrl_st side_brush_motor_data;
#endif

#ifdef USED_PUSH_ROD_MOTOR_1_CTRL
    // uint32_t push_rod_motor1_sub = 0;
    uint32_t             push_rod_motor1_count  = 0;
    uint32_t             push_rod_motor1_enable = RT_FALSE;
    clean_module_ctrl_st push_rod_motor1_data;
#endif

#ifdef USED_PUSH_ROD_MOTOR_2_CTRL
    // uint32_t push_rod_motor2_sub = 0;
    uint32_t             push_rod_motor2_count  = 0;
    uint32_t             push_rod_motor2_enable = RT_FALSE;
    clean_module_ctrl_st push_rod_motor2_data;
#endif

#ifdef USED_WATER_DIST_PUMP_CTRL
    // uint32_t water_distribution_sub = 0;
    uint32_t             water_distribution_enable        = RT_FALSE;
    uint32_t             water_distribution_current_value = 0;
    uint32_t             water_distribution_set_value     = 0;
    uint32_t             water_distribution_div           = 0;
    clean_module_ctrl_st water_distribution_data;
#endif

#ifdef USED_WATER_VALVE_CLEAN_CTRL
    // uint32_t water_valve_clean_sub = 0;
    uint32_t             water_valve_clean_count  = 0;
    uint32_t             water_valve_clean_enable = RT_FALSE;
    clean_module_ctrl_st water_valve_clean_data;
#endif

#ifdef USED_SUNCTION_MOTOR_CTRL
    uint32_t             sunction_motor_sub           = 0;
    uint32_t             sunction_motor_enable        = RT_FALSE;
    uint32_t             sunction_motor_current_value = 0;
    uint32_t             sunction_motor_set_value     = 0;
    uint32_t             sunction_motor_div           = 0;
    clean_module_ctrl_st sunction_motor_data;

    /* need: soft pwm called by use timer ISR */

#endif

#ifdef USED_ROLL_BRUSH_MOTOR_CTRL
    // uint32_t roll_brush_motor_sub = 0;
    uint32_t             roll_brush_motor_enable        = RT_FALSE;
    uint32_t             roll_brush_motor_current_value = 0;
    uint32_t             roll_brush_motor_set_value     = 0;
    uint32_t             roll_brush_motor_div           = 0;
    clean_module_ctrl_st roll_brush_motor_data;

    /* need: soft pwm called by use timer ISR */

#endif

    uint32_t thread_start_time = 0;
    uint32_t thread_utime      = 0;
    uint32_t running_time_div  = 0;

    uint32_t sub_evt = osFlagsError;

    roll_brush_motor_ctrl(0);
#if 0
    rod_motor1_stop;
    rod_motor2_stop;
#else
    rod_motor1_run(0);
    rod_motor2_run(0);
#endif

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_clean_ctrlHandle,
                                   SYS_EVT_SIDE_BRUSH_MOTOR | SYS_EVT_PUSH_ROD_MOTOR_1 | SYS_EVT_PUSH_ROD_MOTOR_2 | SYS_EVT_WATER_DIST_PUMP |
                                       SYS_EVT_WATER_VALVE_CLEAN | SYS_EVT_ROLL_BRUSH_MOTOR | SYS_EVT_SUNCTION_MOTOR,
                                   osFlagsWaitAny, 0);

        if (sub_evt > osFlagsError) {
            sub_evt = 0;
        }

        thread_start_time = clock_cpu_gettime();

#ifdef USED_SIDE_BRUSH_MOTOR_CTRL

        if (sub_evt & SYS_EVT_SIDE_BRUSH_MOTOR) {
            if (xQueuePeek(topic_side_brush_motorHandle, &side_brush_motor_data, 0) != errQUEUE_EMPTY) {
                side_motor_val2speed(side_brush_motor_data.set_value);

                side_motor_speed_ValRight(side_brush_motor_data.set_value, SIDE_BRUSH_MOTOR_SPPED);

                side_brush_motor_set_value = side_motor_pwnVal(side_brush_motor_data.set_value);

                /* Start at high speed and then slowly reduce to the set speed */
                if (side_brush_motor_data.set_value && (last_side_brush_motor_set_value == 0)) {
                    side_brush_motor_current_value = side_motor_pwnVal(100);  // ratio:100/240
                }

                side_brush_motor_enable         = TRUE;
                last_side_brush_motor_set_value = side_brush_motor_data.set_value;
                log_d("sbm :%d", side_brush_motor_data.set_value);
            }
        }

        /* Acceleration and deceleration start and stop of side brush */
        if ((side_brush_motor_enable == TRUE) && side_motor_opsDelay(side_brush_motor_div)) {
            side_brush_motor_div = 0;

            if (side_brush_motor_data.set_value > 0) {
                side_motor_power(side_brush_motor_data.set_value);  // turn on
                // side_motor_pwm_on;
            }

            side_motor_speed_climb_up(side_brush_motor_current_value, side_brush_motor_set_value);

            side_motor_ctrl(side_brush_motor_current_value);

            // log_d("sbm TIME8:%x", (u32)TIM8->CR1);
            // log_d("sbm TIME8 CHANNEL:%x", (u32)TIM8->CCER);
            // log_d("sbm TIME8 ARR:%x", (u32)TIM8->ARR);
            // log_d("sbm TIME8 pulse value:%x", (u32)TIM8->CCR2);

            if (is_side_motor_speed_climb_done(side_brush_motor_current_value, side_brush_motor_set_value)) {
                side_brush_motor_enable = FALSE;

                if (side_brush_motor_data.set_value == 0) {
                    side_motor_power(side_brush_motor_data.set_value);  // cut off
                    // side_motor_pwm_off;
                }
            }
        }

#endif

#ifdef USED_PUSH_ROD_MOTOR_1_CTRL

        if (sub_evt & SYS_EVT_PUSH_ROD_MOTOR_1) {
            if (xQueuePeek(topic_push_rod_motor_1Handle, &push_rod_motor1_data, 0) != errQUEUE_EMPTY) {
                push_rod_motor1_enable = RT_TRUE;
                push_rod_motor1_count  = 0;
                /* off push rod motor1 output */
                rod_motor1_stop;
                // log_d("T1L:%x\nT1H:%x", (GPIOC->IDR & GPIO_PIN_10), (GPIOC->IDR & GPIO_PIN_11));
                // log_d("prm_1 :%d", push_rod_motor1_data.set_value);
            }
        }

        if ((push_rod_motor1_enable == RT_TRUE) && rod_motor1_st_delay(push_rod_motor1_count)) {
            rod_motor1_run(push_rod_motor1_data.set_value);

            if (rod_motor1_ops_delay(push_rod_motor1_count)) {
                /* off push rod motor1 output */
                // log_d("T1L:%x\nT1H:%x", (GPIOC->IDR & GPIO_PIN_10), (GPIOC->IDR & GPIO_PIN_11));
                rod_motor1_stop;
                push_rod_motor1_count  = 0;
                push_rod_motor1_enable = RT_FALSE;
            }
        }

#endif

#ifdef USED_PUSH_ROD_MOTOR_2_CTRL

        if (sub_evt & SYS_EVT_PUSH_ROD_MOTOR_2) {
            if (xQueuePeek(topic_push_rod_motor_2Handle, &push_rod_motor2_data, 0) != errQUEUE_EMPTY) {
                push_rod_motor2_enable = RT_TRUE;
                push_rod_motor2_count  = 0;
                /* cut push rod motor1 output */
                rod_motor2_stop;
                // log_d("T2L:%x\nT2H:%x", (GPIOC->IDR & GPIO_PIN_0), (GPIOC->IDR & GPIO_PIN_1));
                // log_d("prm_2 :%d", push_rod_motor2_data.set_value);
            }
        }

        if ((push_rod_motor2_enable == RT_TRUE) && rod_motor2_st_delay(push_rod_motor2_count)) {
            rod_motor2_run(push_rod_motor2_data.set_value);

            if (rod_motor2_ops_delay(push_rod_motor2_count)) {
                /* cut push rod motor1 output */
                // log_d("T2L:%x\nT2H:%x", (GPIOC->IDR & GPIO_PIN_0), (GPIOC->IDR & GPIO_PIN_1));
                rod_motor2_stop;
                push_rod_motor2_count  = 0;
                push_rod_motor2_enable = RT_FALSE;
            }
        }

#endif

#ifdef USED_WATER_DIST_PUMP_CTRL

        if (sub_evt & SYS_EVT_WATER_DIST_PUMP) {
            if (xQueuePeek(topic_water_dist_pumpHandle, &water_distribution_data, 0) != errQUEUE_EMPTY) {
                if (water_distribution_data.set_value)
                    keep_water_dist_pump_ValRight(water_distribution_data.set_value);
                water_distribution_set_value = get_water_dist_pump_pwnVal(water_distribution_data.set_value);
                water_distribution_enable    = RT_TRUE;

                log_d("wd :%d", water_distribution_data.set_value);
            }
        }

        /* Acceleration and deceleration start and stop of water distribution pump */
        if ((water_distribution_enable == RT_TRUE) && water_dist_pump_opsDelay(water_distribution_div)) {
            water_distribution_div = 0;

            if (water_distribution_data.set_value) {
                water_dist_pump_speed_climb_up(water_distribution_current_value, water_distribution_set_value);

                water_dist_pump_ctrl(water_distribution_data.set_value, water_distribution_current_value);
                // HAL_GPIO_WritePin(B_IO_GPIO_Port,B_IO_Pin,1);
                // pwm_write_pulse(ID_WATER_DISTRIBUTION_PWM, water_distribution_current_value);
                // log_d("B4:%d",B_IO_GPIO_Port->IDR & B_IO_Pin);
                // log_d("C6:%d",GPIOC->IDR & GPIO_PIN_6);

                if (is_water_dist_pump_speed_climb_done(water_distribution_current_value, water_distribution_set_value)) {
                    water_distribution_enable = RT_FALSE;
                }
            } else {
                water_distribution_current_value = water_distribution_set_value;
                water_dist_pump_ctrl(water_distribution_data.set_value, water_distribution_current_value);
                water_distribution_enable = RT_FALSE;
            }
        }

#endif

#ifdef USED_WATER_VALVE_CLEAN_CTRL
        if (sub_evt & SYS_EVT_WATER_VALVE_CLEAN) {
            if (xQueuePeek(topic_water_valve_cleanHandle, &water_valve_clean_data, 0) != errQUEUE_EMPTY) {
                water_valve_clean_enable = RT_TRUE;
                water_valve_clean_count  = 0;
            }
        }

        if (water_valve_clean_enable == RT_TRUE) {
            water_valve_clean_ctrl(water_valve_clean_data.set_value);
            // log_d("DC_IO_1:%x",DC_IO_1_GPIO_Port->IDR & DC_IO_1_Pin);
            if (water_valve_clean_delay(water_valve_clean_count)) {
                /* off water valve clean ctrl */
                // water_valve_clean_ctrl(0);
                water_valve_clean_count  = 0;
                water_valve_clean_enable = RT_FALSE;
            }
        }
#endif

#ifdef USED_ROLL_BRUSH_MOTOR_CTRL

        if (sub_evt & SYS_EVT_ROLL_BRUSH_MOTOR) {
            if (xQueuePeek(topic_roll_brush_motorHandle, &roll_brush_motor_data, 0) != errQUEUE_EMPTY) {
                // taskDISABLE_INTERRUPTS();//soft pwm used,in IT modify

                /* direction ctrl */
                roll_brush_motor_dir_ops(roll_brush_motor_data.set_value);

/* keep value */
#if 1
                if (roll_brush_motor_data.set_value)
                    roll_brush_motor_ValRight(roll_brush_motor_data.set_value, 30, 100);
#else
                if (roll_brush_motor_data.set_value)
                    roll_brush_motor_ValRight(roll_brush_motor_data.set_value, 40, 100);
#endif

// taskENABLE_INTERRUPTS();
#if 1
                roll_brush_motor_set_value = roll_brush_motor_pwnVal(roll_brush_motor_data.set_value);
#else
                roll_brush_motor_set_value = roll_brush_motor_data.set_value;
                roll_brush_motor_pwm_value = roll_brush_motor_set_value;
                log_d("roll_brush_motor_pwm_value :%d", roll_brush_motor_pwm_value);
#endif
                roll_brush_motor_enable = RT_TRUE;
                log_d("rm :%d", roll_brush_motor_data.set_value);
            }
        }

#if 1
        /* roll bush run and stop,delay set */
        if ((roll_brush_motor_enable == TRUE) && roll_brush_motor_opsDelay(roll_brush_motor_div)) {
            roll_brush_motor_div = 0;

            roll_brush_motor_speed_climb_up(roll_brush_motor_current_value, roll_brush_motor_set_value)

                roll_brush_motor_ctrl(roll_brush_motor_current_value);

            if (is_roll_motor_speed_climb_done(roll_brush_motor_current_value, roll_brush_motor_set_value)) {
                roll_brush_motor_enable = FALSE;
            }
        }

#else

        if (roll_brush_motor_enable == TRUE) {
            if (roll_brush_motor_data.set_value) {
                taskDISABLE_INTERRUPTS();
                roll_brush_motor_output_enable = 1;
                taskENABLE_INTERRUPTS();
            } else {
                taskDISABLE_INTERRUPTS();
                roll_brush_motor_output_enable = 0;
                taskENABLE_INTERRUPTS();
                roll_brush_motor_ctrl(roll_brush_motor_data.set_value);
            }

            roll_brush_motor_enable = FALSE;
        }

#endif
#endif

#ifdef USED_SUNCTION_MOTOR_CTRL

        if (sub_evt & SYS_EVT_SUNCTION_MOTOR) {
            if (xQueuePeek(topic_sunction_motorHandle, &sunction_motor_data, 0) != errQUEUE_EMPTY) {
                sunction_motor_pwm_value = sunction_motor_data.set_value;
                sunction_motor_set_value = (100 - sunction_motor_data.set_value) * WATER_DISTRIBUTION_CYCLE_NS / 100;
                sunction_motor_enable    = TRUE;
                log_d("sm :%d", sunction_motor_data.set_value);
            }
        }

#if 0

		/* sunction motor run and stop,delay set */
		if ((sunction_motor_enable == RT_TRUE) && (++sunction_motor_div % WATER_DIST_PUMP_STEP_DELAY == 0))
		{
			sunction_motor_div = 0;
			utils_step_towards_uint(&sunction_motor_current_value, sunction_motor_set_value, WATER_DIST_PUMP_STEP);
			pwm_write_pulse(ID_SUNCTION_MOTOR_PWM, sunction_motor_current_value);

			if (sunction_motor_current_value == sunction_motor_set_value)
			{
				sunction_motor_enable = RT_FALSE;
			}
		}

#else

        if (sunction_motor_enable == TRUE) {
            if (sunction_motor_data.set_value) {
                taskDISABLE_INTERRUPTS();
                sunction_motor_output_enable = 1;
                taskENABLE_INTERRUPTS();
            } else {
                taskDISABLE_INTERRUPTS();
                sunction_motor_output_enable = 0;
                taskENABLE_INTERRUPTS();
                clean_io_ctrl(ID_SUNCTION_MOTOR_CTL, sunction_motor_data.set_value);
            }

            sunction_motor_enable = FALSE;
        }

#endif
#endif
        if (++running_time_div >= 10000) {
            running_time_div = 0;
            thread_utime     = clock_cpu_microsecond_diff(thread_start_time, clock_cpu_gettime());
            if (dev_thread_utime)
                log_d("%s utime: %ld", pcTaskGetName(NULL), thread_utime);
        }

        osDelay(THREAD_YIELD_TIME);
    }
}

void dev_thread_utime_toggle(void) {
    dev_thread_utime = !dev_thread_utime;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), dev_thread_utime_toggle, dev_thread_utime_toggle,
                 dev_thread_utime_toggle);
