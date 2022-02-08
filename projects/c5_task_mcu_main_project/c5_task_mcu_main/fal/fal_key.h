#ifndef __FAL_KEY_H
#define __FAL_KEY_H

#include "stdint.h"

#define USED_FUNCTION_KEY

/*按键状态信息*/
typedef struct key_status_info {
    uint8_t key_status;
} key_status_info_st;

/*工作模式*/
enum { AUTO_MODE = 0, MANUAL_MODE };

enum { RETURN_HOME_BUTTON = 0, AUTO_MANUAL_BUTTON, WASH_FLOOR_BUTTON, DUST_PUSH_BUTTON, STRAINER_BUTTON, BUTTON_DEFAULT_MAX };

typedef enum { WATER_ERROR_LOW = 0, WATER_OK = 50, WATER_ERROR_HIGH = 100 } WATER_STATE;

uint32_t water_level_status_get(uint8_t id);
uint32_t function_button_status_get(uint8_t button_id);
uint8_t  function_button_status_set(uint8_t button_id, uint8_t value);
uint8_t  auto_manual_button_status_set(uint8_t data);
void     button_and_led_linkage_update(uint8_t button_id);
#endif
