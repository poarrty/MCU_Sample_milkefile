/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_adc.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: adc operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include <board.h>
#include "drv_gpio.h"

#define REFER_VOLTAGE 3300      /* reference voltage:3.3V,unit:mv*/
#define CONVERT_BITS  (1 << 12) /* bit width:12bit */

enum { ID_PUSH_ROD_MOTOR_1_ADC = 0, ID_PUSH_ROD_MOTOR_2_ADC };

typedef struct {
    rt_device_t dev_handle;
    char        dev_name[RT_NAME_MAX];
    uint8_t     channel; /* 0-19*/
    uint8_t     used_flag;
} adc_obj_st;

int adc_read(uint8_t adc_obj_id, uint16_t *pulse);
int adc_enable(uint8_t adc_obj_id);
int adc_disable(uint8_t adc_obj_id);

#endif
