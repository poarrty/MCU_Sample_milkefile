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
#include "pal_cliff.h"
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
#define CLIFF_MAX   6
/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
    const osThreadAttr_t cliff_pub_thread_attributes = {
    .name = "cliff_pub_thread",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 256*4
};
static publisher g_uros_cliff;
static sensor_msgs__msg__Range uros_cliff;
/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int cliff_oml_handle1;
extern int cliff_oml_handle2;
extern int cliff_oml_handle3;
extern int cliff_oml_handle4;
extern int cliff_oml_handle5;
extern int cliff_oml_handle6;


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
void cliff_data_public(void *argument)
{
    while(1) 
    {
        uint8_t buffer[CLIFF_MAX];
        device_read(cliff_oml_handle1,&buffer[0],sizeof(uint8_t));
        device_read(cliff_oml_handle2,&buffer[1],sizeof(uint8_t));
        device_read(cliff_oml_handle3,&buffer[2],sizeof(uint8_t));
        device_read(cliff_oml_handle4,&buffer[3],sizeof(uint8_t));
        device_read(cliff_oml_handle5,&buffer[4],sizeof(uint8_t));
        device_read(cliff_oml_handle6,&buffer[5],sizeof(uint8_t));
        for (int i = 0; i < CLIFF_MAX; i++)
        {
            if(buffer[i]==1)
            {
                uros_cliff.range = 1;
                pal_uros_msg_set_timestamp(&uros_cliff.header.stamp);
                message_publish(&g_uros_cliff);
            }
        }
        osDelay(100);
    }
    

}


int pal_cliff_init(void) {
	publisher_init(&g_uros_cliff,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
                   "/cliff", &uros_cliff, BEST, OVERWRITE,
                   sizeof(sensor_msgs__msg__Range));


    osThreadNew(cliff_data_public, NULL, &cliff_pub_thread_attributes);
    return 0;
}
PAL_MODULE_INIT(pal_cliff_init);

int pal_cliff_deInit(void) {
    return 0;
}



#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
