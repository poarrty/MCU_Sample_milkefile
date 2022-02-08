/*******************************************************************************
* File Name         :  CurrentLimitation.h
* Author            :  陈雅枫
* Version           :  1.0
* Date              :  2020/2/18 16:9:40
* Description       :  限制功率输出
********************************************************************************
* History:
* <author>        <time>        <version >        <desc>
* 陈雅枫            2020/2/18         1.0            创建
*
*
*
*
*
*
*
*
*
******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CURRENTLIMITATION_H
#define __CURRENTLIMITATION_H

/* Includes ------------------------------------------------------------------*/
//#include "UserInclude.h"   
//#include "MCInclude.h" 
#include "Type.h"
#include "PIRegulator.h"

typedef const struct
{
    uint8_t bMotor;
	int16_t hDefIsRefLevel1;
	int16_t hDefIsRefLevel2;
	int16_t hDefIsRefLevel3;
	int16_t hDefIsRefLevel4;
	int16_t hDefIsRefLevel5;
	int16_t hDefIsRefLevel6;
	uint16_t hDefLimitTim1;
	uint16_t hDefLimitTim2;
	uint16_t hDefLimitTim3;
	
} CRLMParams_t, *pCRLMParams_t;

  
typedef struct
{		
	uint8_t bLimitState;
	uint8_t bLimitState1;
	uint8_t bLimitState2;
	uint8_t bLimitState3;
	uint8_t bLimitState4;
	uint8_t bLimitFlag;
	uint8_t bRate;
    int16_t hIsRef;
    int16_t hIs;
    int16_t hTotalIs;
    uint8_t bBaseTim;
	uint16_t hLimitCount1;
	uint16_t hLimitCount2;
	uint16_t hLimitCount3; 
	uint16_t hLimitCount4;

	int16_t hTotalIsRef;
	int16_t hIsRefCnt;
	pPI_t oPIIs;

	uint32_t hNtcCnt;
	uint8_t hNtcCntFlag;
	uint8_t hNtcStateFlag;
	
}CRLMVars_t,*pCRLMVars_t;


typedef struct
{
	CRLMVars_t Vars;	
  	pCRLMParams_t pParams;	
	
} CRLM_t, *pCRLM_t;


extern pCRLM_t oCRLM[NBR_OF_MOTORS];


void CRLM_Init(void);
void CRLM_Start(pCRLM_t this);

void CRLM_CalcBusCurrent(uint16_t hM1BusCurrentOffset, uint16_t hM2BusCurrentOffset);

int16_t CurrentLimitation(pCRLM_t this);


#endif 

/**************************************************************END OF FILE****/

