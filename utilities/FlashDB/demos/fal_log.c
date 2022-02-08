#include "fal_log.h"
#include "cmsis_os.h"
#include "elog.h"
#include "shell.h"
#include <flashdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

osMutexId_t mutex_elog = NULL;

const osMutexAttr_t mutex_elog_attr =
{
	"mutex_elog",                                             // human readable mutex name
	osMutexRecursive | osMutexPrioInherit,                    // attr_bits
	NULL,                                                     // memory for control block
	0U                                                        // size for control block
};

#define ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE         (ELOG_LINE_BUF_SIZE - 4)

extern struct fdb_tsdb tsdb;
extern void elog_port_output(const char *log, size_t size);

int fal_log_init(void)
{
	/*添加模块处理函数*/
	mutex_elog = osMutexNew(&mutex_elog_attr);

	/* 初始化elog */
	elog_init();

	/* 设置每个级别的日志输出格式 */
	//输出所有内容
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	//输出日志级别信息和日志TAG
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
	//除了时间、进程信息、线程信息之外，其余全部输出
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME | ELOG_FMT_LVL | ELOG_FMT_TAG);
	//输出所有内容
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);

	/* 启动elog */
	elog_start();

	elog_set_text_color_enabled(true);
	return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_lvl, elog_set_filter_lvl, elog_set_filter_lvl);
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_tag_lvl, elog_set_filter_tag_lvl, elog_set_filter_tag_lvl);

int fal_log_deInit(void)
{
	return 0;
}

void task_fal_log_run(void *argument)
{
	size_t get_log_size = 0;
	static char poll_get_buf[ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE];
	struct fdb_blob blob;

	for (;;)
	{

#ifdef ELOG_ASYNC_LINE_OUTPUT
		get_log_size = elog_async_get_line_log(poll_get_buf, ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE);
#else
		get_log_size = elog_async_get_log(poll_get_buf, ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE);
#endif

		if (get_log_size)
		{
			elog_port_output(poll_get_buf, get_log_size);
			fdb_tsl_append(&tsdb, fdb_blob_make(&blob, poll_get_buf, get_log_size));
		}

		osDelay(50);
	}
}

static char elog_saved_buf[ELOG_ASYNC_POLL_GET_LOG_BUF_SIZE];
static char temp_buff[30];
static fdb_time_t from_time;
static fdb_time_t to_time;
/*遍历寻找时序数据库中所有的log*/
static bool query_cb(fdb_tsl_t tsl, void *arg)
{
	size_t get_log_size = 0;
	struct fdb_blob blob;
	fdb_tsdb_t db = arg;

	if (tsl->status != FDB_TSL_DELETED)
	{
		memset(elog_saved_buf, 0, sizeof(elog_saved_buf));
		get_log_size = fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, elog_saved_buf, sizeof(elog_saved_buf))));
		elog_port_output(elog_saved_buf, get_log_size);
	}

	return false;
}

void elog_find_all_history(void)
{
	fdb_tsl_iter(&tsdb, query_cb, &tsdb);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_find_all_history, elog_find_all_history, elog_find_all_history);
/*end*/

/*删除时序数据库中所有的log*/
static bool del_all_cb(fdb_tsl_t tsl, void *arg)
{
	fdb_tsdb_t db = arg;

	fdb_tsl_set_status(db, tsl, FDB_TSL_DELETED);

	return false;
}

void elog_delete_all_history(void)
{
	fdb_tsl_iter(&tsdb, del_all_cb, &tsdb);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_delete_all_history, elog_delete_all_history, elog_delete_all_history);
/*end*/

/*遍历寻找指定tag(兼容elog)所有的log*/
static bool tag_cb(fdb_tsl_t tsl, void *arg)
{
	size_t get_log_size = 0;
	struct fdb_blob blob;
	fdb_tsdb_t db = arg;
	size_t len = 0;

	if (tsl->status != FDB_TSL_DELETED)
	{
		memset(elog_saved_buf, 0, sizeof(elog_saved_buf));
		get_log_size = fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, elog_saved_buf, sizeof(elog_saved_buf))));

		if (!strncmp(temp_buff, elog_find_tag(elog_saved_buf, 0, &len), strlen(temp_buff)))
		{
			elog_port_output(elog_saved_buf, get_log_size);
		}
	}

	return false;
}

uint8_t elog_find_tag_history(char *tag)
{
	memset(temp_buff, 0, sizeof(temp_buff));

	if (tag != NULL)
	{
		strncpy(temp_buff, tag, sizeof(temp_buff));
		fdb_tsl_iter(&tsdb, tag_cb, &tsdb);
		return 1;
	}
	else
	{
		return 0;
	}
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_find_tag_history, elog_find_tag_history, elog_find_tag_history);
/*end*/

/*删除指定tag(兼容elog)所有的log*/
static bool del_tag_cb(fdb_tsl_t tsl, void *arg)
{
	struct fdb_blob blob;
	fdb_tsdb_t db = arg;
	size_t len = 0;

	if (tsl->status != FDB_TSL_DELETED)
	{
		memset(elog_saved_buf, 0, sizeof(elog_saved_buf));
		fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, elog_saved_buf, sizeof(elog_saved_buf))));

		if (!strncmp(temp_buff, elog_find_tag(elog_saved_buf, 0, &len), strlen(temp_buff)))
		{
			fdb_tsl_set_status(db, tsl, FDB_TSL_DELETED);
		}
	}

	return false;
}

uint8_t elog_delete_tag_history(char *tag)
{
	memset(temp_buff, 0, sizeof(temp_buff));

	if (tag != NULL)
	{
		strncpy(temp_buff, tag, sizeof(temp_buff));
		fdb_tsl_iter(&tsdb, del_tag_cb, &tsdb);
		return 1;
	}
	else
	{
		return 0;
	}
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_delete_tag_history, elog_delete_tag_history, elog_delete_tag_history);
/*end*/

/*遍历寻找指定时间所有的log*/
static bool time_cb(fdb_tsl_t tsl, void *arg)
{
	size_t get_log_size = 0;
	struct fdb_blob blob;
	fdb_tsdb_t db = arg;

	if ((tsl->time >= from_time) && (tsl->time <= to_time) && (tsl->status != FDB_TSL_DELETED))
	{
		printf("time: %u\r\n", tsl->time);
		memset(elog_saved_buf, 0, sizeof(elog_saved_buf));
		get_log_size = fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, elog_saved_buf, sizeof(elog_saved_buf))));
		elog_port_output(elog_saved_buf, get_log_size);
	}

	return false;
}

uint8_t elog_find_time_history(char *time_from, char *time_to)
{
	struct tm tm_from = {0};
	struct tm tm_to = {0};

	char s[2] = ".";
	char *from_token[10] = {0};
	char *to_token[10] = {0};
	char *ptr;

	if (time_from != NULL)
	{
		int i = 0;
		from_token[i] = strtok(time_from, s);

		while (from_token[i] != NULL)
		{
			i++;
			from_token[i] = strtok(NULL, s);
		}

		tm_from.tm_year = strtol(from_token[0], &ptr, 10);
		tm_from.tm_mon  = strtol(from_token[1], &ptr, 10);
		tm_from.tm_mday = strtol(from_token[2], &ptr, 10);
		tm_from.tm_hour = strtol(from_token[3], &ptr, 10);
		tm_from.tm_min  = strtol(from_token[4], &ptr, 10);
		tm_from.tm_sec  = strtol(from_token[5], &ptr, 10);
	}
	else
	{
		return 0;
	}

	if (time_to != NULL)
	{
		int i = 0;
		to_token[i] = strtok(time_to, s);

		while (to_token[i] != NULL)
		{
			i++;
			to_token[i] = strtok(NULL, s);
		}

		tm_to.tm_year = strtol(to_token[0], &ptr, 10);
		tm_to.tm_mon  = strtol(to_token[1], &ptr, 10);
		tm_to.tm_mday = strtol(to_token[2], &ptr, 10);
		tm_to.tm_hour = strtol(to_token[3], &ptr, 10);
		tm_to.tm_min  = strtol(to_token[4], &ptr, 10);
		tm_to.tm_sec  = strtol(to_token[5], &ptr, 10);
	}
	else
	{
		return 0;
	}

	from_time = mktime(&tm_from);
	to_time = mktime(&tm_to);

	fdb_tsl_iter(&tsdb, time_cb, &tsdb);
	return 1;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), elog_find_time_history, elog_find_time_history, elog_find_time_history);
/*end*/
#ifdef __cplusplus
}
#endif

/* @} F_LOG */
/* @} Robot_FAL */
