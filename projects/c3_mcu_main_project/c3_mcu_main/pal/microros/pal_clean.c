/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS清洁组件
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "pal_clean.h"
#include "fal.h"
#include <stdarg.h>
#include "pal_uros.h"
#include "std_msgs/msg/bool.h"
#include "log.h"
#include "multi_button.h"
#include "std_msgs/msg/string.h"
#include "cJSON.h"
#include "define_button.h"
#include "define_carpet.h"
#include "std_msgs/msg/u_int16.h"

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
typedef struct Button_Clean_Ros_Attr
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
    
}BUTTON_CLEAN_ROS_ATTR_T, *BUTTON_CLEAN_ROS_ATTR_P;           //microros属性信息

static publisher  g_uros_carpet = {0};
static std_msgs__msg__UInt16 uros_carpet;

static std_msgs__msg__Bool uros_clean_water_box_sub;
static std_msgs__msg__Bool uros_clean_water_box_pub;

static std_msgs__msg__String uros_water_position_sub = {0};
static std_msgs__msg__String uros_water_position_pub = {0};

static std_msgs__msg__Bool uros_dirt_box_sub;
static std_msgs__msg__Bool uros_dirt_box_pub;

// std_msgs__msg__Bool uros_emerg_sub;
// std_msgs__msg__Bool uros_emerg_pub;

/*****************************************************************
 * 全局变量定义
 ******************************************************************/
struct bus_info bus_gpio_clean_water_box = {
			.bus_name = "gpioE",
			// .bus_addr = ,
};

struct bus_info bus_gpio_water_position = {
			.bus_name = "gpioE",
			// .bus_addr = 5,
};

struct bus_info bus_gpio_dirt_box = {
			.bus_name = "gpioC",
			.bus_addr = 13,
};

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static BUTTON_CLEAN_ROS_ATTR_T button_ros_attr[CLEAN_BUTTON_MAX];
static BUTTON_ATTR_T button_info[CLEAN_BUTTON_MAX]; 
static BUTTON_ATTACH_ATTR_T button_attr[CLEAN_BUTTON_MAX];
/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int button_handle;
extern int carpet_handle;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
void water_position_info_pub(float clean_value, float waste_value, uint8_t index)
{
	cJSON *root = NULL;
	char *string_msg = NULL;
	float temp_data[2] = {0};
	
	root = cJSON_CreateObject();
	if(root != NULL)
	{
		cJSON_AddNumberToObject(root, "clean_status", clean_value);
		cJSON_AddNumberToObject(root, "clean_waring_threshold", 1.0);
		cJSON_AddNumberToObject(root, "waste_status", waste_value);
		cJSON_AddNumberToObject(root, "waste_waring_threshold", 0.0);
		string_msg = cJSON_PrintUnformatted(root);
		 if(string_msg != NULL)
		 {
			((std_msgs__msg__String *)button_ros_attr[index].pub_msg_data)->data.data = string_msg;
			((std_msgs__msg__String *)button_ros_attr[index].pub_msg_data)->data.size = strlen(string_msg)+1;
			((std_msgs__msg__String *)button_ros_attr[index].pub_msg_data)->data.capacity = 0;
			message_publish(&button_ros_attr[index].g_pub_uros_button);
		 }  
		 if(root != NULL)
		 {
			cJSON_Delete(root);
		 }
		 if(string_msg != NULL)
		 {
			 LOG_DEBUG("----->%s",string_msg);
			cJSON_free(string_msg);
		 }
	}
	else
	{
		LOG_DEBUG("creat json obj failed on line %d\r\n", __LINE__);
	}
	return;
}

/*****************************************************************
 * 函数定义
 ******************************************************************/
/************IO调试需要 by-畅一晨 *****************/

int public_callback( uint8_t index, BUTTON_STATUS_TYPE state )
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
			if (strcmp(button_ros_attr[index].button_pub_name, "water_position"))
			{
				// water_position_info_pub(state, 0, index);
				LOG_DEBUG("pub  %s", button_ros_attr[index].button_pub_name);
			}
			break;

			default:
				return -1;
		}
    }
    else if ( BUTTON_PRESS_UP == state)
    {
		for (i = 0; i < 1; i++)
		{
            ((std_msgs__msg__Bool *)button_ros_attr[index].pub_msg_data)->data = 0;
            message_publish(&button_ros_attr[index].g_pub_uros_button);
			LOG_DEBUG("%s press up! time = %ld", (char *)button_ros_attr[index].button_pub_name, time_count);
		}
    }

    return 0;
}


void emerg_exit_callback(const struct ca_device *dev, uint16_t GPIO_Pin)
{
	static uint32_t last_count = 0;

	if ((!last_count) || (osKernelGetTickCount()>(last_count+100)))
	{	
		last_count = osKernelGetTickCount();
		return;
	}

	if (osKernelGetTickCount() > (last_count+5))
	{
		// LOG_DEBUG("emerg exit!!!!!!!!!!!!!!!!!!!!!");
		last_count= 0;
	}

	return;
}

void button_state_sub_callback(const void *msgin) 
{
	int index = 0;

	for(index = 0; index < CLEAN_BUTTON_MAX; index++)
	{
		if (msgin == &button_ros_attr->sub_msg_data)
		{
			message_publish(&button_ros_attr[index].g_pub_uros_button);
		}
	}
}

void clean_button_attr_init(void)
{

#ifdef CLEAN_WATER_BOX

	memset(&button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX], 0, sizeof(BUTTON_ATTR_T));

	button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX].gpio_info = &bus_gpio_clean_water_box;
	button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX].period = 1000,
	button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX].trigger_condition = GPIO_TRIGGER_LOW;
	button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX].index = BUTTON_CLEAN_CLEAN_WATRE_BOX;
	button_info[BUTTON_CLEAN_CLEAN_WATRE_BOX].long_press_hold_type = BUTTON_LONG_PRESS_HOLD_CONTINUE_TRIGGER;
	
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].gpio_info = &bus_gpio_clean_water_box;
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].attach.button_press_down = 1;
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].attach.button_press_up = 1;
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].attach.button_long_press_start = 1;
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].button_callback = public_callback;
	button_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].gpio_exti_callback = NULL;
	
	button_ros_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].pub_msg_data = &uros_clean_water_box_pub;
	button_ros_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].sub_msg_data = &uros_clean_water_box_sub;
	button_ros_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].button_pub_name = "clean_water_box";
	button_ros_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].button_sub_name = "clean_water_box/get";
	button_ros_attr[BUTTON_CLEAN_CLEAN_WATRE_BOX].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);

#endif

#ifdef WATER_POSITION

	memset(&button_info[BUTTON_CLEAN_WATER_POSITION], 0, sizeof(BUTTON_ATTR_T));
	
	button_info[BUTTON_CLEAN_WATER_POSITION].gpio_info = &bus_gpio_water_position;
	button_info[BUTTON_CLEAN_WATER_POSITION].period = 1000;
	button_info[BUTTON_CLEAN_WATER_POSITION].trigger_condition = GPIO_TRIGGER_LOW;
	button_info[BUTTON_CLEAN_WATER_POSITION].index = BUTTON_CLEAN_WATER_POSITION;
	button_info[BUTTON_CLEAN_WATER_POSITION].long_press_hold_type = BUTTON_LONG_PRESS_HOLD_CONTINUE_TRIGGER;

	button_attr[BUTTON_CLEAN_WATER_POSITION].gpio_info = &bus_gpio_water_position;
	button_attr[BUTTON_CLEAN_WATER_POSITION].attach.button_press_down = 1;
	button_attr[BUTTON_CLEAN_WATER_POSITION].attach.button_press_up = 1;
	button_attr[BUTTON_CLEAN_WATER_POSITION].attach.button_long_press_start = 1;
	button_attr[BUTTON_CLEAN_WATER_POSITION].attach.button_exit = 1;
	button_attr[BUTTON_CLEAN_WATER_POSITION].button_callback = public_callback;
	button_attr[BUTTON_CLEAN_WATER_POSITION].gpio_exti_callback = emerg_exit_callback;

	button_ros_attr[BUTTON_CLEAN_WATER_POSITION].pub_msg_data = &uros_water_position_pub;
	button_ros_attr[BUTTON_CLEAN_WATER_POSITION].sub_msg_data = &uros_water_position_sub;
	button_ros_attr[BUTTON_CLEAN_WATER_POSITION].button_pub_name = "water_position";
	button_ros_attr[BUTTON_CLEAN_WATER_POSITION].button_sub_name = "water_position/get";	
	button_ros_attr[BUTTON_CLEAN_WATER_POSITION].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);

#endif

#ifdef DIRT_BOX
	memset(&button_info[BUTTON_CLEAN_DIRT_BOX], 0, sizeof(BUTTON_ATTR_T));

	button_info[BUTTON_CLEAN_DIRT_BOX].gpio_info = &bus_gpio_dirt_box;
	button_info[BUTTON_CLEAN_DIRT_BOX].period = 1000,
	button_info[BUTTON_CLEAN_DIRT_BOX].trigger_condition = GPIO_TRIGGER_LOW;
	button_info[BUTTON_CLEAN_DIRT_BOX].index = BUTTON_CLEAN_DIRT_BOX;
	button_info[BUTTON_CLEAN_DIRT_BOX].long_press_hold_type = BUTTON_LONG_PRESS_HOLD_CONTINUE_TRIGGER;
	
	button_attr[BUTTON_CLEAN_DIRT_BOX].gpio_info = &bus_gpio_dirt_box;
	button_attr[BUTTON_CLEAN_DIRT_BOX].attach.button_press_down = 1;
	button_attr[BUTTON_CLEAN_DIRT_BOX].attach.button_press_up = 1;
	button_attr[BUTTON_CLEAN_DIRT_BOX].attach.button_long_press_start = 1;
	button_attr[BUTTON_CLEAN_DIRT_BOX].button_callback = public_callback;
	button_attr[BUTTON_CLEAN_DIRT_BOX].gpio_exti_callback = NULL;
	
	button_ros_attr[BUTTON_CLEAN_DIRT_BOX].pub_msg_data = &uros_dirt_box_pub;
	button_ros_attr[BUTTON_CLEAN_DIRT_BOX].sub_msg_data = &uros_dirt_box_sub;
	button_ros_attr[BUTTON_CLEAN_DIRT_BOX].button_pub_name = "dirt_box";
	button_ros_attr[BUTTON_CLEAN_DIRT_BOX].button_sub_name = "dirt_box/get";
	button_ros_attr[BUTTON_CLEAN_DIRT_BOX].stMsg = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);


#endif

	return;
}

void clean_button_init(void)
{
	int index = 0;

	clean_button_attr_init();

	for(index = 0; index < CLEAN_BUTTON_MAX; index++)
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


void carpet_public_callback( uint16_t state )
{

	if (state == CARPET_SOFT_GROUND)
	{
		// LOG_DEBUG("carpet publish");
		uros_carpet.data = 1;
	}
	else if (state == CARPET_TOUGH_GROUND)
	{
		// LOG_DEBUG("carpet publish--------------------------");
		uros_carpet.data = 0;
	}
	
	message_publish(&g_uros_carpet);
	
	return;
}

void carpet_init(void)
{
	publisher_init(&g_uros_carpet,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt16),
                   "/carpet", &uros_carpet, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__UInt16));

	device_ioctl( carpet_handle, CARPET_SET_CALLBACK, &carpet_public_callback);
}

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

int pal_clean_init(void)
{
	carpet_init();
	clean_button_init();

	return 0;
}

PAL_MODULE_INIT(pal_clean_init);

#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
