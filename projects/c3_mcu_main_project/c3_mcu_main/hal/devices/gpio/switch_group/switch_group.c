/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         张博炜
  ** Version:        V0.0.1
  ** Date:           2021-11-5
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-10 张博炜 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-9-25       张博炜	     0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "switch_group.h"
#include "gpio_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "mem_pool.h"
#include "sw1.h"
#include "gpio_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "define.h"
/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_switch_group  - switch_group
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

/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/
#define SYS_GET_BIT(val, bit)                           ((val >> bit) & 1)
/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/

/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/
int32_t switch_group_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t switch_group_close(const struct ca_device *dev)
{
	return 0;
}

int32_t switch_group_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t switch_group_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return 0;
}

//对每个切换开关的具体操作
int32_t switch_group_unit_ioctl(struct ca_bus *ca_bus_array_p,  uint32_t cmd,void *arg)
{
	if(NULL == ca_bus_array_p)
	{
		return -1;
	}
	int sw_num = 1 ;
	int ret = 0;
	//计算当前切换开关最大量程
	for(int i = 0;i < SWITCH_MAX_GPIO_NUM;i++)
	{
		if(ca_bus_array_p[i].handler != NULL)
			sw_num*=2;			
	}
	//若判断cmd大于当前切换开关量程 则当前切换开关全置于0 把cmd减去当前切换开工最大量程后的值传给下一个切换开关处理
	if(cmd > sw_num - 1)
	{
		ret = cmd - sw_num +1;
		cmd = 0;
	}
	//根据结果处理io
	for(int i = 0;i < SWITCH_MAX_GPIO_NUM;i++)
	{
		SYS_GET_BIT(cmd, i) ? gpio_ioctl(&ca_bus_array_p[i], GPIO_ACTIVE_HIGH, arg) : gpio_ioctl(&ca_bus_array_p[i], GPIO_ACTIVE_LOW, arg);			
	}
	//把cmd减去当前切换开工最大量程后的值传给下一个切换开关处理
	return ret;
}

//对切换开关群进行操作 比如两个 二选四的 0=0000 1＝0001 2=0010 3=0011 4＝0100 5＝1000 6＝1100
int32_t switch_group_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	if(NULL == dev)
	{
		return -1;
	}
	struct ca_bus *p = (struct ca_bus *)dev->bus->handler;  
	int diff_cmd = 0;
	for (int i=0;i<GPIO_BE_REGISTERED_NUM;i = i+SWITCH_MAX_GPIO_NUM)
	{
		diff_cmd = switch_group_unit_ioctl(p+i,cmd,arg);
		//把cmd减去当前切换开工最大量程后的值传给下一个切换开关处理
		cmd = diff_cmd;
	}
	return 0;
}


/*****************************************************************/
/**
 * Function:       switch_group_init
 * Description:    初始化 switch_group
 * Calls:
 * Calswitch_group By:
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
int32_t switch_group_init(struct ca_device *dev)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = switch_group_open;
	dev->ops.close = switch_group_close;
	dev->ops.read = switch_group_read;
	dev->ops.write = switch_group_write;
	dev->ops.ioctl = switch_group_ioctl;

	struct ca_bus *bus = NULL;

	switch_group_bus_find_name(dev,&bus);
	dev->bus->handler = bus;
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       switch_group_deinit
 * Description:    去初始化 switch_group
 * Calls:
 * Calswitch_group By:
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
int32_t switch_group_deinit(struct ca_device *dev)
{
	return 0;
}

/*****************************************************************/
/**
 * Function:       switch_group_bus_find_name
 * Description:    把输入的gpio bus_info 数组转换成 gpio ca_bus数组
 * Calls:
 * Calswitch_group By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 
 *
 */
/******************************************************************/
int32_t switch_group_bus_find_name(struct ca_device *dev,struct ca_bus **my_bus)
{
	struct ca_bus *bus =NULL;
	if(dev->device_args == NULL )
	{
		return -1;
	}
	struct bus_switch_group *p = (struct bus_switch_group *)dev->device_args;//将切换开关群的bus_info数组取出
	int gpio_num = GPIO_BE_REGISTERED_NUM;
	struct ca_bus *ca_bus_array = mem_block_alloc(sizeof(struct ca_bus)*gpio_num);
	struct bus_info *p2 =(struct bus_info *)p;
	//将元素为bus_info类型的数组转换成ca_bus类型的数组
	for(int i = 0;i<gpio_num;i++)
	{
		bus_find_name(p2+i,&bus);
		if(bus != NULL)
			ca_bus_array[i] = *bus;
		else
			ca_bus_array[i].handler =NULL;
		bus =NULL;
	}

	*my_bus = ca_bus_array;
	
	return 0;
}



DEVICE_DRIVER_INIT(switch_group, gpio, switch_group_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_switch_group */
/* @} Robot_DEVICES */


