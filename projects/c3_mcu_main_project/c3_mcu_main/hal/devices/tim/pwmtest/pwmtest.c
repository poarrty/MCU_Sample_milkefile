/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-25
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-9-25       曾曼云	     0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "devices.h"
#include "define.h"
#include "tim_core.h"
#include "gpio_core.h"
#include <stddef.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal.h"
#include "pwmtest.h"
#include "delay.h"
// #include "tim.h"

/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_GPIO_CORE  - GPIO_CORE
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
int32_t pwmtest_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t pwmtest_close(const struct ca_device *dev)
{
	return 0;
}

/*****************************************************************/
/**
 * Function:       gpio_read
 * Description:    
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - -1 表示失败
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
int32_t pwmtest_read(const struct ca_device *dev, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

/*****************************************************************/
/**
 * Function:       gpio_write
 * Description:    
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - -1 表示失败
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
int32_t pwmtest_write(const struct ca_device *dev, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

int32_t pwmtest_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
    int32_t ret = -1;
	if(NULL == dev)
	{
		return -1;
	}
	
	switch(cmd)
	{
		case 0:
			break;
		default:
			break;
	}
	
	return 0;
}



int32_t pwmtest_driver_init(struct ca_device *dev)
{
	if(NULL == dev)
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = pwmtest_open;
	dev->ops.close = pwmtest_close;
	dev->ops.read = pwmtest_read;
	dev->ops.write = pwmtest_write;
	dev->ops.ioctl = pwmtest_ioctl;

  tim_pwm_start(dev->bus,200,0);
	
	return 0;
}




DEVICE_DRIVER_INIT(pwmtest,tim, pwmtest_driver_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */
/* @} Robot_DEVICES */

