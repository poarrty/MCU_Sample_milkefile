#ifndef _FOTA_PORT_H_
#define _FOTA_PORT_H_

#include <tinycrypt.h>
#include <fastlz.h>
#include <quicklz.h>

/**
 * YMODEM升级功能宏
 */
#define FOTA_YMODEM

/**
 * YMODEM升级及打印信息串口
 */
#define FOTA_YMODEM_HUART huart3
#define FOTA_YMODEM_UART  USART3

/**
 * 版本回滚功能宏
 */
#define FOTA_ROLLBACK

/**
 * 固件存储区在fal分区管理的命名
 */
#ifndef FOTA_FM_PART_NAME
#define FOTA_FM_PART_NAME "main_mcu_temp"
#endif

/**
 * app运行在fal分区管理的命名
 */
#ifndef FOTA_APP_PART_NAME
#define FOTA_APP_PART_NAME "main_mcu_app"
#endif

/**
 * 版本回滚存储的固件在fal分区管理的命名
 */
#ifndef FOTA_ROLLBACK_PART_NAME
#define FOTA_ROLLBACK_PART_NAME "main_mcu_rollback"
#endif

/**
 * 加密解密动态分配内存的大小
 */
#ifndef FOTA_ALGO_BUFF_SIZE
#define FOTA_ALGO_BUFF_SIZE 4096
#endif

/**
 * AES256 encryption algorithm option
 */
#ifndef FOTA_ALGO_AES_IV
#define FOTA_ALGO_AES_IV "0123456789ABCDEF"
#endif

#ifndef FOTA_ALGO_AES_KEY
#define FOTA_ALGO_AES_KEY "C323456789ABCDEF0123456789ABCDEF"
#endif

#ifndef FOTA_BLOCK_HEADER_SIZE
#define FOTA_BLOCK_HEADER_SIZE 4
#endif

#ifndef FOTA_CMPRS_BUFFER_SIZE
#define FOTA_CMPRS_BUFFER_SIZE 4096
#endif

#ifndef FOTA_FASTLZ_BUFFER_PADDING
#define FOTA_FASTLZ_BUFFER_PADDING FASTLZ_BUFFER_PADDING(FOTA_CMPRS_BUFFER_SIZE)
#endif

#ifndef FOTA_QUICKLZ_BUFFER_PADDING
#define FOTA_QUICKLZ_BUFFER_PADDING QLZ_BUFFER_PADDING
#endif

#define LOG_D(format, ...) LOG_DEBUG(format, ##__VA_ARGS__);
#define LOG_I(format, ...) LOG_INFO(format, ##__VA_ARGS__);

#endif
