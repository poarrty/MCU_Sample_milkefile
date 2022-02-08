/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         畅一晨
  ** Version:        V0.0.1
  ** Date:           2021-9-24
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 畅一晨 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-3-25       changyichen   0.0.1       创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "carpet_mu0002.h"
#include "shell.h"
#include "uart_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "mem_pool.h"
#include "define_carpet.h"
#include "lwrb.h"
#include "cmsis_os2.h"
#include "log.h"



/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_MU0002
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
#define  CARPET_MU0002_RECV_BUFF_SIZE_128B  128        //接收数据缓冲器大小

#define CARPET_MU0002_FRAME_LEN_MAX       0x14        //CARPET 一帧最长长度20字节
#define CARPET_MU0002_FRAME_START         0x5A        //起始字节
#define CARPET_MU0002_FRAME_FLAG          0x01        //返回帧标识

#define CARPET_MU0002_INDEX_HEADER        0X00        //帧头索引
#define CARPET_MU0002_INDEX_FRAME_FLAG    0X01        //帧标识索引
#define CARPET_MU0002_INDEX_GROUND_TYPE   0X02        //地面类型索引
#define CARPET_MU0002_INDEX_DATE          0X03        //数据索引
#define CARPET_MU0002_INDEX_CHECKSUM      0X13        //校验码索引

/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/
typedef struct mu0002_callback_node{
	struct list_struct node;                        //回调函数节点
	carpet_callback callback;                       //对应命令的回调函数
	CARPET_CMD_TYPE_E carpet_cmd_type_e;            //IOCTL命令类型
}MU0002_CALLBACK_NODE_T, *MU0002_CALLBACK_NODE_P;   //MU0002回调函数链表节点

typedef enum
{
	CARPET_DETECTION_ONCE,                   //单次检测 
    CARPET_DETECTION_CONTINUE,               //连续检测
    CARPET_DETECTION_STOP_CONTINUE,          //停止连续检测

	CARPET_MODE_MAX                          //最多模式类型
} CARPET_DETECTION_MODE_E;


typedef struct
{
    uint8_t strength1_H;          //一次回波强度高字节
    uint8_t strength1_L;          //一次回波强度低字节
    uint8_t local1_H;             //一次回波位置高字节
    uint8_t local1_L;             //一次回波位置低字节
    uint8_t strength2_H;
    uint8_t strength2_L;
    uint8_t local2_H;
    uint8_t local2_L;
    uint8_t strength3_H;
    uint8_t strength3_L;
    uint8_t local3_H;
    uint8_t local3_L;
    uint8_t strength4_H;
    uint8_t strength4_L;
    uint8_t local4_H;
    uint8_t local4_L;
}CARPET_DATA_T, *CARPET_DATA_P;    //回波强度与位置数据包

typedef struct
{
    uint8_t header;          //帧头
    uint8_t frame_flag;      //帧标识
    uint8_t ground_type;     //地面类型
    CARPET_DATA_T data;      //回波强度与位置数据包
    uint8_t checksum;        //校验码
}CARPET_MU0002_FRAME_DATA_T, *CARPET_MU0002_FRMAE_DATA_P;   //传感器返回数据

typedef struct {
    lwrb_t carpet_mu0002_rbuff;                         //lwrb环形缓冲区
    osThreadId_t carpet_mu0002_thread;	                //线程id
	osMutexId_t mutex_carpet_mu0002_data;               //互斥锁
    osMutexAttr_t mutex_carpet_mu0002_data_attr;        //互斥锁属性
    osSemaphoreId_t carpet_mu0002_empty_id;             //空信号量ID
	osSemaphoreId_t carpet_mu0002_filled_id;            //满信号量ID
    struct list_struct carpet_mu0002_callback_list;     //地毯检测回调函数链表头
	CARPET_CMD_TYPE_E carpet_mu0002_cmd_type_e;         //IOCTL命令类型
    CARPET_GROUND_TYPE_E carpet_ground_type_e;          //地面类型
    uint8_t carpet_recv_data;                           //接收到的数据
	uint8_t recv_sensor_data_num;                      //接收传感器字节数
	uint8_t carpet_recv_buff[128];                      //接收数据缓冲区
    CARPET_MU0002_FRAME_DATA_T carpet_mu0002_frame_data;//传感器返回数据
}CARPET_MU0002_ARG_T, *CARPET_MU0002_ARG_P;           //mu0002地毯检测传感器属性信息

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
const osThreadAttr_t carpet_mu0002_attributes = {
  .name = "carpet_mu0002",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

const osMutexAttr_t mutex_carpet_mu0002_data_attr = {
	"mutex_carpet_mu0002_data",                               // human readable mutex name
	osMutexRecursive | osMutexPrioInherit,                    // attr_bits
	NULL,                                                     // memory for control block   
	0U                                                        // size for control block
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
uint8_t carpet_mu0002_frame_check_sum(uint8_t *data, uint16_t len)
{
	uint8_t check_sum = 0;
	uint16_t i;

	for (i = 0; i < len; i++)
	{
		check_sum += data[i];
	}

	return check_sum;

}


static uint8_t carpet_mu0002_set_mode( const struct ca_device *dev, CARPET_DETECTION_MODE_E mode )
{
	uint8_t once_mode_buff[] = {0x55, 0xAA, 0x0D, 0x0C};
	uint8_t continue_mode_buff[] = {0x55, 0xAA, 0x0C, 0x0B};
    uint8_t stop_continue_mode_buff[] = {0x55, 0xAA, 0x0C, 0x0B};
	uint8_t len = 0;
	uint8_t *tx_buff = NULL;

    if (mode >= CARPET_MODE_MAX)
    {
        LOG_ERROR("CARPET mode type error.");
        return 1;
    }

	if (CARPET_DETECTION_ONCE == mode)
	{
		tx_buff = once_mode_buff;
		len = sizeof(once_mode_buff);
	}
	else if (CARPET_DETECTION_CONTINUE == mode)
	{
		tx_buff = continue_mode_buff;
		len = sizeof(continue_mode_buff);
	}
    else if (CARPET_DETECTION_STOP_CONTINUE == mode)
	{
		tx_buff = stop_continue_mode_buff;
		len = sizeof(stop_continue_mode_buff);
	}

    while(uart_write(dev->bus, (uint8_t *)tx_buff, (uint32_t)len));

    return 0;
}

static uint8_t find_callback(const struct ca_device *dev, struct mu0002_callback_node **publish_callback, CARPET_CMD_TYPE_E callback_type)
{
	CARPET_MU0002_ARG_P p_carpet_args = NULL;

	if ((dev == NULL) || (NULL == dev->device_args))
	{
		return -1;
	}
	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;

	struct list_struct *p_list = NULL;
	struct mu0002_callback_node *p_callback = NULL;

	p_list = &p_carpet_args->carpet_mu0002_callback_list;
	while(list_is_last(p_list, &p_carpet_args->carpet_mu0002_callback_list) != 1)
	{
	  	p_callback = (struct mu0002_callback_node*)p_list->next;  

		if ( (p_callback->callback != NULL) && (callback_type == p_callback->carpet_cmd_type_e) )
		{
			*publish_callback = p_callback;
			return 0;
		} 

		p_list = p_list->next;

	}
	return 1;  //未找到对应callback
}

void carpet_data_handle(const struct ca_device *dev, CARPET_GROUND_TYPE_E ground_type)
{
	CARPET_MU0002_ARG_P p_carpet_args = NULL;
	static struct mu0002_callback_node *p_callback = NULL;
	uint8_t count = 0;

	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;
	p_carpet_args->recv_sensor_data_num = 0;

	if ( NULL == p_callback)
	{
		if ( find_callback(dev, &p_callback, CARPET_SET_CALLBACK) ) 
		{
			//未找到数据发布callback
			// LOG_ERROR("Find publish callback error.");
			return;
		}
	}

	if ( CARPET_SOFT_GROUND == ground_type )
	{
		// LOG_DEBUG("CARPET_SOFT_GROUND");
		p_callback->callback(CARPET_SOFT_GROUND);	
	}
	else if ( CARPET_TOUGH_GROUND == ground_type )
	{
		// //若上次检测结果为软地面，则连续发布三次硬地面信息
		if ( CARPET_SOFT_GROUND == p_carpet_args->carpet_ground_type_e )  
		{
			for ( count = 0; count < 3; count++ )
			{
				// LOG_DEBUG("CARPET_TOUGH_GROUND");
				p_callback->callback(CARPET_TOUGH_GROUND);
			}
		}
	}

	return;
}

static void carpet_data_decode(const struct ca_device *dev)
{
    static uint8_t frame_buff[CARPET_MU0002_FRAME_LEN_MAX];
    static uint8_t buff_index = 0;;
    static uint8_t recv_sta = 0;
	CARPET_MU0002_ARG_P p_carpet_args = NULL;

	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;
    
    CARPET_MU0002_FRMAE_DATA_P rec_frame = (CARPET_MU0002_FRMAE_DATA_P) frame_buff;
    
    uint16_t check_sum = 0;

    if (0 == buff_index)
    {
        memset(frame_buff, 0, 20);
    }

    switch( recv_sta )
    {
        case 0:
            
            frame_buff[buff_index++] = p_carpet_args->carpet_recv_data;
        
            if(buff_index == CARPET_MU0002_INDEX_HEADER+1)
            {
                if(rec_frame->header == CARPET_MU0002_FRAME_START)
                {
                    recv_sta = 1;
                }
                else
                {
                    buff_index = 0;
                    recv_sta = 0;
                }
            }
        
            break;
            
        case 1:
            
            frame_buff[buff_index++] = p_carpet_args->carpet_recv_data;
            
            if(buff_index == CARPET_MU0002_INDEX_FRAME_FLAG+1)
            {
                if(rec_frame->frame_flag == CARPET_MU0002_FRAME_FLAG)
                {
                    recv_sta = 2;
                }
                else
                {
                    buff_index = 0;
                    recv_sta = 0;
                }
            }
            
            break;

        case 2:
            frame_buff[buff_index++] = p_carpet_args->carpet_recv_data;
            
            if(buff_index == CARPET_MU0002_INDEX_GROUND_TYPE+1)
            {
				if (rec_frame->frame_flag == CARPET_SOFT_GROUND || rec_frame->frame_flag == CARPET_TOUGH_GROUND || rec_frame->frame_flag == CARPET_MODULE_ERROR)
				{
					recv_sta = 3;
				}
				else
				{
					buff_index = 0;
                    recv_sta = 0;
				}  
            }
            
            break;
            
        case 3:
            frame_buff[buff_index++] = p_carpet_args->carpet_recv_data;
        
            if(buff_index == CARPET_MU0002_INDEX_CHECKSUM + 1)
            {
                check_sum = carpet_mu0002_frame_check_sum((uint8_t *)rec_frame, CARPET_MU0002_FRAME_LEN_MAX-1);
                if(rec_frame->checksum == check_sum)
                {
					carpet_data_handle(dev, rec_frame->ground_type);
                    p_carpet_args->carpet_ground_type_e = rec_frame->ground_type;
                }
                else
                {
                    // LOG_ERROR("CARPET data receive verify error, want[%#06X], get[%#06X].", rec_frame->checksum, check_sum);
                }
                
                buff_index = 0;
                recv_sta = 0;
            }
            
            break;
            
        default:
            buff_index = 0;
            recv_sta = 0;
            break;
    }
    
}

// /***********数据解析************/
// static void carpet_mu0002_data_handle( const struct ca_device *dev)
// {
//     uint8_t RecvLen = 0;

// 	CARPET_MU0002_ARG_P p_carpet_args = NULL;

// 	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;
	
// 	while (1)
// 	{	
// 		if ( (p_carpet_args->carpet_mu0002_empty_id != NULL) && (p_carpet_args->carpet_mu0002_filled_id != NULL) )
// 		{
// 		 	if (osSemaphoreAcquire(p_carpet_args->carpet_mu0002_filled_id, 20) == osOK)
// 		 	{
// 		 		RecvLen = lwrb_read(&p_carpet_args->carpet_mu0002_rbuff, &p_carpet_args->carpet_recv_data, 1);
// 				if (RecvLen != 1)
// 				{
// 					LOG_DEBUG("CARPET data read from lwrb error. recvlen = %d", RecvLen);
// 					// LOG_ERROR("CARPET data read from lwrb error.");
// 				}
// 				osSemaphoreRelease(p_carpet_args->carpet_mu0002_empty_id);
// 				carpet_data_decode(dev);
// 		 	}
// 			else
// 			{
// 				LOG_ERROR("carpet_mu0002_filled_id acquire error.");
// 			}
		
// 		 }
// 	}

//     return;
// }


void carpet_mu0002_reset(const struct ca_device *dev)
{
    uint8_t reset_sensor_buff[] = {0x55, 0xAA, 0xFF, 0xFF};

	uart_write(dev->bus, (uint8_t *)reset_sensor_buff, sizeof(reset_sensor_buff));
}

uint8_t mu0002_sensor_init(const struct ca_device *dev)
{
	// carpet_mu0002_reset(dev); //若要打开屏蔽，则必须添加延时函数
	
	carpet_mu0002_set_mode(dev, CARPET_DETECTION_CONTINUE);
	
	return 0;
}

static void carpet_mu0002_run(void *argument)
{
	uint8_t RecvLen = 0;
	struct ca_device *dev = (struct ca_device *)argument;
	CARPET_MU0002_ARG_P p_carpet_args = NULL;

	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;
	
	while (1)
	{	
		if ( (p_carpet_args->carpet_mu0002_empty_id != NULL) && (p_carpet_args->carpet_mu0002_filled_id != NULL) )
		{
		 	if (osSemaphoreAcquire(p_carpet_args->carpet_mu0002_filled_id, 20) == osOK)
		 	{
		 		RecvLen = lwrb_read(&p_carpet_args->carpet_mu0002_rbuff, &p_carpet_args->carpet_recv_data, 1);
				if (RecvLen != 1)
				{
					// LOG_ERROR("CARPET data read from lwrb error.");
					LOG_DEBUG("CARPET data read from lwrb error. recvlen = %d", RecvLen);
				}
				osSemaphoreRelease(p_carpet_args->carpet_mu0002_empty_id);
				carpet_data_decode(dev);
		 	}
			else
			{
				// LOG_ERROR("carpet_mu0002_filled_id acquire error.");
			}
		
		 }
	}

    // carpet_mu0002_data_handle(dev);

}

void carpet_mu0002_rx_callback(const struct ca_device *dev, uint8_t rx_buf)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return;
	}
	CARPET_MU0002_ARG_P p_carpet_args = NULL;
	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;

	if (p_carpet_args->recv_sensor_data_num < 190)
	{
		p_carpet_args->recv_sensor_data_num++;
		return;
	}

	if (p_carpet_args->carpet_mu0002_empty_id != NULL && p_carpet_args->carpet_mu0002_filled_id != NULL)
	{
		if (osSemaphoreAcquire(p_carpet_args->carpet_mu0002_empty_id, 0) == osOK)  //中断里不应设置超时等待时间
		{
			lwrb_write(&p_carpet_args->carpet_mu0002_rbuff, &rx_buf, 1);
			osSemaphoreRelease(p_carpet_args->carpet_mu0002_filled_id);
		}
	}
	return;
	
}

static int32_t carpet_mu0002_register_callback( const struct ca_device *dev, carpet_callback callback, CARPET_CMD_TYPE_E callback_type)
{
	struct mu0002_callback_node *p_callback = NULL;
	CARPET_MU0002_ARG_P p_carpet_args = NULL;

	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == callback))
	{
		return -1;
	}
	p_carpet_args = (CARPET_MU0002_ARG_P) dev->device_args;

	p_callback = (struct mu0002_callback_node *)mem_block_alloc(sizeof(struct mu0002_callback_node));

	if((NULL == p_callback) || (callback_type > CARPET_MAX_CMD_TYPE))
	{
		return -1;
	}

	memset(p_callback, 0, sizeof(struct mu0002_callback_node));

	if((NULL == p_carpet_args->carpet_mu0002_callback_list.prev) && (NULL == p_carpet_args->carpet_mu0002_callback_list.next))
	{
		list_head_init(&p_carpet_args->carpet_mu0002_callback_list);
	}

	p_callback->carpet_cmd_type_e = callback_type;
	p_callback->callback = callback;
	
	list_add_tail(&p_callback->node, &p_carpet_args->carpet_mu0002_callback_list);

	return 0;
}



int32_t carpet_mu0002_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t carpet_mu0002_close(const struct ca_device *dev)
{
	return 0;
}

int32_t carpet_mu0002_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	
	return 0;
}

int32_t carpet_mu0002_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

  	uart_write(dev->bus, (uint8_t *)buffer, size);

	return 0;
}

int32_t carpet_mu0002_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	switch (cmd)
	{
		case CARPET_SET_CALLBACK:
			carpet_mu0002_register_callback( dev, (carpet_callback) arg, CARPET_SET_CALLBACK);
		  	break;

		case CARPET_GET_GROUND_STATE:
	
			break;

		default:
			//无效命令
			break;


	}
	return 0;
}
/*****************************************************************/
/**
 * Function:       letter_shell_init
 * Description:    初始化 letter_shell
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
int32_t carpet_mu0002_init(struct ca_device *dev)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}
	CARPET_MU0002_ARG_P p_args;

	if(osKernelGetState() == osKernelRunning)
    {
		osDelay(1000);
    }
    else
    {
		HAL_Delay(1000);
    }

	mu0002_sensor_init(dev);

	dev->ops.open = carpet_mu0002_open;
	dev->ops.close = carpet_mu0002_close;
	dev->ops.read = carpet_mu0002_read;
	dev->ops.write = carpet_mu0002_write;
	dev->ops.ioctl = carpet_mu0002_ioctl;

	uart_rx_register_callback(dev, carpet_mu0002_rx_callback);

	p_args = (CARPET_MU0002_ARG_P)mem_block_alloc(sizeof(CARPET_MU0002_ARG_T));
	if(NULL == p_args)
	{
		return -1;

	}

	memset(p_args, 0, sizeof(CARPET_MU0002_ARG_T));

	p_args->carpet_mu0002_callback_list.next = &p_args->carpet_mu0002_callback_list;
	p_args->carpet_mu0002_callback_list.prev = &p_args->carpet_mu0002_callback_list;
	p_args->carpet_ground_type_e = CARPET_TOUGH_GROUND;
    p_args->carpet_mu0002_empty_id = osSemaphoreNew(CARPET_MU0002_RECV_BUFF_SIZE_128B, CARPET_MU0002_RECV_BUFF_SIZE_128B, NULL);
	p_args->carpet_mu0002_filled_id = osSemaphoreNew(CARPET_MU0002_RECV_BUFF_SIZE_128B, 0U, NULL);
	lwrb_init(&p_args->carpet_mu0002_rbuff, &p_args->carpet_recv_buff, CARPET_MU0002_RECV_BUFF_SIZE_128B);
	p_args->mutex_carpet_mu0002_data = osMutexNew(&mutex_carpet_mu0002_data_attr);
	p_args->carpet_mu0002_thread = osThreadNew(carpet_mu0002_run, (void *)dev, &carpet_mu0002_attributes);

	dev->device_args = (void *) p_args;

	

	return 0;
}

/*****************************************************************/
/**
 * Function:       letter_shell_deinit
 * Description:    去初始化 letter_shell
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
int32_t carpet_mu0002_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(carpet, uart, carpet_mu0002_init);

#ifdef __cplusplus
}
#endif

/* @} Robot_LETTER_SHELL */
/* @} Robot_DEVICES */

