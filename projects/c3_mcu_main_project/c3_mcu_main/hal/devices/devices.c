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

/*****************************************************************
* 包含头文件
******************************************************************/
#include "devices.h"
#include "cmsis_os.h"
#include <string.h>
#include <stddef.h>
#include "define.h"
#include "sys_list.h"
#include "mem_pool.h"

/**
 * @addtogroup Robot_HAL
 * @{
 */

/**
 * @defgroup Robot_DEVICES  - DEVICES
 *
 * @brief  \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
* 私有宏定义
******************************************************************/
#define DEVICE_FILE_MAX	32

/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/

/*****************************************************************
* 全局变量定义
******************************************************************/
struct list_struct ca_bus_list;
struct list_struct ca_device_list;

struct ca_device *dev_file[DEVICE_FILE_MAX];

osMutexId_t mutex_devices = NULL;

const osMutexAttr_t mutex_devices_attr_ = {
    "mutex_devices",                       // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};


/*****************************************************************
* 私有全局变量定义
******************************************************************/
extern const unsigned int _devices_start;
extern const unsigned int _devices_end;

extern const unsigned int _bus_start;
extern const unsigned int _bus_end;


/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/
int32_t bus_register(const uint8_t *bus_name, void *handler)
{
	struct list_struct *p_list = NULL;
	struct ca_bus *p_bus = NULL;
	
	if((NULL == bus_name) || (NULL == handler))
	{
		return -1;
	}

	p_list = &ca_bus_list;
	while(list_is_last(p_list,&ca_bus_list) != 1)
	{
    p_bus = (struct ca_bus*)p_list->next;  
		if(0 == strcmp((char *)p_bus->bus_name, (char *)bus_name))
		{
			//总线已注册
//			log_i("bus[%s] is already registed.", (char *)bus_name);
			return -1;
		}
		p_list = p_list->next;
	}

	p_bus = (struct ca_bus *)mem_block_alloc(sizeof(struct ca_bus));
	if(NULL == p_bus)
	{
		return -1;
	}

	memset(p_bus, 0, sizeof(struct ca_bus));
	memcpy((char *)p_bus->bus_name, bus_name, strlen((char *)bus_name));
	p_bus->handler = handler;
	
	list_add_tail(&p_bus->node, &ca_bus_list);	

	return 0;
}

int32_t bus_unregister(const uint8_t *bus_name)
{
	return 0;
}

int32_t bus_find_name(const struct bus_info *bus_common, struct ca_bus **bus)
{
	struct list_struct *p_list = NULL;
	struct ca_bus *p_bus = NULL;
	struct ca_bus_driver *base = NULL;                                             
  unsigned short count = 0;  
	int n = 0;
	
	if(NULL == bus_common)
	{
		return -1;
	}

	p_list = &ca_bus_list;
	while(list_is_last(p_list,&ca_bus_list) != 1)
	{
    p_bus = (struct ca_bus*)p_list->next;  
		if(0 == strcmp((char *)p_bus->bus_name, (char *)bus_common->bus_name))
		{
			*bus = mem_block_alloc(sizeof(struct ca_bus));
			if(NULL != *bus)
			{
				memcpy(*bus, p_bus, sizeof(struct ca_bus));
				(*bus)->bus_addr = bus_common->bus_addr;
			}
			return 0;
		}
		p_list = p_list->next;
	}

	//查找是否是特殊总线
	base = (struct ca_bus_driver*)&_bus_start;
  count = ((unsigned int)(&_bus_end) - (unsigned int)(&_bus_start)) / sizeof(struct ca_bus_driver);

	for(n = 0; n < count; n++)
	{
		if(strncmp(base[n].bus_name, (const char *)bus_common->bus_name, strlen(base[n].bus_name)) == 0)
		{
			if(NULL != base[n].function)	
			{
				base[n].function(bus_common, &p_bus);
				list_add_tail(&p_bus->node, &ca_bus_list);	
				
				*bus = mem_block_alloc(sizeof(struct ca_bus));
				if(NULL != *bus)
				{
					memcpy(*bus, p_bus, sizeof(struct ca_bus));
					(*bus)->bus_addr = bus_common->bus_addr;
				}
				
				return 0;
			}
		}
	}

	return -1;
}


int32_t device_open(const uint8_t *dev_name, int32_t flags)
{
	struct list_struct *p_list = NULL;
	struct ca_device *p_device = NULL;
	int8_t find_flag = -1;
	int index = 0;
	
	if(NULL == dev_name)
	{
		return -1;
	}

	p_list = &ca_device_list;
	while(list_is_last(p_list,&ca_device_list) != 1)
	{
    p_device = (struct ca_device*)p_list->next;  
		if(0 == strcmp((char *)p_device->device_name, (char *)dev_name))
		{
			find_flag = 0;
			break;
		}
		p_list = p_list->next;
	}

	if(find_flag < 0)
	{
		return -1;
	}

	for(index = 0; index < DEVICE_FILE_MAX; index++)
	{
		if(NULL == dev_file[index])
		{
			dev_file[index] = p_device;
			if(NULL != p_device->ops.open)
			{
				p_device->ops.open(p_device, flags);
			}

			return index;
		}
	}

	return -1;
}

int32_t device_close(const uint8_t dev_handle)
{
	struct ca_device *dev = NULL;
	
	if(dev_handle > DEVICE_FILE_MAX - 1)
	{
		return -1;
	}

	if(NULL == dev_file[dev_handle])
	{
		return -1;
	}

	dev = dev_file[dev_handle];
	dev_file[dev_handle] = NULL;
	
	if(NULL != dev->ops.close)
	{
		return dev->ops.close(dev);
	}
	
	return -1;
}

int32_t device_read(const uint8_t dev_handle, void *buffer, uint32_t size)
{
	struct ca_device *dev = NULL;
	
	if(dev_handle > DEVICE_FILE_MAX - 1)
	{
		return -1;
	}

	if(NULL == dev_file[dev_handle])
	{
		return -1;
	}

	dev = dev_file[dev_handle];

	if(NULL != dev->ops.read)
	{
		return dev->ops.read(dev, buffer, size);
	}
	return 0;
}

int32_t device_write(const uint8_t dev_handle, void *buffer, uint32_t size)
{
	struct ca_device *dev = NULL;
	
	if(dev_handle > DEVICE_FILE_MAX - 1)
	{
		return -1;
	}

	if(NULL == dev_file[dev_handle])
	{
		return -1;
	}

	dev = dev_file[dev_handle];

	if(NULL != dev->ops.write)
	{
		return dev->ops.write(dev, buffer, size);
	}
	return 0;
}

int32_t device_ioctl(const uint8_t dev_handle,  uint32_t cmd, void *arg)
{
	struct ca_device *dev = NULL;
	
	if(dev_handle > DEVICE_FILE_MAX - 1)
	{
		return -1;
	}

	dev = dev_file[dev_handle];

	if(NULL == dev_file[dev_handle])
	{
		return -1;
	}

	if(NULL != dev->ops.ioctl)
	{
		return dev->ops.ioctl(dev, cmd, arg);
	}
	return 0;
}

int32_t device_register(const struct bus_info *bus_common, const uint8_t *dev_name, void *args)
{
	struct ca_bus *bus = NULL;
	struct ca_device *devices = NULL;
	struct list_struct *p_list = NULL;
	struct ca_device *p_device = NULL;
	
	if(NULL == dev_name)
	{
		return -1;
	}
	
	if(NULL != bus_common)
	{
		if(bus_find_name(bus_common, &bus))
		{
			return -1;
		}

		if(NULL == bus)
		{
			return -1;
		}

	}

	if(!device_find(dev_name, &devices))
	{
		return -1;
	}

	p_device = (struct ca_device *)mem_block_alloc(sizeof(struct ca_device));
	if(NULL == p_device)
	{
		return -1;
	}

	memset(p_device, 0, sizeof(struct ca_device));
	memcpy((char *)p_device->device_name, dev_name, strlen((char *)dev_name));
	
	p_device->bus = bus;
	p_device->device_args = args;
	
	list_add_tail(&p_device->node, &ca_device_list);	
	if(device_driver_probe(p_device) < 0)
	{
		mem_block_free(p_device);
		return -1;
	}

	return 0;
}

int32_t device_unregister(const uint8_t *dev_name)
{
	return 0;
}

int32_t device_find(const uint8_t *dev_name, struct ca_device **dev)
{
	struct list_struct *p_list = NULL;
	struct ca_device *p_device = NULL;
	
	if(NULL == dev_name)
	{
		return -1;
	}

	p_list = &ca_device_list;
	while(list_is_last(p_list,&ca_device_list) != 1)
	{
    p_device = (struct ca_device*)p_list->next;  
		if(0 == strcmp((char *)p_device->device_name, (char *)dev_name))
		{
			*dev = p_device;
			return 0;
		}
		p_list = p_list->next;
	}

	return -1;
}

int32_t device_driver_probe(struct ca_device *dev)
{
	struct ca_device_driver *base = NULL;                                             
  unsigned short count = 0;  
	int n = 0;
	
	if((NULL == dev) || (NULL == dev->bus))
	{
		return -1;
	}              

	base = (struct ca_device_driver*)&_devices_start;
  count = ((unsigned int)(&_devices_end) - (unsigned int)(&_devices_start)) / sizeof(struct ca_device_driver);

	for(n = 0; n < count; n++)
	{
		if(strncmp(base[n].device_name, (const char *)dev->device_name,strlen(base[n].device_name)) == 0)
		{
			if(NULL != base[n].function)	
			{
				base[n].function(dev);
			}
		}
	}
	
	return 0;
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
int32_t devices_init(void)
{
	int index = 0;
	
	/*添加模块处理函数*/
	list_head_init(&ca_bus_list);
	list_head_init(&ca_device_list);

	for(index = 0; index < DEVICE_FILE_MAX; index++){
		dev_file[index] = NULL;
	}

	return 0;

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
int32_t devices_deinit(void)
{
	return 0;

}

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */
/* @} Robot_HAL */

