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
#include "button.h"
#include "gpio_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "main.h"
#include "define_button.h"
#include "log.h"
#include "mem_pool.h"
#include "stdbool.h"
#include "log.h"


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
typedef  void (*button_callback)(struct bus_info, BUTTON_STATUS_TYPE type);
/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/
typedef struct
{
    struct list_struct button_list;                    //按键链表头
    osThreadId_t button_thread;	                       //按键处理线程
}BUTTON_ARGUMENT_T, *BUTTON_ARGUMENT_P;                //按键参数信息(按键设备device_args)

typedef struct button_node{
	struct list_struct node;                            //链表节点信息
    struct Button button;                               //按键结构
	struct bus_info gpio_info;                          //按键PIN脚
    struct ca_bus *gpio_bus;                            //按键PIN脚bus信息
    uint8_t state;                                      //按键
    uint8_t index;                                      //按键索引
    int (*button_callback)(uint8_t, BUTTON_STATUS_TYPE);//非中断事件处理函数
    void (*gpio_exti_callback)(const struct ca_device *dev, uint16_t GPIO_Pin);  //中断事件处理函数
}BUTTON_NODE_ATTR_T, *BUTTON_NODE_ATTR_P;               //按键链表节点信息

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
const osThreadAttr_t button_attributes = {
  .name = "button",
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
uint8_t button_Level(struct Button * button)
{
    BUTTON_NODE_ATTR_P p_button = NULL;
    uint8_t read_state_ret = 0;

    if(NULL == button)
    {
        return 0xff;
    }

    p_button = (BUTTON_NODE_ATTR_P)button->args;

    if(NULL == p_button)
    {
        return 0xff;
    }

    gpio_read(p_button->gpio_bus, &read_state_ret, sizeof(uint8_t));

    return read_state_ret;
}

int8_t bus_info_find_button_node(const struct ca_device *dev, BUTTON_NODE_ATTR_P *pp_button, struct bus_info * gpio_info)
{
    struct list_struct *p_list = NULL;
    BUTTON_ARGUMENT_P p_args = NULL;
    BUTTON_NODE_ATTR_P p_node = NULL;

	if((NULL == dev) || (NULL == gpio_info))
	{
		return -1;
	}

    p_args = (BUTTON_ARGUMENT_P)mem_block_alloc(sizeof(BUTTON_ARGUMENT_T));
    if (NULL == p_args)
    {
        return -1;
    }
    p_args = (BUTTON_ARGUMENT_P)dev->device_args;

	p_list = &p_args->button_list;
	while(list_is_last(p_list, &p_args->button_list) != 1)
	{
        p_node = (BUTTON_NODE_ATTR_P) p_list->next;

		if( (0 == strncmp( (char *) p_node->gpio_info.bus_name, (char *) gpio_info->bus_name, sizeof(gpio_info->bus_name))) && (p_node->gpio_info.bus_addr == gpio_info->bus_addr) ) 
		{
			*pp_button = p_node;
			return 0;
		}
		p_list = p_list->next;
	}
    
    return -1;
}

int8_t bus_addr_find_button_node(const struct ca_device *dev, BUTTON_NODE_ATTR_P *pp_button, uint16_t gpio_pin)
{
    struct list_struct *p_list = NULL;
    BUTTON_ARGUMENT_T args;
    BUTTON_NODE_ATTR_P p_node = NULL;

	if(NULL == dev)
	{
		return -1;
	}

    args = *(BUTTON_ARGUMENT_P)dev->device_args;

	p_list = &args.button_list;
	while(list_is_last(p_list, &args.button_list) != 1)
	{
        p_node = (BUTTON_NODE_ATTR_P) p_list->next;

		if( (p_node->gpio_exti_callback != NULL) && ( (0x0001 << p_node->gpio_info.bus_addr) == gpio_pin) ) 
		{
			*pp_button = p_node;
			return 0;
		}
		p_list = p_list->next;

	}
      
    return -1;
}

void button_press_down_callback(void* button)
{
    BUTTON_NODE_ATTR_P p_button = NULL;

    p_button = (BUTTON_NODE_ATTR_P) ((struct Button*)button)->args;
    
    if((NULL != p_button) && (NULL != p_button->button_callback))
    {
        p_button->button_callback(p_button->index, BUTTON_PRESS_DOWN);
    }
}

void button_press_up_callback(void* button)
{
    BUTTON_NODE_ATTR_P p_button = NULL;
    p_button = (BUTTON_NODE_ATTR_P) ((struct Button*)button)->args;
    
    if((NULL != p_button) && (NULL != p_button->button_callback))
    {
        p_button->button_callback(p_button->index, BUTTON_PRESS_UP);
    }
}

void button_long_press_callback(void* button)
{
    BUTTON_NODE_ATTR_P p_button = NULL;
    p_button = (BUTTON_NODE_ATTR_P) ((struct Button*)button)->args;
    
    if((NULL != p_button) && (NULL != p_button->button_callback))
    {
        p_button->button_callback(p_button->index, BUTTON_LONG_PRESS_START);
    }
    
}

void button_exti_callback(const struct ca_device *dev, uint16_t gpio_pin)
{
    BUTTON_NODE_ATTR_P p_button = NULL;


    bus_addr_find_button_node(dev, &p_button, gpio_pin);

    if(NULL != p_button)
    {
        p_button->gpio_exti_callback(dev, gpio_pin);
    }
}
void *attach_callback[BUTTON_EVENT_MAX] = {button_press_down_callback, button_press_up_callback, NULL, NULL, NULL, button_long_press_callback, NULL, NULL};

void button_run(void *dev)
{
    while (1)
    {
        button_ticks();
        if(osKernelGetState() == osKernelRunning)
        {
            osDelay(5);
        }
        else
        {
            HAL_Delay(5);
        }
    }
	
	return;
}

int32_t gpio_button_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t gpio_button_close(const struct ca_device *dev)
{
	return 0;
}

int32_t gpio_button_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t gpio_button_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}
	return 0;
}


bool bit_test(BUTTON_ATTACH_T *attach, uint8_t bit)
{
    uint8_t *num = (uint8_t *) attach;
    uint8_t test = 0x01;

    test &= *num >> bit;

    if (test)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void control_pin_state(BUTTON_PIN_ATTR_P p_arg)
{
    if (p_arg == NULL)
    {
        LOG_DEBUG("control_pin_state arg is null");
        return;
    }

    struct ca_bus *gpio_bus;

    bus_find_name(&p_arg->gpio_info, &gpio_bus);
    
    gpio_write(gpio_bus, &p_arg->state, 1);

    return;
}

int32_t gpio_button_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
    BUTTON_NODE_ATTR_T *p_button = NULL;
    // struct button_ioctl_attr *p_attr = NULL;
    uint8_t index = 0;
    struct bus_info *p = NULL;

    BUTTON_ATTACH_ATTR_T *p_arg = NULL;

	if(NULL == dev)
	{
		return -1;
	}

    switch (cmd)
    {
        case BUTTON_CMD_SET_ADD_BUTTON:

            p_button = (BUTTON_NODE_ATTR_T *)mem_block_alloc(sizeof(BUTTON_NODE_ATTR_T));
            if(NULL == p_button)
            {
                return -1;
            }
            p_button->gpio_info = *((BUTTON_ATTR_P)arg)->gpio_info;
            bus_find_name(&p_button->gpio_info, &p_button->gpio_bus);
            
            button_init_ext(&p_button->button, button_Level, ((BUTTON_ATTR_P)arg)->trigger_condition);
            p_button->button.args = p_button;
            p_button->button.period = ((BUTTON_ATTR_P)arg)->period;
            p_button->index = ((BUTTON_ATTR_P)arg)->index;
            p_button->button.button_level = button_Level(&p_button->button);
            if (0 == p_button->button.period)
            {
                p_button->button.period = 1000;       //若未被赋值，则默认初始化周期为1000ms
            }
            p_button->button.long_press_start_flag = ((BUTTON_ATTR_P)arg)->long_press_hold_type;
            
            list_add_tail(&p_button->node, &((BUTTON_ARGUMENT_P)dev->device_args)->button_list);

            break;
        case BUTTON_CMD_SET_ADD_ATTACH:
            p_arg = (BUTTON_ATTACH_ATTR_P)arg;
            bus_info_find_button_node(dev, &p_button, ((BUTTON_ATTACH_ATTR_P)arg)->gpio_info);
            for(index = 0; index < PRESS_EVENT_MAX; index++)
            {
                if( bit_test( &((BUTTON_ATTACH_ATTR_P)arg)->attach, index) )
                {
                    button_attach(&p_button->button, index, attach_callback[index]);
                    if ((BUTTON_EXIT == index) && (NULL != ((BUTTON_ATTACH_ATTR_P)arg)->gpio_exti_callback ) )
                    {
                        gpio_exti_register_callback(dev, ( 0x0001 << ((BUTTON_ATTACH_ATTR_P)arg)->gpio_info->bus_addr), button_exti_callback);
                        p_button->gpio_exti_callback = ((BUTTON_ATTACH_ATTR_P)arg)->gpio_exti_callback;
                    }
                }
            }

            p_button->button_callback = ((BUTTON_ATTACH_ATTR_P)arg)->button_callback;

            button_start(&p_button->button);

            break;

        case BUTTON_CMD_GET_PIN_STATE:
            control_pin_state((BUTTON_PIN_ATTR_P) arg);
            break;

        default:
            break;

    }
	return 0;
}

/*****************************************************************/
/**
 * Function:       gpio_button_init
 * Description:    初始化 button
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
int32_t gpio_button_init(struct ca_device *dev)
{
	BUTTON_ARGUMENT_P p_args = NULL;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(BUTTON_ARGUMENT_T));

	dev->ops.open = gpio_button_open;
	dev->ops.close = gpio_button_close;
	dev->ops.read = gpio_button_read;
	dev->ops.write = gpio_button_write;
	dev->ops.ioctl = gpio_button_ioctl;

  p_args = (BUTTON_ARGUMENT_P)mem_block_alloc(sizeof(BUTTON_ARGUMENT_T));
	if(NULL == p_args)
	{
		return -1;
	}

    list_head_init(&p_args->button_list);
    p_args->button_thread = osThreadNew(button_run, (void *)dev, &button_attributes);
    dev->device_args = p_args;

	return 0;
}

/*****************************************************************/
/**
 * Function:       gpio_button_deinit
 * Description:    去初始化 button
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
int32_t gpio_button_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(button, gpio, gpio_button_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_LED */
/* @} Robot_DEVICES */


