/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_usart.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:29:28
 * @Description: USART底层驱动接口头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include "usart.h"
#include <stdint.h>

//串口缓冲区最大长度
#ifndef USART_BUFF_MAX_SIZE
#define USART_BUFF_MAX_SIZE 16
#endif

typedef struct {
    //接收长度
    uint8_t rx_len;
    //接收数组
    uint8_t rx_buff[USART_BUFF_MAX_SIZE];
    //接收数据
    uint8_t rx_data;
    //前一次接收的数据
    uint8_t rx_pre_data;
    //是否接收到有效的数据
    uint8_t rx_flag;
    uint8_t rx_printf_len;
    uint8_t rx_printf_buff[USART_BUFF_MAX_SIZE];
} usart_info_stu_t;

extern usart_info_stu_t usart3;
extern usart_info_stu_t usart4;

void drv_usart_init(void);
void drv_usart1_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart3_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart4_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart5_send_buffer(uint8_t *buf, uint16_t len);
void drv_usart_irq_handler_callbake(UART_HandleTypeDef *huart);

#endif
