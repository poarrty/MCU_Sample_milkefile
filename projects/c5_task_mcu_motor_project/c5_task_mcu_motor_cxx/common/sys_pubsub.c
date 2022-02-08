/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: sys_pubsub.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: system publish and subscribe ligic
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "sys_pubsub.h"

#define LOG_TAG "sys_pubsub"
#include "elog.h"
#define IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#define IS_IRQ_MODE()   (__get_IPSR() != 0U)
#define IS_IRQ()        (IS_IRQ_MODE() || (IS_IRQ_MASKED() && (osKernelGetState() == osKernelRunning)))

void task_fal_pubsub_run(void *argument) {
    uint32_t pub_evt;

    for (;;) {
        ///< get system publish event
        pub_evt = osEventFlagsWait(sys_pubHandle, SYS_EVT_ALL, osFlagsWaitAny, osWaitForever);

        ///< Distribute topics to subscribers
        if (pub_evt & SYS_EVT_SUNCTION_MOTOR) {
            osEventFlagsSet(sys_suber_sunction_motorHandle, SYS_EVT_SUNCTION_MOTOR);
            osEventFlagsSet(sys_suber_fan_motorHandle, SYS_EVT_SUNCTION_MOTOR);
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_SUNCTION_MOTOR);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_SUNCTION_MOTOR);
        }

        if (pub_evt & SYS_EVT_ROLL_BRUSH_MOTOR) {
            osEventFlagsSet(sys_suber_roll_motorHandle, SYS_EVT_ROLL_BRUSH_MOTOR);
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_ROLL_BRUSH_MOTOR);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_ROLL_BRUSH_MOTOR);
        }

        if (pub_evt & SYS_EVT_WATER_DIST_PUMP) {
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_WATER_DIST_PUMP);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_WATER_DIST_PUMP);
        }

        if (pub_evt & SYS_EVT_WATER_VALVE_CLEAN) {
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_WATER_VALVE_CLEAN);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_WATER_VALVE_CLEAN);
        }

        if (pub_evt & SYS_EVT_SIDE_BRUSH_MOTOR) {
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_SIDE_BRUSH_MOTOR);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_SIDE_BRUSH_MOTOR);
        }

        if (pub_evt & SYS_EVT_PUSH_ROD_MOTOR_1) {
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_PUSH_ROD_MOTOR_1);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_PUSH_ROD_MOTOR_1);
        }

        if (pub_evt & SYS_EVT_PUSH_ROD_MOTOR_2) {
            osEventFlagsSet(sys_suber_detectionHandle, SYS_EVT_PUSH_ROD_MOTOR_2);
            osEventFlagsSet(sys_suber_clean_ctrlHandle, SYS_EVT_PUSH_ROD_MOTOR_2);
        }

        if (pub_evt & SYS_EVT_FAN_MOTOR_SPEED_SET) {
            osEventFlagsSet(sys_suber_fan_motorHandle, SYS_EVT_FAN_MOTOR_SPEED_SET);
        }

        if (pub_evt & SYS_EVT_FAN_MOTOR_SPEED_FBK) {
            osEventFlagsSet(sys_suber_testHandle, SYS_EVT_FAN_MOTOR_SPEED_FBK);
        }

        if (pub_evt & SYS_EVT_FAN_MOTOR_ERROR_CODE) {
            osEventFlagsSet(sys_suber_testHandle, SYS_EVT_FAN_MOTOR_ERROR_CODE);
        }

        if (pub_evt & SYS_EVT_UPDATE_FILE_RECV) {
            osEventFlagsSet(sys_suber_modbus_otaHandle, SYS_EVT_UPDATE_FILE_RECV);
        }

        if (pub_evt & SYS_EVT_UPDATE_ENTER_BOOT) {
            osEventFlagsSet(sys_suber_modbus_otaHandle, SYS_EVT_UPDATE_ENTER_BOOT);
        }
    }
}

void pub_topic(SYS_EVENT_E evt, void *data) {
    osMessageQueueId_t topic_queue = NULL;

    BaseType_t yield;

    switch (evt) {
        case SYS_EVT_SUNCTION_MOTOR:
            topic_queue = topic_sunction_motorHandle;
            break;

        case SYS_EVT_ROLL_BRUSH_MOTOR:
            topic_queue = topic_roll_brush_motorHandle;
            break;

        case SYS_EVT_WATER_DIST_PUMP:
            topic_queue = topic_water_dist_pumpHandle;
            break;

        case SYS_EVT_WATER_VALVE_CLEAN:
            topic_queue = topic_water_valve_cleanHandle;
            break;

        case SYS_EVT_SIDE_BRUSH_MOTOR:
            topic_queue = topic_side_brush_motorHandle;
            break;

        case SYS_EVT_PUSH_ROD_MOTOR_1:
            topic_queue = topic_push_rod_motor_1Handle;
            break;

        case SYS_EVT_PUSH_ROD_MOTOR_2:
            topic_queue = topic_push_rod_motor_2Handle;
            break;

        case SYS_EVT_FAN_MOTOR_SPEED_SET:
            topic_queue = topic_fan_motor_speed_setHandle;
            break;

        case SYS_EVT_FAN_MOTOR_SPEED_FBK:
            topic_queue = topic_fan_motor_speed_fbkHandle;
            break;

        case SYS_EVT_FAN_MOTOR_ERROR_CODE:
            topic_queue = topic_fan_motor_error_codeHandle;
            break;

        case SYS_EVT_UPDATE_FILE_RECV:
            topic_queue = topic_update_file_recvHandle;
            break;

        case SYS_EVT_UPDATE_ENTER_BOOT:
            topic_queue = topic_update_enter_bootHandle;
            break;

        default:
            break;
    }

    if (topic_queue != NULL) {
        if (IS_IRQ()) {
            xQueueOverwriteFromISR((QueueHandle_t) topic_queue, data, &yield);
        } else {
            xQueueOverwrite((QueueHandle_t) topic_queue, data);
        }

        osEventFlagsSet(sys_pubHandle, evt);
    } else {
        log_e("want to public a null ipc topic!");
    }
}

/*
void pub_test(LAMP_E lamp, LED_COLOR_E color)
{
    LED_CTRL_T led_ctrl;

    led_ctrl.lamp = lamp;
    led_ctrl.color = color;

    xQueueOverwrite(topic_led, &led_ctrl);

    osEventFlagsSet(sys_pub, SYS_EVT_LED_CONTROL);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pubTest, pub_test, pubsub test);
*/