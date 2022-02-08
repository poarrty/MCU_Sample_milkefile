/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal.h"
#include "devices.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "cmsis_os.h"
#include "log.h"
#include "define_motor.h"
#include "define_button.h"
#include "system_status.h"
#include "SEGGER_RTT.h"
/***************************/

/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_FAL 功能适配层 - FAL
 *
 * @brief FAL层实现各种通用的功能模块，并提供统一的接口，为上层调用；
 *        为了便于处理不同的协议，降低协议在上下两层相互转换的复杂和提高系统性能，
 *        该层还支持协议透传功能，即允许上层直接将协议数据发送下来，然后指定到特定的协议模块处理\n
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

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern int led_handle;
extern int dig_usound_S12_handle;
extern int bldcm_handle;
extern int sw1_handle;
extern int button_handle;
extern int shell_handle;

void *fal_buf = NULL;
/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/
void fal_all_init();
/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/
int _write(int file, char *ptr, int len)
{
	SEGGER_RTT_Write(0, ptr, len);
	// device_write(shell_handle, ptr, len);
	return len;
}

static void led_run_blink_cb(void *argument) {
    device_ioctl(led_handle, LED_GPIO_TOGGLE, NULL);
		
}

/************数字超声调试需要 by-张博炜 *****************/
static void du_run_blink_cb(void *argument) {
    device_ioctl(dig_usound_S12_handle, 0 , NULL);
		
}

BUTTON_PIN_ATTR_T bus_gpio_soc = {
	.gpio_info.bus_name = "gpioE",
	.gpio_info.bus_addr = 0,
	.state = 1,
};

uint8_t bsp_pmu_all_on(void) 
{    
	device_ioctl(button_handle, BUTTON_CMD_GET_PIN_STATE, (void *) &bus_gpio_soc);

	return 0;
}

void task_default_run(void *argument) {
	bsp_pmu_all_on();
    for(;;)
    {
    	led_run_blink_cb(NULL);
		// du_run_blink_cb(NULL);
		LOG_DEBUG("led toggle!\r\n");
		osDelay(500);
	}
}
void task_dig_usound_run(void *argument) {
    for(;;)
    {	
		LOG_DEBUG("led toggle!\r\n");
		osDelay(5000);

	}
}

/*****************************************************************/
/**
 * Function:       fal_init
 * Description:    初始化FAL层
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
int fal_Init(void) {
	fal_all_init();		
    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_deinit
 * Description:    释放FAL层资源
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
int fal_deInit(void) {
    return 0;
}

void fal_all_init(){
	task_system_status_init();
	fal_init_call *fal_init_ptr = &_fal_start;
    for (; fal_init_ptr < &_fal_end; fal_init_ptr++) {
        (*fal_init_ptr)();
    }
}

#ifdef __cplusplus
}
#endif

/* @} Go_FAL */
/* @} Robot-NAV_407 */
