/******************************************************************************
* File Name         :  HallSensor.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  霍尔及电角度处理函数                         
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


#ifdef SWITCH_HALL  //用开关霍尔

/* Constants ------------------------------------------------------------------*/

HALLParams_t HALLParamsM1 =
{
  TIM4,
  DBGMCU_TIM4_STOP,
  RCC_APB1Periph_TIM4,
  TIM4_IRQn,
  GPIO_NoRemap_TIMx,
  #if (HALLMATCH == HALLMATCH1)
  GPIOB,                         
  GPIO_Pin_6,         
  GPIOB,                        
  GPIO_Pin_7,         
  GPIOB,                        
  GPIO_Pin_8,    
  #elif(HALLMATCH == HALLMATCH2)
  GPIOA,                         
  GPIO_Pin_15,     
  GPIOB,                        
  GPIO_Pin_10,    
  GPIOB,                        
  GPIO_Pin_3,      
  #elif(HALLMATCH == HALLMATCH3)
  GPIOB,                        
  GPIO_Pin_10,       
  GPIOB,                        
  GPIO_Pin_3,     
  GPIOA,                         
  GPIO_Pin_15,   
  #elif(HALLMATCH == HALLMATCH4)    
  GPIOB,                        
  GPIO_Pin_3,     
  GPIOA,                         
  GPIO_Pin_15,       
  GPIOB,                        
  GPIO_Pin_10,    
  #elif(HALLMATCH == HALLMATCH5)       
  GPIOB,                        
  GPIO_Pin_3,         
  GPIOB,                        
  GPIO_Pin_10,   
  GPIOA,                         
  GPIO_Pin_15,    
  #elif(HALLMATCH == HALLMATCH6)  
  GPIOB,                        
  GPIO_Pin_10,    
  GPIOA,                         
  GPIO_Pin_15,         
  GPIOB,                        
  GPIO_Pin_3,        
  #endif
  PSEUDO_FREQ_CONV,
};

#ifdef DOUBLE_DRIVER
HALLParams_t HALLParamsM2 =
{
  TIM4,
  DBGMCU_TIM4_STOP,
  RCC_APB1Periph_TIM4,
  TIM4_IRQn,
  GPIO_NoRemap_TIMx,
  #if (HALLMATCHS == HALLMATCH1)
  GPIOB, 
  GPIO_Pin_6,           
  GPIOB, 
  GPIO_Pin_7,          
  GPIOB, 
  GPIO_Pin_8,  
  #elif(HALLMATCHS == HALLMATCH2)
  GPIOB, 
  GPIO_Pin_6, 
  GPIOB, 
  GPIO_Pin_8,
  GPIOB, 
  GPIO_Pin_7,
  #elif(HALLMATCHS == HALLMATCH3)
  GPIOB, 
  GPIO_Pin_8,   
  GPIOB, 
  GPIO_Pin_7, 
  GPIOB, 
  GPIO_Pin_6, 
  #elif(HALLMATCHS == HALLMATCH4)    
  GPIOB, 
  GPIO_Pin_7,  
  GPIOB, 
  GPIO_Pin_6,   
  GPIOB, 
  GPIO_Pin_8,
  #elif(HALLMATCHS == HALLMATCH5)       
  GPIOB, 
  GPIO_Pin_7,   
  GPIOB, 
  GPIO_Pin_8,
  GPIOB, 
  GPIO_Pin_6,   
  #elif(HALLMATCHS == HALLMATCH6)  
  GPIOB, 
  GPIO_Pin_8,   
  GPIOB, 
  GPIO_Pin_6,
  GPIOB, 
  GPIO_Pin_7,   
  #endif
  PSEUDO_FREQ_CONV,
};
#endif

#define HALL_COUNTER_RESET  ((u16) 0u)

#define S16_30_PHASE_SHIFT	(s16)(65536/12)
#define S16_60_PHASE_SHIFT  (s16)(65536/6)
#define S16_120_PHASE_SHIFT (s16)(65536/3)

#define S16_90_PHASE_SHIFT      (s16)(S16_30_PHASE_SHIFT * 3)
#define S16_150_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 5)
#define S16_180_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 6)
#define S16_210_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 7)
#define S16_240_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 8)
#define S16_270_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 9)
#define S16_300_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 10)
#define S16_330_PHASE_SHIFT     (s16)(S16_30_PHASE_SHIFT * 11)

//#define LOCKED_TIME        	((int16_t)7200) //堵转时间 7200 PWM cycle = 450ms
//#define LOCKED_TIME         ((int16_t)125) //堵转时间 256*125 PWM cycle = 4s
#define LOCKED_TIME         ((int16_t)500) //堵转时间 256*187 PWM cycle =  6s(187)  (16s)500    10s(311)  

#define CCER_CC1E_Set		((int16_t)0x0001)
#define CCER_CC1E_Reset		((int16_t)0xFFFE)
#define SENSOR_2_DELTA_PERIOD    ((int32_t)(500))
#define SENSOR_5_DELTA_PERIOD    ((int32_t)(1250))

//#define POLE_PAIR_NUM		((u16) 4u) 	//极对数
#define REDUCTION_RATIO		((u16) 13u)	//减速比

#define MEC_RATIO  POLE_PAIR_NUM  

//#define MEC_RATIO  (POLE_PAIR_NUM*REDUCTION_RATIO)

#define DIRECTION_MAX_CNT   ((int8_t) 18u)
#define DIRECTION_ERR_CNT   ((int8_t) 3u)

//#define RUN_BLDC_SPEED_TIME (uint16_t)800//霍尔换向时间大,强制跑方波（建议范围：100-200ms）
#define RUN_BLDC_SPEED_TIME (uint16_t)400  //400

//#define RUN_BLDC_ANGLE//强制跑方波

#define HALL_SIMPLE//HALL简单滤波

#ifdef LINEAR_HALL//191205
extern int32_t hAngleFilterSum[2];
extern int16_t hAngleReceived[2];
extern uint8_t ucHallVal[2];//接收到的霍尔故障信息
extern int16_t hAngleSpdReceived[2];//接收到的角速度
#endif

/* Variables ------------------------------------------------------------------*/

HALL_t HALLpool[NBR_OF_MOTORS]; 

const uint8_t HALL_CwSeq[8] = {0,3,6,2,5,1,4,0};  //645132
const uint8_t HALL_CcwSeq[8] = {0,5,3,1,6,4,2,0}; //623154

const int16_t S16_PHASE_SHIFT_TAB[8] =
{
    0,
    S16_90_PHASE_SHIFT ,
    S16_210_PHASE_SHIFT ,
    S16_150_PHASE_SHIFT ,
    S16_330_PHASE_SHIFT ,
    S16_30_PHASE_SHIFT ,
    S16_270_PHASE_SHIFT ,
    0
};

#ifdef UART_DEBUG
extern uint8_t bFlagTemp1;

extern int16_t hHallAngle1;
#endif


                            
int16_t h30PhaseShift;


int8_t HallPortStatus;

/* declaration ------------------------------------------------------------------*/
static void HALL_Config(pHALL_t this);
static void HALL_ElAngleInit(pHALL_t this);
void HALL_CheckHallState(pHALL_t this);

extern Relay_t Relay;

/*******************************************************************************
* Function Name  : HALL_MeasureInit
* Description    : 变量初始化 用于电机启动
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
void HALL_MeasureInit(pHALL_t this , int16_t hDesCmd)  
{
    pHALLVars_t pVars =&(this->Vars);
    pHALLParams_t pParams = this->pParams;      
   
    TIM_ITConfig(pParams->TIMx, TIM_IT_CC1, DISABLE);
    pVars->hOVFCounter = 0;
    pVars->hElSpeedDpp = 0;

	
	 if(hDesCmd > 0)

		pVars->bSpeed = pVars->bPositive;
	else
		pVars->bSpeed = pVars->bNegative;
          
    pVars->bPositiveCounter = 0;
    pVars->bNegativeCounter = 0;
    pVars->wSensorPeriod[1] = 62500;
    pVars->wSensorPeriod[2] = 62500;
    pVars->wSensorPeriod[3] = 62500;
    pVars->wSensorPeriod[4] = 62500;
    pVars->wSensorPeriod[5] = 62500;
    pVars->wSensorPeriod[6] = 62500;
    pVars->hHallTimeSum = 2000;
    pVars->wSensorCaptureSum = 960000;
    pVars->hLastHallTime = 2000;
    pVars->hAvrgHallTimeSum = pVars->hHallTimeSum * 6 * pVars->bSpeed * pVars->bPositive;

    TIM_SetCounter(pParams->TIMx, 0);
    TIM_Cmd(pParams->TIMx, ENABLE);
    TIM_ITConfig(pParams->TIMx, TIM_IT_CC1, ENABLE);
    
    pVars->hHallATime = 0;
    pVars->hHallBTime = 0;
    pVars->hHallCTime = 0;

	pVars->bHallKeepTime1=0;
	pVars->bHallKeepTime2=0;
	pVars->bHallKeepTime3=0;
    
    HALL_ElAngleInit(this);
}


/*******************************************************************************
* Function Name  : HALL_ElAngleInit
* Description    : 变量初始化 用于电机启动
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
void HALL_ElAngleInit(pHALL_t this) 
{
    pHALLVars_t pVars = &(this->Vars);
       
    pVars->bHallInitFault = 0;

    pVars->bHallPortState=0xff;
    
    HALL_CheckHallState(this);
    
    if(pVars->bHallPortState==0xff)
    {
      //  pVars->bHallInitFault = 1;
        return ;
    }

//    if(pVars->hAvrgElSpeed<5)
//    {
//        pVars->hAvrgElSpeed = 5;
//    }
		 pVars->hAvrgElSpeed = 0;
      
    pVars->bHallNewState = pVars->bHallPortState;
 	pVars->bHallRunState = pVars->bHallNewState;
	
    if(pVars->bHallRunState != pVars->bHallNewState)
    {
        pVars->bCaptureFlag = 0;
        pVars->bHallLastState= pVars->bHallRunState;
        pVars->bHallRunState = pVars->bHallNewState;
    	pVars->hLastHallTime = pVars->hHallTime;
    	pVars->hHallTime = 0;

//		pVars->hElAngle = pVars->hMeasuredElAngle;
//        pVars->wDeltaElAngle = (u32)(S16_60_PHASE_SHIFT);
    }
    h30PhaseShift = S16_30_PHASE_SHIFT * pVars->bSpeed * pVars->bPositive;
    pVars->hMeasuredElAngle = (s16)(S16_PHASE_SHIFT_TAB[pVars->bHallRunState] + pVars->hTheoryAngleShift);

    pVars->hElAngle = pVars->hMeasuredElAngle;
    pVars->wDeltaElAngle = 0;
   
    if(pVars->bPositive>0)
	{
    	pVars->bHallPositiveState = HALL_CwSeq[pVars->bHallRunState];
    	pVars->bHallNegativeState = HALL_CcwSeq[pVars->bHallRunState];
	}
	else
	{
		pVars->bHallPositiveState = HALL_CcwSeq[pVars->bHallRunState];
    	pVars->bHallNegativeState = HALL_CwSeq[pVars->bHallRunState];
	}
	   
   	if(pVars->hLastHallTime > 500)
   	{
   		pVars->hLastHallTime = 500;
   	}

    pVars->hElSpeedDpp = pVars->hAvrgElSpeed * pVars->bSpeed * pVars->bPositive;
   
}


/*******************************************************************************
* Function Name  : HALL_GetPortsVal
* Description    : 读取Hall Sensor 值
* parameters     : this  HALLpool结构指针
* Return         : bHallState 霍尔值
*******************************************************************************/
static uint8_t HALL_GetPortsVal(pHALL_t this)
{
    pHALLParams_t pParams = this->pParams;
   uint8_t bHallState=0;
   
   
	#if (SENSORPLACEMENT == DEGREES_120)
	{
		if(HallPortStatus ==0)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=4;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=2;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=1;
		}
		else	if(HallPortStatus ==1)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=4;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=1;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=2;
		}
		else		if(HallPortStatus ==2)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=1;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=2;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=4;
		}
		else		if(HallPortStatus ==3)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=1;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=4;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=2;
		}
			else	if(HallPortStatus ==4)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=2;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=4;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=1;
		}
			else	if(HallPortStatus ==5)
		{
         if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)      bHallState|=2;
         if((pParams->hH2Port->IDR & pParams->hH2Pin)!=0)      bHallState|=1;
         if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)      bHallState|=4;
		}

	}
	#else
	{
        if((pParams->hH3Port->IDR & pParams->hH3Pin)!=0)       bHallState|=1;
        if((pParams->hH2Port->IDR & pParams->hH2Pin)==0)       bHallState|=2;
        if((pParams->hH1Port->IDR & pParams->hH1Pin)!=0)       bHallState|=4;
	}
    #endif

	return(bHallState);//bHallState  堵转测试把该值设为常数 4 
}

/*******************************************************************************
* Function Name  : HALL_CheckHallState
* Description    : 读取Hall Sensor 值 + 滤波
* parameters     : this  HALLpool结构指针
* Return         : bHallState 霍尔值
*******************************************************************************/
void HALL_CheckHallState(pHALL_t this)
{
    pHALLVars_t pVars = &(this->Vars);
       

	uint8_t bAux1=0;
	uint8_t bAux2=0;
	uint8_t i,j;
	
    
    bAux1 = HALL_GetPortsVal(this);
    
    for (i = 4,j=8; i > 0; i--)
    {
        if (--j == 0) 
            return;
        
        bAux2 = HALL_GetPortsVal(this);
        
        if (bAux1 != bAux2)
        {
            i = 4;
            bAux1 = bAux2;
        }
    }

    pVars->bHallPortState = bAux1;

}


/*******************************************************************************
* Function Name  : HALL_Init
* Description    : 上电HALL初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void HALL_Init(void)
{
    oHall[0] = &HALLpool[0];
    oHall[0]->pParams = &HALLParamsM1;

	#ifdef DOUBLE_DRIVER
    oHall[1] = &HALLpool[1];
    oHall[1]->pParams = &HALLParamsM2;
    #endif

    #ifdef MOTOR_DIR_IS_CW_M1
    oHall[0]->Vars.bPositive = POSITIVE;
    oHall[0]->Vars.bNegative = NEGATIVE;
	oHall[0]->Vars.hPhaseShift = PHASE_SHIFT_M1;
	oHall[0]->Vars.hTheoryAngleShift = Find_TheoryAngleShift(oHall[0]->Vars.hPhaseShift);
    #else
    oHall[0]->Vars.bPositive = NEGATIVE;
    oHall[0]->Vars.bNegative = POSITIVE;
	oHall[0]->Vars.hPhaseShift = PHASE_SHIFT_M1;
	oHall[0]->Vars.hTheoryAngleShift = Find_TheoryAngleShift(oHall[0]->Vars.hPhaseShift);
    #endif

    #ifdef MOTOR_DIR_IS_CW_M2
	oHall[1]->Vars.bPositive = POSITIVE;
    oHall[1]->Vars.bNegative = NEGATIVE;
	oHall[1]->Vars.hPhaseShift = PHASE_SHIFT_M2;
	oHall[1]->Vars.hTheoryAngleShift = Find_TheoryAngleShift(oHall[1]->Vars.hPhaseShift);
    #else
	oHall[1]->Vars.bPositive = NEGATIVE;
    oHall[1]->Vars.bNegative = POSITIVE;
	oHall[1]->Vars.hPhaseShift = PHASE_SHIFT_M2;
	oHall[1]->Vars.hTheoryAngleShift = Find_TheoryAngleShift(oHall[1]->Vars.hPhaseShift);
    #endif

    #ifndef LINEAR_HALL
    HALL_Config(oHall[0]);
    #ifdef DOUBLE_DRIVER
    HALL_Config(oHall[1]);
    #endif
    #endif
}

/*******************************************************************************
* Function Name  : HALL_Config
* Description    : HALL相关IC外设模块初始化
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
static void HALL_Config(pHALL_t this)
{
    TIM_TimeBaseInitTypeDef TIM_HALLTimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_HALLICInitStructure;
    NVIC_InitTypeDef NVIC_InitHALLStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    pHALLVars_t pVars = &(this->Vars);
    pHALLParams_t pParams = this->pParams;    
    TIM_TypeDef* TIMx = pParams->TIMx;    
    uint8_t i;

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

    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure Hall sensors H1 input */
    GPIO_InitStructure.GPIO_Pin = pParams->hH1Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hH1Port , &GPIO_InitStructure);

    /* Configure Hall sensors H2 input */
    GPIO_InitStructure.GPIO_Pin = pParams->hH2Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hH2Port, &GPIO_InitStructure);

    /* Configure Hall sensors H3 input */
    GPIO_InitStructure.GPIO_Pin = pParams->hH3Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pParams->hH3Port, &GPIO_InitStructure);
    

    /* Timer configuration in Clear on capture mode */
    TIM_DeInit(TIMx);

    TIM_TimeBaseStructInit(&TIM_HALLTimeBaseInitStructure);
    /* Set full 16-bit working range */
    TIM_HALLTimeBaseInitStructure.TIM_Period = U16_MAX;
    TIM_HALLTimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_HALLTimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx,&TIM_HALLTimeBaseInitStructure);

    TIM_ICStructInit(&TIM_HALLICInitStructure);
    TIM_HALLICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_HALLICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_HALLICInitStructure.TIM_ICFilter = HALL_IC_FILTER;

    TIM_ICInit(TIMx,&TIM_HALLICInitStructure);

    /* Force input capture from TRC */
    /* Disable the Channel 1: Reset the CC1E Bit */
    TIMx->CCER &= CCER_CC1E_Reset;
    TIMx->CCMR1 |= 0x03u; /* CCS1 = 0b11; */
    TIMx->CCER |= CCER_CC1E_Set;

    /* Force the TIMx prescaler with immediate access (no need of an update event) 
    */ 
    TIM_PrescalerConfig(TIMx, (u16) HALL_MAX_RATIO-1, TIM_PSCReloadMode_Immediate);
    TIM_InternalClockConfig(TIMx);

    /* Enables the XOR of channel 1, channel2 and channel3 */
    TIM_SelectHallSensor(TIMx, ENABLE);

    TIM_SelectInputTrigger(TIMx, TIM_TS_TI1F_ED);
    TIM_SelectSlaveMode(TIMx,TIM_SlaveMode_Reset);

    /* Source of Update event is only counter overflow/underflow */
    TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource_Regular);

    /* Enable the TIMx IRQChannel*/
    NVIC_InitHALLStructure.NVIC_IRQChannel =  pParams->TIMx_IRQChannel;  
    NVIC_InitHALLStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
    NVIC_InitHALLStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
    NVIC_InitHALLStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitHALLStructure);

    /* Clear the TIMx's pending flags */
    TIMx->SR = 0u;

    /* Selected input capture and Update (overflow) events generate interrupt */
    TIM_ITConfig(TIMx, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

    TIM_SetCounter(TIMx, HALL_COUNTER_RESET);
    TIM_Cmd(TIMx, ENABLE);
    
    for (i = 0u;  i< 8; i++)
    {
        pVars->wSensorPeriod[i] = U32_MAX;       
    }

}

/*******************************************************************************
* Function Name  : HALL_CalcElAngle
* Description    : 通过HALL信号量计算出对应的电角速度和电角度
* parameters     : this HALLpool结构体指针      Global FOCVars结构体指针 
* Return         : None
*******************************************************************************/
void HALL_CalcElAngle(pHALL_t this, FOCVars_t *Global)
{
    pHALLVars_t pVars = &(this->Vars);
	pHALLParams_t pParams = this->pParams;
	TIM_TypeDef* TIMx = pParams->TIMx;
	
	uint32_t wHallTimeConv;
    uint32_t wHallPeriod;
    uint16_t hAux=0;
	uint16_t hCaptureDelta;
	int16_t hAvrgHallTimeSum;

	uint8_t bHallDirecChange = 0;
	int8_t bHallDirecFlag;//霍尔方向标志
       
    if(pVars->hHallTime<1000)
        pVars->hHallTime++;
		else
			pVars->hElSpeedDpp=0;

    
    if((Global->bRunning != 0)&&(Global->bBlockageCurrent!=0))
    {
        if(++ pVars->bHallTimeDivisor == 0)
        {
            if(pVars->hHallATime<10000)
                pVars->hHallATime++;

            if(pVars->hHallBTime<10000)
                pVars->hHallBTime++;

            if(pVars->hHallCTime<10000)
                pVars->hHallCTime++;

            if((pVars->hHallATime>LOCKED_TIME)||
                    (pVars->hHallBTime>LOCKED_TIME)||
                    (pVars->hHallCTime>LOCKED_TIME))
            {
                pVars->bBlockageFault = 1;
            }
        }
   }

    pVars->bHallPortState = HALL_GetPortsVal(this);

   // if((pVars->bHallPortState == 0)||
   //         (pVars->bHallPortState == 7))//xu-190629
      if(((pVars->bHallPortState == 0)||
            (pVars->bHallPortState == 7)) && (Relay.bReady ==1))//jimianhao 
    {
        pVars->bHallFaultCnt ++;
        if(pVars->bHallFaultCnt > 12) //3  6
        {
            //pVars->bHallFault = 1;
            pVars->bHallFaultCnt=20;
        }
    }
    else
    {
        pVars->bHallFaultCnt = 0;
		 pVars->bHallFault = 0;
    }
	#ifdef HALL_SIMPLE

    pVars->bHallNewState = pVars->bHallPortState;

    if(pVars->bHallNewState != pVars->bHallRunState)
    {
        if(pVars->bHallNewState == pVars->bHallPositiveState)
        {
            pVars->bHallKeepTime2 = 0;
        	hAux = pVars->hHallTime<<2;
			if((hAux > pVars->hLastHallTime)||(pVars->hHallTime > 5))  //5
           	{

           			pVars->bHallKeepTime1 = 0;
		
                    pVars->bUpdateHallFlag = 1;

//					pVars->bSpeed = pVars->bPositive;
                    pVars->bSpeed = POSITIVE;

                    if(pVars->bPositiveCounter<18)
                        pVars->bPositiveCounter++;

                    pVars->bNegativeCounter = 0;

					if(pVars->hHallTime > RUN_BLDC_SPEED_TIME)//速度很低，跑理论中间角度
						pVars->bPositiveCounter = 1;
                
            }
        }
        else if(pVars->bHallNewState == pVars->bHallNegativeState)
        {
            pVars->bHallKeepTime1 = 0;
			hAux = pVars->hHallTime<<2;
			if((hAux > pVars->hLastHallTime)||(pVars->hHallTime > 5)) //5
			{
           		
	   			pVars->bHallKeepTime2 = 0;
				
	            pVars->bUpdateHallFlag = 1;

	            pVars->bSpeed = NEGATIVE;

	            if(pVars->bNegativeCounter<18)
	                pVars->bNegativeCounter++;

	            pVars->bPositiveCounter = 0;

	            if(pVars->hHallTime > RUN_BLDC_SPEED_TIME)//速度很低，跑理论中间角度
					pVars->bNegativeCounter=1;
                
			}
        }
        else
        {
            pVars->bHallKeepTime3 ++;
            if((pVars->hHallTime > 5)&&(pVars->bHallKeepTime3 > 5))//5  5
            {
                pVars->bHallKeepTime3 = 0;
                pVars->bUpdateHallFlag = 1;

                if(pVars->bNegativeCounter > 0)
                    pVars->bNegativeCounter--;

                if(pVars->bPositiveCounter > 0)
                    pVars->bPositiveCounter--;

                bHallDirecChange = 1;
            }
        }
    }
    else
    {
        pVars->bHallKeepTime3 = 0;
    }
    #else
	   pVars->bHallNewState = pVars->bHallPortState;
    if((pVars->bHallNewState !=pVars->bHallRunState)&&(pVars->bCaptureFlag == 1u))//&&(pVars->bCaptureFlag == 1u)
    {
        if(pVars->bHallNewState == pVars->bHallPositiveState)
        {
            hAux = pVars->hHallTime<<1;
            if((hAux > pVars->hLastHallTime)||(pVars->hHallTime > 100))
            {
               // if((((++pVars->bHallKeepTime1>50)||(pVars->bPositiveCounter>17))&&(((pVars->hHallTime>=pVars->hLastHallTime))/*||(pVars->hHallTime>=(pVars->hLastHallTime*4))*/))
               //     ||(pVars->bSpeed == pVars->bPositive)||(Global->bRunning == 0))
                if((((++pVars->bHallKeepTime1>50)||(pVars->bPositiveCounter>17))&&(((pVars->hHallTime>=pVars->hLastHallTime))/*||(pVars->hHallTime>=(pVars->hLastHallTime*4))*/))
                   ||(pVars->bSpeed == POSITIVE)||(Global->bRunning == 0))
                {
                    pVars->bHallKeepTime1 = 0;

                    pVars->bUpdateHallFlag = 1;

                    //pVars->bSpeed = pVars->bPositive;
                    pVars->bSpeed = POSITIVE;

                    if(pVars->bPositiveCounter<18)
                        pVars->bPositiveCounter++;

                    if(pVars->bNegativeCounter>0)
                        pVars->bNegativeCounter--;

                    if(pVars->hHallTime > RUN_BLDC_SPEED_TIME)
                        pVars->bPositiveCounter=1;
                }
            }
        }
        else if(pVars->bHallNewState == pVars->bHallNegativeState)
        {
            hAux = pVars->hHallTime<<1;
            if((hAux > pVars->hLastHallTime)||(pVars->hHallTime > 100))
            {
               // if((((++pVars->bHallKeepTime2>50)||(pVars->bNegativeCounter>17))&&(((pVars->hHallTime>=pVars->hLastHallTime))/*||(pVars->hHallTime>=(pVars->hLastHallTime*4))*/))
               //     ||(pVars->bSpeed == pVars->bNegative)||(Global->bRunning == 0))
               if((((++pVars->bHallKeepTime2>50)||(pVars->bNegativeCounter>17))&&(((pVars->hHallTime>=pVars->hLastHallTime))/*||(pVars->hHallTime>=(pVars->hLastHallTime*4))*/))
                    ||(pVars->bSpeed ==NEGATIVE)||(Global->bRunning == 0))  
                {
                    pVars->bHallKeepTime2=0;

                    pVars->bUpdateHallFlag = 1;

                   // pVars->bSpeed = pVars->bNegative;
                    pVars->bSpeed = NEGATIVE;

                    if(pVars->bNegativeCounter<18)
                        pVars->bNegativeCounter++;

                    if(pVars->bPositiveCounter>0)
                        pVars->bPositiveCounter--;

                    if(pVars->hHallTime>200)
                        pVars->bNegativeCounter=1;
                }
            }
        }
        else
        {
            if((pVars->hHallTime>20)&&(++pVars->bHallKeepTime3>50))
            {
                uint8_t bHallTemp1;
                uint8_t bHallTemp2;

                pVars->bHallKeepTime3 = 0;

                bHallTemp1 = HALL_CwSeq[pVars->bHallPositiveState];
                bHallTemp2 = HALL_CcwSeq[pVars->bHallNegativeState];

                if(pVars->bHallNewState == bHallTemp1)
                {
                    pVars->bUpdateHallFlag = 1;
                   // pVars->bSpeed = pVars->bPositive;
                    pVars->bSpeed = POSITIVE;
                }
                else if(pVars->bHallNewState == bHallTemp2)
                {
                    pVars->bUpdateHallFlag = 1;
                  //  pVars->bSpeed = pVars->bNegative;
                     pVars->bSpeed = NEGATIVE;
                }
                else if(pVars->hHallTime >pVars->hLastHallTime)
                {
                    pVars->bUpdateHallFlag = 1;
                }

				if(pVars->bNegativeCounter > 0)
                    pVars->bNegativeCounter--;

                if(pVars->bPositiveCounter > 0)
                    pVars->bPositiveCounter--;

                bHallDirecChange = 1;
            }
        }
    }
    #endif

	
    if(pVars->bUpdateHallFlag != 0)
    {
        uint8_t err;

        err = pVars->bHallNewState ^ pVars->bHallRunState;
        
        if(err & 0x01)
            pVars->hHallATime=0;
        else if(err & 0x02)
            pVars->hHallBTime=0;
        else if(err & 0x04)
            pVars->hHallCTime=0;
        
        pVars->bHallLastState = pVars->bHallRunState;
        pVars->bHallRunState = pVars->bHallNewState;

		if(pVars->bPositive == POSITIVE)//正码
		{
        	pVars->bHallPositiveState = HALL_CwSeq[pVars->bHallNewState];
        	pVars->bHallNegativeState = HALL_CcwSeq[pVars->bHallNewState];
		}
		else
		{
			pVars->bHallPositiveState = HALL_CcwSeq[pVars->bHallNewState];
        	pVars->bHallNegativeState = HALL_CwSeq[pVars->bHallNewState];
		}

		pVars->hRemainderElAngle = (uint16_t)pVars->wDeltaElAngle;

#ifdef LINEAR_HALL//191205
		hCaptureDelta = 0;
#else
		if(pVars->bCaptureFlag != 0)
		    hCaptureDelta = (TIM_GetCounter(TIMx)*pVars->hAvrgElSpeed)/HALL_PWM_RATIO;
		else
		    hCaptureDelta = 0;
#endif

		bHallDirecFlag = pVars->bPositive * pVars->bSpeed;//乘法或下面的判断，二选一


		h30PhaseShift = S16_30_PHASE_SHIFT * bHallDirecFlag;
		pVars->hMeasuredElAngle = (int16_t)(S16_PHASE_SHIFT_TAB[pVars->bHallRunState] + pVars->hPhaseShift - h30PhaseShift);
//		pVars->wDeltaElAngle = S16_60_PHASE_SHIFT + pVars->hRemainderElAngle + hCaptureDelta;

	    //----角度强拉----//190709
//	    pVars->hElAngle = pVars->hMeasuredElAngle + (hCaptureDelta * bHallDirecFlag);//一次性强拉到hall边沿对应的角度（再加上捕获到当前时刻的补偿角度）
		pVars->bHallAdjustCnt = 0;
		pVars->wDeltaElAngle = S16_60_PHASE_SHIFT;
	    //----角度强拉----//190709

	    //----角度分多次强拉----//190712
	    pVars->hHallAdjustAngle = (int16_t)(pVars->hMeasuredElAngle + (hCaptureDelta * bHallDirecFlag) - pVars->hElAngle);//差值

	    if(pVars->hHallTime > 100)
	    {
            pVars->bHallAdjustCnt = 16;
            pVars->hHallAdjustAngle = (pVars->hHallAdjustAngle>>4);// * bHallDirecFlag;//差值分为16次补偿
	    }
	    else if(pVars->hHallTime > 40)
        {
            pVars->bHallAdjustCnt = 8;
            pVars->hHallAdjustAngle = (pVars->hHallAdjustAngle>>3);// * bHallDirecFlag;//差值分为8次补偿
        }
        else if(pVars->hHallTime > 15)
        {
            pVars->bHallAdjustCnt = 4;
            pVars->hHallAdjustAngle = (pVars->hHallAdjustAngle>>2);// * bHallDirecFlag;//差值分为4次补偿
        }
	    else
	    {
            pVars->bHallAdjustCnt = 2;
            pVars->hHallAdjustAngle = (pVars->hHallAdjustAngle>>1);// * bHallDirecFlag;//差值分为2次补偿
	    }

		
		
	    //----角度分多次强拉----//190712

		if(pVars->wDeltaElAngle < 0)
			pVars->wDeltaElAngle = 65536 + pVars->wDeltaElAngle;
		
		if(pVars->bHallRunState == 5)
			pVars->bFlag++;


		//---------------------------------------------------------------------------------------------------
		#ifdef SIX_STEP_START
        #ifndef RUN_BLDC_ANGLE
		if(((pVars->bPositiveCounter < 6)&&(pVars->bNegativeCounter < 6))||
		        (bHallDirecChange != 0))
        #endif
        {
			pVars->hElAngle = (int16_t)S16_PHASE_SHIFT_TAB[pVars->bHallRunState] + pVars->hTheoryAngleShift;
			pVars->wDeltaElAngle = 0;
			pVars->bHallAdjustCnt = 0;//190710
        }
        #endif
		//---------------------------------------------------------------------------------------------------	

        wHallTimeConv = pVars->hHallTime*HALL_PWM_RATIO;

#ifdef LINEAR_HALL//191205
				wHallPeriod = wHallTimeConv;
#else
        if((wHallTimeConv > (pVars->wSensorCapture + SENSOR_5_DELTA_PERIOD))||
                ((wHallTimeConv + SENSOR_5_DELTA_PERIOD)< pVars->wSensorCapture)||
                (pVars->bCaptureFlag == 0))
        {
            wHallPeriod = wHallTimeConv;
        }
        else
        {
            wHallPeriod = pVars->wSensorCapture;
        }
#endif

        pVars->wSensorPeriod[pVars->bHallRunState] = wHallPeriod;

        if((pVars->bPositiveCounter>7)||(pVars->bNegativeCounter>7))
        {
            pVars->wSensorCaptureSum = pVars->wSensorPeriod[1] +pVars->wSensorPeriod[2]
                +pVars->wSensorPeriod[3] +pVars->wSensorPeriod[4] +pVars->wSensorPeriod[5]+pVars->wSensorPeriod[6];
        }
        else
        {
            pVars->wSensorCaptureSum = wHallPeriod*6;
        }

        pVars->hHallTimeSum = pVars->wSensorCaptureSum/HALL_PWM_RATIO;
//        pVars->hAvrgElSpeed = (int16_t)((int32_t)((pParams->wPseudoFreqConv*(65536+pVars->hRemainderElAngle))/pVars->wSensorCaptureSum));
        pVars->hAvrgElSpeed = (int16_t)((int32_t)((pParams->wPseudoFreqConv*(65536))/pVars->wSensorCaptureSum));
		
        //----//190809
        if(((pVars->bPositiveCounter < 12)&&(pVars->bNegativeCounter < 12))||
                    (bHallDirecChange != 0))
        {
            hAvrgHallTimeSum = (int16_t)(pVars->hHallTime + pVars->hLastHallTime)*3;
        }
        else
        {
            hAvrgHallTimeSum = (int16_t)pVars->hHallTimeSum;
        }
        pVars->hAvrgHallTimeSum = hAvrgHallTimeSum * bHallDirecFlag;
        //----//190809

        if((pVars->bPositiveCounter<12)&&(pVars->bNegativeCounter<12))
        {
            if(pVars->hAvrgElSpeed > (pVars->hAvrgElSpeedBak+40))
                pVars->hAvrgElSpeed = pVars->hAvrgElSpeedBak+40;

            if(pVars->hAvrgElSpeed < 28)
                pVars->hAvrgElSpeed = 28;
            else if(pVars->hAvrgElSpeed > 340)
                pVars->hAvrgElSpeed = 340;
        }
	
        pVars->hAvrgElSpeedBak = pVars->hAvrgElSpeed;

        pVars->hElSpeedDpp = pVars->hAvrgElSpeed * bHallDirecFlag;
        pVars->hLastHallTime = pVars->hHallTime;
        pVars->hHallTime = 0;

        if(pVars->bCaptureFlag == 0)
        {
            pVars->hOVFCounter = 0u;
            TIM_SetCounter(TIMx,(u16)(0));
        }
        else
            pVars->bCaptureFlag = 0;

        pVars->bDirectionSwap = 0;
				pVars->bUpdateHallFlag = 0;
				pVars->bSixStepMode=0;
    }
	else if(pVars->hHallTime > RUN_BLDC_SPEED_TIME)
	{

		if(pVars->bSixStepMode==0)
		{
			pVars->bSixStepMode=1;
			pVars->hElAngle = (int16_t)S16_PHASE_SHIFT_TAB[pVars->bHallRunState] + pVars->hTheoryAngleShift;
			pVars->wDeltaElAngle = 0;
			pVars->bHallAdjustCnt = 0;//190710
		}

	}

    //----//190809
    if((pVars->hHallTime<<1) > (pVars->hLastHallTime*3))
    {
        hAvrgHallTimeSum = (int16_t)(pVars->hHallTime * 6);
        pVars->hAvrgHallTimeSum = hAvrgHallTimeSum * pVars->bSpeed * pVars->bPositive;
    }
    //----//190809

    if(pVars->bHallAdjustCnt > 0)//190710
    {
        pVars->bHallAdjustCnt --;
        pVars->hElAngle += pVars->hHallAdjustAngle;
    }
	
    if(pVars->wDeltaElAngle > pVars->hAvrgElSpeed)
    {
        pVars->hElAngle += pVars->hElSpeedDpp;
        pVars->wDeltaElAngle -= pVars->hAvrgElSpeed;
    }
    else
    {
		pVars->hElAngle += (pVars->wDeltaElAngle*pVars->bSpeed*pVars->bPositive);
		pVars->wDeltaElAngle = 0;
    }
}

/*******************************************************************************
* Function Name  : HALL_GetElAngle
* Description    : 获取当前电角度
* parameters     : this HALLpool结构体指针
* Return         : pVars->hElAngle 当前电角度
*******************************************************************************/
int16_t HALL_GetElAngle(pHALL_t this)
{
    pHALLVars_t pVars = &(this->Vars);
   
    return (pVars->hElAngle);

}
    int16_t hMecSpeed01Hzltmp;
 int16_t hMecSpeed01Hzltmp1;  
 int16_t hMecSpeed01Hzltmp2; 
/*******************************************************************************
* Function Name  : HALL_GetAvrgMecSpeed01Hz
* Description    : 获取当前电机本体转速（不考虑减速器）
* parameters     : this HALLpool结构体指针
* Return         : hMecSpeed01Hz 当前电机本体转速
*******************************************************************************/
int16_t HALL_GetAvrgMecSpeed01Hz(pHALL_t this)
{
    pHALLVars_t pVars = &(this->Vars);

  
    
    //hMecSpeed01Hz = (int16_t)((pVars->bPositive *  (int32_t)HALL_MEC_RATIO * 100)/ (pVars->hAvrgHallTimeSum));//扩大100倍

	hMecSpeed01Hzltmp = (int16_t)((pVars->hElSpeedDpp *pVars->bPositive*  (int32_t)60)/ (MEC_RATIO));//60
   // hMecSpeed01Hz = (hMecSpeed01Hz*3)>>1;	
    //if((hMecSpeed01Hz > -30)&&(hMecSpeed01Hz < 30))
    //    hMecSpeed01Hz = 0;
  hMecSpeed01Hzltmp1 =(int32_t)(pVars->bPositive*pVars->hElSpeedDpp *60*PWM_FREQUENCY)/(65536*(MEC_RATIO));//60
  hMecSpeed01Hzltmp2  =  (int32_t)hMecSpeed01Hzltmp1*10/60;
	return(hMecSpeed01Hzltmp);
}

/*******************************************************************************
* Function Name  : HALL_GetAvrgMecSpeed01Hz
* Description    : 获取当前电机本体转速（不考虑减速器）
* parameters     : this HALLpool结构体指针
* Return         : hMecSpeed01Hz 当前电机本体转速
*******************************************************************************/
uint16_t HALL_GetHallTime(pHALL_t this)
{
    pHALLVars_t pVars = &(this->Vars);

    uint16_t hSpeedTime;
    
    hSpeedTime = pVars->hHallTime;
    
    return(hSpeedTime);
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : TIM2 M1 HALL捕获中断
* parameters     : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{

    uint32_t wCaptBuf;
    pHALLVars_t pVars = &(oHall[M2]->Vars);//lee

    if (TIM_GetFlagStatus(TIM2, TIM_FLAG_CC1) == SET)//判断是捕获中断还是溢出更新中断
    {
        wCaptBuf = (u32)TIM_GetCapture1(TIM2) + (u32)(0x10000*pVars->hOVFCounter);

        if(wCaptBuf>750) //375  Capt太小认为是无效信号
        {
            pVars->bCaptureFlag = 1u;
            pVars->wSensorCapture = wCaptBuf;

            /* Reset the number of overflow occurred */
            pVars->hOVFCounter = 0u;

            //pVars->bCaptureHall = HALL_GetPortsVal(oHall[M1]);
        }
        else if(pVars->hHallTime>=3)//5 Capt太小认为是无效信号
        {
            pVars->bCaptureFlag = 1u;  
            pVars->wSensorCapture = pVars->hHallTime*HALL_PWM_RATIO;
            pVars->hOVFCounter=0u;

            //pVars->bCaptureHall = HALL_GetPortsVal(oHall[M1]);
        }
        else
        {
            TIM_SetCounter(TIM2,(u16)(TIM_GetCapture1(TIM2) + TIM_GetCounter(TIM2))); 			
        }
    }
    else
    {
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
        // an update event occured for this interrupt request generation
        if(pVars->hOVFCounter < U16_MAX)
        {
            pVars->hOVFCounter++;
        }
        else
        {
            pVars->hOVFCounter = 0u;
        }
    }
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : TIM4 M2 HALL捕获中断
* parameters     : None
* Return         : None
*******************************************************************************/
#if 0//def DOUBLE_DRIVER
void TIM4_IRQHandler(void)
{

    uint32_t wCaptBuf;
    pHALLVars_t pVars = &(oHall[M1]->Vars);


    if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC1) == SET)//判断是捕获中断还是溢出更新中断
    {
        wCaptBuf = (u32)TIM_GetCapture1(TIM4) + (u32)(0x10000*pVars->hOVFCounter);

        if(wCaptBuf>750) //375 Capt太小认为是无效信号
        {

            pVars->bCaptureFlag = 1u;
            pVars->wSensorCapture = wCaptBuf;

            /* Reset the number of overflow occurred */
            pVars->hOVFCounter = 0u;
 
        }
        else if(pVars->hHallTime>=3)//5 Capt太小认为是无效信号
        {   
            pVars->bCaptureFlag = 1u; 
            pVars->wSensorCapture = pVars->hHallTime*HALL_PWM_RATIO;
            pVars->hOVFCounter=0u;       
        } 
        else // 
        {
            TIM_SetCounter(TIM4,(u16)(TIM_GetCapture1(TIM4) + TIM_GetCounter(TIM4)));                
        }
        
    }
    else
    {
        
        TIM_ClearFlag(TIM4, TIM_FLAG_Update);  
        // an update event occured for this interrupt request generation
        if(pVars->hOVFCounter < U16_MAX)
        {
            pVars->hOVFCounter++;
        }
        else
        { 
            pVars->hOVFCounter = 0u;
        }
    }
}
#endif

/*******************************************************************************
* Function Name  : Find_TheoryAngleShift
* Description    : 根据偏差角度，计算相应的理论上偏差角度
* parameters     : None
* Return         : None
*******************************************************************************/
int16_t Find_TheoryAngleShift(int16_t hAngleShift)
{
    int16_t hIndex;
    int32_t wAngle1,wAngle2,wAngle3;

    if(hAngleShift < 0)
    {
        wAngle1 = 65536 + hAngleShift;
    }
    else
    {
        wAngle1 = hAngleShift;
    }

    hIndex = wAngle1/S16_60_PHASE_SHIFT;

    wAngle2 = S16_60_PHASE_SHIFT * hIndex;//下限
    wAngle3 = wAngle2 + S16_60_PHASE_SHIFT;//上限

    if((wAngle3 - wAngle1) > (wAngle1 - wAngle2))//更靠近下限？
    {
        wAngle1 = wAngle2;
    }
    else
    {
        wAngle1 = wAngle3;
    }

    if(wAngle1 > 32767)
        wAngle1 -= 65536;

    return((int16_t)wAngle1);
}


#endif

/**************************************************************END OF FILE****/
