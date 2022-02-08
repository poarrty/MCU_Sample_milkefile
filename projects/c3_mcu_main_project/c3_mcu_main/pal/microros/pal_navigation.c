/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS导航组件
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "pal_navigation.h"
#include "fal.h"
#include "devices.h"
#include "define_motor.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "pal_uros.h"
#include "nav_msgs/msg/odometry.h"
#include "geometry_msgs/msg/twist.h"
#include "cvte_sensor_msgs/msg/encoding.h"
#include "std_msgs/msg/string.h"
#include "cJSON.h"
#include "log.h"
#include "mem_pool.h"
#include "SEGGER_RTT.h"




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

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/


/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
extern int driver_handle;				//驱动器控制句柄
extern int wheel_handle;
static publisher g_uros_odom;
static nav_msgs__msg__Odometry uros_odom = {0};

static publisher g_uros_encoding;
static cvte_sensor_msgs__msg__Encoding uros_encoding;

static int64_t encoding_buffer[2];
// static char odom_buffer1[10];
// static char odom_buffer2[10];

///< motor speed ctrl
static subscrption g_motor_ctrl;
static geometry_msgs__msg__Twist motor_ctrl = {0};

///< motor driver ctrl
static subscrption g_motor_driver_ctrl;
static std_msgs__msg__String motor_driver_ctrl = {0};
static char motor_driver_ctrl_buffer[128];

///< motor driver state
static publisher g_motor_driver_state;
static std_msgs__msg__String motor_driver_state = {0};
static char motor_driver_state_buffer[128];


/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int driver_handle;	

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
void navi_motor_data_callback(const MOTOR_DATA *data, uint32_t size)
{
	if((NULL == data) || (size < 2*sizeof(MOTOR_DATA)))
	{
		return;
	}

	uros_encoding.encoding_num.data[1] = data[0].encoding.encoding_num;
	uros_encoding.encoding_circle = data[0].encoding.encoding_circle;

	uros_encoding.encoding_num.data[0] = -1 * data[1].encoding.encoding_num;
	uros_encoding.encoding_circle = data[1].encoding.encoding_circle;

	pal_uros_msg_set_timestamp(&uros_encoding.header.stamp);
	message_publish(&g_uros_encoding);
}

void navigation_set_speed_callback(const void *msgin) {
    static double xLast = 0, zLast = 0;
    const geometry_msgs__msg__Twist *msg =
        (const geometry_msgs__msg__Twist *) msgin;
		MOTOR_SPEED speed;

        LOG_DEBUG("/cmd_vel: v[%d] w[%d]\r\n", (int16_t)msg->linear.x, (int16_t)msg->angular.z); 

    if (msg->linear.x != xLast || msg->angular.z != zLast) {
        xLast = msg->linear.x;
        zLast = msg->angular.z;
    //    LOG_DEBUG("/cmd_vel: v[%.2f] w[%.2f]", msg->linear.x, msg->angular.z);
    }

		speed.speed_v_t = (int16_t)(msg->linear.x * 1000);
		speed.speed_w_t = (int16_t)(msg->angular.z * 1000);
		device_ioctl(wheel_handle, MOTOR_CMD_SET_SPEED, (void *)&speed);

}

void motor_set_status_callback(const void *msgin) {
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *) msgin;

    cJSON *cmds = NULL;
    cJSON *cmd = NULL;

    if (msg->data.data == NULL) {
        return;
    }

    cmds = cJSON_Parse(msg->data.data);

    if (cmds == NULL) {
        return;
    }

    ///< 电机使能控制指令
    cmd = cJSON_GetObjectItemCaseSensitive(cmds, "motor_enable");
    if (cJSON_IsBool(cmd)) {
        if (cJSON_IsTrue(cmd)) {
					device_ioctl(driver_handle, MOTOR_CMD_ENABLE, NULL);
        } else {
					device_ioctl(driver_handle, MOTOR_CMD_DISABLE, NULL);
        }
    }

    cJSON_Delete(cmds);
}


int pal_navigation_init(void) {
	publisher_init(
        &g_uros_odom, ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        "/odom", &uros_odom, BEST, OVERWRITE, sizeof(nav_msgs__msg__Odometry));

    publisher_init(&g_uros_encoding,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(cvte_sensor_msgs, msg, Encoding),
                   "/encoding", &uros_encoding, BEST, OVERWRITE,
                   sizeof(cvte_sensor_msgs__msg__Encoding));

    publisher_init(&g_motor_driver_state,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
                   "/driver/state", &motor_driver_state, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__String));

    subscrption_init(
        &g_motor_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "/vel_mcu", &motor_ctrl, BEST, navigation_set_speed_callback);

    subscrption_init(&g_motor_driver_ctrl,
                     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
                     "/motor_driver/set", &motor_driver_ctrl, BEST,
                     motor_set_status_callback);

    motor_driver_state.data.data = motor_driver_state_buffer;
    motor_driver_state.data.size = 0;
    motor_driver_state.data.capacity = sizeof(motor_driver_state_buffer);

    motor_driver_ctrl.data.data = motor_driver_ctrl_buffer;
    motor_driver_ctrl.data.size = 0;
    motor_driver_ctrl.data.capacity = sizeof(motor_driver_ctrl_buffer);

    uros_encoding.encoding_num.data = encoding_buffer;
    uros_encoding.encoding_num.capacity = 0;
    uros_encoding.encoding_num.size =
        sizeof(encoding_buffer) / sizeof(encoding_buffer[0]);

    MOTOR_DATA_CB_ARG *cb = (MOTOR_DATA_CB_ARG *)mem_block_alloc(sizeof(MOTOR_DATA_CB_ARG));;
	cb->fn_callback = navi_motor_data_callback;
    cb->period = 18;
	device_ioctl(wheel_handle, MOTOR_CMD_SET_DATA_CB , (void *)cb);		

    return 0;
}
PAL_MODULE_INIT(pal_navigation_init);

int pal_navigation_deInit(void) {
    return 0;
}


#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
