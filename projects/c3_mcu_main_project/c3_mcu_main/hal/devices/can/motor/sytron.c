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
#include "sytron.h"
#include "devices.h"
#include "define_motor.h"
#include "can_core.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "log.h"

/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_SYTRON  - SYTRON森创电机
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
#define PP_Mode 1
#define PV_Mode 3
#define PT_Mode 4

// SDO CMD
#define SDO_W1 0x2F
#define SDO_W2 0x2B
#define SDO_W4 0x23
#define SDO_RD 0x40

// CAN ID
#define Left_Wheel_ID 0x0002
#define Right_Wheel_ID 0x0001

// Object dictionary of CANopen
#define Control_word 0x6040
#define Status_word 0x6041
#define Modes_of_operation 0x6060
#define Modes_of_operation_display 0x6061
#define Position_actual_value 0x6063
#define Velocity_sensor_actual_value 0x6069
#define Velocity_actual_value 0x606C
#define Target_torque 0x6071
#define Target_position 0x607A
#define Profile_velocity 0x6081
#define Profile_accleration 0x6083
#define Profile_deceleration 0x6084
#define Torque_slope 0x6087
#define Position_factor 0x6093
#define Target_velocity 0x60FF

/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/
typedef enum
{
	SYTRON_MOTOR_LEFT,			//左轮电机
	SYTRON_MOTOR_RIGHT,			//右轮电机
	SYTRON_MOTOR_MAX				//最大支持电机数量(轴)
}SYTRON_MOTORS;

struct sytron_data_cb_node
{
	struct list_struct node;
	MOTOR_DATA_CB_ARG data_cb;
};

struct sytron_args
{
	int32_t motor_num;												//当前支持电机数量
	MOTOR_DATA sytron_data[SYTRON_MOTOR_MAX];	//电机数据
	MOTOR_PARAM sytron_param;									//电机参数
	uint8_t data[8];													//接收数据
	int32_t data_len;													//接收数据总长度
	struct list_struct data_cb_list;					//数据回调函数链表
	osThreadId_t sytron_thread;								//处理线程id
	osMutexId_t sytron_mutex;									//互斥锁
	osSemaphoreId_t sytron_semphore;					//信号量	
};

/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
const uint8_t sytron_motor_id[SYTRON_MOTOR_MAX] = {Left_Wheel_ID, Right_Wheel_ID};

//森创默认参数
const MOTOR_PARAM sytron_default_param = {
	.motor_type = MOTOR_TYPE_SYTRON,							//电机类型
	.wheel_diameter = 155,  								//车轮直径/mm
	.wheel_space = 530,					     				//车轮间距/mm
	.motor_acc_up = 150,										//电机控制上升加速度      mm/s^2
	.motor_acc_down = 190,									//电机控制下降加速度      mm/s^2
	.motor_reduction_ratio = 10,						//电机减速比
	.motor_pole = 1,												//电机极对数
	.motor_encoding_circle = 5000,   				//电机转一圈位置增加总量
	.motor_rpm_max = 3000,         					//电机最大转速
	.motor_mmps_max = 5000        					//最大速度 mm/s
};

const osThreadAttr_t sytron_motor_position_attributes = {
	.name = "sytron_position",
	.priority = (osPriority_t) osPriorityNormal,
	.stack_size = 256 * 4,
};
	
const osMutexAttr_t mutex_imu_data_attr = {
	"sytron_mutex",      										// human readable mutex name
	osMutexRecursive | osMutexPrioInherit,  // attr_bits
	NULL,                                   // memory for control block
	0U                                      // size for control block
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
int32_t sytron_write_sdo_ndly(const struct ca_device *dev, uint8_t ID, uint8_t cmd, uint16_t index, uint16_t sub_index, uint32_t data)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
	
	memset(p_sytron->data, 0, sizeof(p_sytron->data));
	p_sytron->data[0] = cmd;
  	p_sytron->data[1] = (uint8_t)(index & 0xFF);
 	p_sytron->data[2] = (uint8_t)(index >> 8 & 0xFF);
	p_sytron->data[3] = sub_index;
	p_sytron->data[4] = (uint8_t)(data & 0xFF);
	p_sytron->data[5] = (uint8_t)(data >> 8 & 0xFF);
	p_sytron->data[6] = (uint8_t)(data >> 16 & 0xFF);
	p_sytron->data[7] = (uint8_t)(data >> 24 & 0xFF);
	
	can_write(dev->bus, 0x600 + ID, p_sytron->data, 8);
	
	return 0;
}

int32_t sytron_write_sdo(const struct ca_device *dev, uint8_t ID, uint8_t cmd, uint16_t index, uint16_t sub_index, uint32_t data)
{
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	sytron_write_sdo_ndly(dev, ID, cmd, index, sub_index, data);

	if(osKernelGetState() == osKernelRunning)
    {
        osDelay(5);
    }
    else
    {
        HAL_Delay(5);
    }
	
    

	return 0;
}

//激活节点
int32_t sytron_activate(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
	
	memset(p_sytron->data, 0, sizeof(p_sytron->data));
	p_sytron->data[0] = 0x01;
	p_sytron->data[1] = sytron_motor_id[motor];

	can_write(dev->bus, 0x000, p_sytron->data, 2);
	
	if(osKernelGetState() == osKernelRunning)
    {
        osDelay(5);
    }
    else
    {
        HAL_Delay(5);
    }

	return 0;
}

//设置速度模式
int32_t sytron_control_mode_set(const struct ca_device *dev, SYTRON_MOTORS motor, uint8_t mode)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
	
	memset(p_sytron->data, 0, sizeof(p_sytron->data));
	p_sytron->data[0] = SDO_W1;
	p_sytron->data[1] = 0x60;
	p_sytron->data[2] = 0x60;
	p_sytron->data[3] = 0x00;
	p_sytron->data[4] = mode;
	p_sytron->data[5] = 0x00;
	p_sytron->data[6] = 0x00;
	p_sytron->data[7] = 0x00;
	
	can_write(dev->bus, 0x600 + sytron_motor_id[motor], p_sytron->data, 8);
	
    if(osKernelGetState() == osKernelRunning)
    {
        osDelay(5);
    }
    else
    {
        HAL_Delay(5);
    }

	return 0;
}

//设置加速度
int32_t sytron_set_accelerate(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	struct sytron_args *p_sytron = NULL;
	uint32_t data = 0;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
	data = 200000 - p_sytron->sytron_param.motor_acc_up * 1000;
	return sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W4, 0x6083, 0x00, data);
}

//设置减速度
int32_t sytron_set_decelerate(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	p_sytron = (struct sytron_args *)dev->device_args;
	uint32_t data = 200000 - p_sytron->sytron_param.motor_acc_down * 1000;
	return sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W4, 0x6084, 0x00, data);
}

//设置目标转速
int32_t sytron_set_target_rpm(const struct ca_device *dev, SYTRON_MOTORS motor, uint32_t rpm)
{
	return sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W4, 0x60FF, 0x00, rpm);
}

//设置位置反馈
int32_t sytron_set_position_feedback(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W1, 0x1A00, 0x00, 0x00000000);  // TPDO1 STOP
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W4, 0x1A00, 0x01, 0x60630020);
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W1, 0x1A00, 0x00, 0x00000001);  // TPDO1 ENABLE. WITH 1 item
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W1, 0x1800, 0x05, 20);	 				// PERIOD 20 ms
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W1, 0x1800, 0x03, 20);	 				// PERIOD 20 ms
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W1, 0x1800, 0x02, 254);  				// PERIOD UPLOAD

	return 0;
}

//设置电机速度
int32_t syntron_set_speed(const struct ca_device *dev, MOTOR_SPEED motor_vw) 
{
	struct sytron_args *p_sytron = NULL;
	int16_t rpm_min, rpm_max;
	int16_t rpm_l, rpm_r;

	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;

	motor_speed_translate(p_sytron->sytron_param, motor_vw, &rpm_l, &rpm_r);

	rpm_min = -1 * p_sytron->sytron_param.motor_rpm_max;
	rpm_max = 1 * p_sytron->sytron_param.motor_rpm_max;

	//底层限速
	rpm_l = (rpm_l > rpm_max) ? (rpm_max) : (rpm_l);
	rpm_l = (rpm_l < rpm_min) ? (rpm_min) : (rpm_l);

	rpm_r = (rpm_r > rpm_max) ? (rpm_max) : (rpm_r);
	rpm_r = (rpm_r < rpm_min) ? (rpm_min) : (rpm_r);

	rpm_l = rpm_l * p_sytron->sytron_param.motor_reduction_ratio;
	rpm_r = rpm_r * p_sytron->sytron_param.motor_reduction_ratio;

	rpm_r = rpm_r*-1;

	sytron_write_sdo(dev, sytron_motor_id[SYTRON_MOTOR_LEFT], SDO_W4, 0x60FF, 0x00, (int16_t)rpm_l);
	sytron_write_sdo(dev, sytron_motor_id[SYTRON_MOTOR_RIGHT], SDO_W4, 0x60FF, 0x00, (int16_t)rpm_r);
	
	return 0;
}

//电机使能
int32_t sytron_motor_enable(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W2, 0x6040, 0x00, 0x06);
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W2, 0x6040, 0x00, 0x07);
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W2, 0x6040, 0x00, 0x0F);

	return 0;
}

//电机失能
int32_t sytron_motor_disable(const struct ca_device *dev, SYTRON_MOTORS motor)
{
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	sytron_write_sdo(dev, sytron_motor_id[motor], SDO_W2, 0x6040, 0x00, 0x07);

	return 0;
}

int32_t sytron_pv_init(const struct ca_device *dev)
{
	int index = 0;
	
	if( NULL == dev )
	{
		return -1;
	}

	for(index = 0; index < SYTRON_MOTOR_MAX; index++)
	{
		//激活节点
		sytron_activate(dev, index);

		//设置速度模式
		sytron_control_mode_set(dev, index, PV_Mode);

		//设置加减速度
		sytron_set_accelerate(dev, index);
		sytron_set_decelerate(dev, index);

		//设置目标转速为0
		sytron_set_target_rpm(dev, index, 0);

		//设置位置反馈
		sytron_set_position_feedback(dev, index);

		//使能电机
		sytron_motor_enable(dev, index);
	}

	if(osKernelGetState() == osKernelRunning)
    {
        osDelay(5);
    }
    else
    {
        HAL_Delay(5);
    }

	
	return 0;
}

//电机参数设置
int32_t sytron_set_param(const struct ca_device *dev, MOTOR_PARAM param)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	p_sytron = (struct sytron_args *)dev->device_args;
	
	if (osMutexAcquire(p_sytron->sytron_mutex, 0) == osOK) 
	{
      memcpy(&p_sytron->sytron_param, &param, sizeof(MOTOR_PARAM));

			//TODO:此处写到flash保存
			
      osMutexRelease(p_sytron->sytron_mutex);
  }

	return 0;
}

//电机参数读取
int32_t sytron_get_param(const struct ca_device *dev, MOTOR_PARAM *param)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args) || (NULL == param))
	{
		return -1;
	}
	
	p_sytron = (struct sytron_args *)dev->device_args;
	
	if (osMutexAcquire(p_sytron->sytron_mutex, 0) == osOK) 
	{
      memcpy(param, &p_sytron->sytron_param, sizeof(MOTOR_PARAM));
			
      osMutexRelease(p_sytron->sytron_mutex);
  }

	return 0;
}

//电机数据设置
int32_t sytron_set_data(const struct ca_device *dev, MOTOR_DATA data)
{
	return 0;
}

//电机数据接收回调
void sytron_rx_callback(const struct ca_device *dev)
{
	struct sytron_args *p_sytron = NULL;
	
	if((NULL == dev)||(NULL == dev->device_args))
	{
		return ;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
	osThreadFlagsSet(p_sytron->sytron_thread, 0x0001U);
}

//电机设置数据回调
int32_t sytron_set_data_cb(const struct ca_device *dev, MOTOR_DATA_CB_ARG *cb_arg)
{
	struct sytron_args *p_sytron = NULL;
	struct sytron_data_cb_node *p_callback = NULL;
	
	if((NULL == dev)||(NULL == dev->device_args) || (NULL == cb_arg))
	{
		return -1;
	}

	p_sytron = (struct sytron_args *)dev->device_args;
			
	p_callback = mem_block_alloc(sizeof(struct sytron_data_cb_node));
	if(NULL == p_callback)
	{
		return -1;
	}

	memcpy(&p_callback->data_cb, cb_arg, sizeof(MOTOR_DATA_CB_ARG));

	list_add_tail(&p_callback->node, &p_sytron->data_cb_list);

	return 0;
}


//电机数据读取
int32_t sytron_get_data(const struct ca_device *dev, MOTOR_DATA *data, uint32_t data_len)
{
	struct sytron_args *p_sytron = NULL;
	uint8_t index = 0;
	
	if((NULL == dev) || (NULL == dev->device_args) || (NULL == data))
	{
		return -1;
	}

	memset(data, 0, data_len);
	p_sytron = (struct sytron_args *)dev->device_args;
	if(data_len < p_sytron->motor_num * sizeof(MOTOR_DATA))
	{
		return -1;
	}
	
	if (osMutexAcquire(p_sytron->sytron_mutex, 0) == osOK) 
	{
		for(index = 0; index < p_sytron->motor_num; index ++)
		{			
			memcpy(&data[index], &p_sytron->sytron_data[index], sizeof(MOTOR_DATA));
		}
				
		osMutexRelease(p_sytron->sytron_mutex);
  }

	return 0;
}

//森创位置信息处理线程
static void sytron_position_run(void *argument) 
{
	struct ca_device *dev = (struct ca_device *)argument;
	struct sytron_args *p_sytron = NULL;
	struct list_struct *p_list = NULL;
	struct sytron_data_cb_node *p_callback = NULL;
	uint32_t id = 0;
	int32_t len = 0;
	int32_t encoding_num = 0;
	int32_t index = 0;
	// uint32_t tmp = 0;
	uint32_t tick = 0;

	can_filter_config(dev->bus, 0, 0);

	if(osKernelGetState() == osKernelRunning)
    {
        osDelay(1000);
		osDelay(1000);
		osDelay(1000);
    }
    else
    {
        HAL_Delay(1000);
		HAL_Delay(1000);
		HAL_Delay(1000);
    }


	sytron_pv_init(dev);
	
	if((NULL == dev) || (NULL == dev->bus) || (NULL == dev->device_args))
	{
		return;
	}

	p_sytron = dev->device_args; 
	
	tick = osKernelGetTickCount();
	
	while(1)
	{
		osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever);
		len = can_read(dev->bus, &id, p_sytron->data, sizeof(p_sytron->data));
		id &= 0xF;
		
		for(index = 0; index < SYTRON_MOTOR_MAX; index++)
		{
			if((id & 0xF) == sytron_motor_id[index])
			{
				if(len == 4)
				{
					encoding_num = ((int32_t) p_sytron->data[0] << 0) +
					((int32_t) p_sytron->data[1] << 8) +
					((int32_t) p_sytron->data[2] << 16) +
					((int32_t) p_sytron->data[3] << 24);
				}
				else
				{
					encoding_num = ((int32_t) p_sytron->data[4] << 0) +
					((int32_t) p_sytron->data[5] << 8) +
					((int32_t) p_sytron->data[6] << 16) +
					((int32_t) p_sytron->data[7] << 24);
				}

				if (osMutexAcquire(p_sytron->sytron_mutex, 0) == osOK) 
				{
					p_sytron->sytron_data[index].encoding.encoding_num = pow(-1, index)*encoding_num;			
					p_sytron->sytron_data[index].encoding.encoding_circle = p_sytron->sytron_param.motor_encoding_circle;
					osMutexRelease(p_sytron->sytron_mutex);
				}
			}
		}

		//发送数据
		p_list = &p_sytron->data_cb_list;
		while(list_is_last(p_list, &p_sytron->data_cb_list) != 1)
		{
		  p_callback = (struct sytron_data_cb_node*)p_list->next;  
			if(NULL != p_callback->data_cb.fn_callback)
			{
				p_callback->data_cb.fn_callback(p_sytron->sytron_data, 2*sizeof(MOTOR_DATA));
			}
			p_list = p_list->next;
		}
		if (NULL != p_callback)
		{
			osDelayUntil(tick);
			tick = osKernelGetTickCount() + p_callback->data_cb.period;
			// LOG_DEBUG("1  ==   %ld,   2 ==   %ld   %ld", p_sytron->sytron_data[0].encoding.encoding_num, p_sytron->sytron_data[1].encoding.encoding_num, p_callback->data_cb.period);
		}

		//TODO:此处还需要考虑异常信息处理
		
	}
}


int32_t sytron_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t sytron_close(const struct ca_device *dev)
{
	return 0;
}

int32_t sytron_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if(size != sizeof(MOTOR_DATA))
	{
		return -1;
	}
	
	return sytron_get_data(dev, (MOTOR_DATA *)buffer, size);
}

int32_t sytron_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return 0;
}

/*****************************************************************/
/**
 * Function:       sytron_ioctl
 * Description:    森创电机控制指令
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
int32_t sytron_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	if(NULL == dev)
	{
		return -1;
	}

	switch(cmd)
	{								
		case MOTOR_CMD_SET_SPEED:					//设置速度mm/s
		
			syntron_set_speed(dev, *(MOTOR_SPEED*) arg);

			break;						
		case MOTOR_CMD_SET_PARAM:					//设置参数
		
			sytron_set_param(dev, *(MOTOR_PARAM*)(arg));
			
			break;						
		case MOTOR_CMD_GET_PARAM:					//读取参数
		
			sytron_get_param(dev, (MOTOR_PARAM*)arg);
			
			break;					
		case MOTOR_CMD_ENABLE:						//使能
		
			sytron_motor_enable(dev, SYTRON_MOTOR_LEFT);
			sytron_motor_enable(dev, SYTRON_MOTOR_RIGHT);
		
			break;								
		case MOTOR_CMD_DISABLE:						//失能
		
			sytron_motor_disable(dev, SYTRON_MOTOR_LEFT);
			sytron_motor_disable(dev, SYTRON_MOTOR_RIGHT);
		
			break;							
		case MOTOR_CMD_RESET:							//复位
		
			break;							
		case MOTOR_CMD_SET_DATA_CB:
			sytron_set_data_cb(dev, (MOTOR_DATA_CB_ARG *)arg);
			break;
		default:
			return -1;
	}
	return 0;
}


/*****************************************************************/
/**
 * Function:       sytron_init
 * Description:    初始化 sytron
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
int32_t sytron_init(struct ca_device *dev)
{
	struct sytron_args *p_args = NULL;

	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = sytron_open;
	dev->ops.close = sytron_close;
	dev->ops.read = sytron_read;
	dev->ops.write = sytron_write;
	dev->ops.ioctl = sytron_ioctl;

	p_args = (struct sytron_args *)mem_block_alloc(sizeof(struct sytron_args));
	if(NULL == p_args)
	{
		return -1;
	}

	memset(p_args, 0, sizeof(struct sytron_args));
	p_args->motor_num = SYTRON_MOTOR_MAX;
	p_args->data_len = sizeof(p_args->data);
	memcpy(&p_args->sytron_param, &sytron_default_param, sizeof(MOTOR_PARAM));
	//TODO:可以从flash中直接读取

	list_head_init(&p_args->data_cb_list);
	p_args->sytron_thread = osThreadNew(sytron_position_run, (void *)dev, &sytron_motor_position_attributes);
	p_args->sytron_semphore = osSemaphoreNew(p_args->data_len, 0U, NULL);
	p_args->sytron_mutex = osMutexNew(&mutex_imu_data_attr);

	dev->device_args = p_args;

	can_rxfifo_register_callback(dev, sytron_rx_callback);

	
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       sytron_deinit
 * Description:    去初始化 sytron
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
int32_t sytron_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(sytron, can, sytron_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_SYTRON */
/* @} Robot_DEVICES */


