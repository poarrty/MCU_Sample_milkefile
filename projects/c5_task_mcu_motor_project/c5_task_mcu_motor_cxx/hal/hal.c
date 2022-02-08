/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: hal.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Hardware abstraction layer interface entry
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_usart.h"
#include "drv_can.h"
#define LOG_TAG "hal"
#include "elog.h"
/******************************************************************************
 * @Function: hal_init
 * @Description: 硬件抽象层初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int hal_init(void) {
    drv_usart_init();
    /* cputime(<ms) init */
    cortexm_cputime_init();
    /* can init */
    rt_hw_can_init();
    return 0;
}

/******************************************************************************
 * @Function: hal_DeInit
 * @Description: 硬件抽象层反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int hal_DeInit(void) {
    return 0;
}
