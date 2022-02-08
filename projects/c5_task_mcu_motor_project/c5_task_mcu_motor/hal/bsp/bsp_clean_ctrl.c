/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_clean_ctrl.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: clean unit operation interface entry
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_clean_ctrl.h"

#define LOG_TAG "bsp_clean_ctrl"
#include "elog.h"
#define USED_BSP_CLEAN_CTRL
#ifdef USED_BSP_CLEAN_CTRL

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])

typedef struct clean_io_status {
    uint8_t  gpio_id;
    uint16_t error_flag;
    uint32_t status;
} clean_io_status_st;

clean_io_status_st push_rod_motor_table[] = {
    // push rod motor 1
    {ID_PUSH_ROD_MOTOR_1_TH, 0, 0},
    {ID_PUSH_ROD_MOTOR_1_TL, 0, 0},
    // push rod motor 2
    {ID_PUSH_ROD_MOTOR_2_TH, 0, 0},
    {ID_PUSH_ROD_MOTOR_2_TL, 0, 0}};

static uint32_t roll_brush_motor_speed   = 0;
static uint32_t sunction_motor_speed     = 0;
static uint32_t suntion_motor_error      = 0;
static uint32_t water_pump_speed         = 0;
static uint32_t water_valve_clean_status = 0;
static uint32_t side_brush_speed         = 0;
static uint32_t push_rod_motor1_status   = 0;
static uint32_t push_rod_motor2_status   = 0;

uint32_t clean_io_ctrl(uint8_t gpio_id, uint32_t value) {
    if (value) {
        gpio_out_on(gpio_id);
    } else {
        gpio_out_off(gpio_id);
    }

    return value;
}

void clean_ctrl_all_reset(void) {
    push_rod_motor_ops(0, 0);
    push_rod_motor_ops(1, 0);

    // sunction_motor_ops(0, 0);
    roll_brush_motor_ops(0, 0);
    water_distribution_pump_ops(0, 0);
    water_valve_clean_ops(0, 0);
    side_brush_motor_ops(0, 0);
    fan_motor_speed_set_ops(0, 0);
}

uint32_t side_brush_motor_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    side_brush_speed    = value;
    set_data.device_num = id;
    set_data.set_value  = value;
    pub_topic(SYS_EVT_SIDE_BRUSH_MOTOR, &set_data);
    return RT_EOK;
}

uint32_t side_brush_motor_status_get(uint8_t id) {
    // log_d("sbm get :%d", side_brush_speed);
    return side_brush_speed;
}

uint32_t side_brush_motor_cw_ops(uint8_t id, uint32_t value) {
    switch (id) {
        case 0:
            clean_io_ctrl(ID_SIDE_BRUSH_MOTOR_LEFT_CW, value);
            break;

        case 1:
            clean_io_ctrl(ID_SIDE_BRUSH_MOTOR_RIGHT_CW, value);
            break;

        default:
            break;
    }

    return 0;
}

uint32_t side_brush_motor_cw_status_get(uint8_t id) {
    switch (id) {
        case 0:
            break;

        case 1:
            break;

        default:
            break;
    }

    return 0;
}

uint32_t push_rod_motor_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    set_data.device_num = id;
    set_data.set_value  = value;

    switch (id) {
        case 0:
            push_rod_motor1_status = value;
            pub_topic(SYS_EVT_PUSH_ROD_MOTOR_1, &set_data);
            break;

        case 1:
            push_rod_motor2_status = value;
            pub_topic(SYS_EVT_PUSH_ROD_MOTOR_2, &set_data);
            break;

        default:
            break;
    }

    return value;
}

uint32_t push_rod_motor_status_get(uint8_t id) {
    uint32_t value = 0;

    switch (id) {
        case 0:
            value = push_rod_motor1_status;
            break;

        case 1:
            value = push_rod_motor2_status;
            break;

        default:
            break;
    }

    // log_d("prm get :%d, %d", id, value);
    return value;
}

uint32_t water_distribution_pump_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    water_pump_speed    = value;
    set_data.device_num = id;
    set_data.set_value  = value;
    pub_topic(SYS_EVT_WATER_DIST_PUMP, &set_data);
    return value;
}

uint32_t water_distribution_pump_speed_get(uint8_t id) {
    // log_d("wdp get :%d", water_pump_speed);
    return water_pump_speed;
}

uint32_t water_distribution_pump_status_get(uint8_t id) {
    // rt_kprintf(" filter pump get\r\n");
    return 1;
}

uint32_t water_valve_clean_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    set_data.device_num      = id;
    set_data.set_value       = value;
    water_valve_clean_status = value;
    pub_topic(SYS_EVT_WATER_VALVE_CLEAN, &set_data);
    return value;
}

uint32_t water_valve_clean_status_get(uint8_t id) {
    return water_valve_clean_status;
}

uint32_t sunction_motor_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    sunction_motor_speed = value;
    set_data.device_num  = id;
    set_data.set_value   = value;
    pub_topic(SYS_EVT_SUNCTION_MOTOR, &set_data);
    return value;
}

uint32_t sunction_motor_speed_get(uint8_t id) {
    log_d("sm get :%ld", sunction_motor_speed);
    return sunction_motor_speed;
}

uint32_t sunction_motor_status_get(uint8_t id) {
    return suntion_motor_error;
}

uint32_t roll_brush_motor_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    // log_d("set roll-%d speed %d", id, value);
    set_data.device_num    = id;
    set_data.set_value     = value;
    roll_brush_motor_speed = value;
    // roll_brush_motor_mq_send(&set_data, sizeof(clean_module_ctrl_st));
    pub_topic(SYS_EVT_ROLL_BRUSH_MOTOR, &set_data);
    return value;
}

uint32_t roll_brush_motor_report_speed_set(uint32_t speed) {
    roll_brush_motor_speed = speed;
    return speed;
}
uint32_t roll_brush_motor_speed_get(uint8_t id) {
    // log_d("rbm get :%ld", roll_brush_motor_speed);
    return roll_brush_motor_speed;
}

uint32_t roll_brush_motor_status_get(uint8_t id) {
    return 1;
}

uint32_t fan_motor_speed_set_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    log_d("set fan-%d speed %ld", id, value);
    set_data.device_num = id;
    set_data.set_value  = value;
    pub_topic(SYS_EVT_FAN_MOTOR_SPEED_SET, &set_data);
    return value;
}

uint32_t fan_motor_speed_fbk_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    // log_d("set fan-%d speed %d", id, value);
    set_data.device_num = id;
    set_data.set_value  = value;
    // sunction_motor_speed = value;
    pub_topic(SYS_EVT_FAN_MOTOR_SPEED_FBK, &set_data);
    return value;
}

uint32_t fan_motor_error_code_ops(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    // log_d("set fan-%d speed %d", id, value);
    set_data.device_num = id;
    set_data.set_value  = value;
    suntion_motor_error = value;
    pub_topic(SYS_EVT_FAN_MOTOR_ERROR_CODE, &set_data);
    return value;
}

uint32_t app_update_file_recv_set(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    set_data.device_num = id;
    set_data.set_value  = value;
    pub_topic(SYS_EVT_UPDATE_FILE_RECV, &set_data);
    return value;
}

uint32_t app_update_enter_boot_set(uint8_t id, uint32_t value) {
    clean_module_ctrl_st set_data;
    set_data.device_num = id;
    set_data.set_value  = value;
    pub_topic(SYS_EVT_UPDATE_ENTER_BOOT, &set_data);
    return value;
}

/**
 * Test the function clock_cpu_delay
 *
 *
 * @param delay_ns delay ns
 *
 * @return none
 */
int clean_ops_test(uint32_t id, uint32_t value) {
    log_d("parameter->id:%d\tparameter->val:%d", id, value);
    log_d("test start");

    if (id == 0) {
        push_rod_motor_ops(0, value);
    } else if (id == 1) {
        push_rod_motor_ops(1, value);
    } else if (id == 2) {
        side_brush_motor_ops(0, value);
    } else if (id == 3) {
        roll_brush_motor_ops(0, value);
    } else if (id == 4) {
        water_distribution_pump_ops(0, value);
    } else if (id == 5) {
        water_valve_clean_ops(0, value);
    } else if (id == 6) {
        fan_motor_speed_set_ops(0, value);
    } else if (id == 7) {
        push_rod_motor_ops(0, 1);
        osDelay(100);
        push_rod_motor_ops(1, 1);
        osDelay(100);
        side_brush_motor_ops(0, 50);
        osDelay(100);
        roll_brush_motor_ops(0, 60);
        osDelay(100);
        water_distribution_pump_ops(0, 50);
        osDelay(100);
        water_valve_clean_ops(0, 1);
        osDelay(100);
        fan_motor_speed_set_ops(0, 20);
    } else if (id == 8) {
        clean_ctrl_all_reset();
    }

    return 0;
}
// clean_ops_test 1
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), clean_ops_test, clean_ops_test, clean_ops_test);

#endif