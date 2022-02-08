/******************************************************************************
* File Name         :  PWMnCurrentFdbk.c
* Author            :  锟斤拷锟脚凤拷
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  PWM锟酵碉拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 锟斤拷锟脚凤拷             19/01/14      1.0               锟斤拷锟斤拷
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





/* ADC1 Data Register address */
#define ADC1_DR_Address     0x4001244Cu	
#define SMPR1_SMP_Set              ((uint32_t) (0x00000007u)) 	/* ADC SMPx mask */
#define SMPR2_SMP_Set              ((uint32_t) (0x00000007u))

#define CR2_JEXTTRIG_Reset      ((uint32_t)0xFFFF7FFFu)
#define AUX_CR2_JEXTTRIG 0x001E1901u
	
#define CCMR2_CH4_DISABLE 0x8FFFu
#define CCMR2_CH4_PWM1    0x6000u
#define CCMR2_CH4_PWM2    0x7000u

#define NB_CONVERSIONS 16u

#define PHASE_OFFSET_MIN_LIMIT (uint16_t)(30781)		
#define PHASE_OFFSET_MAX_LIMIT (uint16_t)(34753)		
#define PHASE_OFFSET_DELTA_LIMIT (uint16_t)(2000)	
#define ZERO_OFFSET_MAX_LIMIT  (uint16_t)(29551)
#define ZERO_OFFSET_MIN_LIMIT  (uint16_t)0//(25579)//1.388v=27567,+-/0.1v//xu-190627

#define T		    (uint16_t)(PWM_PERIOD * 4)
#define T_SQRT3     (uint16_t)(T * SQRT_3)

#define SECTOR_1	(uint32_t)1
#define SECTOR_2	(uint32_t)2
#define SECTOR_3	(uint32_t)3
#define SECTOR_4	(uint32_t)4
#define SECTOR_5	(uint32_t)5
#define SECTOR_6	(uint32_t)6

#define DEADTIME_SET 35

#define CONV_STARTED               ((uint32_t) (0x8))
#define CONV_FINISHED              ((uint32_t) (0xC))
#define FLAGS_CLEARED              ((uint32_t) (0x0))
#define ADC_SR_MASK                ((uint32_t) (0xC))

#define TIMx_CC4E_BIT              ((uint16_t)  0x1000u)



Phase_Step_t MosfetCheckSteps[5] =
{
  {(CCMR_A_LOWSIDE|CCMR_B_LOWSIDE), (CCMR_C_LOWSIDE|CCMR_D_PWM), (A_COMP|B_COMP|C_COMP|D_ON)},   //A-Lo, B-Lo, C-Lo 
	{(CCMR_A_HIGHSIDE|CCMR_B_HIGHSIDE), (CCMR_C_HIGHSIDE|CCMR_D_PWM), (A_COMP|B_COMP|C_COMP|D_ON)},//A-Hi, B-Hi, C-Hi 
	{(CCMR_A_HIGHSIDE|CCMR_B_LOWSIDE), (CCMR_C_PWM|CCMR_D_PWM), (A_COMP|B_COMP|C_OFF|D_ON)},       //A-Hi, B-Lo, C-Idle
	{(CCMR_A_PWM|CCMR_B_HIGHSIDE), (CCMR_C_LOWSIDE|CCMR_D_PWM), (A_OFF|B_COMP|C_COMP|D_ON)},       //B-Hi, C-Lo, A-Idle
	{(CCMR_A_LOWSIDE|CCMR_B_PWM), (CCMR_C_HIGHSIDE|CCMR_D_PWM), (A_COMP|B_OFF|C_COMP|D_ON)}        //C-Hi, A-Lo, B-Idle 
};


PWMCParams_t PWMCParamsM1 =
{
    1,  
	#if (PHASEMATCH == PHASEMATCH1)
    ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,         
    ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1,  
    ADC_Channel_4,                 
    GPIOA,               
    GPIO_Pin_4,    
    #elif (PHASEMATCH == PHASEMATCH2)
	ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,   
    ADC_Channel_2,                 
    GPIOA,               
    GPIO_Pin_2,  
    ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1,  
	#elif (PHASEMATCH == PHASEMATCH3)
	ADC_Channel_2,                 
    GPIOA,               
    GPIO_Pin_2,         
    ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1, 
    ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,      
	#elif (PHASEMATCH == PHASEMATCH4)
	ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1,  
	ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,   
    ADC_Channel_2,                 
    GPIOA,               
    GPIO_Pin_2,  
	#elif (PHASEMATCH == PHASEMATCH5)	     
    ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1,  
    ADC_Channel_2,                 
    GPIOA,               
    GPIO_Pin_2,  
    ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,    
	#elif (PHASEMATCH == PHASEMATCH6)
	ADC_Channel_2,                 
    GPIOA,               
    GPIO_Pin_2,    
	ADC_Channel_0,                  
    GPIOA,              
    GPIO_Pin_0,         
    ADC_Channel_1,                 
    GPIOA,               
    GPIO_Pin_1,      
	#endif
    TIM1,      
    GPIOA,                
    GPIO_Pin_8,       
    GPIOA,                       
    GPIO_Pin_9,             	
    GPIOA,                            
    GPIO_Pin_10,   
    GPIOB,                          
    GPIO_Pin_13,    
    GPIOB,                      
    GPIO_Pin_14,  
    GPIOB,                       
    GPIO_Pin_15, 
    GPIOB,            
    GPIO_Pin_12,      
    ADC_Channel_4,
    TIM1_UP_IRQn,
    TIM1_BRK_IRQn,
};


#ifdef DOUBLE_DRIVER 
PWMCParams_t PWMCParamsM2 = 
{
    2, 
    #if(PHASEMATCH == PHASEMATCH1)
    ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5,         
    ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,  
    ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1,
    #elif(PHASEMATCH == PHASEMATCH2)
	ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5,    
    ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1,
    ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,  
	#elif(PHASEMATCH == PHASEMATCH3)
	ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1,      
    ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,  
    ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5,     
	#elif(PHASEMATCH == PHASEMATCH4)
	ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,  
	ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5, 
    ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1,
	#elif(PHASEMATCH == PHASEMATCH5)	     
    ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,  
    ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1,
    ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5,   
	#elif(PHASEMATCH == PHASEMATCH6)
	ADC_Channel_11,                  
    GPIOC,                
    GPIO_Pin_1, 
	ADC_Channel_5,                  
    GPIOA,                  
    GPIO_Pin_5,      
    ADC_Channel_10,                
    GPIOC,                
    GPIO_Pin_0,      
	#endif
    TIM8,        
    GPIOC,                 
    GPIO_Pin_6,  
    GPIOC,                            
    GPIO_Pin_7,   
    GPIOC,                          
    GPIO_Pin_8,
    GPIOA,                                
    GPIO_Pin_7,  
    GPIOB,                               
    GPIO_Pin_0,   
    GPIOB,                       
    GPIO_Pin_1,                   
    GPIOA,            
    GPIO_Pin_6,   
    ADC_Channel_13,
    TIM8_UP_IRQn,
    TIM8_BRK_IRQn,
};

#endif


#ifdef DOUBLE_DRIVER 
uint16_t hPhaseOffsetBak[6]=
{
	32768,32768,32768,
	32768,32768,32768,
}; 
#else
uint16_t hPhaseOffsetBak[3]=
{
	32768,32768,32768,	
}; 
#endif



static void PWMC_Config(pPWMC_t this);
static void TIMxInit(TIM_TypeDef* TIMx, pPWMC_t this);
static void ADC_ExternalTrigTimerInit(void);
static void StartTimers(void);
static void PWMC_InjectedConvConfig(pPWMC_t this);

PWMC_t PWMCpool[NBR_OF_MOTORS];
 
uint16_t hRegConv[3]; 
uint8_t bFocFlag=0;



/*******************************************************************************
* Function Name  : DelayUs
* Description    : us锟斤拷锟斤拷时锟斤拷锟斤拷支锟街撅拷确锟接迟★拷
* parameters     : i 锟斤拷示锟接迟讹拷锟斤拷us
* Return         : None
*******************************************************************************/
void DelayUs(u16 i)
{
    u16 us;
    while(i--)
    {
    	us=10;
		while(us--) __NOP();
		
    }
}

/*******************************************************************************
* Function Name  : PWMC_Init
* Description    : 锟较碉拷锟绞硷拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
void PWMC_Init(void)
{
    oPwmc[0] = &PWMCpool[0];    
    oPwmc[0]->pParams = &PWMCParamsM1;
    
    PWMC_Config(oPwmc[0]);

	#ifdef DOUBLE_DIVER
    oPwmc[1] = &PWMCpool[1];
    oPwmc[1]->pParams = &PWMCParamsM2;
    PWMC_Config(oPwmc[1]);
    #endif

    StartTimers();

    #ifdef DOUBLE_DRIVER

	FLASH_Read(FLASH_SAVE_ADDR,(uint16_t*)hPhaseOffsetBak,6); //锟较碉拷锟皆硷拷时锟斤拷取锟斤拷一锟轿憋拷锟斤拷牡锟斤拷锟狡拷锟街�
	
	if(hPhaseOffsetBak[0]>35000 || hPhaseOffsetBak[0]<30000) hPhaseOffsetBak[0] =32768; //锟斤拷FLASH锟斤拷锟斤拷写,锟斤拷锟诫赋默锟较筹拷始值,锟斤拷止锟斤拷偏锟斤拷锟届常锟斤拷锟斤拷墓锟斤拷锟�
	if(hPhaseOffsetBak[1]>35000 || hPhaseOffsetBak[1]<30000) hPhaseOffsetBak[1] =32768;		
	if(hPhaseOffsetBak[2]>35000 || hPhaseOffsetBak[2]<30000) hPhaseOffsetBak[2] =32768;
	if(hPhaseOffsetBak[3]>35000 || hPhaseOffsetBak[3]<30000) hPhaseOffsetBak[3] =32768;
	if(hPhaseOffsetBak[4]>35000 || hPhaseOffsetBak[4]<30000) hPhaseOffsetBak[4] =32768;		
	if(hPhaseOffsetBak[5]>35000 || hPhaseOffsetBak[5]<30000) hPhaseOffsetBak[5] =32768;

	#else

//	FLASH_Read(FLASH_SAVE_ADDR,(uint16_t*)hPhaseOffsetBak,3); //锟较碉拷锟皆硷拷时锟斤拷取锟斤拷一锟轿憋拷锟斤拷牡锟斤拷锟狡拷锟街�
	
	if(hPhaseOffsetBak[0]>35000 || hPhaseOffsetBak[0]<30000) hPhaseOffsetBak[0] =32768; //锟斤拷FLASH锟斤拷锟斤拷写,锟斤拷锟诫赋默锟较筹拷始值,锟斤拷止锟斤拷偏锟斤拷锟届常锟斤拷锟斤拷墓锟斤拷锟�
	if(hPhaseOffsetBak[1]>35000 || hPhaseOffsetBak[1]<30000) hPhaseOffsetBak[1] =32768;		
	if(hPhaseOffsetBak[2]>35000 || hPhaseOffsetBak[2]<30000) hPhaseOffsetBak[2] =32768;
	
	#endif
   
}


/*******************************************************************************
* Function Name  : PWMC_Config
* Description    : PWMC锟斤拷锟絀C锟斤拷锟斤拷模锟斤拷锟绞硷拷锟�
* parameters     : this  PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
static void PWMC_Config(pPWMC_t this)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;    
    uint32_t wAux1, wAux2, wAux3, wAux4;
	uint16_t hAux;

    pPWMCVars_t pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;    
    TIM_TypeDef* TIMx = pParams->TIMx;    

    pVars->Half_PWMPeriod = (PWM_PERIOD_CYCLES / 2u);

    /* Peripheral clocks enabling ---------------------------------------------*/

    RCC->AHBENR |= RCC_AHBPeriph_CRC;

    /* ADCCLK = PCLK2 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* Enable ADC2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* Enable the CCS */
    RCC_ClockSecuritySystemCmd((FunctionalState)(ENABLE));

    #ifdef DOUBLE_DRIVER
    if(TIMx == TIM1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        pVars->wADCTriggerSet =   0x989901u;// 0x189901u
        pVars->wADCTriggerUnSet = 0x981901u;// 0x181901u
        pVars->wTIMxCH4_BB_Addr = 0x42258430u;
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
        pVars->wADCTriggerSet =   0x9EE901u;// 0x18E901u  0x98E901u(T3_TRO)
        pVars->wADCTriggerUnSet = 0x986901u;// 0x186901u
        pVars->wTIMxCH4_BB_Addr = 0x42268430u;
    }
	#else
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    pVars->wADCTriggerSet =   0x9E9901u;// 0x18E901u  0x98E901u(T3_TRO)
    pVars->wADCTriggerUnSet = 0x981901u;// 0x181901u
    pVars->wTIMxCH4_BB_Addr = 0x42258430u;
    
//	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//    pVars->wADCTriggerSet =   0x989901u;// 0x189901u
//    pVars->wADCTriggerUnSet = 0x981901u;// 0x181901u
//    pVars->wTIMxCH4_BB_Addr = 0x42258430u;    
	#endif

    /* GPIOs configurations --------------------------------------------------*/
    GPIO_StructInit(&GPIO_InitStructure);

    /****** Configure phase A ADC channel GPIO as analog input ****/    
    GPIO_InitStructure.GPIO_Pin = pParams->hIaPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(pParams->hIaPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hIaPort, pParams->hIaPin);

    /****** Configure phase B ADC channel GPIO as analog input ****/
    GPIO_InitStructure.GPIO_Pin = pParams->hIbPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(pParams->hIbPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hIbPort, pParams->hIbPin);

    /****** Configure phase C ADC channel GPIO as analog input ****/
    GPIO_InitStructure.GPIO_Pin = pParams->hIcPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(pParams->hIcPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hIcPort, pParams->hIcPin);

	TIMxInit(TIMx, this);
	
    /****** Configure TIMx Channel 1, 2 and 3 Outputs ******/ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    hAux = (pParams->hCh1Pin) | (pParams->hCh2Pin);
    hAux = hAux | (pParams->hCh3Pin);
    GPIO_InitStructure.GPIO_Pin = hAux;
    GPIO_Init(pParams->hCh1Port, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hCh1Port, hAux);

    /****** Configure TIMx Channel 1N, 2N and 3N Outputs, if enabled ******/   
    GPIO_InitStructure.GPIO_Pin = pParams->hCh1NPin;
    GPIO_Init(pParams->hCh1NPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hCh1NPort, pParams->hCh1NPin);

    hAux = (pParams->hCh2NPin) | (pParams->hCh3NPin);
    GPIO_InitStructure.GPIO_Pin = hAux;
    GPIO_Init(pParams->hCh2NPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hCh2NPort, hAux);
   
    /****** Configure TIMx BKIN input, if enabled ******/   
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = pParams->hBKINPin;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING
    GPIO_Init(pParams->hBKINPort, &GPIO_InitStructure);
    GPIO_PinLockConfig(pParams->hBKINPort, pParams->hBKINPin);    
   
    /* It saves the sampling time settings before DeInit in case of second instance */  
    wAux1 = ADC1->SMPR1;
    wAux2 = ADC1->SMPR2;
    wAux3 = ADC2->SMPR1;
    wAux4 = ADC2->SMPR2;

    /* ADC1 and ADC2 registers configuration ---------------------------------*/
    /* ADC1 and ADC2 registers reset */  
    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    //ADC_DeInit(ADC3);

    /* ADC1 and ADC2 registers configuration ---------------------------------*/
    /* Enable ADC1 and ADC2 */
    ADC_Cmd(ADC1, ENABLE);
    ADC_Cmd(ADC2, ENABLE);
    //ADC_Cmd(ADC3, ENABLE);


    ADC1->SMPR1 = wAux1;
    ADC1->SMPR2 = wAux2;
    ADC2->SMPR1 = wAux3;
    ADC2->SMPR2 = wAux4;

    /* ADC Init */
    /* ADC1 */
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ADC_ExternalTrigConv_T3_TRGO
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
    ADC_InitStructure.ADC_NbrOfChannel = 3u;//4u
    ADC_Init(ADC1, &ADC_InitStructure);

	ADC_TempSensorVrefintCmd(ENABLE); 			//使锟斤拷锟铰度达拷锟斤拷锟斤拷通锟斤拷
   // ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );    //L Avg  IBus  
   // ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_239Cycles5 );   //Power
   // ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 2, ADC_SampleTime_239Cycles5 );   //R  Avg  IBus 	
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5 );   //Power
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 2, ADC_SampleTime_239Cycles5 );   //Battery
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 3, ADC_SampleTime_239Cycles5 );   //MCU Temp

   // ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 6,  ADC_SampleTime_239Cycles5 ); //M1 Temp   
	//	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 7, ADC_SampleTime_239Cycles5 ); //M2 Temp  
 
    ADC_InjectedDiscModeCmd(ADC1, ENABLE);
    ADC_InjectedDiscModeCmd(ADC2, ENABLE);

    /* Enable external trigger (it will be SW) for ADC1 and ADC2 regular 
    conversions */ 
   // ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
   
    /* Remap of Eternal trigger of ADC1 and 2 into TIM8 CH4 */
    GPIO_PinRemapConfig(GPIO_Remap_ADC1_ETRGINJ, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_ADC2_ETRGINJ, ENABLE);

    /* Start calibration of ADC1 and ADC2 */
    //ADC_StartCalibration(ADC1);
    //ADC_StartCalibration(ADC2);
		
   ADC_ResetCalibration(ADC1);                    //校锟介复位
   while(ADC_GetResetCalibrationStatus(ADC1));    //锟饺达拷锟斤拷位锟斤拷锟�
   ADC_StartCalibration(ADC1);                    //锟斤拷始ADC1校准
  
   ADC_ResetCalibration(ADC2);                    //校锟介复位
   while(ADC_GetResetCalibrationStatus(ADC2));    //锟饺达拷锟斤拷位锟斤拷锟�
   ADC_StartCalibration(ADC2);                    //锟斤拷始ADC1校准

    /* Wait for the end of ADCs calibration */
    while (ADC_GetCalibrationStatus(ADC1) & ADC_GetCalibrationStatus(ADC2))
    {
    }

    /* It is used only to configure the sampling time to the corresponding channel*/
    ADC_InjectedChannelConfig(ADC1, pParams->bIaChannel, 1u, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC1, pParams->bIbChannel, 1u, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC1, pParams->bIcChannel, 1u, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC2, pParams->bIaChannel, 1u, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC2, pParams->bIbChannel, 1u, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC2, pParams->bIcChannel, 1u, ADC_SampleTime_7Cycles5);

    /* DMA Event related to ADC regular conversion*/
    /* DMA1 channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&hRegConv[0]);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3u;//1u; 4u
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* Enable DMA1 Channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    /* Enable ADC1 EOC DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* ADC1 Injected conversions end interrupt enabling */
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    /* Enable the ADC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = (uint8_t) ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC_PRE_EMPTION_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADC_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = (uint8_t) pParams->TIMx_UP_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_UP_PRE_EMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_UP_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  		

	NVIC_InitStructure.NVIC_IRQChannel = pParams->TIMx_BRK_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_BRK_PRE_EMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_BRK_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DBGMCU_Config(DBGMCU_TIM1_STOP, ENABLE);

	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
  	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

/*******************************************************************************
* Function Name  : TIMxInit
* Description    : PWM锟斤拷时锟斤拷锟斤拷始锟斤拷
* parameters     : TIMx锟斤拷时锟斤拷锟斤拷锟斤拷 this      PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
static void TIMxInit(TIM_TypeDef* TIMx, pPWMC_t this)
{  
    TIM_TimeBaseInitTypeDef TIMx_TimeBaseStructure;
    TIM_OCInitTypeDef TIMx_OCInitStructure;
    TIM_BDTRInitTypeDef TIMx_BDTRInitStructure;
    pPWMCVars_t pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;    

    /* TIMx Peripheral Configuration -------------------------------------------*/
    /* TIMx Registers reset */
    TIM_DeInit(TIMx);
    TIM_TimeBaseStructInit(&TIMx_TimeBaseStructure);
    /* Time Base configuration */
    TIMx_TimeBaseStructure.TIM_Prescaler = 0;
    TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIMx_TimeBaseStructure.TIM_Period = pVars->Half_PWMPeriod;
    TIMx_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
    TIMx_TimeBaseStructure.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TIMx, &TIMx_TimeBaseStructure);

    /* Channel 1, 2, 3 and 4 Configuration in PWM mode */
    TIM_OCStructInit(&TIMx_OCInitStructure);  
    TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIMx_OCInitStructure.TIM_Pulse = pVars->Half_PWMPeriod / 2u; /* dummy value */

    /* Channel 1, 2, 3 */
    TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
    TIMx_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;    

    TIMx_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
    #ifdef LowEffect
    TIMx_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low; //锟斤拷锟斤拷IC 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通; 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通  TIM_OCNPolarity_High  锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟脚碉拷锟斤拷效 TIM_OCNPolarity_Low
    TIMx_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;  //锟斤拷锟斤拷IC 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通; 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通 TIM_OCNIdleState_Reset  锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟脚碉拷锟斤拷效 TIM_OCNIdleState_Set
    #else
    TIMx_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //锟斤拷锟斤拷IC 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通; 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通  TIM_OCNPolarity_High  锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟脚碉拷锟斤拷效 TIM_OCNPolarity_Low
    TIMx_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  //锟斤拷锟斤拷IC 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通; 锟斤拷锟脚革拷锟斤拷效, 锟酵碉拷平锟斤拷锟斤拷通 TIM_OCNIdleState_Reset  锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟脚碉拷锟斤拷效 TIM_OCNIdleState_Set
    #endif
    TIM_OC1Init(TIMx, &TIMx_OCInitStructure); 
    TIM_OC2Init(TIMx, &TIMx_OCInitStructure); 
    TIM_OC3Init(TIMx, &TIMx_OCInitStructure);   

	TIM_CtrlPWMOutputs(TIMx, DISABLE);

	/* Channel 4 */
	//------------------------------------------------------------------------

    TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;// TIM_OutputState_Disable;
    TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIMx_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIMx_OCInitStructure.TIM_Pulse = pVars->Half_PWMPeriod - 5u;

    TIM_OC4Init(TIMx, &TIMx_OCInitStructure);
	//-----------------------------------------------------------------------
	
    /* Enables the TIMx Preload on CC1 Register */
    TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    /* Enables the TIMx Preload on CC2 Register */
    TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
    /* Enables the TIMx Preload on CC3 Register */
    TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
    /* Enables the TIMx Preload on CC4 Register */
    TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);

    TIM_BDTRStructInit(&TIMx_BDTRInitStructure);
    /* Dead Time */
    TIMx_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIMx_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIMx_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIMx_BDTRInitStructure.TIM_DeadTime = DEAD_TIME_COUNTS/2u;
    /* BKIN, if enabled */

    TIMx_BDTRInitStructure.TIM_Break = TIM_Break_Enable;//
    TIMx_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//TIM_BreakPolarity_Low
    TIMx_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_ClearITPendingBit(TIMx, TIM_IT_Break);
    TIM_ITConfig(TIMx, TIM_IT_Break, ENABLE);
  
    TIM_BDTRConfig(TIMx, &TIMx_BDTRInitStructure);

    TIM_SelectInputTrigger(TIMx, TIM_TS_ITR1);
    TIM_SelectSlaveMode(TIMx, TIM_SlaveMode_Trigger);

    /* Prepare timer for synchronization */
    TIM_GenerateEvent(TIMx, TIM_EventSource_Update);

	#ifdef DOUBLE_DRIVER
    if (pParams->bInstanceNbr == 1u)
    {
    	//-----------------------------------------------------------------Rep==3
    	TIMx->RCR =0x01u;
        TIM_GenerateEvent(TIMx, TIM_EventSource_Update);
        /* Repetition counter will be set to 3 at next Update */
        TIMx->RCR =0x03u;
    }
    #endif			    
}


/*******************************************************************************
* Function Name  : ADC_ExternalTrigTimerInit
* Description    : 锟斤拷锟斤拷锟斤拷AD锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷始锟斤拷
* parameters     : None
* Return         : None
*******************************************************************************/
static void ADC_ExternalTrigTimerInit(void)
{
    TIM_TimeBaseInitTypeDef TIMx_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_DeInit(TIM3);
    TIM_TimeBaseStructInit(&TIMx_TimeBaseStructure);
   
	TIMx_TimeBaseStructure.TIM_Prescaler = 0;
    TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIMx_TimeBaseStructure.TIM_Period = 2250;
    TIMx_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
    TIMx_TimeBaseStructure.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TIM3, &TIMx_TimeBaseStructure);
	
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Trigger);
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

	//DBGMCU_Config(DBGMCU_TIM3_STOP, ENABLE);
}

/*******************************************************************************
* Function Name  : StartTimers
* Description    : PWM模锟斤拷锟斤拷锟斤拷时锟斤拷同锟斤拷锟斤拷锟斤拷
* parameters     : None
* Return         : None
*******************************************************************************/
static void StartTimers(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  /* Temporary Enable TIM2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0u;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 2u; /* dummy */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
     
  TIM_Cmd(TIM2, ENABLE);
  
//  GPIO_ResetBits(GPIOA,GPIO_Pin_15);

  __NOP();

  TIM_DeInit(TIM2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);

}

/*******************************************************************************
* Function Name  : PWMC_CurrentCalibration
* Description    : PWM锟斤拷锟斤拷锟斤拷锟斤拷锟侥革拷叩锟斤拷锟狡拷锟叫ｏ拷锟�
* parameters     : this  PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
void PWMC_CurrentCalibration(pPWMC_t this)
{
    pPWMCVars_t   pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;    
    TIM_TypeDef*  LocalTIMx = pParams->TIMx;  
    uint8_t i;
    uint16_t hTimeOut;


    if((pVars->bCalibrateCompletionFlag==0)&&(pVars->bOverCurrentFault==0)
        &&(pVars->bMosfetHFault==0)&&(pVars->bMosfetLFault==0))  //锟斤拷锟斤拷锟斤拷锟斤拷      MOS锟斤拷锟斤拷   校锟斤拷锟斤拷锟�
    {
        ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
		ADC_InjectedDiscModeCmd(ADC1, DISABLE);

        pVars->hPhaseAOffset = 0;
        pVars->hPhaseBOffset = 0;
        pVars->hPhaseCOffset = 0;
        pVars->hBusCurrentOffset = 0;

		LocalTIMx->CCMR1 = MosfetCheckSteps[0].CCMR_1;
    	LocalTIMx->CCMR2 = MosfetCheckSteps[0].CCMR_2;
    	LocalTIMx->CCER =  MosfetCheckSteps[0].CCER;
    	LocalTIMx->EGR |= BIT5;

        TIM_ClearFlag(LocalTIMx, TIM_FLAG_Update);
        TIM_CtrlPWMOutputs(LocalTIMx, ENABLE);
			
		ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigConv_None);
//		DMA_ClearFlag(DMA1_FLAG_TC1);
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC1)); 
//		DMA_ClearFlag(DMA1_FLAG_TC1);
		
//		ADC_Cmd(ADC2, DISABLE);
        DelayUs(100);//500

        ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);
		ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

        ADC_InjectedSequencerLengthConfig(ADC1, 4);
		ADC_InjectedChannelConfig(ADC1, pParams->bIaChannel, 1, ADC_SampleTime_28Cycles5);
        ADC_InjectedChannelConfig(ADC1, pParams->bIbChannel, 2, ADC_SampleTime_28Cycles5);
		ADC_InjectedChannelConfig(ADC1, pParams->bIcChannel, 3, ADC_SampleTime_28Cycles5);
		ADC_InjectedChannelConfig(ADC1, pParams->bIbusChannel, 4, ADC_SampleTime_28Cycles5);
		DI();
		ADC_AutoInjectedConvCmd(ADC1, ENABLE);
        /* Clear the ADC1 JEOC pending flag */
		//while( (DMA1_Channel1->CNDTR) != 4){}; //4 锟斤拷锟街革拷位
		//ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigConv_None); // 锟斤拷锟街革拷位
		DelayUs(2);
		 
		ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);	
		ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
		/* ADC Channel used for current reading are read in order to get zero currents ADC values*/
		for(i = 0; i < NB_CONVERSIONS; i++)  
		{
			hTimeOut=0;
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == RESET)
//			while((ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)==RESET)&&(hTimeOut<5000))//20190702
			{
				hTimeOut++;				
				__NOP();
			}
			if(hTimeOut == 5000)
				pVars->bCalibrateOverTimeFlag = 1;
            
			pVars->hPhaseAOffset += (ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1) >> 3);
            pVars->hPhaseBOffset += (ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2) >> 3);
			pVars->hPhaseCOffset += (ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3) >> 3);
			pVars->hBusCurrentOffset += (ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4) >> 3);
			
			ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
			ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
		}

		TIM_CtrlPWMOutputs(LocalTIMx, DISABLE);
		ADC_AutoInjectedConvCmd(ADC1, DISABLE);
		LocalTIMx->CCMR1 = CCMR_A_PWM | CCMR_B_PWM;
		LocalTIMx->CCMR2 = CCMR_C_PWM | CCMR_D_PWM;
		LocalTIMx->EGR |= BIT5;

		ADC_ClearFlag(ADC1, ADC_FLAG_JSTRT);
		ADC_ClearFlag(ADC1, ADC_FLAG_STRT);
//		ADC_Cmd(ADC2, ENABLE);

//		__set_PRIMASK(0);
		EI();
		
		//pVars->hPhaseBOffset = pVars->hPhaseAOffset;

		pVars->hMaxPhaseOffset = GetMaxVal(pVars->hPhaseAOffset, pVars->hPhaseBOffset, pVars->hPhaseCOffset);
		pVars->hMinPhaseOffset = GetMinVal(pVars->hPhaseAOffset, pVars->hPhaseBOffset, pVars->hPhaseCOffset);

		if((pVars->hMaxPhaseOffset > PHASE_OFFSET_MAX_LIMIT)||(pVars->hMinPhaseOffset<PHASE_OFFSET_MIN_LIMIT)
            ||(pVars->hMaxPhaseOffset > (pVars->hMinPhaseOffset+PHASE_OFFSET_DELTA_LIMIT)))
		{
			 pVars->bPhaseOffsetFault = 1;

			#ifdef DOUBLE_DRIVER
		  	if(pParams->TIMx==TIM1) // M1锟斤拷锟斤拷锟斤拷锟狡拷锟斤拷斐ｏ拷锟斤拷没锟斤拷锟揭伙拷蔚锟街�  20191011 jimianhao
      		{
        		pVars->hPhaseAOffset = hPhaseOffsetBak[0];
				pVars->hPhaseBOffset = hPhaseOffsetBak[1];
				pVars->hPhaseCOffset = hPhaseOffsetBak[2];
        	
			}
			else  // M2锟斤拷锟斤拷锟斤拷锟狡拷锟斤拷斐ｏ拷锟斤拷没锟斤拷锟揭伙拷蔚锟街�  20191011 jimianhao
			{
				pVars->hPhaseAOffset = hPhaseOffsetBak[3];
				pVars->hPhaseBOffset = hPhaseOffsetBak[4];
				pVars->hPhaseCOffset = hPhaseOffsetBak[5];
			}
			#else

			pVars->hPhaseAOffset = hPhaseOffsetBak[0];
			pVars->hPhaseBOffset = hPhaseOffsetBak[1];
			pVars->hPhaseCOffset = hPhaseOffsetBak[2];

			#endif					 
		}
		else
		{
			pVars->bPhaseOffsetFault = 0;
			
			if((pVars->hBusCurrentOffset > ZERO_OFFSET_MAX_LIMIT) || (pVars->hBusCurrentOffset < ZERO_OFFSET_MIN_LIMIT))
			{
				pVars->bBusCurrentOffsetFault = 1;
			}
			else
			{
				pVars->bBusCurrentOffsetFault = 0;
//				pVars->bCalibrateCompletionFlag = 1;
                //flash 锟斤拷锟斤拷锟斤拷锟斤拷
			}

			#ifdef DOUBLE_DRIVER
			
			if(pParams->TIMx==TIM1)
			{
				hPhaseOffsetBak[0] = pVars->hPhaseAOffset;
				hPhaseOffsetBak[1] = pVars->hPhaseBOffset; 
				hPhaseOffsetBak[2] = pVars->hPhaseCOffset;

				FLASH_Write(FLASH_SAVE_ADDR,(uint16_t*)hPhaseOffsetBak,6);//锟斤拷锟斤拷M1锟斤拷锟斤拷识锟斤拷锟斤拷锟斤拷锟斤拷偏锟矫诧拷锟斤拷 20191009 jimianhao
			}
			else		 
			{
				hPhaseOffsetBak[3] = pVars->hPhaseAOffset;
				hPhaseOffsetBak[4] = pVars->hPhaseBOffset; 
				hPhaseOffsetBak[5] = pVars->hPhaseCOffset;

				FLASH_Write(FLASH_SAVE_ADDR,(uint16_t*)hPhaseOffsetBak,6);//锟斤拷锟斤拷M2锟斤拷锟斤拷识锟斤拷锟斤拷锟斤拷锟斤拷偏锟矫诧拷锟斤拷 20191009 jimianhao
			}

			#else
			hPhaseOffsetBak[0] = pVars->hPhaseAOffset;
			hPhaseOffsetBak[1] = pVars->hPhaseBOffset; 
			hPhaseOffsetBak[2] = pVars->hPhaseCOffset;

//			FLASH_Write(FLASH_SAVE_ADDR, (uint16_t*)hPhaseOffsetBak, 3);//锟斤拷锟斤拷M1锟斤拷锟斤拷识锟斤拷锟斤拷锟斤拷锟斤拷偏锟矫诧拷锟斤拷
			#endif
		}
		//ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigConv_T3_TRGO);
		ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);//ADC_ExternalTrigConv_T3_TRGO
        PWMC_InjectedConvConfig(this);	

        pVars->bCalibrateCompletionFlag = 1;        
    }      
}

/*******************************************************************************
* Function Name  : PWMC_InjectedConvConfig
* Description    : 注锟斤拷锟斤拷AD锟斤拷锟斤拷
* parameters     : this  PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
void PWMC_InjectedConvConfig(pPWMC_t this)
{    
    pPWMCParams_t pParams = this->pParams;    
    TIM_TypeDef*  LocalTIMx = pParams->TIMx;  
    
	/* ADC1 Injected conversions configuration */ 
	ADC_InjectedSequencerLengthConfig(ADC1,1);
	ADC_InjectedSequencerLengthConfig(ADC2,1);
	
	ADC_InjectedChannelConfig(ADC1, pParams->bIaChannel, 1,  ADC_SampleTime_7Cycles5);
	ADC_InjectedChannelConfig(ADC2, pParams->bIcChannel, 1,  ADC_SampleTime_7Cycles5);

	ADC_InjectedDiscModeCmd(ADC1, ENABLE);	

	
	#ifdef DOUBLE_DRIVER
	
	if(LocalTIMx == TIM1)
	{
	    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);  
	}
    else
    {
        ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4); 
    }
    #else
    
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);  	

    #endif
    
	ADC_ExternalTrigInjectedConvCmd(ADC2,ENABLE);
	
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	
}


/*******************************************************************************
* Function Name  : PWMC_SetPhaseVoltage
* Description    : 锟斤拷Valfa,Vbelta转锟斤拷为PWM锟斤拷锟斤拷锟斤拷PWM占锟秸憋拷锟斤拷锟斤拷锟斤拷一锟轿的碉拷锟斤拷锟斤拷锟斤拷锟姐。
* parameters     : Global 全锟街憋拷锟斤拷锟斤拷Stat_Volt_Input Valfa,Vbelta锟斤拷锟诫，
*				 : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/

uint16_t hAfter=TW_AFTER;
uint16_t hBefore=TW_BEFORE;
uint16_t hTotal=TW_TOTAL;
    uint16_t hTimePhA=0, hTimePhB=0, hTimePhC=0, hTimePhD=0;   
void PWMC_SetPhaseVoltage(FOCVars_t* Global,Volt_Components Stat_Volt_Input, pPWMC_t this)
{
    pPWMCVars_t   pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;
    TIM_TypeDef* TIMx = pParams->TIMx;  
    int32_t wX, wY, wZ, wUAlpha, wUBeta; 
        
    uint16_t hDeltaDuty1,hDeltaDuty2;
    uint16_t hDutyV[3];

	hTotal = hAfter + hBefore;
    pVars->bSectorSwitchFlag = 0;
    pVars->bSamplingInvaildFlag = 0;
       
    wUAlpha = Stat_Volt_Input.qV_Component1 * T_SQRT3;
    wUBeta = -(Stat_Volt_Input.qV_Component2 * T);

    wX = wUBeta;
    wY = (wUBeta + wUAlpha)>>1;
    wZ = (wUBeta - wUAlpha)>>1;
   
    // Sector calculation from wX, wY, wZ
    if (wY<0)
    {
        if (wZ<0)
        {
            pVars->bSector = SECTOR_5;
        }
        else // wZ >= 0
        {
            if (wX<=0)
            {
                pVars->bSector = SECTOR_4;
            }
            else // wX > 0
            {
                pVars->bSector = SECTOR_3;
            }
        }
    }
    else // wY > 0
    {
        if (wZ>=0)
        {
            pVars->bSector = SECTOR_2;
        }
        else // wZ < 0
        {
            if (wX<=0)
            {
                pVars->bSector = SECTOR_6;
            }
            else // wX > 0
            {
                pVars->bSector = SECTOR_1;
            }
        }
    }

    if(pVars->bLastSector != pVars->bSector)
    {
        pVars->bLastSector = pVars->bSector;
        pVars->bSectorSwitchFlag = 1;
    }
        
    /* Duty cycles computation */    
#if 0   // 1 锟斤拷锟绞�   0  锟竭讹拷式
//#ifdef BIPHASE_MODULATION
    switch(pVars->bSector)
    {
        case SECTOR_1: //AB
        	         
            hDutyV[2] = ((wX - wZ)>>17);
            hDutyV[1] = (wX>>17);                            	            
            break;
                
        case SECTOR_2: //BA
        	
        	hDutyV[2] = ((wZ+wY)>>17);
            hDutyV[1] = (wY>>17);	          
            break;
                
        case SECTOR_3: //BC
              
            hDutyV[2] = ((wX-wY)>>17); 
            hDutyV[1] =  ((-wY)>>17);                	        
            break;      
                
        case SECTOR_4: //CB
        
            hDutyV[2] = ((wZ-wX)>>17); 
            hDutyV[1] = (wZ>>17);                    
            break;          
                
        case SECTOR_5: //CA
        
        	hDutyV[2] = ((-(wY+wZ))>>17);
            hDutyV[1] = ((-wZ)>>17);                                                                            	            
            break;              
                
        case SECTOR_6: //AC

            hDutyV[2] = ((wY - wX)>>17);                   
            hDutyV[1] = ((-wX)>>17);     	           
            break;          
                
        default:
        
            break;
    }
    hDutyV[0]=0; 

#else  //TRIPHASE_MODULATION
	switch(pVars->bSector)
    {
	    case SECTOR_1://ABC
	    	
				hDutyV[2]= (T>>3)+((T+wX-wZ)>>18);
				hDutyV[1]= hDutyV[2]+(wZ>>17);
				hDutyV[0]= hDutyV[1]-(wX>>17);        				
				break;
				
	    case SECTOR_2://BAC
	    	
		        hDutyV[1]= (T>>3)+((T+wY-wZ)>>18);
		        hDutyV[2]= hDutyV[1]+(wZ>>17);
		        hDutyV[0]= hDutyV[1]-(wY>>17); 					          		        
		        break;
	        	
	    case SECTOR_3://BCA
	    	
		        hDutyV[0]= (T>>3)+((T-wX+wY)>>18);
		        hDutyV[1]= hDutyV[0]-(wY>>17);
		        hDutyV[2]= hDutyV[1]+(wX>>17);				               	         		        
		        break;
	
	    case SECTOR_4://CBA
	    	
		        hDutyV[0]= (T>>3)+((T+wX-wZ)>>18);
		        hDutyV[1]= hDutyV[0]+(wZ>>17);
		        hDutyV[2]= hDutyV[1]-(wX>>17);            		        
		        break;  
	
	    case SECTOR_5://CAB
	    	
		        hDutyV[1]= (T>>3)+((T+wY-wZ)>>18);
		        hDutyV[0]= hDutyV[1]+(wZ>>17);
		        hDutyV[2]= hDutyV[1]-(wY>>17); 		        
		        break;
	
	    case SECTOR_6://ACB
	    	
		        hDutyV[2]= (T>>3)+((T-wX+wY)>>18);
		        hDutyV[1]= hDutyV[2]-(wY>>17);
		        hDutyV[0]= hDutyV[1]+(wX>>17);						        
		        break;
	        
	    default:
	        	break;
    }

#endif

#ifdef DEADBAND_OFFSET_EN           
    if(pVars->bCompensationTime!=0)         
    {       	
	    pVars->hCompensationTheta = MCM_Arctan2((-Stat_Volt_Input.qV_Component2),Stat_Volt_Input.qV_Component1);///锟斤拷锟斤拷锟轿�(-180~180) 锟斤拷应(-32768~32767)
		
		if(pVars->hCompensationTheta<0)
		{
			pVars->hCompensationTheta+=65535;
		}
				
		#define PRODUCTNUM  3//锟剿伙拷锟斤拷锟接ｏ拷锟斤拷锟斤拷斜锟斤拷锟斤拷锟斤拷锟斤拷龋锟斤拷锟斤拷锟斤拷锟�+-15锟姐，锟斤拷锟斤拷锟斤拷锟侥乘伙拷锟斤拷浠�+-4.5锟斤拷锟斤拷锟斤拷
	
	  
		if(pVars->hCompensationTheta<=IS_THET_60) 	 	
		{
		    pVars->hCompensationTime1 = (pVars->hCompensationTheta-IS_THET_30)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;	    
		}
		else if((pVars->hCompensationTheta>IS_THET_60)&&(pVars->hCompensationTheta<=IS_THET_120))		
		{						
			pVars->hCompensationTime1=(IS_THET_90-pVars->hCompensationTheta)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;		
		}	
		else if((pVars->hCompensationTheta>IS_THET_120)&&(pVars->hCompensationTheta<=IS_THET_180))	
		{			
			pVars->hCompensationTime1=(pVars->hCompensationTheta-IS_THET_150)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;				
		}	
		else if((pVars->hCompensationTheta>IS_THET_180)&&(pVars->hCompensationTheta<=IS_THET_240))		
		{			
			pVars->hCompensationTime1=(IS_THET_210-pVars->hCompensationTheta)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;				
		}	
		else if((pVars->hCompensationTheta>IS_THET_240)&&(pVars->hCompensationTheta<=IS_THET_300))		
		{			
			pVars->hCompensationTime1=(pVars->hCompensationTheta-IS_THET_270)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;					
		}	
		else if((pVars->hCompensationTheta>IS_THET_300)&&(pVars->hCompensationTheta<=IS_THET_360))		
		{			
			pVars->hCompensationTime1=(IS_THET_330-pVars->hCompensationTheta)>>6;
		    pVars->hCompensationTime1=PRODUCTNUM*pVars->hCompensationTime1;						
		}    

		if(pVars->hCompensationTime1<(-pVars->bCompensationTime))
	 		pVars->hCompensationTime1=-pVars->bCompensationTime;
		if(pVars->hCompensationTime1>pVars->bCompensationTime)
			pVars->hCompensationTime1=pVars->bCompensationTime;
			
		pVars->hCompensationTime2=pVars->bCompensationTime;
		pVars->hCompensationTime0=-pVars->bCompensationTime;					
	}
	else
	{
		pVars->hCompensationTime0=0;
		pVars->hCompensationTime1=0;
		pVars->hCompensationTime2=0;
		
	}

    hDutyV[0]+=pVars->hCompensationTime0;
    hDutyV[1]+=pVars->hCompensationTime1;
    hDutyV[2]+=pVars->hCompensationTime2;
	
#endif

	if(hDutyV[0]>32768)hDutyV[0]=0;
    if(hDutyV[1]>32768)hDutyV[1]=0;
    if(hDutyV[2]>32768)hDutyV[2]=0;

    if(hDutyV[0]>=PWM_PERIOD)hDutyV[0]=PWM_PERIOD-1;
    if(hDutyV[1]>=PWM_PERIOD)hDutyV[1]=PWM_PERIOD-1;
    if(hDutyV[2]>=PWM_PERIOD)hDutyV[2]=PWM_PERIOD-1;

	/* Set CC4 as PWM mode 2 (default) */
  	TIMx->CCMR2 &= CCMR2_CH4_DISABLE;
  	TIMx->CCMR2 |= CCMR2_CH4_PWM2;
		    	    	 
    // ADC Syncronization setting value
    if((u16)(PWM_PERIOD-hDutyV[2])>(hAfter))
    {
        hTimePhD = PWM_PERIOD - 1;
		pVars->bSoFOC = 50;
//		GPIO_SetBits(GPIOD,GPIO_Pin_2);
    }
    else
    {
        hDeltaDuty1 = (u16)(PWM_PERIOD-hDutyV[2])<<1;
        hDeltaDuty2 = (u16)(hDutyV[2] - hDutyV[1]);

		if((hDeltaDuty1>=hBefore)||(hDeltaDuty2 >= hBefore))
		{
			if(hDeltaDuty1<hDeltaDuty2)
	        {          
				hTimePhD = hDutyV[2] - hBefore;
//				hTimePhD = hDutyV[1] + TW_AFTER;
				pVars->bSoFOC = 150;				
	        }
			else
			{
				hTimePhD = hDutyV[2] + hAfter;
				if (hTimePhD >= PWM_PERIOD)
				{ 			
					TIMx->CCMR2 &= CCMR2_CH4_DISABLE;   
					TIMx->CCMR2 |= CCMR2_CH4_PWM1;
					hTimePhD = (2u * PWM_PERIOD) - hTimePhD - 1u;
					pVars->bSoFOC = 100;
				}	
			}
		}        
        else
        {
		    pVars->bSamplingInvaildFlag = 1;
            hTimePhD = hDutyV[1] - hBefore;
			pVars->bSoFOC = 200;
        }
    }

    pVars->hLastDuty = hDutyV[2];

    switch(pVars->bSector)
    {
        case SECTOR_1:
        
            hTimePhA = hDutyV[2];
            hTimePhB = hDutyV[1];
			hTimePhC = hDutyV[0];
			pVars->bPhase1AdChannel = pParams->bIbChannel;
			pVars->bPhase2AdChannel = pParams->bIcChannel;
            break;
                
        case SECTOR_2:
                  
            hTimePhB = hDutyV[2];
            hTimePhA = hDutyV[1];
 			hTimePhC = hDutyV[0];
			pVars->bPhase1AdChannel = pParams->bIaChannel;
			pVars->bPhase2AdChannel = pParams->bIcChannel;
            break;
                
        case SECTOR_3:
    
            hTimePhB = hDutyV[2];
            hTimePhC = hDutyV[1];
           	hTimePhA = hDutyV[0];
			pVars->bPhase1AdChannel = pParams->bIaChannel;
			pVars->bPhase2AdChannel = pParams->bIcChannel;
            break;      
                
        case SECTOR_4:
        
            hTimePhC = hDutyV[2];
            hTimePhB = hDutyV[1];
            hTimePhA = hDutyV[0];
			pVars->bPhase1AdChannel = pParams->bIaChannel;
			pVars->bPhase2AdChannel = pParams->bIbChannel;
            break;          
                
        case SECTOR_5:
        
            hTimePhC = hDutyV[2];
            hTimePhA = hDutyV[1];
            hTimePhB = hDutyV[0];
			pVars->bPhase1AdChannel = pParams->bIaChannel;
			pVars->bPhase2AdChannel = pParams->bIbChannel;
            break;              
                
        case SECTOR_6:

            hTimePhA = hDutyV[2];
            hTimePhC = hDutyV[1];
            hTimePhB = hDutyV[0]; 
			pVars->bPhase1AdChannel = pParams->bIbChannel;
			pVars->bPhase2AdChannel = pParams->bIcChannel;
            break;          
                
        default:
        
            break;
    }
  
#ifdef SINGLE_DRIVER
    ADC_InjectedChannelConfig(ADC1, pVars->bPhase1AdChannel,1, ADC_SampleTime_7Cycles5);
    ADC_InjectedChannelConfig(ADC2, pVars->bPhase2AdChannel,1, ADC_SampleTime_7Cycles5);
#endif
    
    if(Global->bRunning == 0)
    {
        hTimePhA = 0;
        hTimePhB = 0;
        hTimePhC = 0;
        hTimePhD = PWM_PERIOD - 1;
//        TIM_CtrlPWMOutputs(TIMx,DISABLE);
    }
    	
	if(hTimePhD < 1)
	{
		hTimePhD = 1;
	}
	else if(hTimePhD >= PWM_PERIOD)
	{
		hTimePhD = PWM_PERIOD - 1;
	}
		
    DMA_ClearFlag(DMA1_FLAG_TC1);

	#if (PHASEMATCH == PHASEMATCH1)
    TIMx->CCR1 = hTimePhA;
    TIMx->CCR2 = hTimePhB;
    TIMx->CCR3 = hTimePhC;
    TIMx->CCR4 = hTimePhD;
	#elif(PHASEMATCH == PHASEMATCH2)
	TIMx->CCR1 = hTimePhA;
    TIMx->CCR2 = hTimePhC;
    TIMx->CCR3 = hTimePhB;
    TIMx->CCR4 = hTimePhD; 
	#elif(PHASEMATCH == PHASEMATCH3)
	TIMx->CCR1 = hTimePhC;
    TIMx->CCR2 = hTimePhB;
    TIMx->CCR3 = hTimePhA;
    TIMx->CCR4 = hTimePhD;
	#elif(PHASEMATCH == PHASEMATCH4)
	TIMx->CCR1 = hTimePhB;
    TIMx->CCR2 = hTimePhA;
    TIMx->CCR3 = hTimePhC;
    TIMx->CCR4 = hTimePhD;
	#elif(PHASEMATCH == PHASEMATCH5)
	TIMx->CCR1 = hTimePhB;
    TIMx->CCR2 = hTimePhC;
    TIMx->CCR3 = hTimePhA;
    TIMx->CCR4 = hTimePhD;
	#elif(PHASEMATCH == PHASEMATCH6)
	TIMx->CCR1 = hTimePhC;
    TIMx->CCR2 = hTimePhA;
    TIMx->CCR3 = hTimePhB;
    TIMx->CCR4 = hTimePhD;
	#endif

    Global->hPwmA = hTimePhA;
    Global->hPwmB = hTimePhB;
    Global->hPwmC = hTimePhC;
	Global->hPwmD = hTimePhD;
	TIMx->CCER |= TIMx_CC4E_BIT;
	ADC1->CR2 = pVars->wADCTriggerSet;
    ADC2->CR2 = pVars->wADCTriggerSet;
	//TIMx->EGR = TIM_PSCReloadMode_Immediate; 
}

/*******************************************************************************
* Function Name  : PWMC_PhaseAdcChannelConfig
* Description    : 锟斤拷锟矫碉拷锟斤拷锟斤拷锟斤拷通锟斤拷
* parameters     : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
void PWMC_PhaseAdcChannelConfig(pPWMC_t this)
{
	pPWMCVars_t   pVars = &(this->Vars);
	ADC_InjectedChannelConfig(ADC1, pVars->bPhase1AdChannel,1, ADC_SampleTime_1Cycles5);
    ADC_InjectedChannelConfig(ADC2, pVars->bPhase2AdChannel,1, ADC_SampleTime_1Cycles5);
}

/*******************************************************************************
* Function Name  : PWMC_GetPhaseCurrents
* Description    : 锟斤拷取AB锟斤拷锟斤拷锟�
* parameters     : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
Curr_Components PWMC_GetPhaseCurrents(pPWMC_t this)
{    
    pPWMCVars_t   pVars = &(this->Vars);  
    
    int32_t wCurrentComponent1=0;
    int32_t wCurrentComponent2=0;     

    Curr_Components Local_Stator_Currents;

    /* Deactivate TIMx CH4 to disable next triggers using bit-banding access */
    *(uint32_t*)(pVars->wTIMxCH4_BB_Addr) = 0u;
	ADC1->CR2 = pVars->wADCTriggerUnSet;
    ADC2->CR2 = pVars->wADCTriggerUnSet;
    
    if(0)//pVars->bSamplingInvaildFlag != 0)
    {          
        switch(pVars->bSector)
	    {           
	        case 3:
	        case 4:     
	            //Saturation of Ia 
	            wCurrentComponent1 = (s32)(pVars->hPhaseAOffset)-((ADC1->JDR1)<<1);  
	            // Saturation of Ib        
	            wCurrentComponent2 = pVars->wPrevCurrentComponent2;             
	            break;
	            
	        case 5:
	            // Saturation of Ib    
	            wCurrentComponent2 = (s32)(pVars->hPhaseBOffset)-((ADC2->JDR1)<<1);
	            //Saturation of Ia 
	            wCurrentComponent1 = pVars->wPrevCurrentComponent1;            
	            break;
	            
	        case 6: 
	            //Saturation of Ib 
	            wCurrentComponent2 = (s32)(pVars->hPhaseBOffset)-((ADC1->JDR1)<<1); 
	            //Saturation of Ia 
	            wCurrentComponent1 = pVars->wPrevCurrentComponent1;                      
	            break;  

	        case 1:   
	        case 2:
	        	//Saturation of Ia                 	            
	            wCurrentComponent1 = pVars->wPrevCurrentComponent1;
	            //Saturation of Ib 
	            wCurrentComponent2 = pVars->wPrevCurrentComponent2; 	             
	            break;
	            
	        default:
	        
	            break;
	    }	      
	}
	else
	{
		switch (pVars->bSector)
	    {
	        case 4:
	        case 5: //Current on Phase C not accessible  
	        
	            //Saturation of Ia
	            wCurrentComponent1 = (s32)(pVars->hPhaseAOffset)-((ADC1->JDR1)<<1); 
	            // Saturation of Ib   
	            wCurrentComponent2 = (s32)(pVars->hPhaseBOffset)-((ADC2->JDR1)<<1);                
	                             
	            break;
	
	        case 6:
	        case 1:  //Current on Phase A not accessible    
	        
	            //Saturation of Ib 
	            wCurrentComponent2 = (s32)(pVars->hPhaseBOffset)-((ADC1->JDR1)<<1);            
	            //Saturation of Ia
	            wCurrentComponent1 = ((ADC2->JDR1)<<1)-(pVars->hPhaseCOffset)-wCurrentComponent2;
	            
	            break;
	
	        case 2:
	        case 3:  // Current on Phase B not accessible   
	
	            //Saturation of Ia 
	            wCurrentComponent1 = (s32)(pVars->hPhaseAOffset)-((ADC1->JDR1)<<1);                
	            // Saturation of Ib
	            wCurrentComponent2 = ((ADC2->JDR1)<<1)- (pVars->hPhaseCOffset) - wCurrentComponent1;		
	                            
	            break;
	
	        default:
	        
	            break;
	    }	    
	}
			
    pVars->wPrevCurrentComponent1 = wCurrentComponent1;
    pVars->wPrevCurrentComponent2 = wCurrentComponent2; 

    if(wCurrentComponent1 < S16_MIN)
    {
        wCurrentComponent1= S16_MIN;
    }  
    else if(wCurrentComponent1 > S16_MAX)
    { 
        wCurrentComponent1= S16_MAX;
    }
    
    if(wCurrentComponent2 < S16_MIN)
    {
        wCurrentComponent2= S16_MIN;
    }
    else if(wCurrentComponent2 > S16_MAX)
    {
        wCurrentComponent2= S16_MAX;
    }
    
    Local_Stator_Currents.qI_Component1 = wCurrentComponent1;
    Local_Stator_Currents.qI_Component2 = wCurrentComponent2;


    pVars->Last_Stator_Currents = Local_Stator_Currents;
    
    return(Local_Stator_Currents);
}

/*******************************************************************************
* Function Name  : PWMC_IRQHandler
* Description    : 锟叫伙拷锟斤拷锟斤拷源锟斤拷锟斤拷锟矫达拷锟斤拷通锟斤拷
* parameters     : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
#if 1//def DOUBLE_DRIVER
static void PWMC_IRQHandler(pPWMC_t this)
{
//    uint32_t wADCInjFlags;
    pPWMCVars_t pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;
    TIM_TypeDef* TIMx = pParams->TIMx;


    /* Switch Context */
    /* Disabling trigger to avoid unwanted conversion */
    ADC1->CR2 = pVars->wADCTriggerUnSet;
    ADC2->CR2 = pVars->wADCTriggerUnSet;


    /* Enabling next Trigger */
    TIMx->CCER |= TIMx_CC4E_BIT;

    /* It re-initilize AD converter in run time when using dual MC */   
    ADC1->CR2 = pVars->wADCTriggerSet;
    ADC2->CR2 = pVars->wADCTriggerSet;

//	PWMC_PhaseAdcChannelConfig(this);

}
#endif

/*******************************************************************************
* Function Name  : PWMC_Clear
* Description    : 锟斤拷PWM值
* parameters     : None
* Return         : None
*******************************************************************************/
void PWMC_Clear(pPWMC_t this)
{
    pPWMCParams_t pParams = this->pParams;
    TIM_TypeDef* TIMx = pParams->TIMx;

	TIMx->CCR1=0;
	TIMx->CCR2=0;
	TIMx->CCR3=0;
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : TIM8 锟斤拷锟斤拷锟叫断凤拷锟斤拷锟斤拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER
void TIM8_UP_IRQHandler(void)
{

#ifdef LINEAR_HALL
    GPIO_SetBits(GPIOC,GPIO_Pin_13); 
#endif
	bMCPwmPeriodCompleted=1;
    TIM_ClearFlag(TIM8, TIM_FLAG_Update);

	POS_CalcElAngle(oPosSen[M2],&FOCVars[M2]);

    PWMC_IRQHandler(oPwmc[M2]);		

	TSK_DualDriveUpdate(M2);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	STC_CalcBusVoltage();
	 ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
	#ifdef J_SCOPE
	JS_WindowsWithTimer8();
	#endif
	//GPIO_ResetBits(GPIOD,GPIO_Pin_2);		
}
#endif

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : TIM8 锟斤拷锟斤拷锟叫断凤拷锟斤拷锟斤拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER
void TIM8_BRK_IRQHandler(void)
{
	TIM_ClearFlag(TIM8, TIM_FLAG_Break);
	oPwmc[M2]->Vars.bOverCurrentFault=1;
}
#endif
u16 tim6_count5 = 0;
u16 tim6_count6 = 0;
u16 tim6_countdelta3 = 0;
u16 tim1_count = 0;
/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : TIM1 锟斤拷锟斤拷锟叫断凤拷锟斤拷锟斤拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
    tim6_count5 = TIM6->CNT;
    tim1_count++;
//	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	bMCPwmPeriodCompleted = 1;
    
    if(oPwmc[M1]->Vars.bCheckMosfetFlag == 1 && oPwmc[M1]->Vars.bCalibrateCompletionFlag == 1)
    {
        TSK_HighFrequencyTask();          
    }
    else 
    {
    
    }
   
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	POS_CalcElAngle(oPosSen[M1], &FOCVars[M1]);
#ifdef Sensorless
   // FORCE_CalcElAngle();
#endif
	#ifdef DOUBLE_DRIVER
    PWMC_IRQHandler(oPwmc[M1]);
    #endif
	#ifndef DOUBLE_DRIVER		
//	PWMC_IRQHandler(oPwmc[M1]);
	//STC_CalcBusVoltage();
    #endif
//	TSK_DualDriveUpdate(M1);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
    STC_CalcBusVoltage();
//    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//	CRLM_CalcBusCurrent(oPwmc[M1]->Vars.hBusCurrentOffset, oPwmc[M2]->Vars.hBusCurrentOffset);
	#ifdef J_SCOPE
//	JS_WindowsWithTimer1();
	#endif
//	GPIO_SetBits(GPIOB,GPIO_Pin_3);
    tim6_count6 = TIM6->CNT;
    tim6_countdelta3 = tim6_count6 - tim6_count5; //实测最大420，420/120=3.5us
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : TIM1 锟斤拷锟斤拷锟叫断凤拷锟斤拷锟斤拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
	TIM_ClearFlag(TIM1, TIM_FLAG_Break);
	oPwmc[M1]->Vars.bOverCurrentFault = 1;
}
u16 adc12_count = 0;
u16 adc12_countset = 100;
/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : ADC 锟叫断凤拷锟斤拷锟斤拷锟�
* parameters     : None
* Return         : None
*******************************************************************************/
u16 tim6_count1 = 0;
u16 tim6_count2 = 0;
u16 tim6_countdelta1 = 0;
void ADC1_2_IRQHandler(void)
{
	uint8_t bMotorNbr = 0;
    
    tim6_count1 = TIM6->CNT;   
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    
    ADC1->SR &= ~(u32)(ADC_FLAG_JEOC | ADC_FLAG_JSTRT);
  
    if(adc12_count >= adc12_countset)
    {
        adc12_count = 0;        
    }
    else
    {
        adc12_count++;    
    }
    
	FOC_CurrController(M1);	
    
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
    tim6_count2 = TIM6->CNT;
    tim6_countdelta1 = tim6_count2 - tim6_count1;    
}

/*******************************************************************************
* Function Name  : PWMC_CheckMosfets
* Description    : Mosfet 锟皆硷拷
* parameters     : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
void PWMC_CheckMosfets(pPWMC_t this)
{
    pPWMCVars_t pVars = &(this->Vars);
    pPWMCParams_t pParams = this->pParams;    
    TIM_TypeDef* TIMx = pParams->TIMx;    
    uint8_t i,imax;
    
    if(pVars->bCheckMosfetFlag == 0)
    {
        TIMx->CCR1 = 0;
    	TIMx->CCR2 = 0;
        TIMx->CCR3 = 0;
    	    
        pVars->bOverCurrentFault = 0;
        pVars->bMosfetHFault = 0;
        pVars->bMosfetLFault = 0;
                
        imax = 5;
        for(i = 0; i < imax; i++)
        {
            if(pVars->bOverCurrentFault == 0)
            {
                DelayUs(30);
				DI();
                TIMx->CCMR1 = MosfetCheckSteps[i].CCMR_1;
                TIMx->CCMR2 = MosfetCheckSteps[i].CCMR_2;
                TIMx->CCER = MosfetCheckSteps[i].CCER;
                TIMx->EGR |= BIT5;
                TIM_CtrlPWMOutputs(TIMx, ENABLE);
                DelayUs(15);
                TIM_CtrlPWMOutputs(TIMx, DISABLE);
                TIMx->CCMR1 = CCMR_A_PWM | CCMR_B_PWM;
                TIMx->CCMR2 = CCMR_C_PWM | CCMR_D_PWM;
                TIMx->CCER = A_OFF | B_OFF | C_OFF | D_ON;
                TIMx->EGR |= BIT5;
				EI();
                if (TIMx->SR & 0x0080)
                {
                    pVars->bOverCurrentFault = 1;
                    TIMx->SR &= 0xff7f;
					i = 6;//锟斤拷锟斤拷一路锟斤拷锟斤拷锟斤拷锟斤拷锟狡筹拷,锟斤拷锟斤拷锟皆硷拷
                }
                else
                {
                    pVars->bOverCurrentFault = 0;
                }
            }
            else
            {
                if(i == 1)
                {
                    pVars->bMosfetHFault = 1;
                    i=6;//锟斤拷锟斤拷一路锟斤拷锟斤拷锟斤拷锟斤拷锟狡筹拷,锟斤拷锟斤拷锟皆硷拷
                }
                else if(i == 2)
                {
                    pVars->bMosfetLFault = 1;
                    i = 6;//锟斤拷锟斤拷一路锟斤拷锟斤拷锟斤拷锟斤拷锟狡筹拷,锟斤拷锟斤拷锟皆硷拷
                }								
                break;
            }
        }

		DelayUs(30);
//		DI();
//        TIM_CtrlPWMOutputs(TIMx,ENABLE);
//        TIMx->CCMR1 = MosfetCheckSteps[0].CCMR_1;
//    	TIMx->CCMR2 = MosfetCheckSteps[0].CCMR_2;
//    	TIMx->CCER =  MosfetCheckSteps[0].CCER;
//    	TIMx->EGR |= BIT5;
//        DelayUs(15);
//        TIM_CtrlPWMOutputs(TIMx,DISABLE);
//        TIMx->CCMR1 = CCMR_A_PWM|CCMR_B_PWM;
//        TIMx->CCMR2 = CCMR_C_PWM|CCMR_D_PWM;
//        TIMx->EGR |= BIT5;
//        EI();
        pVars->bCheckMosfetFlag = 1;
    }
}



#ifdef DEADBAND_OFFSET_EN
/*******************************************************************************
* Function Name  : PWMC_CalcCompensationTime
* Description    : 锟斤拷锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷锟�
* parameters     : this PWMCpool锟结构指锟斤拷
* Return         : None
*******************************************************************************/
void PWMC_CalcCompensationTime(Volt_Components Stat_Volt_q_d, pPWMC_t this)
{
	pPWMCVars_t pVars = &(this->Vars);
		
	if(Stat_Volt_q_d.qV_Component1>31128)//95%
	{
		if(++pVars->bCompensationCounter>10)
		{
			pVars->bCompensationCounter=0;
			if(pVars->bCompensationTime>0)
			{
				pVars->bCompensationTime--;
			}
		}
	}	
	else if(Stat_Volt_q_d.qV_Component1<29490)//90%
	{
		if(--pVars->bCompensationCounter<(-10))
		{
			pVars->bCompensationCounter=0;
			if(pVars->bCompensationTime<DEADTIME_SET)
			{
				pVars->bCompensationTime++;
			}
		}
	}
	else 
	{
		pVars->bCompensationCounter=0;
	}
}
#endif


/**************************************************************END OF FILE****/
