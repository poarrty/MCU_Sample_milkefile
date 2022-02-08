/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_can.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: can operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#include "drv_can.h"
#include <board.h>

#define CAN_OBJ_FLAG_USED    (1ul << 1)
#define CAN_OBJ_FLAG_NO_USED (1ul << 0)

typedef struct {
    rt_device_t                  dev_handle;
    char                         dev_name[RT_NAME_MAX];
    struct can_configure *       config;
    struct rt_can_filter_config *filter;
    uint8_t                      used_flag;
    uint16_t                     open_flag;
} can_obj_st;

int can_read_data(uint8_t can_obj_id, struct rt_can_msg *msg);
int can_send_data(uint8_t can_obj_id, struct rt_can_msg *msg);
#endif
