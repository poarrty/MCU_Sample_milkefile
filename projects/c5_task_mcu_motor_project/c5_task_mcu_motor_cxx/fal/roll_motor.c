/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: roll_motor.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: roll motor operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "board.h"

#define LOG_TAG "roll_motor"
#include "elog.h"
#ifdef USED_ROLL_BRUSH_MOTOR_CTRL_CAN

#define DEBUG_PRINTF_DATA     (1 << 0)
#define DEBUG_PRINTF_INFO     (1 << 1)
#define ROLL_BRUSH_MOTOR_STEP 35
#define CAN_SEND_CYCLE        10
#define STEP_DELAY            1
#define ROLL_MOTOR_POLES      7
#define ROLL_MOTOR_MAX_SPEED  3000

extern CAN_HandleTypeDef hcan;

static uint32_t printf_mask = 0;
uint32_t        debug_count = 0;

void can_rx_thread(void *parameter) {
    uint32_t size;
    // uint32_t roll_brush_motor_sub = 0;
    clean_module_ctrl_st roll_brush_motor_info = {0};
    uint8_t              roll_brush_run_flag   = 0;
    uint32_t             div                   = 0;
    uint32_t             send_count            = 0;
    uint32_t             ret_roll_speed        = 0;
    uint32_t             roll_cur_spped        = 0;
    uint32_t             roll_set_speed        = 0;
    struct rt_can_msg    rxmsg                 = {0};
    struct rt_can_msg    msg                   = {0};

    uint32_t thread_start_time = 0;
    uint32_t thread_utime      = 0;
    uint32_t running_time_div  = 0;

    uint32_t sub_evt = osFlagsError;

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_roll_motorHandle, SYS_EVT_ROLL_BRUSH_MOTOR, osFlagsWaitAny, 0);

        if (sub_evt > osFlagsError) {
            sub_evt = 0;
        }

        thread_start_time = clock_cpu_gettime();
#if 0

		if (roll_brush_motor_mq_recv(&roll_brush_motor_info, sizeof(clean_module_ctrl_st), 0) == RT_EOK)
		{
			utils_truncate_number(&roll_brush_motor_info.set_value, 0, 1500);
			roll_set_speed = roll_brush_motor_info.set_value * 7;
			roll_brush_run_flag = 1;
		}

#endif

        if (sub_evt & SYS_EVT_ROLL_BRUSH_MOTOR) {
            if (xQueuePeek(topic_roll_brush_motorHandle, &roll_brush_motor_info, 0) != errQUEUE_EMPTY) {
                utils_truncate_number_int(&roll_brush_motor_info.set_value, 0, ROLL_MOTOR_MAX_SPEED);
                roll_set_speed      = roll_brush_motor_info.set_value * ROLL_MOTOR_POLES;
                roll_brush_run_flag = TRUE;
                send_count          = 0;
                log_d("roll set :%d", roll_brush_motor_info.set_value);
                // log_d("TTTTTTTTTTTTTTTTTTTTTT");
                // log_d("MCR:%x", hcan.Instance->MCR);
                // log_d("MSR:%x", hcan.Instance->MSR);
                // log_d("TSR:%x", hcan.Instance->TSR);
                // log_d("RF0R:%x", hcan.Instance->RF0R);
                // log_d("RF1R:%x", hcan.Instance->RF1R);
                // log_d("IER:%x", hcan.Instance->IER);
                // log_d("ESR:%x", hcan.Instance->ESR);
                // log_d("BTR:%x", hcan.Instance->BTR);
            }
        }

        /* send the frame data of can*/
        if ((roll_brush_run_flag == TRUE) && ((++div) % CAN_SEND_CYCLE == 0)) {
            log_d("ROLL");
            div = 0;

            if (send_count++ >= STEP_DELAY) {
                send_count = STEP_DELAY;
                utils_step_towards_uint(&roll_cur_spped, roll_set_speed, ROLL_BRUSH_MOTOR_STEP);
            }

            msg.id  = 0x00000301;
            msg.ide = RT_CAN_EXTID; /* standard format */
            msg.rtr = RT_CAN_DTR;   /* data frame */
            msg.len = 4;            /* length */

            for (uint8_t i = 0; i < 4; i++) {
                msg.data[i] = ((roll_cur_spped >> 8 * (3 - i)) & 0xff);
            }

            size = can_send_data(0, &msg);

            if (size == 0) {
                log_d("write data failed!\n");
            }

            if ((roll_cur_spped == 0) && (roll_set_speed == 0)) {
                roll_brush_run_flag = RT_FALSE;
            }
        }

        /* read frame data from CAN */
        if (can_read_data(0, &rxmsg) > 0) {
            /* print ID and content */
            if (printf_mask & DEBUG_PRINTF_DATA) {
                log_d("ID:%x", rxmsg.id);

                for (uint8_t i = 0; i < 8; i++) {
                    log_d("%02x ", rxmsg.data[i]);
                }

                log_d("\n");
            }

            uint8_t cmd = ((rxmsg.id >> 8) & 0xff);
            log_d("cmd:%x", cmd);

            switch (cmd) {
                case 9:
                    ret_roll_speed = 0;

                    for (uint8_t i = 0; i < 4; i++) {
                        ret_roll_speed |= (rxmsg.data[i] << 8 * (3 - i));
                    }

                    roll_brush_motor_info.cur_value  = ret_roll_speed;
                    roll_brush_motor_info.current    = (rxmsg.data[4] << 8) + rxmsg.data[5] / 10.0;
                    roll_brush_motor_info.duty_cycle = (rxmsg.data[6] << 8) + rxmsg.data[7] / 1000.0;
                    roll_brush_motor_report_speed_set(roll_brush_motor_info.set_value);
                    break;

                default:
                    break;
            }
        }

        debug_count++;

        if (++running_time_div >= 10000) {
            running_time_div = 0;
            thread_utime     = clock_cpu_microsecond_diff(thread_start_time, clock_cpu_gettime());
            log_d("%s utime: %ld\r\n", pcTaskGetName(NULL), thread_utime);
        }

        osDelay(1);
    }
}

#ifdef RT_USING_FINSH
void roll_debug(int argc, char **argv) {
    uint32_t status = 0;

    if (argc < 3) {
        log_w("%s input parm error,argv[1]:id, argv[2]: status\r\n", __FUNCTION__);
        return;
    }

    status = atoi(argv[2]);

    if (status) {
        printf_mask |= (1 << atoi(argv[1]));
    } else {
        printf_mask &= ~(1 << atoi(argv[1]));
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), roll_debug, roll_debug, roll_debug 0 1);
#endif
#endif
