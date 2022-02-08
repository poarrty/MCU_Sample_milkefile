/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_can.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: can operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_can.h"
#include "can.h"
#define LOG_TAG "bsp_can"
#include "elog.h"
#define USED_BSP_CAN

#ifdef USED_BSP_CAN

extern CAN_HandleTypeDef hcan;

int can_read_data(uint8_t can_obj_id, struct rt_can_msg *msg) {
    return _can_int_rx(&drv_can1.device, msg, sizeof(struct rt_can_msg));
}

int can_send_data(uint8_t can_obj_id, struct rt_can_msg *msg) {
    return _can_ops.sendmsg(&drv_can1.device, msg, sizeof(struct rt_can_msg));
}

#endif