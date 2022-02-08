/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: sys.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-06-03 19:03:05
 * @Description: 系统相关接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __SYS_H__
#define __SYS_H__

#include <stdint.h>

typedef struct {
    uint32_t device_id;
    uint32_t hal_version;
    uint32_t revision_id;
    uint32_t uid[3];
    uint32_t sys_clk;
    uint32_t hclk;
    uint32_t pclk1;
    uint32_t pclk2;
} sys_info_stu_t;

void sys_init(void);
void sys_info_display(void);

#endif
