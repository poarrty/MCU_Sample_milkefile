/**
 * @file shell_port.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include "usart.h"
#include "shell.h"

extern Shell shell;

void userShellInit(void);
void letter_shell_recv_irq_handler(UART_HandleTypeDef *huart);
void letter_shell_recv_handler(void);

#endif
