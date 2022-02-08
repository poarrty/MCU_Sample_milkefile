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

#ifndef _DEFINE_MOTOR_H
#define _DEFINE_MOTOR_H

/*****************************************************************
* 包含头文件
******************************************************************/

/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_DEVICES
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
* 宏定义
******************************************************************/
#define PI      3.1415926f

#define MOTOR_DISABLE 0
#define MOTOR_ENABLE 	1

/*****************************************************************
* 结构定义
******************************************************************/

//电机类型
typedef enum
{
	MOTOR_TYPE_SYTRON,								//森创
	MOTOR_TYPE_FENGDEKONG,						//风得控
	MOTOR_TYPE_CVTE,									//自研电机，TODO:此处应有型号
	MOTOR_TYPE_MAX
}MOTOR_TYPE;

//电机控制指令类型
typedef enum
{
	MOTOR_CMD_START,								//启动
	MOTOR_CMD_STOP,									//停止
	MOTOR_CMD_SET_SPEED,						//设置速度mm/s
	MOTOR_CMD_SET_RPM,							//设置转速		rad/min
	MOTOR_CMD_SET_PARAM,						//设置参数
	MOTOR_CMD_GET_PARAM,						//读取参数
	MOTOR_CMD_ENABLE,								//使能
	MOTOR_CMD_DISABLE,							//失能
	MOTOR_CMD_RESET,								//复位
	MOTOR_CMD_GET_CURRENT,					//获取电流值
	MOTOR_CMD_SET_CURRENT,					//设置电流自动检测
	MOTOR_CMD_SET_DATA_CB,					//注册数据回调函数
	MOTOR_CMD_MAX
}MOTOR_CMD;

//电机刹车类型
typedef enum {
    MOTOR_LOCK_NO,						//不刹车
    MOTOR_LOCK_NORMAL,				//正常刹车
    MOTOR_LOCK_EMERGER,				//紧急刹车
    MOTOR_LOCK_MAX
} MOTOR_LOCK_TYPE;

//电机异常类型
#pragma pack(push,1)
typedef struct
{
    uint32_t motor_over_voltage:1;	//过压
		uint32_t motor_over_current:1;	//过流
		uint32_t motor_under_voltage:1;	//欠压
		uint32_t motor_over_load:1;			//过载
		uint32_t motor_over_heat:1;			//过热
		//TODO:继续增加
} MOTOR_ERROR;
#pragma pack(pop)

//电机编码数据
typedef struct
{
	int32_t encoding_num;				//当前编码值
  int32_t encoding_circle;		//一圈的编码值
}MOTOR_ENCODING;

//电机速度
typedef struct
{
	int16_t speed_v_t;					//线速度
	int16_t speed_w_t;					//角速度
}MOTOR_SPEED;

//电机数据
typedef struct
{
	MOTOR_ERROR error;					//电机当前异常
	MOTOR_ENCODING encoding;		//电机编码数据
	MOTOR_SPEED speed;					//电机速度
	float rpm;								//电机转速
	float fg_rpm;						//反馈转速
	uint16_t current;						//电流
	//TODO：此处可添加温度
}MOTOR_DATA;

//电机参数
typedef struct
{
	MOTOR_TYPE motor_type;					//电机类型
	uint32_t wheel_diameter;  			//车轮直径/mm
	uint32_t wheel_space;     			//车轮间距/mm
	uint32_t motor_acc_up;					//电机控制上升加速度      mm/s^2
	uint32_t motor_acc_down;				//电机控制下降加速度      mm/s^2
  uint32_t motor_reduction_ratio;	//电机减速比
  uint32_t motor_pole;						//电机极对数
  uint32_t motor_encoding_circle; //电机转一圈位置增加总量
  uint32_t motor_rpm_max;         //电机最大转速
  uint32_t motor_mmps_max;        //最大速度 mm/s
  uint32_t motor_current_max;			//电机允许最大电流
}MOTOR_PARAM;

typedef void (*motor_data_callback)(const MOTOR_DATA *data, uint32_t size); 
//注册电机数据回调函数参数
typedef struct
{
	uint32_t period;									//周期(单位:ms)
	motor_data_callback fn_callback;	//回调函数
}MOTOR_DATA_CB_ARG;

//无刷直流电机总线
typedef struct
{
	struct bus_info pwm;						//无刷直流电机pwm控制速度
	struct bus_info fg_ic;					//无刷直流电机速度反馈(输入捕获方式,三选一)
	struct bus_info fg_exti;				//无刷直流电机速度反馈(gpio中断方式,三选一)
	struct bus_info fg_etr;					//无刷直流电机速度反馈(etr方式,编码器,三选一)
	struct bus_info adc;						//无刷直流电机电流采样
	struct bus_info gpio_vcc;				//无刷直流电机电源控制
	struct bus_info gpio_brake;			//无刷直流电机刹车
	struct bus_info gpio_cw_ccw;		//无刷直流电机反转
}MOTOR_BLDCM_BUS;

//无刷直流电机属性
typedef struct
{
	uint32_t bldcm_nums;						//无刷直流电机数量(左右)
	MOTOR_BLDCM_BUS *bldcm_bus;			//无刷直流电机使用总线参数
	uint32_t gpio_cw_ccw_status;		//无刷直流电机反转状态
	void *user_args;								//用户参数
}MOTOR_BLDCM_ATTR;

//有刷直流电机属性
typedef struct
{
	uint8_t open_state;                //打开电机PIN脚的状态
}MOTOR_BDCM_ATTR;

/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/


/*****************************************************************/
/**
 * Function:       speed_translate
 * Description:    速度转换，将线速度和角速度转换为下发给电机的 rpm(圈/分钟)
 * Calls:          
 * Called By:      
 * @param[in]      
 * @param[out]     
 * @return         
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
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
static uint8_t motor_speed_translate(MOTOR_PARAM motor_param, MOTOR_SPEED motor_vw, float  *rpm_l, float  *rpm_r)
{
	float  rpm_v, rpm_turn;
	float rpm_unit = 0;

	if((NULL == rpm_l) || (NULL == rpm_r))
	{
		return -1;
	}
	
  rpm_unit = 298 / 60 ;				///< 2*PI*R/60 --> 1 rpm 对应的 unit mm/s

//   rpm_v = motor_vw.speed_v_t/((int16_t)rpm_unit);
  rpm_v = motor_vw.speed_v_t/rpm_unit;																										///< 直行速度

  rpm_turn = 1.0*motor_vw.speed_w_t/1000*350/2/rpm_unit;		///< 转弯速度
  
  *rpm_l = rpm_v+ rpm_turn;
  *rpm_r = rpm_v- rpm_turn;
    
    return 0;
}

/*****************************************************************
* 函数说明
******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */


#endif
