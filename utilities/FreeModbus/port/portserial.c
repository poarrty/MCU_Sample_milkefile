/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: portserial.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "port.h"
#include "mbport.h"

/* ----------------------- STM32F407 includes -------------------------------*/
#include "stm32f4xx_ll_usart.h"
#include "stdio.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Static variables ---------------------------------*/
uint8_t modbufUartRxBuff;
uint8_t modbufUartTxBuff;

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{  
    if(xRxEnable) 
    {        
        HAL_UART_Receive_IT(&MODBUS_PORT, &modbufUartRxBuff, 1);
    } 
    else 
    {    
        HAL_UART_AbortReceive_IT(&MODBUS_PORT);
    }
    
    if(xTxEnable) 
    {    
        //485则需要做数据方向控制
        
        //发送中断
        LL_USART_EnableIT_TXE(MODBUS_PORT.Instance);
        
        //启动发送
        pxMBFrameCBTransmitterEmpty();
    } 
    else 
    {
        //485需要做数据方向控制
    }  
    
  
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    /* Performed by CubeMX */
    return TRUE; 
}

void vMBPortSerialClose( void )
{
    //串口一直开启
}


BOOL xMBPortSerialPutByte(CHAR ucByte)
{  
    modbufUartTxBuff = ucByte;
    HAL_UART_Transmit_IT(&MODBUS_PORT, (uint8_t *)&modbufUartTxBuff, 1);
        
    return TRUE;
       
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
    *pucByte = modbufUartRxBuff;
    return TRUE;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{    
    BOOL bTaskWoken = FALSE;
        
    if(huart->Instance == MODBUS_PORT.Instance)
    {        
        bTaskWoken = pxMBFrameCBTransmitterEmpty();    
    }

    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );  
       
}
*/

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   BOOL bTaskWoken = FALSE;
        
    if(huart->Instance == MODBUS_PORT.Instance)
    {        
        bTaskWoken = pxMBFrameCBByteReceived();
   
        HAL_UART_Receive_IT(&MODBUS_PORT, &modbufUartRxBuff, 1);     
    }
    
    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE ); 
}
*/


