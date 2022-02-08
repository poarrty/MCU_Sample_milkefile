/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Protocol abstraction layer interface entry header file
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __PAL_H__
#define __PAL_H__
#include "board.h"

int  pal_init(void);
int  pal_DeInit(void);
void g_publisher_init(void);
void g_subscrption_init(void);

#endif
