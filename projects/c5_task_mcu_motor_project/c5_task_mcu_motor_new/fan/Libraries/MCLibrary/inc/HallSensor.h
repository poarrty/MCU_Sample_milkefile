/******************************************************************************
* File Name         :  HallSensor.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  霍尔及电角度处理                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/01/14      1.0               创建   
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
*
*******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HALLSENSOR_H
#define __HALLSENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "Type.h"


#define GPIO_NoRemap_TIMx ((uint32_t)(0))

/* HALL SENSORS PLACEMENT ----------------------------------------------------*/
#define	DEGREES_120 0u
#define	DEGREES_60 1u


#define SENSORPLACEMENT  DEGREES_120

#define SIX_STEP_START
#define HALL_IC_FILTER 11
#define HALL_MAX_RATIO  15
#define NEGATIVE          (s8)-1
#define POSITIVE          (s8)1
#define NEGATIVE_SWAP     (s8)-2
#define POSITIVE_SWAP     (s8)2
#define HALL_ERROR        (s8)127


#define TF_REGULATION_RATE  ((uint16_t)(PWM_FREQUENCY/REGULATION_EXECUTION_RATE))
#define	ROTOR_SPEED_FACTOR  ((u32)(SYSCLK_FREQ_72MHz/TF_REGULATION_RATE))
#define PSEUDO_FREQ_CONV    ((u32)(ROTOR_SPEED_FACTOR/HALL_MAX_RATIO))
#define HALL_TIM_PERIOD     ((int16_t)(SYSCLK_FREQ_72MHz/TF_REGULATION_RATE))
#define HALL_PWM_RATIO    	((int16_t)(HALL_TIM_PERIOD/HALL_MAX_RATIO))


typedef struct {
    uint32_t wPeriod; 
    int8_t bDirection;
} PeriodMeas_s;


typedef const struct
{     
    TIM_TypeDef* TIMx;      

    uint32_t DBGMCU_TIMx; 
    
    uint32_t RCC_APB1Periph_TIMx;    
    
    uint8_t TIMx_IRQChannel;      
    
    uint32_t wTIMxRemapping; 
    
    GPIO_TypeDef* hH1Port; 
    
    uint16_t hH1Pin;      
    
    GPIO_TypeDef* hH2Port; 
    
    uint16_t hH2Pin;      
    
    GPIO_TypeDef* hH3Port; 
    
    uint16_t hH3Pin;        
	
	uint32_t wPseudoFreqConv;
} HALLParams_t, *pHALLParams_t;

typedef struct
{

    volatile uint16_t hOVFCounter;      	//TIM 溢出计数器 将16位 TIM 扩大为 32位计数
                
    volatile uint32_t wSensorPeriod[8]; 	//Hall 周期BUF
                
    volatile uint32_t wSensorCapture;		//Hall 更新周期

    volatile uint32_t wSensorCaptureSum;	//一个电周期Hall捕获值
                 
	volatile uint16_t hRemainderElAngle;    //剩余角度

    uint8_t bBlockageFault;	//堵转标志
     
    int8_t bSpeed;   //电机方向

    uint16_t hLastHallTime;  //前一个hall 更新时间

    uint16_t hHallTime; //当前hall 更新时间

    uint32_t hHallTimeSum;  // 一个电周期hall 更新时间

    uint8_t bHallPortState; // 未经过滤波的hall状态

    uint8_t bHallFaultCnt; //hall故障标志
    uint8_t bHallFault; //hall故障标志

    uint8_t bHallNewState; 	// 识别到的新hall状态

    volatile uint8_t bHallRunState;  // 目前驱动电机的hall状态

    uint8_t bHallPositiveState;	// hall顺时针的预测状态

    uint8_t bHallNegativeState;	// hall逆时针的预测状态

    uint8_t bHallLastState;	// 前一个hall状态

    int8_t bPositive;	// hall正方向值

    uint8_t bHallInitFault;	// hall初始化检测故障

    int8_t bNegative;
		
	 int8_t bDirectionSwap;

	 int8_t bSixStepMode;		

    uint8_t bHallAdjustCnt;//hall角度差值矫正计数//190710
    int16_t hHallAdjustAngle;//hall角度矫正差值//190710

    uint16_t hHallATime;

    uint16_t hHallBTime;

    uint16_t hHallCTime;

    uint8_t bHallTimeDivisor;

    uint8_t bCaptureFlag;
    uint8_t bCaptureHall;//190709
    uint8_t bNoteJumpHallVal;//190709
    int8_t bHallRunDirCnt;//190709
    int8_t bHallRunDirFlag;//190709 //电机霍尔方向标志，1正向 -1反向
    uint8_t bHallMotorDirFlag;//190709 //电机正反码标志，0正码 1反码

    uint8_t bUpdateHallFlag;

    volatile uint8_t bPositiveCounter;

    volatile uint8_t bNegativeCounter;

    volatile int32_t wDeltaElAngle;
	
	int16_t hMeasuredElAngle;

    int16_t hElAngle;
//    int16_t hElAngle2;//190712

    int16_t hElSpeedDpp;

    int16_t hPhaseShift;
    int16_t hTheoryAngleShift;//190710

    int16_t hAvrgElSpeed;
	int16_t hAvrgElSpeedBak;
	int16_t hAvrgHallTimeSum;//190809
	uint8_t bFlag;
	int16_t hDiff;

	uint8_t bHallKeepTime1;
	uint8_t bHallKeepTime2;
	uint8_t bHallKeepTime3;
   
}HALLVars_t,*pHALLVars_t;



typedef struct
{
	HALLVars_t Vars;
	pHALLParams_t pParams;
}HALL_t, *pHALL_t;


extern pHALL_t oHall[NBR_OF_MOTORS];



void HALL_Init(void);
void HALL_CalcElAngle(pHALL_t this, FOCVars_t *Global);
int16_t HALL_GetElAngle(pHALL_t this);
void HALL_MeasureInit(pHALL_t this , int16_t hDesCmd);
extern int16_t HALL_GetAvrgMecSpeed01Hz(pHALL_t this);
extern uint16_t HALL_GetHallTime(pHALL_t this);
//extern void HALL_ElAngleInit(pHALL_t this); 

int16_t Find_TheoryAngleShift(int16_t hAngleShift);//190710







#endif

/**************************************************************END OF FILE****/
