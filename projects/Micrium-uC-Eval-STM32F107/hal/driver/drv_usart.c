/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: drv_usart.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:29:28
 * @Description: USART底层驱动接口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "drv_usart.h"
#include "SEGGER_RTT.h"
#include "crc16.h"
#include "log.h"
#include "shell_port.h"
#include <cmsis_os.h>
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>

#define LOG_TAG "drv_usart"
#include "elog.h"

#define Terminal0_write(data, len) HAL_UART_Transmit(&huart5, (uint8_t *) data, len, 1000)
#define Terminal1_write(data, len) SEGGER_RTT_Write(0, data, len)

usart_info_stu_t usart3;
usart_info_stu_t usart4;

/******************************************************************************
 * @Function: drv_usart_init
 * @Description: USART底层接口初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void drv_usart_init(void) {}

/******************************************************************************
 * @Function: fputc
 * @Description: 重定义printf需要
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
#if __LOG_EN__
int _write(int file, char *data, int len) {
    HAL_StatusTypeDef status;

    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    status = Terminal0_write(data, len);
    Terminal1_write(data, len);

    return ((status == HAL_OK) ? len : 0);
}
#endif

/******************************************************************************
 * @Function: drv_uart5_rxne_irq_callbake
 * @Description: UART5接收中断服务函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void drv_uart5_rxne_irq_callbake(UART_HandleTypeDef *huart) {
    letter_shell_recv_irq_handler();
}

/******************************************************************************
 * @Function: drv_uart5_send_buffer
 * @Description: USART5发送指定长度的数据
 * @Input: buf：要发送的数据地址
 *         len：数据长度
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {uint8_t} *buf
 * @param {uint16_t} len
 *******************************************************************************/
void drv_uart5_send_buffer(uint8_t *buf, uint16_t len) {
    Error_Check_Callbake(HAL_UART_Transmit(&huart5, buf, len, 1000));
}

/******************************************************************************
 * @Function: HAL_UART_RxCpltCallback
 * @Description: 串口接收中断回调函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART5) {
        drv_uart5_rxne_irq_callbake(huart);
    }
}
