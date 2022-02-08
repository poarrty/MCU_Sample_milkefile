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

#ifndef _DEFINE_BMS_H
#define _DEFINE_BMS_H

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

/*****************************************************************
 * 结构定义
 ******************************************************************/
//电池模式设置
typedef enum {
    BMS_GET_DATA,			//获取电池数据
    BMS_SET_PARAM,		//设置电池参数
    BMS_SET_MODE,     //设置电池充（放）电MOS状态 
}BMS_CMD;

//电池状态常量
typedef enum {
    POWER_SUPPLY_STATUS_UNKNOWN = 0,   //未知
    POWER_SUPPLY_STATUS_CHARGING,      //充电
    POWER_SUPPLY_STATUS_DISCHARGING,   //放电
    POWER_SUPPLY_STATUS_NOT_CHARGING,  //不充电
    POWER_SUPPLY_STATUS_FULL,          //满电
}BMS_STATUS;

//电池健康常数
typedef enum {
    POWER_SUPPLY_HEALTH_UNKNOWN = 0,            //未知
    POWER_SUPPLY_HEALTH_GOOD,                   //良好
    POWER_SUPPLY_HEALTH_OVERHEAT,               //过热
    POWER_SUPPLY_HEALTH_DEAD,                   //死机
    POWER_SUPPLY_HEALTH_OVERVOLTAGE,            //过压
    POWER_SUPPLY_HEALTH_UNSPEC_FAILURE,         //非特指的错误
    POWER_SUPPLY_HEALTH_COLD,                   //过冷
    POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE,  //看门狗定时过期
    POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE,    //安全定时过期
}BMS_HEALTH;

//电池技术常数
typedef enum {
    POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0,  //未知技术
    POWER_SUPPLY_TECHNOLOGY_NIMH,         //镍氢电池
    POWER_SUPPLY_TECHNOLOGY_LION,         //锂离子电池
    POWER_SUPPLY_TECHNOLOGY_LIPO,         //锂聚合物电池
    POWER_SUPPLY_TECHNOLOGY_LIFE,         //锂铁电池
    POWER_SUPPLY_TECHNOLOGY_NICD,         //镍镉电池
    POWER_SUPPLY_TECHNOLOGY_LIMN,         //锂锰电池
}BMS_TECHNOLOGY;

//电池异常类型
#pragma pack(push,1)
typedef struct
{
    uint32_t bms_one_under_voltage:1;    //单体欠压
    uint32_t bms_one_over_voltage:1;     //单体过压
    uint32_t bms_under_voltag:1;         //总压过低
    uint32_t bms_over_voltage:1;         //总压过高
    uint32_t bms_discharge_current:1;    //放电过流
    uint32_t bms_charge_current:1;       //充电过流
    uint32_t bms_discharge_low_temp:1;   //放电低温
    uint32_t bms_discharge_high_temp:1;  //放电高温
    uint32_t bms_charge_low_temp:1;      //充电低温
    uint32_t bms_charge_high_temp:1;     //充电高温
    uint32_t bms_low_temp:1;             //环境低温
    uint32_t bms_high_temp:1;            //环境高温
    uint32_t bms_charge_voltage:1;       //充电过压
    uint32_t bms_short_circuit:1;        //短路
}BMS_ERROR;
#pragma pack(pop)

//电池实时参数
typedef struct 
{
    uint16_t bms_voltage;									//电池电压(单位:mV)
    uint16_t bms_temperature;							//当前温度(单位:℃)
    uint16_t bms_current;									//放电电流(单位:mA)
    uint16_t bms_charge;									//充电电流(单位:mA)
    uint16_t bms_capacity;								//满充容量(单位:mAh)
    uint16_t bms_design_capacity;					//设计容量(单位:mAh)
    uint16_t bms_percentage;							//电池百分比
    BMS_STATUS bms_status;								//电池状态
    BMS_HEALTH bms_health;								//电池健康状态
    BMS_TECHNOLOGY bms_technology;				//电池化学技术
    BMS_ERROR bms_error;									//电池错误信息
    int8_t bms_present;										//电池在位
} BMS_DATA;

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */

#endif
