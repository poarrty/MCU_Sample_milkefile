#ifndef __CLEAN_CTRL_H
#define __CLEAN_CTRL_H

#include "stdint.h"

int32_t  clean_device_error_status_get(uint8_t clean_dev_id);
uint32_t clean_device_error_status_clear(uint8_t clean_dev_id, uint32_t value);
uint32_t clean_device_status_get(uint8_t clean_dev_id);
/*设定设定接口函数 */
void fliter_pmup_set_data(uint32_t data);
void sewage_water_valve_set_data(uint32_t data);
void clean_water_valve_set_data(uint32_t data);
void waste_water_valve_set_data(uint32_t data);

#endif
