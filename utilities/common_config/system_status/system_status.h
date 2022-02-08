#ifndef _SYSTEM_STATUS_H
#define _SYSTEM_STATUS_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t     flag;
    const char *info[7];
} sys_rcc_flag_info_stu_t;

int task_system_status_init(void);

#ifdef __cplusplus
}
#endif

#endif
