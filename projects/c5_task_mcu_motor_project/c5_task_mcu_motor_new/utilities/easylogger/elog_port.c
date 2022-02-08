/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include "board.h"
#include "rtc.h"
#include "usart.h"
#include "elog.h"
FILE *f_log = (FILE *) &f_log;

extern osMutexId_t mutex_elog;

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */

    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    /* add your code here */

    /*
    ///< 锟斤拷印锟斤拷 ELOG 锟斤拷锟斤拷
    //fprintf(f_log, "%.*s", size, log);
    */

    ///< 锟斤拷印锟斤拷 shell 锟斤拷锟斤拷
    shellWriteEndLine(&shell, (char *) log, size);

    /*
    uint8_t *data;
    data = (uint8_t *)log;
    //while(ELOG_USART_H.gState == HAL_UART_STATE_BUSY_TX);
    HAL_UART_Transmit_DMA(&ELOG_USART_H, data, size);
    */
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    /* add your code here */
    //__set_PRIMASK(1);

    // osKernelLock();

    /*
    if(__get_IPSR() || mutex_elog == NULL)
        ;//__set_PRIMASK(1);
    else
        osMutexAcquire(mutex_elog, osWaitForever);
    */

    if (mutex_elog != NULL) {
        osMutexAcquire(mutex_elog,
                       osWaitForever);  ///< 锟斤拷锟叫讹拷锟叫碉拷锟矫ｏ拷锟斤拷直锟接凤拷锟斤拷
                                        ///< err, 锟斤拷影锟斤拷使锟斤拷
    }
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    /* add your code here */
    //__set_PRIMASK(0);

    // osKernelUnlock();

    /*
    if(__get_IPSR() || mutex_elog == NULL)
        ;//__set_PRIMASK(0);
    else
        osMutexRelease(mutex_elog);
    */

    if (mutex_elog != NULL) {
        osMutexRelease(mutex_elog);  ///< 锟斤拷锟叫讹拷锟叫碉拷锟矫ｏ拷锟斤拷直锟接凤拷锟斤拷
                                     ///< err, 锟斤拷影锟斤拷使锟斤拷
    }
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    static char     buff[128];
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    /* add your code here */

    /*
    uint32_t ts = HAL_GetTick();
    sprintf(buff, "%d", ts);
    */

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    sprintf(buff, "%d-%d-%d %02d:%02d:%02d.%03d", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds, 0);

    return buff;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    /* add your code here */
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    /* add your code here */
    return "";
}

void elog_hook(const char *expr, const char *func, size_t line) {
    elog_a("elog", "(%s) has assert failed at %s:%ld.", expr, func, line);
}
