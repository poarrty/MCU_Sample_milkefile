/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: version.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 11:44:26
 * @Description: 版本信息文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "version.h"
#include "log.h"

/******************************************************************************
 * @Function: version_init
 * @Description: 版本初始化
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void version_init(void) {
    version_info_display();
}

/******************************************************************************
 * @Function: version_info_display
 * @Description: 版本信息显示
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
void version_info_display(void) {
    LOG_CUSTOM(INFO_COLOR "\r\n\r\n[VERSION]:%s_%s_%s_%s\r\n\r\n" COLOR_END, CURRENT_BRANCH, COMMIT_ID, BUILD_TIME, SOFTWARE_VERSION);
}
