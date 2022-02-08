/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: init.c
 * @Author: Chen yuliang
 * @Version: V1.0
 * @Date: 2021-05-14 09:29:46
 * @Description: 
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "extern_init.h"

#define LOG_TAG "extern_init"
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
/******************************************************************************
 * @Function: init
 * @Description: 
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int extern_init(void) {
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
 * @Function: DeInit
 * @Description: 
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int extern_DeInit(void) {
    return 0;
}
