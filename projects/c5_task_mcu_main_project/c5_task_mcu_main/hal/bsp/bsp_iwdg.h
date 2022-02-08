#ifndef BSP_IWDG_H
#define BSP_IWDG_H

#include "stdint.h"

void iwdg_feed(void);
void iwdg_write_timeout(uint16_t time);

#endif