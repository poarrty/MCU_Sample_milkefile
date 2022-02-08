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

#include "uart_core.h"
#include "main.h"
#include "devices.h"
#include "define.h"
#include "sys_list.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stdbool.h"


/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_UART_CORE  - UART_CORE
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
struct uart_node{
	struct list_struct node;
	uint8_t rx_buf;
	uart_rx_callback callback;
	const struct ca_device *dev;
};

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
struct list_struct uart_rx_callback_list = {.next = &uart_rx_callback_list, .prev = &uart_rx_callback_list};

/*****************************************************************
* 外部变量声明
******************************************************************/
extern osSemaphoreId_t microros_tx_empty_id;

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
	struct list_struct *p_list = NULL;
	struct uart_node *p_uart = NULL;
	const struct ca_device *p_dev = NULL;
	struct rs485_handler *p_rs485 = NULL;

	p_list = &uart_rx_callback_list;
	while(list_is_last(p_list, &uart_rx_callback_list) != 1)
	{
	  p_uart = (struct uart_node*)p_list->next;  
		p_dev = p_uart->dev;
		
		if((NULL == p_dev)||(NULL == p_dev->bus)||(NULL == p_dev->bus->handler))
		{
			p_list = p_list->next;
			continue;
		}

		p_rs485 = (struct rs485_handler *)p_dev->bus->handler;
		
		if((p_dev->bus->handler == (void *)huart) || (p_rs485->uart->handler == (void *)huart))
		{
			if(NULL != p_uart->callback)
			{
				p_uart->callback(p_dev, p_uart->rx_buf);
			}

			HAL_UART_Receive_IT(huart, &p_uart->rx_buf, 1);
			break;
		}

		p_list = p_list->next;
	}

}

int32_t uart_rx_register_callback(const struct ca_device *dev, uart_rx_callback callback)
{
	struct uart_node *p_uart = NULL;
	struct rs485_handler *p_rs485 = NULL;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == callback))
	{
		return -1;
	}
	
	p_uart = (struct uart_node *)mem_block_alloc(sizeof(struct uart_node));
	if(NULL == p_uart)
	{
		return -1;
	}

	memset(p_uart, 0, sizeof(struct uart_node));

	
	if((NULL == uart_rx_callback_list.next) && (NULL == uart_rx_callback_list.next))
	{
		list_head_init(&uart_rx_callback_list);
	}

	p_uart->dev = dev;
	p_uart->callback = callback;
	
	list_add_tail(&p_uart->node, &uart_rx_callback_list);

	if(0 == strncmp((char *)dev->bus->bus_name, "rs485", strlen((char *)"rs485")))
	{
		p_rs485 = dev->bus->handler;
		if((NULL != p_rs485->uart) && (NULL != p_rs485->uart->handler))
		{
			HAL_UART_Receive_IT((UART_HandleTypeDef *)p_rs485->uart->handler, &p_uart->rx_buf, 1);
		}
	}
	else
	{
		HAL_UART_Receive_IT((UART_HandleTypeDef *)dev->bus->handler, &p_uart->rx_buf, 1);
	}
	
	return 0;
}

/*****************************************************************/
/**
 * Function:			 uart_read
 * Description: 	 
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *	- 0 表示成功
 *	- -1 表示失败
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
int32_t uart_read(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

/*****************************************************************/
/**
 * Function:			 uart_write
 * Description: 	 
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *	- 0 表示成功
 *	- -1 表示失败
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
int32_t uart_write(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	if(HAL_OK != HAL_UART_Transmit(bus->handler, (uint8_t *)buffer, len, 1000))
	{
		return -1;
	}

	return 0;
}

/*****************************************************************/
/**
 * Function:			 rs485_read
 * Description: 	 
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *	- 0 表示成功
 *	- -1 表示失败
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
int32_t rs485_read(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	struct rs485_handler *handler = NULL;
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	handler = (struct rs485_handler *)bus->handler;
	
	return uart_read(handler->uart, buffer, len);

}

/*****************************************************************/
/**
 * Function:			 rs485_write
 * Description: 	 
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *	- 0 表示成功
 *	- -1 表示失败
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
int32_t rs485_write(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	struct rs485_handler *handler = NULL;
	int8_t write_status;
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	handler = (struct rs485_handler *)bus->handler;

	//设置为发送模式
	if(handler->gpio_read_status == GPIO_ACTIVE_HIGH)
	{
		write_status = GPIO_ACTIVE_LOW;
	}
	else{
		write_status = GPIO_ACTIVE_HIGH;
	}
	
	gpio_write(handler->gpio_ctrl, (void *)&write_status, sizeof(write_status));
	
	uart_write(handler->uart, buffer, len);

	//设置为接收模式
	gpio_write(handler->gpio_ctrl, (void *)&handler->gpio_read_status, sizeof(handler->gpio_read_status));
	return 0;
}

int32_t rs485_init(struct bus_rs485 *rs485, struct ca_bus **bus)
{
	struct rs485_handler *handler = NULL;
	struct ca_bus *p_bus = NULL;
	
	if(NULL == rs485)
	{
		return -1;
	}

	handler = mem_block_alloc(sizeof(struct rs485_handler));
	if(NULL == handler)
	{
		return -1;
	}
	
	if(bus_find_name(&rs485->uart, &handler->uart))
	{
		return -1;
	}

	if(bus_find_name(&rs485->gpio_ctrl, &handler->gpio_ctrl))
	{
		return -1;
	}

	p_bus = (struct ca_bus *)mem_block_alloc(sizeof(struct ca_bus));
	if(NULL == p_bus)
	{
		return -1;
	}

	memset(p_bus, 0, sizeof(struct ca_bus));
	memcpy((char *)p_bus->bus_name, rs485->common.bus_name, strlen((char *)rs485->common.bus_name));
	handler->gpio_read_status = rs485->gpio_read_status;
	p_bus->handler = handler;

	*bus = p_bus;

	//设置为接收状态
	gpio_write(handler->gpio_ctrl, (void *)&handler->gpio_read_status, sizeof(handler->gpio_read_status));

	return 0;
}

BUS_DRIVER_INIT(rs485, rs485_init);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	bool bTaskWoken = false;
  /* Prevent unused argument(s) compilation warning */

  if (huart->Instance == USART1)
  {
    osSemaphoreRelease(microros_tx_empty_id);
  }
  portEND_SWITCHING_ISR(bTaskWoken ? pdTRUE : pdFALSE);
}



#ifdef __cplusplus
}
#endif

/* @} Robot_UART_CORE */
/* @} Robot_DEVICES */

