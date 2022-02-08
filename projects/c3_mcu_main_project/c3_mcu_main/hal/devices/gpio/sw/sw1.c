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
 * @defgroup Robot_sw1  - sw1
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
#define SYS_GET_BIT(val, bit)                           ((val >> bit) & 1)
/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/

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
int32_t sw1_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t sw1_close(const struct ca_device *dev)
{
	return 0;
}

int32_t sw1_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t sw1_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return 0;
}

int32_t sw1_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	if(NULL == dev)
	{
		return -1;
	}
	
	//return gpio_ioctl(handler->gpio_ctrl[0], cmd, arg);
	struct sw_handler *handler = (struct sw_handler *)dev->bus->handler;
	SYS_GET_BIT(cmd, 0) ? gpio_ioctl(handler->gpio_ctrl[0], GPIO_ACTIVE_HIGH, arg) : gpio_ioctl(handler->gpio_ctrl[0], GPIO_ACTIVE_LOW, arg);
    SYS_GET_BIT(cmd, 1) ? gpio_ioctl(handler->gpio_ctrl[1], GPIO_ACTIVE_HIGH, arg) : gpio_ioctl(handler->gpio_ctrl[1], GPIO_ACTIVE_LOW, arg);
    SYS_GET_BIT(cmd, 2) ? gpio_ioctl(handler->gpio_ctrl[2], GPIO_ACTIVE_HIGH, arg): gpio_ioctl(handler->gpio_ctrl[2], GPIO_ACTIVE_LOW, arg);
	return 0;
}


/*****************************************************************/
/**
 * Function:       sw1_init
 * Description:    初始化 sw1
 * Calls:
 * Calsw1 By:
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
int32_t sw1_init(struct ca_device *dev)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = sw1_open;
	dev->ops.close = sw1_close;
	dev->ops.read = sw1_read;
	dev->ops.write = sw1_write;
	dev->ops.ioctl = sw1_ioctl;
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       sw1_deinit
 * Description:    去初始化 sw1
 * Calls:
 * Calsw1 By:
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
int32_t sw1_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(sw1, sw, sw1_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_sw1 */
/* @} Robot_DEVICES */


