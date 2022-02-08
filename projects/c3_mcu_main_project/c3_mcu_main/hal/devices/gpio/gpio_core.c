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
#include "gpio_core.h"
#include <stddef.h>
#include <string.h>
#include "main.h"
#include "sys_list.h"
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
struct gpio_node{
	struct list_struct node;
	gpio_exti_callback callback;
	const struct ca_device *dev;
	uint16_t gpio_pin;
};

struct gpio_pin_irq_map
{
    uint16_t pinbit;
    IRQn_Type irqno;
};

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
struct list_struct gpio_exti_callback_list = {.next = &gpio_exti_callback_list, .prev = &gpio_exti_callback_list};;

static const struct gpio_pin_irq_map gpio_pin_irq_map[] =
{
#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0)
    {GPIO_PIN_0, EXTI0_1_IRQn},
    {GPIO_PIN_1, EXTI0_1_IRQn},
    {GPIO_PIN_2, EXTI2_3_IRQn},
    {GPIO_PIN_3, EXTI2_3_IRQn},
    {GPIO_PIN_4, EXTI4_15_IRQn},
    {GPIO_PIN_5, EXTI4_15_IRQn},
    {GPIO_PIN_6, EXTI4_15_IRQn},
    {GPIO_PIN_7, EXTI4_15_IRQn},
    {GPIO_PIN_8, EXTI4_15_IRQn},
    {GPIO_PIN_9, EXTI4_15_IRQn},
    {GPIO_PIN_10, EXTI4_15_IRQn},
    {GPIO_PIN_11, EXTI4_15_IRQn},
    {GPIO_PIN_12, EXTI4_15_IRQn},
    {GPIO_PIN_13, EXTI4_15_IRQn},
    {GPIO_PIN_14, EXTI4_15_IRQn},
    {GPIO_PIN_15, EXTI4_15_IRQn},
#elif defined(SOC_SERIES_STM32MP1)
    {GPIO_PIN_0, EXTI0_IRQn},
    {GPIO_PIN_1, EXTI1_IRQn},
    {GPIO_PIN_2, EXTI2_IRQn},
    {GPIO_PIN_3, EXTI3_IRQn},
    {GPIO_PIN_4, EXTI4_IRQn},
    {GPIO_PIN_5, EXTI5_IRQn},
    {GPIO_PIN_6, EXTI6_IRQn},
    {GPIO_PIN_7, EXTI7_IRQn},
    {GPIO_PIN_8, EXTI8_IRQn},
    {GPIO_PIN_9, EXTI9_IRQn},
    {GPIO_PIN_10, EXTI10_IRQn},
    {GPIO_PIN_11, EXTI11_IRQn},
    {GPIO_PIN_12, EXTI12_IRQn},
    {GPIO_PIN_13, EXTI13_IRQn},
    {GPIO_PIN_14, EXTI14_IRQn},
    {GPIO_PIN_15, EXTI15_IRQn},
#else
    {GPIO_PIN_0, EXTI0_IRQn},
    {GPIO_PIN_1, EXTI1_IRQn},
    {GPIO_PIN_2, EXTI2_IRQn},
    {GPIO_PIN_3, EXTI3_IRQn},
    {GPIO_PIN_4, EXTI4_IRQn},
    {GPIO_PIN_5, EXTI9_5_IRQn},
    {GPIO_PIN_6, EXTI9_5_IRQn},
    {GPIO_PIN_7, EXTI9_5_IRQn},
    {GPIO_PIN_8, EXTI9_5_IRQn},
    {GPIO_PIN_9, EXTI9_5_IRQn},
    {GPIO_PIN_10, EXTI15_10_IRQn},
    {GPIO_PIN_11, EXTI15_10_IRQn},
    {GPIO_PIN_12, EXTI15_10_IRQn},
    {GPIO_PIN_13, EXTI15_10_IRQn},
    {GPIO_PIN_14, EXTI15_10_IRQn},
    {GPIO_PIN_15, EXTI15_10_IRQn},
#endif
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	struct list_struct *p_list = NULL;
	struct gpio_node *p_gpio = NULL;
	const struct ca_device *p_dev = NULL;

	p_list = &gpio_exti_callback_list;
	while(list_is_last(p_list, &gpio_exti_callback_list) != 1)
	{
	  	p_gpio = (struct gpio_node*)p_list->next;  
		p_dev = p_gpio->dev;
		
		if((NULL == p_dev)||(NULL == p_dev->bus)||(NULL == p_dev->bus->handler))
		{
			p_list = p_list->next;
			continue;
		}
		int gpio_pin = (uint16_t)(1 << (uint8_t)p_gpio->gpio_pin);

		if((NULL != p_dev->bus)  || (gpio_pin == GPIO_Pin))
		{
			if(NULL != p_gpio->callback)
			{
				p_gpio->callback(p_dev, p_gpio->gpio_pin);
			}

			break;
		}

		p_list = p_list->next;
	}
	return;
}

int32_t gpio_exti_register_callback(const struct ca_device *dev, uint16_t gpio_pin, gpio_exti_callback callback)
{
	struct gpio_node *p_gpio = NULL;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == callback))
	{
		return -1;
	}
	
	p_gpio = (struct gpio_node *)mem_block_alloc(sizeof(struct gpio_node));
	if(NULL == p_gpio)
	{
		return -1;
	}

	memset(p_gpio, 0, sizeof(struct gpio_node));

	
	if((NULL == gpio_exti_callback_list.next) && (NULL == gpio_exti_callback_list.prev))
	{
		list_head_init(&gpio_exti_callback_list);
	}

	p_gpio->dev = dev;
	p_gpio->gpio_pin = gpio_pin;
	p_gpio->callback = callback;
	
	list_add_tail(&p_gpio->node, &gpio_exti_callback_list);
	

	return 0;
}

int32_t gpio_exti_start(const struct ca_bus *bus, GPIO_EXTI_TYPE exti_type)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}

	HAL_NVIC_DisableIRQ(gpio_pin_irq_map[bus->bus_addr].irqno);
		
  switch (exti_type)
  {
      case GPIO_EXTI_RISING:
          GPIO_InitStruct.Pull = GPIO_PULLDOWN;
          GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
          break;
      case GPIO_EXTI_FALLING:
          GPIO_InitStruct.Pull = GPIO_PULLUP;
          GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
          break;
      default:
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
          break;
  }
	GPIO_InitStruct.Pin = gpio_pin_irq_map[bus->bus_addr].pinbit;
  HAL_GPIO_Init(bus->handler, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_EnableIRQ(gpio_pin_irq_map[bus->bus_addr].irqno);

	return 0;
}

int32_t gpio_exti_stop(const struct ca_bus *bus)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(NULL == bus)
	{
		return -1;
	}

	HAL_NVIC_DisableIRQ(gpio_pin_irq_map[bus->bus_addr].irqno);

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
int32_t gpio_read(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	int *state = (int *)buffer;
	*state = 10;
	
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}
	*state = HAL_GPIO_ReadPin(bus->handler, (uint16_t)(1 << (uint8_t)bus->bus_addr));

	return 0;
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
int32_t gpio_write(const struct ca_bus *bus, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	uint16_t gpio_pin = 0;
	if((NULL == bus)||(NULL == bus->handler)||(NULL == buffer))
	{
		return -1;
	}
	gpio_pin = (uint16_t)(1 << (uint8_t)bus->bus_addr);
	*((uint8_t *)buffer) ? HAL_GPIO_WritePin(bus->handler,gpio_pin,GPIO_PIN_SET) : HAL_GPIO_WritePin(bus->handler,gpio_pin,GPIO_PIN_RESET) ;	

	return ret;

}

int32_t gpio_ioctl(const struct ca_bus *bus,  uint32_t cmd, void *arg)
{
	uint16_t gpio_pin = 0;
	gpio_pin = (uint16_t)(1 << (uint8_t)bus->bus_addr);
	if((NULL == bus)||(NULL == bus->handler))
	{
		return -1;
	}
	
	switch(cmd)
	{
		case LED_GPIO_TOGGLE:
			HAL_GPIO_TogglePin((GPIO_TypeDef *)bus->handler, gpio_pin);
			break;
		case GPIO_ACTIVE_HIGH:
			HAL_GPIO_WritePin((GPIO_TypeDef *)bus->handler, gpio_pin,GPIO_PIN_SET);
			break;
		case GPIO_ACTIVE_LOW:
			HAL_GPIO_WritePin((GPIO_TypeDef *)bus->handler, gpio_pin,GPIO_PIN_RESET);
			break;
		default:
			break;
	}
	
	return 0;
}

int32_t sw_init(struct bus_sw *sw, struct ca_bus **bus)
{
	struct sw_handler *handler = NULL;
	struct ca_bus *p_bus = NULL;
	
	if(NULL == sw)
	{
		return -1;
	}

	handler = mem_block_alloc(sizeof(struct sw_handler));
	if(NULL == handler)
	{
		return -1;
	}
	if(bus_find_name(&sw->gpio_ctrl[0], &handler->gpio_ctrl[0]))
	{
		return -1;
	}

	if(bus_find_name(&sw->gpio_ctrl[1], &handler->gpio_ctrl[1]))
	{
		return -1;
	}

	if(bus_find_name(&sw->gpio_ctrl[2], &handler->gpio_ctrl[2]))
	{
		return -1;
	}
	p_bus = (struct ca_bus *)mem_block_alloc(sizeof(struct ca_bus));
	if(NULL == p_bus)
	{
		return -1;
	}
	memset(p_bus, 0, sizeof(struct ca_bus));
	memcpy((char *)p_bus->bus_name, sw->common.bus_name, strlen((char *)sw->common.bus_name));
	p_bus->handler = handler;
	*bus = p_bus;

	return 0;
}

BUS_DRIVER_INIT(sw, sw_init);

#ifdef __cplusplus
}
#endif



/* @} Robot_I2C_CORE */
/* @} Robot_DEVICES */

