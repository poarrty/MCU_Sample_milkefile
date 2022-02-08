/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: test.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: test operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include <board.h>

#define LOG_TAG "fal_test"
#include "elog.h"
#define TEST_THREAD_NAME              "test"
#define TEST_THREAD_STACK_SIZE        512
#define TEST_THREAD_PRIO              20
#define TEST_THREAD_HOLD_RUNNING_TIME 10

uint8_t  id[10]          = {0};
uint32_t ops_value[10]   = {0};
uint32_t fan_motor_speed = 0;  // rpm
#ifdef USED_DEVICE_OUTPUT_TEST
static uint8_t fal_app_running = RT_TRUE;
#else
static uint8_t fal_app_running = RT_FALSE;
#endif
clean_module_ctrl_st fan_motor_fbk        = {0};
clean_module_ctrl_st fan_motor_error_code = {0};
void                 test_thread_entry(void *param) {
    uint8_t status = 0;
    // uint32_t fan_motor_speed_fbk_sub = 0;
    // uint32_t fan_motor_error_code_sub = 0;

    for (uint8_t i = 0; i < sizeof(ops_value) / sizeof(ops_value[0]); i++) {
        ops_value[i] = 1;
    }

    ops_value[0] = 100;
    ops_value[1] = 15000;
    ops_value[3] = 3000;
    ops_value[4] = 100;
    osDelay(5000);
    log_w("enter test");
#if 0

	for (uint8_t i = ID_SIDE_BRUSH_MOTOR_LEFT_CW; i <= ID_EEPROM_RW; i++)
	{
		gpio_out_on(i);
	}

#endif
    side_brush_motor_ops(id[0], ops_value[0]);
    osDelay(100);
    sunction_motor_ops(id[1], ops_value[1]);
    osDelay(100);
    roll_brush_motor_ops(id[3], ops_value[3]);
    osDelay(100);
    water_distribution_pump_ops(id[4], ops_value[4]);

    uint32_t sub_evt = osFlagsError;

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_testHandle, SYS_EVT_FAN_MOTOR_SPEED_FBK | SYS_EVT_FAN_MOTOR_ERROR_CODE, osFlagsWaitAny, 0);

        while (fal_app_running) {
            if (sub_evt & SYS_EVT_FAN_MOTOR_SPEED_FBK) {
                if (xQueuePeek(topic_fan_motor_speed_fbkHandle, &fan_motor_fbk, 0) != errQUEUE_EMPTY) {
                    utils_truncate_number_int(&fan_motor_fbk.set_value, 0, 17000);
                    log_d("fan motor fbk :%d", fan_motor_fbk.set_value);
                }
            }

            if (sub_evt & SYS_EVT_FAN_MOTOR_ERROR_CODE) {
                if (xQueuePeek(topic_fan_motor_error_codeHandle, &fan_motor_error_code, 0) != errQUEUE_EMPTY) {
                    utils_truncate_number_int(&fan_motor_error_code.set_value, 0, 17000);
                    log_d("fan motor error code :%d", fan_motor_error_code.set_value);
                }
            }

            id[2]        = status / 2;
            ops_value[2] = status % 2;
            id[5]        = status / 2;
            ops_value[5] = status % 2;

            if (++status >= 4) {
                status = 0;
            }

            push_rod_motor_ops(id[2], ops_value[2]);
            side_brush_motor_cw_ops(id[5], ops_value[5]);

            //        if (ops_value[0] == 100)
            //            ops_value[0] = 0;
            //        else
            //            ops_value[0] = 100;
            osDelay(100);
            side_brush_motor_ops(id[0], ops_value[0]);
            osDelay(100);
            //        if (ops_value[1] == 100)
            //            ops_value[1] = 0;
            //        else
            //            ops_value[1] = 100;
            sunction_motor_ops(id[1], ops_value[1]);
            osDelay(100);
            //        if (ops_value[3] == 1500)
            //            ops_value[3] = 0;
            //        else
            //            ops_value[3] = 1500;
            roll_brush_motor_ops(id[3], ops_value[3]);
            osDelay(100);
            //        if (ops_value[4] == 100)
            //            ops_value[4] = 0;
            //        else
            //            ops_value[4] = 100;
            water_distribution_pump_ops(id[4], ops_value[4]);

            fan_motor_speed_set_ops(id[3], fan_motor_speed);

            osDelay(5000);
        }

        osDelay(10);
    }
}

void fault_test_by_div0(void) {
    int x, y, z;
    x = 10;
    y = 0;
    z = x / y;
    printf("fault_test_by_div0 z:%d\n", z);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_div0, fault_test_by_div0, cm backtrace teset);