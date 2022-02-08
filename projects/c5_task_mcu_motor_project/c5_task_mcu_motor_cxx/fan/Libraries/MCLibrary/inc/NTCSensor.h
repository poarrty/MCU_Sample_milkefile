/******************************************************************************
* File Name         :  NTCSensor.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  NTC检测程序                  
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
#ifndef __NTCSENSOR_H
#define __NTCSENSOR_H


#include "Type.h"
//#include "MCTasks.h"


typedef const struct
{
	uint16_t hLowPassFilterBW;        
	uint16_t hOverTempThreshold;       
	uint16_t hOverTempDeactThreshold; 
	int16_t hSensitivity; 
	uint16_t hTempADMask; 
	uint8_t bTempShift;
	uint16_t hV0; 
  //int32_t wV0;	
	uint16_t hT0;  
	
}NTCParams_t, *pNTCParams_t;



typedef struct
{
	uint16_t *pTempAD;         
	uint16_t hAvTemp;   
	uint8_t bSensorFault;

}NTCVars_t, *pNTCVars_t;



typedef struct
{
	NTCVars_t Vars;			/*!< Derived class members container */
	pNTCParams_t pParams;	/*!< Derived class parameters container */
}NTC_t, *pNTC_t;

extern pNTC_t oNTC[NBR_OF_MOTORS];
 

void NTC_Init(void);
int16_t NTC_GetAvTemp_C(pNTC_t this);
void NTC_CalcAvTemperature(pNTC_t this);


#endif /*__NTC_TEMPERATURESENSORCLASS_H*/
/**************************************************************END OF FILE****/
