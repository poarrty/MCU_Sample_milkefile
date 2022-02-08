/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:			 bldcm.c (brushless direct current motor 无刷直流电机)
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
#include "bldcm.h"
#include "devices.h"
#include "define_motor.h"
#include "tim_core.h"
#include "gpio_core.h"
#include "adc_core.h"
#include "mem_pool.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "log.h"
#include <math.h>
#include "pid.h"
#include "shell.h"
/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_BLDCM  - 无刷直流电机
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
typedef enum
{
	BLDCM_MOTOR_LEFT,			//左轮电机
	BLDCM_MOTOR_RIGHT,		//右轮电机
	BLDCM_MOTOR_MAX				//最大支持电机数量(轴)
}BLDCM_MOTORS;

typedef struct 
{
	struct ca_bus *pwm;								//无刷直流电机pwm控制速度
	struct ca_bus *fg_ic;							//无刷直流电机速度反馈(输入捕获方式,三选一)
	struct ca_bus *fg_exti;						//无刷直流电机速度反馈(gpio中断方式,三选一)
	struct ca_bus *fg_etr;						//无刷直流电机速度反馈(etr方式,编码器,三选一)
	struct ca_bus *adc;								//无刷直流电机电流采样
	struct ca_bus *gpio_vcc;					//无刷直流电机电源控制
	struct ca_bus *gpio_brake;				//无刷直流电机刹车
	struct ca_bus *gpio_cw_ccw;				//无刷直流电机反转
}BLDCM_BUS;
struct bldcm_data_cb_node
{
	struct list_struct node;
	MOTOR_DATA_CB_ARG data_cb;
};
struct bldcm_args
{
	int32_t motor_num;															//当前支持电机数量
	uint8_t gpio_cw_ccw_status;											//电机反转状态
	BLDCM_BUS bldcm_bus[BLDCM_MOTOR_MAX];						//电机控制总线
	MOTOR_DATA bldcm_data[BLDCM_MOTOR_MAX];					//电机数据
	MOTOR_PARAM bldcm_param;												//电机参数
	uint32_t fg_interval[BLDCM_MOTOR_MAX];					//电机反馈中断间隔计数
	osThreadId_t bldcm_thread[3];											//处理线程id
	osMutexId_t bldcm_mutex;												//互斥锁
	osSemaphoreId_t bldcm_semphore;									//信号量	
	int32_t is_enable_current_detect;								//是否使能电流检测，默认不使能
	uint8_t first_etr_it_flag[BLDCM_MOTOR_MAX];						//判断是否第一次进中断
	uint8_t real_cw_ccw_status[BLDCM_MOTOR_MAX];					//左右电机反转状态
	struct list_struct data_cb_list;					      //数据回调函数链表
};

/*****************************************************************
* 全局变量定义
******************************************************************/

float v_set_rpm[2];
float v_real_rpm[2];
float v_conv_val[2];
/*****************************************************************
* 私有全局变量定义
******************************************************************/
	const osThreadAttr_t bldcm_thread_attributes = {
		.name = "bldcm_thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 256*4
	};
		const osThreadAttr_t bldcm_pub_thread_attributes = {
		.name = "bldcm_pub_thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 256*4
	};
			const osThreadAttr_t bldcm_motor_ctrl_thread_attributes = {
		.name = "bldcm_motor_ctrl_thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 256*4
	};
const osMutexAttr_t mutex_bldcm_attr = {
  "bldcm_mutex",      										// human readable mutex name
  osMutexRecursive | osMutexPrioInherit,  // attr_bits
  NULL,                                   // memory for control block
  0U                                      // size for control block
};

/*****************************************************************
* 外部变量声明
******************************************************************/
int flag = 0;
/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/
void bldcm_ic_callback(const struct ca_device *dev, void *handler)
{
	//TODO:编码值
	uint8_t index = 0;
	struct bldcm_args *p_args = NULL;

	if((NULL == dev) || (NULL == dev->device_args))
	{
		return;
	}

	p_args = (struct bldcm_args *)dev->device_args;

	for(index = 0; index < p_args->motor_num; index++)
	{
		if((NULL != p_args->bldcm_bus[index].fg_ic) && (p_args->bldcm_bus[index].fg_ic->handler == handler))
		{
			if(0 == p_args->fg_interval[index])
			{
				p_args->fg_interval[index] = osKernelGetTickCount();
			}
			else
			{
				p_args->fg_interval[index] = osKernelGetTickCount() - p_args->fg_interval[index];
				osSemaphoreRelease(p_args->bldcm_semphore);
				//关中断
				tim_capture_stop(p_args->bldcm_bus[index].fg_ic);
			}
			break;
		}
	}
	
}

void bldcm_exti_callback(const struct ca_device *dev, uint16_t gpio_pin)
{
	//TODO:编码值
	uint8_t index = 0;
	struct bldcm_args *p_args = NULL;

	if((NULL == dev) || (NULL == dev->device_args))
	{
		return;
	}

	p_args = (struct bldcm_args *)dev->device_args;

	for(index = 0; index < p_args->motor_num; index++)
	{
		if((NULL != p_args->bldcm_bus[index].fg_exti) && (p_args->bldcm_bus[index].fg_exti->bus_addr == gpio_pin))
		{
			if(0 == p_args->fg_interval[index])
			{
				p_args->fg_interval[index] = osKernelGetTickCount();
			}
			else
			{
				p_args->fg_interval[index] = osKernelGetTickCount() - p_args->fg_interval[index];
				osSemaphoreRelease(p_args->bldcm_semphore);
				//关中断
				gpio_exti_stop(p_args->bldcm_bus[index].fg_exti);
			}
			break;
		}
	}
	
}

void bldcm_etr_reload_callback(const struct ca_device *dev, void *handler)
{

	//TODO:编码值
	uint8_t index = 0;
	struct bldcm_args *p_args = NULL;

	if((NULL == dev) || (NULL == dev->device_args))
	{
		return;
	}

	p_args = (struct bldcm_args *)dev->device_args;

	

	for(index = 0; index < p_args->motor_num; index++)
	{
		if((p_args->first_etr_it_flag[index] == 0)&& (p_args->bldcm_bus[index].fg_etr->handler == handler))//判断是否为第一次进中断
		{
			p_args->first_etr_it_flag[index] = 1;
			return;
		}
		if((NULL != p_args->bldcm_bus[index].fg_etr) && (p_args->bldcm_bus[index].fg_etr->handler == handler))
		{
			//编码值只能这里写，其他地方只能读
			if(p_args->real_cw_ccw_status[index]==1)
				p_args->bldcm_data[index].encoding.encoding_num += tim_get_period(p_args->bldcm_bus[index].fg_etr);
			if(p_args->real_cw_ccw_status[index]==0)
				p_args->bldcm_data[index].encoding.encoding_num -= tim_get_period(p_args->bldcm_bus[index].fg_etr);
			break;
		}
	}
	
}

//查找总线
int32_t bldcm_bus_find(MOTOR_BLDCM_BUS *motor_bus, BLDCM_BUS *bldcm_bus)
{
	uint8_t index = 0;
	struct ca_bus **pp_ca_bus = NULL;
	struct bus_info *p_bus_info = NULL;
	
	if((NULL == motor_bus) || (NULL == bldcm_bus))
	{
		return -1;
	}

	memset(bldcm_bus, 0, sizeof(BLDCM_BUS));

	if(sizeof(MOTOR_BLDCM_BUS)/sizeof(struct bus_info) != sizeof(BLDCM_BUS)/sizeof(struct ca_bus *))
	{
		return -1;
	}

	for(index = 0; index < sizeof(MOTOR_BLDCM_BUS)/sizeof(struct bus_info); index++)
	{
		p_bus_info = (struct bus_info *)((void *)motor_bus + index*sizeof(struct bus_info));
		pp_ca_bus = (struct ca_bus **)((void *)bldcm_bus + index*sizeof(struct ca_bus *));
		
		if(0 != strlen((char *)p_bus_info->bus_name))
		{
			bus_find_name(p_bus_info, pp_ca_bus);
		}
	}
	
	return 0;
}

//设置电机转速
int32_t bldcm_set_rpm(const struct ca_device *dev, BLDCM_MOTORS motor, int32_t rpm)
{	

	struct bldcm_args *p_bldcm = NULL;
	float duty_cycle = 0;
	uint8_t cw_ccw_status = 0; 
	p_bldcm = dev->device_args;
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	if(abs(rpm) > p_bldcm->bldcm_param.motor_rpm_max)
	{
		return -1;
	}
	if (p_bldcm->real_cw_ccw_status[motor]==0)
	{
		if(rpm>0)
		{
			//本来反转 现在变成正转
			p_bldcm->bldcm_data[motor].encoding.encoding_num  = p_bldcm->bldcm_data[motor].encoding.encoding_num -tim_get_counter(p_bldcm->bldcm_bus[motor].fg_etr);
			tim_set_counter(p_bldcm->bldcm_bus[motor].fg_etr,0);
			p_bldcm->real_cw_ccw_status[motor] = 1;
			PID_Init(&pid[motor]);
		}
	}
	else 
	{
		if(rpm<0)
		{
			//本来正转 现在变成反转
			p_bldcm->bldcm_data[motor].encoding.encoding_num  = p_bldcm->bldcm_data[motor].encoding.encoding_num +tim_get_counter(p_bldcm->bldcm_bus[motor].fg_etr);
			tim_set_counter(p_bldcm->bldcm_bus[motor].fg_etr,0);
			p_bldcm->real_cw_ccw_status[motor] = 0;
			PID_Init(&pid[motor]);
		}
	}
	

	if(rpm > 0)
	{
		cw_ccw_status = p_bldcm->gpio_cw_ccw_status ^ 1;
		gpio_write(p_bldcm->bldcm_bus[motor].gpio_cw_ccw, (void *)&cw_ccw_status, sizeof(cw_ccw_status));

	}
	else
	{
		gpio_write(p_bldcm->bldcm_bus[motor].gpio_cw_ccw, (void *)&p_bldcm->gpio_cw_ccw_status, sizeof(p_bldcm->gpio_cw_ccw_status));
	}
	if(strcmp((char *)dev->device_name,"bldcm_wheel")==0)
	{
		p_bldcm->bldcm_data[motor].rpm = rpm;
		return 0;
	}
	//根据转速计算对应的占空比(实际转速≈最高转速*占空比，实际转速会随负载变化而变化)
	duty_cycle = (float)abs(rpm)/(float)p_bldcm->bldcm_param.motor_rpm_max;	
	//设置占空比
	tim_pwm_start(p_bldcm->bldcm_bus[motor].pwm, duty_cycle,0);
	//保存到数据中

	if (osMutexAcquire(p_bldcm->bldcm_mutex, 0) == osOK) 
	{
			p_bldcm->bldcm_data[motor].rpm = rpm;			
      osMutexRelease(p_bldcm->bldcm_mutex);
  	}	

	


	return 0;
}

//设置电机速度
int32_t bldcm_set_speed(const struct ca_device *dev, MOTOR_SPEED motor_vw) 
{
	struct bldcm_args *p_bldcm = NULL;
	float rpm_min, rpm_max;
	float  rpm[BLDCM_MOTOR_MAX];
	int8_t index = 0;
	
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_bldcm = (struct bldcm_args *)dev->device_args;


	//先计算为转速，再设置
	motor_speed_translate(p_bldcm->bldcm_param, motor_vw, &rpm[BLDCM_MOTOR_LEFT], &rpm[BLDCM_MOTOR_RIGHT]);

	rpm_min = -1 * p_bldcm->bldcm_param.motor_rpm_max;
	rpm_max = 1 * p_bldcm->bldcm_param.motor_rpm_max;

	//底层限速
	for(index = 0; index < BLDCM_MOTOR_MAX; index++)
	{
		rpm[index] = (rpm[index] > rpm_max) ? (rpm_max) : (rpm[index]);
		rpm[index] = rpm[index] * p_bldcm->bldcm_param.motor_reduction_ratio;
		rpm[index] *= pow(-1, index);

		//设置转速
		bldcm_set_rpm(dev, index, rpm[index]);

		//保存到数据中
		if (osMutexAcquire(p_bldcm->bldcm_mutex, 0) == osOK) 
		{
				memcpy(&p_bldcm->bldcm_data[index].speed, &motor_vw, sizeof(MOTOR_SPEED));			
	      osMutexRelease(p_bldcm->bldcm_mutex);
	  }
	}	

  return 0;
}

//电机电流设置
int32_t bldcm_set_current(const struct ca_device *dev, int32_t enable)
{
	struct bldcm_args *p_bldcm = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	if(NULL != p_bldcm)
	{
		p_bldcm->is_enable_current_detect = enable;
	}
	
	return 0;
}

//电机电流读取
int32_t bldcm_get_current(const struct ca_device *dev, BLDCM_MOTORS motor, uint16_t *current)
{
	struct bldcm_args *p_bldcm = NULL;
	int8_t index = 0;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}

	p_bldcm = (struct bldcm_args *)dev->device_args;
	if(NULL == p_bldcm->bldcm_bus[motor].adc)
	{
		return -1;
	}
	
	*current = adc_read(p_bldcm->bldcm_bus[motor].adc, p_bldcm->bldcm_bus[motor].adc->bus_addr);
	
	return 0;
}


//电机参数设置
int32_t bldcm_set_param(const struct ca_device *dev, MOTOR_PARAM param)
{
	struct bldcm_args *p_bldcm = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args))
	{
		return -1;
	}
	
	p_bldcm = (struct bldcm_args *)dev->device_args;
	
	if (osMutexAcquire(p_bldcm->bldcm_mutex, 0) == osOK) 
	{
      memcpy(&p_bldcm->bldcm_param, &param, sizeof(MOTOR_PARAM));

			//TODO:此处写到flash保存
			
      osMutexRelease(p_bldcm->bldcm_mutex);
  }

	return 0;
}


//电机参数读取
int32_t bldcm_get_param(const struct ca_device *dev, MOTOR_PARAM *param)
{
	struct bldcm_args *p_bldcm = NULL;
	
	if((NULL == dev) || (NULL == dev->device_args) || (NULL == param))
	{
		return -1;
	}

	p_bldcm = (struct bldcm_args *)dev->device_args;
	
	if (osMutexAcquire(p_bldcm->bldcm_mutex, 0) == osOK) 
	{
      memcpy(param, &p_bldcm->bldcm_param, sizeof(MOTOR_PARAM));
			
      osMutexRelease(p_bldcm->bldcm_mutex);
  }

	return 0;
}

//电机数据设置
int32_t bldcm_set_data(const struct ca_device *dev, MOTOR_DATA data)
{

	return 0;
}

//电机数据读取
int32_t bldcm_get_data(const struct ca_device *dev, MOTOR_DATA *data, uint32_t data_len)
{
	struct bldcm_args *p_bldcm = NULL;
	int8_t index = 0;
	
	if((NULL == dev) || (NULL == dev->device_args) || (NULL == data))
	{
		return -1;
	}

	memset(data, 0, data_len);
	p_bldcm = (struct bldcm_args *)dev->device_args;
	if(data_len < p_bldcm->motor_num * sizeof(MOTOR_DATA))
	{
		return -1;
	}
	
	if (osMutexAcquire(p_bldcm->bldcm_mutex, 0) == osOK) 
	{
			for(index = 0; index < p_bldcm->motor_num; index++)
			{
				memcpy(&data[index], &p_bldcm->bldcm_data[index], sizeof(MOTOR_DATA));
				if(NULL != p_bldcm->bldcm_bus[index].fg_etr)
				{	
					if(p_bldcm->real_cw_ccw_status[index]==1)
						data[index].encoding.encoding_num  = p_bldcm->bldcm_data[index].encoding.encoding_num + tim_get_counter(p_bldcm->bldcm_bus[index].fg_etr);
					if(p_bldcm->real_cw_ccw_status[index]==0)
						data[index].encoding.encoding_num  = p_bldcm->bldcm_data[index].encoding.encoding_num - tim_get_counter(p_bldcm->bldcm_bus[index].fg_etr);
					//data[index].encoding.encoding_num  = p_bldcm->bldcm_data[index].encoding.encoding_num - tim_get_counter(p_bldcm->bldcm_bus[index].fg_etr)+tim_get_period(p_bldcm->bldcm_bus[index].fg_etr);
				}

				bldcm_get_current(dev, index, &data[index].current);
			}
			
      osMutexRelease(p_bldcm->bldcm_mutex);
  }

	return 0;
}

//电机设置数据回调
int32_t bldcm_set_data_cb(const struct ca_device *dev, MOTOR_DATA_CB_ARG *cb_arg)
{
	struct bldcm_args *p_bldcm = NULL;
	struct bldcm_data_cb_node *p_callback = NULL;
	
	if((NULL == dev)||(NULL == dev->device_args) || (NULL == cb_arg))
	{
		return -1;
	}

	p_bldcm = (struct bldcm_args *)dev->device_args;
			
	p_callback = mem_block_alloc(sizeof(struct bldcm_data_cb_node));
	if(NULL == p_callback)
	{
		return -1;
	}

	memcpy(&p_callback->data_cb, cb_arg, sizeof(MOTOR_DATA_CB_ARG));

	list_add_tail(&p_callback->node, &p_bldcm->data_cb_list);

	return 0;
}

//位置信息处理线程
static void bldcm_position_run(void *argument) 
{
	struct ca_device *dev = (struct ca_device *)argument;
	struct bldcm_args *p_bldcm = NULL;
	uint16_t freq;		//频率(每秒产生多少次脉冲)
	uint16_t cnt = 0;	//etr计数值
	uint8_t index = 0;
	int time_flag = 0;
	
	if((NULL == dev) || (NULL == dev->bus) || (NULL == dev->device_args))
	{
		return;
	}

	p_bldcm = dev->device_args; 
	
  while(1)
  {
		//若使用etr方式，则循环读取cnt的值，计算出频率(每秒产生多少次脉冲)
		//若使用输入捕获或者gpio中断，则定时开启中断，读取频率，关中断
		for(index = 0; index < p_bldcm->motor_num; index++)
		{
			//检测电流
			if(p_bldcm->is_enable_current_detect > 0)
			{
				bldcm_get_current(dev, index, &p_bldcm->bldcm_data[index].current);
				if(p_bldcm->bldcm_data[index].current > p_bldcm->bldcm_param.motor_current_max)
				{
					p_bldcm->bldcm_data[index].error.motor_over_current = 1;
					//TODO：触发过流预警，可添加过滤机制
				}
			}

			//检测速度反馈
			if(NULL == p_bldcm->bldcm_bus[index].fg_etr)
			{
				if(NULL != p_bldcm->bldcm_bus[index].fg_ic)
				{
					p_bldcm->fg_interval[index] = 0;
					//开中断
					tim_capture_start(p_bldcm->bldcm_bus[index].fg_ic, TIM_IC_RISING);
					
					//开信号量 //TODO:这里超时的值应该按照实际转速来设置
					osStatus_t status = osSemaphoreAcquire(p_bldcm->bldcm_semphore, osWaitForever);
					if((osErrorTimeout != status) && (p_bldcm->fg_interval[index] != 0))
					{
						//打印fg_interval的值
						freq = configCPU_CLOCK_HZ / p_bldcm->fg_interval[index];
						LOG_DEBUG("%d p_bldcm->fg_interval[%d] = %ld\r\n", status, index, p_bldcm->fg_interval[index]);
					}

				}
				else if(NULL != p_bldcm->bldcm_bus[index].fg_exti)
				{
					p_bldcm->fg_interval[index] = 0;
					//开中断
					gpio_exti_start(p_bldcm->bldcm_bus[index].fg_exti, GPIO_EXTI_RISING);
					
					
					//开信号量 //TODO:这里超时的值应该按照实际转速来设置					
					osStatus_t status = osSemaphoreAcquire(p_bldcm->bldcm_semphore, osWaitForever);
					if((osErrorTimeout != status) && (p_bldcm->fg_interval[index] != 0))
					{
						freq = configCPU_CLOCK_HZ / p_bldcm->fg_interval[index];
						LOG_DEBUG("%d p_bldcm->fg_interval[%d] = %ld\r\n", status, index, p_bldcm->fg_interval[index]);
					}		

					
				}
				
			}
			else{
				//开信号量 //TODO:这里超时的值应该按照实际转速来设置
				cnt = p_bldcm->bldcm_data[index].encoding.encoding_num + tim_get_counter(p_bldcm->bldcm_bus[index].fg_etr);
				osDelay(50);
				cnt = p_bldcm->bldcm_data[index].encoding.encoding_num + tim_get_counter(p_bldcm->bldcm_bus[index].fg_etr) - cnt;
				//LOG_DEBUG(" p_bldcm->fg_interval[%d] = %d\r\n", index, cnt);
				p_bldcm->bldcm_data[index].fg_rpm = cnt*20/2; //code->rps //*60/120
				freq = configCPU_CLOCK_HZ / p_bldcm->fg_interval[index] * cnt;
			}
			
			if(freq < p_bldcm->bldcm_data[index].rpm * 60 * 0.3)		//反馈显示实际转速比设置的转速小于30%，可能已经过载
			{
				//TODO:触发过载预警，可添加过滤机制
			}


		}

		osDelay(80);
		// MOTOR_DATA buffer[2];
		// bldcm_get_data(dev, buffer, 2*sizeof(MOTOR_DATA));
		// if(buffer[0].encoding.encoding_num!=0)
		// {
		// 	LOG_DEBUG(" encoding_num[0] = %ld\r\n", buffer[0].encoding.encoding_num);
		// 	LOG_DEBUG(" encoding_num[1] =         %ld\r\n", buffer[1].encoding.encoding_num);
		// }
	}
}
static void bldcm_motor_ctrl(void *argument)
{
	struct bldcm_args *p_bldcm = NULL;
	struct ca_device *dev = (struct ca_device *)argument;
	p_bldcm = dev->device_args;
	// pid[0]=(struct _pid *)mem_block_alloc(sizeof(pid));
	// pid[1]=(struct _pid *)mem_block_alloc(sizeof(pid));
	PID_Init(&pid[0]);
	PID_Init(&pid[1]);
	float real_rpm = 0;
	float conv_val = 0;
	float duty_cycle = 0;
	int cout[2] = {0};
	while(1)
	{
		MOTOR_DATA buffer[2];
		bldcm_get_data(dev, buffer, 2*sizeof(MOTOR_DATA));
		for(int i=0;i<2;i++)
		{	
			real_rpm = buffer[i].fg_rpm;
			pid[i].SetSpeed = fabs(buffer[i].rpm);
			if(pid[i].SetSpeed == 0){
				PID_Init(&pid[i]);
				conv_val = 0.01;
			}else{
				conv_val = PID_Speed_Incr(&pid[i],real_rpm);
			}
			tim_pwm_start(p_bldcm->bldcm_bus[i].pwm, conv_val,0);
			cout[i]++;
			if(cout[i]>=100)
			{
				LOG_DEBUG("set_rpm:%f real_rpm:%fconv_val:%f", buffer[i].rpm, real_rpm,conv_val);
				cout[i] = 0;
			}
			//uart_send_senser(buffer[i].rpm,real_rpm,conv_val,0,0,0,0,0);
			v_conv_val[i] = conv_val;
			v_real_rpm[i] = real_rpm;
			v_set_rpm[i] = buffer[i].rpm;
 		}
		osDelay(50);
	}

}
static void bldcm_pub_run(void *argument) 
{
	struct ca_device *dev = (struct ca_device *)argument;
	struct bldcm_args *p_bldcm = NULL;
	struct list_struct *p_list = NULL;
	struct bldcm_data_cb_node *p_callback = NULL;

	MOTOR_DATA buffer[2];

	buffer[0].encoding.encoding_circle = 120;
	buffer[1].encoding.encoding_circle = 120;

	if((NULL == dev) || (NULL == dev->bus) || (NULL == dev->device_args))
	{
		return;
	}

	p_bldcm = dev->device_args; 
	int tick = 0;
	while(1)
	{	
		p_list = &p_bldcm->data_cb_list;
		while(list_is_last(p_list, &p_bldcm->data_cb_list) != 1)
		{
		  p_callback = (struct bldcm_data_cb_node*)p_list->next;  
			if(NULL != p_callback->data_cb.fn_callback)
			{
				if(tick%p_callback->data_cb.period == 0)
				{
					bldcm_get_data(dev, buffer, 2*sizeof(MOTOR_DATA));
					p_callback->data_cb.fn_callback(buffer, 2*sizeof(MOTOR_DATA));
				}
			}
			p_list = p_list->next;
		}
		osDelay(1);
		tick ++;
		if(tick >= 0x7ffffff0)
			tick =0;
	}
	
}

int32_t bldcm_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t bldcm_close(const struct ca_device *dev)
{
	return 0;
}

int32_t bldcm_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if(size != sizeof(MOTOR_DATA))
	{
		return -1;
	}
	
	return bldcm_get_data(dev, (MOTOR_DATA *)buffer, size);
}

int32_t bldcm_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return 0;
}

/*****************************************************************/
/**
 * Function:       bldcm_ioctl
 * Description:    无刷直流电机控制指令
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
int32_t bldcm_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	int8_t index = 0;
	
	if(NULL == dev)
	{
		return -1;
	}

	switch(cmd)
	{								
		case MOTOR_CMD_SET_SPEED:					//设置速度mm/s
		
			bldcm_set_speed(dev, *(MOTOR_SPEED*)arg);

			break;	
		case MOTOR_CMD_SET_RPM:					 //设置转速		rad/min

			for(index = 0; index < BLDCM_MOTOR_MAX; index++)
			{
				bldcm_set_rpm(dev, index, pow(-1,index) * *(int32_t*)arg);
			}

			break;		
		case MOTOR_CMD_SET_PARAM:					//设置参数
		
			bldcm_set_param(dev, *(MOTOR_PARAM*)(arg));
			
			break;						
		case MOTOR_CMD_GET_PARAM:					//读取参数
		
			bldcm_get_param(dev, (MOTOR_PARAM*)arg);
			
			break;						
		case MOTOR_CMD_RESET:							//复位
		
			break;							
		case MOTOR_CMD_SET_CURRENT:
			bldcm_set_current(dev, *(int32_t *)arg);
			break;
		case MOTOR_CMD_SET_DATA_CB:
			bldcm_set_data_cb(dev, (MOTOR_DATA_CB_ARG *)arg);
		default:
			return -1;
	}
	return 0;
}


/*****************************************************************/
/**
 * Function:       bldcm_init
 * Description:    初始化 bldcm
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
int32_t bldcm_init(struct ca_device *dev)
{
	struct bldcm_args *p_args = NULL;
	MOTOR_BLDCM_ATTR *p_bldcm_attr = NULL;
	MOTOR_BLDCM_BUS *p_bus = NULL;
	uint8_t index = 0;

	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->device_args))
	{
		return -1;
	}

	p_bldcm_attr = dev->device_args;
	if((p_bldcm_attr->bldcm_nums <= 0) && (p_bldcm_attr->bldcm_nums > BLDCM_MOTOR_MAX))
	{
		return -1;
	}
		
	p_bus = p_bldcm_attr->bldcm_bus;
	if(NULL == p_bus)
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));
	dev->ops.open = bldcm_open;
	dev->ops.close = bldcm_close;
	dev->ops.read = bldcm_read;
	dev->ops.write = bldcm_write;
	dev->ops.ioctl = bldcm_ioctl;

	p_args = (struct bldcm_args *)mem_block_alloc(sizeof(struct bldcm_args));
	if(NULL == p_args)
	{
		return -1;
	}

	memset(p_args, 0, sizeof(struct bldcm_args));
	p_args->motor_num = BLDCM_MOTOR_MAX;
	p_args->gpio_cw_ccw_status = p_bldcm_attr->gpio_cw_ccw_status;
	p_args->real_cw_ccw_status[0] = p_bldcm_attr->gpio_cw_ccw_status;
	p_args->real_cw_ccw_status[1] = p_bldcm_attr->gpio_cw_ccw_status^0x01;

	for(index = 0; index < p_bldcm_attr->bldcm_nums; index++)	
	{
		bldcm_bus_find(&p_bus[index], &p_args->bldcm_bus[index]);
		
		if(NULL != p_args->bldcm_bus[index].fg_ic)
		{
			tim_ic_capture_register_callback(dev, p_args->bldcm_bus[index].fg_ic->handler, bldcm_ic_callback);
		}

		if(NULL != p_args->bldcm_bus[index].fg_exti)
		{
			gpio_exti_register_callback(dev, p_args->bldcm_bus[index].fg_exti->bus_addr, bldcm_exti_callback);
		}

		if(NULL != p_args->bldcm_bus[index].fg_etr)
		{
			tim_period_elapsed_register_callback(dev, p_args->bldcm_bus[index].fg_etr->handler, bldcm_etr_reload_callback);
			tim_period_elapsed_start(p_args->bldcm_bus[index].fg_etr);//一定要先开中断再开etr，否则无法进入中断！！！
			tim_etr_start(p_args->bldcm_bus[index].fg_etr);
		}
		if(NULL != p_args->bldcm_bus[index].pwm)
		{
			tim_pwm_start(p_args->bldcm_bus[index].pwm, 0,0);//pwm置0，防止上电就转
		}
	}
	dev->device_args = (void *)p_args;
	list_head_init(&p_args->data_cb_list);
	p_args->bldcm_semphore = osSemaphoreNew(1, 0U, NULL);
	p_args->bldcm_thread[0] = osThreadNew(bldcm_position_run, (void *)dev, &bldcm_thread_attributes);
    p_args->bldcm_mutex = osMutexNew(&mutex_bldcm_attr);
	//TODO:参数配置
	if(strcmp((char *)dev->device_name,"bldcm_wheel")==0)
	{
		p_args->bldcm_thread[1] = osThreadNew(bldcm_pub_run, (void *)dev, &bldcm_pub_thread_attributes);
		p_args->bldcm_param.motor_rpm_max = 12080;//最大转速 140转每分钟     最大速度0.7m每秒
		p_args->bldcm_param.wheel_diameter = 95;
		p_args->bldcm_param.wheel_space = 350;
		p_args->bldcm_param.motor_reduction_ratio = 1;
		p_args->bldcm_thread[2] = osThreadNew(bldcm_motor_ctrl, (void *)dev, &bldcm_motor_ctrl_thread_attributes);
	}
	else
	{
		p_args->bldcm_param.motor_rpm_max = 10;
		p_args->bldcm_param.wheel_diameter = 200;
		p_args->bldcm_param.wheel_space = 100;
		p_args->bldcm_param.motor_reduction_ratio = 1;
	}
	return 0;
}

/*****************************************************************/
/**
 * Function:       bldcm_deinit
 * Description:    去初始化 bldcm
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
int32_t bldcm_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(bldcm, tim, bldcm_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_BLDCM */
/* @} Robot_DEVICES */


