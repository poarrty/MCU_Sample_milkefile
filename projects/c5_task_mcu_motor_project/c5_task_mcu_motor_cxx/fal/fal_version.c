/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_version.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:44:26
 * @Description: 版本信息文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "fal_version.h"
#include "log.h"

#define LOG_TAG "fal_version"

/******************************************************************************
 * @Function: fal_version_init
 * @Description: 版本初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_init(void) {
    fal_version_info_display();
}

/******************************************************************************
 * @Function: fal_version_info_display
 * @Description: 版本信息显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void fal_version_info_display(void) {
    LOG_CUSTOM(INFO_COLOR "\r\n\r\n[VERSION]:%s_%s_%s_%s\r\n\r\n" COLOR_END, CURRENT_BRANCH, COMMIT_ID, BUILD_TIME, SOFTWARE_VERSION);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fal_version_info_display, fal_version_info_display,
                 show version message);
