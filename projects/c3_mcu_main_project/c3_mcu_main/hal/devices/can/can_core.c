/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-11-01
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

#include "can_core.h"
#include "main.h"
#include "devices.h"
#include "define.h"
#include "sys_list.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>


/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_CAN_CORE  - CAN_CORE
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
struct can_node{
	struct list_struct node;
	can_rx_callback callback;
	uint32_t filter_bank;
	const struct ca_device *dev;
};

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
struct list_struct can_rx_fifo_callback_list = {.next = NULL, .prev = NULL};

/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/

void can_fifo_callback(CAN_HandleTypeDef *hcan, int32_t fifo_num)
{
	struct list_struct *p_list = NULL;
	struct can_node *p_can = NULL;
	const struct ca_device *p_dev = NULL;

	p_list = &can_rx_fifo_callback_list;
	while(list_is_last(p_list, &can_rx_fifo_callback_list) != 1)
	{
		p_can = (struct can_node*)p_list->next;  
		p_dev = p_can->dev;
		
		if((NULL == p_dev)||(NULL == p_dev->bus)||(NULL == p_dev->bus->handler))
		{
			p_list = p_list->next;
			continue;
		}

		if(p_dev->bus->handler == (void *)hcan)
		{
			if((NULL != p_can->callback) && (p_dev->bus->bus_addr == fifo_num))
			{
				p_can->callback(p_dev);
			}		
			break;
		}

		p_list = p_list->next;
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	can_fifo_callback(hcan, 0);
	HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	can_fifo_callback(hcan, 1);
	HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

int32_t can_rxfifo_register_callback(const struct ca_device *dev, can_rx_callback callback)
{
	struct can_node *p_can = NULL;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == callback))
	{
		return -1;
	}
	
	p_can = (struct can_node *)mem_block_alloc(sizeof(struct can_node));
	if(NULL == p_can)
	{
		return -1;
	}

	memset(p_can, 0, sizeof(struct can_node));
	
	if((NULL == can_rx_fifo_callback_list.next) && (NULL == can_rx_fifo_callback_list.prev))
	{
		list_head_init(&can_rx_fifo_callback_list);
	}

	p_can->dev = dev;
	p_can->callback = callback;
	
	list_add_tail(&p_can->node, &can_rx_fifo_callback_list);
	
	return 0;
}

int32_t can_filter_config(struct ca_bus *bus, uint32_t filter_id, uint32_t filter_ext_id)
{
	uint32_t filter_bank = 0;
	
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	if(0 == strcmp((char *)bus->bus_name, (char *)"can2"))
	{
		filter_bank = 14;
	}
	else
	{
		filter_bank = 0;
	}
	
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = filter_bank;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = filter_id & 0x1FFFFFFF;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = (uint32_t)bus->bus_addr;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(bus->handler, &sFilterConfig) != HAL_OK) 
	{
			return -1;
	}

	if (HAL_CAN_Start(bus->handler) == HAL_OK) 
	{
			//log_i("can start success!\r\n");
			if(bus->bus_addr == 1)
			{
				HAL_CAN_ActivateNotification(bus->handler, CAN_IT_RX_FIFO1_MSG_PENDING);  ///< 接收处理完成，恢复接收中断
			}
			else 
			{
				HAL_CAN_ActivateNotification(bus->handler, CAN_IT_RX_FIFO0_MSG_PENDING);  ///< 接收处理完成，恢复接收中断
			}     
	} else {
			return -1;
	}
	
	return 0;
}


int32_t can_read(const struct ca_bus *bus, uint32_t *id, void *buffer, uint32_t len)
{
	CAN_RxHeaderTypeDef rx_header;
	int32_t ret = -1;
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	if(len < 8)
	{
		return -1;
	}

	ret = HAL_CAN_GetRxFifoFillLevel(bus->handler, (uint32_t)bus->bus_addr);
	if(ret > 0)
	{
		if (HAL_CAN_GetRxMessage(bus->handler, (uint32_t)bus->bus_addr, &rx_header, buffer) != HAL_OK)
		{
			return -1;
		}
		
		*id = rx_header.StdId;
		ret = rx_header.DLC;
	}

	if(bus->bus_addr == 1)
	{
		HAL_CAN_ActivateNotification(bus->handler, CAN_IT_RX_FIFO1_MSG_PENDING);	///< 接收处理完成，恢复接收中断
	}
	else 
	{
		HAL_CAN_ActivateNotification(bus->handler, CAN_IT_RX_FIFO0_MSG_PENDING);	///< 接收处理完成，恢复接收中断
	}

	return ret;
}

int32_t can_write(const struct ca_bus *bus, uint16_t id, void *buffer, uint32_t len)
{
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}
	
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;

	TxHeader.StdId = id;
	TxHeader.ExtId = id;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = len;
	TxHeader.TransmitGlobalTime = DISABLE;

	if (HAL_CAN_GetTxMailboxesFreeLevel(bus->handler) == 0) {
	    return 1;
	}

	if (HAL_CAN_AddTxMessage(bus->handler, &TxHeader, buffer, &TxMailbox) == HAL_OK) 
	{
	    return 0;
	} 
	else 
	{
	    return 1;
	}
}


#ifdef __cplusplus
}
#endif

/* @} Robot_CAN_CORE */
/* @} Robot_DEVICES */

