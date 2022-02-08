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
  ** <adce>          <author>    <version >    <desc>
  ** 2021-9-25       曾曼云	     0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "devices.h"
#include "define.h"
#include "adc_core.h"
#include "tim_core.h"
#include "gpio_core.h"
#include <stddef.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal.h"
#include "cliff_oml.h"
#include "delay.h"
#include "adc.h"
#include "log.h"
#include "mem_pool.h"
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
typedef StaticTask_t osStaticThreadDef_t;
struct cliff_oml_args
{
										
	struct ca_bus  *cliff_oml_bus;						//断崖的adc总线
	uint8_t cliff_status;								//检测断崖状态
	uint8_t cliff_collect_flag;							//开始采集信号标记位

};
/*****************************************************************
* 全局变量定义
******************************************************************/


/*****************************************************************
* 私有全局变量定义
******************************************************************/
osThreadAttr_t cliff_oml_attributes = {
  .name = "cliff_oml",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};



/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/
int32_t cliff_oml_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t cliff_oml_close(const struct ca_device *dev)
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
int32_t cliff_oml_read(const struct ca_device *dev, void *buffer, uint32_t len)
{
	//进来的buffer是个uint_8

	int32_t ret = -1;
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}
	struct cliff_oml_args *cliff_args = NULL;
	cliff_args = (struct cliff_oml_args *)dev->device_args;
	if(NULL==cliff_args)
	{
		return -1;
	}
	memcpy(buffer, (void *)&cliff_args->cliff_status, sizeof(uint8_t));

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
int32_t cliff_oml_write(const struct ca_device *dev, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

int32_t cliff_oml_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
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

void cliff_oml_it_callback(const struct ca_device *dev,void* handle)
{
	struct cliff_oml_args *cliff_args = NULL;
	cliff_args = (struct cliff_oml_args *)dev->device_args;
	cliff_args->cliff_collect_flag =1;
	return ;
}

void cliff_oml_run(void *argument)
{
	int ret1 = 0;
	int ret2 = 0;
	struct ca_bus *cliff_oml_adc = NULL;
	struct ca_device *dev =NULL;
	struct cliff_oml_args *cliff_args = NULL;
	dev = (struct ca_device *)argument;
	cliff_args = (struct cliff_oml_args *)dev->device_args;
    cliff_oml_adc = cliff_args->cliff_oml_bus;
	while(1)
	{
		if(cliff_args->cliff_collect_flag == 1)
		{
			my_delay(1000);
			ret1 = adc_read(cliff_oml_adc,cliff_oml_adc->bus_addr);
			//LOG_DEBUG("%s>>>>>>>>>>>>>> %.4f \r\n",dev->device_name,ret1*3.3f/4096);
			my_delay(400);
			ret2 = adc_read(cliff_oml_adc,cliff_oml_adc->bus_addr);;
			double v_diff = (ret2-ret1)*3.3f/4096;
			if(v_diff > 0)
			{
				//LOG_DEBUG(">>>>>>>>>>>>>> %.4f \r\n",v_diff);
				//LOG_DEBUG("%s>>>>>>>>>>>>>> %.4f \r\n",dev->device_name,v_diff);
				if(v_diff>=0.005)
				{
					cliff_args->cliff_status = 0;
				}
				else
				{
					cliff_args->cliff_status = 1;
				}
			}
			cliff_args->cliff_collect_flag = 0;
		}
		
		osDelay(100);
	}
}

int32_t cliff_oml_driver_init(struct ca_device *dev)
{
	if(NULL == dev)
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = cliff_oml_open;
	dev->ops.close = cliff_oml_close;
	dev->ops.read = cliff_oml_read;
	dev->ops.write = cliff_oml_write;
	dev->ops.ioctl = cliff_oml_ioctl;

  	tim_pwm_start(dev->bus,0.6,0);

	struct ca_bus *bus = NULL;

	struct cliff_oml_args *cliff_oml_args = NULL;
	cliff_oml_args = (struct cliff_oml_args *)mem_block_alloc(sizeof(struct cliff_oml_args));
	memset(cliff_oml_args, 0, sizeof(struct cliff_oml_args));
    if(bus_find_name(dev->device_args, &bus))
	{
		return -1;
	}
	cliff_oml_args->cliff_oml_bus = bus;
  	dev->device_args = (void *)cliff_oml_args;
	tim_period_elapsed_start(dev->bus);
	tim_period_elapsed_register_callback(dev,NULL,cliff_oml_it_callback);
	osThreadId_t cliff_oml_thread;
	cliff_oml_thread = osThreadNew(cliff_oml_run, (void *)dev, &cliff_oml_attributes);
	return 0;
}

DEVICE_DRIVER_INIT(cliff_oml,tim, cliff_oml_driver_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */
/* @} Robot_DEVICES */

