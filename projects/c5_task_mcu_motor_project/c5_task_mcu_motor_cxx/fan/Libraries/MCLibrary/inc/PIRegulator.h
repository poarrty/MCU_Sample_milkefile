/******************************************************************************
* File Name         :  PIRegulator.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  PI调整器                       
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
#ifndef __PIREGULATOR_H
#define __PIREGULATOR_H

#include "Type.h"
//#include "Math.h"
//#include "MCTasks.h"



typedef const struct
{
    int16_t hDefKpGain;          
    int16_t hDefKiGain; 
	int16_t hDefKsGain;
    uint16_t hKpDivisor;           
    uint16_t hKiDivisor;          
    int32_t  wDefMaxIntegralTerm; 
    int32_t  wDefMinIntegralTerm;
    int16_t  hDefMaxOutput;      
    int16_t  hDefMinOutput;       
    uint16_t hKpDivisorPOW2;     
    uint16_t hKiDivisorPOW2;      
} PIParams_t, *pPIParams_t;


typedef struct
{
  int16_t       hKpGain;
  int16_t       hKiGain;
  int16_t       hKsGain;
  int32_t       wIntegralTerm;
  int32_t       wUpperIntegralLimit;
  int32_t       wLowerIntegralLimit;
  int16_t       hUpperOutputLimit;  
  int16_t       hLowerOutputLimit;    
  uint16_t      hKpDivisor;
  uint16_t      hKiDivisor;
  uint16_t      hKpDivisorPOW2;
  uint16_t      hKiDivisorPOW2;
}PIVars_t,*pPIVars_t;


typedef struct
{
    PIVars_t Vars;		
    pPIParams_t pParams;	
}PI_t, *pPI_t;




extern int16_t PI_Controller(pPI_t this, int32_t wProcessVarError);
void PI_SetIntegralTerm(pPI_t this, int32_t wIntegralTermValue);
void PI_Init(void);




extern pPI_t oPIDSpeed[NBR_OF_MOTORS];
extern pPI_t oPIDIq[NBR_OF_MOTORS];
extern pPI_t oPIDId[NBR_OF_MOTORS];
extern pPI_t oPIDIs[NBR_OF_MOTORS];
extern pPI_t oPIDVs[NBR_OF_MOTORS];
extern pPI_t oPIAcc[NBR_OF_MOTORS];
extern pPI_t oPINTc[NBR_OF_MOTORS];




#endif /*__PID_PICLASS_H*/

/**************************************************************END OF FILE****/
