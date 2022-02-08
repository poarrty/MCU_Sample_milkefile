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
#include "led.h"
#include "gpio_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "define_motor.h"
#include "log.h"
#include "mem_pool.h"


/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_LED  - LED
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
int32_t gpio_motor_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t gpio_motor_close(const struct ca_device *dev)
{
	return 0;
}

int32_t gpio_motor_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	uint8_t read_state_ret = 0;
	MOTOR_DATA motor_data;

	if (NULL == dev->bus)
	{
		LOG_DEBUG("read bdcm dev->bus error!");
		return -1;
	}
	gpio_read(dev->bus, &read_state_ret, sizeof(uint8_t));
	if (1 == ((MOTOR_BDCM_ATTR *)dev->device_args)->open_state)
	{
		((MOTOR_DATA *)buffer)->rpm = read_state_ret;
	}
	else if (0 == ((MOTOR_BDCM_ATTR *)dev->device_args)->open_state)
	{
		((MOTOR_DATA *)buffer)->rpm = !(0x01 & read_state_ret);
	}

	return 0;
}

int32_t gpio_motor_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return 0;
}



int8_t close_gpio_motor(const struct ca_device *dev)
{
	uint8_t state = 0;

	if (NULL == dev->bus)
	{
		LOG_DEBUG("clode bdcm dev->bus NULL!");
		return -1;
	}
    
    state = ((MOTOR_BDCM_ATTR *)dev->device_args)->open_state ? 0 : 1;
	gpio_write(dev->bus, (void*) &state, 1);

	return 0;
}

int8_t open_gpio_motor(const struct ca_device *dev)
{
	uint8_t state = 0;

	if (NULL == dev->bus)
	{
		LOG_DEBUG("open bdcm dev->bus NULL!");
		return -1;
	}
    
    state = ((MOTOR_BDCM_ATTR *)dev->device_args)->open_state ? 1 : 0;
	gpio_write(dev->bus, (void*) &state, 1);

	return 0;
}

int8_t set_speed_gpio_motor(const struct ca_device *dev,  MOTOR_SPEED motor_vw)
{
	if (0 == motor_vw.speed_v_t)
	{
		close_gpio_motor(dev);
	}
	else
	{
		open_gpio_motor(dev);
	}

	return 0;
}

int32_t gpio_motor_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	if(NULL == dev)
	{
		return -1;
	}

	switch (cmd)
	{
		case MOTOR_CMD_START:
			open_gpio_motor(dev);

			break;

		case MOTOR_CMD_STOP:
			close_gpio_motor(dev);
			break;

		case MOTOR_CMD_SET_SPEED:
			set_speed_gpio_motor(dev, *(MOTOR_SPEED*)arg);
			break;

		case MOTOR_CMD_GET_PARAM:
			
			return -1;
			break;


		default:
			break;
	}

	return 0;
}


/*****************************************************************/
/**
 * Function:       led_init
 * Description:    初始化 led
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
int32_t gpio_motor_init(struct ca_device *dev)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = gpio_motor_open;
	dev->ops.close = gpio_motor_close;
	dev->ops.read = gpio_motor_read;
	dev->ops.write = gpio_motor_write;
	dev->ops.ioctl = gpio_motor_ioctl;
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       bdcm_deinit
 * Description:    去初始化 led
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
int32_t gpio_motor_deinit(struct ca_device *dev)
{
	return 0;
}
DEVICE_DRIVER_INIT(gpio_motor, gpio, gpio_motor_init);

#ifdef __cplusplus
}
#endif

/* @} Robot_LED */
/* @} Robot_DEVICES */


