/******************************************************************************
* File Name         :  EncoderSensor.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  20/09/09
* Description       :  编码器处理函数                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫          20/09/09      1.0               创建   
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


#ifdef ENCODER_SENSOR  

//#define  ENC_ANGLECALC



#define ENC_PHASE_SHIFT_M1 (int16_t)(18500)// -200  12000(开环) 14310(速度环) 编码器采用MT6825  15000(N2,N12清0,无须0点校正)  3150(加入0点校正,2分频,没清0,N0)  -500(加入0点校正,2分频)(N2,N12清0)    -950(加入0点校正,2分频,没清0,N10)
#define ENC_PHASE_SHIFT_M2 (int16_t)(-16200)//-200 12000(开环) 14310(速度环)       -15000(清0)    -18000(N2,N12清0)  -30000(加入0点校正,2分频,没清0,N0)   32700 -950(加入0点校正,2分频,没清0,NN1)

#define S16_60_PHASE_SHIFT  (int16_t)(65536/6)

//#define ENC_PWM_CALC_TEMP (uint16_t)(4096/25)

#define ENC_PWM_CALC_TEMP (uint16_t)(256/25)

#define LOCKED_TIME         ((uint16_t)46000) //编码器电机 堵转时间 

/* Constants ------------------------------------------------------------------*/

ENCParams_t ENCParamsM1 =
{
  TIM2,
  DBGMCU_TIM2_STOP,
  RCC_APB1Periph_TIM2,
  TIM2_IRQn,
  EXTI15_10_IRQn,//EXTI3_IRQn
  EXTI_Line13,// EXTI_Line3
  GPIO_PortSourceGPIOC,// GPIO_PortSourceGPIOA
  GPIO_PinSource13,// GPIO_PinSource3
  GPIO_FullRemap_TIM2,
  GPIOA,                         
  GPIO_Pin_15,         
  GPIOB,                        
  GPIO_Pin_3,    
  GPIOC, //GPIOA
  GPIO_Pin_13, // GPIO_Pin_3
  GPIOB,                        
  GPIO_Pin_10,    
  16384,//4095(1024线4分频)     16383(4096线4分频) 
  20, //25
	12500,//M1  12625 12500
	12935,//12625(M1)
};

#ifdef DOUBLE_DRIVER
ENCParams_t ENCParamsM2 =
{
  TIM4,
  DBGMCU_TIM4_STOP,
  RCC_APB1Periph_TIM4,
  TIM4_IRQn,
  EXTI2_IRQn,//EXTI2_IRQn
  EXTI_Line2,
  GPIO_PortSourceGPIOD,// GPIO_PortSourceGPIOC
  GPIO_PinSource2,
  GPIO_NoRemap_TIMx, 
  GPIOB, 
  GPIO_Pin_6,           
  GPIOB, 
  GPIO_Pin_7,   
  GPIOD, // GPIOC
  GPIO_Pin_2,  
  GPIOB, 
  GPIO_Pin_8,  
  16384, //4095(1024线4分频)     16383(4096线4分频)
  20, //25
	12500,//M2 12945 12800
	12625,//12935(M2)
};
#endif

#define NEGATIVE          (s8)-1
#define POSITIVE          (s8)1


#define POLE_PAIR_NUM		((u16) 25u) //极对数
#define REDUCTION_RATIO		((u16) 1u)	//减速比

#define MEC_RATIO  POLE_PAIR_NUM  


/* Variables ------------------------------------------------------------------*/

ENC_t ENCpool[NBR_OF_MOTORS]; 





/* declaration ------------------------------------------------------------------*/
static void ENC_Config(pENC_t this);
void ENC_ElAngleInit(pENC_t this) ;
extern FOCVars_t FOCVars[NBR_OF_MOTORS];


/*******************************************************************************
* Function Name  : ENC_MeasureInit
* Description    : 变量初始化 用于电机启动
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
void ENC_MeasureInit(pENC_t this , int16_t hDesCmd)
{
    pENCVars_t pVars =&(this->Vars);
    pENCParams_t pParams = this->pParams;     

    uint16_t i;

    pVars->wElSpeedSum = 0;
    pVars->hElSpeedCnt = 0;

	for(i=0;i<256;i++)
    	pVars->hElSpeedBuf[i] = 0;
   

   // ENC_ElAngleInit(this);
}


/*******************************************************************************
* Function Name  : ENC_ElAngleInit
* Description    : 变量初始化 用于电机启动
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/

extern void DelayUs(u16 i);
void ENC_ElAngleInit(pENC_t this) 
{
	
  	int32_t  wCnt;
	int32_t  hAuxTmp;
	
	uint16_t hCapCnt;  //elfern

	pENCVars_t pVars = &(this->Vars);
	pENCParams_t pParams = this->pParams;  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef	TIMx_ICInitStructure;
	NVIC_InitTypeDef  	NVIC_ENCInitStructure;

	int16_t hAux;
	int16_t hAux1;
	
	//初始化定时器 	 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  //0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler =2;	//63  0 1  7  2
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(pParams->TIMx, &TIM_TimeBaseStructure); 
  
	//初始化TIM输入捕获参数
	TIMx_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIMx_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获  TIM_ICPolarity_Rising  TIM_ICPolarity_BothEdge
	TIMx_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIMx_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIMx_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(pParams->TIMx, &TIMx_ICInitStructure);

//if(pParams->TIMx==TIM2)
//{	
	//中断分组初始化
	NVIC_ENCInitStructure.NVIC_IRQChannel = pParams->TIMx_IRQChannel;  //中断
	NVIC_ENCInitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_ENCInitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级0级
	NVIC_ENCInitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能  ENABLE
	NVIC_Init(&NVIC_ENCInitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
//}	
	TIM_ITConfig(pParams->TIMx,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
	TIM_Cmd(pParams->TIMx,ENABLE );	//使能定时器5
 
 
  DelayUs(50000);
/*	
while ( (pVars->bPWMInputCapFlag==0)&&(pVars->bInitFault==0) )
{
	if(TIM_GetFlagStatus(pParams->TIMx, TIM_FLAG_CC3) == SET)
	{			

		if(pVars->ICPolarity==0)
		{
			pVars->hPulseRiseCnt =TIM_GetCapture3(pParams->TIMx);		//记录第一次上升沿的CNT值
			TIM_OC3PolarityConfig(pParams->TIMx,TIM_ICPolarity_Falling); // 设置为下降沿捕获
			pVars->ICPolarity = 1;
		}
		else if(pVars->ICPolarity==1)
		{
			pVars->hPulseFallCnt =TIM_GetCapture3(pParams->TIMx);		//记录第一次上升沿的CNT值
			pVars->ICPolarity = 2;
			TIM_SetCounter(pParams->TIMx,(u16)(0)); 
			
		}		
		else if(pVars->ICPolarity==2)
		{
			pVars->hPulseFallCnt1 =TIM_GetCapture3(pParams->TIMx);		//记录第一次上升沿的CNT值
			TIM_OC3PolarityConfig(pParams->TIMx,TIM_ICPolarity_Rising); 			
      pVars->ICPolarity = 3;
		}
		else
		{
			pVars->hPulseRiseCnt1 =TIM_GetCapture3(pParams->TIMx);		//记录第一次上升沿的CNT值		
			TIM_SetCounter(pParams->TIMx,(u16)(0)); 	
			pVars->bPWMInputCapFlag = 1;
			TIM_ITConfig(pParams->TIMx,TIM_IT_Update|TIM_IT_CC3,DISABLE);//关中断			
			
		}

		TIM_ClearFlag(pParams->TIMx, TIM_FLAG_CC3);	//清除中断标志位 			
		
	}
	else if(TIM_GetFlagStatus(pParams->TIMx, TIM_FLAG_Update) == SET)
	{
		TIM_ClearFlag(pParams->TIMx, TIM_FLAG_Update);
		if(++pVars->bCounter>3)
		{
			pVars->bCounter = 0;
			pVars->bInitFault = 1;
			pVars->bFaultState |= 0x08;			
			TIM_ITConfig(pParams->TIMx,TIM_IT_Update|TIM_IT_CC3,DISABLE);//关中断
		}

	}
}

	 
	hAux = pVars->hPulseFallCnt - pVars->hPulseRiseCnt ;//高电平占空比
  pVars->hHighCnt= hAux;

	hAux1 = pVars->hPulseRiseCnt1 - pVars->hPulseFallCnt1 ;//低电平占空比
  pVars->hLowCnt = hAux1;

  //if( (hAux + hAux1)>12800 && (hAux + hAux1)<13280) //正常周期范围内  400-415
  if( (hAux + hAux1)>pParams->hPwmPeriod && (hAux + hAux1)<13280)  //正常周期范围内  与电机一致性有关,要具体调试 12500
	{
		 hAuxTmp= hAux<<12;
		 pVars->hPulsesCnt= (s16)(hAuxTmp/(hAux + hAux1));
	}
	else //异常直接清0,表明已在零点附近
	{
		if(hAux>pParams->hPwmPeriod && hAux1>pParams->hPwmPeriod ) // 高电平占空比> 低电平占空比, 取高电平占空比 12500 12800 
		{ 
			if(hAux>hAux1)	//高电平占空比> 低电平占空比, 低电平占空比有效,说明零点在0-40范围内  正常应该是高电平占空比< 低电平占空比
      {				
		    hAuxTmp= hAux1<<12;
		    pVars->hPulsesCnt= (s16)(hAuxTmp/(pParams->hPwmPeriod1)); // hAux + hAux1 12500 12800  pParams->hPwmPeriod
				//pVars->hPulsesCnt=pVars->hPulsesCnt-50;
				pVars->hPulsesCnt = 4095 -  pVars->hPulsesCnt;
			}
			else //高电平占空比< 低电平占空比, 高电平占空比有效,说明零点在4060-4095范围内 高电平占空比> 低电平占空比
      {				
		    hAuxTmp= hAux<<12;
		    pVars->hPulsesCnt= (s16)(hAuxTmp/(pParams->hPwmPeriod1)); // hAux + hAux1  pParams->hPwmPeriod
				//pVars->hPulsesCnt=pVars->hPulsesCnt-50;
			}	   

		}
	
	}
*/
//	pVars->hMeasuredElAngle = hAux*ENC_PWM_CALC_TEMP;
//	pVars->hElAngle = pVars->hMeasuredElAngle + pVars->hPhaseShift;
	//pVars->hPulsesCnt = (hAux*256)/25;
	//pVars->hAngelInit = (hAux*256)/25;
	
	//hAuxTmp= hAux<<12;
	
	
  // pVars->hPulsesCnt=  hAuxTmp /( pVars->hPulseFallCnt1 - pVars->hPulseRiseCnt);		

  while ( pVars->bPWMInputCapFlag==1 )	//计算电机初始位置角度
  {
	 if( pVars->bCapLowCount >= 12)   //读PWM引脚线坏了或没有接，程序不能跑，需要报警提示，待增加
		{
			
			for(hCapCnt=0;hCapCnt<5;hCapCnt++) 
		  { 
//				if(pParams->TIMx==TIM2) //elfern
//				{
//				  if(CAP_LOW_VAL_ARR[hCapCnt+1]>pVars->hCAPMax) 
//				  {
//					  pVars->hCAPMax=CAP_LOW_VAL_ARR[hCapCnt+1];
//				   }
//				   pVars->hCAPSUM+=CAP_LOW_VAL_ARR[hCapCnt+1];
//					 
//				  if(CAP_PEROID_VAL_ARR[hCapCnt+1]>pVars->hPEROIDMax)
//				  {
//					  pVars->hPEROIDMax=CAP_PEROID_VAL_ARR[hCapCnt+1];
//				   }
//				   pVars->hPEROIDSUM +=CAP_PEROID_VAL_ARR[hCapCnt+1];					 
//					 
//				 }
//				else
//				 {
//				   if(CAP_LOW_VAL2_ARR[hCapCnt+1]>pVars->hCAPMax)
//				   {
//					   pVars->hCAPMax=CAP_LOW_VAL2_ARR[hCapCnt+1];
//				   }
//				    pVars->hCAPSUM +=CAP_LOW_VAL2_ARR[hCapCnt+1];		

//				  if(CAP_PEROID_VAL2_ARR[hCapCnt+1]>pVars->hPEROIDMax)
//				  {
//					  pVars->hPEROIDMax=CAP_PEROID_VAL2_ARR[hCapCnt+1];
//				   }
//				   pVars->hPEROIDSUM +=CAP_PEROID_VAL2_ARR[hCapCnt+1];		
//					 
//				 }

				  if(pVars->wCapLowLevelVal[hCapCnt+1]>pVars->hCapMax) 
				  {
					  pVars->hCapMax=pVars->wCapLowLevelVal[hCapCnt+1];
				   }
				   pVars->hCapSum+=pVars->wCapLowLevelVal[hCapCnt+1];
					 
				  if(pVars->wCapPeriodVal[hCapCnt+1]>pVars->hPeriodMax)
				  {
					  pVars->hPeriodMax=pVars->wCapPeriodVal[hCapCnt+1];
				   }
				   pVars->hPeriodSum +=pVars->wCapPeriodVal[hCapCnt+1];					 
					 
			}
			pVars->hCapMin=pVars->hCapMax; //获取占空比最大值
	    	pVars->hPeriodMin=pVars->hPeriodMax;//获取周期最大值
			
			for(hCapCnt=0;hCapCnt<5;hCapCnt++)  //获取最小值
		  {
//				if(pParams->TIMx==TIM2) //elfern
//				{
//				  if(CAP_LOW_VAL_ARR[hCapCnt+1]<pVars->hCAPMin)
//				  {
//					  pVars->hCAPMin=CAP_LOW_VAL_ARR[hCapCnt+1];
//				  }
//				  if(CAP_PEROID_VAL_ARR[hCapCnt+1]<pVars->hPEROIDMin)
//				  {
//					  pVars->hPEROIDMin=CAP_PEROID_VAL_ARR[hCapCnt+1];
//				  }
//				}
//				else
//				{
//				  if(CAP_LOW_VAL2_ARR[hCapCnt+1]<pVars->hCAPMin)
//				  {
//					  pVars->hCAPMin=CAP_LOW_VAL2_ARR[hCapCnt+1];
//				  }
//				  if(CAP_PEROID_VAL2_ARR[hCapCnt+1]<pVars->hPEROIDMin)
//				  {
//					  pVars->hPEROIDMin=CAP_PEROID_VAL2_ARR[hCapCnt+1];
//				  }
//				}		

				if(pVars->wCapLowLevelVal[hCapCnt+1]<pVars->hCapMin)
				  {
					  pVars->hCapMin=pVars->wCapLowLevelVal[hCapCnt+1];
				  }
				  if(pVars->wCapPeriodVal[hCapCnt+1]<pVars->hPeriodMin)
				  {
					  pVars->hPeriodMin=pVars->wCapPeriodVal[hCapCnt+1];
				  }
			}
			
			pVars->hHighPulsesCnt=(pVars->hCapSum-pVars->hCapMax-pVars->hCapMin)/3;		//实际对应高电平占的时间,H
		   	pVars->hPeriodPulsesCnt =(pVars->hPeriodSum - pVars->hPeriodMax-pVars->hPeriodMin)/3;	//全周期时间，T   
	
       		pVars->hPWMEndCnt	=		( pVars->hPeriodPulsesCnt * 8 ) /4119 ; // 帧尾长度
			pVars->hPWMDataCnt = (pVars->hPeriodPulsesCnt * 4095)/4119; // 4095对应的长度
			pVars->hPWMData_low_Cnt = pVars->hPeriodPulsesCnt - pVars->hHighPulsesCnt -  pVars->hPWMEndCnt; // 低信号占的长度，T-H-帧尾
			
			pVars->hPulsesCnt=  (pVars->hPWMData_low_Cnt * (int32_t)16384) /pVars->hPWMDataCnt;	//	电机初始位置角度	 调好零点(N2, N12)
//			if(	pParams->TIMx==TIM4)
//			  {pVars->hPulsesCnt=  ( (  pVars->hPWMData_low_Cnt -( pVars->hPeriodPulsesCnt - 21623 -pVars->hPWMEndCnt)  ) * (int32_t)16384) /pVars->hPWMDataCnt;}  //电角度0点对应的PWM值   21623(加入0点校正,必须要在全周期值内调0,2分频,N0,没有清0)  21291(加入0点校正,必须要在全周期值内调0,2分频,NN1,没有清0)
//			else
//				{pVars->hPulsesCnt=  ( (  pVars->hPWMData_low_Cnt -( pVars->hPeriodPulsesCnt - 21399 -pVars->hPWMEndCnt)  ) * (int32_t)16384) /pVars->hPWMDataCnt;}  //电角度0点对应的PWM值      342(加入0点校正,必须要在全周期值内调0,2分频,N2,N12,已经清0)  21399(NN1,没有清0 )  21763(N10,没有清0 )
				//{pVars->hPulsesCnt=  (pVars->hPWMData_low_Cnt * (int32_t)16384) /pVars->hPWMDataCnt;} //调好零点(N2, N12)
				
			// pVars->hPulsesCnt=  (  ( pVars->hPWMDataCnt -pVars->hPWMData_low_Cnt) * (int32_t)16384) /pVars->hPWMDataCnt;
			
/*
			if((pVars->hHighPulsesCnt >= 全周期值的1.5倍) || (pVars->hPeriodPulsesCnt >= 全周期值的1.5倍) )  //全周期值的1.5倍 防止错位
			{
				pVars->bPWMStartErr=1; 
			}
      else
      {
       pVars->hPWMEndCnt	=		( pVars->hPeriodPulsesCnt * 8 ) /4119 ; // 帧尾长度
			 pVars->hPWMDataCnt = (pVars->hPeriodPulsesCnt * 4095)/4119; // 4095对应的长度
			 pVars->hPWMData_low_Cnt = pVars->hPeriodPulsesCnt - pVars->hHighPulsesCnt -  pVars->hPWMEndCnt; // 低信号占的长度，T-H-帧尾
			
			 pVars->hPulsesCnt=  (pVars->hPWMData_low_Cnt * (int32_t)16384) /pVars->hPWMDataCnt;	//	电机初始位置角度	 调好零点(N2, N12)			
			}			
*/			
			pVars->bCapLowCount=0;
			
			pVars->bPWMStartFlag=1; //正常为1, 若为0则PWM信号异常或断线 编码器故障

    }
			
        pVars->bPWMInputCapFlag=0;
  }

    if(pVars->bPWMStartFlag==0) 
		 {pVars->bPWMStartErr=1;} //正常为0, 若为1则PWM信号异常或断线 编码器故障
	
}

/*******************************************************************************
* Function Name  : HALL_Init
* Description    : 上电HALL初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void ENC_Init(void)
{
    oEncoder[0] = &ENCpool[0];   
    oEncoder[0]->pParams = &ENCParamsM1;
    oEncoder[0]->Vars.bPositive = POSITIVE;
    oEncoder[0]->Vars.bNegative = NEGATIVE;
    oEncoder[0]->Vars.hPhaseShift = ENC_PHASE_SHIFT_M1;
    oEncoder[0]->Vars.bInitFlag = 0;
    ENC_Config(oEncoder[0]);
    oEncoder[0]->Vars.bInitFlag = 1;
    
	#ifdef DOUBLE_DRIVER
	oEncoder[1] = &ENCpool[1];
	oEncoder[1]->pParams = &ENCParamsM2;
    oEncoder[1]->Vars.bPositive = POSITIVE;
    oEncoder[1]->Vars.bNegative = NEGATIVE;
    oEncoder[1]->Vars.hPhaseShift = ENC_PHASE_SHIFT_M2;
    oEncoder[1]->Vars.bInitFlag = 0;    
    ENC_Config(oEncoder[1]);     
    oEncoder[1]->Vars.bInitFlag = 1;
    #endif

}

/*******************************************************************************
* Function Name  : HALL_Config
* Description    : HALL相关IC外设模块初始化
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
static void ENC_Config(pENC_t this)
{
    TIM_TimeBaseInitTypeDef TIM_ENCTimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ENCICInitStructure;
    NVIC_InitTypeDef NVIC_ENCInitStructure;
    GPIO_InitTypeDef GPIO_ENCInitStructure;
    EXTI_InitTypeDef EXTI_ENCInitStructure;
    
	pENCVars_t pVars = &(this->Vars);
    pENCParams_t pParams = this->pParams;    
    TIM_TypeDef* TIMx = pParams->TIMx;  
    

    /* HW Init */  
    
    /* TIMx clock enable */  
    RCC_APB1PeriphClockCmd(pParams->RCC_APB1Periph_TIMx, ENABLE);
    /* TIM2 Counter Clock stopped when the core is halted */
    DBGMCU_Config(pParams->DBGMCU_TIMx , ENABLE);    

    GPIO_PinRemapConfig(pParams->wTIMxRemapping, ENABLE);  

     if ((TIMx == TIM2) && ((pParams->wTIMxRemapping == GPIO_PartialRemap1_TIM2) 
        || (pParams->wTIMxRemapping == GPIO_FullRemap_TIM2)))
    {
      /* Only PA15 and PB3 are released JTAG-DP Disabled and SW-DP Enabled */
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
      /* To disable all JTAG/SWD pins Full SWJ Disabled (JTAG-DP + SW-DP) uncomment line below */
      /*GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);*/
    }
   

    GPIO_StructInit(&GPIO_ENCInitStructure);
    /* Configure Hall sensors H1 input */
    GPIO_ENCInitStructure.GPIO_Pin = pParams->hEnAPin;
    GPIO_ENCInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hEnAPort , &GPIO_ENCInitStructure);

    /* Configure Hall sensors H2 input */
    GPIO_ENCInitStructure.GPIO_Pin = pParams->hEnBPin;
    GPIO_ENCInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hEnBPort, &GPIO_ENCInitStructure);

    /* Configure Hall sensors H3 input */
    GPIO_ENCInitStructure.GPIO_Pin = pParams->hEnZPin;
    GPIO_ENCInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hEnZPort, &GPIO_ENCInitStructure);

    /* Configure Hall sensors PWM input */
    GPIO_ENCInitStructure.GPIO_Pin = pParams->hPWMPin;
    GPIO_ENCInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU  GPIO_Mode_IN_FLOATING
    GPIO_Init(pParams->hPWMPort, &GPIO_ENCInitStructure);		
		
	//if(TIMx==TIM2)
	ENC_ElAngleInit(this);
//	pVars->bPWMInputCapFlag = 0;
//    ENC_PWMCheck(this); 

   // while(pVars->bPWMInputCapFlag!=1);
//   return;
    

    /* Timer configuration in Clear on capture mode */
    TIM_DeInit(TIMx);

    TIM_TimeBaseStructInit(&TIM_ENCTimeBaseInitStructure);
    /* Set full 16-bit working range */
    TIM_ENCTimeBaseInitStructure.TIM_Prescaler = 0;//0
    TIM_ENCTimeBaseInitStructure.TIM_Period = pParams->hPulsesNum -1;//pParams->hPulsesNum  4095
    TIM_ENCTimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_ENCTimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx,&TIM_ENCTimeBaseInitStructure);

    TIM_EncoderInterfaceConfig(TIMx, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ICStructInit(&TIM_ENCICInitStructure);
    TIM_ENCICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
    TIM_ENCICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ENCICInitStructure.TIM_ICFilter = ENC_IC_FILTER;

    TIM_ICInit(TIMx,&TIM_ENCICInitStructure);

    TIM_ClearFlag(TIMx, TIM_FLAG_Update);

    TIM_SetCounter(pParams->TIMx, pVars->hPulsesCnt); //  0
      // TIM_SetCounter(pParams->TIMx, 0);

    /* Enable the TIMx IRQChannel*/
//    NVIC_ENCInitStructure.NVIC_IRQChannel =  pParams->TIMx_IRQChannel;  
//    NVIC_ENCInitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
//    NVIC_ENCInitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
//    NVIC_ENCInitStructure.NVIC_IRQChannelCmd = ENABLE;

//    NVIC_Init(&NVIC_ENCInitStructure);


    NVIC_ENCInitStructure.NVIC_IRQChannel = pParams->EnZ_IRQChannel;
	NVIC_ENCInitStructure.NVIC_IRQChannelPreemptionPriority= 0;
	NVIC_ENCInitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_ENCInitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_ENCInitStructure);

	GPIO_EXTILineConfig(pParams->EnZ_PortSourceGPIOx,pParams->EnZ_PinSourcex);


	EXTI_ENCInitStructure.EXTI_Line = pParams->EnZ_Linex;
	EXTI_ENCInitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_ENCInitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_ENCInitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_ENCInitStructure);

	TIM_Cmd(TIMx, ENABLE);

	
    

}

/*******************************************************************************
* Function Name  : ENC_CalcElAngle
* Description    : 通过编码器信号量计算出对应的电角速度和电角度
* parameters     : this HALLpool结构体指针      Global FOCVars结构体指针 
* Return         : None
*******************************************************************************/
uint16_t hTIM4Cnt;
uint16_t hTIM2Cnt;
extern DriverMsg_t 	DriverMsg;

void ENC_CalcElAngle(pENC_t this, FOCVars_t *Global)
{
    pENCVars_t pVars = &(this->Vars);
	pENCParams_t pParams = this->pParams;
	TIM_TypeDef* TIMx = pParams->TIMx;
	
	int32_t hAngleTemp;//int16_t
	int16_t hAux;
	int8_t bPort;
	int16_t hPreAux;
	int32_t   hAngle_S32_Temp;
   int16_t  hHalfPulsesNum;	
	uint16_t  tmp_test;
	
	if(pVars->bInitFlag ==0)
		return;

	
	hAux = TIM_GetCounter(TIMx);//(清0)
//	 if(  pVars->hZ_StateCnt>=2 )  //4(不清0) 2(清0)
//		{ hAux = TIM_GetCounter(TIMx)+ pVars->hZCnt;}//加入Z脉冲清0的偏置角 
//	 else
//		{hAux = TIM_GetCounter(TIMx);} 	
			
	//hAux = TIM_GetCounter(TIMx) + pVars->hPulsesCnt;
	
//	if(hAux>=16384)
//		hAux -= 16384;
//	if(hAux<=-16384)
//			hAux += 16384;
	
//	if((hAux - hPreAux) > 8192)
//	{
//		hAux = hAux - 16384;//  4096
//	}
//	else if((hAux - hPreAux) < -8192)
//	{
//		
//		hAux = hAux +16384; // 4096
//	}

//	hPreAux = hAux;
	
//	if(TIMx==TIM4)   //elfern 
//	 {
//	   hTIM4Cnt= TIM_GetCounter(TIMx);
//		 pVars->hRunCnt = hTIM4Cnt;
//		 pVars->hSpdFeedBack = -ENC_GetAvrgMecSpeed01Hz(oEncoder[M2]);
//	 }

//	if(TIMx==TIM2)
//	 { 
//	   hTIM2Cnt= TIM_GetCounter(TIMx);
//		 pVars->hRunCnt = hTIM2Cnt;
//		 pVars->hSpdFeedBack = ENC_GetAvrgMecSpeed01Hz(oEncoder[M1]);  
//	 }	 

     pVars->hRunCnt = TIM_GetCounter(TIMx);
	 pVars->hSpdFeedBack = ENC_GetAvrgMecSpeed01Hz(this);  
	
	
	 pVars->hDletaCnt = pVars->hRunCnt -  pVars->hPreCnt ;
     if(pVars->hDletaCnt >16000)
     {
        pVars->hDletaCnt = 16384 - pVars->hRunCnt -  pVars->hPreCnt ;
     }

    //AB信号断线或电路异常故障检测 (A,B分别持续为0或1,则电路异常或断线)
    #ifdef DOUBLE_DRIVER
   	if( pVars->hPreCnt != pVars->hRunCnt  && ControlCommand.hSpeedM2==0 && ControlCommand.hSpeedM1==0 && FOCVars[M2].bRunning ==0  && FOCVars[M1].bRunning ==0 && bCanCommFault==0 ) // && FOCVars[M2].bRunning ==0  && FOCVars[M1].bRunning ==0
	#else
   	if( pVars->hPreCnt != pVars->hRunCnt  && ControlCommand.hSpeedM1==0 && FOCVars[M1].bRunning ==0 && bCanCommFault==0 )
   	#endif
   	{ 	 
		   if( GPIO_ReadInputDataBit(pParams->hEnAPort, pParams->hEnAPin) ==0 )    //1
		    {
		      	pVars->hEncoderA0StateCnt++;
		       	if(pVars->hEncoderA0StateCnt>800)
		        {
		           pVars->bEncoderA0Fault=1;
		           pVars->hEncoderA0StateCnt=1500;
	           }
		    }
		   else
			  {
				  if( pVars->bEncoderA0Fault==0)
				     pVars->hEncoderA0StateCnt=0;
			  } 

		   if( GPIO_ReadInputDataBit(pParams->hEnBPort, pParams->hEnBPin)==0 )  //1
		    {
		      pVars->hEncoderB0StateCnt++;
		       if(pVars->hEncoderB0StateCnt>800)
		         {
		           pVars->bEncoderB0Fault=1;
		           pVars->hEncoderB0StateCnt=1500;
	           }
		    }
		   else
			  {
				  if( pVars->bEncoderB0Fault==0)
				     pVars->hEncoderB0StateCnt=0;
				}

		   if( GPIO_ReadInputDataBit(pParams->hEnAPort, pParams->hEnAPin) ==1)    //1
		    {
		      pVars->hEncoderA1StateCnt++;
		       if(pVars->hEncoderA1StateCnt>800)
		         {
		           pVars->bEncoderA1Fault=1;
		           pVars->hEncoderA1StateCnt=1500;
	           }
		    }
		   else
			  {
				  if( pVars->bEncoderA1Fault==0)
				     pVars->hEncoderA1StateCnt=0;
			  } 

		   if( GPIO_ReadInputDataBit(pParams->hEnBPort, pParams->hEnBPin)==1)  //1
		    {
		      pVars->hEncoderB1StateCnt++;
		       if(pVars->hEncoderB1StateCnt>800)
		         {
		           pVars->bEncoderB1Fault=1;
		           pVars->hEncoderB1StateCnt=1500;
	           }
		    }
		   else
			  {
				  if( pVars->bEncoderB1Fault==0)
				     pVars->hEncoderB1StateCnt=0;
				}				
				
		}
		 
	 pVars->hPreCnt = pVars->hRunCnt;

	 #ifdef DOUBLE_DRIVER
	 if(pVars->hDletaCnt==0 && ControlCommand.hSpeedM2!=0 && ControlCommand.hSpeedM1!=0 && FOCVars[M2].bRunning ==1  && FOCVars[M1].bRunning ==1 && bCanCommFault==0 &&  pVars->hSpdFeedBack!=0) //A/B脉冲运行中异常判断
	 #else
	 if(pVars->hDletaCnt==0 && ControlCommand.hSpeedM1!=0 && FOCVars[M1].bRunning ==1 && bCanCommFault==0 &&  pVars->hSpdFeedBack!=0) //A/B脉冲运行中异常判断
	 #endif
	 {
		 
		 if(pVars->hTIMErrCnt>6000)  //10000(连续1.25s)
			 pVars->bEncoderFault=1;
		 else
			 pVars->hTIMErrCnt++;
	 }
	 else
	  {pVars->hTIMErrCnt=0;}
	 
	 if(pVars->hDletaCnt>20)//A/B脉冲偏差过大故障
	 {
		 if(pVars->hDletaErrCnt>100)
			  pVars->bEncoderDletaCntFault=1; 
		 else 
			 pVars->hDletaErrCnt++;			 
	 }
	 else 
	 {
	 	pVars->hDletaErrCnt=0;
	 }


    //判断Z信号异常(没有进入Z中断)
		if(pVars->bEncoderB0Fault==0 && pVars->bEncoderB1Fault==0 && pVars->bEncoderA0Fault==0 && pVars->bEncoderA1Fault==0)
		{
			if(TIM_GetCounter(TIMx)>16200)
		     pVars->hEncoderZCnt++;
		  if(pVars->hEncoderZCnt>800 && pVars->hZ_StateCnt==0)	       
			{
				pVars->bEncoderZFault=1; 
				pVars->hEncoderZCnt=801;
			}
	       
		}
	 
		 
	//pVars->hPulsesCnt = hAux;
	
	//hAngleTemp = (s16)((pParams->bPoleNum * pVars->hPulsesCnt)<<4);//4  65536/4096=16(左移16位)
	//	 hAngleTemp = (s16)((pParams->bPoleNum * hAux)<<2);//4
		
//	 hAngleTemp = ((pParams->bPoleNum * (s32)hAux))% 16384;//4	
//		hAngleTemp = (hAngleTemp <<2) ;
//		tmp_test = (u16)(hAngleTemp);
//		hAngleTemp = (s16)tmp_test;
	 
	  hHalfPulsesNum = pParams->hPulsesNum>>1;
	  hAngleTemp = (s16) ( pParams->bPoleNum * (((hAux - hHalfPulsesNum)<<15)/hHalfPulsesNum )  ); 	 //   hAux
	 

	pVars->hPreMeasuredElAngle = pVars->hMeasuredElAngle;	

	pVars->hMeasuredElAngle = hAngleTemp;
	

	pVars->hDeltaElAngle = pVars->hMeasuredElAngle - pVars->hPreMeasuredElAngle;	
	
	pVars->wElSpeedSum += pVars->hDeltaElAngle;

	pVars->wElSpeedSum -= pVars->hElSpeedBuf[pVars->hElSpeedCnt];
	  
	pVars->hElSpeedBuf[pVars->hElSpeedCnt] = pVars->hDeltaElAngle;

	pVars->hElSpeedDpp = pVars->wElSpeedSum>>8;

	#ifdef ENC_ANGLECALC

	pVars->hElAngle = pVars->hElAngle+pVars->hElSpeedDpp;

	if(pVars->bHallAdjustCnt > 0)
    {
        pVars->bHallAdjustCnt --;
        pVars->hElAngle += pVars->hAdjustAngle;
    }

    #else 

	pVars->hElAngle =  (pVars->hMeasuredElAngle + pVars->hPhaseShift); 


    #endif
	
	
	if(pVars->hElSpeedCnt < 255)
		pVars->hElSpeedCnt++;
	else 
		pVars->hElSpeedCnt = 0;


	if(pVars->hEncoderTime<10000)
        pVars->hEncoderTime++;   
		

	pVars->hInc60Angle += pVars->hElSpeedDpp;
	pVars->wInc360Angle += pVars->hElSpeedDpp;


    if((pParams->hEnAPort->IDR & pParams->hEnAPin)!=0)      
    	bPort = 1;
    else
    	bPort = 0;

    if(bPort !=  pVars->bEncoderALevel)
    {
		pVars->bEncoderALevel = bPort;
		pVars->bEncoderAState = 1;
    }

    if((pParams->hEnBPort->IDR & pParams->hEnBPin)!=0)      
    	bPort = 1;
    else
    	bPort = 0;

    if(bPort !=  pVars->bEncoderBLevel)
    {
		pVars->bEncoderBLevel = bPort;
		pVars->bEncoderBState = 1;
    }


	if(pVars->hInc60Angle>S16_60_PHASE_SHIFT) 
	{
	
		pVars->hInc60Angle = 0 ;
		pVars->hLastEncoderTime = pVars->hEncoderTime;
		pVars->hEncoderTime = 0; 


	}	
	
/*
	if(pVars->wInc360Angle > (int32_t)1638400) 
	{
		pVars->wInc360Angle = 0;

		if((pVars->bEncoderAState == 0)||(pVars->bEncoderBState == 0))
		{
			pVars->bEncoderFault=1;

			if(pVars->bEncoderAState == 0)
				pVars->bFaultState |= 0x01;
			else
				pVars->bFaultState |= 0x02;
		}
		else
		{
			pVars->bEncoderAState = 0;
			pVars->bEncoderBState = 0;

		}

		if(pVars->bEncoderZState == 0)
		{
			pVars->bEncoderFault=1;
			pVars->bFaultState |= 0x04;
		}
		else
		{
			pVars->bEncoderZState = 0;

		}


	}
*/

	if((Global->bRunning != 0)&&(Global->bBlockageCurrent!=0))
	//if((Global->bRunning != 0))
	{
/*		
		if( (pVars->wElSpeedSum > 1)||(pVars->wElSpeedSum < (-1))  )   
		{
			pVars->hBlockageTimer = 0;

		}
		else
		{
			if(pVars->hBlockageTimer<60000) //40000
				pVars->hBlockageTimer++;
		}
*/
	

		if( pVars->hDletaCnt==0 && ControlCommand.hSpeedM2!=0 && ControlCommand.hSpeedM1!=0 )   
		{
			if(pVars->hBlockageTimer<60000) //40000
				pVars->hBlockageTimer++;
		}
		else   
		{
			pVars->hBlockageTimer = 0;

		}
		
		if (pVars->hBlockageTimer >= LOCKED_TIME) //  32000
		{
			pVars->bBlockageFault = 1;
		}
	}	
	else
	{
		pVars->hBlockageTimer = 0;
	}

}

/*******************************************************************************
* Function Name  : ENC_GetElAngle
* Description    : 获取当前电角度
* parameters     : this ENCpool结构体指针
* Return         : pVars->hElAngle 当前电角度
*******************************************************************************/
int16_t ENC_GetElAngle(pENC_t this)
{
    pENCVars_t pVars = &(this->Vars);
   
    return (pVars->hElAngle);

}
int16_t hMecSpeed01Hz; 

/*******************************************************************************
* Function Name  : ENC_GetAvrgMecSpeed01Hz
* Description    : 获取当前电机本体转速（不考虑减速器）
* parameters     : this HALLpool结构体指针
* Return         : hMecSpeed01Hz 当前电机本体转速
*******************************************************************************/
int16_t ENC_GetAvrgMecSpeed01Hz(pENC_t this)
{
    pENCVars_t pVars = &(this->Vars);

	int32_t wTemp;; 

       
    
    //hMecSpeed01Hz = (int16_t)((pVars->bPositive *  (int32_t)HALL_MEC_RATIO * 100)/ (pVars->hAvrgHallTimeSum));//扩大100倍

//	hMecSpeed01Hz = (int16_t)((pVars->hElSpeedDpp *pVars->bPositive*  (int32_t)60)/ (MEC_RATIO));//60
   wTemp = (pVars->hElSpeedDpp *50);//60  32

   if(wTemp>32767)
   	wTemp =32767;
   else if(wTemp<-32767)
	 wTemp =-32767;

   hMecSpeed01Hz = (int16_t)wTemp;

   // hMecSpeed01Hz = (hMecSpeed01Hz*3)>>1;	
    //if((hMecSpeed01Hz > -30)&&(hMecSpeed01Hz < 30))
    //    hMecSpeed01Hz = 0;

    return(hMecSpeed01Hz);
}




/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : 编码器Z外部中断函数，PC13
* parameters     : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	static uint8_t hCnt_flag1=0;
	
	pENCVars_t pVars = &(oEncoder[M1]->Vars);
	
	if(EXTI_GetITStatus(EXTI_Line13)!= RESET)
	{

		if(hCnt_flag1==0)
		{
			pVars->hZCnt=  TIM_GetCounter(TIM2);
			// UARTC_PutChar(pVars->hZCnt>>8);
			// UARTC_PutChar(pVars->hZCnt);
			//hCnt_flag1=1;
			if(pVars->hZCnt<16384)
				hCnt_flag1=1;		
		}
		//pVars->hZCnt=  TIM_GetCounter(TIM2);
		
		if(hCnt_flag1==1)
		 { 
		   pVars->hZ_StateCnt++ ;
			 if( pVars->hZ_StateCnt>1000)
				 pVars->hZ_StateCnt=1500;
		 }
		 	
		//TIM_SetCounter(TIM2,(u16)(0)); 				

		//if( (pVars->hZCnt<500 ||  pVars->hZCnt>15500) && pVars->hZ_StateCnt>=4   )  //(清0计数值为1进入Z中断,不清0计数值为4才进入Z中断)
		  if( (pVars->hZCnt<200  || TIM_GetCounter(TIM2)>16300) && pVars->hZ_StateCnt>=2  )//(清0计数值为1进入Z中断,不清0计数值为4才进入Z中断)
		  {TIM_SetCounter(TIM2,(u16)(0)); }
		
		
		EXTI_ClearITPendingBit(EXTI_Line13);
		//pVars->bPWMInputCapFlag = 1;
		pVars->bEncoderZState = 1;
    pVars->bZ_flag = ~(pVars->bZ_flag);
		
		#ifdef ENC_ANGLECALC

		pVars->hAdjustAngle = (int16_t)(pVars->hPhaseShift - pVars->hElAngle);
		
		 if(pVars->hElSpeedDpp < 100)
		{
		    pVars->bHallAdjustCnt = 16;
		    pVars->hAdjustAngle >>= 4;
		}
		else if(pVars->hElSpeedDpp < 280)
		{
		    pVars->bHallAdjustCnt = 8;
		    pVars->hAdjustAngle >>= 3;
		}
		else if(pVars->hElSpeedDpp < 720)
		{
		    pVars->bHallAdjustCnt = 4;
		    pVars->hAdjustAngle >>= 2;
		}
		else
		{
		    pVars->bHallAdjustCnt = 2;
		    pVars->hAdjustAngle >>= 1;
		}
		#endif

	}
}



/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : 编码器Z外部中断函数，PD2
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER
void EXTI2_IRQHandler(void)
{
	static uint8_t hCnt_flag=0;
	
	pENCVars_t pVars = &(oEncoder[M2]->Vars);
	
	if(EXTI_GetITStatus(EXTI_Line2)!= RESET)
	{
     
		if(hCnt_flag==0)
		{
			pVars->hZCnt=  TIM_GetCounter(TIM4);
			// UARTC_PutChar(pVars->hZCnt>>8);
			// UARTC_PutChar(pVars->hZCnt);
			//hCnt_flag=1;
			 if(pVars->hZCnt<16384)
				hCnt_flag=1;
		}

		//pVars->hZCnt=  TIM_GetCounter(TIM4);
		
		if(hCnt_flag==1)
		 { 
		   pVars->hZ_StateCnt++ ;
			 if( pVars->hZ_StateCnt>1000)
				 pVars->hZ_StateCnt=1500;
		 }
		
		//TIM_SetCounter(TIM4,(u16)(0));	   
		 
		 //if( (pVars->hZCnt<500  || TIM_GetCounter(TIM4)>15500) && pVars->hZ_StateCnt>=4  ) //(清0计数值为1进入Z中断,不清0计数值为4才进入Z中断)
		 if( (pVars->hZCnt<200  || TIM_GetCounter(TIM4)>16300) && pVars->hZ_StateCnt>=2  )//(清0计数值为1进入Z中断,不清0计数值为4才进入Z中断)
		  { 
			   TIM_SetCounter(TIM4,(u16)(0));
			}			 
		 
		EXTI_ClearITPendingBit(EXTI_Line2);
		//pVars->bPWMInputCapFlag = 1;
		pVars->bEncoderZState = 1;
    pVars->bZ_flag = ~(pVars->bZ_flag);
		#ifdef ENC_ANGLECALC
		pVars->hAdjustAngle = (int16_t)(pVars->hPhaseShift - pVars->hElAngle);
		
		 if(pVars->hElSpeedDpp < 100)
		{
		    pVars->bHallAdjustCnt = 16;
		    pVars->hAdjustAngle >>= 4;
		}
		else if(pVars->hElSpeedDpp < 280)
		{
		    pVars->bHallAdjustCnt = 8;
		    pVars->hAdjustAngle >>= 3;
		}
		else if(pVars->hElSpeedDpp < 720)
		{
		    pVars->bHallAdjustCnt = 4;
		    pVars->hAdjustAngle >>= 2;
		}
		else
		{
		    pVars->bHallAdjustCnt = 2;
		    pVars->hAdjustAngle >>= 1;
		}
		#endif

	}
}
#endif

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : TIM2 M1 Encoder捕获中断
* parameters     : None
* Return         : None
*******************************************************************************/
u8  Flag1 =0;
u8  Flag2 =0;
u8  Flag3 =0;
u16 g_TestCnt = 0;
u16 g_TestCnt1 = 0;
u16 g_TestCnt2 = 0;
u16 g_TestCnt3 = 0;
uint8_t bPWM_CAPTURE_STA1;

void TIM2_IRQHandler(void)
{
	pENCVars_t pVars = &(oEncoder[M1]->Vars);
	static uint8_t ICPolarity=0;
	static uint8_t bCounter=0;

		if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)//捕获1发生捕获事件 
		{	
			 g_TestCnt1++;
			if(pVars->bPWM_CapSta &0x40 )		//捕获到一个上升沿  pVars->bPWM_CAPTURE_STA		    
			{	  			
				g_TestCnt2++;
				Flag3 =~Flag3;
			//	pVars->bPWM_CAPTURE_STA|=0x80;		//标记成功捕获到一次低电平脉宽
			    pVars->bPWM_CapSta=TIM_GetCapture3(TIM2);//获取当前的捕获值.
				//pVars->hCAP_LOW_VAL_END =  pVars->hPWM_CAPTURE_VAL;

				if(pVars->bCapLowCount  < 6) //测试信号周期，两个上升沿的时间
				{
					pVars->hCapPeriodValEnd = pVars->hPWM_CapVal;
					if(pVars->hCapPeriodValEnd < pVars->hCapPeriodValStart)
					{
						pVars->hCapPeriodVal =  65536-  pVars->hCapPeriodValStart ;
						pVars->hCapPeriodVal += pVars->hCapPeriodValEnd;
					}
					else
					{
						pVars->hCapPeriodVal = pVars->hCapPeriodValEnd  - pVars->hCapPeriodValStart;
					}


					if(pVars->bCapLowCount <6)
					 {
						TIM2->CNT =0;
						pVars->wCapPeriodVal[pVars->bCapLowCount] = (uint32_t)pVars->wCapPeriodVal;
						TIM_ClearITPendingBit(TIM2, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位	
					 }					
					
	      }

				else
				{
				
					pVars->hCapLowValEnd =  pVars->hPWM_CapVal;
					
					if(pVars->hCapLowValEnd < pVars->hCapLowValStart)
					{
					  	pVars->hCapLowVal =  65536 -  pVars->hCapLowValStart;
				    	pVars->hCapLowVal += pVars->hCapLowValEnd;				
					}
					else
					{			
				    	pVars->hCapLowVal = pVars->hCapLowValEnd - pVars->hCapLowValStart;
					}
					
					
					if(pVars->bCapLowCount <12)
					{				
					  TIM2->CNT =0;
					  pVars->wCapLowLevelVal[pVars->bCapLowCount-6] = pVars->hCapLowVal;
					  TIM_ClearITPendingBit(TIM2, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位							
					}
					else
					{
						TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC3,DISABLE);
						TIM_ITConfig(TIM4,TIM_IT_CC3,ENABLE);	
						 pVars->bPWMInputCapFlag = 1;
					
					}
				}

				pVars->bCapLowCount++;
				pVars->bPWM_CapSta  =0; // pVars->bPWM_CAPTURE_STA 
				

	 			TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				g_TestCnt++;
				Flag2 =~Flag2;
				pVars->hCapPeriodValEnd = 0;
				pVars->hCapLowValStart = 0;
				pVars->hCapLowValEnd = 0;
				pVars->hCapLowValStart = 0;
				pVars->bPWM_CapSta = 0;			//清空 	pVars->bPWM_CAPTURE_STA
				pVars->hPWM_CapVal = 0;
				pVars->bPWM_CapSta |= 0X40;		//标记捕获到了下降沿  	pVars->bPWM_CAPTURE_STA
				 		
				pVars->hCapPeriodValStart = TIM_GetCapture3(TIM2);
				pVars->hCapLowValStart = TIM_GetCapture3(TIM2);
				if(pVars->bCapLowCount >5)
				{
					TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获 
				}
	 			//TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获 //屏蔽此句能测总数低电平
				TIM_Cmd(TIM2,ENABLE ); 	//使能定时器5
				
				
				TIM_ClearITPendingBit(TIM2, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位	
				
				
			}		    
		}
    	else
		{ 
		   //TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC3,DISABLE);
			if(++bCounter>2)
			{
			     bCounter = 0;
			     pVars->bInitFault = 1;
			     TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC3,DISABLE);//关中断					
			}
		}//关中断	

}


/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : TIM4 M1 Encoder捕获中断
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER
void TIM4_IRQHandler(void)
{
	pENCVars_t pVars = &(oEncoder[M2]->Vars);
	static uint8_t ICPolarity=0;
	static uint8_t bCounter=0;

	if(TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//捕获1发生捕获事件
	{	
		if(pVars->bPWM_CapSta&0X40)		//捕获到一个上升沿 		
		{	  			
			//pVars->bPWM_CAPTURE_STA|=0X80;		//标记成功捕获到一次低电平脉宽
			pVars->hPWM_CapVal = TIM_GetCapture3(TIM4);//获取当前的捕获值.
			//pVars->hCAP_LOW_VAL_END = pVars->hPWM_CAPTURE_VAL;
				
			if(pVars->bCapLowCount < 6) //测试信号周期，两个上升沿的时间
			{
				pVars->hCapPeriodValEnd = pVars->hPWM_CapVal;
				if(pVars->hCapPeriodValEnd < pVars->hCapPeriodValStart)
				{
					pVars->hCapPeriodVal =  65536-  pVars->hCapPeriodValStart;
					pVars->hCapPeriodVal += pVars->hCapPeriodValEnd;
				}
				else
				{
					pVars->hCapPeriodVal = pVars->hCapPeriodValEnd - pVars->hCapPeriodValStart;
				}


				if(pVars->bCapLowCount <6)
				{
					TIM4->CNT =0;
					pVars->wCapPeriodVal[pVars->bCapLowCount] = pVars->hCapPeriodVal;
					TIM_ClearITPendingBit(TIM4, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位	
				}					

			}
			else
			{
				
					pVars->hCapLowValEnd  =  pVars->hPWM_CapVal;
					
					if(pVars->hCapLowValEnd < pVars->hCapLowValStart)
					{
						pVars->hCapLowVal =  65536 -  pVars->hCapLowValStart;
				    	pVars->hCapLowVal += pVars->hCapLowValEnd;				
					}
					else
					{			
				    	pVars->hCapLowVal = pVars->hCapLowValEnd - pVars->hCapLowValStart;
					}
					
					
					if(pVars->bCapLowCount <12)
					{				
					  TIM4->CNT =0;
					  pVars->wCapLowLevelVal[pVars->bCapLowCount-6] = pVars->hCapLowVal;
					  TIM_ClearITPendingBit(TIM4, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位							
					}
					else
					{
						TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,DISABLE);
						
						 pVars->bPWMInputCapFlag = 1;
					
					}
				}				
				

				pVars->bCapLowCount++;
				pVars->bPWM_CapSta =0;
			
	 			TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else//还未开始,第一次捕获上升沿
			{
				pVars->hCapPeriodValEnd = 0;
				pVars->hCapPeriodValStart = 0;
				pVars->hCapLowValEnd = 0;
				pVars->hCapLowValStart = 0;
				pVars->bPWM_CapSta = 0;			//清空
				pVars->hPWM_CapVal = 0;
				pVars->bPWM_CapSta|=0X40;		//标记捕获到了下降沿
				TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5
	 			//TIM_SetCounter(TIM5,0);
				pVars->hCapLowValStart = TIM_GetCapture3(TIM4);
				pVars->hCapPeriodValStart = TIM_GetCapture3(TIM4);
	 			//TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获 //屏蔽此句能测总数低电平
				
				if(pVars->bCapLowCount >5)
			   {
					TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获 
				 }				
				
				TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5				
				
				TIM_ClearITPendingBit(TIM4, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位					
				
			}		    
		}
   		else	
	 	{	
	   		//TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,DISABLE);
		   if(++bCounter>2)
			{
				bCounter = 0;
				pVars->bInitFault = 1;
				TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,DISABLE);//关中断					
			}
		}//关中断 

}
#endif



#endif

/**************************************************************END OF FILE****/
