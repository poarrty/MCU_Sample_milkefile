/********************************************************************************
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
#include "UserInclude.h"
#include "MCInclude.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LINEARHALLSENSOR_H
#define __LINEARHALLSENSOR_H


#define SPEEDNUM 512  //400



typedef __packed struct
{
	uint16_t HallFaultM1:2;//HallFaultM2(用整理前旧版本)
	uint16_t HallFaultM2:2;//HallFaultM1
	uint16_t HallStateM1:3;//HallStateM2
	uint16_t HallStateM2:3;//HallStateM1
	uint16_t SeqNum:6;		
}BlockVars_t;


typedef __packed union
{
   uint16_t HalfWord;
   BlockVars_t Block;   
}ComplexVars_t;

typedef const struct
{     
    int16_t hAngleShift;
	uint8_t bAngleIndex;
	uint8_t bAuxIndex;
	uint8_t bSpeedIndex;	
	uint8_t bMotor;  
	
} HALLParams_t, *pHALLParams_t;

typedef struct
{	
	int32_t wElSpeedSum;
    int16_t hElAngle;
	int16_t hElAngleBak;
	int16_t hElSpeedDpp;
	int16_t hElSpeedBuf[SPEEDNUM];
	uint16_t hElSpeedCnt;//int16_t
	int8_t  bSpeed;
	uint16_t hHallTime; 
	uint16_t hLastHallTime;
	uint16_t hHallATime;
	uint16_t hHallBTime;
	uint16_t hHallCTime;
	uint8_t bHallPreState;
	uint8_t bHallState;
	uint8_t bHallFault;
	uint8_t bSPIFla;
	uint8_t bHallInitFault;
	uint8_t bSPICommError;
	uint8_t bBlockageFault;
	uint8_t bHallTimeDivisor;
	int32_t wElSpeedAvrg;
	int16_t bHallSeqNum;
	int16_t hDiff;
	uint8_t bSPISeqNum;
    uint8_t bSPICommErrorCnt;
}HALLVars_t,*pHALLVars_t;


typedef struct
{
	HALLVars_t Vars;
	pHALLParams_t pParams;
}HALL_t, *pHALL_t;


extern pHALL_t oHall[NBR_OF_MOTORS];

void LINHALL_Init(void);
int16_t LINHALL_GetAvrgMecSpeed01Hz(pHALL_t this);
void LINHALL_HandleHallParam(pHALL_t this, FOCVars_t *Global);
int16_t LINHALL_GetElAngle(pHALL_t this);
void LINHALL_MeasureInit(pHALL_t this, int16_t hDesCmd);  



#endif




/**************************************************************END OF FILE****/
