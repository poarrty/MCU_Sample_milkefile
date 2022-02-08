/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer_Interface.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This module implements the State Observer of 
*                      the PMSM B-EMF, thus identifying rotor speed and position
*
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 11/07/08 v2.0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "UserInclude.h"
#include "MCInclude.h"


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define LOWER_THRESHOLD_FACTOR    0.8  //percentage of forced speed
#define UPPER_THRESHOLD_FACTOR    1    //percentage of forced speed

#define LOW_THRESHOLD             (u8) (LOWER_THRESHOLD_FACTOR*10)
#define UP_THRESHOLD              (u8) (UPPER_THRESHOLD_FACTOR*10)

#define HC2_INIT                  (s16)((F1*wK1_LO)/SAMPLING_FREQ)
#define HC4_INIT                  (s16)(((wK2_LO*MAX_CURRENT)/MAX_BEMF_VOLTAGE)\
                                        *F2/SAMPLING_FREQ)
//Do not be modified
#define SLESS_T_ALIGNMENT_PWM_STEPS       (u32) ((SLESS_T_ALIGNMENT * SAMPLING_FREQ)\
                                                                          /1000) 
#define SLESS_ALIGNMENT_ANGLE_S16         (s16)((s32)(SLESS_ALIGNMENT_ANGLE)\
                                                                    * 65536/360)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//static u16 bConvCounter;
//static s16 hAngle = 0;
//static u32 wTime = 0;
//static s32 wStart_Up_Freq = 0;
//static s32 wStart_Up_I;
//static s16 hFreq_Inc;
//static s32 hI_Inc;
u16 bConvCounter;
s16 hAngle = 0;
u32 wTime = 0;
u32 wTime_test = 0;
u32 wTime_test_align = 0;
u32 wTime_test_ramp = 0;
s32 wStart_Up_Freq = 0;
s32 wStart_Up_I;
s16 hFreq_Inc;
s32 hI_Inc;


Start_upStatus_t  Start_Up_State = S_INIT;

#ifdef OBSERVER_GAIN_TUNING
volatile s32 wK1_LO = K1;
volatile s32 wK2_LO = K2;
volatile s16 hPLL_P_Gain = PLL_KP_GAIN;
volatile s16 hPLL_I_Gain = PLL_KI_GAIN;
#endif
volatile SystStatus_t State;

/*******************************************************************************
* Function Name : STO_StateObserverInterface_Init
* Description : It fills and passes to the State Obsever module the data 
*               structure necessary for rotor position observation 
* Input : None
* Output : None
* Return : None
*******************************************************************************/
 StateObserver_Const StateObserver_ConstStruct;
void STO_StateObserverInterface_Init(void)
{

     s16 htempk;
// StateObserver_ConstStruct.hC1 = C1;
// StateObserver_ConstStruct.hC3 = C3;
// StateObserver_ConstStruct.hC5 = C5;
//
//   {
//
//    StateObserver_ConstStruct.hF3 = 1;
//    htempk = (s16)((100*65536)/(F2*2*PI));
//    while (htempk != 0)
//    {
//      htempk /=2;
//      StateObserver_ConstStruct.hF3 *=2;
//    }
//    StateObserver_ConstStruct.hC6 = (s16)((F2*StateObserver_ConstStruct.hF3*2*
//                                                                    PI)/65536);
//  }
//
//#ifdef OBSERVER_GAIN_TUNING
//  /* lines below for debug porpose*/
//  StateObserver_ConstStruct.hC2 = C2;
//  StateObserver_ConstStruct.hC4 = C4;
//
//  StateObserver_ConstStruct.hC2 = (s16)((F1*wK1_LO)/SAMPLING_FREQ);
//  StateObserver_ConstStruct.hC4 = (s16)((((wK2_LO*MAX_CURRENT)/(MAX_BEMF_VOLTAGE
//                                                       ))*F2)/(SAMPLING_FREQ));
//  StateObserver_ConstStruct.PLL_P = hPLL_P_Gain;
//  StateObserver_ConstStruct.PLL_I = hPLL_I_Gain;
//#else
//  StateObserver_ConstStruct.hC2 = C2;
//  StateObserver_ConstStruct.hC4 = C4;
//  StateObserver_ConstStruct.PLL_P = PLL_KP_GAIN;
//  StateObserver_ConstStruct.PLL_I = PLL_KI_GAIN;
//#endif
//  StateObserver_ConstStruct.hF1 = F1;
//  StateObserver_ConstStruct.hF2 = F2;
//  StateObserver_ConstStruct.wMotorMaxSpeed_dpp = MOTOR_MAX_SPEED_DPP;
//  StateObserver_ConstStruct.hPercentageFactor = PERCENTAGE_FACTOR;
//

  STO_Gains_Init();
}
  
/*******************************************************************************
* Function Name : STO_Check_Speed_Reliability
* Description : Check for the continuity of the speed reliability. If the speed 
*               is continously not reliable, the motor must be stopped 
* Input : None
* Output : None
* Return : boolean value: TRUE if speed is reliable, FALSE otherwise.
*******************************************************************************/
bool STO_Check_Speed_Reliability(void)
{
  static u8 bCounter=0;
  bool baux;
  
  if(STO_IsSpeed_Reliable() == FALSE)
  {
   bCounter++;
   if (bCounter >= RELIABILITY_HYSTERESYS)
   {
     bCounter = 0;
     baux = FALSE;
   }
   else
   {
     baux = TRUE;
   }
  }
  else
  {
   bCounter = 0;
   baux = TRUE;
  }
  return(baux);
}
           
/*******************************************************************************
* Function Name : IsObserverConverged
* Description : Check for algorithm convergence. The speed reliability and the
*               range of the value of the estimated speed are checked. 
* Input : None
* Output : None
* Return : boolean value: TRUE if algortihm converged, FALSE otherwise.
*******************************************************************************/
s16 hEstimatedSpeed;
s16 hUpperThreshold;
s16 hLowerThreshold;
u8 leeupcnt=0;
u8 leelowcnt=0;
u8 leeupcnt1=0;
u8 leelowcnt1=0;
u8 leespdeffcnt=0;
bool IsObserverConverged(void)
{ 
    if(RunStoSensorlessMode==0)
    {
        hEstimatedSpeed = hMecSpeed01Hzltmp2;
    }
    else
    {
        hEstimatedSpeed = STO_Get_Speed_Hz();
    }
    hEstimatedSpeed = (hEstimatedSpeed < 0 ? -hEstimatedSpeed : hEstimatedSpeed);  //wStart_Up_Freq
    hUpperThreshold = ((wStart_Up_Freq / 65536) * 170)/(POLE_PAIR_NUM * 16);//hAvrMecSpeed01Hz  这个地方是不是可以改成170，ST源码是170和150；
    hUpperThreshold = (hUpperThreshold < 0 ? -hUpperThreshold : hUpperThreshold);
    hLowerThreshold = ((wStart_Up_Freq / 65536) *150) / (POLE_PAIR_NUM * 16);
    hLowerThreshold = (hLowerThreshold < 0 ? -hLowerThreshold : hLowerThreshold);

  // If the variance of the estimated speed is low enough...
    if(STO_IsSpeed_Reliable() == TRUE)
    { 
        if(hEstimatedSpeed > MINIMUM_SPEED)
        {
      //...and the estimated value is quite close to the expected value... 
            if(hEstimatedSpeed >= hLowerThreshold)
            {
                if(hEstimatedSpeed <= hUpperThreshold)
                {
                    bConvCounter++;
                    if (bConvCounter >= NB_CONSECUTIVE_TESTS)
                    {
                    // ...the algorithm converged.
                        return(TRUE);
                    }
                    else
                    {
                        leeupcnt++;
                        return(FALSE);
                    }            
                }
                else
                { 
                    bConvCounter = 0;
                    leeupcnt1++;
                    return(FALSE);
                }              
            }
            else
            { 
                bConvCounter = 0;
                leelowcnt++;
                return(FALSE);
            } 
        }
        else
        { 
            bConvCounter = 0;
            leelowcnt1++;
            return(FALSE);
        } 
    }
    else
    { 
        bConvCounter = 0;
        leespdeffcnt++;
        return(FALSE);
    }    
}

/*******************************************************************************
* Function Name : STO_Get_Speed_Hz
* Description : It returns the motor mechanical speed (Hz*10)
* Input : None.
* Output : None.
* Return : hRotor_Speed_Hz.   
* 应该是0.1hz，机械0.1hz
*******************************************************************************/
s16 STO_Get_Speed_Hz(void)
{
  return (s16)((STO_Get_Speed() * SAMPLING_FREQ * 10)/(65536 * POLE_PAIR_NUM));
}

/*******************************************************************************
* Function Name : STO_Get_Mechanical_Angle
* Description : It returns the rotor position (mechanical angle,s16) 
* Input : None.
* Output : None.
* Return : hRotor_El_Angle/pole pairs number.
*******************************************************************************/
s16 STO_Get_Mechanical_Angle(void)
{
    return ((s16)(STO_Get_Electrical_Angle()/POLE_PAIR_NUM));
}
MOTERMODE_TypeDef motormode;
int16_t hForceSpeed01Hz=0;
int32_t  RampLocalTime=0;
		 int16_t hForcedMecSpeed01Hz;
		 uint8_t ObserverConverged=0;
#define RAMPUOTIME 300

void  HandleMode(void)
{
	 switch(motormode)
	 {
		 case MOTORSTOP:
					if((int16_t)(oSTC[M1]->Vars.hSpeedRef01HzExt)!=0)
					{
					   
						 FOCVars[M1].bRunning = 1;
						 FOCVars[M2].bRunning = 1;
						 TIM_CtrlPWMOutputs(TIM1, ENABLE);
						 TIM_CtrlPWMOutputs(TIM8, ENABLE);
						 ObserverConverged =0;
						RampLocalTime=0;
						 //FOCVars[M1].bSpdRef0Flage = 1;
						 //FOCVars[M2].bSpdRef0Flage = 1;
							motormode=MOTORSTART;
					}
			 break;
		 case MOTORSTART:
        if(RampLocalTime <150)  
	         RampLocalTime	++;
				RampUpControl();
				if(RampLocalTime>25)
				{
					RampLocalTime=0;
          motormode=MOTOROPEN;
				}
			 break ;
		 case MOTOROPEN:
			 #ifdef Sensorless 	
        if(RampLocalTime <1550)  
	         RampLocalTime	++;
	 
      // RampUpControl();
		// ObserverConverged = IsObserverConverged();		//????
#endif	 
				//if(RampLocalTime >=RAMPUOTIME)  
			 if(ObserverConverged == TRUE)
			 {
						motormode=MOTORSWITCH;
			 }
			 break ;
     case MOTORSWITCH:
			     motormode=MOTORLOOP;
			  break ;
		 case MOTORLOOP:

			 break ;
		 case MOTORBRAKE:
			 break ;
		 	case MOTORSLEEP:
			 break ;
     default :
		 	break ;		
	 }		 
			if((ControlCommand.hSpeedM1==0)&&((FOCVars[M1].bRunning != 0)||(motormode != MOTORSTOP)))
				 {
				 			FOCVars[M1].bRunning = 0;
						 FOCVars[M2].bRunning = 0;
						 TIM_CtrlPWMOutputs(TIM1, DISABLE);
						 TIM_CtrlPWMOutputs(TIM8, DISABLE);
						 //bMotorEabsStartFlag=1;
						 //FOCVars[M1].bSpdRef0Flage = 0;
						 //FOCVars[M2].bSpdRef0Flage = 0;
					   //hleeSpeedtemp =3000;
							motormode=MOTORSTOP;
				 }
}
volatile int16_t  hForceElSpeedDpp=0;
volatile int16_t  hForceElAngle=0;
volatile int16_t  hForceTorque_Reference=0;
//int16_t hMecSpeed01Hz;
int32_t wElSpeedDpp32=0;
int8_t Oflag=0;
uint8_t  An2Flag=0;
volatile int32_t  hAvrMecSpeed01Hz=0;
int32_t leedpp=26432;
int16_t hleeSpeedtime=100;
int8_t RampUpControl( void)
{

      bool SpeedSensorReliability = FALSE ;

    if(motormode == MOTORSTART)//
     {
           wElSpeedDpp32 = 0;
           hForceElSpeedDpp = ( int16_t )(wElSpeedDpp32 / 65536 );
                    /* Convert dpp into Mec01Hz */
           hAvrMecSpeed01Hz = ( int16_t )( ( hForceElSpeedDpp *
                                    ( int32_t )SAMPLING_FREQ * 10 ) /
                                  ( 65536 * ( int32_t )POLE_PAIR_NUM) );

           // hAvrMecSpeed01Hz = hMecSpeed01Hz;hElSpeedDpp
     }
    else if(motormode == MOTOROPEN)//
     {
                if(hleeSpeedtime<RAMPUOTIME)
                {
                    hleeSpeedtime++;
                    wElSpeedDpp32 += leedpp;
                }
          //if(pHandle->_Super.hAvrMecSpeed01Hz<0)

                hForceElSpeedDpp = ( int16_t )( wElSpeedDpp32 / 65536 );
                    /* Convert dpp into Mec01Hz */
                hAvrMecSpeed01Hz = ( int16_t )( (hForceElSpeedDpp *
                                    ( int32_t )SAMPLING_FREQ * 10 ) /
                                  ( 65536 * ( int32_t )POLE_PAIR_NUM ) );

         //   hAvrMecSpeed01Hz = hMecSpeed01Hz;

     }

    SpeedSensorReliability = STO_Check_Speed_Reliability();

  return ( SpeedSensorReliability );
}

void FORCE_CalcElAngle(void)
{
    hForceElAngle += hForceElSpeedDpp;
}
uint16_t loop_run_flag = 0;
uint16_t sto_restart_count = 0;
uint8_t sto_startfail_flag = 0;
uint8_t sto_rampup_overtime = 0;
uint32_t i_local_pwm_steps = 0;
uint32_t freq_startup_pwm_steps = 0;
uint16_t align_delay_count = 0;
uint16_t align_delay_countset = 1; //62.5u * 10000 * 4 = 625ms * 4
uint16_t restart_delay_count = 0;
uint16_t restart_delay_countset = 40000; //62.5u * 10000 * 4 = 625ms * 4
//I_LOCAL_PWM_STEPS FREQ_STARTUP_PWM_STEPS
/*******************************************************************************
* Function Name : STO_Start_Up
* Description : This function implements the ramp-up by forcing a stator current 
*               with controlled amplitude and frequency. If the observer 
*               algorithm converged, it also assign RUN to State variable  
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void STO_Start_Up(void)
{
    s16 hAux;
    i_local_pwm_steps = I_LOCAL_PWM_STEPS;
    freq_startup_pwm_steps = FREQ_STARTUP_PWM_STEPS;     
    
#ifdef NO_SPEED_SENSORS_ALIGNMENT
  static u32 wAlignmentTbase=0;
#endif  
  
    switch(Start_Up_State)
    {
        case S_INIT:
    //Init Ramp-up variables
    if (FOCVars[M1].hTeref  >= 0)
    {
      hFreq_Inc = FREQ_INC;
      hI_Inc = I_INC;           //当前增量是0
      if (wTime == 0)
      {
        wStart_Up_I = FIRST_I_STARTUP * 1024;    //电流是在这个地方直接给了阶跃 FINAL_I_STARTUP FIRST_I_STARTUP
      }
    }
    else
    {
      hFreq_Inc = -(s16)FREQ_INC;
      hI_Inc = -(s16)I_INC; 
      if (wTime == 0)
      {
        wStart_Up_I = -(s32)FIRST_I_STARTUP * 1024;
      }
    }
    Start_Up_State = ALIGNMENT;
//    Start_Up_State = RAMP_UP;
    break;
    
  case ALIGNMENT:
#ifdef NO_SPEED_SENSORS_ALIGNMENT

#else
    wTime ++;
    wTime_test_align = wTime;
    if (wTime <= I_LOCAL_PWM_STEPS)
    {
/*        
        wStart_Up_Freq = 0;
      //wStart_Up_I = 0;
        hAux = wStart_Up_Freq / 65536;
//    hForceElAngle = (s16)(hForceElAngle + (s32)(65536/(SAMPLING_FREQ/hAux)));
        hForceElAngle = 0; //-32767->32767对应-180->180，270°就是-90°，所以给 16,383.5    
   
        hAux = wStart_Up_I / 1024;
        hForceTorque_Reference = hAux; //
*/        
        hForceElAngle = 0; //-32767->32767对应-180->180，270°就是-90°，所以给 16,383.5   -16384      
        hForceTorque_Reference = I_LOCAL_STARTUP; //I_LOCAL_STARTUP
    }
    else 
    {
        Start_Up_State = RAMP_UP;
        wTime = 0; 
/*        
        hForceElAngle = 0;     
//        hForceTorque_Reference = 0;        
        if(align_delay_count >= align_delay_countset)
        {
            Start_Up_State = RAMP_UP;
            wTime = 0;  
//            wStart_Up_I = 0;           
        }
        else
        {
            align_delay_count++;
        }
*/      
    }

#endif    
    break;
    
  case RAMP_UP:
    wTime ++;  
    wTime_test_ramp = wTime;
    if (wTime <= I_STARTUP_PWM_STEPS)
    {     
      wStart_Up_Freq += hFreq_Inc;
      wStart_Up_I += hI_Inc;
    }
    else if (wTime <= FREQ_STARTUP_PWM_STEPS )
    {
      wStart_Up_Freq += hFreq_Inc;
    }       
    else if (wTime >= 2 * FREQ_STARTUP_PWM_STEPS )
    {
        sto_rampup_overtime++;
      //MCL_SetFault(START_UP_FAILURE); //lee 启动故障
      //Re_initialize Start Up
     // STO_StartUp_Init();
    }
    
    //Add angle increment for ramp-up
    hAux = wStart_Up_Freq / 65536;
    hForceElAngle = (s16)(hForceElAngle + (s32)(65536/(SAMPLING_FREQ / hAux)));
        
    hAux = wStart_Up_I / 1024;
    hForceTorque_Reference = hAux;          

    if (IsObserverConverged() == TRUE || (wTime >= 2 * FREQ_STARTUP_PWM_STEPS))
    {     
        wTime_test = wTime;
        Start_Up_State = LOOPRUN;
    }    
    break;
        case LOOPRUN:  
//        loop_run_flag = 10000;  
//            if((oSTC[M1]->Vars.hTorqueRef  == 0 || wTime >= 2 * FREQ_STARTUP_PWM_STEPS) && sto_restart_count <= 3 )
            if((wTime >= 2 * FREQ_STARTUP_PWM_STEPS) && sto_restart_count <= 3 )

            {
                sto_restart_count++;
                STO_StartUp_Init();             
                STO_Init();  
                if(FOCVars[M1].bRunning == 1)
                {                
                  FOCVars[M1].bRunning = 0;				
                } 
                if(restart_delay_count >= restart_delay_countset)
                {
                      Start_Up_State = S_INIT;
                }
                else
                {
                    Start_Up_State = LOOPRUN;
                    restart_delay_count++;                 
                }                 
            }
            else if(sto_restart_count > 3)
            {
                sto_startfail_flag = 214;
            }
	 
    break;
  default:
    break;
  }    
}

/*******************************************************************************
* Function Name : STO_StartUp_Init
* Description : This private function initializes the sensorless start-up
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void STO_StartUp_Init(void)
{
  //Re_initialize Start Up
  Start_Up_State = S_INIT;  
  hAngle = 0;
  wTime = 0;
  wStart_Up_Freq = 0;
  bConvCounter = 0; 
}      
      
/*******************************************************************************
* Function Name : STO_Obs_Gains_Update
* Description : This function updates state observer gains after they have been
*               changed by the user interface
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
#ifdef OBSERVER_GAIN_TUNING
void STO_Obs_Gains_Update(void)
{
  StateObserver_GainsUpdate STO_GainsUpdateStruct;

  STO_GainsUpdateStruct.PLL_P = hPLL_P_Gain;
  STO_GainsUpdateStruct.PLL_I = hPLL_I_Gain;
  STO_GainsUpdateStruct.hC2 = HC2_INIT;  
  STO_GainsUpdateStruct.hC4 = HC4_INIT;            
  STO_Gains_Update(&STO_GainsUpdateStruct);
}      
#endif      
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
