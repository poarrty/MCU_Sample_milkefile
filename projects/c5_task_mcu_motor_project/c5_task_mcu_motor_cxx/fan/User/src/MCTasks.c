/******************************************************************************
* File Name         :  MCTasks.c
* Author            :  ���ŷ�
* Version           :  1.0
* Date              :  19/01/14
* Description       :  ����������������
********************************************************************************
* History:
* <author>        <time>        <version >        <desc>
* ���ŷ�             19/01/14      1.0               ����
* �����            20/02/10                         �޸�                  
* �޸�����: 1 HandleMagneticBrake()��һЩ�߼������޸�
*           2 ���ҵ����Ƶ������TSK_HighFrequencyTaskM1(),TSK_HighFrequencyTaskM2()����ͨ���쳣ʱƽ�����ٴ���
*           3 �����ٶ�ģʽ�Ϳ�������ģʽ,Ĭ��Ϊ�ٶȿ���,ע��//#define SPEED_MODE��Ϊ��������
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
#define _SWShutdown_En

#ifdef JlinkSCope
char JS_RTT_UpBuffer[4096]; // J-Scope RTT Buffer
int JS_RTT_Channel = 1;     // J-Scope RTT Channel
#endif
#ifdef JlinkSCope
ValBufferType acValBuffer;
#endif

/* Private variables----------------------------------------------------------*/
pPI_t 	oPIDSpeed[NBR_OF_MOTORS];
pPI_t 	oPIDSpeedLike[NBR_OF_MOTORS];
pPI_t 	oPIDIq[NBR_OF_MOTORS];
pPI_t 	oPIDId[NBR_OF_MOTORS];
pPI_t 	oPIDIs[NBR_OF_MOTORS];
pPI_t 	oPIDVs[NBR_OF_MOTORS];
pPI_t 	oPIAcc[NBR_OF_MOTORS];
pSTC_t 	oSTC[NBR_OF_MOTORS];
pCLM_t 	oCLM[NBR_OF_MOTORS];
pPWMC_t oPwmc[NBR_OF_MOTORS];
pHALL_t oHall[NBR_OF_MOTORS];
pMTPA_t oMTPA[NBR_OF_MOTORS];
pCRLM_t oCRLM[NBR_OF_MOTORS];
pPI_t 	oPINTc[NBR_OF_MOTORS]; 
pPI_t 	oPIDOvs[NBR_OF_MOTORS];
//pENC_t oEncoder[NBR_OF_MOTORS];

//pPOS_t oPosFdbk[NBR_OF_MOTORS];


FOCVars_t FOCVars[NBR_OF_MOTORS];


Relay_t Relay;
extern DriverMsg_t 	DriverMsg;

uint8_t  bMagneticBrakeRelease = 0;
uint8_t bBreakLockingFlag = 0;
uint8_t bMotorEabsStopFlag =0;
uint8_t  bMagneticBrakeStaLooseRunFlag=0;

uint8_t Slope_static_flagM1;
uint8_t Slope_static_flagM2;

uint8_t R_BREAK;
uint8_t L_BREAK;

uint8_t bBatteryChargeSta = 0;
uint8_t bMCPwmPeriodCompleted =0;
uint8_t bMcDriverErrFlag = 0;
uint8_t bThermalShutdownFlag;
uint8_t PowerEnableFlag;
uint16_t wOvAbnormalCnt;
uint8_t bOvAbnormalFlag;
uint8_t bcompeletFlag;
uint16_t bcompeletCnt;
uint8_t bPowerDownFlag;
uint16_t wPowerDownCnt;
uint16_t hAD_err_flag1;
uint16_t hAD_err_flag2;
uint16_t hAD_err_flag3;
uint16_t hAD_err_flag4;
uint8_t bMotorSlopeStopFlag;
uint16_t hMosCheckCnt;
uint8_t  bMagneticBrakeOutputStartFlag;
uint8_t   bMagneticBrakeFlag;
uint16_t  hMotorStartCnt;
uint8_t  bMototRunStateFlag;

static uint8_t bMCBootCompleted = 0;
static uint8_t bMC1msCompleted = 0;
static uint8_t bMC16msCompleted = 0;
static uint8_t FOC_index = 0;
int16_t hMCUTemp1 = 0;
int16_t hMCUTemp2 = 0;

extern uint8_t  bNeedEncryption;
extern uint16_t hCanMessageID;


#ifdef SPEED_MODE//---------
int16_t hHallSpeed[NBR_OF_MOTORS];
int16_t hHallSpeedDelta[NBR_OF_MOTORS];
#endif//---------

#ifdef UART_DEBUG
uint8_t bFlagTemp1 = 0;

int16_t hHallAngle1;

#endif

 int16_t leeHallAngle =0;
int16_t leeEIAngle =0;
u16   hMecSpeed01HZ = 0;
u16  hMecSpeedRPM = 0;
/* Private functions ---------------------------------------------------------*/
static void TSK_HighFrequencyTaskM1(void);
static void FOC_Clear(uint8_t bMotor);

static void FOC_CalcCurrRef(uint8_t bMotor);

static void TSK_HighFrequencyTaskM2(void);

int16_t MC_MotorEabsStop(uint8_t bMotor);
int16_t MC_RampSpdCompensation(uint8_t bMotor);

void FOC_Init(uint8_t bMotor);
void MC_MotorStart(uint8_t bMotor);
void MC_FaultDetection(void);
void HandleMagneticBrake(void);
void MC_BreakerHandle(uint8_t bMotor);
void MC_MagneticBrakeCheck(void);
void HandlebarHandle(void);
Volt_Components Valphabeta, Vqd;

//extern uint16_t hRegConv[7]; 
//#define RUN_BLDC//��ֲ�ķ���������ʽ//190716
extern HALL_LR_DEF     HALL_Msg;

#ifdef RUN_BLDC//190716
extern void ChangePhase(uint8_t Hall_state,int16_t L_PWM,TIM_TypeDef* TIMx);
#endif

/*******************************************************************************
* Function Name  : GlobalVariable_Init
* Description    : ȫ�ֱ�����ʼ������
* parameters     : None
* Return         : None
*******************************************************************************/
void GlobalVariable_Init(void)
{
   	bCanMagneticBrakeRelease = 0;
	bCanCommFault = 0;
	bCanMagneticBrakeStaM1 = 0;
    bCanMagneticBrakeStaM2 = 0;
    bNeedEncryption = 0;
    hCanMessageID = 0;

	bBreakLockingFlag = 0;
	bMotorEabsStopFlag =0;	
	Slope_static_flagM1=0;
	Slope_static_flagM2=0;

	bMagneticBrakeRelease = 0;
    bBatteryChargeSta = 0;
    bMCPwmPeriodCompleted =0;
    bMcDriverErrFlag = 0;
    bThermalShutdownFlag=0;
	wOvAbnormalCnt=0;
	bOvAbnormalFlag=0;
	bcompeletFlag=0;
	bcompeletCnt=0;
    bPowerDownFlag=0;
    wPowerDownCnt=0;
    hAD_err_flag1=0;
    hAD_err_flag2=0;
    hAD_err_flag3=0;
    hAD_err_flag4=0;

    bMC1msCompleted = 0;
    bMC16msCompleted = 0;
    FOC_index = 0;
    hMCUTemp1 = 0;
    hMCUTemp2 = 0;
	PowerEnableFlag=1; //�Ͽص�Դʹ�����ж�, �����ϵ�"1",����Ϊ"0", ��ʼ��Ĭ��Ϊ"1"
	bMagneticBrakeStaLooseRunFlag=0;
	bMotorSlopeStopFlag=0;
	hMosCheckCnt=0;
    hMotorStartCnt=0;
    bMagneticBrakeFlag=0;
    bMototRunStateFlag=0;
    bMagneticBrakeOutputStartFlag=0;	 
}

/*******************************************************************************
* Function Name  : MCboot
* Description    : ����ϵ��������������ø�ģ���ʼ��
* parameters     : None
* Return         : None
*******************************************************************************/
void MCboot(void)
{
    bMCBootCompleted = 0;	
	GlobalVariable_Init(); //ȫ�ֱ�����ʼ������
	//POS_Init();
    //UARTC_Init();
    PWMC_Init();
    CLM_Init();
    PI_Init();

#if (MTPA_ENABLING == ENABLE)
    MTPA_Init();
#endif

#ifndef Sensorless      
	POS_Init();
#endif
	CRLM_Init();
    STC_Init();
    TB_Init();   
   	NTC_Init();
#ifdef UI_DEBUG
    UI_InitChannelPool();
#endif
    Relay.bCommand = 1;
    bMCBootCompleted = 1;		
}


/*******************************************************************************
* Function Name  : MC_FaultDetection
* Description    : ���ϼ��
* parameters     : None
* Return         : None
*******************************************************************************/
extern void DelayUs(u16 i);

void MC_FaultDetection(void)
{
    PWMC_CheckMosfets(oPwmc[M1]);  //bdh�����ȥ�����У�����֣���֪��ʲôԭ�����������������ط�Ҳ�����������
	DelayUs(130);//63
//	if(hRegConv[3]<30000) {hAD_err_flag1=1000;}
    PWMC_CurrentCalibration(oPwmc[M1]);
	DelayUs(130);//63

	#ifdef DOUBLE_DRIVER
	PWMC_CheckMosfets(oPwmc[M2]);
	DelayUs(130);//63
	if(hRegConv[3]<30000) {hAD_err_flag3=1000;}
		PWMC_CurrentCalibration(oPwmc[M2]);
	DelayUs(130);//63

	
	if(hRegConv[3]<30000) 
	{
		hAD_err_flag4=1000;
	}
	else 
	{
		hAD_err_flag4=0;
	}
		#endif
}


/*******************************************************************************
* Function Name  : MC_Relay
* Description    : �̵������ƺ���
* parameters     : None
* Return         : None
*******************************************************************************/
void MC_Relay(void)
{
    int32_t VoltTemp;
    switch(Relay.bCommand) //初始化给的1
    {
        case 0 :
            break;
        case 1 :
            Relay.hCounter = 460;//187;
            Relay.bCommand = 2;
            break;
        case 2 :
            Relay.hCounter --;
            if(Relay.hCounter > 400)//16ms * (460 - 400) = 960ms
            {
                if(hVs > 38400) // 45392对应48V，欠压保护值40.5V,所以设为40.5/48*45392= 38299
                {
                    Relay.bCommand = 3;
                    GPIO_SetBits(PRECHARGE_GPIO_Port, PRECHARGE_GPIO_Pin);
                }
            }
            else
            {
                Relay.hCounter = 0;
                Relay.bCommand = 0;
                Relay.bFail = 0;
                Relay.bStatus = 0;
                Relay.bFail_lv = 1;     //电池欠压 
                Relay.bReady = 0;   
                Relay.bMosCheck = 0;
            }
            break;

        case 3:
            Relay.hCounter --;

            VoltTemp = (int32_t)hVs;        //电池电压
            VoltTemp -= (int32_t)hVrelay;   //预充电电压
            if((VoltTemp < 11384) && (hVs > 38400) && (Relay.hCounter < 390)) //11384对应12V 125
    //        if(1)//((VoltTemp < 20000)&&(hVs > 14600))//41600(21V�ɰ�)  40000  3000
            {
                GPIO_SetBits(RELAY_GPIO_Port, RELAY_GPIO_Pin);
                Relay.hCounter = 63;
                Relay.bCommand = 4;
            }
            else if(Relay.hCounter == 0)
            {
                Relay.bFail = 1;    //预充电失败
                GPIO_ResetBits(PRECHARGE_GPIO_Port, PRECHARGE_GPIO_Pin);									
                Relay.hCounter = 0;//0
                Relay.bCommand = 0;//0
                Relay.bStatus = 0;//1
                Relay.bReady = 0;   
                Relay.bMosCheck = 0;
            }
            break;
            
        case 4:
            Relay.hCounter --;

            VoltTemp = (int32_t)hVs;
            VoltTemp -= (int32_t)hVrelay;
    //      if(1) 
            if(VoltTemp < 11384)    
            {
                Relay.bCommand = 0;
                GPIO_ResetBits(PRECHARGE_GPIO_Port, PRECHARGE_GPIO_Pin);
                Relay.bStatus = 1;
                Relay.bReady = 1;    
                Relay.bMosCheck = 1; 
            }
            else if(Relay.hCounter == 0)
            {
                Relay.bFail = 1;
                GPIO_ResetBits(RELAY_GPIO_Port, RELAY_GPIO_Pin);
                GPIO_ResetBits(PRECHARGE_GPIO_Port, PRECHARGE_GPIO_Pin);
                        
                Relay.bCommand = 0;
                Relay.bStatus = 0;//1
                Relay.bReady = 0;   
                Relay.bMosCheck = 0;
            }
            break;
    }
}


/*******************************************************************************
* Function Name  : TSK_HighFrequencyTaskM1
* Description    : ������Ƶ������
* parameters     : None
* Return         : None
*******************************************************************************/

void TSK_HighFrequencyTaskM1(void)
{
    int16_t hRefTemp;

    hRefTemp = ControlCommand.hSpeedM1;   
    
    if(hRefTemp > MOTOR_MAX_SPEED_RPM)
    {
    	hRefTemp = MOTOR_MAX_SPEED_RPM;
    }
    else if(hRefTemp < -MOTOR_MAX_SPEED_RPM)
    {
    	hRefTemp = -MOTOR_MAX_SPEED_RPM;
    }

		
	#ifdef SPEED_MODE
	oSTC[M1]->Vars.hSpeedRef01HzExt = hRefTemp / 6;// rpm转换到01hz，6倍关系
	#else 
	oSTC[M1]->Vars.hTorqueRef = hRefTemp;
	#endif	
    
/*
    if((oPosSen[M1]->Vars.bBlockageFault != 0))//|| (oEncoder[M1]->Vars.bHallFault != 0))//xu-190628	
    {
        oSTC[M1]->Vars.hTorqueRef =  0;
		oSTC[M1]->Vars.hSpeedRef01HzExt = 0;
    }
*/
    
    if(FOCVars[M1].bMotorStop == 0) // FOCVars[M2].bMotorStop
    {
        FOC_CalcCurrRef(M1);
    }
}


/*******************************************************************************
* Function Name  : TSK_HighFrequencyTaskM2
* Description    : �ҵ����Ƶ������
* parameters     : None
* Return         : None
*******************************************************************************/

#ifdef DOUBLE_DRIVER
int32_t spd_ref1;
void TSK_HighFrequencyTaskM2(void)
{
	int32_t hRefTemp;

	//hRefTemp = ControlCommand.hSpeedM1;
	//hRefTemp = ControlCommand.hSpeedM2; //��������

//////////////////////����״̬�������Ƴ����ٰ������������ȼ��ٺ���������������Ƴ����ٴ���//////////////
/*
    if( (bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 1) || (bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1))	// 	����״̬�������Ƴ����ٰ������������ȼ��ٺ���������������Ƴ����ٴ���
    {    	
        hRefTemp = MC_MotorEabsStop(M2);
    }
*/		

    if( (bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 1) || (bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1))	// 	����״̬�������Ƴ����ٰ������������ȼ��ٺ���������������Ƴ����ٴ���
    {    	
        hRefTemp = MC_MotorEabsStop(M2);
    }
    else if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 0)	//����ƽ�غ�б����ʻ����
	{
	  //  hRefTemp = MC_RampSpdCompensation(M2);	
		hRefTemp = ControlCommand.hSpeedM1;

//		spd_ref1++;
//		   if(spd_ref1>10000)
//			 {spd_ref1 = 10000;}		
//		hRefTemp = spd_ref1;

//	 ControlCommand.hSpeedM1 = hRefTemp;		
//		hRefTemp = ControlCommand.hSpeedM1;


		if(hRefTemp>32767)
		hRefTemp =32767;
	}	
	else hRefTemp =0;  


    if(hRefTemp > 32767)
    {
    	hRefTemp = 32767;
    }
    else if(hRefTemp < -32767)
    {
    	hRefTemp = -32767;
    }
		
	#ifdef SPEED_MODE
	oSTC[M2]->Vars.hSpeedRef01HzExt =hRefTemp;
	#else 
	oSTC[M2]->Vars.hTorqueRef = hRefTemp;
	#endif		
    

    //if((oHall[M2]->Vars.bBlockageFault != 0)|| (oHall[M2]->Vars.bHallFault != 0))//xu-190628
		if((oEncoder[M2]->Vars.bBlockageFault != 0))
    {
        oSTC[M2]->Vars.hTorqueRef =  0;
		oSTC[M2]->Vars.hSpeedRef01HzExt = 0;

    }

    if(FOCVars[M2].bMotorStop == 0) // FOCVars[M1].bMotorStop 
    {
        FOC_CalcCurrRef(M2);

    }

}
#endif

/*******************************************************************************
* Function Name  : MC_Scheduler
* Description    : ���ҵ����Ƶ�������
* parameters     : None
* Return         : None
*******************************************************************************/

#ifdef DOUBLE_DRIVER
void MC_HiFreqScheduler(void)
{
    static uint8_t bSchedulerPoint=0;
    if (bMCBootCompleted == 1)
    {
        if((bSchedulerPoint&0x01)==0)
            TSK_HighFrequencyTaskM1();
        else
            TSK_HighFrequencyTaskM2();
		
        bSchedulerPoint++;

    }
}
#endif

s32 break_time = 0;
extern uint8_t sto_startfail_flag;
/*******************************************************************************
* Function Name  : TSK_HighFrequencyTask
* Description    : ��Ƶ������
* parameters     : None
* Return         : None
*******************************************************************************/
void TSK_HighFrequencyTask(void)//xu-190627
{
    if(bMCPwmPeriodCompleted == 1)
    {      
        bMCPwmPeriodCompleted = 0;
        
		if((oPwmc[M1]->Vars.bOverCurrentFault == 1) || (ControlCommand.hSpeedM1 == 0) || (oPosSen[M1]->Vars.bBlockageFault == 1) || 
            (bPowerDownFlag == 1 ) || (bOvAbnormalFlag == 1) || (sto_startfail_flag == 214) || (oPwmc[M1]->Vars.bOverCurrentFault1 == 1)) 
        {
            if(FOCVars[M1].bRunning == 1)
            {                
                FOCVars[M1].bRunning = 0;
                break_time = 32000;  //32000��ɲ������				
            }
            break_time--;
            oSTC[M1]->Vars.hTorqueRef = 0;           
			oSTC[M1]->Vars.hSpeedRef01HzExt = 0;
                        if (break_time > 0)   //�ػ�ɲ��
            {
                TIM1->CCR1 = 0;
                TIM1->CCR2 = 0;
                TIM1->CCR3 = 0;
            }
            else
            {
                break_time = 0;			
			    TIM_CtrlPWMOutputs(TIM1, DISABLE);
            }	
			bMcDriverErrFlag = 1; 
            STO_StartUp_Init();             
            STO_Init();              
        }
        else
        {
			bMcDriverErrFlag = 0;
			TSK_HighFrequencyTaskM1();    
        }	
    }
}

/*******************************************************************************
* Function Name  : FOC_Clear
* Description    : ������Ƶ������
* parameters     : None
* Return         : None
*******************************************************************************/
void FOC_Clear(uint8_t bMotor)
{
    Curr_Components Inull = {(int16_t)0, (int16_t)0};
    Volt_Components Vnull = {(int16_t)0, (int16_t)0};

    FOCVars[bMotor].Iab = Inull;
    FOCVars[bMotor].Ialphabeta = Inull;
    FOCVars[bMotor].Iqd = Inull;
    FOCVars[bMotor].Iqdref = Inull;
    FOCVars[bMotor].hTeref = (int16_t)0;
    FOCVars[bMotor].Vqd = Vnull;
    FOCVars[bMotor].Valphabeta = Vnull;
    FOCVars[bMotor].hElAngle = (int16_t)0;

    PI_SetIntegralTerm(oPIDIq[bMotor], (int32_t)0);
    PI_SetIntegralTerm(oPIDId[bMotor], (int32_t)0);

    PI_SetIntegralTerm(oPIDIs[bMotor], (int32_t)0);
    PI_SetIntegralTerm(oPIDVs[bMotor], (int32_t)0);
    PI_SetIntegralTerm(oPIAcc[bMotor], (int32_t)0);
	PI_SetIntegralTerm(oPINTc[bMotor], (int32_t)0);
	PI_SetIntegralTerm(oPIDOvs[bMotor], (int32_t)0);
		
	PI_SetIntegralTerm(oPIDSpeed[bMotor], (int32_t)0);
		
    STC_Clear(oSTC[bMotor]);

	MTPA_Clear(oMTPA[bMotor]);

}


/*******************************************************************************
* Function Name  : FOC_CalcCurrRef
* Description    : ������Ƶ������
* parameters     : None
* Return         : None
*******************************************************************************/
void FOC_CalcCurrRef(uint8_t bMotor)
{
	FOCVars[bMotor].hTeref = STC_CalcTorqueReference(oSTC[bMotor], &FOCVars[bMotor]);
	FOCVars[bMotor].Iqdref.qI_Component1 = FOCVars[bMotor].hTeref;
#ifdef SPEED_MODE     
    if( ((FOCVars[bMotor].bRunning == 0 ) && (oSTC[bMotor]->Vars.hSpeedRef01HzExt != 0)  && (bMagneticBrakeFlag == 0 )) || 
 	 (FOCVars[bMotor].bBlockageStartFlag == 1 && ControlCommand.hSpeedM1 == 0 && ControlCommand.hSpeedM2 == 0 )) //�ٶȻ�
#else
     // FOCVars[bMotor].Iqdref.qI_Component1 = FOCVars[bMotor].hTeref;
	  if((FOCVars[bMotor].bRunning == 0)&&(oSTC[bMotor]->Vars.hTorqueRef != 0)) // û���ٶȻ�
#endif
    {
        FOC_Clear(bMotor);
        MC_MotorStart(bMotor);
        FOCVars[bMotor].bBlockageStartFlag = 0;
    }
#ifdef SPEED_MODE
    else if((FOCVars[bMotor].Vqd.qV_Component1 == 0) && (( oSTC[bMotor]->Vars.hSpeedRef01HzExt == 0 ) || ( bCanCommFault == 1 ))) 
	 // else if((oHall[bMotor]->Vars.hElSpeedDpp ==0)&& (( oSTC[bMotor]->Vars.hSpeedRef01HzExt == 0 ) || ( bCanCommFault==1 ))) 
#else
    else if((FOCVars[bMotor].Vqd.qV_Component1 == 0)&&(( oSTC[bMotor]->Vars.hTorqueRef == 0 ) || ( bCanCommFault==1 )))
#endif
    {
		oSTC[bMotor]->Vars.hTorqueRef = 0; //����ٶȻ���VQΪ0��IQ������Ϊ0����� 
		oSTC[bMotor]->Vars.hOvFlag = 0;
/*
        if(oPosSen[bMotor]->Vars.bBlockageFault == 1) 
        { 
            FOCVars[bMotor].bRunning = 0;
        }
*/        
    }	
}

/*******************************************************************************
* Function Name  : MC_MotorStart
* Description    : �����������
* parameters     : bMotor M1����        M2�ҵ��
* Return         : None
*******************************************************************************/
void MC_MotorStart(uint8_t bMotor)
{
    pPWMCVars_t pPWMCVars = &(oPwmc[bMotor]->Vars);
    pSTCVars_t pSTCVars = &(oSTC[bMotor]->Vars);
    pPOSVars_t pPosVars = &(oPosSen[bMotor]->Vars);
	
    TIM_TypeDef* TIMx = (oPwmc[bMotor]->pParams->TIMx);

    if(Relay.bStatus == 1)
    {
        PWMC_CheckMosfets(oPwmc[bMotor]);    //bdh��ȥ��������������
#ifndef Sensorless
#ifdef SPEED_MODE		
        POS_MeasureInit(oPosSen[bMotor], pSTCVars->hSpeedRef01HzExt);		
#else
	POS_MeasureInit(oPosSen[bMotor], pSTCVars->hTorqueRef);
#endif
#endif
        
        if((pPWMCVars->bOverCurrentFault != 0)
                ||(pPWMCVars->bMosfetHFault != 0)			         
                ||(pPWMCVars->bMosfetLFault != 0)
                ||(pPosVars->bBlockageFault != 0)				
                ||(pSTCVars->bPowerDownFault != 0)
                //||(pPosVars->bInitFault != 0)
                //||(pPWMCVars->bPhaseOffsetFault!= 0)
				   )
        {
            FOCVars[bMotor].bMotorStop = 1;
            //TIM_CtrlPWMOutputs(TIMx,DISABLE);
            return;
        }
        else
        {
            FOCVars[bMotor].bMotorStop = 0;        
        }

        FOC_Init(bMotor);
        FOCVars[bMotor].bRunning = 1;
        TIM_CtrlPWMOutputs(TIMx, ENABLE);   //������ȥ������ô��������ٶȣ�Ҳ����ȥadc�ж�
    }
}


/*******************************************************************************
* Function Name  : FOC_Init
* Description    : FOC ������ʼ����ÿ�ε������ʱ����
* parameters     : bMotor M1����        M2�ҵ��
* Return         : None
*******************************************************************************/
void FOC_Init(uint8_t bMotor)
{
    Volt_Components Valphabeta, Vqd;
    int16_t hElAngledpp;	
#ifdef RUN_BLDC//190716
    int16_t temp;
    uint8_t tempHall;
#endif
	//Vqd.qV_Component1 =oSTC[bMotor]->Vars.hTorqueRef;
#ifdef SPEED_MODE  
   Vqd.qV_Component1 =oSTC[bMotor]->Vars.hTorqueRef;
#else
    Vqd.qV_Component1 = FOCVars[bMotor].hTeref;
#endif		

    Vqd.qV_Component2 = 0;

    FOCVars[bMotor].Vqd = Vqd;

    FOCVars[bMotor].bStartCount = 0;

    Vqd = Circle_Limitation(oCLM[bMotor], Vqd);

    Valphabeta = MCM_Rev_Park(Vqd, hElAngledpp);

#ifdef RUN_BLDC//190716
    temp = oSTC[bMotor]->Vars.hTorqueRef>>3;
    if(temp < 0 )
        temp = -temp;
    if(temp > PWM_PERIOD)
        temp = PWM_PERIOD;
    tempHall = oHall[bMotor]->Vars.bHallRunState;
    tempHall = 7 - tempHall;
    ChangePhase(tempHall, temp, oPwmc[bMotor]->pParams->TIMx);
#else
    PWMC_SetPhaseVoltage(&FOCVars[bMotor],Valphabeta,oPwmc[bMotor]);
#endif

    oPwmc[bMotor]->pParams->TIMx->CCR4 = oPwmc[bMotor]->Vars.Half_PWMPeriod - 1;

    FOCVars[bMotor].Valphabeta = Valphabeta;
    FOCVars[bMotor].hElAngle = hElAngledpp;

    oSTC[bMotor]->Vars.hIsRef = oSTC[bMotor]->pParams->hIsRefDefault + oPwmc[bMotor]->Vars.hBusCurrentOffset;

    CRLM_Start(oCRLM[bMotor]);
    STO_StartUp_Init();
}


/*******************************************************************************
* Function Name  : TSK_DualDriveUpdate
* Description    : ������һ��FOCת���ĵ��
* parameters     : oDrive M1����    M2�ҵ��
* Return         : None
*******************************************************************************/
void TSK_DualDriveUpdate(uint8_t oDrive)
{
    FOC_index = oDrive;
}



/*******************************************************************************
* Function Name  : TSK_GetDriveIndex
* Description    : ��ȡFOCת���ĵ������
* parameters     : None
* Return         : ���ص������
*******************************************************************************/
uint8_t TSK_GetDriveIndex(void)
{
    return FOC_index;
}


/*******************************************************************************
* Function Name  : TB_Init
* Description    : ʱ����ʼ������
* parameters     : None
* Return         : None
*******************************************************************************/
void TB_Init(void)
{
    SysTick_Config(120000);//1ms
}


/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : �δ�ʱ���жϴ�������
* parameters     : None
* Return         : None
*******************************************************************************/
/*
void SysTick_Handler(void)
{
    static u8 Tick1msCnt=1;//1
    static u8 Tick16msCnt=16;

    if(--Tick1msCnt == 0)
    {
        Tick1msCnt = 1;//1
        bMC1msCompleted=1; 						//1ms
    }
    if(--Tick16msCnt == 0)
    {
        Tick16msCnt= 16;
        bMC16msCompleted=1; 					//16ms
    }

}
*/


/*******************************************************************************
* Function Name  : MediumFrequencyTaskM1
* Description    : ������Ƶ���� ��Ƶ��1ms�������ٶȻ�
* parameters     : None
* Return         : None
*******************************************************************************/

void MediumFrequencyTaskM1(void)
{      
	STO_Calc_Speed();   //����ƽ��ת�٣��ж��ٶȿɿ��ԣ�
	        
#ifdef SPEED_MODE //�ٶȻ�PI����ȡ�����ٶȣ�����PI��

    int16_t hReference = 0;
    int16_t hError = 0;
    int16_t hFeedback;
	  
    if(FOCVars[M1].bRunning == 1 ) // || FOCVars[M1].bBlockageCurrent==1
    {
        if(bCanCommFault == 0)						 
        {
            #ifdef Sensorless
//            HandleMode();
//            STO_Obs_Gains_Update();
            #endif
            hReference = oSTC[M1]->Vars.hSpeedRef01HzExt;
            oSTC[M1]->Vars.hPreSpeedRef01HzExt = hReference;			 
        }
        else 
        {
            
        }

        if(RunStoSensorlessMode == 0)
        {
            hFeedback = (-POS_GetAvrgMecSpeed01Hz(oPosSen[M1]));        
        }
        else
        {
            hFeedback = STO_Get_Speed_Hz();    //��е 0.1hz    
        }        
      
        #ifdef LINEAR_HALL
        hError = hReference - (-hFeedback);// 
        #else
        hError = hReference - (hFeedback);// 		       
        #endif	  	 
        hMecSpeed01HZ = hFeedback;
        hMecSpeedRPM  = hFeedback * 6;
//		leehReference = hReference;
        oSTC[M1]->Vars.hTorqueRef = PI_Controller(oSTC[M1]->Vars.oPISpeed, (int32_t)hError);      
    }
#endif
}

/*******************************************************************************
* Function Name  : MediumFrequencyTaskM2
* Description    : �ҵ����Ƶ����
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER

#endif

/*******************************************************************************
* Function Name  : MC_Scheduler
* Description    : ���ҵ����Ƶ�������
* parameters     : None
* Return         : None
*******************************************************************************/

#ifdef DOUBLE_DRIVER

#endif

s32 Ed_sum = 0;
s16 Ed_ave = 0;
s16 Ed_ave_set = -1000; //保护阈值暂定设置为800
u16 Ed_ave_timecount = 0;
u16 Ed_ave_timecountset = 400; //1ms * 400
u16 Ed_ave_count = 0;
u16 Ed_ave_countset = 128; //平均值计算
u16 Ed_ave_errorflag = 0;  //堵转错误标志

u16 over_cur_count = 0;
u16 over_cur_countset = 2500;   //2500ms 500ms

Curr_Components Iqd1;
s32 iq_sum = 0;
u16 iq_ave_count = 0;
u16 iq_ave_countset = 128; //平均值计算
s16 iq_over_set = 4458; // 4458 / 32767 * 147A = 20A ,6687:30A
u16 iq_ave_errorflag = 0;

/*******************************************************************************
* Function Name  : MediumFrequencyTask
* Description    : ��Ƶ����
* parameters     : None
* Return         : None
*******************************************************************************/

void MediumFrequencyTask(void)
{
    bMC1msCompleted = 1;    
    if(bMC1msCompleted == 1)
    {
        bMC1msCompleted = 0;
#ifdef DOUBLE_DRIVER	
        MC_MedFreqScheduler();
#else
        if(Start_Up_State == RAMP_UP || Start_Up_State == LOOPRUN)  //é¢å®ä½çæ¶åä¸æ§è¡éåº¦ç¯
        {
            MediumFrequencyTaskM1();
        }    
#endif

#ifdef DEADBAND_OFFSET_EN
        PWMC_CalcCompensationTime(FOCVars[M1].Vqd, oPwmc[M1]);
        #ifdef DOUBLE_DRIVER
        PWMC_CalcCompensationTime(FOCVars[M2].Vqd, oPwmc[M2]);
        #endif
#endif
//欠压
        if ((hVs < POWERDOWNLEVEL) && (Relay.bReady == 1)) //母线欠压
        {
            if(wPowerDownCnt > 2000)    // 手册欠压延迟时间2s  
            {
                bPowerDownFlag = 1;     //欠压
                wPowerDownCnt = 8000;
            }
            else 
            {
                wPowerDownCnt++;            
            }
        }
        else
        {
            wPowerDownCnt = 0;		  	
//            bPowerDownFlag = 0;                    	  	  	  	
        }
//过压
   	    if(hVs >= OVSREF2 ) //OVSREF OVSREF2	
   	    {
            if(++wOvAbnormalCnt > 2000)  //2s，手册提供的过压延迟时间
            {
                bOvAbnormalFlag = 1;
                wOvAbnormalCnt = 3000;
            }
        }
        else
        {
            wOvAbnormalCnt = 0;
        } 

//堵转保护      
        if(Ed_ave <= Ed_ave_set)
        {
            if(Ed_ave_timecount >= Ed_ave_timecountset)
            {
                Ed_ave_errorflag = 1;
//                oPosSen[M1]->Vars.bBlockageFault = 1;               
            } 
            else
            {
                Ed_ave_timecount++;
            }
        }
        else
        {
            Ed_ave_timecount = 0;    
//            Ed_ave_errorflag = 0;            
        }

//过流保护：软件过流，用iq判断
//        if(FOCVars[M1].Iqd.qI_Component1 >= IQMAX) 
        if(Iqd1.qI_Component1 >= iq_over_set)         
        {
            if(over_cur_count >= over_cur_countset)
            {
                oPwmc[M1]->Vars.bOverCurrentFault1 = 1;                       
            }
            else
            {
                over_cur_count++;
            }                     
        }
        else
        {
            over_cur_count = 0;
//            oPwmc[M1]->Vars.bOverCurrentFault1 = 0;              
        }         
        
        if(oPwmc[M1]->Vars.bOverCurrentFault !=0 ) //硬件过流
        {
            DriverMsg.hDriverFlag.bit.b0 = 1;    
        }
        
        if(oPosSen[M1]->Vars.bBlockageFault != 0) //堵转
        {
            DriverMsg.hDriverFlag.bit.b1 = 1;    
        }
        
        if(oPwmc[M1]->Vars.bMosfetHFault != 0)
        {
            DriverMsg.hDriverFlag.bit.b2 = 1;    
        }
       
        if(oPwmc[M1]->Vars.bMosfetLFault != 0)
        {
            DriverMsg.hDriverFlag.bit.b3 = 1;    
        }
        
        if(Relay.bFail_lv != 0) //启动过程电池欠压
        {
            DriverMsg.hDriverFlag.bit.b4 = 1;    
        }  
        
        if(Relay.bFail != 0)    //预充电失败
        {
            DriverMsg.hDriverFlag.bit.b5 = 1;    
        }  
        
        if(bPowerDownFlag == 1) //电池欠压
        {
            DriverMsg.hDriverFlag.bit.b6 = 1;         
        }
        
        if(bOvAbnormalFlag == 1) //电池过压
        {
            DriverMsg.hDriverFlag.bit.b7 = 1;         
        }        

        if(oPwmc[M1]->Vars.bOverCurrentFault1 !=0 ) //软件过流
        {
            DriverMsg.hDriverFlag.bit.b8 = 1;    
        } 
        
        if(oPwmc[M1]->Vars.bPhaseOffsetFault !=0 ) //电流偏置异常
        {
            DriverMsg.hDriverFlag.bit.b9 = 1;    
        }  
        
//        HandlebarHandle();            //bdh去掉可以
//		MC_MagneticBrakeCheck();	    //bdh去掉可以	
    }
}

/*******************************************************************************
* Function Name  : HandleMagneticBrake
* Description    : ɲ�������������߼�����
* parameters     : None
* Return         : None
*******************************************************************************/
uint16_t hMagneticBrake_Locking_Cnt=0;	
uint16_t hMagneticBrakeOutputStartCnt=0;
void HandleMagneticBrake(void)
{	
	static uint16_t hBreakLockingCnt = 0;    
	static uint16_t hEabsStopCnt=0;

    static uint8_t bMotorEabsStartFlag = 0;
	static uint8_t bMagneticBrakeStaCloseCnt=0;
	static uint8_t bMagneticBrakeStaLooseFlag=0;
	
		//static uint16_t hMagneticBrake_Locking_Cnt=0;

	if(bCanCommFault==1) 
		bCanMagneticBrakeRelease = 0;// ����ͨ�Ź���ʱ���ӽ�������Ҫ���� 2019029 jimianhao    
	
	if((bCanMagneticBrakeRelease) == 0 && (bMagneticBrakeRelease ==0)) //����״̬���綯���У�Ĭ��״̬��
	{  
		#ifdef DOUBLE_DRIVER
		#else
		if(ABS(ControlCommand.hSpeedM1) >30  && bCanCommFault == 0 && bMcDriverErrFlag==0 ) 
		{			
			//GPO_MagneticBrake_Release();
			bBreakLockingFlag = 0;
			hBreakLockingCnt  = 0;
			FOCVars[M1].bMotorEabsStop = 0;			
			FOCVars[M1].bSlopeStaticFlag = 0;
			FOCVars[M1].bSpdRef0Flage=0;
			FOCVars[M1].hDollySpdLimit=0;			

		    hMagneticBrake_Locking_Cnt=0;
			bMagneticBrakeStaLooseRunFlag=0;
            bMotorSlopeStopFlag=0;
			bMototRunStateFlag=0;				
		}

		#endif
		
		#ifdef DOUBLE_DRIVER
		if(((ABS(ControlCommand.hSpeedM1) <30 ) && ( ABS(ControlCommand.hSpeedM2) <30 ) && (bCanCommFault == 0) )||((bCanCommFault == 1) 
		&& ((oSTC[M1]->Vars.hPreSpeedRef01HzExt==0 && oSTC[M2]->Vars.hPreSpeedRef01HzExt==0 ) || (FOCVars[M1].hTeref==0 && FOCVars[M2].hTeref==0 )) ) || (bMcDriverErrFlag==1) ) 
		#else
		if(((ABS(ControlCommand.hSpeedM1) <30 ) &&  (bCanCommFault == 0) )||((bCanCommFault == 1) && ((oSTC[M1]->Vars.hPreSpeedRef01HzExt==0 )||(FOCVars[M1].hTeref==0 ))||(bMcDriverErrFlag==1))) 
		#endif
		{
			if(bMcDriverErrFlag==1 || bCanCommFault == 1)
			{				
				TIM_SetCompare1(TIM3,0);//ռ�ձ�Ϊ0 ��ɲ������
				bMagneticBrakeOutputStartFlag=0;//����ɲ������־
				hMagneticBrakeOutputStartCnt=0;
				bMototRunStateFlag=1;//����ͣ����־
				if(bBreakLockingFlag ==0)				
					bBreakLockingFlag = 1; 
			}
			else
			{
				#ifdef DOUBLE_DRIVER
				if((FOCVars[M1].bMotorEabsStop ==1) && (FOCVars[M2].bMotorEabsStop ==1) && (bCanCommFault == 0) ) //����ƽ�غ�б��ͣ��ɲ��
				#else
				if((FOCVars[M1].bMotorEabsStop ==1) && (bCanCommFault == 0)) //����ƽ�غ�б��ͣ��ɲ��
				#endif
				{

					TIM_SetCompare1(TIM3,0);//ռ�ձ�Ϊ0 ��ɲ������
					bMagneticBrakeOutputStartFlag=0;//����ɲ������־
					hMagneticBrakeOutputStartCnt=0;
					bMototRunStateFlag=1;//����������ֹͣ��־	

					if(  ++ hBreakLockingCnt > MAGNETICBRAKE_DELAY_LOCKING_TIME)				
					{
						hBreakLockingCnt = MAGNETICBRAKE_DELAY_LOCKING_TIME+1;
						if( bBreakLockingFlag ==0)				
						bBreakLockingFlag = 1;            		           		
					}
				}

			}

			#ifdef DOUBLE_DRIVER
			if(( FOCVars[M1].bRunning == 0 ) && ( FOCVars[M2].bRunning == 0 ) && ( ControlCommand.hSpeedM1 ==0 ) && ( ControlCommand.hSpeedM2 ==0 ) )
			#else
			if(( FOCVars[M1].bRunning == 0 ) && ( ControlCommand.hSpeedM1 ==0 ) && ( ControlCommand.hSpeedM2 ==0 ))
			#endif
			{
				if(  ++ hBreakLockingCnt > MAGNETICBRAKE_DELAY_LOCKING_TIME)				
				{
					hBreakLockingCnt = MAGNETICBRAKE_DELAY_LOCKING_TIME+1; 
					bBreakLockingFlag = 1;   
				}
			}
		}

	}		
	else if(( bCanMagneticBrakeRelease == 1 )&&( bMagneticBrakeRelease == 0))	//����״̬��ֻ�����ƣ����ܵ綯	20190812 jimianhao
	{
	
			if(bMagneticBrakeOutputStartFlag==1)												 
			{									
				hMagneticBrakeOutputStartCnt=0;
				bMagneticBrakeRelease = 1;
	 			if(hVs <= 44000)
					TIM_SetCompare1(TIM3,(BREAK_PWM_Period *6/100));
				else
					TIM_SetCompare1(TIM3,(BREAK_PWM_Period *5/100));	
					 
			}	
			else
			{
				if(hMagneticBrakeOutputStartCnt>=10)//250(200ms,1ms����һ��)
				{
					hMagneticBrakeOutputStartCnt=25000;
					bMagneticBrakeOutputStartFlag=1;
				}
				else
				{
					hMagneticBrakeOutputStartCnt++;
					bMagneticBrakeOutputStartFlag=0;	

					if(bMagneticBrakeFlag==0 && bMototRunStateFlag==1)	//bMototRunStateFlag��ֹΪ1					
					// TIM_SetCompare1(TIM3,BREAK_PWM_Period);//3200ռ�ձ�Ϊ100%	
					TIM_SetCompare1(TIM3,(BREAK_PWM_Period *9/10));//3200ռ�ձ�Ϊ90%	
				}
			}
		
		//	bMagneticBrakeRelease = 1;
			TIM_CtrlPWMOutputs(TIM1,DISABLE);			
			FOCVars[M1].bMotorEabsStop = 0;
			FOCVars[M1].bSpdStaticFlag = 0;
			FOCVars[M1].bRunning =0;
			FOCVars[M1].bSpdRef0Flage=0;
			FOCVars[M1].hDollySpdLimit=0;


			#ifdef DOUBLE_DRIVER
			#endif
			
		    bMotorEabsStopFlag=0;
			bBreakLockingFlag = 0;
			hEabsStopCnt  = 0;
			bMotorEabsStartFlag=0;			
		  
	}
	else if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 1) 
	{
     	#ifdef SPEED_MODE  
     	
			FOCVars[M1].bRunning =1;			
			FOCVars[M1].bSpdRef0Flage=1;
			TIM_CtrlPWMOutputs(TIM1,ENABLE);			
     	#endif
			if(FOCVars[M1].bMotorEabsStop ==1)

			{		

				TIM_SetCompare1(TIM3,0);//ռ�ձ�Ϊ0 ��ɲ������
				bMagneticBrakeOutputStartFlag=0;//����ɲ������־
				hMagneticBrakeOutputStartCnt=0;
				bMototRunStateFlag=1;//���ӽ������ϱ�־	
				
				if(++hEabsStopCnt > EabsStop_DELAY_TIME)			  
				{
					hEabsStopCnt = EabsStop_DELAY_TIME+1;
					if(bMotorEabsStopFlag ==0)			  
						bMotorEabsStopFlag = 1; 
					if(bBreakLockingFlag ==0)				
						bBreakLockingFlag = 1;

					#ifdef DOUBLE_DRIVER
					if( FOCVars[M1].Vqd.qV_Component1==0 &&  FOCVars[M2].Vqd.qV_Component1==0)
					#else
					if(FOCVars[M1].Vqd.qV_Component1==0)
					#endif
					{
						bMagneticBrakeRelease = 0;	
						
						FOCVars[M1].bSpdRef0Flage=0;
						
						#ifdef DOUBLE_DRIVER
						FOCVars[M2].bSpdRef0Flage=0;
						#endif
						
						if(bMagneticBrakeFlag==0)	
						{									
							FOC_Clear(M1);							
							MC_MotorStart(M1);	
							
							#ifdef DOUBLE_DRIVER
							FOC_Clear(M2);
							MC_MotorStart(M2);	
							#endif
						}
						TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | (CH1_HON|CH2_HON|CH3_HON |CH1_LON|CH2_LON|CH3_LON);
						
						#ifdef DOUBLE_DRIVER
						TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | (CH1_HON|CH2_HON|CH3_HON |CH1_LON|CH2_LON|CH3_LON);	
						#endif
					}
				}							 			 
			}				
	}
	else //  bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1
	{	
		
		 FOCVars[M1].bSpdRef0Flage=0;
		 
		 #ifdef DOUBLE_DRIVER
		 FOCVars[M2].bSpdRef0Flage=0;
		 #endif
		 
      #ifdef SPEED_MODE	
      	#ifdef DOUBLE_DRIVER
	   	if((DriverMsg.hSpeedM1 > ManualTrolleyMaxSpd3km || DriverMsg.hSpeedM1 < -ManualTrolleyMaxSpd3km) &&
	  	  (DriverMsg.hSpeedM2 > ManualTrolleyMaxSpd3km || DriverMsg.hSpeedM2 < -ManualTrolleyMaxSpd3km))
	  	 #else
		if((DriverMsg.hSpeedM1 > ManualTrolleyMaxSpd3km || DriverMsg.hSpeedM1 < -ManualTrolleyMaxSpd3km))	  	 
	  	 #endif
	  	 {
  	  	    FOCVars[M1].bRunning =1;		    
 		    TIM_CtrlPWMOutputs(TIM1,ENABLE);		    
	  	 }
		 else
		 {}
     #endif
	 
	}
}


/*******************************************************************************
* Function Name  : BlockageFaultReset
* Description    : �����ת���ϱ�־
* parameters     : None
* Return         : None
*******************************************************************************/
#ifdef DOUBLE_DRIVER

#else
void BlockageFaultReset(pPOS_t left,FOCVars_t *GlobalLeft)
{
    if((left->Vars.bBlockageFault == 1)&&(ControlCommand.hSpeedM1 == 0)&&(GlobalLeft->bRunning == 0))	
    {
        left->Vars.bBlockageFault = 0;        
			
		 FOC_Clear(M1);		
         MC_MotorStart(M1);			 
    }
}
#endif

/*******************************************************************************
* Function Name  : LowFrequencyTask
* Description    : ��Ƶ����
* parameters     : None
* Return         : None
*******************************************************************************/
//extern void ADC_ExternalTrigTimerInit(void);
uint16_t bSWDDdisableDelay = 0;
void LowFrequencyTask(void)
{	
    bMC16msCompleted = 1;    
    if(bMC16msCompleted == 1)
    {
//        SystemCoreClockUpdate();    //用来查看实际时钟频率
		bMC16msCompleted = 0;
		 		
        MC_Relay();
			
//        CAN_CommFaultCheck(); //CAN ͨ�Ź����ж� 20190929  jimianhao       bdh��ȥ������ 
        if(Relay.bMosCheck == 1 )//�̵������Ϻ����Լ� 20191028 jimianhao
        {
            MC_FaultDetection();//190716
            Relay.bMosCheck = 0;			
        }

		#ifdef DOUBLE_DRIVER
		BlockageFaultReset(oEncoder[M1], oEncoder[M2], &FOCVars[M1], &FOCVars[M2]);
		#else
//		BlockageFaultReset(oPosSen [M1], &FOCVars[M1]); //bdh��ȥ������
		#endif

//		HandleMagneticBrake(); //bdh��ȥ������

//        NTC_CalcAvTemperature(oNTC[0]);	//bdh��ȥ������		
//        hMCUTemp1 = NTC_GetAvTemp_C(oNTC[0]);		//MCU�¶Ȼ�ȡ  //bdh��ȥ������
/*     
        if(++bcompeletCnt > 40) //�����¿�׼����������ʱ640ms
	   	{
			bcompeletFlag = 1;//׼�������ֱ�־
		    bcompeletCnt = 50;
	   	}
	 
*/        
    }
   
}


/*******************************************************************************
* Function Name  : MC_MotorEabsStop
* Description    : ����״̬�������Ƴ����ٰ������������ȼ��ٺ���������������Ƴ����ٴ���
* parameters     : bMotor M1����        M2�ҵ��
* Return         : �����ƶ����
*******************************************************************************/
int16_t MC_MotorEabsStop(uint8_t bMotor)
{
	static int8_t bCheckFlag[2] = {0,0};
	static int16_t hSpeedBak[2] = {0,0};
    static int16_t hSpeedRefTemp[2] = {0,0};
	static int16_t MC_StopDelayCnt[2] = {0,0};

	int16_t hSpeedTemp;
	int16_t hSpeedRef = 0;
	
//	 hSpeedTemp = POS_GetAvrgMecSpeed01Hz(oPosSen[bMotor]);

	#ifdef DOUBLE_DRIVER

	 if(bMotor==M1)
	    hSpeedTemp =DriverMsg.hSpeedM1;
	  else
	  	hSpeedTemp =DriverMsg.hSpeedM2;

  	#else
		hSpeedTemp =DriverMsg.hSpeedM1;
  	#endif
	  
#ifndef SPEED_MODE //����

     if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 1) //����״̬�������Ƴ����ٰ������������ȼ��ٺ��������
     {
        if(FOCVars[bMotor].bMotorEabsStop == 0)
        {        
        	hSpeedTemp = POS_GetAvrgMecSpeed01Hz(oPosSen[bMotor]);
            hSpeedBak[bMotor]= hSpeedTemp;
        
        	if( hSpeedBak[bMotor] > 0)
        	{
        	   // hSpeedRefTemp[bMotor] -=2;       		        		       		
        	    hSpeedRefTemp[bMotor] -= 2;       		
        		if(hSpeedRefTemp[bMotor] <= -unlock_spd_ref_max)        		
        		   hSpeedRefTemp[bMotor] = -unlock_spd_ref_max;       		       		
        	}
            if( hSpeedBak[bMotor] < 0)
        	{
        	   //hSpeedRefTemp[bMotor] += 2;     		
        		hSpeedRefTemp[bMotor] += 2;       		
        		if(hSpeedRefTemp[bMotor] >= unlock_spd_ref_max)          		
        			hSpeedRefTemp[bMotor] = unlock_spd_ref_max;        		       		
        	}
			
			if(++MC_StopDelayCnt[bMotor]>spd0_ref_time)//����3000,  �ջ�5000
			{
				FOCVars[bMotor].bMotorEabsStop = 1 ; 
				MC_StopDelayCnt[bMotor]=0;
			}
             // hSpeedRef = hSpeedRefTemp[bMotor];           
        }
		else
		{
		  if(bMotorEabsStopFlag==1)
		  {												  
    		  if(hSpeedRefTemp[bMotor]>0)				  
    			hSpeedRefTemp[bMotor] -=2;//2				  
    		  else				  
    			hSpeedRefTemp[bMotor] +=2;//2
    			
    		  if(hSpeedRefTemp[bMotor]<=4 && hSpeedRefTemp[bMotor]>=-4)
    		  {
    			hSpeedRefTemp[bMotor]=0;
    			FOCVars[bMotor].bSpdStaticFlag=1;
    		  }						  
		  }
		}	
	   //hSpeedRef = hSpeedRefTemp[bMotor];
     }
	 
	 if( bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1 ) //����״̬�������Ƴ����ٴ���
	 {
        hSpeedTemp = POS_GetAvrgMecSpeed01Hz(oPosSen[bMotor]);
        
        if(hSpeedTemp > ManualTrolleyMaxSpd3km || hSpeedTemp < -ManualTrolleyMaxSpd3km)
        {
          if(hSpeedTemp>0) 
             hSpeedRefTemp[bMotor] -=2; 
          else
             hSpeedRefTemp[bMotor] +=2;  
        }
        else 
        {
           if(hSpeedRefTemp[bMotor]!=0)
           {
             if(hSpeedRefTemp[bMotor]<0)
               hSpeedRefTemp[bMotor] +=2;
             else
               hSpeedRefTemp[bMotor] -=2;                   
             
             if(hSpeedRefTemp[bMotor]<10 && hSpeedRefTemp[bMotor]>-10)
                hSpeedRefTemp[bMotor]=0; 
           }
           else
           {

           #ifdef DOUBLE_DRIVER
             if(bMotor==M1)
                TIM_CtrlPWMOutputs(TIM1,DISABLE);
             else 
                TIM_CtrlPWMOutputs(TIM8,DISABLE);
          #else
				TIM_CtrlPWMOutputs(TIM1,DISABLE);
          #endif
           }
        }
         //hSpeedRef = hSpeedRefTemp[bMotor];	 
	 }
#else
     hSpeedRefTemp[bMotor]= 0;		  
#endif	

     hSpeedRef = hSpeedRefTemp[bMotor];

	return( hSpeedRef );

}



/*******************************************************************************
* Function Name  : FOC_CurrController
* Description    : FOC�任����
* parameters     : oDrive M1����    M2�ҵ��
* Return         : None
*******************************************************************************/
int16_t hSpeedBak1[2] = {0,0};
int16_t MC_StopDelayCnt2[2] = {0,0};
int16_t MC_StopDelayCnt3[2] = {0,0};

int16_t MC_StartVq[2] = {0,0};
int16_t MC_StartCompleteFlag[2] = {0,0};
int16_t hDollyCnt[2] = {0,0};
int8_t RunStoSensorlessMode =1;  //

int16_t hElAngledppShift=0;
uint8_t uart_out = 100;
uint8_t uart_out_cyc = 11;
s16 est_speed_dpp = 0;
s16 est_speed_01hz = 0;
s16 est_pos_dpp = 0;
s16 hForceElAngle_test = 0;
u16 tim6_count3 = 0;
u16 tim6_count4 = 0;
u16 tim6_countdelta2 = 0;
Curr_Components Bemf_alphabeta, Bemf_qd;

extern s16 hBemf_alfa_est, hBemf_beta_est;
#pragma inline
void FOC_CurrController(uint8_t bMotor)
{
    TIM_TypeDef* TIMx = (oPwmc[bMotor]->pParams->TIMx);
    Curr_Components Iab, Ialphabeta, Iqd;
    //Volt_Components Valphabeta, Vqd;
    int32_t wProcessError;
    int16_t hElAngledpp;
#ifdef UART_DEBUG
    static uint8_t bUARTCnt = 0;
    extern uint8_t bHalltxxx;
    extern int16_t hAccRegulatorOut;
    extern int16_t hIsRegulatorOut;
    extern int16_t hVsRegulatorOut;
#endif
	
#ifdef RUN_BLDC//190716
    int16_t temp;
    uint8_t tempHall;
#endif

    if(FOCVars[bMotor].bRunning == 1 ) // || FOCVars[bMotor].bBlockageCurrent==1
    {       
        STO_Start_Up();

        if(RunStoSensorlessMode == 0)
            hElAngledpp = POS_GetElAngle(oPosSen[bMotor]);
        else
        {
            if((Start_Up_State == RAMP_UP)||(Start_Up_State == ALIGNMENT)||(Start_Up_State == S_INIT))//
                hElAngledpp = hForceElAngle;
            else if (Start_Up_State == LOOPRUN)//
                hElAngledpp = STO_Get_Electrical_Angle() + hElAngledppShift;					
        }
        leeEIAngle = hElAngledpp;
        leeHallAngle = POS_GetElAngle(oPosSen[bMotor]);
        est_speed_dpp = STO_Get_Speed();  
        est_speed_01hz = STO_Get_Speed_Hz();       
        hForceElAngle_test = hForceElAngle;
        est_pos_dpp = STO_Get_Electrical_Angle();
        
        Iab = PWMC_GetPhaseCurrents(oPwmc[bMotor]);
//        Iab.qI_Component1 = 7 * Iab.qI_Component1;
//        Iab.qI_Component2 = 7 * Iab.qI_Component2;           
        Ialphabeta = MCM_Clarke(Iab);
        Iqd = MCM_Park(Ialphabeta, hElAngledpp);

        Bemf_alphabeta.qI_Component1 = hBemf_alfa_est;
        Bemf_alphabeta.qI_Component2 = hBemf_beta_est;        
        Bemf_qd = MCM_Park(Bemf_alphabeta, hElAngledpp);
        if(Ed_ave_count >= Ed_ave_countset)
        {
            Ed_ave = Ed_sum / Ed_ave_countset; //Ed_sum >> 7;
            Ed_ave_count = 0;
            Ed_sum = 0;
        }
        else
        {
            Ed_ave_count++;
            Ed_sum += Bemf_qd.qI_Component2;
        } 
        
        if(RunStoSensorlessMode != 0)
		{
            if (Start_Up_State != LOOPRUN)
            {
                //����LOOPRUN�����ֵ�����ٶȸ���ֵ�������������ǰ׷����������
                FOCVars[bMotor].Iqdref.qI_Component1 = hForceTorque_Reference;
            }
            else
            {

            }
		}
        //��һ����ʲô���ã����������˲���
        Iqd.qI_Component1 = (Iqd.qI_Component1*6+ FOCVars[bMotor].Iqd.qI_Component1*10)>>4;
        Iqd.qI_Component2 = (Iqd.qI_Component2*6+ FOCVars[bMotor].Iqd.qI_Component2*10)>>4;
        if(iq_ave_count >= iq_ave_countset)
        {
            Iqd1.qI_Component1 = iq_sum / iq_ave_countset; //Ed_sum >> 7;
            iq_ave_count = 0;
            iq_sum = 0;
        }
        else
        {
            iq_ave_count++;
            iq_sum += Iqd.qI_Component1;
        }         
#ifdef SPEED_MODE

		#ifdef DOUBLE_DRIVER	
		
		if(bMotor==M1)
			  hSpeedBak1[bMotor] =DriverMsg.hSpeedM1;
		else
			  hSpeedBak1[bMotor] =DriverMsg.hSpeedM2;

		#else

			hSpeedBak1[bMotor] =DriverMsg.hSpeedM1;

        #endif

		 if( FOCVars[bMotor].bSpdRef0Flage==1)
		 {

           if(FOCVars[bMotor].bMotorEabsStop ==0)
           {
				if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 1) 
				{
					FOCVars[bMotor].hDollySpdLimit += 1; 
					if(FOCVars[bMotor].hDollySpdLimit>(PWM_PERIOD-1)) 
					{
						FOCVars[bMotor].hDollySpdLimit =PWM_PERIOD-1;}  //2000 PWM_PERIOD-1						
						MC_BreakerHandle(bMotor);
					}
			   		else
			   		{
			     		if(hSpeedBak1[bMotor]<0) //0 -300
	             		{          		          
		                	FOCVars[bMotor].Vqd.qV_Component1 += 4;//2		   
		                	if(FOCVars[bMotor].Vqd.qV_Component1 >= unlock_spd_ref_max)	 		   
			              		FOCVars[bMotor].Vqd.qV_Component1 = unlock_spd_ref_max;	   		  
	             		}
	             		if(hSpeedBak1[bMotor]>0)  //0  300
	             		{			 	         		
		           			FOCVars[bMotor].Vqd.qV_Component1 -= 4;//2			
		          			if(FOCVars[bMotor].Vqd.qV_Component1 <= -unlock_spd_ref_max)		
		              			FOCVars[bMotor].Vqd.qV_Component1 = -unlock_spd_ref_max;			
	             		}				 
					}
						 
			  		if(++MC_StopDelayCnt2[bMotor]>2000)//4000
			  		{
						FOCVars[bMotor].bMotorEabsStop = 1 ;	
						MC_StopDelayCnt2[bMotor]=0;
					
						if(FOCVars[bMotor].bCanMagneticBrakeState==1 ) 
				 		{					
					 		if( FOCVars[bMotor].Vqd.qV_Component1<3000 &&  FOCVars[bMotor].Vqd.qV_Component1>-3000) //2500						
					  			FOCVars[bMotor].bSlopeStaticFlag = 0; //�����ƽ·�Ͼ�ֹ					
				   			else
					 			FOCVars[bMotor].bSlopeStaticFlag = 1; //�����б���Ͼ�ֹ					               
				 		}					
			  		}
            	}
				else
	        	{
	           		if(bBreakLockingFlag==0)//1  ɲ�����Ϻ���ʱ
	           		{         
						//if(  FOCVars[bMotor].Vqd.qV_Component1<2500 &&  FOCVars[bMotor].Vqd.qV_Component1>-2500) //2500
						#ifdef LINEAR_HALL
						if(  FOCVars[bMotor].Vqd.qV_Component1<3500 &&  FOCVars[bMotor].Vqd.qV_Component1>-3500)
						#else
						if(  FOCVars[bMotor].Vqd.qV_Component1<3500 &&  FOCVars[bMotor].Vqd.qV_Component1>-3500)  //2500
						#endif			 
						{
              				if( FOCVars[bMotor].Vqd.qV_Component1>0)		//�����ɲ��	  
								FOCVars[bMotor].Vqd.qV_Component1 -=2;//2				  
				      		else
					    		FOCVars[bMotor].Vqd.qV_Component1 +=2;//2
				  
				     		if( FOCVars[bMotor].Vqd.qV_Component1<=4 &&  FOCVars[bMotor].Vqd.qV_Component1>=-4)			  
					   		{
					     		FOCVars[bMotor].Vqd.qV_Component1=0;
				   	   		}
						 	FOCVars[bMotor].bSlopeStaticFlag = 0; //�����ƽ·�Ͼ�ֹ
						}
						else
						{
							FOCVars[bMotor].bSlopeStaticFlag = 1;} //�����б���Ͼ�ֹ					
                                    
							if(FOCVars[M1].Vqd.qV_Component1==0 && FOCVars[M2].Vqd.qV_Component1!=0 )
					  			FOCVars[M2].Vqd.qV_Component1=0;
							if(FOCVars[M2].Vqd.qV_Component1==0 && FOCVars[M1].Vqd.qV_Component1!=0 )
					  			FOCVars[M1].Vqd.qV_Component1=0;	
                   
			         
	            		}
			    		else
			    		{				 
							// if(ABS(hSpeedBak1[bMotor])>=420 && FOCVars[bMotor].bCanMagneticBrakeState==1)              
							if(hSpeedBak1[bMotor]!=0 && FOCVars[bMotor].bCanMagneticBrakeState==1) 	              								
				         		FOCVars[bMotor].bMotorEabsStop = 0 ;				  
                  
				 
							if( FOCVars[bMotor].bSlopeStaticFlag ==0 ) //ƽ·��ͣ������Ϊ0����PWM�ܿ���ʡ��
							{
								if( FOCVars[bMotor].Vqd.qV_Component1>0)			  
									FOCVars[bMotor].Vqd.qV_Component1 -=2;//2				  
								else
									FOCVars[bMotor].Vqd.qV_Component1 +=2;//2

								if( FOCVars[bMotor].Vqd.qV_Component1<=4 &&  FOCVars[bMotor].Vqd.qV_Component1>=-4)			  				   
								{
									FOCVars[bMotor].Vqd.qV_Component1=0;				   
									TIM_CtrlPWMOutputs(TIMx,DISABLE);//��ƽ·��ͣ����PWM�ܿ���,��б���ϱ���,������һ������������,���鲻��
									FOCVars[bMotor].bRunning =0;					   
								}
							}
				  			else
						  	{
			                    if(FOCVars[bMotor].bCanMagneticBrakeState==0) //б����ͣ������Ϊ0������PWM�ܿ�����б���ϱ���,������һ���������������鲻��,����ʻ���ֶ�ɲ��,��ͣ������,VQһֱ���ַ������
			                    {
									if( FOCVars[bMotor].Vqd.qV_Component1>0)		  
										FOCVars[bMotor].Vqd.qV_Component1 -=2;//2				  
									else
										FOCVars[bMotor].Vqd.qV_Component1 +=2;//2

									if( FOCVars[bMotor].Vqd.qV_Component1<=4 &&  FOCVars[bMotor].Vqd.qV_Component1>=-4)			  				   					  
										FOCVars[bMotor].Vqd.qV_Component1=0;				   	  
			                    }
									
						  }
			    	}
            	}	
				Vqd.qV_Component1 = FOCVars[bMotor].Vqd.qV_Component1;
				FOCVars[bMotor].hSpdRef0Cnt=0;
		  }
		  else
		  {
			  //if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease ==0) 	
			  if( (bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease ==0) && (bMagneticBrakeStaLooseRunFlag==0)) 
			  {        									 
				 wProcessError = (int32_t)(FOCVars[bMotor].Iqdref.qI_Component1) - Iqd.qI_Component1;
	             Vqd.qV_Component1 = PI_Controller(oPIDIq[bMotor],wProcessError); 
	             //if( ( oSTC[M1]->Vars.hSpeedRef01HzExt==0 && oSTC[M2]->Vars.hSpeedRef01HzExt==0 && bCanCommFault == 0 ) ||    // oSTC[M1]->Vars.hSpeedRef01HzExt==0 && oSTC[M2]->Vars.hSpeedRef01HzExt==0
		            // (oSTC[M1]->Vars.hPreSpeedRef01HzExt==0 && oSTC[M2]->Vars.hPreSpeedRef01HzExt==0 && bCanCommFault == 1))// ControlCommand.hSpeedM1==0 && ControlCommand.hSpeedM2==0
				if( ( ABS(oSTC[M1]->Vars.hSpeedRef01HzExt)<30 && ABS(oSTC[M2]->Vars.hSpeedRef01HzExt)<30 && bCanCommFault == 0 ) ||    // oSTC[M1]->Vars.hSpeedRef01HzExt==0 && oSTC[M2]->Vars.hSpeedRef01HzExt==0
					(ABS(oSTC[M1]->Vars.hPreSpeedRef01HzExt)<30 && ABS(oSTC[M2]->Vars.hPreSpeedRef01HzExt)<30 && bCanCommFault == 1))
				{ 		   
					if(FOCVars[bMotor].hSpdRef0Cnt>2000)//3000  2000  4000
					{
						FOCVars[bMotor].bSpdRef0Flage=1;
						MC_StartCompleteFlag[bMotor]=1;
					}
					else
						FOCVars[bMotor].hSpdRef0Cnt++;
					}
		            MC_StopDelayCnt2[bMotor]=0;	
			       	 
			  }			  	

			  if( (bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1) || (bMagneticBrakeStaLooseRunFlag==1)) //����״̬�������Ƴ����ɿ�ɲ���ֶ��Ƴ����ٴ���		  
              {
                    FOCVars[bMotor].bSpdRef0Flage=0;
              }			  			  	
			
		  }
#else
        if(FOCVars[bMotor].Iqdref.qI_Component1>0)     //���������
        {
            wProcessError = (int32_t)(IQMAX) - Iqd.qI_Component1;
            Vqd.qV_Component1 = PI_Controller(oPIDIq[bMotor],wProcessError);
            if((Vqd.qV_Component1 > FOCVars[bMotor].Iqdref.qI_Component1)||(Vqd.qV_Component1<0))
                Vqd.qV_Component1 = FOCVars[bMotor].Iqdref.qI_Component1;
        }
        else if(FOCVars[bMotor].Iqdref.qI_Component1<0)
        {
            wProcessError = (int32_t)(-IQMAX) - Iqd.qI_Component1;
            //hVqLimit = PI_Controller(oPIDIq[bMotor],wProcessError);
            //Vqd.qV_Component1  = hVqLimit;
            Vqd.qV_Component1 = PI_Controller(oPIDIq[bMotor],wProcessError);
            //---------------------------------------------------------------------------------?IQ elfern 191014
            if((Vqd.qV_Component1 < FOCVars[bMotor].Iqdref.qI_Component1)||(Vqd.qV_Component1>0))
                Vqd.qV_Component1 = FOCVars[bMotor].Iqdref.qI_Component1;
        }
        else Vqd.qV_Component1 = FOCVars[bMotor].Iqdref.qI_Component1;

        //FOCVars[bMotor].IqdHF.qI_Component1 = FOCVars[bMotor].Iqdref.qI_Component1;
#endif				

#if 0//(MTPA_ENABLING == ENABLE)  // 
        MTPA_CalcCurrRef(oMTPA[bMotor], &FOCVars[bMotor]);
        //FOCVars[bMotor].Iqdref.qI_Component2 = 0;
#else
        FOCVars[bMotor].Iqdref.qI_Component2 = 0;
#endif

        wProcessError = (int32_t)(FOCVars[bMotor].Iqdref.qI_Component2) - Iqd.qI_Component2;

        if (Start_Up_State != LOOPRUN)
//        if (Start_Up_State == S_INIT || Start_Up_State == ALIGNMENT)
        {
            Vqd.qV_Component2 = 0;
        }
        else
        {
            Vqd.qV_Component2 = PI_Controller(oPIDId[bMotor], wProcessError);
        }
        FOCVars[bMotor].Vqd = Vqd;

        Vqd = Circle_Limitation(oCLM[bMotor], Vqd);

        Valphabeta = MCM_Rev_Park(Vqd, hElAngledpp);

#ifdef RUN_BLDC//190716

#else
      //  PWMC_SetPhaseVoltage(&FOCVars[bMotor],Valphabeta,oPwmc[bMotor]);
      // if( bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1 ) //����״̬�������Ƴ����ٴ���
       if( (bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1 ) || 
	   	    ((FOCVars[M1].bCanMagneticBrakeState==1 && FOCVars[M2].bCanMagneticBrakeState==1) && bBreakLockingFlag ==1 &&  FOCVars[M1].Vqd.qV_Component1==0 &&  FOCVars[M2].Vqd.qV_Component1==0
			     && (bMagneticBrakeStaLooseRunFlag==1) )
	   	  )
       	{
					 
#ifdef SPEED_MODE
	      // if( (hSpeedBak1[bMotor] > ManualTrolleyMaxSpd3km) || (hSpeedBak1[bMotor] < -ManualTrolleyMaxSpd3km) ) //  hSpeedBak1[bMotor] DriverMsg.hSpeedM2
          if(hDollyCnt[bMotor]>10)
          {
		    if(((DriverMsg.hSpeedM1 > ManualTrolleyMaxSpd3km)  && (DriverMsg.hSpeedM2 > ManualTrolleyMaxSpd3km) )|| 
		  	   ((DriverMsg.hSpeedM1 < -ManualTrolleyMaxSpd3km) && (DriverMsg.hSpeedM2 < -ManualTrolleyMaxSpd3km) )) 
             {
			    FOCVars[bMotor].hDollySpdLimit += 1; //1
			    if(FOCVars[bMotor].hDollySpdLimit>(PWM_PERIOD-1)) //2000
			  	  FOCVars[bMotor].hDollySpdLimit =PWM_PERIOD-1;	//2000	PWM_PERIOD-1
             }
		    else
		     {	   	  
			    if(FOCVars[bMotor].hDollySpdLimit<=10)
			       FOCVars[bMotor].hDollySpdLimit=0;
			    else
				   FOCVars[bMotor].hDollySpdLimit -=1; 
		      }
			hDollyCnt[bMotor]=0;
          }
		  else
		  {
		  	hDollyCnt[bMotor]++;  
          }        
            MC_BreakerHandle(bMotor);	  	
#endif
       }
	   else	
	   {
			if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 0) 
			{
			   PWMC_SetPhaseVoltage(&FOCVars[bMotor], Valphabeta, oPwmc[bMotor]);
			   hDollyCnt[bMotor] = 0;
			}		
	   }
#endif

        if(FOCVars[bMotor].bStartCount < 7)
            FOCVars[bMotor].bStartCount++;

        FOCVars[bMotor].Iab = Iab;
        FOCVars[bMotor].Ialphabeta = Ialphabeta;
        FOCVars[bMotor].Iqd = Iqd;
        FOCVars[bMotor].Valphabeta = Valphabeta;
        FOCVars[bMotor].hElAngle = hElAngledpp;
#ifdef Sensorless
        //leeIsObserverConverged=IsObserverConverged();
        tim6_count3 = TIM6->CNT;          
        if(Start_Up_State == RAMP_UP || Start_Up_State == LOOPRUN)  
        {
            STO_Calc_Rotor_Angle(FOCVars[bMotor].Valphabeta, Ialphabeta, hVrelay);
        } 
        tim6_count4 = TIM6->CNT;
        tim6_countdelta2 = tim6_count4 - tim6_count3;          
#endif
        
#ifdef UART_DEBUG       
        if(bUARTCnt > uart_out_cyc) //  9  17
        {
            bUARTCnt = 0;

		 //  UARTC_PutChar(oPosSen[M1]->Vars.hElAngle >> 8); 
		 //  UARTC_PutChar(oPosSen[M1]->Vars.hElAngle);
            u8 temp[8];
            if(RunStoSensorlessMode==0)
            temp[0] =0xAA;
            else 
            temp[0] =0x66;
            
            temp[1] =hElAngledpp>>8;//FOCVars[M1].Ialphabeta.qI_Component1 >>8;//0xBB;Iab
            temp[2] =hElAngledpp;//FOCVars[M1].Ialphabeta.qI_Component1;//0xCC;Ialphabeta
            UARTC_PutChar(temp[0]);
            UARTC_PutChar(temp[1]);   
            UARTC_PutChar(temp[2] >>8);                    
            if(uart_out == 100)
            {
                UARTC_PutChar(hForceElAngle_test >> 8);
                UARTC_PutChar(hForceElAngle_test);   
//                UARTC_PutChar(est_pos_dpp >>8);
//                UARTC_PutChar(est_pos_dpp);   
//                UARTC_PutChar((est_speed_dpp * 10) >> 8);
//                UARTC_PutChar((est_speed_dpp * 10)); 
                UARTC_PutChar((est_speed_01hz * 10) >> 8);
                UARTC_PutChar((est_speed_01hz * 10));                              
            }
            else if(uart_out == 101)
            {
                UARTC_PutChar(hForceElAngle >> 8);
                UARTC_PutChar(hForceElAngle);   
                UARTC_PutChar(est_speed_dpp >>8);
                UARTC_PutChar(est_speed_dpp);                
            }          
            else if(uart_out == 0)
            {
                UARTC_PutChar(FOCVars[M1].Iab.qI_Component1 >>8);
                UARTC_PutChar(FOCVars[M1].Iab.qI_Component1 );     
                UARTC_PutChar(FOCVars[M1].Iab.qI_Component2 >>8);
                UARTC_PutChar(FOCVars[M1].Iab.qI_Component2 );                
            }
            else if(uart_out == 1)
            {
                UARTC_PutChar(FOCVars[M1].Ialphabeta.qI_Component1 >>8);
                UARTC_PutChar(FOCVars[M1].Ialphabeta.qI_Component1 );    
                UARTC_PutChar(FOCVars[M1].Ialphabeta.qI_Component2 >>8);
                UARTC_PutChar(FOCVars[M1].Ialphabeta.qI_Component2 );                
            }
            else if(uart_out == 2)
            {
                UARTC_PutChar(FOCVars[M1].Iqd.qI_Component1>>8); 
                UARTC_PutChar(FOCVars[M1].Iqd.qI_Component1);   
                UARTC_PutChar(FOCVars[M1].Iqd.qI_Component2>>8); 
                UARTC_PutChar(FOCVars[M1].Iqd.qI_Component2);                 
            }
            else if(uart_out == 3)
            {
                UARTC_PutChar(FOCVars[M1].Iqdref.qI_Component1>>8); 
                UARTC_PutChar(FOCVars[M1].Iqdref.qI_Component1);	               
                UARTC_PutChar(FOCVars[bMotor].hTeref >>8); 
                UARTC_PutChar(FOCVars[bMotor].hTeref);	                
//                UARTC_PutChar(FOCVars[M1].Iqdref.qI_Component2>>8); 
//                UARTC_PutChar(FOCVars[M1].Iqdref.qI_Component2);	               
            }
            else if (uart_out == 4)
            {
                UARTC_PutChar(FOCVars[M1].Vqd.qV_Component1  >>8); 
                UARTC_PutChar(FOCVars[M1].Vqd.qV_Component1 ); 
                UARTC_PutChar(FOCVars[M1].Vqd.qV_Component2  >>8); 
                UARTC_PutChar(FOCVars[M1].Vqd.qV_Component2 );                 
            }
            else if (uart_out == 5)
            {
                UARTC_PutChar(FOCVars[M1].Valphabeta.qV_Component1  >>8); 
                UARTC_PutChar(FOCVars[M1].Valphabeta.qV_Component1 ); 
                UARTC_PutChar(FOCVars[M1].Valphabeta.qV_Component2  >>8); 
                UARTC_PutChar(FOCVars[M1].Valphabeta.qV_Component2 );               
            } 
            else if (uart_out == 6)
            {
                UARTC_PutChar(hBemf_alfa_est >> 8); 
                UARTC_PutChar(hBemf_alfa_est); 
                UARTC_PutChar(hBemf_beta_est >> 8); 
                UARTC_PutChar(hBemf_beta_est);               
            }                          
        }
        else
            bUARTCnt ++;

#endif        
        
#ifdef JlinkSCope
        acValBuffer.Reveal1 = hElAngledpp;
        acValBuffer.Reveal2 = hElAngledpp;
        ///acValBuffer.Reveal6 = FOCVars[M1].Iqdref.qI_Component2;
        // acValBuffer.Reveal2 = StateM1;
//      SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
#endif

    }
}

/*******************************************************************************
* Function Name  :  MC_RampSpdCompensation
* Description    :  б�·����´���
* parameters     : bMotor M1����        M2�ҵ��
* Return         : �����ƶ����
*******************************************************************************/
int16_t hSpeedRefTemp1[2] = {0,0};
//int16_t hSpeedBak1[2] = {0,0};
int16_t hSpeedTemp1;

int16_t MC_RampSpdCompensation(uint8_t bMotor)
{
	//static int8_t bCheckFlag[2] = {0,0};
	//static int16_t hSpeedBak[2] = {0,0};

	//int16_t hSpeedTemp;
    int16_t hSpeedRef = 0;
	//static int16_t hSpeedRefTemp[2] = {0,0};
	static int16_t max_spd_ref[2] = {0,0};
	static int16_t MC_StopDelayCnt[2] = {0,0};
	static int16_t hLimitSpdRefTemp[2] = {0,0};

   if(bMotor==M1)
     hSpeedTemp1 =DriverMsg.hSpeedM1;
   else
     hSpeedTemp1 =DriverMsg.hSpeedM2;
	
  if( ControlCommand.hSpeedM1==0 && ControlCommand.hSpeedM2==0 )	
  {
  
#ifndef SPEED_MODE //����
	 if(FOCVars[bMotor].bMotorEabsStop == 0)
	 {		
		
		hSpeedBak1[bMotor] = hSpeedTemp1;
		if(hSpeedBak1[bMotor]<0)
		{
           //hSpeedRefTemp1[bMotor] += 2;//2			    
		   hSpeedRefTemp1[bMotor] += 2;//2		   
		   if(hSpeedRefTemp1[bMotor] >= unlock_spd_ref_max)	   
			  hSpeedRefTemp1[bMotor] = unlock_spd_ref_max;	   		  
		}
		if(hSpeedBak1[bMotor]>0)
		{
			//hSpeedRefTemp1[bMotor] -= 2;//2    		  			
			hSpeedRefTemp1[bMotor] -= 2;//2			
			if(hSpeedRefTemp1[bMotor] <= -unlock_spd_ref_max)				
			  hSpeedRefTemp1[bMotor] = -unlock_spd_ref_max;			
		}
		
		if(++MC_StopDelayCnt[bMotor]>3000)//����3000,  �ջ�5000
		{
			FOCVars[bMotor].bMotorEabsStop = 1 ;	
			MC_StopDelayCnt[bMotor]=0;
		}
		
	 }
	 else
	 {
	    if(bBreakLockingFlag==1)//ɲ�����Ϻ���ʱ
	     {										
			if( hSpeedRefTemp1[bMotor]<2500 && hSpeedRefTemp1[bMotor]>-2500)  //2500 ����ɲ��ʱȡ���ü����ж�,��������ƽ�ػ���б��,ɲ�����Ϻ󲹳�ֵ��Ҫ��0
			{
			  if(hSpeedRefTemp1[bMotor]>0)
			  {
				hSpeedRefTemp1[bMotor] -=2;//2
			  }
			  else
			  {
				hSpeedRefTemp1[bMotor] +=2;//2
			  }
			  if(hSpeedRefTemp1[bMotor]<=4 && hSpeedRefTemp1[bMotor]>=-4)
			  {
				hSpeedRefTemp1[bMotor]=0;
			  }
			  FOCVars[bMotor].bSlopeStaticFlag = 0; //�����ƽ·�Ͼ�ֹ
			}
            else // ����ɲ��ʱȡ���ü����ж�,��������ƽ�ػ���б��,ɲ�����Ϻ󲹳�ֵ��Ҫ��0
		   	{
		   	  FOCVars[bMotor].bSlopeStaticFlag = 1;//�����б���Ͼ�ֹ,����ɲ��ʱȡ���ñ�־λ�ж�			 
		   	  if(bMotor==M1) Slope_static_flagM1=1;//���M1��б���Ͼ�ֹ,����ɲ��ʱȡ���ñ�־λ�ж�		
			  else Slope_static_flagM2=1;//���M2��б���Ͼ�ֹ,����ɲ��ʱȡ���ñ�־λ�ж�		
			  
			  if(Slope_static_flagM1==1 && Slope_static_flagM2==0)hSpeedRefTemp1[M2]=hSpeedRefTemp1[M1];
			  if(Slope_static_flagM1==0 && Slope_static_flagM2==1)hSpeedRefTemp1[M1]=hSpeedRefTemp1[M2]; 				
           	}
           /*
              if(hSpeedRefTemp1[bMotor]>0)	//�����ɲ��	  	  
				hSpeedRefTemp1[bMotor] -=2;//2			  
			  else			  
				hSpeedRefTemp1[bMotor] +=2;//2	
				
			  if(hSpeedRefTemp1[bMotor]<=4 && hSpeedRefTemp1[bMotor]>=-4)			  
				hSpeedRefTemp1[bMotor]=0;			  
           */
			
	     }
     }
#else
     /* if(FOCVars[bMotor].bSpdRef0Flage==1)
	  	hSpeedRefTemp1[bMotor]=0;//0 (FOCVars[bMotor].Vqd.qV_Component1)/2
	  else
	  	hSpeedRefTemp1[bMotor]=0;*/
	  	hSpeedRefTemp1[bMotor]=0;
#endif		 
     hSpeedRef = hSpeedRefTemp1[bMotor];
  }
  else
  {
     #ifndef SPEED_MODE //����
	 if(ControlCommand.hSpeedM1>0 && ControlCommand.hSpeedM2>0 && DriverMsg.hSpeedM1<0 && DriverMsg.hSpeedM2<0 ) 
	 	hSpeedRefTemp1[bMotor] +=1;//2
	 if(ControlCommand.hSpeedM1<0 && ControlCommand.hSpeedM2<0 && DriverMsg.hSpeedM1>0  && DriverMsg.hSpeedM2>0 ) 
	 	hSpeedRefTemp1[bMotor] -=1;//2	
	 #endif

     if(hSpeedTemp1>forward_max_spd || hSpeedTemp1<-backward_max_spd )  //��ǰ���ͺ�������,ǰ������6.5km/h������6.5km/h,���˳���3.5km/h������3.5km/h,
      {
         if(hSpeedTemp1>0)  
            hLimitSpdRefTemp[bMotor] -=2; 
         else
            hLimitSpdRefTemp[bMotor] +=2;  
      }
      else 
      {            
         if(hLimitSpdRefTemp[bMotor]<0)
           hLimitSpdRefTemp[bMotor] +=2;
         else
           hLimitSpdRefTemp[bMotor] -=2; 				  
         
         if(hLimitSpdRefTemp[bMotor]<10 && hLimitSpdRefTemp[bMotor]>-10)
           hLimitSpdRefTemp[bMotor]=0;           
      }

      if(bMotor==M1)  hSpeedRef = ((int32_t)ControlCommand.hSpeedM1 + hSpeedRefTemp1[bMotor] );
      else  hSpeedRef = ((int32_t)ControlCommand.hSpeedM2 + hSpeedRefTemp1[bMotor] );         
      hSpeedRef += hLimitSpdRefTemp[bMotor];   
	 			
     MC_StopDelayCnt[bMotor]=0;
	 Slope_static_flagM1=0;
	 Slope_static_flagM2=0;				
  }
/*
    else if( bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 1 ) //����״̬�������Ƴ����ٴ���
	 {
      #ifndef SPEED_MODE //����
        if(hSpeedTemp1 > ManualTrolleyMaxSpd3km || hSpeedTemp1 < -ManualTrolleyMaxSpd3km)
        {
          if(hSpeedTemp1>0) 
             hSpeedRefTemp1[bMotor] -=2; 
          else
             hSpeedRefTemp1[bMotor] +=2;  
        }
        else 
        {
           if(hSpeedRefTemp1[bMotor]!=0)
           {
             if(hSpeedRefTemp1[bMotor]<0)
               hSpeedRefTemp1[bMotor] +=2;
             else
               hSpeedRefTemp1[bMotor] -=2;                   
             
             if(hSpeedRefTemp1[bMotor]<10 && hSpeedRefTemp1[bMotor]>-10)
                hSpeedRefTemp1[bMotor]=0; 
           }
           else
           {
             if( hSpeedTemp1==0)
             {
               if(bMotor==M1)
                  TIM_CtrlPWMOutputs(TIM1,DISABLE);
               else 
                  TIM_CtrlPWMOutputs(TIM8,DISABLE);
             }   
           }
        }
      #else  
         hSpeedRefTemp1[bMotor] = 0;	  
      #endif
         hSpeedRef = hSpeedRefTemp1[bMotor];	
	 }
	else // bCanMagneticBrakeRelease == 1 && bMagneticBrakeRelease == 0
	   hSpeedRef = 0;	
	 
*/
	return( hSpeedRef );

}

/*******************************************************************************
* Function Name  :  MC_BreakerHandle
* Description    :  �ƶ�����
* parameters     : bMotor M1����        M2�ҵ��
* Return         : �����ƶ����
*******************************************************************************/
void MC_BreakerHandle(uint8_t bMotor)
{
   TIM_TypeDef* TIMx = (oPwmc[bMotor]->pParams->TIMx);

  TIM_SetCompare1(TIMx,FOCVars[bMotor].hDollySpdLimit);//A������ȫ�� ����PWM����
  TIM_SetCompare2(TIMx,FOCVars[bMotor].hDollySpdLimit);//B������ȫ�� ����PWM����
  TIM_SetCompare3(TIMx,FOCVars[bMotor].hDollySpdLimit);//C������ȫ�� ����PWM����
  TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | (CH1_PWM |CH2_PWM);
  TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | (CH3_PWM);
  TIMx->CCER =  (TIMx->CCER & CCER_MASK)  | (CH1_LON|CH2_LON|CH3_LON);	
}

/*******************************************************************************
* Function Name  :  MC_MagneticBrakeCheck
* Description    :  ���ɲ������        (IOģ��PWM������Ƶ��ɲ��,�����ʻ���ֶ�ɲ��״̬, ��IO��ⲻ����ʻ���ɿ�ɲ��״̬)
* parameters     :  None
* Return         :  None
*******************************************************************************/

uint16_t  bMagneticBrakeStaM1CheckHighCnt;
uint16_t  bMagneticBrakeStaM1CheckLowCnt;
uint16_t  bMagneticBrakeStaM1CheckHighCnt2;
uint16_t  bMagneticBrakeStaM1CheckLowCnt2;
uint8_t   bMagneticBrakeStaM1Check;

#ifdef DOUBLE_DRIVER 
uint16_t  bMagneticBrakeStaM2CheckHighCnt;
uint16_t  bMagneticBrakeStaM2CheckLowCnt;
uint16_t  bMagneticBrakeStaM2CheckHighCnt2;
uint16_t  bMagneticBrakeStaM2CheckLowCnt2;		
uint8_t   bMagneticBrakeStaM2Check;
#endif

void MC_MagneticBrakeCheck(void)
{
//	static uint16_t   hMagneticBrakeOutputStartCnt=0;
//	uint8_t   bMagneticBrakeStaM1Check;
//	uint8_t   bMagneticBrakeStaM2Check;
//    static uint16_t   bMagneticBrakeStaM1CheckHighCnt;
//    static uint16_t   bMagneticBrakeStaM1CheckLowCnt;
//    static uint16_t   bMagneticBrakeStaM2CheckHighCnt;
//    static uint16_t   bMagneticBrakeStaM2CheckLowCnt;	

//    static uint16_t   bMagneticBrakeStaM1CheckHighCnt2;
//    static uint16_t   bMagneticBrakeStaM1CheckLowCnt2;
//    static uint16_t   bMagneticBrakeStaM2CheckHighCnt2;
//    static uint16_t   bMagneticBrakeStaM2CheckLowCnt2;		
#ifdef DOUBLE_DRIVER 
    bMagneticBrakeStaM2Check =GPI_MagneticBrakeStaM2();
#endif
	bMagneticBrakeStaM1Check = GPI_MagneticBrakeStaM1();
	//��������챵��ɲ������߼�: �ɿ�Ϊ0,����Ϊ1;
	//���ε��ɲ�����߼�:�ɿ��ֱ�Ϊ1, ����Ϊ0;
			 
	#ifdef DOUBLE_DRIVER
	if(((bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0) &&((FOCVars[M1].bRunMagneticBrakeState==1 &&  FOCVars[M1].bCanMagneticBrakeState==0 ) 
	|| (FOCVars[M2].bRunMagneticBrakeState==1 &&  FOCVars[M2].bCanMagneticBrakeState==0 )||(FOCVars[M1].bStopMagneticBrakeState==1 &&  FOCVars[M1].bCanMagneticBrakeState==0) 
	|| (FOCVars[M2].bStopMagneticBrakeState==1 &&  FOCVars[M2].bCanMagneticBrakeState==0))) || (bCanMagneticBrakeRelease==1 && bMagneticBrakeRelease ==1) 
	|| ( (oEncoder[M2]->Vars.bBlockageFault==1) || (oEncoder[M1]->Vars.bBlockageFault==1)))
	#else
	if((bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0) &&((FOCVars[M1].bRunMagneticBrakeState==1 &&  FOCVars[M1].bCanMagneticBrakeState==0 ) 
	|| (FOCVars[M1].bStopMagneticBrakeState==1 &&  FOCVars[M1].bCanMagneticBrakeState==0))|| (bCanMagneticBrakeRelease==1 && bMagneticBrakeRelease ==1)  
	|| (oPosSen [M1]->Vars.bBlockageFault==1))
	#endif
	{
		bMagneticBrakeFlag = 1;
	}

	if(bCanMagneticBrakeRelease == 0 && bMagneticBrakeRelease == 0 && bMagneticBrakeFlag == 1)		 			
	{
		#ifdef DOUBLE_DRIVER
		if(ControlCommand.hSpeedM1==0 && ControlCommand.hSpeedM2==0)
		#else
		if(ControlCommand.hSpeedM1 == 0)
		#endif
			hMotorStartCnt++;
		else
			hMotorStartCnt=0;
	}																
		
		 //if( (hMotorStartCnt>1000) || (hMotorStartCnt<50 && hMotorStartCnt>5  && ABS(ControlCommand.hSpeedM1)<30 && ABS(ControlCommand.hSpeedM2)<30)  ) //1400
		 if( hMotorStartCnt > 1500   ) //1000
		 { 
			 bMagneticBrakeFlag = 0;
			 hMotorStartCnt = 0;
			 
			 FOCVars[M1].bRunMagneticBrakeState=0; 			 
			 FOCVars[M1].bStopMagneticBrakeState=0;
			 #ifdef DOUBLE_DRIVER
			 FOCVars[M2].bRunMagneticBrakeState=0;
			 FOCVars[M2].bStopMagneticBrakeState=0;		
			 #endif
		 }

		#ifdef DOUBLE_DRIVER
		if(((( ABS(ControlCommand.hSpeedM1)>30 && ABS(ControlCommand.hSpeedM2)>30 && bCanCommFault==0 && bMcDriverErrFlag==0 ) 
		|| (bBreakLockingFlag ==0 &&  ((FOCVars[M1].bRunning == 1)|| (FOCVars[M2].bRunning == 1)) ) )  //bOvAbnormalFlag
		&& (bMotorSlopeStopFlag==0) && (bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0)  )||
		( bCanMagneticBrakeRelease==1 && bMagneticBrakeRelease ==1 ))
		#else
		if((((ABS(ControlCommand.hSpeedM1)>30 && bCanCommFault==0 && bMcDriverErrFlag==0 ) 
		|| (bBreakLockingFlag ==0 &&  (FOCVars[M1].bRunning == 1)))  //bOvAbnormalFlag
		&& (bMotorSlopeStopFlag==0) && (bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0))||
		(bCanMagneticBrakeRelease==1 && bMagneticBrakeRelease ==1))
		#endif
		{
		if(bMagneticBrakeOutputStartFlag==1) //��챵������ɲ��												 
		{									
			hMagneticBrakeOutputStartCnt=0;

			if(bMagneticBrakeFlag==0 && bMototRunStateFlag==0)		
			{
				//TIM_SetCompare1(TIM3,(BREAK_PWM_Period *6/100));//ռ�ձ�Ϊ6%	
				if(hVs <= 44000)
					TIM_SetCompare1(TIM3, (BREAK_PWM_Period * 6 / 100));// <48Vռ�ձ�Ϊ6%	
				else
					TIM_SetCompare1(TIM3, (BREAK_PWM_Period * 5 / 100));	// >48Vռ�ձ�Ϊ5%
			}
		}
		else
		{
			if(hMagneticBrakeOutputStartCnt>=150)//250(200ms,1ms����һ��)
			{
				hMagneticBrakeOutputStartCnt=25000;
				bMagneticBrakeOutputStartFlag=1;
			}
			else
			{
				hMagneticBrakeOutputStartCnt++;
				bMagneticBrakeOutputStartFlag=0;	

				if(bMagneticBrakeFlag==0 && bMototRunStateFlag==0)
					TIM_SetCompare1(TIM3, (BREAK_PWM_Period * 8 / 10));//3200ռ�ձ�Ϊ90%	
			}
		}
		//			  if(bMagneticBrakeFlag==0 && bMototRunStateFlag==0)			 
		//					// TIM_SetCompare4(TIM4,(BREAK_PWM_Period/2));//1600ռ�ձ�Ϊ50%	ռ�ձ��ɵ�ѹ�仯���仯 60V(55500)--40%, 55V(50900)-45%  48V(44200-44400)--50%, 44V(40600)   40V(36990)--60%  38V�ػ�(35150)
		//					{
		//						if(hVs <= 44000)
		//							TIM_SetCompare4(TIM4,(BREAK_PWM_Period *6/10));
		//			      else
		//							TIM_SetCompare4(TIM4,(BREAK_PWM_Period *5/10));	
		//					}				 

		//���ɿ�ɲ�����PWMʱ����������߻��������ɲ���ɿ����·����
		if(bMagneticBrakeStaM1Check==1)
		{
			bMagneticBrakeStaM1CheckLowCnt=0;
			bMagneticBrakeStaM1CheckHighCnt++;
			if(bMagneticBrakeStaM1CheckHighCnt>MagneticBrakeStaCheckMaxNum)
				bMagneticBrakeStaM1CheckHighCnt = MagneticBrakeStaCheckMaxNum;				 
		}
		else
		{
			bMagneticBrakeStaM1CheckHighCnt=0;
			bMagneticBrakeStaM1CheckLowCnt++;					  
			if(bMagneticBrakeStaM1CheckLowCnt>MagneticBrakeStaCheckMaxNum)
				bMagneticBrakeStaM1CheckLowCnt =MagneticBrakeStaCheckMaxNum;
		}
			bMagneticBrakeStaM1CheckHighCnt2=0;
			bMagneticBrakeStaM1CheckLowCnt2=0;
			
		//				  if(bMagneticBrakeStaM1CheckHighCnt>=MagneticBrakeStaCheckMaxNum ) // || bMagneticBrakeStaM1CheckLowCnt>=MagneticBrakeStaCheckMaxNum
		//					{
		//						FOCVars[M1].bCanMagneticBrakeState=1;	
		//						FOCVars[M1].bRunMagneticBrakeState=1;
		//					}						
		#ifdef DOUBLE_DRIVER
		if(bMagneticBrakeStaM2Check==1)
		{
			bMagneticBrakeStaM2CheckLowCnt=0;
			bMagneticBrakeStaM2CheckHighCnt++;
			if(bMagneticBrakeStaM2CheckHighCnt>MagneticBrakeStaCheckMaxNum)
				bMagneticBrakeStaM2CheckHighCnt = MagneticBrakeStaCheckMaxNum;				 
		}
		else
		{
			bMagneticBrakeStaM2CheckHighCnt=0;
			bMagneticBrakeStaM2CheckLowCnt++;					  
			if(bMagneticBrakeStaM2CheckLowCnt>MagneticBrakeStaCheckMaxNum)
				bMagneticBrakeStaM2CheckLowCnt = MagneticBrakeStaCheckMaxNum;
		}
					bMagneticBrakeStaM2CheckHighCnt2=0;
			bMagneticBrakeStaM2CheckLowCnt2=0;	
        #endif
		//				  if(bMagneticBrakeStaM2CheckHighCnt>=MagneticBrakeStaCheckMaxNum ) // || bMagneticBrakeStaM2CheckLowCnt>=MagneticBrakeStaCheckMaxNum
		//					{
		//						FOCVars[M2].bCanMagneticBrakeState=1;		
		//						FOCVars[M2].bRunMagneticBrakeState=1;
		//					}						
		}
		#ifdef DOUBLE_DRIVER
		if(((FOCVars[M1].bRunning == 0) && (FOCVars[M2].bRunning == 0) && (ABS(ControlCommand.hSpeedM1) <20) && (ABS(ControlCommand.hSpeedM2) <20) &&  (bBreakLockingFlag ==1))	//������ֹ
		|| ( bMototRunStateFlag==1  && bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0))//����������ֹͣ�����ͣ����־�Լ����ӽ������ϱ�־
		#else
		if(((FOCVars[M1].bRunning == 0)  && (ABS(ControlCommand.hSpeedM1) <20) &&(bBreakLockingFlag ==1))	//������ֹ
		|| (bMototRunStateFlag==1 && bCanMagneticBrakeRelease==0 && bMagneticBrakeRelease ==0))//����������ֹͣ�����ͣ����־�Լ����ӽ������ϱ�־
		#endif
		{
			TIM_SetCompare1(TIM3, 0);//ռ�ձ�Ϊ0 ��ɲ������
			bMagneticBrakeOutputStartFlag = 0;
			hMagneticBrakeOutputStartCnt = 0;
			//����ֹ�����PWMʱ�������������ɲ���ɿ�
			if(bMagneticBrakeStaM1Check == 0)//1
			{
				bMagneticBrakeStaM1CheckLowCnt2 = 0;
				bMagneticBrakeStaM1CheckHighCnt2++;
				if(bMagneticBrakeStaM1CheckHighCnt2 > MagneticBrakeStaCheckMaxNum)
					bMagneticBrakeStaM1CheckHighCnt2 = MagneticBrakeStaCheckMaxNum;				 
			}
			else
			{
				bMagneticBrakeStaM1CheckHighCnt2 = 0;
				bMagneticBrakeStaM1CheckLowCnt2++;					  
				if(bMagneticBrakeStaM1CheckLowCnt2 > MagneticBrakeStaCheckMaxNum)
					bMagneticBrakeStaM1CheckLowCnt2 = MagneticBrakeStaCheckMaxNum;
			}

			if(bMagneticBrakeStaM1CheckHighCnt2 >= MagneticBrakeStaCheckMaxNum ) 
			{
				FOCVars[M1].bCanMagneticBrakeState = 1;				  		
				FOCVars[M1].bStopMagneticBrakeState = 1;
			}						
			if( bMagneticBrakeStaM1CheckLowCnt2 >= MagneticBrakeStaCheckMaxNum) 
				FOCVars[M1].bCanMagneticBrakeState = 0;	

			bMagneticBrakeStaM1CheckHighCnt = 0;
			bMagneticBrakeStaM1CheckLowCnt = 0;

			#ifdef DOUBLE_DRIVER
			if(bMagneticBrakeStaM2Check==0)
			{
				bMagneticBrakeStaM2CheckLowCnt2=0;
				bMagneticBrakeStaM2CheckHighCnt2++;
				if(bMagneticBrakeStaM2CheckHighCnt2>MagneticBrakeStaCheckMaxNum)
					bMagneticBrakeStaM2CheckHighCnt2 = MagneticBrakeStaCheckMaxNum;				 
			}
			else
			{
				bMagneticBrakeStaM2CheckHighCnt2=0;
				bMagneticBrakeStaM2CheckLowCnt2++;					  
				if(bMagneticBrakeStaM2CheckLowCnt2>MagneticBrakeStaCheckMaxNum)
					bMagneticBrakeStaM2CheckLowCnt2 = MagneticBrakeStaCheckMaxNum;
			}

			if(bMagneticBrakeStaM2CheckHighCnt2>=MagneticBrakeStaCheckMaxNum ) 
			{
				FOCVars[M2].bCanMagneticBrakeState=1;		
				FOCVars[M2].bStopMagneticBrakeState=1;
			}						
			if( bMagneticBrakeStaM2CheckLowCnt2>=MagneticBrakeStaCheckMaxNum) 
			{
				FOCVars[M2].bCanMagneticBrakeState=0;	
			}						
			
			bMagneticBrakeStaM2CheckHighCnt=0;
			bMagneticBrakeStaM2CheckLowCnt=0;	

			#endif
		}		
}
void HandlebarHandle(void)
{
    if((int16_t)(oSTC[M1]->Vars.hSpeedRef01HzExt)!=0)
    {
        FOCVars[M1].bRunning = 1;
        FOCVars[M2].bRunning = 1;
        TIM_CtrlPWMOutputs(TIM1, ENABLE);
        TIM_CtrlPWMOutputs(TIM8, ENABLE);
        //ObserverConverged =0;
        //RampLocalTime=0;
        //FOCVars[M1].bSpdRef0Flage = 1;
        //FOCVars[M2].bSpdRef0Flage = 1;
        motormode = MOTORSTART;
    }
    
    if((ControlCommand.hSpeedM1 == 0) && ((FOCVars[M1].bRunning != 0) || (motormode != MOTORSTOP)))
    {
        FOCVars[M1].bRunning = 0;
        FOCVars[M2].bRunning = 0;
        TIM_CtrlPWMOutputs(TIM1, DISABLE);
        TIM_CtrlPWMOutputs(TIM8, DISABLE);
        //bMotorEabsStartFlag=1;
        //FOCVars[M1].bSpdRef0Flage = 0;
        //FOCVars[M2].bSpdRef0Flage = 0;
        //hleeSpeedtemp =3000;
        motormode = MOTORSTOP;
    }
}
/**************************************************************END OF FILE****/
