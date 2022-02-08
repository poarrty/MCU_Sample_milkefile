#ifndef __APP_LED_H
#define __APP_LED_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdint.h"
/* maro ****************************************/
#define LED_BREATHING_RATES    150
#define LED_DEFAULT_BRIGHTNESS 50
#define LED_BLINK_EMERGENCY    (60)   // 300MS = 60*5ms
#define LED_BLINK_NAVI_NORMAL  (150)  // 750MS = 150*5ms
#define LED_BLINK_CHARGE       (100)  // 500MS = 100*5ms
/* function ***********************************/
void led_brightness_data_set(int data);
void led_blink_data_set(int data);
#endif
