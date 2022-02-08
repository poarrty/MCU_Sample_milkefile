/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-4-29
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-04 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#define taskDISABLE_INTERRUPTS() portDISABLE_INTERRUPTS()

#include "fal_misc.h"
#include "cmsis_os.h"
#include "log.h"
#include "rtc.h"
#include "shell.h"
#include "shell_port.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "flashdb.h"

#define LOG_TAG "fal_misc"
// #include "elog.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

SFU_RESET_IdTypeDef e_wakeup_source_id = SFU_RESET_UNKNOWN;

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern struct fdb_kvdb kvdb;
extern struct fdb_blob blob;
/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void SFU_BOOT_ManageResetSources(void);

/*****************************************************************
 * 函数定义
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       fal_misc_init
 * Description:    初始化 fal_misc
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_misc_init(void) {
    // bsp_thtb_inner_init();

    SFU_BOOT_ManageResetSources();

    return 0;
}

/*****************************************************************/
/**
 * Function:       fal_misc_deInit
 * Description:    释放 fal_misc 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int fal_misc_deInit(void) {
    return 0;
}

extern TIM_HandleTypeDef htim5;
unsigned long            getRunTimeCounterValue(void) {
    unsigned long ts = 0;
    uint32_t      ts_l;

    HAL_SuspendTick();

    ts = HAL_GetTick();

    ts_l = __HAL_TIM_GET_COUNTER(&htim5);

    ts = ts * 10 + ts_l / 100;

    HAL_ResumeTick();

    return ts;
}

BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *const pcHeader =
        "   State   Prior   Stack   "
        "ID\r\n**********************************************************\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
    "Task" so the column lines up with the centre of the heading. */
    configASSERT(configMAX_TASK_NAME_LEN > 3);

    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskList(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

/*-----------------------------------------------------------*/

#if (configGENERATE_RUN_TIME_STATS == 1)

BaseType_t prvRunTimeStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *const pcHeader =
        "   Time(*100us)    "
        "Time(%)\r\n*******************************************************"
        "\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Pad the string "task" with however many bytes necessary to make it the
    length of a task name.  Minus three for the null terminator and half the
    number of characters in "Task" so the column lines up with the centre of
    the heading. */
    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskGetRunTimeStats(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

#endif /* configGENERATE_RUN_TIME_STATS */
/*-----------------------------------------------------------*/

char cStringBuffer[2048] = {0};

int show_task_state(void) {
    uint32_t ts1, ts2;
    (void) ts1;
    (void) ts2;

    ts1 = HAL_GetTick();
    prvTaskStatsCommand(cStringBuffer, 2048, NULL);
    ts2 = HAL_GetTick();

    printf("%s\r\n", cStringBuffer);
    // printf("start[%d] end[%d] len[%d]\r\n", ts1, ts2, strlen(cStringBuffer));

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_state, show_task_state, show_task_state);

int show_task_cpu(void) {
    uint32_t ts1, ts2;
    (void) ts1;
    (void) ts2;

    ts1 = HAL_GetTick();
    prvRunTimeStatsCommand(cStringBuffer, 2048, NULL);
    ts2 = HAL_GetTick();
    printf("%s\r\n", cStringBuffer);
    // printf("start[%d] end[%d] len[%d]\r\n", ts1, ts2, strlen(cStringBuffer));
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_cpu, show_task_cpu, show_task_cpu);

int show_float(int vali, float valf) {
    printf("%d %f\r\n", vali, valf);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_float, show_float, show_float);

uint8_t read_coredown(void) {
    fdb_kv_get_blob(&kvdb, "hardfault", fdb_blob_make(&blob, cStringBuffer, sizeof(cStringBuffer)));

    printf("\r\nhardfault msg:\r\n");
    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "---\r\n");

    printf("%s", cStringBuffer);

    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "---\r\n");

    memset(cStringBuffer, 0, sizeof(cStringBuffer));
    fdb_kv_get_blob(&kvdb, "wwdgtimeout", fdb_blob_make(&blob, cStringBuffer, sizeof(cStringBuffer) + 1));

    printf("\r\nwwdgtimeout msg:\r\n");
    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "---\r\n");

    printf("%s", cStringBuffer);

    printf("\r\n");
    printf(
        "----------------------------------------------------------------------"
        "---\r\n");

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), read_coredown, read_coredown, read_coredown);

extern char *vTaskName(void);

void wwdgtimeout_coredown_save(void) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    // HAL_WWDG_Refresh(&hwwdg);
    memset(cStringBuffer, 0, sizeof(cStringBuffer));

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    sprintf(cStringBuffer, "[wwdg timeout at %d-%d-%d %02d:%02d:%02d]\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
            time.Seconds);

    // prvRunTimeStatsCommand(cStringBuffer+strlen(cStringBuffer),
    // 2048-strlen(cStringBuffer), NULL);
    sprintf(cStringBuffer + strlen(cStringBuffer), "Last running task: %s", vTaskName());

    cStringBuffer[strlen(cStringBuffer)] = '\0';

    fdb_kv_set_blob(&kvdb, "wwdgtimeout", fdb_blob_make(&blob, cStringBuffer, strlen(cStringBuffer) + 1));

    // HAL_WWDG_Refresh(&hwwdg);
    // sys_paras_wwdg_to_inc();
}

void hardfault_coredown_save(void) {
    // HAL_WWDG_Refresh(&hwwdg);

    cStringBuffer[strlen(cStringBuffer)] = '\0';

    // sys_paras_write_coredown(cStringBuffer, strlen(cStringBuffer) + 1);
    fdb_kv_set_blob(&kvdb, "hardfault", fdb_blob_make(&blob, cStringBuffer, strlen(cStringBuffer) + 1));
    // HAL_WWDG_Refresh(&hwwdg);
    // sys_paras_hardfault_inc();
}

void hardfault_coredown_append(const char *format, ...) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    static uint8_t flag = 0;

    if (flag == 0) {
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
        sprintf(cStringBuffer, "[Hardfault at %d-%d-%d %02d:%02d:%02d]\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
                time.Seconds);
        flag = 1;
    }

    // HAL_WWDG_Refresh(&hwwdg);

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* 实现数据输出 */
    vsprintf(cStringBuffer + strlen(cStringBuffer), format, args);

    va_end(args);
}

// void HAL_WWDG_EarlyWakeupCallback (WWDG_HandleTypeDef * hwwdg)
// {
//  HAL_WWDG_Refresh(hwwdg);

//  wwdgtimeout_coredown_save();

//  while (1);
// }

/**
 * @brief  Return the reset source  detected after a reboot. The related flag is
 * reset at the end of this function.
 * @param  peResetpSourceId: to be filled with the detected source of reset
 * @note   In case of multiple reset sources this function return only one of
 * them. It can be improved returning and managing a combination of them.
 * @retval SFU_SUCCESS if successful, SFU_ERROR otherwise
 */
void SFU_LL_SECU_GetResetSources(SFU_RESET_IdTypeDef *peResetpSourceId) {
    /* Check if the last reset has been generated from a Watchdog exception */
    if ((__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) || (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)) {
        *peResetpSourceId = SFU_RESET_WDG_RESET;

    }

    /* Check if the last reset has been generated from a Low Power reset */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
        *peResetpSourceId = SFU_RESET_LOW_POWER;

    }

    /* Check if the last reset has been generated from a Software reset  */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
        *peResetpSourceId = SFU_RESET_SW_RESET;

    }
    /* Check if the last reset has been generated from a Hw pin reset  */
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        *peResetpSourceId = SFU_RESET_HW_RESET;

    }
    /* Unknown */
    else {
        *peResetpSourceId = SFU_RESET_UNKNOWN;
    }
}

/**
 * @brief  Clear the reset sources. This function should be called after the
 * actions on the reset sources has been already taken.
 * @param  none
 * @note   none
 * @retval none
 */
void SFU_LL_SECU_ClearResetSources() {
    /* Clear reset flags  */
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

#define TRACE LOG_DEBUG

/**
 * @brief  Manage the  the Reset sources, and if the case store the error for
 * the next steps
 * @param  None
 * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
 */
static void SFU_BOOT_ManageResetSources(void) {
    // SFU_ErrorStatus     e_ret_status = SFU_SUCCESS;
    // SFU_RESET_IdTypeDef e_wakeup_source_id = SFU_RESET_UNKNOWN;
    // SFU_EXCPT_IdTypeDef e_exception;

    /* Check the wakeup sources */
    SFU_LL_SECU_GetResetSources(&e_wakeup_source_id);

    switch (e_wakeup_source_id) {
            /*
             * Please note that the example of reset causes handling below is
             * only a basic example to illustrate the way the RCC_CSR flags can
             * be used to do so. It is based on the behaviors we consider as
             * normal and abnormal for the SB_SFU and UserApp example projects
             * running on a Nucleo HW board.
             * Hence this piece of code must systematically be revisited and
             * tuned for the targeted system (software and hardware expected
             * behaviors must be assessed to tune this code).
             *
             * One may use the "uExecID" parameter to determine if the last exec
             * status was in the SB_SFU context or UserApp context to implement
             * more clever checks in the reset cause handling below.
             */

        case SFU_RESET_WDG_RESET:
            TRACE("WARNING: A Reboot has been triggered by a Watchdog reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* This event has to be considered as an error to manage */
            // e_exception = SFU_EXCPT_WATCHDOG_RESET;
            break;

        case SFU_RESET_LOW_POWER:
            TRACE("INFO: A Reboot has been triggered by a LowPower reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_HW_RESET:
            TRACE("INFO: A Reboot has been triggered by a Hardware reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. This is because a Nucleo board offers a RESET
               button triggering the HW reset. But this is strictly related to
               the final system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_BOR_RESET:
            TRACE("INFO: A Reboot has been triggered by a BOR reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_SW_RESET:
            TRACE("INFO: A Reboot has been triggered by a Software reset!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage, also because a sw reset is generated when the
               State Machine forces a Reboot. But this is strictly related to
               the final system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;

        case SFU_RESET_OB_LOADER:
            TRACE(
                "WARNING: A Reboot has been triggered by an Option Bytes "
                "reload!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage, also because an OptionByte loader is called
               after applying some of the security protections (see
               SFU_CheckApplyStaticProtections). But this is strictly related to
               the final system. If needed to be managed as an error please add
               the right error code in the following src code line. Typically we
               may implement a more clever check where we determine if this OB
               reset occurred only once because SB_SFU had to tune the OB
               initially, or if it occurred again after these initial settings.
               If so, we could consider it as an attack. */
            // e_exception = SFU_EXCPT_NONE;
            break;

        default:
            TRACE(
                "WARNING: A Reboot has been triggered by an Unknown reset "
                "source!");
            /* WARNING: This might be generated by an attempted attack, a bug or
               your code! Add your code here in order to implement a custom
               action for this event, e.g. trigger a mass erase or take any
               other  action in order to protect your system, or simply discard
               it if this is expected.
               ...
               ...
            */
            /* In the current implementation this event is not considered as an
               error to manage. But this is strictly related to the final
               system. If needed to be managed as an error
               please add the right error code in the following src code line */
            // e_exception = SFU_EXCPT_NONE;
            break;
    }

    /* Once the reset sources has been managed and a possible error has been
     * set, clear the reset sources */
    SFU_LL_SECU_ClearResetSources();
}

static uint8_t sys_reboot_source(void) {
    printf("\r\n");

    switch (e_wakeup_source_id) {
        case SFU_RESET_WDG_RESET:
            printf("WARNING: A Reboot has been triggered by a Watchdog reset!");

            break;

        case SFU_RESET_LOW_POWER:
            printf("INFO: A Reboot has been triggered by a LowPower reset!");

            break;

        case SFU_RESET_HW_RESET:
            printf("INFO: A Reboot has been triggered by a Hardware reset!");

            break;

        case SFU_RESET_BOR_RESET:
            printf("INFO: A Reboot has been triggered by a BOR reset!");

            break;

        case SFU_RESET_SW_RESET:
            printf("INFO: A Reboot has been triggered by a Software reset!");

            break;

        case SFU_RESET_OB_LOADER:
            printf(
                "WARNING: A Reboot has been triggered by an Option Bytes "
                "reload!");

            break;

        default:
            printf(
                "WARNING: A Reboot has been triggered by an Unknown reset "
                "source!");

            break;
    }

    printf("\r\n");

    return e_wakeup_source_id;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys_reboot_source, sys_reboot_source, sys_reboot_source);

int show_heap_state(void) {
    HeapStats_t heapstate;
    vPortGetHeapStats(&heapstate);
    /* The total heap size currently available - this is the sum of all the free
     * blocks, not the largest block that can be allocated. */
    printf("xAvailableHeapSpaceInBytes      : %d\r\n", heapstate.xAvailableHeapSpaceInBytes);
    /* The maximum size, in bytes, of all the free blocks within the heap at the
     * time vPortGetHeapStats() is called. */
    printf("xSizeOfLargestFreeBlockInBytes  : %d\r\n", heapstate.xSizeOfLargestFreeBlockInBytes);
    /* The minimum size, in bytes, of all the free blocks within the heap at the
     * time vPortGetHeapStats() is called. */
    printf("xSizeOfSmallestFreeBlockInBytes : %d\r\n", heapstate.xSizeOfSmallestFreeBlockInBytes);
    /* The number of free memory blocks within the heap at the time
     * vPortGetHeapStats() is called. */
    printf("xNumberOfFreeBlocks             : %d\r\n", heapstate.xNumberOfFreeBlocks);
    /* The minimum amount of total free memory (sum of all free blocks) there
     * has been in the heap since the system booted. */
    printf("xMinimumEverFreeBytesRemaining  : %d\r\n", heapstate.xMinimumEverFreeBytesRemaining);
    /* The number of calls to pvPortMalloc() that have returned a valid memory
     * block. */
    printf("xNumberOfSuccessfulAllocations  : %d\r\n", heapstate.xNumberOfSuccessfulAllocations);
    /* The number of calls to vPortFree() that has successfully freed a block of
     * memory. */
    printf("xNumberOfSuccessfulFrees        : %d\r\n", heapstate.xNumberOfSuccessfulFrees);
    printf("\r\n");
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_heap_state, show_heap_state, show_heap_state);

#ifdef __cplusplus
}
#endif

/* @} FAL_MISC */
/* @} Robot_FAL */
