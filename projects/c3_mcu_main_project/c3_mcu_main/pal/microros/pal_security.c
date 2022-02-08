/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS安全管理
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "pal_security.h"
#include "fal.h"
#include "devices.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "define_button.h"
#include "std_msgs/msg/bool.h"
#include "pal_uros.h"
#include "log.h"



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
typedef struct Button_Security_Ros_Attr
{
    publisher g_pub_uros_button;                  //发布
	subscrption g_sub_uros_button;                //订阅
	void *pub_msg_data;                           //发布数据地址
	void *sub_msg_data;                           //订阅数据地址
    char *button_pub_name;                        //发布者名称
	char *button_sub_name;                        //订阅者名称
	const rosidl_message_type_support_t *stMsg;   //消息数据结构
	BUTTON_PUB_DATA_TYPE_E pub_data_type;         //消息类型
    uint8_t index;                                //button句柄
    
}BUTTON_SECURITY_ROS_ATTR_T, *BUTTON_SECURITY_ROS_ATTR_P;           //microros属性信息

/*****************************************************************
 * 全局变量定义
 ******************************************************************/


/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static std_msgs__msg__Bool uros_emerg_sub;
static std_msgs__msg__Bool uros_emerg_pub;

static std_msgs__msg__Bool uros_crash_left_sub;
static std_msgs__msg__Bool uros_crash_left_pub;

static std_msgs__msg__Bool uros_crash_right_sub;
static std_msgs__msg__Bool uros_crash_right_pub;

struct bus_info bus_gpio_emerg = {
			.bus_name = "gpioE",
			.bus_addr = 2,
};
struct bus_info bus_gpio_crash_left = {
			.bus_name = "gpioE",
			.bus_addr = 3,
};
struct bus_info bus_gpio_crash_right = {
			.bus_name = "gpioE",
			.bus_addr = 4,
};
static BUTTON_SECURITY_ROS_ATTR_T button_ros_attr[SECURITY_BUTTON_MAX];
static BUTTON_ATTR_T button_info[SECURITY_BUTTON_MAX]; 
static BUTTON_ATTACH_ATTR_T button_attr[SECURITY_BUTTON_MAX];

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int button_handle;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static int security_public_callback( uint8_t index, BUTTON_STATUS_TYPE state )
{
	static uint32_t time_count = 0;
	uint8_t i = 0;

	time_count = osKernelGetTickCount();
    if ( (BUTTON_PRESS_DOWN == state) || (BUTTON_LONG_PRESS_START == state) )
    {
		switch (button_ros_attr[index].pub_data_type)
		{
			case BUTTON_PUB_DATA_TYPE_BOOL:
				((std_msgs__msg__Bool *)button_ros_attr[index].pub_msg_data)->data = 1;
				message_publish(&button_ros_attr[index].g_pub_uros_button);
				LOG_DEBUG("%s press down! time = %ld", (char *) button_ros_attr[index].button_pub_name, time_count);
			break;

			case BUTTON_PUB_DATA_TYPE_STRING:
            	LOG_DEBUG("pub  %s", button_ros_attr[index].button_pub_name);
			// if (strcmp(button_ros_attr[index].button_pub_name, "water_position"))
			// {
			// 	water_position_info_pub(state, 0, index);
			// }
			break;

			default:
				return -1;
		}
    }
    else if ( BUTTON_PRESS_UP == state)
    {
		
        ((std_msgs__msg__Bool *)button_ros_attr[index].pub_msg_data)->data = 0;
        message_publish(&button_ros_attr[index].g_pub_uros_button);
        LOG_DEBUG("%s press up! time = %ld", (char *)button_ros_attr[index].button_pub_name, time_count);
    }

    return 0;
}

static void emerg_exit_callback(const struct ca_device *dev, uint16_t GPIO_Pin)
{
	static uint32_t last_count = 0;

	if ((!last_count) || (osKernelGetTickCount()>(last_count+100)))
	{	
		last_count = osKernelGetTickCount();
		return;
	}

	if (osKernelGetTickCount() > (last_count+5))
	{
		LOG_DEBUG("exit!!!!!!!!!!!!!!!!!!!!!");
		last_count= 0;
	}

	return;
}

static void button_state_sub_callback(const void *msgin) 
{
	int index = 0;

	for(index = 0; index < SECURITY_BUTTON_MAX; index++)
	{
		if (msgin == &button_ros_attr->sub_msg_data)
		{
			// message_publish(&button_ros_attr[index].g_pub_uros_button);
		}
	}
}

/*****************************************************************
 * 函数定义
 ******************************************************************/
void security_button_attr_init(void)
{

#ifdef EMERG

	memset(&button_info[BUTTON_SECURITY_EMERG], 0, sizeof(BUTTON_ATTR_T));

	button_info[BUTTON_SECURITY_EMERG].gpio_info = &bus_gpio_emerg;
	button_info[BUTTON_SECURITY_EMERG].trigger_condition = GPIO_TRIGGER_HIGH;
	button_info[BUTTON_SECURITY_EMERG].index = BUTTON_SECURITY_EMERG;
	
	button_attr[BUTTON_SECURITY_EMERG].gpio_info = &bus_gpio_emerg;
	button_attr[BUTTON_SECURITY_EMERG].attach.button_press_down = 1;
	button_attr[BUTTON_SECURITY_EMERG].attach.button_press_up = 1;
	button_attr[BUTTON_SECURITY_EMERG].attach.button_exit = 1;
	button_attr[BUTTON_SECURITY_EMERG].button_callback = security_public_callback;
	button_attr[BUTTON_SECURITY_EMERG].gpio_exti_callback = emerg_exit_callback;
	
	button_ros_attr[BUTTON_SECURITY_EMERG].pub_msg_data = &uros_emerg_pub;
	button_ros_attr[BUTTON_SECURITY_EMERG].sub_msg_data = &uros_emerg_sub;
	button_ros_attr[BUTTON_SECURITY_EMERG].button_pub_name = "/emerg";
	button_ros_attr[BUTTON_SECURITY_EMERG].button_sub_name = "/emerg/get";
	button_ros_attr[BUTTON_SECURITY_EMERG].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);

#endif

#ifdef CRASH_LEFT

	memset(&button_info[BUTTON_SECURITY_CRASH_LEFT], 0, sizeof(BUTTON_ATTR_T));
	
	button_info[BUTTON_SECURITY_CRASH_LEFT].gpio_info = &bus_gpio_crash_left;
	button_info[BUTTON_SECURITY_CRASH_LEFT].trigger_condition = GPIO_TRIGGER_LOW;
	button_info[BUTTON_SECURITY_CRASH_LEFT].index = BUTTON_SECURITY_CRASH_LEFT;

	button_attr[BUTTON_SECURITY_CRASH_LEFT].gpio_info = &bus_gpio_crash_left;
	button_attr[BUTTON_SECURITY_CRASH_LEFT].attach.button_press_down = 1;
	button_attr[BUTTON_SECURITY_CRASH_LEFT].attach.button_press_up = 1;
	button_attr[BUTTON_SECURITY_CRASH_LEFT].attach.button_exit = 1;
	button_attr[BUTTON_SECURITY_CRASH_LEFT].button_callback = security_public_callback;
	button_attr[BUTTON_SECURITY_CRASH_LEFT].gpio_exti_callback = emerg_exit_callback;

	button_ros_attr[BUTTON_SECURITY_CRASH_LEFT].pub_msg_data = &uros_crash_left_pub;
	button_ros_attr[BUTTON_SECURITY_CRASH_LEFT].sub_msg_data = &uros_crash_left_sub;
	button_ros_attr[BUTTON_SECURITY_CRASH_LEFT].button_pub_name = "/crash_left";
	button_ros_attr[BUTTON_SECURITY_CRASH_LEFT].button_sub_name = "/crash_left/get";	
	button_ros_attr[BUTTON_SECURITY_CRASH_LEFT].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);

#endif

#ifdef CRASH_RIGHT

	memset(&button_info[BUTTON_SECURITY_CRASH_RIGHT], 0, sizeof(BUTTON_ATTR_T));
	
	button_info[BUTTON_SECURITY_CRASH_RIGHT].gpio_info = &bus_gpio_crash_right;
	button_info[BUTTON_SECURITY_CRASH_RIGHT].trigger_condition = GPIO_TRIGGER_LOW;
	button_info[BUTTON_SECURITY_CRASH_RIGHT].index = BUTTON_SECURITY_CRASH_RIGHT;

	button_attr[BUTTON_SECURITY_CRASH_RIGHT].gpio_info = &bus_gpio_crash_right;
	button_attr[BUTTON_SECURITY_CRASH_RIGHT].attach.button_press_down = 1;
	button_attr[BUTTON_SECURITY_CRASH_RIGHT].attach.button_press_up = 1;
	button_attr[BUTTON_SECURITY_CRASH_RIGHT].attach.button_exit = 1;
	button_attr[BUTTON_SECURITY_CRASH_RIGHT].button_callback = security_public_callback;
	button_attr[BUTTON_SECURITY_CRASH_RIGHT].gpio_exti_callback = emerg_exit_callback;

	button_ros_attr[BUTTON_SECURITY_CRASH_RIGHT].pub_msg_data = &uros_crash_right_pub;
	button_ros_attr[BUTTON_SECURITY_CRASH_RIGHT].sub_msg_data = &uros_crash_right_sub;
	button_ros_attr[BUTTON_SECURITY_CRASH_RIGHT].button_pub_name = "/crash_right";
	button_ros_attr[BUTTON_SECURITY_CRASH_RIGHT].button_sub_name = "/crash_right/get";	
	button_ros_attr[BUTTON_SECURITY_CRASH_RIGHT].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);

#endif

	return;
}

void security_button_init(void)
{
	int index = 0;

	security_button_attr_init();
	
	for(index = 0; index < SECURITY_BUTTON_MAX; index++)
	{
		publisher_init(&button_ros_attr[index].g_pub_uros_button, button_ros_attr[index].stMsg,
                   button_ros_attr[index].button_pub_name, &button_ros_attr[index].pub_msg_data, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__Bool));
		subscrption_init(&button_ros_attr[index].g_sub_uros_button, button_ros_attr[index].stMsg,    
        			button_ros_attr[index].button_sub_name, &button_ros_attr[index].sub_msg_data, BEST, button_state_sub_callback);
		device_ioctl(button_handle, BUTTON_CMD_SET_ADD_BUTTON, (void *) &button_info[index]);
		device_ioctl(button_handle, BUTTON_CMD_SET_ADD_ATTACH, (void *) &button_attr[index]);
	}

	return;
}

int pal_security_init(void)
{
	security_button_init();

	return 0;
}

PAL_MODULE_INIT(pal_security_init);

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
