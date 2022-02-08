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

#include "hal.h"
#include "common.h"
#include "fal.h"
#include "pal.h"
#include "fal_version.h"
#include "fal_letter_shell.h"
#include "flashdb.h"
#include "log.h"
#include "fal_log.h"
#include "main.h"

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

    return 0;
}

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
