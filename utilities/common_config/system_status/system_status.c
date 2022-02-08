#include "main.h"
#include "cmsis_os.h"
#include "system_status.h"
#include "system_status_config.h"
#include "shell.h"
#include "flashdb.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "stdint.h"

#ifdef HAL_RTC_MODULE_ENABLED
#include "rtc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 私有宏定义
 ******************************************************************/
#define INTERVAL_RUN_TIME_COMMAND 0x01
#define TOTAL_RUN_TIME_COMMAND    0x02

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
sys_rcc_flag_info_stu_t sys_rcc_flag_info = {
    0,
    {"Pin reset!", "Power reset!", "Soft reset!", "IWDG reset!", "WWDG reset!", "Low power reset!", "Low Vol reset!"}};

#ifndef STRING_RAM_EXT
char cStringBuffer[2048] = {0};
#endif
uint16_t cpu_show_interval_sec = DEFAULT_HTOP_INTERVAL;

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/
const osThreadAttr_t task_htop_attributes = {.name = "task_htop", .priority = (osPriority_t) osPriorityNormal, .stack_size = 256 * 4};
/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/
extern struct fdb_kvdb kvdb;

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/
static void task_htop_run(void *argument);
__weak void check_rcc_flag(void);

#if ((configGENERATE_RUN_TIME_STATS == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS > 0) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))
BaseType_t prvRunTimeStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString, uint8_t Commamd);
#endif
/*****************************************************************
 * 函数定义
 ******************************************************************/

int task_system_status_init(void) {
    check_rcc_flag();
    osThreadNew(task_htop_run, NULL, &task_htop_attributes);

    return 0;
}

static void task_htop_run(void *argument) {
    osDelay(1000);
    while (1) {
        if (cpu_show_interval_sec) {
            prvRunTimeStatsCommand(cStringBuffer, sizeof(cStringBuffer), NULL, INTERVAL_RUN_TIME_COMMAND);
            SYS_PRINT("In %d Sec\r\n%s\r\n", cpu_show_interval_sec, cStringBuffer);
        } else {
            osDelay(1000);
        }
    }
}

/*系统参数*/
__weak uint8_t sys(void) {
    uint32_t on_time;

#ifdef HAL_RTC_MODULE_ENABLED
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
#endif

    struct fdb_blob blob;
    uint32_t        hardfault_count;

    on_time = HAL_GetTick();

    SYS_PRINT("\r\n");
#ifdef SOFTWARE_VERSION
    SYS_PRINT("Version                     : %s\r\n", SOFTWARE_VERSION);
#endif
#ifdef MODEL_NAME
    SYS_PRINT("Model Name                  : %s\r\n", MODEL_NAME);
#endif
#ifdef CURRENT_BRANCH
    SYS_PRINT("Git Branch                  : %s\r\n", CURRENT_BRANCH);
#endif
#ifdef COMMIT_ID
    SYS_PRINT("Git Commit Id               : %s\r\n", COMMIT_ID);
#endif
#ifdef BUILD_TIME
    SYS_PRINT("Bulid Time                  : %s\r\n", BUILD_TIME);
#endif
    SYS_PRINT("Sys on time                 : %02ld:%02ld:%02ld\r\n", on_time / 1000 / 3600, on_time / 1000 % 3600 / 60,
           on_time / 1000 % 3600 % 60);
#ifdef HAL_RTC_MODULE_ENABLED
    SYS_PRINT("Sys current time            : %d-%d-%d %02d:%02d:%02d.%03ld\r\n", date.Year + 2000, date.Month, date.Date, time.Hours, time.Minutes,
           time.Seconds, (time.SecondFraction - time.SubSeconds) * 4);
#endif
    SYS_PRINT("UID(96 bits)                : 0x%08lx%08lx%08lx \r\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());

    for (size_t i = 0; i < 7; i++) {
        if (sys_rcc_flag_info.flag & (1 << i)) {
            SYS_PRINT("Reset Source                : %s\r\n", sys_rcc_flag_info.info[i]);
        }
    }

    fdb_kv_get_blob(&kvdb, "hardfault_count", fdb_blob_make(&blob, &hardfault_count, sizeof(hardfault_count)));
    if (hardfault_count >= 1000) {
        hardfault_count = 0;
        fdb_kv_set_blob(&kvdb, "hardfault_count", fdb_blob_make(&blob, &hardfault_count, sizeof(hardfault_count)));
    }

    SYS_PRINT("Hardfault count             : %ld\r\n", hardfault_count);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), sys, sys, sys);

__weak void check_rcc_flag(void) {
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        //这是外部RST管脚复位
        sys_rcc_flag_info.flag |= (1 << 0);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
        //这是上电复位
        sys_rcc_flag_info.flag |= (1 << 1);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
        //这是软件复位
        sys_rcc_flag_info.flag |= (1 << 2);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
        //这是独立看门狗复位
        sys_rcc_flag_info.flag |= (1 << 3);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
        //这是窗口看门狗复位
        sys_rcc_flag_info.flag |= (1 << 4);
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
        //这是低功耗复位
        sys_rcc_flag_info.flag |= (1 << 5);
    }
#ifdef RCC_FLAG_BORRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) {
        //这是上电复位或欠压复位
        sys_rcc_flag_info.flag |= (1 << 6);
    }
#endif
    //清除RCC中复位标志
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

extern TIM_HandleTypeDef SYS_HTIM;
unsigned long            getRunTimeCounterValue(void) {
    unsigned long ts = 0;
    uint32_t      ts_l;

    HAL_SuspendTick();

    ts = HAL_GetTick();

    ts_l = __HAL_TIM_GET_COUNTER(&SYS_HTIM);

    ts = ts * 10 + ts_l / 100;

    HAL_ResumeTick();

    return ts;
}

/*系统CPU*/
#if ((configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS > 0))

static char *prvWriteNameToBuffer(char *pcBuffer, const char *pcTaskName) {
    size_t x;

    /* Start by copying the entire string. */
    strcpy(pcBuffer, pcTaskName);

    /* Pad the end of the string with spaces to ensure columns line up when
    printed out. */
    for (x = strlen(pcBuffer); x < (size_t)(configMAX_TASK_NAME_LEN - 1); x++) {
        pcBuffer[x] = ' ';
    }

    /* Terminate. */
    pcBuffer[x] = (char) 0x00;

    /* Return the new end of string. */
    return &(pcBuffer[x]);
}

#endif /* ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) */

#if ((configGENERATE_RUN_TIME_STATS == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS > 0) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))

void vTaskGetIntervalRunTimeStats(char *pcWriteBuffer, uint32_t interval) {
    TaskStatus_t *pxFirstTaskStatusArray;
    TaskStatus_t *pxTaskStatusArray;
    UBaseType_t   uxArraySize, x;
    uint32_t      ulFirstCalTime;
    uint32_t      ulTotalTime, ulStatsAsPercentage;

#if (configUSE_TRACE_FACILITY != 1)
    {
#error configUSE_TRACE_FACILITY must also be set to 1 in FreeRTOSConfig.h to use vTaskGetRunTimeStats().
    }
#endif

    /*
     * PLEASE NOTE:
     *
     * This function is provided for convenience only, and is used by many
     * of the demo applications.  Do not consider it to be part of the
     * scheduler.
     *
     * vTaskGetRunTimeStats() calls uxTaskGetSystemState(), then formats part
     * of the uxTaskGetSystemState() output into a human readable table that
     * displays the amount of time each task has spent in the Running state
     * in both absolute and percentage terms.
     *
     * vTaskGetRunTimeStats() has a dependency on the sprintf() C library
     * function that might bloat the code size, use a lot of stack, and
     * provide different results on different platforms.  An alternative,
     * tiny, third party, and limited functionality implementation of
     * sprintf() is provided in many of the FreeRTOS/Demo sub-directories in
     * a file called printf-stdarg.c (note printf-stdarg.c does not provide
     * a full snprintf() implementation!).
     *
     * It is recommended that production systems call uxTaskGetSystemState()
     * directly to get access to raw stats data, rather than indirectly
     * through a call to vTaskGetRunTimeStats().
     */

    // /* Make sure the write buffer does not contain a string. */
    // *pcWriteBuffer = (char) 0x00;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    uxArraySize = uxTaskGetNumberOfTasks();

    /* Allocate an array index for each task.  NOTE!  If
    configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
    equate to NULL. */
    pxFirstTaskStatusArray = pvPortMalloc(
        uxTaskGetNumberOfTasks() *
        sizeof(TaskStatus_t)); /*lint !e9079 All values returned by pvPortMalloc() have at least the alignment required by the MCU's stack and
                                  this allocation allocates a struct that has the alignment requirements of a pointer. */
    pxTaskStatusArray = pvPortMalloc(
        uxTaskGetNumberOfTasks() *
        sizeof(TaskStatus_t)); /*lint !e9079 All values returned by pvPortMalloc() have at least the alignment required by the MCU's stack and
                                  this allocation allocates a struct that has the alignment requirements of a pointer. */

    if ((pxTaskStatusArray != NULL) && (pxFirstTaskStatusArray != NULL)) {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState(pxFirstTaskStatusArray, uxArraySize, &ulFirstCalTime);

        uint32_t ulFirstRunTimeCounter[uxArraySize];

        osDelay(interval);
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalTime);

        /* Get interval time*/
        ulTotalTime = ulTotalTime - ulFirstCalTime;

        /* For percentage calculations. */
        ulTotalTime /= 100UL;

        /* Make sure the write buffer does not contain a string. */
        *pcWriteBuffer = (char) 0x00;

        /* Avoid divide by zero errors. */
        if (ulTotalTime > 0UL) {
            /* Create a human readable table from the binary data. */
            for (x = 0; x < uxArraySize; x++) {
                /*  Get First Count */
                for (size_t i = 0; i < uxArraySize; i++) {
                    if (strcmp(pxFirstTaskStatusArray[i].pcTaskName, pxTaskStatusArray[x].pcTaskName) == 0) {
                        ulFirstRunTimeCounter[x] = pxFirstTaskStatusArray[i].ulRunTimeCounter;
                        break;
                    }
                }

                /* What percentage of the total run time has the task used?
                                    This will always be rounded down to the nearest integer.
                                    ulTotalRunTimeDiv100 has already been divided by 100. */
                ulStatsAsPercentage = (pxTaskStatusArray[x].ulRunTimeCounter - ulFirstRunTimeCounter[x]) / ulTotalTime;
                /* Write the task name to the string, padding with
                spaces so it can be printed in tabular form more
                easily. */
                pcWriteBuffer = prvWriteNameToBuffer(pcWriteBuffer, pxTaskStatusArray[x].pcTaskName);

                if (ulStatsAsPercentage > 0UL) {
#ifdef portLU_PRINTF_SPECIFIER_REQUIRED
                    {
                        sprintf(pcWriteBuffer, "\t%lu\t\t%lu%%\r\n", (pxTaskStatusArray[x].ulRunTimeCounter - ulFirstRunTimeCounter[x]),
                                ulStatsAsPercentage);
                    }
#else
                    {
                        /* sizeof( int ) == sizeof( long ) so a smaller
                        printf() library can be used. */
                        sprintf(
                            pcWriteBuffer, "\t%u\t\t%u%%\r\n",
                            (unsigned int) (pxTaskStatusArray[x].ulRunTimeCounter - ulFirstRunTimeCounter[x]),
                            (unsigned int) ulStatsAsPercentage); /*lint !e586 sprintf() allowed as this is compiled with many compilers and this
                                                                    is a utility function only - not part of the core kernel implementation. */
                    }
#endif
                } else {
/* If the percentage is zero here then the task has
consumed less than 1% of the total run time. */
#ifdef portLU_PRINTF_SPECIFIER_REQUIRED
                    { sprintf(pcWriteBuffer, "\t%lu\t\t<1%%\r\n", (pxTaskStatusArray[x].ulRunTimeCounter - ulFirstRunTimeCounter[x])); }
#else
                    {
                        /* sizeof( int ) == sizeof( long ) so a smaller
                        printf() library can be used. */
                        sprintf(pcWriteBuffer, "\t%u\t\t<1%%\r\n",
                                (unsigned int) (pxTaskStatusArray[x].ulRunTimeCounter -
                                                ulFirstRunTimeCounter[x])); /*lint !e586 sprintf() allowed as this is compiled with many
                                                                               compilers and this is a utility function only - not part of the
                                                                               core kernel implementation. */
                    }
#endif
                }

                pcWriteBuffer += strlen(pcWriteBuffer); /*lint !e9016 Pointer arithmetic ok on char pointers especially as in this case where it
                                                           best denotes the intent of the code. */
            }
        } else {
            mtCOVERAGE_TEST_MARKER();
        }

        /* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
        is 0 then vPortFree() will be #defined to nothing. */
        vPortFree(pxFirstTaskStatusArray);
        vPortFree(pxTaskStatusArray);
    } else {
        mtCOVERAGE_TEST_MARKER();
    }
}

BaseType_t prvRunTimeStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString, uint8_t Commamd) {
    const char *const pcHeader = "   Time(*100us)    Time(%)\r\n*******************************************************\r\n";
    BaseType_t        xSpacePadding;
    char              InfoHeader[128] = {0};
    uint16_t          index           = 0;
    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(InfoHeader, "Task");
    index += strlen(InfoHeader);

    /* Pad the string "task" with however many bytes necessary to make it the
    length of a task name.  Minus three for the null terminator and half the
    number of characters in "Task" so the column lines up with the centre of
    the heading. */
    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        InfoHeader[index++] = ' ';

        /* Ensure always terminated. */
        InfoHeader[index] = 0x00;
    }

    strcat(InfoHeader, pcHeader);

    if (Commamd == INTERVAL_RUN_TIME_COMMAND) {
        vTaskGetIntervalRunTimeStats(pcWriteBuffer + strlen(InfoHeader), cpu_show_interval_sec * 1000);
        memcpy(pcWriteBuffer, InfoHeader, strlen(InfoHeader));
    } else if (Commamd == TOTAL_RUN_TIME_COMMAND) {
        vTaskGetRunTimeStats(pcWriteBuffer + strlen(InfoHeader));
        memcpy(pcWriteBuffer, InfoHeader, strlen(InfoHeader));
    }

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

int show_task_cpu(void) {
    prvRunTimeStatsCommand(cStringBuffer, sizeof(cStringBuffer), NULL, TOTAL_RUN_TIME_COMMAND);
    SYS_PRINT("%s\r\n", cStringBuffer);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_cpu, show_task_cpu, show_task_cpu);

int show_task_htop(uint16_t sec) {
    cpu_show_interval_sec = sec;
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_htop, show_task_htop, show_task_htop);

#endif /* ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) \
          ) */
/*-----------------------------------------------------------*/

/*任务堆栈*/
BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *const pcHeader = "   State   Prior   Stack   ID\r\n**********************************************************\r\n";
    BaseType_t        xSpacePadding;

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

int show_task_state(void) {
    prvTaskStatsCommand(cStringBuffer, sizeof(cStringBuffer), NULL);
    SYS_PRINT("%s\r\n", cStringBuffer);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_task_state, show_task_state, show_task_state);
/*-----------------------------------------------------------*/

/*Freertos堆栈*/
__weak int show_heap_state(void) {
    HeapStats_t heapstate;
    vPortGetHeapStats(&heapstate);
    /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    SYS_PRINT("xAvailableHeapSpaceInBytes      : %d\r\n", heapstate.xAvailableHeapSpaceInBytes);
    /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    SYS_PRINT("xSizeOfLargestFreeBlockInBytes  : %d\r\n", heapstate.xSizeOfLargestFreeBlockInBytes);
    /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    SYS_PRINT("xSizeOfSmallestFreeBlockInBytes : %d\r\n", heapstate.xSizeOfSmallestFreeBlockInBytes);
    /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    SYS_PRINT("xNumberOfFreeBlocks             : %d\r\n", heapstate.xNumberOfFreeBlocks);
    /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    SYS_PRINT("xMinimumEverFreeBytesRemaining  : %d\r\n", heapstate.xMinimumEverFreeBytesRemaining);
    /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    SYS_PRINT("xNumberOfSuccessfulAllocations  : %d\r\n", heapstate.xNumberOfSuccessfulAllocations);
    /* The number of calls to vPortFree() that has successfully freed a block of memory. */
    SYS_PRINT("xNumberOfSuccessfulFrees        : %d\r\n", heapstate.xNumberOfSuccessfulFrees);
    SYS_PRINT("\r\n");
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), show_heap_state, show_heap_state, show_heap_state);

/*hardfault信息保存*/
__weak void hardfault_coredown_append(const char *format, ...) {
#ifdef HAL_RTC_MODULE_ENABLED
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
#endif
    // HAL_WWDG_Refresh(&hwwdg);

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* 实现数据输出 */
    vsprintf(cStringBuffer + strlen(cStringBuffer), format, args);

    va_end(args);
}

__weak void hardfault_coredown_save(void) {
    struct fdb_blob blob;
    uint32_t        hardfault_count = 0;

    // HAL_WWDG_Refresh(&hwwdg);

    fdb_kv_get_blob(&kvdb, "hardfault_count", fdb_blob_make(&blob, &hardfault_count, sizeof(hardfault_count)));
    hardfault_count++;
    SYS_PRINT("hardfault_count : %ld\r\n", hardfault_count);
    fdb_kv_set_blob(&kvdb, "hardfault_count", fdb_blob_make(&blob, &hardfault_count, sizeof(hardfault_count)));

    cStringBuffer[strlen(cStringBuffer)] = '\0';
    fdb_kv_set_blob(&kvdb, "hardfault", fdb_blob_make(&blob, cStringBuffer, strlen(cStringBuffer) + 1));
}

void clear_coredown_conut(void) {
    struct fdb_blob blob;
    uint32_t        hardfault_count;
    hardfault_count = 0;
    fdb_kv_set_blob(&kvdb, "hardfault_count", fdb_blob_make(&blob, &hardfault_count, sizeof(hardfault_count)));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), clear_coredown_conut, clear_coredown_conut,
                 clear_coredown_conut);

uint8_t read_coredown(void) {
    struct fdb_blob blob;
    fdb_kv_get_blob(&kvdb, "hardfault", fdb_blob_make(&blob, cStringBuffer, sizeof(cStringBuffer)));

    SYS_PRINT("\r\nhardfault msg:\r\n");
    SYS_PRINT("\r\n");
    SYS_PRINT("-------------------------------------------------------------------------\r\n");

    SYS_PRINT("%s", cStringBuffer);

    SYS_PRINT("\r\n");
    SYS_PRINT("-------------------------------------------------------------------------\r\n");

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), read_coredown, read_coredown, read_coredown);

/*用于测试未对齐的hardfault*/
void fault_test_by_unalign(void) {
    volatile int *SCB_CCR = (volatile int *) 0xE000ED14;  // SCB->CCR
    volatile int *p;
    volatile int  value;

    *SCB_CCR |= (1 << 3); /* bit3: UNALIGN_TRP. */

    p     = (int *) 0x00;
    value = *p;
    SYS_PRINT("addr:0x%02X value:0x%08X\r\n", (int) p, value);

    p     = (int *) 0x04;
    value = *p;
    SYS_PRINT("addr:0x%02X value:0x%08X\r\n", (int) p, value);

    p     = (int *) 0x03;
    value = *p;
    SYS_PRINT("addr:0x%02X value:0x%08X\r\n", (int) p, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_unalign, fault_test_by_unalign,
                 fault_test_by_unalign);

/*用于测试除0错误的hardfault*/
void fault_test_by_div0(void) {
    volatile int *SCB_CCR = (volatile int *) 0xE000ED14;  // SCB->CCR
    int           x, y, z;

    *SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */

    x = 10;
    y = 0;
    z = x / y;
    SYS_PRINT("z:%d\n", z);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_div0, fault_test_by_div0, fault_test_by_div0);

#ifdef __cplusplus
}
#endif