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
#include "main.h"
#include "devices.h"
#include "define.h"
#include "sys_list.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>
#include "adc_core.h"



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

uint16_t TIM_ADC_GET(const struct ca_bus *bus)
{
	uint16_t ret = 0;
	
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}
	
  HAL_ADC_Start((ADC_HandleTypeDef *)bus->handler);     
	HAL_ADC_PollForConversion((ADC_HandleTypeDef *)bus->handler, 50); 
	ret = HAL_ADC_GetValue((ADC_HandleTypeDef *)bus->handler);
  return ret; 
}

uint16_t adc_read(const struct ca_bus *bus, uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel;                                         /* 通道 */
	sConfig.Rank = 1;                              
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;                  /* 采样时间 */
	if (HAL_ADC_ConfigChannel((ADC_HandleTypeDef *)bus->handler, &sConfig) != HAL_OK)             
	{
		Error_Handler();
	}
	HAL_ADC_Start((ADC_HandleTypeDef *)bus->handler);
	HAL_ADC_PollForConversion((ADC_HandleTypeDef *)bus->handler, HAL_MAX_DELAY);
	return (uint16_t)HAL_ADC_GetValue((ADC_HandleTypeDef *)bus->handler);
}

#ifdef __cplusplus
extern "C" {
#endif

