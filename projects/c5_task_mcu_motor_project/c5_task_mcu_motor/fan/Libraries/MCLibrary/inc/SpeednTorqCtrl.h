/******************************************************************************
* File Name         :  SpeednTorqCtrl.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  速度及转矩控制函数                        
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
#ifndef __SPEEDNTORQCTRL_H
#define __SPEEDNTORQCTRL_H

/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"


typedef const struct
{
	uint8_t bMotor;
	uint16_t hMaxMecSpeed01Hz;
	uint16_t hMinMecSpeed01Hz;
	uint16_t hMaxTorque;
	int16_t  hMinTorque;
//	uint16_t hPowerDownVolt;
	int16_t hIsRefDefault;
//	uint16_t hVsRefDefault;
  
}STCParams_t, *pSTCParams_t;


typedef struct
{		
	int16_t hSpeedRef01HzExt;
	int16_t hPreSpeedRef01HzExt;
	int16_t hTorqueRef;
	pPI_t oPISpeed;
//	pPI_t oPISpeedLike;
	pPI_t oPIDIq;
    pPI_t oPIIs;
    pPI_t oPIVs;
//    pPI_t oPIAcc;//190713
    pPI_t oPINTc;
//	pPI_t oPIOVs;
    int16_t hVqBuf[64];//190713
//  	pHALL_t oHall;
  	pPOS_t oPosSen;
	pPWMC_t oPwmc;
	pCRLM_t oCRLM;
	pNTC_t  oNTC;
    int16_t hTorqueRefBuf[24];
    uint8_t bTorqueIndex;
    uint8_t bVqIndex;//190713
    int32_t wVqSum;//190713
    int16_t hVqPre;//190713
    int16_t hVqBak;//190713
    int16_t hAccRegulatorOut;//190713
    int16_t hIsRegulatorOut;//190713
    int16_t hVsRegulatorOut;//190713
    int16_t hTmpRegulatorOut;
	int16_t hOvsRegulatorOut;
    int16_t hRefError;
    int32_t wTorqueSum;
    uint8_t bPowerDownFault;
    uint8_t bTaskIndex;//190713
    uint8_t bRegulatorLimit;
    uint16_t hIsRef;
    uint16_t hIs;
//    int16_t hVs;
//    uint16_t hVsRef;
    uint8_t bSvpwmInitFlag;
	uint8_t bDirectionFlag;	
	uint32_t hNtcCnt;
	uint8_t hNtcCntFlag;
	uint8_t hNtcStateFlag;
	uint8_t hOvFlag;
    
}STCVars_t,*pSTCVars_t;



typedef struct
{
	STCVars_t Vars;			/*!< Derived class members container */
	pSTCParams_t pParams;	/*!< Derived class parameters container */
}STC_t, *pSTC_t;


void STC_Clear(pSTC_t this);
void STC_Init(void);


extern pSTC_t oSTC[NBR_OF_MOTORS];
 


int16_t STC_CalcTorqueReference(pSTC_t this, FOCVars_t *Global);



#endif /* __SPEEDNTORQCTRLCLASS_H */

/**************************************************************END OF FILE****/
