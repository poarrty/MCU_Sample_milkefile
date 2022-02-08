/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: detection.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: detection operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "detection.h"

#define LOG_TAG "detection"
#include "elog.h"
#define SIDE_BRUSH_MOTOR_LEFT_DET_PIN  GET_PIN(B, 9)   // FG_ZBS_MCU_GPIO_Port,FG_ZBS_MCU_Pin
#define SIDE_BRUSH_MOTOR_RIGHT_DET_PIN GET_PIN(C, 9)   // FG_YBS_MCU_GPIO_Port,FG_YBS_MCU_Pin
#define PUSH_ROD_MOTOR_1_DET_PIN       GET_PIN(C, 3)   // T1_OC_MCU_GPIO_Port,T1_OC_MCU_Pin//A,6
#define PUSH_ROD_MOTOR_2_DET_PIN       GET_PIN(B, 0)   // T2_OC_MCU_GPIO_Port,T2_OC_MCU_Pin
#define WATER_DISTRIBUTION_DET_PIN     GET_PIN(B, 1)   // B_OC_MCU_GPIO_Port,B_OC_MCU_Pin
#define WATER_VALVE_CLEAN_DET_PIN      GET_PIN(C, 13)  // DC_IO_1_GPIO_Port,DC_IO_1_Pin
#define ROLL_BRUSH_MOTOR_DET_PIN       GET_PIN(B, 11)  // FG_FENG_MCU_GPIO_Port,FG_FENG_MCU_Pin

#define MOTOR_REDUCTION_RATIO           28
#define PULSE_COUNT_CYCLE               6
#define SIDE_BRUSH_MOTOR_MIN_SPEED      30
#define SIDE_BRUSH_MOTOR_DET_DELAY_TIME 1000
#define PUSH_ROD_MOTOR_MIN_FREQ         3
#define PUSH_ROD_MOTOR_DET_DELAY_TIME   300
#define PUSH_ROD_MOTOR_DET_CLOSE_TIME   9500
#define SIDE_BURSH_MOTOR_SPEED_COUNT    (500 / THREAD_YILED_TIME)
#define PUSH_ROD_MOTOR_FREQ_COUNT       5
//#define WATER_DISTRIBUTION_DET_DELAY_TIME   300
#define WATER_DISTRIBUTION_DET_DELAY_TIME 600
#define WATER_DISTRIBUTION_MIN_SPEED      10
#define WATER_DISTRIBUTION_FREQ_COUNT     5
#define WATER_VALVE_CLEAN_DET_DELAY_TIME  300
#define WATER_VALVE_CLEAN_DEBOUNCE_COUNT  10
//#define ROLL_BRUSH_MOTOR_DET_DELAY_TIME     300
#define ROLL_BRUSH_MOTOR_DET_DELAY_TIME    600
#define ROLL_BRUSH_MOTOR_DET_MIN_SPEED     10
#define ROLL_BRUSH_MOTOR_FREQ_COUNT        5
#define ROLL_BRUSH_MOTOR_REDUCTION_RATIO   1
#define ROLL_BRUSH_MOTOR_PULSE_COUNT_CYCLE 4
#define KEEP_TIME_US                       100000
#define AD_SAMPLING_TOTAL_NUM              10
#define AD_SAMPLING_DEL_NUM                1
#define PUSH_ROD_MOTOR_BLOCK_VOL           2700
#define THREAD_YILED_TIME                  5

/* detection logic */
#define delay_time(time, delay_time) (++time % delay_time == 0)
/* water valve clean logic */
#define water_valve_clean_delay_time(time) delay_time(time, WATER_VALVE_CLEAN_DET_DELAY_TIME)
/* push rod logic */
#define rod_motor_stop(id)                             \
    do {                                               \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TH, 0); \
        clean_io_ctrl(ID_PUSH_ROD_MOTOR_##id##_TL, 0); \
    } while (0)
#define rod_motor1_stop rod_motor_stop(1)
#define rod_motor2_stop rod_motor_stop(2)

#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION
struct input_cap_freq sbm_left_freq_info;
struct input_cap_freq sbm_right_freq_info;
#endif
#ifdef USED_PUSH_ROD_MOTOR_1_DETECTION
#ifndef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
struct input_cap_freq prm_1_freq_info;
#endif
#endif
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION
#ifndef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
struct input_cap_freq prm_2_freq_info;
#endif
#endif
#ifdef USED_WATER_DIST_PUMP_DETECTION
struct input_cap_freq water_distribution_freq_info;
#endif
#ifdef USED_WATER_VALVE_CLEAN_DETECTION
struct input_cap_freq water_valve_clean_freq_info;
#endif
#ifdef USED_ROLL_BRUSH_MOTOR_DETECTION
struct input_cap_freq roll_brush_motor_freq_info;
#endif

static uint8_t clean_dev_err_status[ID_CLEAN_DEVICE_MAX_ERR_STA] = {0};

extern void clean_ctrl_all_reset(void);

bool det_thread_utime;
bool side_brush_speed_info;
bool water_distribution_info;

uint32_t clean_device_error_status_get(uint8_t clean_dev_id) {
    if (clean_dev_id < ID_CLEAN_DEVICE_MAX_ERR_STA) {
        // log_d("cdes get :%d, %d", clean_dev_id, clean_dev_err_status[clean_dev_id]);
        return clean_dev_err_status[clean_dev_id];
    }

    return 0;
}

uint32_t clean_device_error_status_clear(uint8_t clean_dev_id, uint32_t value) {
    taskENTER_CRITICAL();

    if (value) {
        clean_dev_err_status[clean_dev_id] = 0;
    }

    taskEXIT_CRITICAL();
    return RT_EOK;
}

#ifdef RT_USING_FINSH
void detection_device_info(int argc, char **argv) {
    log_d("sm sta :%d", clean_dev_err_status[ID_SUNCTION_MOTOR_ERR_STA]);
    log_d("rm sta :%d", clean_dev_err_status[ID_ROLL_MOTOR_ERR_STA]);
    log_d("wd sta :%d", clean_dev_err_status[ID_WATER_DIST_ERR_STA]);
    log_d("wv sta :%d", clean_dev_err_status[ID_WATER_VALVE_CLEAN_ERR_STA]);
    log_d("sbm_l sta :%d", clean_dev_err_status[ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA]);
    log_d("sbm_r sta :%d", clean_dev_err_status[ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA]);
    log_d("prm_x :%d", clean_dev_err_status[ID_PUSH_ROD_MOTOR_1_ERR_STA]);
    log_d("prm_c :%d", clean_dev_err_status[ID_PUSH_ROD_MOTOR_2_ERR_STA]);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), detection_device_info, detection_device_info,
                 show water level info);
#endif

#ifdef USED_PUSH_ROD_MOTOR_1_DETECTION
#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
static void push_rod_motor1_det_init(void) {
    adc_enable(ID_PUSH_ROD_MOTOR_1_ADC);
}

static void push_rod_motor1_det_deinit(void) {
    adc_disable(ID_PUSH_ROD_MOTOR_1_ADC);
}
#else
static void push_rod_motor1_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, PUSH_ROD_MOTOR_1_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &prm_1_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, PUSH_ROD_MOTOR_1_DET_PIN, PIN_IRQ_ENABLE);
}

static void push_rod_motor1_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, PUSH_ROD_MOTOR_1_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, PUSH_ROD_MOTOR_1_DET_PIN);
}
#endif
#endif

#ifdef USED_PUSH_ROD_MOTOR2_DETECTION
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
static void push_rod_motor2_det_init(void) {
    adc_enable(ID_PUSH_ROD_MOTOR_2_ADC);
}

static void push_rod_motor2_det_deinit(void) {
    adc_disable(ID_PUSH_ROD_MOTOR_2_ADC);
}
#else
static void push_rod_motor2_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, PUSH_ROD_MOTOR_2_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &prm_2_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, PUSH_ROD_MOTOR_2_DET_PIN, PIN_IRQ_ENABLE);
}

static void push_rod_motor2_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, PUSH_ROD_MOTOR_2_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, PUSH_ROD_MOTOR_2_DET_PIN);
}
#endif
#endif

#ifdef USED_WATER_DIST_PUMP_DETECTION
static void water_distribution_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, WATER_DISTRIBUTION_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &water_distribution_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, WATER_DISTRIBUTION_DET_PIN, PIN_IRQ_ENABLE);
}

static void water_distribution_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, WATER_DISTRIBUTION_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, WATER_DISTRIBUTION_DET_PIN);
}
#endif

#ifdef USED_ROLL_BRUSH_MOTOR_DETECTION
static void roll_brush_motor_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, ROLL_BRUSH_MOTOR_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &roll_brush_motor_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, ROLL_BRUSH_MOTOR_DET_PIN, PIN_IRQ_ENABLE);
}

static void roll_brush_motor_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, ROLL_BRUSH_MOTOR_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, ROLL_BRUSH_MOTOR_DET_PIN);
}
#endif

#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION
#ifdef SIDE_BRUSH_MOTOR_LETF_TIMX_IC
static void side_brush_motor_left_det_init(void) {
    ic_timx_chx_enable();
}

static void side_brush_motor_left_det_deinit(void) {
    ic_timx_chx_disable();
}
#else
static void side_brush_motor_left_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, SIDE_BRUSH_MOTOR_LEFT_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &sbm_left_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, SIDE_BRUSH_MOTOR_LEFT_DET_PIN, PIN_IRQ_ENABLE);
}

static void side_brush_motor_left_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, SIDE_BRUSH_MOTOR_LEFT_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, SIDE_BRUSH_MOTOR_LEFT_DET_PIN);
}
#endif
static void side_brush_motor_right_det_init(void) {
    _stm32_pin_ops.pin_attach_irq(0, SIDE_BRUSH_MOTOR_RIGHT_DET_PIN, PIN_IRQ_MODE_RISING, input_cap_freq_handler, &sbm_right_freq_info);
    _stm32_pin_ops.pin_irq_enable(0, SIDE_BRUSH_MOTOR_RIGHT_DET_PIN, PIN_IRQ_ENABLE);
}

static void side_brush_motor_right_det_deinit(void) {
    _stm32_pin_ops.pin_irq_enable(0, SIDE_BRUSH_MOTOR_RIGHT_DET_PIN, PIN_IRQ_DISABLE);
    _stm32_pin_ops.pin_detach_irq(0, SIDE_BRUSH_MOTOR_RIGHT_DET_PIN);
}
#endif

void detection_device_thread_entry(void *param) {
    uint32_t temp_data       = 0;
    uint64_t last_recv_time1 = 0;

#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION
    uint32_t side_brush_motor_delay_count = 0;
    uint64_t last_recv_time2              = 0;
    uint8_t  side_brush_motor_det_enable  = 0;
    // uint32_t side_brush_motor_sub = 0;
    uint32_t             side_brush_motor_left_speed_temp   = 0;
    uint32_t             side_brush_motor_right_speed_temp  = 0;
    uint32_t             side_brush_motor_left_speed_count  = 0;
    uint32_t             side_brush_motor_right_speed_count = 0;
    clean_module_ctrl_st side_brush_motor_data;

#endif
#ifdef USED_PUSH_ROD_MOTOR_1_DETECTION
    uint32_t push_rod_motor1_delay_count = 0;
    uint8_t  push_rod_motor1_tdet_enable = DET_DISABLE;
    uint8_t  push_rod_motor1_ldet_enable = DET_DISABLE;
    // uint32_t push_rod_motor1_sub = 0;
    clean_module_ctrl_st push_rod_motor1_data;

#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
    uint32_t push_rod_motor1_cov_vol                       = 0;
    uint16_t push_rod_motor1_ad_vol[AD_SAMPLING_TOTAL_NUM] = {0};
    uint16_t push_rod_motor1_block_count                   = 0;
#else
    uint32_t push_rod_motor1_freq_count = 0;
#endif

#endif
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION
    uint32_t push_rod_motor2_delay_count = 0;
    uint8_t  push_rod_motor2_tdet_enable = DET_DISABLE;
    uint8_t  push_rod_motor2_ldet_enable = DET_DISABLE;
    // uint32_t push_rod_motor2_sub = 0;
    clean_module_ctrl_st push_rod_motor2_data;

#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
    uint32_t push_rod_motor2_cov_vol                       = 0;
    uint16_t push_rod_motor2_ad_vol[AD_SAMPLING_TOTAL_NUM] = {0};
    uint16_t push_rod_motor2_block_count                   = 0;
#else
    uint32_t push_rod_motor2_freq_count = 0;
#endif

#endif
#ifdef USED_WATER_DIST_PUMP_DETECTION
    uint32_t water_distribution_delay_count = 0;
    uint8_t  water_distribution_det_enable  = 0;
    // uint32_t water_distribution_sub = 0;
    uint32_t             water_distribution_speed_count = 0;
    clean_module_ctrl_st water_distribution_data;
#endif
#ifdef USED_WATER_VALVE_CLEAN_DETECTION
    uint32_t water_valve_clean_delay_count = 0;
    uint8_t  water_valve_clean_det_enable  = 0;
    // uint32_t water_valve_clean_sub = 0;
    uint32_t             water_valve_clean_debounce_count = 0;
    clean_module_ctrl_st water_valve_clean_data;
#endif
#ifdef USED_SUNCTION_MOTOR_DETECTION
    uint8_t              sunction_motor_det_enable = 0;
    uint32_t             sunction_motor_sub        = 0;
    clean_module_ctrl_st sunction_motor_data;

#endif

#ifdef USED_ROLL_BRUSH_MOTOR_DETECTION
    uint32_t roll_brush_motor_delay_count = 0;
    // uint32_t roll_brush_motor_sub = 0;
    uint8_t              roll_brush_motor_det_enable  = 0;
    uint32_t             roll_brush_motor_speed_count = 0;
    clean_module_ctrl_st roll_brush_motor_data;

#endif

    uint32_t thread_start_time = 0;
    uint32_t thread_utime      = 0;
    uint32_t running_time_div  = 0;

    uint32_t sub_evt = osFlagsError;

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_detectionHandle,
                                   SYS_EVT_SIDE_BRUSH_MOTOR | SYS_EVT_PUSH_ROD_MOTOR_1 | SYS_EVT_PUSH_ROD_MOTOR_2 | SYS_EVT_WATER_DIST_PUMP |
                                       SYS_EVT_WATER_VALVE_CLEAN | SYS_EVT_ROLL_BRUSH_MOTOR | SYS_EVT_SUNCTION_MOTOR,
                                   osFlagsWaitAny, 0);

        if (sub_evt > osFlagsError) {
            sub_evt = 0;
        }

        thread_start_time = clock_cpu_gettime();

#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION

        if (sub_evt & SYS_EVT_SIDE_BRUSH_MOTOR) {
            if (xQueuePeek(topic_side_brush_motorHandle, &side_brush_motor_data, 0) != errQUEUE_EMPTY) {
                if (side_brush_motor_data.set_value >= SIDE_BRUSH_MOTOR_MIN_SPEED / 10) {
                    if (side_brush_motor_det_enable != DET_ENABLE) {
                        side_brush_motor_det_enable        = DET_ENABLE;
                        side_brush_motor_left_speed_count  = 0;
                        side_brush_motor_right_speed_count = 0;
                        side_brush_motor_left_det_init();
                        side_brush_motor_right_det_init();
                        side_brush_motor_delay_count                            = 0;
                        clean_dev_err_status[ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA]  = RT_EOK;
                        clean_dev_err_status[ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA] = RT_EOK;
                        taskDISABLE_INTERRUPTS();
                        memset(&sbm_left_freq_info, 0, sizeof(struct input_cap_freq));
                        memset(&sbm_right_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                } else {
                    if (side_brush_motor_det_enable != DET_DISABLE) {
                        side_brush_motor_det_enable        = DET_DISABLE;
                        side_brush_motor_left_speed_count  = 0;
                        side_brush_motor_right_speed_count = 0;
                        side_brush_motor_left_det_deinit();
                        side_brush_motor_right_det_deinit();
                        taskDISABLE_INTERRUPTS();
                        memset(&sbm_right_freq_info, 0, sizeof(struct input_cap_freq));
                        memset(&sbm_left_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                }
            }
        }

        if (side_brush_motor_det_enable == DET_ENABLE) {
            if (++side_brush_motor_delay_count >= SIDE_BRUSH_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME) {
                side_brush_motor_delay_count = SIDE_BRUSH_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME;
                /* Get edge brush speed,Last edge trigger time */
                taskDISABLE_INTERRUPTS();
                side_brush_motor_left_speed_temp  = sbm_left_freq_info.input_freq;
                side_brush_motor_right_speed_temp = sbm_right_freq_info.input_freq;
                last_recv_time1                   = sbm_left_freq_info.curr_edge_trigger_time;
                last_recv_time2                   = sbm_right_freq_info.curr_edge_trigger_time;
                taskENABLE_INTERRUPTS();
                /* If the left brush frequency acquisition times out, the speed is 0 */
                uint32_t tm_int = clock_cpu_microsecond_diff(last_recv_time1, clock_cpu_gettime());

                if (last_recv_time1 && (tm_int >= KEEP_TIME_US)) {
                    side_brush_motor_left_speed_temp = 0;
                }

                /* If the right brush frequency acquisition times out, the speed is 0 */
                tm_int = clock_cpu_microsecond_diff(last_recv_time2, clock_cpu_gettime());

                if (last_recv_time2 && (tm_int >= KEEP_TIME_US)) {
                    side_brush_motor_right_speed_temp = 0;
                }

                /* Calculate the left and right brush speed according to the frequency */
                side_brush_motor_left_speed_temp  = (side_brush_motor_left_speed_temp * 60) / (MOTOR_REDUCTION_RATIO * PULSE_COUNT_CYCLE);
                side_brush_motor_right_speed_temp = (side_brush_motor_right_speed_temp * 60) / (MOTOR_REDUCTION_RATIO * PULSE_COUNT_CYCLE);
                // log_d("side brush left speed:%d",side_brush_motor_left_speed_temp);
                // log_d("side brush right speed:%d",side_brush_motor_right_speed_temp);

                /* Judge whether the right brush speed is lower than the locked rotor threshold,
                If the rotor is locked for several times continuously, it will be closed */
                if (side_brush_motor_left_speed_temp < SIDE_BRUSH_MOTOR_MIN_SPEED - 10) {
                    if (side_brush_motor_left_speed_count++ >= SIDE_BURSH_MOTOR_SPEED_COUNT) {
                        side_brush_motor_left_speed_count                      = 0;
                        clean_dev_err_status[ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA] = RT_ERROR;
                        side_brush_motor_ops(0, 0);
                        log_e("sbm left oc, %d %d", (unsigned int) side_brush_motor_left_speed_temp,
                              (unsigned int) side_brush_motor_right_speed_temp);
                    }
                } else {
                    side_brush_motor_left_speed_count = 0;
                }

                /* Judge whether the right brush speed is lower than the locked rotor threshold,
                If the rotor is locked for several times continuously, it will be closed */
                if (side_brush_motor_right_speed_temp < SIDE_BRUSH_MOTOR_MIN_SPEED - 10) {
                    if (side_brush_motor_right_speed_count++ >= SIDE_BURSH_MOTOR_SPEED_COUNT) {
                        side_brush_motor_right_speed_count                      = 0;
                        clean_dev_err_status[ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA] = RT_ERROR;
                        side_brush_motor_ops(0, 0);
                        log_e("sbm right oc, %d %d", (unsigned int) side_brush_motor_left_speed_temp,
                              (unsigned int) side_brush_motor_right_speed_temp);
                    }
                } else {
                    side_brush_motor_right_speed_count = 0;
                }
            } else {
                taskDISABLE_INTERRUPTS();
                sbm_left_freq_info.input_freq  = 0;
                sbm_right_freq_info.input_freq = 0;
                taskENABLE_INTERRUPTS();
            }
        }

#endif

#ifdef USED_PUSH_ROD_MOTOR_1_DETECTION

        if (sub_evt & SYS_EVT_PUSH_ROD_MOTOR_1) {
            if (xQueuePeek(topic_push_rod_motor_1Handle, &push_rod_motor1_data, 0) != errQUEUE_EMPTY) {
                if (push_rod_motor1_data.set_value) {
                    if (push_rod_motor1_tdet_enable != DET_ENABLE) {
                        push_rod_motor1_tdet_enable                       = DET_ENABLE;
                        push_rod_motor1_ldet_enable                       = DET_DISABLE;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_1_ERR_STA] = RT_EOK;
                        push_rod_motor1_det_init();
                        // log_d("push rod2 motor start detection");
                        push_rod_motor1_delay_count = 0;
#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
                        push_rod_motor1_cov_vol = 0;
                        memset(push_rod_motor1_ad_vol, 0, AD_SAMPLING_TOTAL_NUM);
                        push_rod_motor1_block_count = 0;
#else
                        push_rod_motor1_freq_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&prm_1_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
#endif
                    }
                } else {
                    if (push_rod_motor1_ldet_enable != DET_ENABLE) {
                        push_rod_motor1_ldet_enable = DET_ENABLE;
                        push_rod_motor1_tdet_enable = DET_DISABLE;
                        push_rod_motor1_delay_count = 0;
                        push_rod_motor1_det_deinit();
#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
                        push_rod_motor1_cov_vol = 0;
                        memset(push_rod_motor1_ad_vol, 0, AD_SAMPLING_TOTAL_NUM);
                        push_rod_motor1_block_count = 0;
#else
                        push_rod_motor1_freq_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&prm_1_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
#endif
                    }
                }
            }
        }

        if ((push_rod_motor1_tdet_enable == DET_ENABLE) || (push_rod_motor1_ldet_enable == DET_ENABLE)) {
            /* The detection time has exceeded the normal time */
            push_rod_motor1_delay_count++;

            if (push_rod_motor1_delay_count > PUSH_ROD_MOTOR_DET_CLOSE_TIME / THREAD_YILED_TIME) {
                /* close detection */
                push_rod_motor1_tdet_enable = DET_DISABLE;
                push_rod_motor1_ldet_enable = DET_DISABLE;
            } else if (push_rod_motor1_delay_count >= PUSH_ROD_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME) {
#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
                memset(push_rod_motor1_ad_vol, 0, AD_SAMPLING_TOTAL_NUM);

                for (uint8_t i = 0; i < AD_SAMPLING_TOTAL_NUM; i++) {
                    if (adc_read(ID_PUSH_ROD_MOTOR_1_ADC, &push_rod_motor1_ad_vol[i]) != RT_EOK) {
                        log_w("%s ad read error", __FUNCTION__);
                        break;
                    }
                }

                // sort
                bubble_sort(push_rod_motor1_ad_vol, AD_SAMPLING_TOTAL_NUM);
                // for(int i = 0; i < 10; i++)
                // rt_kprintf("%d\r\n",push_rod_motor1_ad_vol[i]);
                // average ADC Value
                push_rod_motor1_cov_vol = average_get(push_rod_motor1_ad_vol, AD_SAMPLING_TOTAL_NUM, AD_SAMPLING_DEL_NUM);
                log_d("prm vol:%d", push_rod_motor1_cov_vol);
                // transformation voltage
                push_rod_motor1_cov_vol = push_rod_motor1_cov_vol * REFER_VOLTAGE / CONVERT_BITS;

                if (push_rod_motor1_cov_vol >= PUSH_ROD_MOTOR_BLOCK_VOL) {
                    if (push_rod_motor1_block_count++ >= PUSH_ROD_MOTOR_FREQ_COUNT) {
                        push_rod_motor1_block_count                       = 0;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_1_ERR_STA] = RT_ERROR;
                        rod_motor1_stop;
                        // push_rod_motor_ops(PUSH_ROD_MOTOR_1_NUM, DET_DISABLE);
                        log_e("prm 1 oc, vol:%d", push_rod_motor1_cov_vol);
                    }
                } else {
                    push_rod_motor1_block_count = 0;
                }

#else
                taskDISABLE_INTERRUPTS();
                temp_data = prm_1_freq_info.input_freq;
                taskENABLE_INTERRUPTS();

                if (temp_data >= PUSH_ROD_MOTOR_MIN_FREQ) {
                    if (push_rod_motor1_freq_count++ > PUSH_ROD_MOTOR_FREQ_COUNT) {
                        push_rod_motor1_freq_count                        = 0;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_1_ERR_STA] = RT_ERROR;
                        rod_motor1_stop;
                        // push_rod_motor_ops(PUSH_ROD_MOTOR_1_NUM, DET_DISABLE);
                        log_e("prm 1 oc");
                    }
                } else {
                    push_rod_motor1_freq_count = 0;
                }

#endif
            } else {
#ifdef USED_PUSH_ROD_MOTOR1_DETECTION_ADC
                push_rod_motor1_cov_vol     = 0;
                push_rod_motor1_block_count = 0;
#else
                taskDISABLE_INTERRUPTS();
                prm_1_freq_info.input_freq = 0;
                taskENABLE_INTERRUPTS();
#endif
            }
        }

#endif

#ifdef USED_PUSH_ROD_MOTOR2_DETECTION

        if (sub_evt & SYS_EVT_PUSH_ROD_MOTOR_2) {
            if (xQueuePeek(topic_push_rod_motor_2Handle, &push_rod_motor2_data, 0) != errQUEUE_EMPTY) {
                if (push_rod_motor2_data.set_value) {
                    if (push_rod_motor2_tdet_enable != DET_ENABLE) {
                        push_rod_motor2_tdet_enable                       = DET_ENABLE;
                        push_rod_motor2_tdet_enable                       = DET_DISABLE;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_2_ERR_STA] = RT_EOK;
                        // log_d("push rod2 motor start detection");
                        push_rod_motor2_det_init();
                        push_rod_motor2_delay_count = 0;
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
                        push_rod_motor2_cov_vol = 0;
                        memset(push_rod_motor2_ad_vol, 0, AD_SAMPLING_TOTAL_NUM);
                        push_rod_motor2_block_count = 0;
#else
                        push_rod_motor2_freq_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&prm_2_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
#endif
                    }
                } else {
                    if (push_rod_motor2_ldet_enable != DET_ENABLE) {
                        push_rod_motor2_ldet_enable = DET_ENABLE;
                        push_rod_motor2_tdet_enable = DET_DISABLE;
                        push_rod_motor2_delay_count = 0;
                        push_rod_motor2_det_deinit();
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
                        push_rod_motor2_cov_vol = 0;
                        memset(push_rod_motor2_ad_vol, 0, AD_SAMPLING_TOTAL_NUM);
                        push_rod_motor2_block_count = 0;
#else
                        push_rod_motor2_freq_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&prm_2_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
#endif
                    }
                }
            }
        }

        if ((push_rod_motor2_tdet_enable == DET_ENABLE) || (push_rod_motor2_ldet_enable == DET_ENABLE)) {
            push_rod_motor2_delay_count++;

            /* The detection time has exceeded the normal time */
            if (push_rod_motor2_delay_count > PUSH_ROD_MOTOR_DET_CLOSE_TIME / THREAD_YILED_TIME) {
                /* close detection */
                push_rod_motor2_tdet_enable = DET_DISABLE;
                push_rod_motor2_ldet_enable = DET_DISABLE;
            } else if (push_rod_motor2_delay_count >= PUSH_ROD_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME) {
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC

                for (uint8_t i = 0; i < AD_SAMPLING_TOTAL_NUM; i++) {
                    if (adc_read(ID_PUSH_ROD_MOTOR_2_ADC, &push_rod_motor2_ad_vol[i]) != RT_EOK) {
                        log_w("%s ad read error", __FUNCTION__);
                        break;
                    }
                }

                // sort
                bubble_sort(push_rod_motor2_ad_vol, AD_SAMPLING_TOTAL_NUM);
                // average adc value
                push_rod_motor2_cov_vol = average_get(push_rod_motor2_ad_vol, AD_SAMPLING_TOTAL_NUM, AD_SAMPLING_DEL_NUM);
                // transformation voltage
                push_rod_motor2_cov_vol = push_rod_motor2_cov_vol * REFER_VOLTAGE / CONVERT_BITS;

                // log_e("prm 2,vol:%d", push_rod_motor2_cov_vol);
                if (push_rod_motor2_cov_vol >= PUSH_ROD_MOTOR_BLOCK_VOL) {
                    if (push_rod_motor2_block_count++ >= PUSH_ROD_MOTOR_FREQ_COUNT) {
                        push_rod_motor2_block_count                       = 0;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_2_ERR_STA] = RT_ERROR;
                        rod_motor2_stop;
                        // push_rod_motor_ops(PUSH_ROD_MOTOR_1_NUM, DET_DISABLE);
                        log_e("prm 2 oc,vol:%d", push_rod_motor2_cov_vol);
                    }
                } else {
                    push_rod_motor2_block_count = 0;
                }

#else
                taskDISABLE_INTERRUPTS();
                temp_data = prm_2_freq_info.input_freq;
                taskENABLE_INTERRUPTS();

                if (temp_data >= PUSH_ROD_MOTOR_MIN_FREQ) {
                    if (push_rod_motor2_freq_count++ > PUSH_ROD_MOTOR_FREQ_COUNT) {
                        push_rod_motor2_freq_count                        = 0;
                        clean_dev_err_status[ID_PUSH_ROD_MOTOR_2_ERR_STA] = RT_ERROR;
                        rod_motor2_stop;
                        // push_rod_motor_ops(PUSH_ROD_MOTOR_2_NUM, DET_DISABLE);
                        log_e("prm 2 oc");
                    }
                } else {
                    push_rod_motor2_freq_count = 0;
                }

#endif
            } else {
#ifdef USED_PUSH_ROD_MOTOR2_DETECTION_ADC
                push_rod_motor2_cov_vol     = 0;
                push_rod_motor2_block_count = 0;
#else
                taskDISABLE_INTERRUPTS();
                prm_2_freq_info.input_freq = 0;
                taskENABLE_INTERRUPTS();
#endif
            }
        }

#endif

#ifdef USED_WATER_DIST_PUMP_DETECTION

        if (sub_evt & SYS_EVT_WATER_DIST_PUMP) {
            if (xQueuePeek(topic_water_dist_pumpHandle, &water_distribution_data, 0) != errQUEUE_EMPTY) {
                if (water_distribution_data.set_value) {
                    if (water_distribution_det_enable != DET_ENABLE) {
                        water_distribution_det_enable               = DET_ENABLE;
                        water_distribution_speed_count              = 0;
                        clean_dev_err_status[ID_WATER_DIST_ERR_STA] = RT_EOK;
                        water_distribution_det_init();
                        water_distribution_delay_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&water_distribution_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                } else {
                    if (water_distribution_det_enable != DET_DISABLE) {
                        water_distribution_det_enable  = DET_DISABLE;
                        water_distribution_speed_count = 0;
                        water_distribution_det_deinit();
                        taskDISABLE_INTERRUPTS();
                        memset(&water_distribution_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                }
            }
        }

        if (water_distribution_det_enable == DET_ENABLE) {
            if (++water_distribution_delay_count >= WATER_DISTRIBUTION_DET_DELAY_TIME / THREAD_YILED_TIME) {
                water_distribution_delay_count = WATER_DISTRIBUTION_DET_DELAY_TIME / THREAD_YILED_TIME;
                taskDISABLE_INTERRUPTS();
                temp_data       = water_distribution_freq_info.input_freq;
                last_recv_time1 = water_distribution_freq_info.curr_edge_trigger_time;
                taskENABLE_INTERRUPTS();
                /* no edage change for time length, it is stoped,Considered speed is 0 */
                uint32_t tm_int = clock_cpu_microsecond_diff(last_recv_time1, clock_cpu_gettime());

                if (last_recv_time1 && (tm_int >= KEEP_TIME_US)) {
                    temp_data = 0;
                }

                if (temp_data <= WATER_DISTRIBUTION_MIN_SPEED) {
                    if (water_distribution_speed_count++ >= WATER_DISTRIBUTION_FREQ_COUNT) {
                        water_distribution_speed_count              = 0;
                        clean_dev_err_status[ID_WATER_DIST_ERR_STA] = RT_ERROR;
                        water_distribution_pump_ops(0, 0);
                        // water_distribution_pump_ops(WATER_DISTRIBUTION_NUM, DET_DISABLE);
                        log_e("water dist oc");
                    }
                }
            } else {
                taskDISABLE_INTERRUPTS();
                water_distribution_freq_info.input_freq = 0;
                taskENABLE_INTERRUPTS();
            }
        }

#endif

#ifdef USED_WATER_VALVE_CLEAN_DETECTION

        if (sub_evt & SYS_EVT_WATER_VALVE_CLEAN) {
            if (xQueuePeek(topic_water_valve_cleanHandle, &water_valve_clean_data, 0) != errQUEUE_EMPTY) {
                if (water_valve_clean_data.set_value) {
                    if (water_valve_clean_det_enable != DET_ENABLE) {
                        water_valve_clean_det_enable                       = DET_ENABLE;
                        clean_dev_err_status[ID_WATER_VALVE_CLEAN_ERR_STA] = RT_EOK;
                        water_valve_clean_delay_count                      = 0;
                        water_valve_clean_debounce_count                   = 0;
                        // taskDISABLE_INTERRUPTS();
                        memset(&water_valve_clean_freq_info, 0, sizeof(struct input_cap_freq));
                        // taskENABLE_INTERRUPTS();
                    }
                } else {
                    if (water_valve_clean_det_enable != DET_DISABLE) {
                        water_valve_clean_det_enable     = DET_DISABLE;
                        water_valve_clean_delay_count    = 0;
                        water_valve_clean_debounce_count = 0;
                        // taskDISABLE_INTERRUPTS();
                        memset(&water_valve_clean_freq_info, 0, sizeof(struct input_cap_freq));
                        // taskENABLE_INTERRUPTS();
                    }
                }
            }
        }

        if (water_valve_clean_det_enable == DET_ENABLE) {
            if (water_valve_clean_delay_time(water_valve_clean_delay_count)) {
                water_valve_clean_delay_count = WATER_VALVE_CLEAN_DET_DELAY_TIME / THREAD_YILED_TIME;
                // taskDISABLE_INTERRUPTS();
                temp_data = gpio_in_status_read(ID_WATER_VALCE_CLEAN_OC);
                // taskENABLE_INTERRUPTS();
                // log_e("water valve clean status：%d",temp_data);

                if (!temp_data) {
                    if (water_valve_clean_debounce_count++ >= WATER_VALVE_CLEAN_DEBOUNCE_COUNT) {
                        water_valve_clean_debounce_count            = 0;
                        clean_dev_err_status[ID_WATER_DIST_ERR_STA] = RT_ERROR;
                        water_valve_clean_ops(0, 0);
                        // water_distribution_pump_ops(WATER_DISTRIBUTION_NUM, DET_DISABLE);
                        log_e("water valve clean oc");
                    }
                }
            } else {
                // taskDISABLE_INTERRUPTS();
                // taskENABLE_INTERRUPTS();
            }
        }

#endif

#ifdef USED_SUNCTION_MOTOR_DETECTION
        if (sub_evt & SYS_EVT_SUNCTION_MOTOR) {
            if (xQueuePeek(topic_sunction_motorHandle, &sunction_motor_data, 0) != errQUEUE_EMPTY) {
                if (sunction_motor_data.set_value) {
                    if (sunction_motor_det_enable != DET_ENABLE) {
                        sunction_motor_det_enable                       = DET_ENABLE;
                        clean_dev_err_status[ID_SUNCTION_MOTOR_ERR_STA] = RT_EOK;
                    }
                }
            }
        }

#endif

#ifdef USED_ROLL_BRUSH_MOTOR_DETECTION

        if (sub_evt & SYS_EVT_ROLL_BRUSH_MOTOR) {
            if (xQueuePeek(topic_roll_brush_motorHandle, &roll_brush_motor_data, 0) != errQUEUE_EMPTY) {
                if (roll_brush_motor_data.set_value) {
                    if (roll_brush_motor_det_enable != DET_ENABLE) {
                        roll_brush_motor_det_enable                 = DET_ENABLE;
                        roll_brush_motor_speed_count                = 0;
                        clean_dev_err_status[ID_ROLL_MOTOR_ERR_STA] = RT_EOK;
                        roll_brush_motor_det_init();
                        roll_brush_motor_delay_count = 0;
                        taskDISABLE_INTERRUPTS();
                        memset(&roll_brush_motor_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                } else {
                    if (roll_brush_motor_det_enable != DET_DISABLE) {
                        roll_brush_motor_det_enable  = DET_DISABLE;
                        roll_brush_motor_speed_count = 0;
                        roll_brush_motor_det_deinit();
                        taskDISABLE_INTERRUPTS();
                        memset(&roll_brush_motor_freq_info, 0, sizeof(struct input_cap_freq));
                        taskENABLE_INTERRUPTS();
                    }
                }
            }
        }

        if (roll_brush_motor_det_enable == DET_ENABLE) {
            if (++roll_brush_motor_delay_count >= ROLL_BRUSH_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME) {
                roll_brush_motor_delay_count = ROLL_BRUSH_MOTOR_DET_DELAY_TIME / THREAD_YILED_TIME;
                taskDISABLE_INTERRUPTS();
                temp_data       = roll_brush_motor_freq_info.input_freq;
                temp_data       = temp_data * 10;
                last_recv_time1 = roll_brush_motor_freq_info.curr_edge_trigger_time;
                taskENABLE_INTERRUPTS();
                /* no edage change for time length, it is stoped,Considered speed is 0 */
                uint32_t tm_int = clock_cpu_microsecond_diff(last_recv_time1, clock_cpu_gettime());

                if (last_recv_time1 && (tm_int >= KEEP_TIME_US)) {
                    temp_data = 0;
                }

                temp_data = (temp_data * 60) / (ROLL_BRUSH_MOTOR_REDUCTION_RATIO * ROLL_BRUSH_MOTOR_PULSE_COUNT_CYCLE);

                if (temp_data <= ROLL_BRUSH_MOTOR_DET_MIN_SPEED) {
                    if (roll_brush_motor_speed_count++ >= ROLL_BRUSH_MOTOR_FREQ_COUNT) {
                        roll_brush_motor_speed_count                = 0;
                        clean_dev_err_status[ID_ROLL_MOTOR_ERR_STA] = RT_ERROR;
                        roll_brush_motor_ops(0, 0);
                        log_e("roll brush motor oc");
                    }
                }
            } else {
                taskDISABLE_INTERRUPTS();
                roll_brush_motor_freq_info.input_freq = 0;
                taskENABLE_INTERRUPTS();
            }
        }

#endif

        if (++running_time_div >= 2000) {
            running_time_div = 0;
            thread_utime     = clock_cpu_microsecond_diff(thread_start_time, clock_cpu_gettime());
#ifdef USED_WATER_DIST_PUMP_DETECTION
            if (water_distribution_info)
                log_d("water dis :%d", water_distribution_freq_info.input_freq);
#endif
#ifdef USED_SIDE_BRUSH_MOTOR_DETECTION
            if (side_brush_speed_info)
                log_d("left :%ld, right :%ld", side_brush_motor_left_speed_temp, side_brush_motor_right_speed_temp);
#endif
            if (det_thread_utime)
                log_d("%s utime: %ld", pcTaskGetName(NULL), thread_utime);
        }

        osDelay(THREAD_YILED_TIME);
    }
}

void water_distribution_info_toggle(void) {
    water_distribution_info = !water_distribution_info;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), water_distribution_info_toggle, water_distribution_info_toggle,
                 water_distribution_info_toggle);

void side_brush_speed_info_toggle(void) {
    side_brush_speed_info = !side_brush_speed_info;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), side_brush_speed_info_toggle, side_brush_speed_info_toggle,
                 side_brush_speed_info_toggle);

void det_thread_utime_toggle(void) {
    det_thread_utime = !det_thread_utime;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), det_thread_utime_toggle, det_thread_utime_toggle,
                 det_thread_utime_toggle);

#ifdef RT_USING_FINSH
void time_check(void) {
    uint32_t time_start = clock_cpu_gettime();
    osDelay(100);
    log_d("time :%d", (unsigned int) clock_cpu_microsecond_diff(time_start, clock_cpu_gettime()));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), time_check, time_check, time check);

void rbm_speed(void) {
#ifdef USED_ROLL_BRUSH_MOTOR_DETECTION
    log_d("roll brush motor speed:%d", roll_brush_motor_freq_info.input_freq * 10);
#endif
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rbm_speed, rbm_speed, roll brush motor speed);
#endif
