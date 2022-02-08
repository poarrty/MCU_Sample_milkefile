#ifndef BSP_GPIO_IN_H
#define BSP_GPIO_IN_H

#include "stdint.h"

enum {
    ID_WATER_LEVEL = 0,
    ID_ULTTRASOUND,
    ID_FILTER_PUMP_OC,
    ID_SEWAGE_WATER_VALVE_OC,
    ID_CLEAN_WATER_VALVE_OC,
    ID_WASTE_WATER_VALVE_OC,
    ID_KEY_RETURN_HOME,
    ID_KEY_AUTO_MANUAL_MODE,
    ID_KEY_WASH_FLOOR,
    ID_KEY_DUST_PUSH,
    ID_KEY_STRAINER,
};

void    gpio_in_init_for_pca9539(void);
int32_t gpio_in_status_read(uint8_t gpio_in_id);

#endif