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
 * File: $Id: porttimer.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "cmsis_os.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

#include "tim.h"
#include "stdio.h"

BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{ 
    HAL_TIM_Base_Stop_IT(&MODBUS_TIM_TIMEOUT);
    __HAL_TIM_CLEAR_IT(&MODBUS_TIM_TIMEOUT, TIM_IT_UPDATE);
    
   __HAL_TIM_SET_AUTORELOAD(&MODBUS_TIM_TIMEOUT, usTim1Timerout50us-1);
    
    return true;
   
}

void vMBPortTimersEnable()
{
    HAL_TIM_Base_Stop_IT(&MODBUS_TIM_TIMEOUT);
    __HAL_TIM_CLEAR_IT(&MODBUS_TIM_TIMEOUT, TIM_IT_UPDATE);
    __HAL_TIM_SET_COUNTER(&MODBUS_TIM_TIMEOUT, 0);
 
    HAL_TIM_Base_Start_IT(&MODBUS_TIM_TIMEOUT);
}

void vMBPortTimersDisable(  )
{   
    HAL_TIM_Base_Stop_IT(&MODBUS_TIM_TIMEOUT);
    __HAL_TIM_CLEAR_IT(&MODBUS_TIM_TIMEOUT, TIM_IT_UPDATE);
    __HAL_TIM_SET_COUNTER(&MODBUS_TIM_TIMEOUT, 0);
}

extern osThreadId_t task_pal_modbusHandle;

void modbusTimerIRQ(void)
{   
    HAL_TIM_Base_Stop_IT(&MODBUS_TIM_TIMEOUT);
    
    BOOL bTaskWoken = FALSE;

    bTaskWoken = pxMBPortCBTimerExpired();
    
    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
    
    if(pdTRUE && task_pal_modbusHandle!= NULL)
        osThreadFlagsSet(task_pal_modbusHandle, 0x0001U);
    
}
