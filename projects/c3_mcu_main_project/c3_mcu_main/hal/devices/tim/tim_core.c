/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         张博炜
  ** Version:        V0.0.1
  ** Date:           2021-11-2
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 张博炜 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-11-2      张博炜	     0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/

#include "tim_core.h"
#include "main.h"
#include "devices.h"
#include "define.h"
#include "sys_list.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>
// #include "tim.h"



/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_UART_CORE  - UART_CORE
 *
 * @brief  \n
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
struct tim_node{
	struct list_struct node;
	tim_callback callback;
	const struct ca_device *dev;
	void *handler;
};
/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
struct list_struct tim_ic_capture_callback_list = {.next = NULL, .prev = NULL};;
struct list_struct tim_period_elapsed_callback_list = {.next = NULL, .prev = NULL};;

/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	struct list_struct *p_list = NULL;
	struct tim_node *p_tim = NULL;
	const struct ca_device *p_dev = NULL;

	p_list = &tim_ic_capture_callback_list;
	while(list_is_last(p_list, &tim_ic_capture_callback_list) != 1)
	{
	  p_tim = (struct tim_node*)p_list->next;  
		p_dev = p_tim->dev;
		
		if((NULL == p_dev)||(NULL == p_dev->bus)||(NULL == p_dev->bus->handler))
		{
			p_list = p_list->next;
			continue;
		}
		
		if(((NULL != p_dev->bus) && (p_dev->bus->handler == (void *)htim)) || (p_tim->handler == htim))
		{
			if(NULL != p_tim->callback)
			{
				p_tim->callback(p_dev, (void *)htim);
			}

			break;
		}

		p_list = p_list->next;
	}
}
/******************************************************************************
 * @Function: tim_ic_capture_register_callback
 * @Description: 注册输入捕获相关的回调函数
 * @Input: struct ca_bus,tim_ic_capture_callback
 * @Output: None
 * @Return: int32_t
 * @Others: None
*******************************************************************************/
int32_t tim_ic_capture_register_callback(const struct ca_device *dev, void *handler, tim_callback callback)
{
	struct tim_node *p_tim = NULL;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == callback))
	{
		return -1;
	}
	
	p_tim = (struct tim_node *)mem_block_alloc(sizeof(struct tim_node));
	if(NULL == p_tim)
	{
		return -1;
	}

	memset(p_tim, 0, sizeof(struct tim_node));

	
	if((NULL == tim_ic_capture_callback_list.next) && (NULL == tim_ic_capture_callback_list.prev))
	{
		list_head_init(&tim_ic_capture_callback_list);
	}

	p_tim->dev = dev;
	p_tim->handler = handler;
	p_tim->callback = callback;
	
	list_add_tail(&p_tim->node, &tim_ic_capture_callback_list);
	

	return 0;
}

void MX_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	struct list_struct *p_list = NULL;
	struct tim_node *p_tim = NULL;
	const struct ca_device *p_dev = NULL;

	p_list = &tim_period_elapsed_callback_list;
	while(list_is_last(p_list, &tim_period_elapsed_callback_list) != 1)
	{
	  p_tim = (struct tim_node*)p_list->next;  
		p_dev = p_tim->dev;
		
		if((NULL == p_dev)||(NULL == p_dev->bus)||(NULL == p_dev->bus->handler))
		{
			p_list = p_list->next;
			continue;
		}
		
		if(((NULL != p_dev->bus) && (p_dev->bus->handler == (void *)htim)) || (p_tim->handler == htim))
		{
			if(NULL != p_tim->callback)
			{
				p_tim->callback(p_dev, (void *)htim);
			}

			break;
		}

		p_list = p_list->next;
	}
}

int32_t tim_period_elapsed_register_callback(const struct ca_device *dev, void *handler, tim_callback callback)
{
	struct tim_node *p_tim = NULL;
	
	if((NULL == dev)||(NULL == callback))
	{
		return -1;
	}

	if((NULL == handler) && (NULL == dev->bus))
	{
		return -1;
	}
	
	p_tim = (struct tim_node *)mem_block_alloc(sizeof(struct tim_node));
	if(NULL == p_tim)
	{
		return -1;
	}

	memset(p_tim, 0, sizeof(struct tim_node));

	
	if((NULL == tim_period_elapsed_callback_list.next) && (NULL == tim_period_elapsed_callback_list.prev))
	{
		list_head_init(&tim_period_elapsed_callback_list);
	}

	p_tim->dev = dev;
	p_tim->handler = handler;
	p_tim->callback = callback;
	
	list_add_tail(&p_tim->node, &tim_period_elapsed_callback_list);
	

	return 0;
}



/******************************************************************************
 * @Function: TIM_Capture_START
 * @Description: 开启定时器输入捕获
 * @Input: struct ca_bus,int8_t
 * @Output: None
 * @Return: int8_t
 * @Others: polarity 设置的捕获极性
*******************************************************************************/

int32_t tim_capture_start(const struct ca_bus *bus, TIM_IC_INT_TYPE polarity)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}
	
	__HAL_TIM_SET_COUNTER((TIM_HandleTypeDef *)bus->handler, 0); //将计数值清零

	switch(polarity)
	{
		case TIM_IC_RISING:
			__HAL_TIM_SET_CAPTUREPOLARITY((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4, TIM_INPUTCHANNELPOLARITY_RISING); 
			break;
		case TIM_IC_FALLING:
			__HAL_TIM_SET_CAPTUREPOLARITY((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4, TIM_INPUTCHANNELPOLARITY_FALLING); 
			break;
		default:
			__HAL_TIM_SET_CAPTUREPOLARITY((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4, TIM_INPUTCHANNELPOLARITY_BOTHEDGE); 
			break;
	}
	
	if(HAL_OK != HAL_TIM_IC_Start_IT((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4))
  {
		return -1;
	}
	
	return 0;
}
/******************************************************************************
 * @Function: TIM_Capture_STOP
 * @Description: 输入捕获结束
 * @Input: struct ca_bus
 * @Output: None
 * @Return: int8_t
 * @Others: None
*******************************************************************************/
int32_t tim_capture_stop(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	if(HAL_OK != HAL_TIM_IC_Stop_IT((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4))
  {
		return -1;
	}
	
	return 0;
}
/******************************************************************************
 * @Function: TIM_ReadCapturedValue(
 * @Description: 读取捕获值
 * @Input: struct ca_bus
 * @Output: None
 * @Return: int32_t
 * @Others: None
*******************************************************************************/
int32_t tim_capture_read(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	return HAL_TIM_ReadCapturedValue((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4);
}

//启动pwm输出，duty_cycle占空比
int32_t tim_pwm_start(const struct ca_bus *bus, float duty_cycle, uint32_t pulsesNum)
{
	uint32_t pulse = 0;
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	pulse = (duty_cycle * __HAL_TIM_GET_AUTORELOAD((TIM_HandleTypeDef *)bus->handler));

	__HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4, pulse);
	
  if(HAL_OK != HAL_TIM_PWM_Start((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4))
  {
		return -1;
	}

	return 0;
}


int32_t tim_pwm_stop(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}
	
	if(HAL_OK != HAL_TIM_PWM_Stop((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4))
  {
		return -1;
	}

	return 0;
}

//设置pwm输出占空比，duty_cycle 单位百分比
int32_t tim_pwm_set_duty_cycle(const struct ca_bus *bus, uint32_t duty_cycle)
{
	uint32_t pulse = 0;
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	pulse = (duty_cycle * __HAL_TIM_GET_AUTORELOAD((TIM_HandleTypeDef *)bus->handler))/100;
	__HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *)bus->handler, bus->bus_addr * 4, pulse);
	
	return 0;
}

int32_t tim_etr_start(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	__HAL_TIM_SET_COUNTER((TIM_HandleTypeDef *)bus->handler, 0); //将计数值清零

	if(HAL_OK != HAL_TIM_Base_Start((TIM_HandleTypeDef *)bus->handler))
  {
		return -1;
	}

	return 0;
}

int32_t tim_get_counter(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	return __HAL_TIM_GET_COUNTER((TIM_HandleTypeDef *)bus->handler); 
}

int32_t tim_set_counter(const struct ca_bus *bus,uint32_t counter)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	return __HAL_TIM_SET_COUNTER((TIM_HandleTypeDef *)bus->handler, counter); //将计数值清零; 
}

int32_t tim_get_period(const struct ca_bus *bus)
{
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	return __HAL_TIM_GET_AUTORELOAD((TIM_HandleTypeDef *)bus->handler); 
}


int32_t tim_period_elapsed_start(const struct ca_bus *bus)
{
	int8_t ret = 0;
	
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}
	ret = HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *)bus->handler);  
	return ret;
}

