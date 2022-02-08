/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 09:31:03
 * @Description: 功能抽象层接口入口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __FAL_H__
#define __FAL_H__

#include <stdio.h>

int fal_init(void);
int fal_DeInit(void);

int set_local_rtc(uint16_t year, uint16_t moon, uint16_t date, uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);
#endif
