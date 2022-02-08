#include "app_led.h"
#include "string.h"
#include "stdbool.h"
#include "bsp_pwm.h"
#include "bsp_gpio_out.h"
#include "clean_def.h"
#include "bsp_pwm.h"
#include "shell.h"
#include "log.h"
#include "bsp_cputime.h"
#include "stdlib.h"

#define LOG_TAG "app_led"
#include "elog.h"

#define LED_THREAD_YIELD_TIME 5 /*系统调度切除周期必须大于占空比周期*/

typedef enum MAV_RGBW_STATUS {
    MAV_RGBW_STATUS_ON       = 1,    /* 0x01  | */
    MAV_RGBW_STATUS_OFF      = 2,    /* 0x02  | */
    MAV_RGBW_STATUS_RGBW     = 4,    /* 0x04  | */
    MAV_RGBW_STATUS_FLOW     = 8,    /* 0x08  | */
    MAV_RGBW_STATUS_COLOR    = 16,   /* 0x10  | */
    MAV_RGBW_STATUS_ENUM_END = 0x81, /*  | */
    MAV_RGBW_CYCLE_BREATHING = 32,
} MAV_RGBW_STATUS;

static uint8_t  rgbw_show_status = MAV_RGBW_CYCLE_BREATHING;
static bool     app_rgbw_debug   = false;
static uint16_t rgbw_debug[4];
static uint32_t led_brightness_data;
static uint32_t led_blink_data;
static uint16_t div_rgbw  = 0;
static uint16_t led_index = 0;

void led_brightness_data_set(int data) {
    led_brightness_data = data;
    div_rgbw            = 0;
    led_index           = 0;
    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, led_brightness_data_set,
                 led_brightness_data_set, set brightness data);

void led_blink_data_set(int data) {
    led_blink_data = data;
    div_rgbw       = 0;
    led_index      = 0;
    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, led_blink_data_set,
                 led_blink_data_set, set blink data);

void led_rgbw_update(uint32_t r, uint32_t g, uint32_t b, uint32_t w) {
    pwm_write_pulse(ID_LED_PWM, r);
}

void task_led_run(void *argument) {
    uint16_t led_breathing_rates = 0;
    uint16_t breathing_delay     = 0;
    uint32_t rgbw[4]             = {0, 0, 0, 0};
    uint32_t rgbs[4]             = {0, 0, 0, 0};

    rgbw_show_status    = MAV_RGBW_CYCLE_BREATHING;
    led_brightness_data = LED_DEFAULT_BRIGHTNESS;
    led_blink_data      = LED_BREATHING_RATES;
    log_d("[rgbw]init success!");

    while (1) {
        osDelay(LED_THREAD_YIELD_TIME);
        // uint32_t start_time = clock_cpu_gettime();
#ifdef USED_LED_CTRL
        /* recv*/
        switch (rgbw_show_status) {
            case MAV_RGBW_CYCLE_BREATHING:
                if (div_rgbw == 0 && led_index == 0) {
                    rgbs[0] = led_brightness_data * LED_PWM_CYCLE_NS / LED_PWM_MAX_LEVLE;
                    rgbs[1] = 0;
                    rgbs[2] = 0;
                    rgbs[3] = 0;

                    if (led_breathing_rates == 0) {
                        led_breathing_rates = LED_BREATHING_RATES;
                    } else {
                        led_breathing_rates = led_blink_data;
                    }

                    breathing_delay = 0;
                }
                break;
            default:
                break;
        }

        if (app_rgbw_debug == true) {
            led_rgbw_update(rgbw_debug[0], rgbw_debug[1], rgbw_debug[2], rgbw_debug[3]);
            continue;
        }

        if (breathing_delay == 0) {
            led_index = 0;

            if (div_rgbw < led_breathing_rates) {
                if (rgbw[0] != rgbs[0]) {
                    rgbw[0] = rgbs[0];
                    led_rgbw_update(rgbw[0], rgbw[1], rgbw[2], rgbw[3]);
                }
            } else if (div_rgbw < 2 * led_breathing_rates) {
                rgbs[0] = 0;

                if (rgbw[0] != rgbs[0]) {
                    rgbw[0] = rgbs[0];
                    led_rgbw_update(rgbw[0], rgbw[1], rgbw[2], rgbw[3]);
                }
            }

            if (++div_rgbw >= 2 * led_breathing_rates) {
                div_rgbw = 0;
            }
        } else {
            /*呼吸值改变等待时间*/
            div_rgbw++;

            if (div_rgbw >= breathing_delay) {
                div_rgbw    = 0;
                float scale = 0.f;
                led_index++;

                /* 呼吸值倍率转化以及呼吸时间总时间控制*/
                if (led_index >= led_breathing_rates) {
                    led_index = 0;
                } else if (led_index >= (led_breathing_rates / 2) && led_index < led_breathing_rates) {
                    scale = (led_breathing_rates - led_index) / (((float) led_breathing_rates) / 2);
                } else if (led_index < (led_breathing_rates / 2)) {
                    scale = led_index / (((float) led_breathing_rates) / 2);
                }

                led_rgbw_update((uint16_t)(rgbw[0] * scale + 0.5f), (uint16_t)(rgbw[1] * scale + 0.5f), (uint16_t)(rgbw[2] * scale + 0.5f),
                                (uint16_t)(rgbw[3] * scale + 0.5f));
            }
        }
#endif
        // LOG_D("%s time :%d", __FUNCTION__,
        // clock_cpu_microsecond_diff(start_time, clock_cpu_gettime()));
    }
}

void rgbw_ctrl(char argc, char **argv) {
    if (argc < 6) {
        return;
    }

    if (atoi(argv[1])) {
        app_rgbw_debug = true;
        rgbw_debug[0]  = atoi(argv[2]);
        rgbw_debug[1]  = atoi(argv[3]);
        rgbw_debug[2]  = atoi(argv[4]);
        rgbw_debug[3]  = atoi(argv[5]);
    } else {
        app_rgbw_debug = false;
        rgbw_debug[0]  = 0;
        rgbw_debug[1]  = 0;
        rgbw_debug[2]  = 0;
        rgbw_debug[3]  = 0;
    }
}
