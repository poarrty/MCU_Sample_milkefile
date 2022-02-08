/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_inputcap.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: input capture
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_INPUTCAP_H
#define __BSP_INPUTCAP_H

#include "board.h"
#include "drv_tim_ic.h"

struct input_cap_freq {
    uint8_t  state;
    uint8_t  state_switch;
    uint32_t first_edge_trigger_time;
    uint32_t second_edge_trigger_time;
    uint32_t curr_edge_trigger_time;
    uint32_t time_interval;
    uint32_t input_freq;
};

void input_cap_freq_handler(void *param);

#endif
