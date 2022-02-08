/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: common.c
 * @Author: Chen yuliang
 * @Version: V1.0
 * @Date: 2021-05-14 09:30:17
 * @Description: 公共接口入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "common_def.h"

/******************************************************************************
 * @Function: common_init
 * @Description: 公共接口初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
char log_buff[512];
char temp_buff[16];

int common_init(void) {
    common_def_init();
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
