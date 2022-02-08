/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      hal.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-01-
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-01 robot创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/

#include "hal.h"
#include "devices.h"
#include "define.h"
#include "define_motor.h"
/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_HAL 硬件适配层 - HAL
 *
 * @brief
 * HAL层屏蔽了硬件平台的差异，向上层提供了统一的调用函数接口，支持不同的硬件平台
 * \n \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/


/*****************************************************************
 * 全局变量定义
 ******************************************************************/
int cliff_handle_ = -1;
int bsm_handle = -1;
int imu_handle = -1;
int vel_handle = -1;
int shell_handle = -1;
int led_handle = -1;
int dig_usound_S12_handle = -1;
int pwmtest_handle = -1;
int sw1_handle = -1;
int driver_handle = -1;						//发动机
int cliff_oml_handle1 =-1;
int cliff_oml_handle2 =-1;
int cliff_oml_handle3 =-1;
int cliff_oml_handle4 =-1;
int cliff_oml_handle5 =-1;
int cliff_oml_handle6 =-1;
int switch_group1_handle = -1;
int bldcm_handle = -1;
int button_handle = -1;
int carpet_handle = -1;
int gpio_motor_handle = -1;
int test_motor_handle = -1;

int wheel_handle =-1;
int up_down_push_rod_handle=-1;
int side_brush_handle=-1;
int roller_brush_handle=-1;
int roller_tube_handle=-1;
int clean_water_pump_handle=-1;
int sewage_water_pump_handle=-1;
int water_valve_handle=-1;
int fan_motor_handle=-1;
/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
struct bus_rs485 rs4851 = {
	.common = {
			.bus_name = "rs4851",
		},
	.uart = {
			.bus_name = "uart3",
		},
	// .gpio_ctrl = {
	// 		.bus_name = "gpioA",
	// 		.bus_addr = 1,
	// 	},
	.gpio_read_status = GPIO_ACTIVE_HIGH,
};

struct bus_sw sw1 = {
	.common = {
			.bus_name = "sw1",
		},
	.gpio_ctrl[0] = {
			.bus_name = "gpioD",
			.bus_addr = 5,
		},
	.gpio_ctrl[1] = {
			.bus_name = "gpioD",
			.bus_addr = 6,
		},
	.gpio_ctrl[2] = {
			.bus_name = "gpioB",
			.bus_addr = 3,
		},

};

struct bus_info bus_gpio_led = {
			.bus_name = "gpioA",
			.bus_addr = 15,
};

struct bus_info shell_uart = {
			.bus_name = "uart3",
};

struct bus_info bus_gpio_dig_usound = {
			.bus_name = "gpioC",
			.bus_addr = 9,
};
struct bus_info bus_tim_dig_usound = {
			.bus_name = "tim8",
			.bus_addr = 2,
};

struct bus_info bus_tim_pwmtest = {
			.bus_name = "tim4",
			.bus_addr = 1,
};

struct bus_info bus_can_sytron = {
			.bus_name = "can1",
};

struct bus_info bus_tim_cliff_oml = {
			.bus_name = "tim1",
			.bus_addr = 0,
};

struct bus_info bus_shell = {
			.bus_name = "uart3",
};

//断崖adc
struct bus_info bus_adc_cliff_oml1 = {
			.bus_name = "adc1",	
			.bus_addr = 4,		
};
struct bus_info bus_adc_cliff_oml2 = {
			.bus_name = "adc1",	
			.bus_addr = 13,		
};
struct bus_info bus_adc_cliff_oml3 = {
			.bus_name = "adc1",	
			.bus_addr = 3,		
};
struct bus_info bus_adc_cliff_oml4 = {
			.bus_name = "adc1",	
			.bus_addr = 4,		
};
struct bus_info bus_adc_cliff_oml5 = {
			.bus_name = "adc1",	
			.bus_addr = 8,		
};
struct bus_info bus_adc_cliff_oml6 = {
			.bus_name = "adc1",	
			.bus_addr = 9,		
};

struct bus_switch_group bus_switch_group1[] ={
	{
	.gpio_ctrl[0] = {
			.bus_name = "gpioA",
			.bus_addr = 6,
		},
	.gpio_ctrl[1] = {
			.bus_name = "gpioB",
			.bus_addr = 2,
		},
	.gpio_ctrl[2] = {
			.bus_name = "gpioB",  //若是两路切换开关，则第三路留空
			.bus_addr = 4,
		}
	}

};

MOTOR_BLDCM_BUS motor_bldcm_bus_1 = {
	.pwm.bus_name = "tim13",
	.pwm.bus_addr = 0 ,
	// .fg_etr.bus_name = "tim2",
	// .fg_etr.bus_addr = 1, 
	// .gpio_cw_ccw.bus_name ="gpioE",
	// .gpio_cw_ccw.bus_addr = 9
};


MOTOR_BLDCM_ATTR motor_bldcm_attr_1 = {
			.bldcm_nums = 1 ,	
			.gpio_cw_ccw_status = 1,
			.bldcm_bus = &motor_bldcm_bus_1,
};
MOTOR_BLDCM_BUS motor_wheel_bus[2] = {
	{
	.pwm.bus_name = "tim4",//left
	.pwm.bus_addr = 0,
	.fg_etr.bus_name = "tim3",
	.fg_etr.bus_addr = 1, 
	.gpio_cw_ccw.bus_name ="gpioG",
	.gpio_cw_ccw.bus_addr = 11,
	},
	{
	.pwm.bus_name = "tim11",//right
	.pwm.bus_addr = 0 ,
	.fg_etr.bus_name = "tim2",
	.fg_etr.bus_addr = 1, 
	.gpio_cw_ccw.bus_name ="gpioG",
	.gpio_cw_ccw.bus_addr = 12,
	}
};
MOTOR_BLDCM_ATTR motor_wheel_attr = {
			.bldcm_nums = 2 ,	
			.gpio_cw_ccw_status = 1,//1正转 0反转    上层speed给下正值为正转  负值为反转   
			.bldcm_bus = motor_wheel_bus,
};
MOTOR_BLDCM_BUS motor_fan_bus = {
	.pwm.bus_name = "tim8",
	.pwm.bus_addr = 0 ,
};
MOTOR_BLDCM_ATTR motor_fan_attr = {
			.bldcm_nums = 1 ,	
			.gpio_cw_ccw_status = 1,
			.bldcm_bus = &motor_fan_bus ,
};
MOTOR_BLDCM_BUS motor_roller_tube_bus = {
	.pwm.bus_name = "tim9",
	.pwm.bus_addr = 0 ,
};
MOTOR_BLDCM_ATTR motor_roller_tube_attr = {
			.bldcm_nums = 1 ,	
			.gpio_cw_ccw_status = 1,
			.bldcm_bus = &motor_roller_tube_bus ,
};
MOTOR_BLDCM_BUS motor_side_brush_bus[2] = {
	{
	.pwm.bus_name = "tim12",//left
	.pwm.bus_addr = 1,
	.gpio_cw_ccw.bus_name = "gpioD",
	.gpio_cw_ccw.bus_addr = 10
	},
	{
	.pwm.bus_name = "tim1",//right
	.pwm.bus_addr = 3 ,
	.gpio_cw_ccw.bus_name = "gpioE",
	.gpio_cw_ccw.bus_addr = 15
	}
};
MOTOR_BLDCM_ATTR motor_side_brush_attr = {
			.bldcm_nums = 2 ,	
			.gpio_cw_ccw_status = 1,
			.bldcm_bus = motor_side_brush_bus,
};
MOTOR_BLDCM_BUS motor_roller_brush_bus = {
	.pwm.bus_name = "tim1",
	.pwm.bus_addr = 2 ,
	.gpio_cw_ccw.bus_name = "gpioE",
	.gpio_cw_ccw.bus_addr =10,
};
MOTOR_BLDCM_ATTR motor_roller_brush_attr = {
			.bldcm_nums = 1 ,	
			.gpio_cw_ccw_status = 0,
			.bldcm_bus = &motor_roller_brush_bus ,
};


struct bus_info bus_button = {
			.bus_name = "gpioA",
};

struct bus_info bus_carpet = {
			.bus_name = "uart1",
};

struct bus_info bus_gpio_motor = {
			.bus_name = "gpioA",
};

sensor_obj_st sensor_objs[] = {
	{
		.bus_com = &shell_uart,
		.dev_name = "letter_shell",
		.handle = &shell_handle
	},
	// {
	// 	.bus_com = &bus_gpio_led,
	// 	.dev_name = "led",
	// 	.handle = &led_handle
	// },
	{
		.bus_com = &bus_tim_dig_usound,
		.dev_name = "du_S12",
		.handle = &dig_usound_S12_handle,
		.args = (void *)&bus_gpio_dig_usound
	},
	// {
	// 	.bus_com = &bus_can_sytron,
	// 	.dev_name = "sytron",
	// 	.handle = &driver_handle,
	// },
	{
		.bus_com = &bus_tim_cliff_oml,
		.dev_name = "cliff_oml1",
		.handle = &cliff_oml_handle1,
		.args = (void *)&bus_adc_cliff_oml1
	},
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "cliff_oml2",
	// 	.handle = &cliff_oml_handle2,
	// 	.args = (void *)&bus_adc_cliff_oml2
	// },
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "cliff_oml3",
	// 	.handle = &cliff_oml_handle3,
	// 	.args = (void *)&bus_adc_cliff_oml3
	// },
	// 	{
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "cliff_oml4",
	// 	.handle = &cliff_oml_handle4,
	// 	.args = (void *)&bus_adc_cliff_oml4
	// },
	// 	{
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "cliff_oml5",
	// 	.handle = &cliff_oml_handle5,
	// 	.args = (void *)&bus_adc_cliff_oml5
	// },
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "cliff_oml6",
	// 	.handle = &cliff_oml_handle6,
	// 	.args = (void *)&bus_adc_cliff_oml6
	// },
	// {
	// 	.bus_com = &bus_gpio_led,
	// 	.dev_name = "switch_group",
	// 	.handle = &switch_group1_handle,
	// 	.args = (void *)bus_switch_group1
	// },
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "bldcm_cwp",
	// 	.handle = &clean_water_pump_handle,
	// 	.args = (void *)&motor_bldcm_attr_1
	// },
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "bldcm_wheel",
	// 	.handle = &wheel_handle,
	// 	.args = (void *)&motor_wheel_attr
	// },
	// 	{
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "bldcm_fan",
	// 	.handle = &fan_motor_handle,
	// 	.args = (void *)&motor_fan_attr 
	// },
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "bldcm_rt",
	// 	.handle = &roller_tube_handle,
	// 	.args = (void *)&motor_roller_tube_attr 
	// },
	{
		.bus_com = &bus_tim_cliff_oml,
		.dev_name = "bldcm_sb",
		.handle = &side_brush_handle,
		.args = (void *)&motor_side_brush_attr 
	},
	// {
	// 	.bus_com = &bus_tim_cliff_oml,
	// 	.dev_name = "bldcm_rb",
	// 	.handle = &roller_brush_handle,
	// 	.args = (void *)&motor_roller_brush_attr 
	// },

	// {
	// 	.bus_com = &bus_button,
	// 	.dev_name = "button",
	// 	.handle = &button_handle,
	// },
	// {
	// 	.bus_com = &bus_gpio_motor,
	// 	.dev_name = "gpio_motor",
	// 	.handle = &gpio_motor_handle,
	// },
	// {
    //     .bus_com = &bus_carpet,
    //     .dev_name = "carpet",
    //     .handle = &carpet_handle,
    // }
	
};

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       hal_init
 * Description:    初始化HAL层
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int  hal_init(void)
{
	int index = 0;

	for(index = 0; index < ITEM_NUM(sensor_objs); index++)
	{
		device_register(sensor_objs[index].bus_com, sensor_objs[index].dev_name, sensor_objs[index].args);
		*(sensor_objs[index].handle) = device_open(sensor_objs[index].dev_name, 0);
	}
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       hal_deinit
 * Description:    释放HAL层资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int  hal_deInit(void)
{
	/*释放资源*/
	devices_deinit();
	return 0;
}

#ifdef __cplusplus
}
#endif

/* @} Robot_HAL */
/* @} Robot-NAV_407 */

/**************************** CMSIS ********************************/
/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup H_CMSIS 微控制器软件接口标准 - CMSIS
 *
 * @brief (CMSIS) 是 Cortex-M 处理器系列的与供应商无关的硬件抽象层。\n
 * CMSIS
 * 可实现与处理器和外设之间的一致且简单的软件接口，从而简化软件的重用，缩短微控制器开发人员新手的学习过程，并缩短新设备的上市时间。\n
 * \n
 * @{
 */

/* @} H_CMSIS */
/* @} Robot_HAL */

/**************************** Driver ********************************/
/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup H_DRIVER 外围驱动接口API - DRIVER
 *
 * @brief 包含处理器的外围驱动接口API。\n
 * \n
 * @{
 */

/* @} H_DRIVER */
/* @} Robot_HAL */
