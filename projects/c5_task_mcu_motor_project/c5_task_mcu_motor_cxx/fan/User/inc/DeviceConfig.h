/******************************************************************************
* File Name         :  DeviceConfig.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  器件参数配置                         
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
#ifndef __DEVICECONFIG_H
#define __DEVICECONFIG_H


#define SYSCLK_FREQ_72MHz  120000000uL  // 72000000uL
#define ADV_TIM_CLK_MHz     120uL  // 72uL

#define PWM_FREQUENCY       16000uL  // 16000uL

#define PWM_PERIOD_CYCLES 	(uint16_t)((ADV_TIM_CLK_MHz * 1000000uL) / (PWM_FREQUENCY))
#define PWM_PERIOD  		(uint16_t)(PWM_PERIOD_CYCLES / 2u)

#define REGULATION_EXECUTION_RATE 1     //2//2
#define REP_COUNTER 		(uint16_t) ((REGULATION_EXECUTION_RATE *2u)-1u)

#define DEADTIME_NS  2500u
#define TIM_CLOCK_DIVIDER  1
#define DEAD_TIME_COUNTS  (uint16_t)(DEAD_TIME_ADV_TIM_CLK_MHz * DEADTIME_NS/1000uL)
#define DEAD_TIME_ADV_TIM_CLK_MHz  (ADV_TIM_CLK_MHz*TIM_CLOCK_DIVIDER)

#define TNOISE_NS                    3000 //3000//2550
#define TRISE_NS                      2550 // 2550

#if (TNOISE_NS > TRISE_NS)
  #define MAX_TNTR_NS TNOISE_NS
#else
  #define MAX_TNTR_NS TRISE_NS
#endif

#define CURR_SAMPLING_TIME            1
#define ADC_CLK_MHz        12uL
#define SAMPLING_TIME_NS (((CURR_SAMPLING_TIME) * 1000uL/ADC_CLK_MHz)+(7000uL/(2*ADC_CLK_MHz)))
#define SAMPLING_TIME (uint16_t)(((uint16_t)(SAMPLING_TIME_NS) * ADV_TIM_CLK_MHz)/1000uL)
#define MAX_TWAIT2 (uint16_t)((TW_AFTER - SAMPLING_TIME)/2)

#define TW_AFTER ((uint16_t)(((DEADTIME_NS+MAX_TNTR_NS)*64ul)/1000ul))
#define TW_BEFORE (((uint16_t)(((((uint16_t)(SAMPLING_TIME_NS)))*64ul)/1000ul))+1)
#define TW_TOTAL  ((uint16_t)(((DEADTIME_NS+MAX_TNTR_NS+SAMPLING_TIME_NS)*64ul)/1000ul))


#endif 

/**************************************************************END OF FILE****/
