/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: sys_pubsub.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: system publish and subscribe ligic
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __SYS_EVENT_H
#define __SYS_EVENT_H

#include <board.h>
#include <queue.h>

#define SYS_EVT_H 0x00000001U

/* System event set */
typedef enum {
    SYS_EVT_SUNCTION_MOTOR       = SYS_EVT_H << 0,
    SYS_EVT_ROLL_BRUSH_MOTOR     = SYS_EVT_H << 1,
    SYS_EVT_WATER_DIST_PUMP      = SYS_EVT_H << 2,
    SYS_EVT_WATER_VALVE_CLEAN    = SYS_EVT_H << 3,
    SYS_EVT_SIDE_BRUSH_MOTOR     = SYS_EVT_H << 4,
    SYS_EVT_PUSH_ROD_MOTOR_1     = SYS_EVT_H << 5,
    SYS_EVT_PUSH_ROD_MOTOR_2     = SYS_EVT_H << 6,
    SYS_EVT_FAN_MOTOR_SPEED_SET  = SYS_EVT_H << 7,
    SYS_EVT_FAN_MOTOR_SPEED_FBK  = SYS_EVT_H << 8,
    SYS_EVT_FAN_MOTOR_ERROR_CODE = SYS_EVT_H << 9,
    SYS_EVT_UPDATE_FILE_RECV     = SYS_EVT_H << 10,
    SYS_EVT_UPDATE_ENTER_BOOT    = SYS_EVT_H << 11,
    SYS_EVT_ALL                  = SYS_EVT_SUNCTION_MOTOR | SYS_EVT_ROLL_BRUSH_MOTOR | SYS_EVT_WATER_DIST_PUMP | SYS_EVT_WATER_VALVE_CLEAN |
                  SYS_EVT_SIDE_BRUSH_MOTOR | SYS_EVT_PUSH_ROD_MOTOR_1 | SYS_EVT_PUSH_ROD_MOTOR_2 | SYS_EVT_FAN_MOTOR_SPEED_SET |
                  SYS_EVT_FAN_MOTOR_SPEED_FBK | SYS_EVT_FAN_MOTOR_ERROR_CODE | SYS_EVT_UPDATE_FILE_RECV | SYS_EVT_UPDATE_ENTER_BOOT
} SYS_EVENT_E;
///< publish
extern osEventFlagsId_t sys_pubHandle;
///< subscribe
extern osEventFlagsId_t sys_suber_sunction_motorHandle;
extern osEventFlagsId_t sys_suber_fan_motorHandle;
extern osEventFlagsId_t sys_suber_detectionHandle;
extern osEventFlagsId_t sys_suber_clean_ctrlHandle;
extern osEventFlagsId_t sys_suber_roll_motorHandle;
extern osEventFlagsId_t sys_suber_testHandle;
extern osEventFlagsId_t sys_suber_modbus_otaHandle;

///< topic
extern osMessageQueueId_t topic_sunction_motorHandle;
extern osMessageQueueId_t topic_roll_brush_motorHandle;
extern osMessageQueueId_t topic_water_dist_pumpHandle;
extern osMessageQueueId_t topic_water_valve_cleanHandle;
extern osMessageQueueId_t topic_side_brush_motorHandle;
extern osMessageQueueId_t topic_push_rod_motor_1Handle;
extern osMessageQueueId_t topic_push_rod_motor_2Handle;
extern osMessageQueueId_t topic_fan_motor_speed_setHandle;
extern osMessageQueueId_t topic_fan_motor_speed_fbkHandle;
extern osMessageQueueId_t topic_fan_motor_error_codeHandle;
extern osMessageQueueId_t topic_update_file_recvHandle;
extern osMessageQueueId_t topic_update_enter_bootHandle;

void pub_topic(SYS_EVENT_E evt, void *data);

#endif
