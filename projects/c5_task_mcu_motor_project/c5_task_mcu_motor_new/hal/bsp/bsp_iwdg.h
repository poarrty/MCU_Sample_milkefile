/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_pwm.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: pwm operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_IWDG_H
#define __BSP_IWDG_H
#include <board.h>

int iwdg_feed(void);
int iwdg_write_timeout(uint16_t time);
#endif
