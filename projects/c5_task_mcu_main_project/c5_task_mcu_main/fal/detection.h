#ifndef __DETECTION_H
#define __DETECTION_H

#include "stdint.h"

enum { DET_DISABLE = 0, DET_ENABLE, DET_DEFAULT };

uint32_t clean_device_error_status_get(uint8_t clean_dev_id);

#endif
