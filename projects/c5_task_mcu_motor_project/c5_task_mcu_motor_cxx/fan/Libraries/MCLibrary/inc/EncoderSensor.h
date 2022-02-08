/******************************************************************************
* File Name         :  EncoderSensor.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  20/09/09
* Description       :  编码器处理函数                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫          20/09/09      1.0               创建   
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
#ifndef __ENCODERSENSOR_H
#define __ENCODERSENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "Type.h"


#define GPIO_NoRemap_TIMx ((uint32_t)(0))
#define ENC_IC_FILTER 9




typedef const struct
{     
    TIM_TypeDef* TIMx;      

    uint32_t DBGMCU_TIMx; 
    
    uint32_t RCC_APB1Periph_TIMx;    
    
    uint8_t TIMx_IRQChannel;      

    uint8_t EnZ_IRQChannel;   

    uint32_t EnZ_Linex;
    
    uint8_t EnZ_PortSourceGPIOx;

    uint8_t  EnZ_PinSourcex;
   
    uint32_t wTIMxRemapping; 
    
    GPIO_TypeDef* hEnAPort; 
    
    uint16_t hEnAPin;      
    
    GPIO_TypeDef* hEnBPort; 
    
    uint16_t hEnBPin;      
    
    GPIO_TypeDef* hEnZPort; 
    
    uint16_t hEnZPin;  

    GPIO_TypeDef* hPWMPort; 
    
    uint16_t hPWMPin; 

	uint16_t hPulsesNum;   

    uint8_t bPoleNum; 
		
		uint16_t hPwmPeriod;
		
		uint16_t hPwmPeriod1;

} ENCParams_t, *pENCParams_t;

typedef struct
{
	int16_t hPulsesCnt;//uint16_t
	uint16_t hEncoderTime;
	uint16_t hLastEncoderTime;  //前一个hall 更新时间
	uint16_t hBlockageTimer;
	int16_t hPhaseShift;
	int16_t hDeltaElAngle;//int16_t
	int16_t hMeasuredElAngle;//int16_t
	int16_t hElAngle;//int16_t
	int16_t hAdjustAngle;
	uint8_t bHallAdjustCnt;
	int16_t hPreMeasuredElAngle;//int16_t
	int16_t hElSpeedCnt;
	int32_t wElSpeedSum;
	int16_t hElSpeedDpp;
	int16_t hElSpeedBuf[256];
	int16_t hInc60Angle;
	int32_t wInc360Angle;
	uint8_t bBlockageFault;
	uint8_t bInitFault;
	uint8_t bEncoderFault;
	uint8_t bFaultState;
	uint8_t bEncoderAState;
	uint8_t bEncoderBState;
	uint8_t bEncoderZState;
	uint8_t bEncoderALevel;
	uint8_t bEncoderBLevel;
	uint8_t bInitFlag;
	int8_t bPositive;
	int8_t bNegative;
	int8_t bPWMInputCapFlag;
	//uint16_t hPulseRiseCnt;
	//uint16_t hPulseFallCnt;
	///uint16_t hAngelInit;
	//uint16_t hPulseFallCnt1;
  uint16_t hZCnt;
  //uint16_t hPeriod;
	//uint16_t hPulseRiseCnt1;
	//int16_t  hHighCnt;
  //int16_t  hLowCnt;
  //uint8_t ICPolarity;
  //uint8_t bCounter;	
	uint8_t bPWM_CapSta;
	int32_t hPWM_CapVal;
	int32_t hCapLowValEnd;
	int32_t hCapLowValStart;
	int32_t hCapLowVal;
	uint8_t bCapLowCount;
	int32_t hCapMax;
	int32_t hCapSum;
	int32_t hCapMin;
	int32_t hCapPeriodValEnd;
	int32_t hCapPeriodValStart;
	int32_t hCapPeriodVal;
	int32_t hPeriodMax;
	int32_t hPeriodSum;
	int32_t hPeriodMin;	
	int32_t hHighPulsesCnt;
	int32_t hPeriodPulsesCnt;
	int32_t hPWMEndCnt;
  //int32_t hPWMStartCnt;	
  int32_t hPWMDataCnt;	
	int32_t hPWMData_low_Cnt;	
	uint8_t bPWMStartFlag;
	uint8_t bZ_flag;
	uint16_t hZ_StateCnt;
  int16_t  hRunCnt;
	int16_t  hPreCnt;
	int16_t  hDletaCnt;
	uint16_t  hTIMErrCnt;
	int16_t  hDletaErrCnt;
	uint8_t  bEncoderDletaCntFault;
	int16_t  hSpdFeedBack;
	uint8_t bPWMStartErr;
	uint16_t hEncoderA0StateCnt;
	uint16_t hEncoderB0StateCnt;
	uint8_t bEncoderA0Fault;
	uint8_t bEncoderB0Fault;
	uint16_t hEncoderA1StateCnt;
	uint16_t hEncoderB1StateCnt;
	uint8_t bEncoderA1Fault;
	uint8_t bEncoderB1Fault;	
	uint16_t hEncoderZCnt;
	uint8_t bEncoderZFault;
	uint32_t wCapLowLevelVal[6];
	uint32_t wCapPeriodVal[6];

}ENCVars_t,*pENCVars_t;



typedef struct
{
	ENCVars_t Vars;
	pENCParams_t pParams;
}ENC_t, *pENC_t;


extern pENC_t oEncoder[NBR_OF_MOTORS];



void ENC_Init(void);
void ENC_CalcElAngle(pENC_t this, FOCVars_t *Global);
int16_t ENC_GetElAngle(pENC_t this);
void ENC_MeasureInit(pENC_t this , int16_t hDesCmd);
extern int16_t ENC_GetAvrgMecSpeed01Hz(pENC_t this);
extern uint16_t ENC_GetENCTime(pENC_t this);








#endif

/**************************************************************END OF FILE****/
