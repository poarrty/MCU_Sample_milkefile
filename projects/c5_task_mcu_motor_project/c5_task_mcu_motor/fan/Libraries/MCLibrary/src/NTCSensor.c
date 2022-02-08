/******************************************************************************
* File Name         :  NTCSensor.c
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

/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"




NTCParams_t NTCParamsM1 =
{ 
	8,
	32767,
	32767,
	43,			//平均4.3mv /度  	* 10为拉伸
	33000,		//满载电压3300mV * 10为拉伸
	16,			//12位AD左对齐得到16位数据
	14300,		//25度标准电压 1430mV * 10为拉伸
	25,			//25度基准温度

};


#ifdef DOUBLE_DRIVER
NTCParams_t NTCParamsM2 =
{    
	/*
		8,
		32767,
		32767,
		1,
		0xFF00,
		16,
		1,
		1,
	*/
		8,
		32767,
		32767,
		43, 		//平均4.3mv /度		* 10为拉伸
		33000,		//满载电压3300mV * 10为拉伸
		16, 		//12位AD左对齐得到16位数据
		14300,		//25度标准电压 1430mV * 10为拉伸
		25, 		//25度基准温度


};
#endif

NTC_t NTCpool[NBR_OF_MOTORS];
pNTC_t oNTC[NBR_OF_MOTORS];



/*******************************************************************************
* Function Name  : NTC_Init
* Description    : NTC初始化程序
* parameters     : None
* Return         : None
*******************************************************************************/
//static void NTC_Init(void)
void NTC_Init(void)
{
    oNTC[0] = &NTCpool[0];
	oNTC[0]->pParams = &NTCParamsM1;
	oNTC[0]->Vars.pTempAD = &hRegConv[4];

    #ifdef DOUBLE_DRIVER
    oNTC[1] = &NTCpool[1];    
    oNTC[1]->pParams = &NTCParamsM2;  
	oNTC[1]->Vars.pTempAD = &hRegConv[4];
	#endif 
}

/*******************************************************************************
* Function Name  : NTC_Clear
* Description    : 
* parameters     : 
* Return         : None
*******************************************************************************/
//static void NTC_Clear(pNTC_t this)
//{
//	this->Vars.hAvTemp = 0u;
//}



/*******************************************************************************
* Function Name  : NTC_CalcAvTemperature
* Description    : 
* parameters     : 
* Return         : None
*******************************************************************************/
void NTC_CalcAvTemperature(pNTC_t this)
{
	int32_t wtemp;//uint32_t
	uint16_t hAux;
	pNTCVars_t pVars =&(this->Vars);
	pNTCParams_t pParams = this->pParams;   

	pVars->bSensorFault = 0;
    //pVars->hAvTemp = (uint16_t)((*(pVars->pTempAD) * pParams->hTempADMask)>>pParams->bTempShift);
	 	
	hAux = (uint16_t)((*(pVars->pTempAD) * pParams->hTempADMask)>>pParams->bTempShift);	

	wtemp = pParams->hV0 - hAux;// hV0 wV0
	
	hAux = wtemp/pParams->hSensitivity + pParams->hT0;//hAux  wtemp
	
	pVars->hAvTemp = hAux;
  /*
	if(hAux != 0xFFFFu)
	{
		wtemp =  (uint32_t)(pParams->hLowPassFilterBW)-1u;
		wtemp *= (uint32_t) (pVars->hAvTemp);
		wtemp += hAux;
		wtemp /= (uint32_t)(pParams->hLowPassFilterBW);
		pVars->hAvTemp = (uint16_t) wtemp;
		
	}
	else 
		pVars->bSensorFault = 1;
*/
}



/*******************************************************************************
* Function Name  : NTC_GetAvTemp_C
* Description    : 
* parameters     : 
* Return         : None
*******************************************************************************/
int16_t NTC_GetAvTemp_C(pNTC_t this)
{
	pNTCVars_t pVars =&(this->Vars);
	/*
	int32_t wTemp;
	
	pNTCParams_t pParams = this->pParams; 
	pVars->hAvTemp = (uint16_t)((*(pVars->pTempAD) * pParams->hTempADMask)>>pParams->bTempShift) ;

	wTemp = (int32_t)(pParams->wV0 - pVars->hAvTemp);
	//wTemp -= (int32_t)(pParams->wV0);
	wTemp = wTemp/pParams->hSensitivity + (int32_t)(pParams->hT0);*/

	return(pVars->hAvTemp);
	//return(wTemp);
}



/**************************************************************END OF FILE****/
