/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: device.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-12-29
 * @Description: device logic
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef _DEVICES_H
#define _DEVICES_H

/* Private includes ----------------------------------------------------------*/
#include "board.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Private macro -------------------------------------------------------------*/
struct ca_device;

struct bus_info {
    uint8_t bus_name[8];
    uint8_t bus_addr;
};

// struct bus_sw{
// 	struct bus_info common;
// 	struct bus_info gpio_ctrl[3];
// };

// struct bus_switch_group{
// 	struct bus_info gpio_ctrl[3];
// };

struct ca_bus {
    rt_list_t node;
    char      bus_name[8];
    uint16_t  bus_addr;
    void *    handler;
};

struct ca_device_ops {
    int32_t (*init)(const struct ca_device *dev);
    int32_t (*open)(const struct ca_device *dev, int32_t flags);
    int32_t (*close)(const struct ca_device *dev);
    int32_t (*read)(const struct ca_device *dev, void *buffer, uint32_t size);
    int32_t (*write)(const struct ca_device *dev, void *buffer, uint32_t size);
    int32_t (*ioctl)(const struct ca_device *dev, uint32_t cmd, void *arg);
};

struct ca_device {
    rt_list_t            node;
    char                 device_name[RT_NAME_MAX];
    struct ca_device_ops ops;
    struct ca_bus *      bus;
    void *               device_args;
    void *               user_args;
};

struct ca_device_driver {
    const char *device_name;
    const char *bus_name;
    int32_t (*function)(struct ca_device *);
};

struct ca_bus_driver {
    const char *bus_name;
    int32_t (*function)(const struct bus_info *, struct ca_bus **);
};

#define DEVICE_DRIVER_INIT(_dev_name, _bus_name, _func)                                             \
    const struct ca_device_driver driver_##_dev_name##_init __attribute__((section("devices"))) = { \
        .device_name = #_dev_name,                                                                  \
        .bus_name    = #_bus_name,                                                                  \
        .function    = (int32_t(*)(struct ca_device *)) _func,                                      \
    }

#define BUS_DRIVER_INIT(_bus_name, _func)                                                   \
    const struct ca_bus_driver bus_##_bus_name##_init __attribute__((section("buses"))) = { \
        .bus_name = #_bus_name,                                                             \
        .function = (int32_t(*)(const struct bus_info *, struct ca_bus **)) _func,          \
    }

/*****************************************************************
 * 全局变量声明
 ******************************************************************/
extern struct ca_bus    bus_list_head;
extern struct ca_device device_list_head;
/*****************************************************************
 * 函数原型声明
 ******************************************************************/
int32_t bus_register(const char *bus_name, void *handler);
int32_t bus_unregister(const char *bus_name);
int32_t bus_find_name(const struct bus_info *bus_common, struct ca_bus **bus);

int32_t device_open(const char *dev_name, int32_t flags);
int32_t device_close(const uint8_t dev_handle);
int32_t device_read(const uint8_t dev_handle, void *buffer, uint32_t size);
int32_t device_write(const uint8_t dev_handle, void *buffer, uint32_t size);
int32_t device_ioctl(const uint8_t dev_handle, uint32_t cmd, void *arg);
int32_t device_register(const struct bus_info *bus_common, const char *dev_name, void *args);
int32_t device_unregister(const char *dev_name);
int32_t device_find(const char *dev_name, struct ca_device **dev);
int32_t device_driver_probe(struct ca_device *dev);
int32_t devices_init(void);
int32_t devices_deinit(void);

int32_t section_index_bus_driver(const struct bus_info *bus_common);

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */

#endif
