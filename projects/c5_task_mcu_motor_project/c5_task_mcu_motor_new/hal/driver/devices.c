/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: device.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-12-29
 * @Description: device logic
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "devices.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEVICE_ALLOC pvPortMalloc
#define DEVICE_FREE  vPortFree

#define DEVICE_FILE_MAX 32

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
struct ca_bus     bus_list_head;
struct ca_device  device_list_head;
struct ca_device *dev_file[DEVICE_FILE_MAX];

/* device section pos in flash */
extern const unsigned int _devices_start;
extern const unsigned int _devices_end;
/* buses section pos in flash */
extern const unsigned int _bus_start;
extern const unsigned int _bus_end;

/* Private function prototypes -----------------------------------------------*/

/*****************************************************************
 * 函数定义
 ******************************************************************/
int32_t bus_register(const char *bus_name, void *handler) {
    struct ca_bus *p_bus = NULL;

    if ((NULL == bus_name) || (NULL == handler)) {
        return -RT_ERROR;
    }

    p_bus = &bus_list_head;
    while (p_bus->node.next != &bus_list_head.node) {
        if (0 == strcmp((char *) p_bus->bus_name, (char *) bus_name)) {
            return -RT_ERROR;
        }
        p_bus = rt_list_entry(p_bus->node.next, struct ca_bus, node);
    }

    p_bus = (struct ca_bus *) DEVICE_ALLOC(sizeof(struct ca_bus));
    if (NULL == p_bus) {
        return RT_ENOMEM;
    }

    memset(p_bus, 0, sizeof(struct ca_bus));
    memcpy((char *) p_bus->bus_name, bus_name, strlen((char *) bus_name));
    p_bus->handler = handler;

    rt_list_insert_after(&bus_list_head.node, &p_bus->node);

    return RT_EOK;
}

int32_t bus_unregister(const char *bus_name) {
    return RT_EOK;
}

int32_t bus_find_name(const struct bus_info *bus_common, struct ca_bus **bus) {
    struct ca_bus *p_bus = NULL;

    if (NULL == bus_common) {
        return RT_EEMPTY;
    }

    p_bus = &bus_list_head;
    while (p_bus->node.next != &bus_list_head.node) {
        if (0 == strcmp((char *) p_bus->bus_name, (char *) bus_common->bus_name)) {
            *bus = DEVICE_ALLOC(sizeof(struct ca_bus));
            if (NULL != *bus) {
                memcpy(*bus, p_bus, sizeof(struct ca_bus));
                (*bus)->bus_addr = bus_common->bus_addr;
            }
            return RT_EOK;
        }
        p_bus = rt_list_entry(p_bus->node.next, struct ca_bus, node);
    }

    return -RT_ERROR;
}

int32_t device_open(const char *dev_name, int32_t flags) {
    struct ca_device *p_device  = NULL;
    int8_t            find_flag = -1;
    int               index     = 0;

    if (NULL == dev_name) {
        return -RT_ERROR;
    }

    p_device = &device_list_head;
    while (p_device->node.next != &device_list_head.node) {
        if (0 == strcmp((char *) p_device->device_name, (char *) dev_name)) {
            find_flag = 0;
            break;
        }
        p_device = rt_list_entry(p_device->node.next, struct ca_device, node);
    }

    if (find_flag < 0) {
        return -RT_ERROR;
    }

    for (index = 0; index < DEVICE_FILE_MAX; index++) {
        if (NULL == dev_file[index]) {
            dev_file[index] = p_device;
            if (NULL != p_device->ops.open) {
                p_device->ops.open(p_device, flags);
            }

            return index;
        }
    }

    return -RT_ERROR;
}

int32_t device_close(const uint8_t dev_handle) {
    struct ca_device *dev = NULL;

    if (dev_handle > DEVICE_FILE_MAX - 1) {
        return -RT_ERROR;
    }

    if (NULL == dev_file[dev_handle]) {
        return -RT_ERROR;
    }

    dev                  = dev_file[dev_handle];
    dev_file[dev_handle] = NULL;

    if (NULL != dev->ops.close) {
        return dev->ops.close(dev);
    }

    return -RT_ERROR;
}

int32_t device_read(const uint8_t dev_handle, void *buffer, uint32_t size) {
    struct ca_device *dev = NULL;

    if (dev_handle > DEVICE_FILE_MAX - 1) {
        return -RT_ERROR;
    }

    if (NULL == dev_file[dev_handle]) {
        return -RT_ERROR;
    }

    dev = dev_file[dev_handle];

    if (NULL != dev->ops.read) {
        return dev->ops.read(dev, buffer, size);
    }
    return RT_EOK;
}

int32_t device_write(const uint8_t dev_handle, void *buffer, uint32_t size) {
    struct ca_device *dev = NULL;

    if (dev_handle > DEVICE_FILE_MAX - 1) {
        return -RT_ERROR;
    }

    if (NULL == dev_file[dev_handle]) {
        return -RT_ERROR;
    }

    dev = dev_file[dev_handle];

    if (NULL != dev->ops.write) {
        return dev->ops.write(dev, buffer, size);
    }
    return RT_EOK;
}

int32_t device_ioctl(const uint8_t dev_handle, uint32_t cmd, void *arg) {
    struct ca_device *dev = NULL;

    if (dev_handle > DEVICE_FILE_MAX - 1) {
        return -RT_ERROR;
    }

    dev = dev_file[dev_handle];

    if (NULL == dev_file[dev_handle]) {
        return -RT_ERROR;
    }

    if (NULL != dev->ops.ioctl) {
        return dev->ops.ioctl(dev, cmd, arg);
    }
    return RT_EOK;
}

int32_t device_register(const struct bus_info *bus_common, const char *dev_name, void *args) {
    struct ca_bus *   bus      = NULL;
    struct ca_device *devices  = NULL;
    struct ca_device *p_device = NULL;

    if (NULL == dev_name) {
        return RT_EEMPTY;
    }

    if (NULL != bus_common) {
        if (bus_find_name(bus_common, &bus)) {
            return -RT_ERROR;
        }

        if (NULL == bus) {
            return RT_EEMPTY;
        }
    }

    if (!device_find(dev_name, &devices)) {
        return -RT_ERROR;
    }

    p_device = (struct ca_device *) DEVICE_ALLOC(sizeof(struct ca_device));
    if (NULL == p_device) {
        return RT_EEMPTY;
    }

    memset(p_device, 0, sizeof(struct ca_device));
    memcpy((char *) p_device->device_name, dev_name, strlen((char *) dev_name));

    p_device->bus         = bus;
    p_device->device_args = args;

    rt_list_insert_after(&device_list_head.node, &p_device->node);
    if (device_driver_probe(p_device) < 0) {
        DEVICE_FREE(p_device);
        return -RT_ERROR;
    }

    return RT_EOK;
}

int32_t device_unregister(const char *dev_name) {
    return 0;
}

int32_t device_find(const char *dev_name, struct ca_device **dev) {
    struct ca_device *p_device = NULL;

    if (NULL == dev_name) {
        return RT_EEMPTY;
    }

    p_device = &device_list_head;
    while (p_device->node.next != &device_list_head.node) {
        if (0 == strcmp((char *) p_device->device_name, (char *) dev_name)) {
            *dev = p_device;
            return RT_ERROR;
        }
        p_device = rt_list_entry(p_device->node.next, struct ca_device, node);
    }

    return -RT_ERROR;
}

int32_t device_driver_probe(struct ca_device *dev) {
    struct ca_device_driver *base  = NULL;
    unsigned short           count = 0;
    int                      n     = 0;

    if ((NULL == dev) || (NULL == dev->bus)) {
        return -RT_ERROR;
    }

    base  = (struct ca_device_driver *) &_devices_start;
    count = ((unsigned int) (&_devices_end) - (unsigned int) (&_devices_start)) / sizeof(struct ca_device_driver);

    for (n = 0; n < count; n++) {
        if (strncmp(base[n].device_name, (const char *) dev->device_name, strlen(base[n].device_name)) == 0) {
            if (NULL != base[n].function) {
                base[n].function(dev);
            }
        }
    }

    return RT_EOK;
}

/*****************************************************************/
/**
 * Function:       devices_init
 * Description:    初始化 devices
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int32_t devices_init(void) {
    int index = 0;

    /* list init */
    rt_list_init(&bus_list_head.node);
    rt_list_init(&device_list_head.node);

    for (index = 0; index < DEVICE_FILE_MAX; index++) {
        dev_file[index] = NULL;
    }

    return RT_EOK;
}

/*****************************************************************/
/**
 * Function:       devices_deInit
 * Description:    释放 devices 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int32_t devices_deinit(void) {
    return RT_EOK;
}

int32_t section_index_bus_driver(const struct bus_info *bus_common) {
    struct ca_bus *       p_bus = NULL;
    struct ca_bus_driver *base  = NULL;
    unsigned short        count = 0;
    int                   n     = 0;
    /* multi */
    base  = (struct ca_bus_driver *) &_bus_start;
    count = ((unsigned int) (&_bus_end) - (unsigned int) (&_bus_start)) / sizeof(struct ca_bus_driver);

    for (n = 0; n < count; n++) {
        if (strncmp(base[n].bus_name, (const char *) bus_common->bus_name, strlen(base[n].bus_name)) == 0) {
            if (NULL != base[n].function) {
                base[n].function(bus_common, &p_bus);
                return RT_EOK;
            }
        }
    }
    return -RT_ERROR;
}

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */
/* @} Robot_HAL */
