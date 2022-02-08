/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: sunction_motor.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: sunction motor operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "board.h"

#define LOG_TAG "sunction_motor"
#include "elog.h"
#ifdef USED_SUNCTION_MOTOR_CTRL_CAN

#define DEBUG_PRINTF_DATA         (1 << 0)
#define DEBUG_PRINTF_INFO         (1 << 1)
#define SUNCTION_BRUSH_MOTOR_STEP 800
#define SUNCTION_MOTOR_MAX_DUTY   100
#define SUNCTION_MOTOR_MAX_SPEED  31500
#define CAN_SEND_CYCLE            10
#define STEP_DELAY                12

static uint32_t printf_mask = 0;

void sunciton_motor_can_rx_thread(void *parameter) {
    uint32_t size;
    // uint32_t sunction_motor_sub = 0;
    clean_module_ctrl_st sunction_motor_info      = {0};
    uint8_t              sunction_motor_run_flag  = 0;
    uint32_t             div                      = 0;
    uint32_t             send_count               = 0;
    uint32_t             ret_sunction_motor_speed = 0;
    uint32_t             sunction_motor_set_speed = 0;
    uint32_t             sunction_motor_cur_spped = 0;

    struct rt_can_msg rxmsg = {0};
    struct rt_can_msg msg   = {0};

    uint32_t sub_evt = osFlagsError;

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_sunction_motorHandle, SYS_EVT_SUNCTION_MOTOR, osFlagsWaitAny, 0);

        if (sub_evt > osFlagsError) {
            sub_evt = 0;
        }

        if (sub_evt & SYS_EVT_SUNCTION_MOTOR) {
            if (xQueuePeek(topic_sunction_motorHandle, &sunction_motor_info, 0) != errQUEUE_EMPTY) {
                utils_truncate_number_int(&sunction_motor_info.set_value, 0, SUNCTION_MOTOR_MAX_DUTY);
                sunction_motor_set_speed = sunction_motor_info.set_value * SUNCTION_MOTOR_MAX_SPEED / SUNCTION_MOTOR_MAX_DUTY;
                sunction_motor_run_flag  = TRUE;

                if ((sunction_motor_cur_spped == 0) && (sunction_motor_info.set_value > 0)) {
                    sunction_motor_cur_spped = 3000;
                    send_count               = 0;
                }

                log_d("sunction_ set :%d", sunction_motor_info.set_value);
            }
        }

        /* send frame data from CAN */
        if ((sunction_motor_run_flag == TRUE) && ((++div) % CAN_SEND_CYCLE == 0)) {
            div = 0;

            if (send_count++ >= STEP_DELAY) {
                send_count = STEP_DELAY;
                utils_step_towards_uint(&sunction_motor_cur_spped, sunction_motor_set_speed, SUNCTION_BRUSH_MOTOR_STEP);
            }

            msg.id  = 0x00000302;
            msg.ide = RT_CAN_EXTID; /* standard format */
            msg.rtr = RT_CAN_DTR;   /* data format */
            msg.len = 4;            /* length */

            for (uint8_t i = 0; i < 4; i++) {
                msg.data[i] = ((sunction_motor_cur_spped >> 8 * (3 - i)) & 0xff);
            }

            size = can_send_data(0, &msg);

            if (size == 0) {
                log_d("write data failed!\n");
            }

            if ((sunction_motor_cur_spped == 0) && (sunction_motor_set_speed == 0)) {
                sunction_motor_run_flag = RT_FALSE;
            }
        }

        /* read frame data from CAN */
        if (can_read_data(0, &rxmsg) > 0) {
            log_d("SUNCTION");

            /* print ID and content */
            if (printf_mask & DEBUG_PRINTF_DATA) {
                log_d("ID:%x", rxmsg.id);

                for (uint8_t i = 0; i < 8; i++) {
                    log_d("%02x ", rxmsg.data[i]);
                }

                log_d("\n");
            }

            uint8_t cmd = ((rxmsg.id >> 8) & 0xff);

            switch (cmd) {
                case 9:
                    ret_sunction_motor_speed = 0;

                    if ((rxmsg.id & 0xff) == 0x02) {
                        for (uint8_t i = 0; i < 4; i++) {
                            ret_sunction_motor_speed |= (rxmsg.data[i] << 8 * (3 - i));
                        }

                        sunction_motor_info.cur_value  = ret_sunction_motor_speed;
                        sunction_motor_info.current    = (rxmsg.data[4] << 8) + rxmsg.data[5] / 10.0;
                        sunction_motor_info.duty_cycle = (rxmsg.data[6] << 8) + rxmsg.data[7] / 1000.0;
                        // sunction_motor_report_speed_set(sunction_motor_info.set_value);
                    }

                    break;

                default:
                    break;
            }
        }

        osDelay(1);
    }
}

#ifdef RT_USING_FINSH
void sunction_debug(int argc, char **argv) {
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
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sunction_debug, sunction_debug, sunction__debug 0 1);
#endif
#endif