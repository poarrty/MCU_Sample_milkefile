/******************************************************************************
* File Name         :  SpeednTorqCtrl.c
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

/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"


STC_t STCpool[NBR_OF_MOTORS];

#define spd_hMaxTorque  25000
//uint16_t  DefTemperatureMax = 60;  //降流温度设定最大值 STM32的内部温度传感器支持的温度范围为：-40~125度
//#define  DefNtcCnt   2400000 //高温限流时间(10min)


//#define VSREF   	42500 //37781(18.5V)  44500(22V)  49250(24V)  42500(21V) 欠压阈值  65536(31.9V)  
//#define OVSREF      61562  //过压阈值  59510(29V)    61562(30V)    65536(31.9V)  51302(25V)  55406(27V) 63615(31V)

//#define POWERDOWNLEVEL 		32500  // 31484  36800(18V)  32500(16V) 掉电阈值 

//#define hADCAvgCurL   		hRegConv[0]
//#define hADCAvgCurR   		hRegConv[1]
//#define hADCBusVolt 		hRegConv[2] 
//#define hADCBatVolt 		hRegConv[3]


#define EBS_CURRENT         4000//190715


uint16_t hVs;       //鐢垫睜鐢靛帇
uint16_t hVrelay;   //棰勫厖鐢靛悗鐢靛帇
uint16_t hM1TmpValue;
uint16_t hM2TmpValue;

STCParams_t STCParamsM1 =
{
	0,
	840,   // 140 RPM * 6 
	10,    // 10 RPM *6
	IQMAX, //
	1000,
//	31484,   //15V       
//	37781, //  18V  
	9600//15888, 20A    9532,12.5A     12710,16.4A    11638,15A   19396,25A  9600(新联谊电机),12.5A       11600,15A  15451,20A
};//lee

STCParams_t STCParamsM2 =
{
	1,
	840,   // 140 RPM * 6 
	10,    // 10 RPM *6
	IQMAX, //
	500,
//	31484,   //15V       
//	37781, //  18V   
	9600//15888, 20A   9532,13.5A   12710,16.4A  11638,15.7A  9100,12.5A  11200,15.1A  4550,6.5A   9600(新联谊电机),12.5A       11600,15A  15451,20A
};
  


/*******************************************************************************
* Function Name  : STC_Init
* Description    : 变量初始化函数
* parameters     : None
* Return         : None
*******************************************************************************/
void STC_Init(void)
{   
    oSTC[0] = &STCpool[0];      
    oSTC[0]->pParams = &STCParamsM1;
//    oSTC[0]->Vars.oHall = oHall[0];
    oSTC[0]->Vars.oPosSen = oPosSen[0];
	oSTC[0]->Vars.oPwmc = oPwmc[0];
    oSTC[0]->Vars.oPIVs = oPIDVs[0];
    oSTC[0]->Vars.oPISpeed = oPIDSpeed[0];
	oSTC[0]->Vars.oPIDIq = oPIDIq[0];
	//oSTC[0]->Vars.oPIAcc = oPIAcc[0];
    oSTC[0]->Vars.hIsRef = oSTC[0]->pParams->hIsRefDefault;
	//oSTC[0]->Vars.oPISpeedLike = oPIDSpeedLike[0];
	oSTC[0]->Vars.oCRLM = oCRLM[0];
    oSTC[0]->Vars.oNTC =oNTC[0];
	oSTC[0]->Vars.oPINTc = oPINTc[0];
	//oSTC[0]->Vars.oPIOVs = oPIDOvs[0];
	
	#ifdef DOUBLE_DRIVER
	oSTC[1] = &STCpool[1];
    oSTC[1]->pParams = &STCParamsM2;
//    oSTC[1]->Vars.oHall = oHall[1];
    oSTC[1]->Vars.oPosSen = oPosSen[1];
	oSTC[1]->Vars.oPwmc = oPwmc[1];
    oSTC[1]->Vars.oPIVs = oPIDVs[1];
    oSTC[1]->Vars.oPISpeed = oPIDSpeed[1];  
	oSTC[1]->Vars.oPIDIq = oPIDIq[1];
	//oSTC[1]->Vars.oPIAcc = oPIAcc[1];
    oSTC[1]->Vars.hIsRef = oSTC[1]->pParams->hIsRefDefault;
	//oSTC[1]->Vars.oPISpeedLike = oPIDSpeedLike[1];
	oSTC[1]->Vars.oCRLM = oCRLM[1];
	oSTC[1]->Vars.oNTC =oNTC[1];
	oSTC[1]->Vars.oPINTc = oPINTc[1];
	//oSTC[1]->Vars.oPIOVs = oPIDOvs[1];
	#endif
}


/*******************************************************************************
* Function Name  : STC_CalcTorqueReference
* Description    : 计算输出转矩
* parameters     : this  STCpool结构指针        Global FOC全局变量 
* Return         : 计算后的转矩
*******************************************************************************/
//int16_t hFdb1;
//int16_t hRef1;
//int16_t hTorqueTemp= 0;
int16_t hRefAccSpeed = 64;//硬启动

int16_t hDiff;

int16_t hTorqueReference = 0;
int16_t hPreTorqueReference = 0;

int16_t STC_CalcTorqueReference(pSTC_t this, FOCVars_t *Global)
{
    pSTCVars_t pVars = &(this->Vars);
    pSTCParams_t pParams = this->pParams;    
    int32_t wCurrentReference;
    int16_t hError;
	int8_t bDirection = 1;

	#ifdef SPEED_MODE
    wCurrentReference = pVars->hTorqueRef;//开环：hSpeedRef01HzExt；速度环：速度环输出
    #else
    wCurrentReference = pVars->hTorqueRef;
    #endif
	
    if(Global->bRunning==1)
    {              
	   //---------------------------------------------------------加减速曲线
		hTorqueReference = wCurrentReference;

        if(pVars->bTaskIndex == 0)//190713
		{
		    //if(pVars->hIs>29000 )  //堵转电流判断  29000
		    if(pVars->oCRLM->Vars.hIs > (pVars->oCRLM->pParams->hDefIsRefLevel3 / 7) ) //pVars->oCRLM->pParams->hDefIsRefLevel1  8448(11A)
            {
                Global->bBlockageCurrent = 1;//1
                Global->bBlockageStartFlag = 1;
            }
            else
            {
                Global->bBlockageCurrent = 0;            
            }											
		}
		else if(pVars->bTaskIndex == 1)
		{
        //--------------------------------------------------------欠压
		}
		else if(pVars->bTaskIndex == 2)
		{
		}
		else// if(pVars->bTaskIndex == 3)
		{
	    //--------------------------------------------------------加速度//190713
		}

		if(pVars->bTaskIndex < 1)//elfern 191014  1
		{
		    pVars->bTaskIndex ++;
		}				
		else
		{
		    pVars->bTaskIndex = 0;
		}

        //--------------------------------------------------------筛选
        if(hTorqueReference < 0)
    	{
			bDirection = -1;
			hTorqueReference = -hTorqueReference; 
    	}		

        pVars->bRegulatorLimit = 0;
                  
         //--------------------------------------------------------开环控制电压过充处理     
#ifndef SPEED_MODE  
        if(hVs >= OVSREF &&  hVs < OVSREF2) // 当>30V && <31V一直保持上一次值,不做加减
        {
             // GPIO_SetBits(GPIOD,GPIO_Pin_2);
            hPreTorqueReference = Global->Vqd.qV_Component1;//上一次Vq值 	
            hTorqueReference = hPreTorqueReference; 					 

            if(hTorqueReference < 0) 
            {
	            bDirection = -1;
	            hTorqueReference = -hTorqueReference;
            } 
	        else			 
            {
                bDirection = 1;
            }

	        pVars->hOvFlag =1; 
        }
        else if(hVs >= OVSREF2 )
        {
            // GPIO_SetBits(GPIOD,GPIO_Pin_2);
            hPreTorqueReference = Global->Vqd.qV_Component1;//上一次Vq值

	        if(hPreTorqueReference>0 )
                hTorqueReference = hPreTorqueReference+2;   
	        else
                hTorqueReference = hPreTorqueReference-2;

	        if(hTorqueReference < 0) 
	        {
                bDirection = -1;
                hTorqueReference = -hTorqueReference;
	        } 
	        else			 
            {
                bDirection = 1;
            }

	        pVars->hOvFlag =1;			  
        }
        else
        {
            if(pVars->hOvFlag==1)
            {	   
                hPreTorqueReference = Global->Vqd.qV_Component1;//上一次Vq值
                if( pVars->hTorqueRef <hPreTorqueReference )
                {
                    hPreTorqueReference -=5;
                    hTorqueReference = hPreTorqueReference; 
                    if(hTorqueReference < 0) 
                    {
                        bDirection = -1;
                        hTorqueReference = -hTorqueReference;
                    } 
                else			  
                    bDirection = 1; 
                }
                else if( pVars->hTorqueRef > hPreTorqueReference )
                {
                    hPreTorqueReference += 5;
                    hTorqueReference = hPreTorqueReference;
                    if(hTorqueReference < 0) 
                    {
                        bDirection = -1;
                        hTorqueReference = -hTorqueReference;
                    } 
		         else			  
                    bDirection = 1; 
                }			  
                else
                    pVars->hOvFlag = 0;	   
            }     
            //GPIO_ResetBits(GPIOD,GPIO_Pin_2);
        }
#endif        		

        //--------------------------------------------------------掉电
        if(hVs < POWERDOWNLEVEL ) //电池断电
        {
            pVars->bPowerDownFault = 1;
			hTorqueReference = Global->Vqd.qV_Component1;

			if(hTorqueReference < 0)
			{
				bDirection = -1;
				hTorqueReference = -hTorqueReference;
			}
		
			if(hTorqueReference > 40)
				hTorqueReference -= 40;
			else 
			{
				hTorqueReference = 0;
				//Global->bRunning=0;
				//TIM_CtrlPWMOutputs(pVars->oPwmc->pParams->TIMx,DISABLE);
			}						
        }
        else if(pVars->bPowerDownFault != 0)
        {
            pVars->bPowerDownFault = 0;
        }
    }
    else
    {
        pVars->bTaskIndex = 0;//190713
//        pVars->hAccRegulatorOut = 0;  
        pVars->hIsRegulatorOut = 0;
        pVars->hVsRegulatorOut = 0;
		pVars->hTmpRegulatorOut=0;
		pVars->hNtcStateFlag =0;
	    pVars->hNtcCntFlag=0;
        pVars->hOvsRegulatorOut = 0;

		if(wCurrentReference < 0)
		{
			bDirection = -1;
			wCurrentReference = -wCurrentReference;
		}
		#ifdef SPEED_MODE
		if(wCurrentReference > pParams->hMinTorque)// hMinMecSpeed01Hz
        {
            hTorqueReference = wCurrentReference;
        }
		#else
        if(wCurrentReference > pParams->hMinTorque)
        {
            hTorqueReference = wCurrentReference;
        }
		#endif		
    }
  
    return (hTorqueReference * bDirection);
}

/*******************************************************************************
* Function Name  : STC_Clear
* Description    : 将速度环路的积分量清0
* parameters     : this  STCpool结构指针  
* Return         : None
*******************************************************************************/
void STC_Clear(pSTC_t this)
{
    #ifdef SPEED_MODE
    PI_SetIntegralTerm(this->Vars.oPISpeed,0);
    #endif
}



/*******************************************************************************
* Function Name  : STC_CalcBusCurrent
* Description    : 读取总线电流
* parameters     : None  
* Return         : None
*******************************************************************************/
void STC_CalcBusCurrent(void)
{
	oSTC[M1]->Vars.hIs = 0;//hADCAvgCurL;
	#ifdef DOUBLE_DRIVER
	oSTC[M2]->Vars.hIs = 0//hADCAvgCurR;
	#endif
}


/*******************************************************************************
* Function Name  : STC_CalcBusVoltage
* Description    : 读取总线电压
* parameters     : None 
* Return         : None
*******************************************************************************/
void STC_CalcBusVoltage(void)//电压对应关系错误，互换//XU-190626
{
    hVs = hADCBatVolt;
	hVrelay = hADCBusVolt;
	// hM1TmpValue = hM1Temp; 		    
//   hM2TmpValue=hM2Temp ;	
}



/**************************************************************END OF FILE****/
