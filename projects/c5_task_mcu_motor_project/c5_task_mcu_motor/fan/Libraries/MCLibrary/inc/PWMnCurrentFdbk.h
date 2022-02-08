/******************************************************************************
* File Name         :  PWMnCurrentFdbk.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  PWM和电流采样处理                        
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
#ifndef __PWMNCURRENTFDBK_H
#define __PWMNCURRENTFDBK_H

#include "Type.h"


#define CCMR_A_PWM (BIT6|BIT5|BIT3)
#define CCMR_B_PWM (BIT14|BIT13|BIT11)
#define CCMR_C_PWM (BIT6|BIT5|BIT3)
#define CCMR_A_HIGHSIDE (BIT6|BIT4|BIT3)
#define CCMR_A_LOWSIDE  (BIT6|BIT3)
#define CCMR_B_HIGHSIDE (BIT14|BIT12|BIT11)
#define CCMR_B_LOWSIDE  (BIT14|BIT11)
#define CCMR_C_HIGHSIDE (BIT6|BIT4|BIT3)
#define CCMR_C_LOWSIDE  (BIT6|BIT3)


#ifdef LowEffect
#define PWM_POL_LS_A BIT3  //分离器件上桥高有效,下桥低有效
#define PWM_POL_HS_A 0//BIT1
#define PWM_POL_LS_B BIT7
#define PWM_POL_HS_B 0//BIT5
#define PWM_POL_LS_C BIT11 
#define PWM_POL_HS_C 0//BIT9
#else
#define PWM_POL_LS_A 0  //集成IC上桥高有效,下桥高有效
#define PWM_POL_HS_A 0//BIT1
#define PWM_POL_LS_B 0
#define PWM_POL_HS_B 0//BIT5
#define PWM_POL_LS_C 0 
#define PWM_POL_HS_C 0//BIT9
#endif
#define PWM_POL_A PWM_POL_LS_A|PWM_POL_HS_A
#define PWM_POL_B PWM_POL_LS_B|PWM_POL_HS_B
#define PWM_POL_C PWM_POL_LS_C|PWM_POL_HS_C

#define A_ON_HS  (BIT0|PWM_POL_A) // PWM mode on High Side
#define A_ON_LS  (BIT2|PWM_POL_A) // PWM mode on Low Side
#define A_COMP (BIT2|BIT0|PWM_POL_A)
#define A_OFF PWM_POL_A

#define B_ON_HS (BIT4|PWM_POL_B)	// PWM mode on High Side
#define B_ON_LS (BIT6|PWM_POL_B) // PWM mode on Low Side
#define B_COMP (BIT6|BIT4|PWM_POL_B)
#define B_OFF PWM_POL_B

#define C_ON_HS (BIT8|PWM_POL_C) // PWM mode on High Side
#define C_ON_LS (BIT10|PWM_POL_C) // PWM mode on Low Side
#define C_COMP (BIT8|BIT10|PWM_POL_C)
#define C_OFF (PWM_POL_C)

#define CCMR_D_PWM (BIT14|BIT13|BIT12|BIT11)
#define D_ON BIT12
//#define D_ON (BIT13|BIT12)//190719//修改CC4极性

//#define hADCAvgCurL   		hRegConv[0]
//#define hADCAvgCurR   		hRegConv[1]
#define hADCBusVolt 		  hRegConv[0]
#define hADCBatVolt 		  hRegConv[1]
//#define hM1Temp 		      hRegConv[5]
//#define hM2Temp 		      hRegConv[6]

//#define hADCBusVolt 		  hRegConv[2]
//#define hADCBatVolt 		  hRegConv[3]
typedef const struct 
{                 
	uint16_t CCMR_1; //Phase A /Phase B
	uint16_t CCMR_2; //Phase C	
	uint16_t CCER;	
} Phase_Step_t;


typedef const struct
{
  uint8_t bInstanceNbr;            
  uint8_t bIaChannel;                  
  GPIO_TypeDef* hIaPort;               
  uint16_t hIaPin;                 
  uint8_t bIbChannel;                  
  GPIO_TypeDef* hIbPort;                
  uint16_t hIbPin;                   
  uint8_t bIcChannel;                   
  GPIO_TypeDef* hIcPort;                
  uint16_t hIcPin;          
  TIM_TypeDef*  TIMx;                  
  GPIO_TypeDef* hCh1Port;               
  uint16_t hCh1Pin;                   
  GPIO_TypeDef* hCh2Port;              
  uint16_t hCh2Pin;                    
  GPIO_TypeDef* hCh3Port;              
  uint16_t hCh3Pin;                   
  GPIO_TypeDef* hCh1NPort;               
  uint16_t hCh1NPin;                   
  GPIO_TypeDef* hCh2NPort;             
  uint16_t hCh2NPin;                   
  GPIO_TypeDef* hCh3NPort;              
  uint16_t hCh3NPin;                  
  GPIO_TypeDef* hBKINPort;               
  uint16_t hBKINPin;   
  uint8_t bIbusChannel;   
  uint8_t TIMx_UP_IRQChannel; 
  uint8_t TIMx_BRK_IRQChannel; 
}PWMCParams_t, *pPWMCParams_t;


typedef struct
{
	uint16_t hPhaseAOffset;  
	uint16_t hPhaseBOffset;   
	uint16_t hPhaseCOffset;  
	uint16_t hMaxPhaseOffset;
	uint16_t hMinPhaseOffset;
	uint16_t hBusCurrentOffset;

	uint8_t bPhaseOffsetFault;
	uint8_t bBusCurrentOffsetFault;
	uint8_t bCalibrateCompletionFlag;  
	uint8_t bCalibrateOverTimeFlag; 
	uint8_t bSectorSwitchFlag;
	uint8_t bSamplingInvaildFlag;
	uint8_t bCheckMosfetFlag;
	uint8_t bOverCurrentFault;      //纭欢姣嶇嚎杩囨祦
	uint8_t bOverCurrentFault1;     //杞欢锛宨q杩囨祦   
	uint8_t bMosfetHFault;
	uint8_t bMosfetLFault;
	uint8_t bSector;
	uint16_t hLastDuty;
	uint8_t bLastSector;

	uint8_t bPhase1AdChannel;
	uint8_t bPhase2AdChannel;
	uint16_t Half_PWMPeriod;        
	volatile uint8_t bSoFOC;  
	volatile uint8_t  bIndex;

	int8_t bCompensationTime;   
	int8_t bCompensationCounter;   
	int16_t hCompensationTheta;   
	int16_t hCompensationTime0;   
	int16_t hCompensationTime1;   
	int16_t hCompensationTime2;   
	
	uint32_t wADCTriggerSet; 
	uint32_t wADCTriggerUnSet;
	uint32_t wTIMxCH4_BB_Addr;

	uint32_t wPrevCurrentComponent1;
	uint32_t wPrevCurrentComponent2;
	Curr_Components Last_Stator_Currents;
 }PWMCVars_t,*pPWMCVars_t;


typedef struct
{
   PWMCVars_t Vars;		/*!< Derived class members container */
   pPWMCParams_t pParams;	/*!< Derived class parameters container */      
}PWMC_t, *pPWMC_t;



void PWMC_Init(void);
void PWMC_AD_RESTARTInit(void);
Curr_Components PWMC_GetPhaseCurrents(pPWMC_t this);
void PWMC_SetPhaseVoltage(FOCVars_t* Global,Volt_Components Stat_Volt_Input, pPWMC_t this);
void PWMC_CheckMosfets(pPWMC_t this);
void PWMC_CurrentCalibration(pPWMC_t this);
void PWMC_SetInjectedConvTrig(pPWMC_t this, FunctionalState cmd);
void PWMC_PhaseAdcChannelConfig(pPWMC_t this);
void PWMC_Clear(pPWMC_t this);

#ifdef DEADBAND_OFFSET_EN
void PWMC_CalcCompensationTime(Volt_Components Stat_Volt_q_d, pPWMC_t this);
#endif

extern void STC_CalcBusCurrent(void);
extern void STC_CalcBusVoltage(void);


//extern GlobalVars_t GlobalM1,GlobalM2;
extern pPWMC_t oPwmc[NBR_OF_MOTORS];


extern uint16_t hRegConv[3];  


#endif 

/**************************************************************END OF FILE****/
