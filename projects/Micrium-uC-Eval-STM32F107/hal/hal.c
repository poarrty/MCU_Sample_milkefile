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
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include "board.h"
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
    /* segger rtt init */
    SEGGER_RTT_Init();
    return 0;
}

/* cputime(<ms) init */
int cortexm_cputime_init(void);
INIT_BOARD__EXPORT(cortexm_cputime_init);

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
