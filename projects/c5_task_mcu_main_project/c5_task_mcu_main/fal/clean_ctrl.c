#include "bsp_clean_ctrl.h"
#include "clean_def.h"
#include "bsp_gpio_out.h"
#include "bsp_gpio_in.h"
#include "bsp_pwm.h"
#include "common.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "button.h"
#include "shell.h"
#include "log.h"
#include "clean_ctrl.h"
#include "cmsis_os.h"

#define LOG_TAG "clean_ctrl"
#include "elog.h"

#define THREAD_YIELD_TIME_MS       5
#define FILTER_PUMP_TIME_COUNT     (100 / THREAD_YIELD_TIME_MS)
#define KEEP_TIME_MS               (4500 / THREAD_YIELD_TIME_MS)
#define FILTER_PUMP_DET_DELAY_TIME (1000 / THREAD_YIELD_TIME_MS)

/* 过滤泵操作 */
static uint8_t  filter_pump_ctrl_enable    = 0;
static uint32_t filter_pump_current_value  = 0;
static uint32_t filter_pump_set_value      = 0;
static uint32_t filter_pump_set_speed      = 0;
static uint32_t filter_pump_time_count     = 0;
static uint8_t  filter_pump_det_enable     = 0;
static uint8_t  filter_pump_det_start_once = 0;
static uint32_t filter_pump_det_count      = 0;

/* 污排水阀操作 */
static uint8_t  sewage_water_valve_ctrl_enable     = 0;
static uint32_t sewage_water_valve_set_value       = 0;
static uint8_t  sewage_water_valve_det_enable      = 0;
static uint8_t  sewage_water_valve_start_once      = 0;
static uint32_t sewage_water_valve_det_count       = 0;
static uint32_t sewage_water_valve_det_delay_close = 0;

/* 清水阀操作 */
static uint8_t  clean_water_valve_ctrl_enable    = 0;
static uint32_t clean_water_valve_set_value      = 0;
static uint8_t  clean_water_valve_det_enable     = 0;
static uint8_t  clean_water_valve_det_start_once = 0; /* 状态变化时才有效*/

/* 污水阀操作 */
static uint8_t  waste_water_valve_ctrl_enable    = 0;
static uint32_t waste_water_valve_set_value      = 0;
static uint8_t  waste_water_valve_det_enable     = 0;
static uint8_t  waste_water_valve_det_start_once = 0; /* 状态变化时有效 */

/* 设备检测 */
static button_t detection_device[ID_DET_DEFAULT_MAX];
static uint8_t  clean_dev_err_status[ID_DET_DEFAULT_MAX] = {0};

int32_t clean_device_error_status_get(uint8_t clean_dev_id) {
    if (clean_dev_id < ID_DET_DEFAULT_MAX) {
        return clean_dev_err_status[clean_dev_id];
    }
    return 0;
}

uint32_t clean_device_error_status_clear(uint8_t clean_dev_id, uint32_t value) {
    if (value) {
        clean_dev_err_status[clean_dev_id] = 0;
    }
    return RT_EOK;
}

uint32_t clean_device_status_get(uint8_t clean_dev_id) {
    uint32_t speed = 0;
    if (clean_dev_id < ID_DET_DEFAULT_MAX) {
        switch (clean_dev_id) {
            case ID_FILTER_PUMP_DET:
                speed = filter_pump_set_speed;
                break;
            case ID_SEWAGE_WATER_VALVE_DET:
                speed = sewage_water_valve_set_value;
                break;
            case ID_WASTE_WATER_VALVE_DET:
                speed = waste_water_valve_set_value;
                break;
            default:
                break;
        }
    }
    return speed;
}

static int filter_pump_det_status_read(void) {
    return gpio_in_status_read(ID_FILTER_PUMP_OC);
}

static void filter_pump_det_press_handler(void *param) {
    /* 过滤水泵过流异常 */
    clean_dev_err_status[ID_FILTER_PUMP_DET] = 1;
    fliter_pmup_set_data(0);
    waste_water_valve_set_data(0);
    sewage_water_valve_set_data(0);
    log_e("filter pump oc error");
}

static void filter_pump_det_init(void) {
    button_create("pump det", &detection_device[ID_FILTER_PUMP_DET], filter_pump_det_status_read, PIN_HIGH);
    button_attach(&detection_device[ID_FILTER_PUMP_DET], BUTTON_LONG, filter_pump_det_press_handler);
}

static void filter_pump_det_deinit(void) {
    button_delete(&detection_device[ID_FILTER_PUMP_DET]);
    memset(&detection_device[ID_FILTER_PUMP_DET], 0, sizeof(button_t));
}

static int sewage_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_SEWAGE_WATER_VALVE_OC);
}

static void sewage_water_valve_det_press_handler(void *param) {
    /* 污排水阀过流异常 */
    clean_dev_err_status[ID_SEWAGE_WATER_VALVE_DET] = 1;
    sewage_water_valve_set_data(0);
    log_e("sewage water valve oc error");
}

static void sewage_water_valve_det_init(void) {
    button_create("sewage water", &detection_device[ID_SEWAGE_WATER_VALVE_DET], sewage_water_valve_det_status_read, PIN_HIGH);
    button_attach(&detection_device[ID_SEWAGE_WATER_VALVE_DET], BUTTON_LONG, sewage_water_valve_det_press_handler);
}

static void sewage_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_SEWAGE_WATER_VALVE_DET]);
    memset(&detection_device[ID_SEWAGE_WATER_VALVE_DET], 0, sizeof(button_t));
}

static int clean_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_CLEAN_WATER_VALVE_OC);
}

static void clean_water_valve_det_press_handler(void *param) {
    /* 清水阀断路异常 */
    clean_dev_err_status[ID_CLEAN_WATER_VALVE_DET] = 1;
    fliter_pmup_set_data(0);
    waste_water_valve_set_data(0);
    sewage_water_valve_set_data(0);
    log_e("clean water valve oc error");
}

static void clean_water_valve_det_init(void) {
    button_create("clean water", &detection_device[ID_CLEAN_WATER_VALVE_DET], clean_water_valve_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_CLEAN_WATER_VALVE_DET], BUTTON_LONG, clean_water_valve_det_press_handler);
}

static void clean_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_CLEAN_WATER_VALVE_DET]);
    memset(&detection_device[ID_CLEAN_WATER_VALVE_DET], 0, sizeof(button_t));
}

static int waste_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_WASTE_WATER_VALVE_OC);
}

static void waste_water_valve_det_press_handler(void *param) {
    /* 污水阀断路异常 */
    clean_dev_err_status[ID_WASTE_WATER_VALVE_DET] = 1;
    fliter_pmup_set_data(0);
    waste_water_valve_set_data(0);
    sewage_water_valve_set_data(0);
    log_e("waste water valve oc error");
}

static void waste_water_valve_det_init(void) {
    button_create("waste water", &detection_device[ID_WASTE_WATER_VALVE_DET], waste_water_valve_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_WASTE_WATER_VALVE_DET], BUTTON_LONG, waste_water_valve_det_press_handler);
}

static void waste_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_WASTE_WATER_VALVE_DET]);
    memset(&detection_device[ID_WASTE_WATER_VALVE_DET], 0, sizeof(button_t));
}

/*设定设定接口函数 */
void fliter_pmup_set_data(uint32_t data) {
    filter_pump_set_value = data;
    filter_pump_set_speed = data;
    if (filter_pump_set_value > 0) {
        filter_pump_set_value = FILTER_PUMP_CYCLE_NS;
        if (!filter_pump_det_enable) {
            /* 延迟启动过流检测 */
            filter_pump_det_enable     = 1;
            filter_pump_det_start_once = 1;
            filter_pump_det_count      = 0;
        }
    } else {
        filter_pump_set_value = 0;
        if (filter_pump_det_enable) {
            filter_pump_det_enable     = 0;
            filter_pump_det_start_once = 1;
        }
    }
    filter_pump_ctrl_enable = 1;
    filter_pump_time_count  = 0;
    log_d("filter pump set:%ld", filter_pump_set_value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, fliter_pmup_set_data,
                 fliter_pmup_set_data, fliter_pmup_set_data 1);

void sewage_water_valve_set_data(uint32_t data) {
    sewage_water_valve_set_value = data;
    /* 开启污排水阀过流检测，启动完全打开后关闭检测功能 */
    if (sewage_water_valve_set_value) {
        sewage_water_valve_det_enable = 1;
    } else {
        sewage_water_valve_det_enable = 0;
    }
    sewage_water_valve_start_once  = 1;
    sewage_water_valve_det_count   = 0;
    sewage_water_valve_ctrl_enable = 1;
    log_d("sewage water valve set:%d", sewage_water_valve_det_enable);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, sewage_water_valve_set_data,
                 sewage_water_valve_set_data, sewage_water_valve_set_data 1);

void clean_water_valve_set_data(uint32_t data) {
    clean_water_valve_set_value   = data;
    clean_water_valve_ctrl_enable = 1;
    if (clean_water_valve_set_value > 0) {
        if (!clean_water_valve_det_enable) {
            clean_water_valve_det_enable     = 1;
            clean_water_valve_det_start_once = 1;
        }
    } else {
        if (clean_water_valve_det_enable) {
            clean_water_valve_det_enable     = 0;
            clean_water_valve_det_start_once = 1;
        }
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, clean_water_valve_set_data,
                 clean_water_valve_set_data, clean_water_valve_set_data 1);

void waste_water_valve_set_data(uint32_t data) {
    waste_water_valve_set_value   = data;
    waste_water_valve_ctrl_enable = 1;
    if (waste_water_valve_set_value > 0) {
        if (!waste_water_valve_det_enable) {
            waste_water_valve_det_enable     = 1;
            waste_water_valve_det_start_once = 1;
        }
    } else {
        if (waste_water_valve_det_enable) {
            waste_water_valve_det_enable     = 0;
            waste_water_valve_det_start_once = 1;
        }
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, waste_water_valve_set_data,
                 waste_water_valve_set_data, waste_water_valve_set_data 1);

// #ifdef RT_USING_FINSH
// #include "finsh.h"
// void detection_device_info(int argc, char **argv)
// {
//     LOG_D("fp sta :%d\r\n", clean_dev_err_status[ID_FILTER_PUMP_DET]);
//     LOG_D("swv sta :%d\r\n",
//     clean_dev_err_status[ID_SEWAGE_WATER_VALVE_DET]); LOG_D("cwv sta
//     :%d\r\n", clean_dev_err_status[ID_CLEAN_WATER_VALVE_DET]); LOG_D("wwv sta
//     :%d\r\n", clean_dev_err_status[ID_WASTE_WATER_VALVE_DET]);
// }
// MSH_CMD_EXPORT_ALIAS(detection_device_info, det_info, show water level info);
// #endif

/*******************************************************************************
 * Function Name  : task_clean_ctrl_det_run
 * Description    : 清洁设备组件控制线程处理函数
 * Input          : param：未使用
 * Output         : NULL
 * Return         : NULL
 *******************************************************************************/
void task_clean_ctrl_det_run(void *argument) {
    while (1) {
        osDelay(THREAD_YIELD_TIME_MS);
#ifdef USED_FLITER_PUMP_CTRL
        /* 过滤泵加减速启动 */
        if ((filter_pump_ctrl_enable) && (filter_pump_time_count++ >= FILTER_PUMP_TIME_COUNT)) {
            filter_pump_time_count = 0;
            if (filter_pump_set_value) {
                utils_step_towards_uint(&filter_pump_current_value, filter_pump_set_value, FILTER_PUMP_CYCLE_NS / 100);
                if ((filter_pump_current_value < filter_pump_set_value) && (filter_pump_current_value > FILTER_PUMP_CYCLE_NS / 10)) {
                    filter_pump_current_value = filter_pump_set_value;
                }
            } else {
                filter_pump_current_value = filter_pump_set_value;
            }

            /* 过滤水泵PWM控制 */
            pwm_write_pulse(ID_FILTER_PUMP_PWM, filter_pump_current_value);
            if (filter_pump_current_value == filter_pump_set_value) {
                filter_pump_ctrl_enable = 0;
            }
            log_d("fliter pump curr value :%ld", filter_pump_current_value);
        }
#endif

#ifdef USED_FILTER_PUMP_DETECTION
        if (filter_pump_det_start_once) {
            if (filter_pump_det_enable) {
                if (++filter_pump_det_count >= FILTER_PUMP_DET_DELAY_TIME) {
                    filter_pump_det_start_once = 0;
                    filter_pump_det_count      = 0;
                    filter_pump_det_init();
                }
            } else {
                filter_pump_det_start_once = 0;
                filter_pump_det_deinit();
            }
        }
#endif

#ifdef USED_SEWAGE_WATER_CTRL
        if (sewage_water_valve_ctrl_enable) {
            clean_io_ctrl(ID_SEWAGE_WATER_VALVE, sewage_water_valve_set_value);
            sewage_water_valve_ctrl_enable = 0;
            log_d("sewage water set:%ld", sewage_water_valve_set_value);
        }
#endif
#ifdef USED_SEWAGE_WATER_DETECTION
        if (sewage_water_valve_start_once) {
            if (sewage_water_valve_det_enable) {
                sewage_water_valve_start_once      = 0;
                sewage_water_valve_det_delay_close = 1;
                sewage_water_valve_det_init();
            } else {
                sewage_water_valve_start_once      = 0;
                sewage_water_valve_det_delay_close = 0;
                sewage_water_valve_det_deinit();
            }
        }
        /* 污排水阀达到全开时间，则关闭过流保护 */
        if (sewage_water_valve_det_delay_close && (++sewage_water_valve_det_count >= KEEP_TIME_MS)) {
            sewage_water_valve_det_delay_close = 0;
            sewage_water_valve_det_deinit();
        }
#endif

#ifdef USED_CLEAN_WATER_CTRL
        if (clean_water_valve_ctrl_enable) {
            clean_io_ctrl(ID_CLEAN_WATER_VALVE, clean_water_valve_set_value);
            clean_water_valve_ctrl_enable = 0;
            log_d("clean water valve set:%ld", clean_water_valve_set_value);
        }
#endif

#ifdef USED_CLEAN_WATER_DETECTION
        if (clean_water_valve_det_start_once) {
            clean_water_valve_det_start_once = 0;
            /* 清水阀断路检测 */
            if (clean_water_valve_det_enable) {
                clean_water_valve_det_init();
            } else {
                clean_water_valve_det_deinit();
            }
        }
#endif

#ifdef USED_WASTE_WATER_CTRL
        if (waste_water_valve_ctrl_enable) {
            clean_io_ctrl(ID_WASTE_WATER_VALVE, waste_water_valve_set_value);
            waste_water_valve_ctrl_enable = 0;
            log_d("waste water valve set:%ld", waste_water_valve_set_value);
        }
#endif

#ifdef USED_WASTE_WATER_DETECTION
        if (waste_water_valve_det_start_once) {
            waste_water_valve_det_start_once = 0;
            if (waste_water_valve_det_enable) {
                /* 污水阀断路检测 */
                waste_water_valve_det_init();
            } else {
                waste_water_valve_det_deinit();
            }
        }

#endif
    }
}
