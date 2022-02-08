/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-24
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-3-25       sulikang    0.0.1         创建文件
******************************************************************/

#ifndef _DEVICES_H
#define _DEVICES_H

/*****************************************************************
* 包含头文件
******************************************************************/
#include <stdint.h>
#include "hal.h"
#include "sys_list.h"
#include "cmsis_os.h"
/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_DEVICES
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
* 宏定义
******************************************************************/
#define MIN(x,y)		((x)>(y) ? (y) : (x))
#define MAX(x,y)		((x)>(y) ? (x) : (y))

/*****************************************************************
* 结构定义
******************************************************************/
struct ca_device;

struct bus_info{
	uint8_t bus_name[8];
	uint8_t bus_addr;
};

struct bus_rs485{
	struct bus_info common;
	struct bus_info uart;
	struct bus_info gpio_ctrl;
	int8_t gpio_read_status;
};

struct bus_sw{
	struct bus_info common;
	struct bus_info gpio_ctrl[3];
};

struct bus_switch_group{
	struct bus_info gpio_ctrl[3];
};

struct ca_bus{
	struct list_struct node;
	uint8_t bus_name[8];
	uint16_t bus_addr;
	void *handler;
};

struct ca_device_ops{
	int32_t (*open)  (const struct ca_device *dev, int32_t flags);
	int32_t (*close) (const struct ca_device *dev);
	int32_t (*read)  (const struct ca_device *dev, void *buffer, uint32_t size);
	int32_t (*write) (const struct ca_device *dev, void *buffer, uint32_t size);
	int32_t (*ioctl) (const struct ca_device *dev,  uint32_t cmd, void *arg);
};

struct ca_device{
	struct list_struct node;
	uint8_t device_name[16];
	struct ca_device_ops ops;
	struct ca_bus *bus;
	void *device_args;
	void *user_args;
};

struct ca_device_driver{
	const char *device_name;
	const char *bus_name;
	int32_t (*function)(struct ca_device *);        
};

struct ca_bus_driver{
	const char *bus_name;
	int32_t (*function)(const struct bus_info *, struct ca_bus **);        
};


#define DEVICE_DRIVER_INIT(_dev_name, _bus_name, _func) \
	const struct ca_device_driver \
	driver_##_dev_name##_init __attribute__((section("devices"))) =  \
	{ \
			.device_name = #_dev_name, \
			.bus_name = #_bus_name, \
			.function = (int32_t (*)(struct ca_device *))_func, \
	}

#define BUS_DRIVER_INIT(_bus_name, _func) \
		const struct ca_bus_driver \
		bus_##_bus_name##_init __attribute__((section("buses"))) =	\
		{ \
				.bus_name = #_bus_name, \
				.function = (int32_t (*)(const struct bus_info *, struct ca_bus **))_func, \
		}



/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/
int32_t bus_register(const uint8_t *bus_name, void *handler);
int32_t bus_unregister(const uint8_t *bus_name);
int32_t bus_find_name(const struct bus_info *bus_common, struct ca_bus **bus);

int32_t device_open(const uint8_t *dev_name, int32_t flags);
int32_t device_close(const uint8_t dev_handle);
int32_t device_read(const uint8_t dev_handle, void *buffer, uint32_t size);
int32_t device_write(const uint8_t dev_handle, void *buffer, uint32_t size);
int32_t device_ioctl(const uint8_t dev_handle,  uint32_t cmd, void *arg);
int32_t device_register(const struct bus_info *bus_common, const uint8_t *dev_name, void *args);
int32_t device_unregister(const uint8_t *dev_name);
int32_t device_find(const uint8_t *dev_name, struct ca_device **dev);
int32_t device_driver_probe(struct ca_device *dev);
int32_t devices_init(void);
int32_t devices_deinit(void);

/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */


#endif
