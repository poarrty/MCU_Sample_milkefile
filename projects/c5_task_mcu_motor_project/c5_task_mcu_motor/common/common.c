/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: common.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 09:30:17
 * @Description: 公共接口入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "common_def.h"
#include "sys.h"
#include "cm_backtrace.h"
/******************************************************************************
 * @Function: common_init
 * @Description: 公共接口初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int common_init(void) {
    common_def_init();
    sys_init();
    cm_backtrace_init("c5_task_mcu_motor", MODEL_NAME, SOFTWARE_VERSION);
    return 0;
}

/******************************************************************************
 * @Function: common_DeInit
 * @Description: 公共接口反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int common_DeInit(void) {
    return 0;
}
