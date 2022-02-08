/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         liuchunyang
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
#include "bms_dwg01.h"
#include "devices.h"
#include "define_bms.h"
#include "uart_core.h"
#include "Modbus.h"
#include <stddef.h>
#include <string.h>
#include "mem_pool.h"
#include "log.h"

/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_BMS_DWG01  - BMS_DWG01
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
#define BMS_DWG01_ADDR 0X01
#define read_hold_register 0X03
#define write_hold_register 0X06

#define DWG_REGISTER_ERROR		0x2000			//电池错误信息寄存器地址
#define DWG_REGISTER_VOLTAGE	0x1020			//电池电压寄存器地址
#define DWG_REGISTER_CURRENT	0x1200			//电池电流寄存器地址(包含充电时电流和放电时电流)
#define DWG_REGISTER_CAPACITY	0x1300			//电池容量寄存器地址(包含百分比,满电容量，当前容量)
#define DWG_REGISTER_PARAM		0x3002			//电池参数寄存器地址(包含电池类型，设计容量，正常电压值)
#define DWG_REGISTER_MODE			0x4000			//电池模式寄存器地址(包含开关充电、开关放电模式)

/*****************************************************************
 * 私有结构体/共用体/枚举定义
 ******************************************************************/
struct dwg_args{
	BMS_DATA dwg_data;								 //电池通用参数
	modbusHandler_t modbus_handle;		 //modbus处理句柄
	uint16_t modbus_data[8];					 //modbus缓冲数据
	osMutexId_t mutex_bms_Handle; 		 //互斥锁ID
	osThreadId_t task_bms_Handle; 		 //线程ID
};

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
 ******************************************************************/
const osThreadAttr_t bms_dwg01_thread_attributes = {
	.name = "bms_dwg01_thread",
	.priority = (osPriority_t) osPriorityNormal,
	.stack_size = 256*4
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
void bms_rx_callback(const struct ca_device *dev, uint8_t rx_buf) 
{
	struct dwg_args *p_args = NULL;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if ((NULL == dev) || (NULL == dev->device_args)) 
	{
			return;
	}

	p_args = (struct dwg_args *)dev->device_args;

	RingAdd(&p_args->modbus_handle.xBufferRX, rx_buf);
	
	xTimerResetFromISR(p_args->modbus_handle.xTimerT35, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  
	
}

//读取电池错误信息
int32_t dwg01_get_error(const struct ca_device *dev, BMS_ERROR *error) 
{
	modbus_t telegram;
	struct dwg_args *p_args = NULL;
	uint32_t u32NotificationValue = 0;
	
	if ((NULL == dev) || (NULL == dev->device_args)) 
	{
			return -1;
	}

	p_args = (struct dwg_args *)dev->device_args;

	telegram.u8id = BMS_DWG01_ADDR;												//从机地址
  telegram.u8fct = 3; 																		//读操作(0x3是读, 0x6是写)
  telegram.u16RegAdd = DWG_REGISTER_ERROR; 							//寄存器地址
  telegram.u16CoilsNo = 1;																//数量
  telegram.u16reg = p_args->modbus_data;									//缓存地址

	ModbusQuery(&p_args->modbus_handle, telegram);
	
	u32NotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block until query finishes
  if(u32NotificationValue != ERR_OK_QUERY)
  {
		return -1;
  }

	//TODO:解析p_args->modbus_data，获取错误信息

	return 0;
}

//读取电池电压
int32_t dwg01_get_voltage(const struct ca_device *dev, uint16_t *voltage) 
{
	return 0;
}

//读取电池电流(包含放电电流和充电电流)
int32_t dwg01_get_current(const struct ca_device *dev, uint16_t *current, uint16_t *charge) 
{
	return 0;
}

//读取电池容量(包含百分比,满电容量，当前容量)
int32_t dwg01_get_capacity(const struct ca_device *dev, uint16_t *percentage, uint16_t *full_capacity, uint16_t *current_capacity) 
{
	
	return 0;
}

//读取电池参数(包含电池类型，设计容量，正常电压值)
int32_t dwg01_get_param(const struct ca_device *dev, BMS_TECHNOLOGY *type, uint16_t *design_capacity, uint16_t *normal_voltage) 
{
	return 0;
}

//设置功能模式(包含开关充电、开关放电模式)
int32_t dwg01_set_mode(const struct ca_device *dev, uint16_t mode) 
{
	return 0;
}

//初始化modbus
int32_t dwg01_modbus_init(const struct ca_device *dev, modbusHandler_t *modbus_handle)
{
	struct dwg_args *p_args = NULL;
	struct rs485_handler *p_485 = NULL;

	if((NULL == dev) || (NULL == dev->bus) ||(NULL == dev->device_args) || (NULL == modbus_handle))
	{
		return -1;
	}

	p_485 = (struct rs485_handler *)dev->bus->handler;
	if((NULL == p_485->uart) || (NULL == p_485->gpio_ctrl))
	{
		return -1;
	}
	
	p_args = (struct dwg_args *)dev->device_args;
	
	modbus_handle->uModbusType = MB_MASTER;
  modbus_handle->port = p_485->uart->handler;
  modbus_handle->u8id = 0; // For master it must be 0
  modbus_handle->u16timeOut = 1000;
  modbus_handle->EN_Port = NULL;
	modbus_handle->EN_Port = p_485->gpio_ctrl->handler;
  modbus_handle->EN_Pin = p_485->gpio_ctrl->bus_addr;
  modbus_handle->u16regs = p_args->modbus_data;
  modbus_handle->u16regsize= sizeof(p_args->modbus_data)/sizeof(p_args->modbus_data[0]);
  modbus_handle->xTypeHW = USART_HW;
	
  ModbusInit(modbus_handle);
  ModbusStart(modbus_handle);
	
	return 0;
}

void bms_dwg01_run(void *argument) 
{
	while(1)
	{
		
	}
}

int32_t bms_dwg01_open(const struct ca_device *dev, int32_t flags) {
    return 0;
}

int32_t bms_dwg01_close(const struct ca_device *dev) {
    return 0;
}

int32_t bms_dwg01_read(const struct ca_device *dev, void *buffer, uint32_t size) 
{
	struct dwg_args *p_args = NULL;

	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	if(size < sizeof(BMS_DATA))
	{
		return -1;
	}

	p_args = (struct dwg_args *)dev->device_args;

  osMutexAcquire(p_args->mutex_bms_Handle, 100);  //加锁

	memcpy(buffer, &p_args->dwg_data, sizeof(BMS_DATA));
	
  osMutexRelease(p_args->mutex_bms_Handle);  //释放锁
  
  return 0;
}

int32_t bms_dwg01_write(const struct ca_device *dev, void *buffer, uint32_t size) 
{
    if ((NULL == dev)|| (NULL == buffer)) 
		{
        return -1;
    }

    return 0;
}

int32_t bms_dwg01_ioctl(const struct ca_device *dev, uint32_t cmd, void *arg) 
{
	if (NULL == dev) 
	{
		return -1;
	}
	
	switch (cmd) 
	{
		case BMS_SET_MODE:
			//return dwg01_set_mode(dev, arg);
			break;
		default:
	    LOG_DEBUG("BMS_CMD is error!\r\n");
	    break;
	}
	
	return -1;
}

/*****************************************************************/
/**
 * Function:       bms_init
 * Description:    初始化 bms
 * Calls:
 * Calbms By:
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
int32_t bms_dwg01_init(struct ca_device *dev) 
{
	struct dwg_args *p_args = NULL;
	
  if ((NULL == dev) || (NULL == dev->bus) || (NULL == dev->bus->handler)) 
	{
      return -1;
  }
	
  memset(&dev->ops, 0, sizeof(struct ca_device_ops));

  dev->ops.open = bms_dwg01_open;
  dev->ops.close = bms_dwg01_close;
  dev->ops.read = bms_dwg01_read;
  dev->ops.write = bms_dwg01_write;
  dev->ops.ioctl = bms_dwg01_ioctl;

	p_args = (struct dwg_args *)mem_block_alloc(sizeof(struct dwg_args));
	if(NULL == p_args)
	{
		return -1;
	}

	memset(p_args, 0, sizeof(struct dwg_args));
	
	//互斥锁
	osMutexAttr_t mutex_bms_attributes;
	memset(&mutex_bms_attributes, 0x0, sizeof(osMutexAttr_t));
	mutex_bms_attributes.name = "mutex_bms";
	p_args->mutex_bms_Handle = osMutexNew(&mutex_bms_attributes);

	p_args->task_bms_Handle = osThreadNew(bms_dwg01_run, (void *)dev, &bms_dwg01_thread_attributes);
	
	dwg01_modbus_init(dev, &p_args->modbus_handle);
  uart_rx_register_callback(dev, bms_rx_callback);

  return 0;
}

/*****************************************************************/
/**
 * Function:       bms_deinit
 * Description:    去初始化 bms
 * Calls:
 * Calbms By:
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
int32_t bms_dwg01_deinit(struct ca_device *dev) {
    return 0;
}

DEVICE_DRIVER_INIT(bms_dwg01, rs485, bms_dwg01_init);

#ifdef __cplusplus
}
#endif

/* @} Robot_BMS_DWG01 */
/* @} Robot_DEVICES */

