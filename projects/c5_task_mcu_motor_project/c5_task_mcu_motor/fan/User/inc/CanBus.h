/******************************************************************************
* File Name         :  CanBus.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  Can总线处理程序                      
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/01/14      1.0               创建   
* 陈雅枫             20/02/07      1.1               修改代码格式
* 
*
* 
* 
*
* 
*
*
*
*
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CANBUS_H
#define __CANBUS_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Type.h"
#include "UartComm.h"
#include "RC4.h"


//PC通信命令定义	
#define CMD_CONNET_TEST					(0x00)
#define CMD_DISCONNECT					(0x01)
#define CMD_USER_PARA_WRITE				(0x02)
#define	CMD_USER_PARA_READ				(0x03)
#define CMD_PROJ_PARA_WRITE				(0x04)
#define	CMD_PROj_PARA_READ				(0x05)
#define CMD_REMOTE_SIME					(0x06)
#define CMD_STAR_REALTIME				(0x07)
#define CMD_STOP_REALTIME				(0x08)

//DSP通信命令定义
#define DSP_CONNET_TEST					(0x01)
#define DSP_CONTROL_PARA_CMD			(0x02)
#define DSP_MOTOR_PARA_SET				(0x03)
#define	DSP_USER_PARA_SET				(0x04)
#define	DSP_LIGHT_CONTROL				(0x05)
#define DSP_GET_RADAR_INFO				(0x06)
#define DSP_GET_ELECTRONIC_INFO			(0x07)
#define DSP_UPATE_MOTOR_CMD				(0x08)
#define DSP_GET_ROBOT			        (0x09)

#define CAN_Baudrate_Prescaler   12 // 1M : 6  500K : 12 (外部时钟72M 或120M)  内部时钟(64M) 500K : 8

/* 28 | 27 | 26 | 25 | 24 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |*/

/*| 发送设备号                      |    接收设备号                    |  命令字节定义                     |END |   消息当前包序号                             |*/
//设备定义
#define CANDRV_CONTROL_ID               ((uint32_t)0x01)
#define CANDRV_MOTOR_ID                 ((uint32_t)0x0)
#define CANDRV_BAT_ID                   ((uint32_t)0x03)
#define CANDRV_BLUETOOTH_ID             ((uint32_t)0x04)
#define CANDRV_LAMP_ID                  ((uint32_t)0x05)
#define CANDRV_UPDATE_ID                ((uint32_t)0x06)            //升级设备高26字节

#define CAN_SEND_BITPOS     (23)
#define CAN_RECV_BITPOS     (17)
#define CAN_CMD_BITPOS      (11)
#define CAN_END_BITPOS      (10)            //多帧数据包结束标志位

#define CAN_FILTER_ID       ((unsigned)((CANDRV_MOTOR_ID << CAN_RECV_BITPOS)))
#define CAN_MASK_ID         ((unsigned)(((unsigned)0x3F << CAN_RECV_BITPOS)))
#define CAN_SEND_ID         ((unsigned)((CANDRV_MOTOR_ID << CAN_SEND_BITPOS)|( CANDRV_CONTROL_ID << CAN_RECV_BITPOS)))


typedef struct
{
    uint32_t wFuncID;
    uint16_t hDataCount;
    uint8_t *pdata;
	uint8_t curMsgLen;
}CANProtocol_t;  

typedef __packed struct//连接命令参数定义
{
	uint8_t bproduction;
	uint8_t bHWvserion[3];
	uint8_t bSWvserion[3];

}CANVersion_t;

typedef __packed struct  //摇杆参数类型定义
{
	int16_t  hSpeedM1;	//左电机转速  uint16_t
	int16_t  hSpeedM2;	//右电机转速 uint16_t
	
}ControlCmd_t;


typedef __packed struct
{
	uint16_t b0:1;
	uint16_t b1:1;
	uint16_t b2:1;
	uint16_t b3:1;
	uint16_t b4:1;
	uint16_t b5:1;
	uint16_t b6:1;
	uint16_t b7:1;
	uint16_t b8:1;
	uint16_t b9:1;	
	uint16_t b10:1;
	uint16_t b11:1;
	uint16_t b12:1;
	uint16_t b13:1;
	uint16_t b14:1;
	uint16_t b15:1;
}Bit_t;


typedef __packed union
{
   uint16_t halfword;
   Bit_t bit;   
}Flag_t;

typedef __packed struct
{
	uint16_t left_motor_cur:8;
	uint16_t right_motor_cur:8;
}CUR_LR_DEF;
typedef __packed union{
   uint16_t CUR;
   CUR_LR_DEF bit;
}CUR_DEF; //反馈左右电机电流

typedef __packed struct
{
	uint16_t hLeftHall:16;
	uint16_t hRightHall:16;
}HALL_LR_DEF;//反馈左右电机霍尔计数值


typedef __packed struct //驱动板返回的电机信息类型定义
{
	Flag_t hDriverFlag;
	int16_t hSpeedM1; 
	int16_t hSpeedM2;
	int8_t bData1;
	int8_t bData2;
}DriverMsg_t;



typedef __packed struct  //电机参数类型定义
{
	//DO0;
	uint16_t DIS;//电磁阀开关
	uint16_t SLT;//休眠时间
	uint16_t PHD;//电机相序
	uint16_t MOP;//电机极对数
	//DT0
	uint16_t MMS;//电机最高转速
	uint16_t LMD;//左电机旋转方向
	uint16_t RMD;//右电机旋转方向
	uint16_t MMC;//电机最大电流

	uint16_t LMC;//左电机补偿
	uint16_t RMC;//右电机补偿
	
}MotorParam_t;



typedef __packed struct  //用户参数类型定义
{
	uint16_t FSH;//前进速度最大值
	uint16_t FSL;//前进速度最小值
	uint16_t FAH;//前进加速度最大值
	uint16_t FAL;//前进加速度最小值
	uint16_t FDH;//前进减速度最大值
	uint16_t FDL;//前进减速带最小值

	uint16_t BSH;//后退速度最大值
	uint16_t BSL;//后退速度最小值
	uint16_t BAH;//后退加速度最大值
	uint16_t BAL;//后退加速度最小值
	uint16_t BDH;//后退减速度最大值
	uint16_t BDL;//后退减速带最小值

	uint16_t TSH;//转弯速度最大值
	uint16_t TSL;//转弯速度最小值
	uint16_t TAH;//转弯加速度最大值
	uint16_t TAL;//转弯加速度最小值
	uint16_t TDH;//转弯减速度最大值
	uint16_t TDL;//转弯减速带最小值
	
}UserParam_t;



extern ControlCmd_t ControlCommand;
extern uint8_t  bCanCommFault;
extern uint8_t  bCanMagneticBrakeRelease;
extern uint8_t  bCanMagneticBrakeStaM1;
extern uint8_t  bCanMagneticBrakeStaM2;



void CAN_Config(void);
void CAN_CommunicationTask(void);
void CAN_CommFaultCheck(void);



#endif 

/**************************************************************END OF FILE****/
