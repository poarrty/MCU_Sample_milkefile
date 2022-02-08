/******************************************************************************
* File Name         :  MTPAControl.c
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


/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"




#define MTPA_max   20000

#if (MTPA_ENABLING == ENABLE)


#define TEQUE_REF                   5000   
#define MAGNETIC_REF                2500//3000  1500


MTPA_t MTPApool[NBR_OF_MOTORS]; 

MTPAParams_t MTPAParamsM1 =
{
  (int16_t)TEQUE_REF,       
  (int16_t)MAGNETIC_REF,      
};

#ifdef DOUBLE_DRIVER
MTPAParams_t MTPAParamsM2 =
{
  (int16_t)TEQUE_REF,       
  (int16_t)MAGNETIC_REF,   
};
#endif


static int16_t MTPA(pMTPA_t this, int16_t hTeref);

/*******************************************************************************
* Function Name  : MTPA_Init
* Description    : 上电MTPA初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void MTPA_Init(void)
{
    oMTPA[0] = &MTPApool[0];
    oMTPA[0]->pParams = &MTPAParamsM1;
	oMTPA[0]->Vars.hTorqueCurr =0;
	oMTPA[0]->Vars.hIncCounter = 0;
	oMTPA[0]->Vars.hDecCounter = 0;


#ifdef DOUBLE_DRIVER
    oMTPA[1] = &MTPApool[1];
    oMTPA[1]->pParams = &MTPAParamsM2;	
	oMTPA[1]->Vars.hTorqueCurr =0;
	oMTPA[1]->Vars.hIncCounter = 0;
	oMTPA[1]->Vars.hDecCounter = 0;
#endif
		
	
}


/*******************************************************************************
* Function Name  : MTPA_Clear
* Description    : 参数清零
* parameters     : None
* Return         : None
*******************************************************************************/
void MTPA_Clear(pMTPA_t this)
{

	pMTPAVars_t pVars = &(this->Vars);
	
	pVars->hTorqueCurr =0;
	pVars->hIncCounter = 0;
	pVars->hDecCounter = 0;
	
}


/*******************************************************************************
* Function Name  : MTPA
* Description    : MTPA计算
* parameters     : IQ给定值
* Return         : ID给定值
*******************************************************************************/
#pragma inline
static int16_t MTPA(pMTPA_t this, int16_t hTeref)
{
	int16_t hIdRef;
	
	pMTPAParams_t pParams = this->pParams;

	hTeref = (hTeref < 0 ? (-hTeref) : (hTeref));   

	if(hTeref>pParams->hTeRef )
	{
		//hIdRef = pParams->hMagRef;
	//	hIdRef =hIdRef >>2;
		
		hIdRef = hTeref - pParams->hTeRef;		
		if(hIdRef > pParams->hMagRef)  hIdRef=  pParams->hMagRef ;
	}
	else //if(hTeref>(pParams->hTeRef-1000))
	{
		hIdRef  = 0;
	}

	return (hIdRef);   
}





/*******************************************************************************
* Function Name  : MTPA_CalcCurrRef
* Description    : IqdRef 计算
* parameters     : this MTPApool结构体指针、      Global FOCVars结构体指针、  hTeref==PreVQ
* Return         : None
*******************************************************************************/
void MTPA_CalcCurrRef(pMTPA_t this, FOCVars_t *Global)
{
	int16_t hMagRef;
		
	pMTPAVars_t pVars = &(this->Vars);
		
	pVars->hTorqueCurr=(Global->Iqd.qI_Component1+pVars->hTorqueCurr*127)>>7;  
	
//	if((Global-> Vqd.qV_Component1)>MTPA_max  || (Global-> Vqd.qV_Component1)<-MTPA_max) 
//	{hMagRef = MTPA(this, pVars->hTorqueCurr);}
//	else hMagRef =0;
	
	hMagRef = MTPA(this, pVars->hTorqueCurr);
	
		
	
	if(Global-> Vqd.qV_Component1<0) hMagRef=-hMagRef;// Vqd.qV_Component1(VQ<0)    Iqd.qI_Component1 (IQ<0)

	if(Global->Iqdref.qI_Component2<hMagRef)
	{
		if(++pVars->hIncCounter>200)
		{
			pVars->hIncCounter = 101;
			pVars->hDecCounter =0;
			Global->Iqdref.qI_Component2+=50;

		}
	}
	else if(Global->Iqdref.qI_Component2>hMagRef)
	{
		if(++pVars->hDecCounter >200)
		{
			pVars->hIncCounter = 0;
			pVars->hDecCounter = 101;
			Global->Iqdref.qI_Component2-=50;

			if(Global->Iqdref.qI_Component2<hMagRef)
				Global->Iqdref.qI_Component2 = hMagRef; 
		}
	}

//	Global->IqdHF.qI_Component1 = hIqRef;
	//Global->IqdHF.qI_Component2 = Global->Iqdref.qI_Component2;
 
}


#endif

/**************************************************************END OF FILE****/
