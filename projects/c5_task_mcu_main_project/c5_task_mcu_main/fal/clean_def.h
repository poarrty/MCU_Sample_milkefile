#ifndef __CLEAN_DEF_H
#define __CLEAN_DEF_H

#include "stdint.h"

enum { ID_FILTER_PUMP_DET = 0, ID_SEWAGE_WATER_VALVE_DET, ID_CLEAN_WATER_VALVE_DET, ID_WASTE_WATER_VALVE_DET, ID_DET_DEFAULT_MAX };

void fliter_pmup_set_data(uint32_t data);
void sewage_water_valve_set_data(uint32_t data);
void clean_water_valve_set_data(uint32_t data);
void waste_water_valve_set_data(uint32_t data);

#endif