/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: device.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-12-29
 * @Description: device logic
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "buses.h"
#include "devices.h"
#include "can.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "tim.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bus_rs485 {
    struct bus_info common;
    struct bus_info uart;
    struct bus_info gpio_ctrl;
    int8_t          gpio_read_status;
};

struct rs485_handler {
    struct ca_bus *uart;
    struct ca_bus *gpio_ctrl;
    int8_t         gpio_read_status;
};

struct bus_rs485 rs4851 = {
    .common =
        {
            .bus_name = "rs4851",
        },
    .uart =
        {
            .bus_name = "uart3",
        },
    // .gpio_ctrl = {
    // 		.bus_name = "gpioA",
    // 		.bus_addr = 1,
    // 	},
};

extern CAN_HandleTypeDef  hcan;
extern I2C_HandleTypeDef  hi2c1;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef  hrtc;
extern TIM_HandleTypeDef  htim4;
extern TIM_HandleTypeDef  htim8;

int buses_cubemx_init(void) {
    devices_init();
    bus_register((const char *) "can", (void *) &hcan);
    bus_register((const char *) "gpioA", (void *) GPIOA);
    bus_register((const char *) "gpioB", (void *) GPIOB);
    bus_register((const char *) "gpioC", (void *) GPIOC);
    bus_register((const char *) "gpioD", (void *) GPIOD);
    bus_register((const char *) "gpioE", (void *) GPIOE);
    bus_register((const char *) "gpioF", (void *) GPIOF);
    bus_register((const char *) "gpioG", (void *) GPIOG);
    bus_register((const char *) "i2c1", (void *) &hi2c1);
    bus_register((const char *) "iwdg", (void *) &hiwdg);
    bus_register((const char *) "rtc", (void *) &hrtc);
    bus_register((const char *) "tim4", (void *) &htim4);
    bus_register((const char *) "tim8", (void *) &htim8);
    bus_register((const char *) "uart2", (void *) &huart2);
    bus_register((const char *) "uart5", (void *) &huart5);
    int32_t rs485_init(struct bus_rs485 *rs485);
    rs485_init(&rs4851);
    return 0;
}
INIT_BOARD__EXPORT(buses_cubemx_init);

int32_t rs485_init(struct bus_rs485 *rs485) {
    struct rs485_handler *handler = NULL;
    struct ca_bus *       p_bus   = NULL;

    if (NULL == rs485) {
        return -1;
    }

    handler = pvPortMalloc(sizeof(struct rs485_handler));
    if (NULL == handler) {
        return -1;
    }

    if (bus_find_name(&rs485->uart, &handler->uart)) {
        return -1;
    }

    if (bus_find_name(&rs485->gpio_ctrl, &handler->gpio_ctrl)) {
        return -1;
    }

    p_bus = (struct ca_bus *) pvPortMalloc(sizeof(struct ca_bus));
    if (NULL == p_bus) {
        return -1;
    }

    memset(p_bus, 0, sizeof(struct ca_bus));
    memcpy((char *) p_bus->bus_name, rs485->common.bus_name, strlen((char *) rs485->common.bus_name));
    handler->gpio_read_status = rs485->gpio_read_status;
    p_bus->handler            = handler;

    rt_list_insert_after(&bus_list_head.node, &p_bus->node);

    return 0;
}

#ifdef __cplusplus
}
#endif