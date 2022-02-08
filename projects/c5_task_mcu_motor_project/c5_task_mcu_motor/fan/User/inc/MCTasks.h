/******************************************************************************
* File Name         :  MCTasks.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  电控驱动的任务调用                       
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
#ifndef __MCTASKS_H
#define __MCTASKS_H

/* Includes ------------------------------------------------------------------*/
#include "MCInclude.h"



#define GPO_MagneticBrake_Release()     GPIO_SetBits(GPIOC, GPIO_Pin_12)
#define GPO_MagneticBrake_Locking()     GPIO_ResetBits(GPIOC, GPIO_Pin_12)


#define GPI_MagneticBrakeStaM1() 	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)
#define GPI_MagneticBrakeStaM2() 	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)
#define GPI_ChargeCheck() 			GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)

//#define GPI_PowerEnable() 	        GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)


extern FOCVars_t FOCVars[NBR_OF_MOTORS];
extern  uint8_t bMCPwmPeriodCompleted;


void MCboot(void);
void MC_Relay(void);


void MC_Scheduler(void);
void TSK_SafetyTask(void);
void TSK_HighFrequencyTask(void);
void TSK_DualDriveUpdate(uint8_t oDrive);
void TSK_HardwareFaultTask(void);
uint8_t TSK_GetDriveIndex(void);

void FOC_CurrController(uint8_t bMotor);

void MediumFrequencyTask(void);
void LowFrequencyTask(void);
void TB_Init(void);
void GlobalVariable_Init(void);

#endif /* __MCTASKS_H */


/**************************************************************END OF FILE****/
