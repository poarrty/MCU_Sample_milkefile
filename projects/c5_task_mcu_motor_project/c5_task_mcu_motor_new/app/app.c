/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: app.c
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-14 09:29:46
 * @Description: APP入口文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "board.h"

#define LOG_TAG "app"
#include "elog.h"

/* board section pos in flash */
extern const uint32_t *_board_init_start;
extern const uint32_t *_board_init_end;
/* prev section pos in flash */
extern const uint32_t *_pre_init_start;
extern const uint32_t *_pre_init_end;
/* device section pos in flash */
extern const uint32_t *_device_init_start;
extern const uint32_t *_device_init_end;
/* component section pos in flash */
extern const uint32_t *_component_init_start;
extern const uint32_t *_component_init_end;
/* env section pos in flash */
extern const uint32_t *_env_init_start;
extern const uint32_t *_env_init_end;
/* app section pos in flash */
extern const uint32_t *_app_init_start;
extern const uint32_t *_app_init_end;

/* KVDB object */
struct fdb_kvdb kvdb = {0};

static void lock(fdb_db_t db) {
    __disable_irq();
}

static void unlock(fdb_db_t db) {
    __enable_irq();
}
/******************************************************************************
 * @Function: app_init
 * @Description: APP初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int app_init(void) {
    hal_init();
    common_init();
    pal_init();
    fal_version_init();

    struct fdb_default_kv default_kv;
    default_kv.kvs = NULL;
    default_kv.num = 0;
    /* set the lock and unlock function if you want */
    fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, lock);
    fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, unlock);

    fdb_kvdb_init(&kvdb, "env", "fdb_kvdb", &default_kv, NULL);

    fal_log_init();

    fal_letter_shell_init();

    /* init */
    INIT__EXPORT(_board_init_start, _board_init_end);
    INIT__EXPORT(_pre_init_start, _pre_init_end);
    INIT__EXPORT(_device_init_start, _device_init_end);
    INIT__EXPORT(_component_init_start, _component_init_end);
    INIT__EXPORT(_env_init_start, _env_init_end);
    INIT__EXPORT(_app_init_start, _app_init_end);

    return 0;
}

int board_init(void) {
    log_d("cyl init test!");
    return 0;
}
INIT_PREV__EXPORT(board_init);
/******************************************************************************
 * @Function: app_DeInit
 * @Description: APP反初始化函数
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int app_DeInit(void) {
    return 0;
}
