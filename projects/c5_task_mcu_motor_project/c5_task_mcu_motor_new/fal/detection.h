/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: detection.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: detection operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __DETECTION_H
#define __DETECTION_H

#include "board.h"

enum {
    ID_SUNCTION_MOTOR_ERR_STA = 0,
    ID_ROLL_MOTOR_ERR_STA,
    ID_WATER_DIST_ERR_STA,
    ID_WATER_VALVE_CLEAN_ERR_STA,
    ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA,
    ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA,
    ID_PUSH_ROD_MOTOR_1_ERR_STA,
    ID_PUSH_ROD_MOTOR_2_ERR_STA,
    ID_CLEAN_DEVICE_MAX_ERR_STA
};

enum { ID_PUSH_ROD_MOTOR_1_DET = 0, ID_PUSH_ROD_MOTOR_2_DET, ID_WATER_DISTRIBUTION_DET, ID_DET_DEFAULT_MAX };

enum { DET_DISABLE = 0, DET_ENABLE, DET_DEFAULT };

uint32_t clean_device_error_status_get(uint8_t clean_dev_id);

#endif
