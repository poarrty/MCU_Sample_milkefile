#include "bsp_clean_ctrl.h"
#include "bsp_gpio_out.h"
#include "clean_def.h"
#include "bsp_pwm.h"
//#include "orb_def.h"

#define LOG_TAG "clean_ctrl"
//#include "rtdbg.h"

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])

// static uint32_t filter_pump_status = 0;
// static uint32_t sewage_water_valve_status = 0;
// static uint32_t clean_water_valve_status = 0;
// static uint32_t waste_water_valve_status = 0;
// static uint32_t led_brightness_status = 0;
// static uint32_t led_blink_status = 0;

uint32_t clean_io_ctrl(uint8_t gpio_id, uint32_t value) {
    if (value) {
        gpio_out_on(gpio_id);
    } else {
        gpio_out_off(gpio_id);
    }
    return value;
}
