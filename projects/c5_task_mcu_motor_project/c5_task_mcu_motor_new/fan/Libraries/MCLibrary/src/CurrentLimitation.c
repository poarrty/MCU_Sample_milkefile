/*******************************************************************************
* File Name         :  CurrentLimitation.c
* Author            :  陈雅枫
* Version           :  1.0
* Date              :  2020/2/18 16:5:12
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



/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"

#define DIFF_THRESHOLD 1536  //1525  1536(2A)  2304(3A)
#define  DefTemperatureMax  80  //降流温度设定最大值 STM32的内部温度传感器支持的温度范围为：-40~125度
#define  DefNtcCnt   2400 //高温限流时间(10min) 12000(50ms调用一次)  2400(2min)    1200(1min)       2400000(250us调用一次)


CRLM_t CRLMpool[NBR_OF_MOTORS];

CRLMParams_t CurrentLimitationParams1 =
{
    0,
	7600,		//12.5A 9600(联谊)   9100(nidec)
	9600,		//15A  11600(联谊)   11200(nidec)
	10451,		//20A  15451(联谊)   14560(nidec)
	7280,       //10A   高温限流值
	5460,       //7.5A   高温限流值
	3640,       //5A   高温限流值
	1,	//10min 12000(50ms调用一次)         2400000(250us调用一次)
	3600,	//3min  720000 (250us调用一次)
	100,		//5sec  20000 
};

#ifdef DOUBLE_DRIVER
CRLMParams_t CurrentLimitationParams2 =
{
     1,
	7600,		//12.5A 9600(联谊)   9100(nidec) 
	9600,		//15A  11600(联谊)   11200(nidec)
	10451,		//20A  15451(联谊)   14560(nidec)
	7280,       //10A   高温限流值
	5460,       //7.5A   高温限流值
	3640,       //5A   高温限流值
	1,	//10min 12000(50ms调用一次)         2400000(250us调用一次)     1(直接限流12.5A)
	3600,	//3min  720000 (250us调用一次)
	100,		//5sec  20000 
};
#endif

/*******************************************************************************
* Function Name  : CLM_Init
* Description    : 变量初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void CRLM_Init(void)
{
    oCRLM[0] = &CRLMpool[0];
	oCRLM[0]->pParams = &CurrentLimitationParams1;
	oCRLM[0]->Vars.hTotalIsRef = oCRLM[0]->pParams->hDefIsRefLevel1;// + oCRLM[1]->pParams->hDefIsRefLevel1;
	oCRLM[0]->Vars.hIsRef = oCRLM[0]->Vars.hTotalIsRef;
	oCRLM[0]->Vars.oPIIs = oPIDIs[0];

	#ifdef DOUBLE_DRIVER
    oCRLM[1] = &CRLMpool[1];    
	oCRLM[1]->pParams = &CurrentLimitationParams2;  
	oCRLM[1]->Vars.hTotalIsRef = oCRLM[0]->Vars.hTotalIsRef;
	oCRLM[1]->Vars.hIsRef =oCRLM[1]->Vars.hTotalIsRef;  //oCRLM[1]->pParams->hDefIsRefLevel1 (直接限流12.5A)  oCRLM[1]->Vars.hTotalIsRef	
	oCRLM[1]->Vars.oPIIs = oPIDIs[1];
	#endif

}

/*******************************************************************************
* Function Name  : CRLM_Start
* Description    : 给定电流初始化
* parameters     : this               
* Return         : void
********************************************************************************/
void CRLM_Start(pCRLM_t this)
{
	pCRLMVars_t pVars = &(this->Vars);
	pVars->hIsRef = pVars->hTotalIsRef;        
}

/*******************************************************************************
* Function Name  : CurrentLimitation
* Description    : 
* parameters     : this               
* Return         : 返回给定电流和反馈电流差
********************************************************************************/
int16_t CurrentLimitation(pCRLM_t this)
{
	pCRLMVars_t pVars = &(this->Vars);
	pCRLMParams_t pParams = this->pParams;

//	uint8_t bRate;  //uint8_t
//	uint8_t bLimitFlag;
	int16_t hError;
	int16_t hIsRegulatorOut;
//	int32_t wTemp;
//	uint8_t bLimitState3;
	

	if(pVars->hIs > pParams->hDefIsRefLevel3)
	{
		pVars->bLimitState3 = 1;// pVars->bLimitState3
	}
	
	else if(pVars->hIs < (pParams->hDefIsRefLevel3 - DIFF_THRESHOLD))//   13915
	{
		if(pParams->bMotor==1)
		{
		pVars->bLimitState3 = 0;
		pVars->hLimitCount3 = 0;
		}
		else
		{
		pVars->bLimitState3 = 0;
		pVars->hLimitCount3 = 0;		
		}
	}

	if(pVars->hIs > pParams->hDefIsRefLevel2)
	{
		pVars->bLimitState2 = 1;
	}
	else if(pVars->hIs < (pParams->hDefIsRefLevel2 - DIFF_THRESHOLD))
	{
		pVars->bLimitState2 = 0;
		pVars->hLimitCount2 = 0;
	}

	if(pVars->hIs > pParams->hDefIsRefLevel1)
	{
		pVars->bLimitState1 = 1;
	}
	else if(pVars->hIs < (pParams->hDefIsRefLevel1 - DIFF_THRESHOLD))
	{
		pVars->bLimitState1 = 0;
		pVars->hLimitCount1 = 0;
	}
/*
    if(hMCUTemp> DefTemperatureMax )  //高温降流
    {
       pVars->bLimitState4 = 4;
    }
 	else if(hMCUTemp < ( hMCUTemp- TEMP_THRESHOLD) )
	{
	   pVars->bLimitState4 = 0;
	}
*/
	if(++pVars->bBaseTim>2) //200 50ms (250us调用一次)     2(直接限流12.5A)  
	{

		pVars->bBaseTim = 0; 

		if(pVars->bLimitState1>0)
		{
			if(++pVars->hLimitCount1 > pParams->hDefLimitTim1)
			{
				pVars->hLimitCount1 = pParams->hDefLimitTim1+1;
				pVars->bLimitFlag = 1;
			}			
		}

		if(pVars->bLimitState2>0)
		{
			if(++pVars->hLimitCount2 > pParams->hDefLimitTim2)
			{
				pVars->hLimitCount2 = pParams->hDefLimitTim2+1;
				pVars->bLimitFlag = 1;
			}			
		}

		if(pVars->bLimitState3>0) //pVars->bLimitState3
		{
			if(++pVars->hLimitCount3 > pParams->hDefLimitTim3)
			{
				pVars->hLimitCount3 = pParams->hDefLimitTim3+1;
				pVars->bLimitFlag = 1;
			}			
		}
        /*
        if(pVars->bLimitState4>0)
        {
           pVars->bLimitFlag = 2;
        }
        */

		if( pVars->bLimitFlag == 1 )// ||  pVars->bLimitFlag == 2)   //单个电机电流不大于25A
		{
         // if(pVars->bLimitFlag ==1) //动态限流
         // {
			if(pVars->hIsRef > pParams->hDefIsRefLevel1)
			  pVars->hIsRef -=96; //从25A-12.5A需8s(60)    10s(48)  5s(96)
			else
			  {
			    pVars->hIsRef = pParams->hDefIsRefLevel1; 
				pVars->bLimitFlag = 0;
			  }
         // }
		  /*
          if(pVars->bLimitFlag ==2)//高温降流
          {
			if(pVars->hIsRef > pParams->hDefIsRefLevel4) //高温限流值  
			  pVars->hIsRef -=115; //从25A-10A需   5s(115)  降15A(11520)  
			else
			  {
			    pVars->hIsRef = pParams->hDefIsRefLevel4; 
				pVars->bLimitFlag = 0;
			  }
          }	*/	  
		}	
       
	  /* if( (pVars->hTotalIs >pVars->hTotalIsRef)  && (pVars->bLimitFlag != 1) )  //两路电机总电流必须小于25A            //&& (pVars->bLimitFlag != 1)          
	    {	 
	       wTemp = pVars->hTotalIsRef <<7;
	       pVars->bRate = (uint8_t)( wTemp / pVars->hTotalIs); //uint8_t	 
	       pVars->hIsRef =( pVars->hIs * pVars->bRate )>>7;	 
	    }*/		
 
/*
       /////////////////////////////高温降流////////////////////////////////////////////////
      
		if(hMCUTemp1 > DefTemperatureMax  )			
		{
		  if(pVars->hNtcStateFlag==0)
		  {		  
		    if( pVars->hIsRef > pParams->hDefIsRefLevel4) //10A  从25A-10A需   5s(115)  降15A(11520)
		     pVars->hIsRef -= 115; // 1
		    else 
		     {
		  	  pVars->hIsRef = pParams->hDefIsRefLevel4;
			   pVars->hNtcCntFlag=1;
		     }
		  }
		  else if(pVars->hNtcStateFlag==1)
		  {
		    if( pVars->hIsRef > pParams->hDefIsRefLevel5) //7.5A  从25A-7.5A需   5s(134)  降17.5A(13440)  降2.5A(1920)
		       pVars->hIsRef -= 19; //1
		    else 
		     {
		  	   pVars->hIsRef =  pParams->hDefIsRefLevel5;
			   pVars->hNtcCntFlag=1;
		     }			  
		  }
		  else if(pVars->hNtcStateFlag==2)
		  {
		    if( pVars->hIsRef > pParams->hDefIsRefLevel6) //5A  从25A-5A需   5s(153)  降20A(15360)  降2.5A(1920)
		       pVars->hIsRef -= 19; //1
		    else 
		     {
		  	   pVars->hIsRef =  pParams->hDefIsRefLevel6;
			   pVars->hNtcCntFlag=1;
		     }			  
		  }	
		  else
		  	 pVars->hNtcCntFlag=0;
		  
		  
		}

         if( pVars->hNtcCntFlag==1 )
         {
		   if (++pVars->hNtcCnt > DefNtcCnt)
		   {
			 if(hMCUTemp1 > DefTemperatureMax  )
             {
               pVars->hNtcStateFlag +=1;             			   
			   if(pVars->hNtcStateFlag >2) //2
			      pVars->hNtcStateFlag =2;//2			  
             }
			 
			 pVars->hNtcCntFlag=0;
			 pVars->hNtcCnt=0;
		   }
         }
*/      
	}


	hError = pVars->hIsRef - pVars->hIs;	

	hIsRegulatorOut = PI_Controller(pVars->oPIIs, (int32_t)hError); 

	return (hIsRegulatorOut);

  
}


/*******************************************************************************
* Function Name  : STC_CalcBusCurrent
* Description    : 读取总线电流
* parameters     : None  
* Return         : None
*******************************************************************************/
void CRLM_CalcBusCurrent(uint16_t hM1BusCurrentOffset, uint16_t hM2BusCurrentOffset)
{
	oCRLM[M1]->Vars.hIs = 0;//hADCAvgCurL - hM1BusCurrentOffset ;	
	oCRLM[M1]->Vars.hTotalIs = oCRLM[M1]->Vars.hIs + oCRLM[M2]->Vars.hIs;

	oCRLM[M2]->Vars.hIs = 0;//hADCAvgCurR - hM2BusCurrentOffset;
	oCRLM[M2]->Vars.hTotalIs = oCRLM[M1]->Vars.hTotalIs;
}



/**************************************************************END OF FILE****/
