/******************************************************************************
* File Name         :  MTPAControl.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/12/14
* Description       :  针对内嵌式电机做最大扭矩处理                        
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/12/14      1.0               创建   
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
#ifndef __MTPACONTROL_H
#define __MTPACONTROL_H

/* Includes ------------------------------------------------------------------*/
//#include "Type.h"
//#include "MCTasks.h"
//#include "DeviceConfig.h"




typedef const struct
{
  int16_t  hTeRef;               
  int16_t  hMagRef;                     
} MTPAParams_t, *pMTPAParams_t;


typedef struct
{   
	int16_t hTorqueCurr;   
	uint16_t hIncCounter;
	uint16_t hDecCounter;
	
 }MTPAVars_t,*pMTPAVars_t;

typedef struct
{
	MTPAVars_t Vars;	
	pMTPAParams_t pParams;	
} MTPA_t, *pMTPA_t;


extern pMTPA_t oMTPA[NBR_OF_MOTORS];

void MTPA_Init(void);
void MTPA_Clear(pMTPA_t this);
void MTPA_CalcCurrRef(pMTPA_t this, FOCVars_t *Global);



#endif 

/**************************************************************END OF FILE****/
