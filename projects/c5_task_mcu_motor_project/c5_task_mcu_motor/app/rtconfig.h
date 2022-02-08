/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: rtconfig.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: none
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

#define RT_CONFIG_VECT_TAB_OFFSET 0x00010000U

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX   12
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND     1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

//#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP
#define RT_USING_MEMTRACE

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE     128
#define RT_CONSOLE_DEVICE_NAME "uart5"
#define RT_VER_NUM             0x40002
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M4

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY   10

/* C++ features */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY   20
#define FINSH_THREAD_STACK_SIZE 1536
#define FINSH_CMD_SIZE          80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10
//#define FINSH_USING_AUTH
//#define FINSH_DEFAULT_PASSWORD        "cvteteach"
/* Device virtual file system */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
//#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ (1024 + 128)
//#define RT_USING_I2C
// #define RT_USING_I2C_BITOPS
#define RT_USING_PWM
// #define RT_USING_WDT
#define RT_USING_CAN
#define RT_USING_HWTIMER
#define RT_USING_ADC
#define RT_USING_CPUTIME
/* Using Hardware Crypto drivers */

/* Using WiFi */

/* Using USB */

/* POSIX layer and C standard library */

/* Network */

/* Socket abstraction layer */

/* Network interface device */

/* light weight TCP/IP stack */

/* Modbus master and slave stack */

/* AT commands */

/* VBUS(Virtual Software BUS) */

/* Utilities */

/* RT-Thread online packages */

#define RT_S_DISCRETE_INPUT_START      0
#define RT_S_DISCRETE_INPUT_NDISCRETES 16
#define RT_S_COIL_START                0
#define RT_S_COIL_NCOILS               64
#define RT_S_REG_INPUT_START           0
#define RT_S_REG_INPUT_NREGS           100
#define RT_S_REG_HOLDING_START         0
#define RT_S_REG_HOLDING_NREGS         100
#define RT_S_HD_RESERVE                0
#define RT_S_IN_RESERVE                0
#define RT_S_CO_RESERVE                0
#define RT_S_DI_RESERVE                0
/* end of advanced configuration */
#define MB_SLAVE_RTU
#define MB_SLAVE_SAMPLE
#define MB_SAMPLE_SLAVE_ADDR         0x5A
#define MB_SLAVE_USING_PORT_NUM      2
#define MB_SLAVE_USING_PORT_BAUDRATE 115200

#define PKG_USING_FAL
#define FAL_PART_HAS_TABLE_CFG
#define PKG_USING_YMODEM_OTA
#define YMODEM_USING_CRC_TABLE
/* IoT - internet of things */

/* Wi-Fi */

/* Marvell WiFi */

/* Wiced WiFi */

/* IoT Cloud */

/* security packages */

/* language packages */

/* multimedia packages */

/* tools packages */

/* system packages */
#define RT_USING_ULOG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_TIME_USING_TIMESTAMP
#define ULOG_USING_FILTER
#define ULOG_USING_ASYNC_OUTPUT
#define ULOG_USING_ISR_LOG
#define ULOG_USING_COLOR
//#define ULOG_OUTPUT_FLOAT
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_LVL                   LOG_LVL_DBG
#define ULOG_OUTPUT_THREAD_NAME           "ulog"
#define ULOG_ASYNC_OUTPUT_BUF_SIZE        4096
#define ULOG_ASYNC_OUTPUT_THREAD_STACK    1024
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 19
/* peripheral libraries and drivers */

/* miscellaneous packages */

/* samples: kernel and components samples */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32F1

/* Hardware Drivers Config */

#define SOC_GD32F303RCT6

/* Onboard Peripheral Drivers */

#define BSP_USING_USB_TO_USART

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_UART5
//#define BSP_UART5_RX_USING_DMA
#define BSP_USING_UART2
//#define BSP_USING_UART3
//#define BSP_UART3_RX_USING_DMA
//#define BSP_USING_UART4
//#define BSP_UART4_RX_USING_DMA
// #define BSP_USING_SOFT_I2C
// #define BSP_USING_I2C2
#define BSP_USING_PWM8
#define BSP_USING_PWM8_CH1
#define BSP_USING_PWM8_CH2
#define BSP_USING_PWM8_CH3
// #define BSP_USING_CAN
#define BSP_USING_CAN1
#define BSP_USING_ON_CHIP_FLASH
#define BSP_USING_TIM
#define BSP_USING_TIM3
#define BSP_USING_ADC2
/* Board extended module Drivers */

/*User defined macros*/
#define APP_MAJOR_VERSION  5
#define APP_MINOR_VERSION  0
#define APP_REVISE_VERSION 5
#define HW_MAJOR_VERSION   0
#define HW_MINOR_VERSION   1
#define HW_REVISE_VERSION  0

//#define USED_DEVICE_OUTPUT_TEST
//#define USED_IO_OUTPUT_TEST
#define USED_IWDG
#endif
