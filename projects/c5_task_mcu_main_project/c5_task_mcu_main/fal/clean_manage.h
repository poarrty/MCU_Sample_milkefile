#ifndef __CLEAN_MANAGE_H
#define __CLEAN_MANAGE_H

#include "stdint.h"

void clean_manage_pal_init(void);
void enable_mcu_hub_motor(bool flag);
bool get_emerg_flag(void);
#endif