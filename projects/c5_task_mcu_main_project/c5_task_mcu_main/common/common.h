/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: common.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 09:30:17
 * @Description: 公共接口入口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include "stdint.h"
#include "stddef.h"

int      common_init(void);
int      common_DeInit(void);
int      utils_truncate_number_uint(uint32_t *number, uint32_t min, uint32_t max);
void     utils_step_towards_uint(uint32_t *value, uint32_t goal, uint32_t step);
int      utils_truncate_number_int(int *number, int min, int max);
void     utils_step_towards_int(int *value, int goal, int step);
int      utils_truncate_number_float(float *number, float min, float max);
void     utils_step_towards_float(float *value, float goal, float step);
uint16_t average_get(uint16_t *in_data, uint8_t len, uint8_t del_len);
void     bubble_sort(uint16_t *data, uint8_t n);
#endif
