/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS充电和电池
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "pal_du.h"
#include "fal.h"
#include "devices.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "pal_uros.h"
#include "log.h"
#include "mem_pool.h"
#include "sensor_msgs/msg/range.h"

/**
 * @addtogroup Robot_PAL 协议适配层 - PAL
 * @{
 */

/**
 * @defgroup Robot_PAL_UROS microROS接口处理
 *
 * @brief 
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define USOUND_RECV_BUFF_SIZE   256
/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
static uint8_t usound_recv_buff[USOUND_RECV_BUFF_SIZE];
static publisher g_uros_usound[8];
static sensor_msgs__msg__Range uros_usound[8];
/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
    const osThreadAttr_t du_pub_thread_attributes = {
    .name = "du_pub_thread",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 256*4
};
/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/



/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/


/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/


static void fal_usound_data_public(uint16_t data,
                                   sensor_msgs__msg__Range msg_uros_usound,
                                   publisher pub_uros_usound) {
    msg_uros_usound.radiation_type = 3;
    msg_uros_usound.min_range = 150.0;
    msg_uros_usound.max_range = 1400.0;
    msg_uros_usound.range = (float) data;
    pal_uros_msg_set_timestamp(&msg_uros_usound.header.stamp);
    message_publish(&pub_uros_usound);

    return;
}

void du_data_public(void *argument)
{
    while(1) 
    {

        osDelay(100);
    }
    

}

int pal_du_init(void) {
	int num = 0;
    char *usound_name[8] = {"/usound_1", "/usound_2", "/usound_3", "/usound_4",
                            "/usound_5", "/usound_6", "/usound_7", "/usound_8"};

    for (num = 0; num < 8; num++) {
        publisher_init(&g_uros_usound[num],
                       ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
                       usound_name[num], &uros_usound[num], BEST, OVERWRITE,
                       sizeof(sensor_msgs__msg__Range));
    }
    osThreadNew(du_data_public, NULL, &du_pub_thread_attributes);
    return 0;
}
PAL_MODULE_INIT(pal_du_init);

int pal_du_deInit(void) {
    return 0;
}



#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
