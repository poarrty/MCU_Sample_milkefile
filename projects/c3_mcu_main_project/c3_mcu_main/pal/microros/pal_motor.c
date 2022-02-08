/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         曾曼云
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:		microROS清洁电机控制
 ** Others:
 ** Function List:
 ** History:        2021-11 曾曼云 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       曾曼云						1.0
 *创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "pal_motor.h"
#include "fal.h"
#include "devices.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "pal_uros.h"
#include "log.h"
#include "shell.h"
#include "define_motor.h"
#include "gpio.h"
#include "std_msgs/msg/string.h"
#include "cJSON.h"
#include "sensor_msgs/msg/range.h"
#include "SEGGER_RTT.h"
#include "cputime.h"
#include "fal_motor.h"
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
#define RPM_MAX 10
/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/
/*****************************************************************
 * 全局变量定义
 ******************************************************************/
static subscrption g_clean_component_control;
static std_msgs__msg__String clean_component_control = {0};
static char clean_component_control_buffer[256];

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
static subscrption g_clean_motor_ctrl;
static sensor_msgs__msg__Range clean_motor_ctrl;
/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int clean_water_pump_handle;
extern int wheel_handle;
extern int up_down_push_rod_handle;
extern int side_brush_handle;
extern int roller_brush_handle;
extern int roller_tube_handle;
extern int clean_water_pump_handle;
extern int sewage_water_pump_handle;
extern int water_valve_handle;
extern int fan_motor_handle;
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
int clean_ops_test(uint8_t clean_module_index, uint8_t speed_switch) {
    LOG_DEBUG("clean_module_index = %d,  speed_switch = %d", clean_module_index,
              speed_switch);
    int rpm = speed_switch;
    switch (clean_module_index) {
        case UP_DOWM_PUSH_ROD:
            device_ioctl(up_down_push_rod_handle, MOTOR_CMD_SET_RPM,
                         (void *) &rpm);

            break;
        case SIDE_BRUSH:
            device_ioctl(side_brush_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);

            break;
        case ROLLER_BRUSH:
            device_ioctl(roller_brush_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);

            break;
        case ROLLER_TUBE:
            device_ioctl(roller_tube_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);

            break;
        case CLEAN_WATER_PUMP:
            device_ioctl(clean_water_pump_handle, MOTOR_CMD_SET_RPM,
                         (void *) &rpm);

            break;
        case SEWAGE_WATER_PUMP:
            // device_ioctl(sewage_water_pump_handle, MOTOR_CMD_SET_RPM ,
            // (void*)&rpm);
            if (rpm == 1) {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14,
                                  GPIO_PIN_SET);  //污水泵使能
            } else {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
            }

            break;

        case FAN_MOTOR:
            device_ioctl(fan_motor_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);

            break;
        case CLEAN_WATER_VALVE:
            if (rpm == 1) {
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
            }
            break;
        case SEWAGE_WATER_VALVE:
            if (rpm == 1) {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,
                                  GPIO_PIN_SET);  //污水泵使能
            } else {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
            }
            break;

        default:
            break;
    }
    return 0;
}

int test_help(void) {
    LOG_DEBUG("********************清洁组件测试命令********************");
    LOG_DEBUG("*                                                      * ");
    LOG_DEBUG("* 0.推杆     开启    clean_ops_test 0 1                  *");
    LOG_DEBUG("*          关闭    clean_ops_test 0 0                  *");
    LOG_DEBUG("* 1.边刷     开启    clean_ops_test 1 5（比例百分之50） *");
    LOG_DEBUG("*          关闭    clean_ops_test 1 0                  *");
    LOG_DEBUG("* 2.滚刷     开启    clean_ops_test 2 5（比例百分之50） *");
    LOG_DEBUG("*          关闭    clean_ops_test 2 0                  *");
    LOG_DEBUG("* 3.滚筒     开启    clean_ops_test 3 5（比例百分之50） *");
    LOG_DEBUG("*          关闭    clean_ops_test 3 0                  *");
    LOG_DEBUG("* 4.净水泵   开启    clean_ops_test 4 5（比例百分之50） *");
    LOG_DEBUG("*          关闭    clean_ops_test 4 0                  *");
    LOG_DEBUG("* 5.污水泵   开启    clean_ops_test 5 1                  *");
    LOG_DEBUG("*          关闭    clean_ops_test 5 0                  *");
    LOG_DEBUG("* 6.水阀     开启    clean_ops_test 6 1                  *");
    LOG_DEBUG("*          关闭    clean_ops_test 6 0                  *");
    LOG_DEBUG("* 7.风机     开启    clean_ops_test 7 5（比例百分之50） *");
    LOG_DEBUG("*          关闭    clean_ops_test 7 0                  *");
    LOG_DEBUG(
        "* 8.清水阀    开启   clean_ops_test 8 1 			"
        "		*");
    LOG_DEBUG("*          关闭    clean_ops_test 8 0                  *");
    LOG_DEBUG(
        "* 9.污水阀    开启   clean_ops_test 9  1 			"
        "	*");
    LOG_DEBUG("*          关闭    clean_ops_test 9 0                  *");
    LOG_DEBUG("*                                                      * ");
    LOG_DEBUG("********************移动控制测试命令********************");
    LOG_DEBUG("*                                                      * ");
    LOG_DEBUG("*开启 motor_speed_vw 100（线速度mm/s） 50(角速度rad/s) *");
    LOG_DEBUG("*关闭 motor_speed_vw 0 0                               *");
    LOG_DEBUG("*                                                      * ");
    LOG_DEBUG("********************************************************");
    return 0;
}

int motor_speed_vw(int16_t speed_v, int16_t speed_w) {
    LOG_DEBUG("speed_v = %d,  speed_w = %d", speed_v, speed_w);
    MOTOR_SPEED x;
    x.speed_v_t = speed_v;
    x.speed_w_t = speed_w;
    device_ioctl(wheel_handle, MOTOR_CMD_SET_SPEED, (void *) &x);
    return 0;
}

void clean_ctrl_comp_all_start(void) {
    int rpm;
    //开边刷
    rpm = clean_ctrl_comp_info.dev_set_value[SIDE_BRUSH] * RPM_MAX;
    device_ioctl(side_brush_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);
    //开滚刷
    rpm = clean_ctrl_comp_info.dev_set_value[ROLLER_BRUSH] * RPM_MAX;
    device_ioctl(roller_brush_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);
    //滚筒
    rpm = clean_ctrl_comp_info.dev_set_value[ROLLER_TUBE] * RPM_MAX;
    device_ioctl(roller_tube_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);
    //清水泵
    rpm = clean_ctrl_comp_info.dev_set_value[CLEAN_WATER_PUMP];
    device_ioctl(clean_water_pump_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);
    //污水泵
    rpm = (int) clean_ctrl_comp_info.dev_set_value[SEWAGE_WATER_VALVE];
    if (rpm == 1) {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);  //污水泵使能
    } else {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
    }
    //风机
    rpm = clean_ctrl_comp_info.dev_set_value[FAN_MOTOR] * RPM_MAX;
    device_ioctl(fan_motor_handle, MOTOR_CMD_SET_RPM, (void *) &rpm);
    //清水阀
    rpm = (int) clean_ctrl_comp_info.dev_set_value[CLEAN_WATER_VALVE];
    if (rpm == 1) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    }
    //污水阀
    rpm = (int) clean_ctrl_comp_info.dev_set_value[SEWAGE_WATER_VALVE];
    if (rpm == 1) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);  //污水泵使能
    } else {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
    }
}

void set_speed_callback(const void *msgin) {
    cJSON *json_msg = NULL;
    cJSON *json_comp = NULL;
    cJSON *json_temp = NULL;
    if (msgin == NULL)
        return;

    const std_msgs__msg__String *string_msg =
        (const std_msgs__msg__String *) msgin;
    if (string_msg->data.data != NULL) {
        json_msg = cJSON_Parse(string_msg->data.data);
        if (json_msg == NULL)
            return;
        json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, "model");
        if (json_temp != NULL) {
            clean_ctrl_comp_info.model = cJSON_GetStringValue(json_temp);
            if (!strncasecmp(clean_ctrl_comp_info.model, "mopping",sizeof("mopping"))) {
                clean_ctrl_comp_info.mode = ID_MODE_MOPPING;
            } else if (!strncasecmp(clean_ctrl_comp_info.model, "dedusting",sizeof("dedusting"))) {
                clean_ctrl_comp_info.mode = ID_MODE_DEDUSTING;
            } else {
                clean_ctrl_comp_info.mode = ID_MODE_NULL;
            }
            LOG_DEBUG("current mode :%s---%d", clean_ctrl_comp_info.model,clean_ctrl_comp_info.mode);
        }
        json_comp = cJSON_GetObjectItemCaseSensitive(json_msg, "component");
        if (json_comp != NULL) {
            for (uint8_t i = 0; i < CLEAN_MODULE_TYPE_MAX; i++) {
                json_temp = cJSON_GetObjectItemCaseSensitive(json_comp,mcu_dev_name[i]);
                if (json_temp != NULL) {
                    clean_ctrl_comp_info.dev_set_value[i] = json_temp->valuedouble;
                    LOG_DEBUG("%d, %s:%f", clean_ctrl_comp_info.mode,mcu_dev_name[i],clean_ctrl_comp_info.dev_set_value[i]);
                } else {
                    LOG_DEBUG("%s get error", mcu_dev_name[i]);
                }
            }
        }
        clean_ctrl_comp_info.last_recv_time = clock_cpu_gettime();
        cJSON_Delete(json_msg);
    }
}

void clean_ops_test_callback(const void *msgin)
{
	const sensor_msgs__msg__Range *msg = (const sensor_msgs__msg__Range *) msgin;

	int64_t clean_module_index = msg->radiation_type;
	int64_t speed_switch = msg->min_range;
	int64_t test_arg = msg->range;
    int64_t rpm = msg->range;
	LOG_DEBUG("clean_module_index = %lld, range = %lld\r\n", clean_module_index, test_arg);  
	LOG_DEBUG("range = %lld\r\n", test_arg); 

	switch (clean_module_index)
	{
	case UP_DOWM_PUSH_ROD:
		device_ioctl(up_down_push_rod_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case SIDE_BRUSH:
		device_ioctl(side_brush_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case ROLLER_BRUSH:
		device_ioctl(roller_brush_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case ROLLER_TUBE:
		device_ioctl(roller_tube_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case CLEAN_WATER_PUMP:
		device_ioctl(clean_water_pump_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case SEWAGE_WATER_PUMP:
		//device_ioctl(sewage_water_pump_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);
      if(rpm == 1)
      {
         HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14,GPIO_PIN_SET);//污水泵使能
      }
      else
      {
         HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14,GPIO_PIN_RESET);
      }

		break;

	case FAN_MOTOR:
		device_ioctl(fan_motor_handle, MOTOR_CMD_SET_RPM , (void*)&rpm);

		break;
	case CLEAN_WATER_VALVE:
	if(rpm == 1)
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	}
		break;
	case SEWAGE_WATER_VALVE:
	if(rpm == 1)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_SET);//污水泵使能
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_RESET);
	}
		break;

	
	default:
		break;
	}
	return;
}

 int pal_motor_init(void) {
    subscrption_init(
        &g_clean_component_control, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "/clean_component/control", &clean_component_control, BEST, set_speed_callback);

	subscrption_init(&g_clean_motor_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
	"/clean_motor_ctrl", &clean_motor_ctrl, BEST, clean_ops_test_callback);

    clean_component_control.data.data = clean_component_control_buffer;
    clean_component_control.data.size = 0;
    clean_component_control.data.capacity =
        sizeof(clean_component_control_buffer);

    return 0;
}
PAL_MODULE_INIT(pal_motor_init);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 test_help, test_help, test_help);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 clean_ops_test, clean_ops_test, clean_ops_test);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 motor_speed_vw, motor_speed_vw, motor_speed_vw);
#ifdef __cplusplus
}
#endif

/* @} Robot_PAL_UROS */
/* @} Robot_PAL */
