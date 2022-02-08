#include "bsp_iwdg.h"
#include "stm32f1xx.h"
#include "main.h"
#include "iwdg.h"
#include "shell.h"
#include "log.h"

void iwdg_feed(void) {
    if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK) {
        log_d("watch dog keepalive fail");
    }
}

void iwdg_write_timeout(uint16_t time) {
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Reload    = time * LSI_VALUE / (1 << (IWDG_PRESCALER_256 + 2)) / 1000;

    if (hiwdg.Init.Reload > 0xfff) {
        log_d("watch dog set timeout too large");
    }

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        log_d("watch dog set timeout fail");
    }
}
