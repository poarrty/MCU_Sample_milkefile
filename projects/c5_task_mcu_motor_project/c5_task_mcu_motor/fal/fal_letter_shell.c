/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_letter_shell.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-14
 * @Description: Functional abstraction layer: clean define
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "fal_letter_shell.h"

#define LOG_TAG "fal_letter_shell"
#include "elog.h"
/******************************************************************************
 * @Function: fal_letter_shell_init
 * @Description: letter-shell初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_letter_shell_init(void) {
    userShellInit();
}

/******************************************************************************
 * @Function: set_log_level
 * @Description: 设置LOG打印级别
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int set_log_level(int argc, char *argv[]) {
    if (argc != 2) {
        LOG_CUSTOM(
            "\033[36mUsage:set_log_level <OFF> <ERROR> <WARNING> <DEBUG> "
            "<DEBUG_ARRAY> <INFO>\r\n");
        return 0;
    }

    if (!strcmp(argv[1], "OFF")) {
        log_level = LOG_LEVEL_OFF;
    } else if (!strcmp(argv[1], "ERROR")) {
        log_level = LOG_LEVEL_ERROR;
    } else if (!strcmp(argv[1], "WARNING")) {
        log_level = LOG_LEVEL_WARNING;
    } else if (!strcmp(argv[1], "DEBUG")) {
        log_level = LOG_LEVEL_DEBUG;
    } else if (!strcmp(argv[1], "DEBUG_ARRAY")) {
        log_level = LOG_LEVEL_DEBUG_ARRAY;
    } else if (!strcmp(argv[1], "INFO")) {
        log_level = LOG_LEVEL_INFO;
    }

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, set_log_level, set_log_level,
                 set log level);

/******************************************************************************
 * @Function: get_sys_tick
 * @Description: 获取系统tick计数值
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *argv
 *******************************************************************************/
int get_sys_tick(int argc, char *argv[]) {
    LOG_CUSTOM("\033[36mReturn：current tick:%ld\033[0m\r\n", HAL_GetTick());

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, get_sys_tick, get_sys_tick,
                 get sys tick);

/******************************************************************************
 * @Function: reboot
 * @Description: 软重启
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {int} argc
 * @param {char} *agrv
 *******************************************************************************/
int reboot(int argc, char *agrv[]) {
    //关闭总中断
    __set_FAULTMASK(1);
    //请求单片机重启
    NVIC_SystemReset();

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, reboot, reboot, sys reboot);