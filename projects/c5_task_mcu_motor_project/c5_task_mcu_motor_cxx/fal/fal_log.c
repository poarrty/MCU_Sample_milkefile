/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         chenyuliang
 ** Version:        V0.0.1
 ** Date:           2021-11-19
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-03 sulikang 创建
 ** <time>          <author>    <version >    <desc>
 ** 2021-3-25       sulikang    0.0.1         创建文件
 ******************************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "fal_log.h"

#define LOG_TAG "fal_log"
#include "elog.h"
/**
 * @addtogroup Robot_FAL
 * @{
 */

/**
 * @defgroup F_LOG 日志管理 - LOG
 *
 * @brief  \n
 * \n
 * @{
 */

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
osMutexId_t mutex_elog = NULL;

const osMutexAttr_t mutex_elog_attr = {
    "mutex_elog",                           // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明
 ******************************************************************/

/*****************************************************************
 * 私有函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 ******************************************************************/
extern void elog_port_output(const char *log, size_t size);
extern void elog_hook(const char *expr, const char *func, size_t line);

/*****************************************************************/
/**
 * Function:       fal_log_init
 * Description:    初始化 fal_log
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
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
int fal_log_init(void) {
    /*添加模块处理函数*/
    mutex_elog = osMutexNew(&mutex_elog_attr);

    if (mutex_elog == NULL) {
        // log_e("Display mutex create failed.");
    } else {
        // log_w("Display mutex create ok.");
    }

    /* 初始化elog */
    elog_init();

    /* cyl:set elog hook for assert */
    elog_assert_set_hook(elog_hook);

    /* 设置每个级别的日志输出格式 */
    //输出所有内容
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    //输出日志级别信息和日志TAG
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
    //除了时间、进程信息、线程信息之外，其余全部输出
    // elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_TIME |
    // ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
    //输出所有内容
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);

    /* 启动elog */
    elog_start();

    elog_set_text_color_enabled(true);

    elog_set_filter_tag_lvl("defaultTask", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("task_shell", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("device_run", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("det_thread", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("can_rx", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("fan_motor_speed__set", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("sm_can_rx", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("task_fal_pubsub", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("pal_ctl", ELOG_FILTER_LVL_SILENT);
    elog_set_filter_tag_lvl("fal_usound", ELOG_FILTER_LVL_SILENT);

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_lvl, elog_set_filter_lvl, elog_set_filter_lvl);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_tag_lvl, elog_set_filter_tag_lvl, elog_set_filter_tag_lvl);

/*****************************************************************/
/**
 * Function:       fal_log_deInit
 * Description:    释放 fal_log 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
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
int fal_log_deInit(void) {
    return 0;
}
#define ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE (ELOG_LINE_BUF_SIZE - 4)

void task_fal_log_run(void *argument) {
    size_t      get_log_size = 0;
    static char poll_get_buf[ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE];

    for (;;) {
        // app_log 暂无处理业务
        // elog_flush();
        // osDelay(50);  ///< 50 ms 对应 1000 byte 数据

#ifdef ELOG_ASYNC_LINE_OUTPUT
        get_log_size = elog_async_get_line_log(poll_get_buf, ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE);
#else
        get_log_size = elog_async_get_log(poll_get_buf, ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE);
#endif

        if (get_log_size) {
            elog_port_output(poll_get_buf, get_log_size);
        }

        osDelay(50);
    }
}

#ifdef __cplusplus
}
#endif

/* @} F_LOG */
/* @} Robot_FAL */
