#ifndef __BSP_CLEAN_CTRL_H
#define __BSP_CLEAN_CTRL_H

#include "stdint.h"
uint32_t clean_io_ctrl(uint8_t gpio_id, uint32_t value);
uint32_t filter_pump_ops(uint8_t id, uint32_t value);
uint32_t filter_pump_status_get(uint8_t id);
uint32_t water_valve_ops(uint8_t id, uint32_t value);
uint32_t water_valve_status_get(uint8_t id);
uint32_t led_brightness_ops(uint8_t id, uint32_t value);
uint32_t led_brightness_get(uint8_t id);
uint32_t led_blink_ops(uint8_t id, uint32_t value);
uint32_t led_blink_get(uint8_t id);
uint32_t app_version_info_get(uint8_t id);
uint32_t app_update_info_get(uint8_t id);
uint32_t app_update_file_recv_set(uint8_t id, uint32_t value);
uint32_t app_update_enter_boot_set(uint8_t id, uint32_t value);
#endif