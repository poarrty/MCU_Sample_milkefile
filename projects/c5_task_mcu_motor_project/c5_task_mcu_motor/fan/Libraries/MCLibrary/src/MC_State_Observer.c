/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : MC_State_Observer.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This module implements the State Observer of 
*                      the PMSM B-EMF, thus identifying rotor speed and position
*
********************************************************************************
* History:
* 21/11/07 v1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "UserInclude.h"
#include "MCInclude.h"



/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE (u8)64//64

/* Private variables ---------------------------------------------------------*/
s32 wIalfa_est,wIbeta_est;
static s32 wBemf_alfa_est, wBemf_beta_est;
static s32 wSpeed_PI_integral_sum;
s16 hBemf_alfa_est, hBemf_beta_est;

s16 hSpeed_Buffer[BUFFER_SIZE];
//static s16 hRotor_El_Angle;
//static s16 hRotor_Speed_dpp;
//static volatile s16 hSpeed_P_Gain, hSpeed_I_Gain;
s16 hRotor_El_Angle;
s16 hRotor_Speed_dpp;
volatile s16 hSpeed_P_Gain, hSpeed_I_Gain;
static bool Max_Speed_Out, Min_Speed_Out, Is_Speed_Reliable;
static u8 bSpeed_Buffer_Index;

extern const s16 hSin_Cos_Table[256];

static volatile s16 hC2, hC4;
s16 hF1, hF2, hF3, hC1, hC3,hC5, hC6;



static s32 wMotorMaxSpeed_dpp;
static u16 hPercentageFactor;

/* Private function prototypes -----------------------------------------------*/
s16 Calc_Rotor_Speed(s16, s16);
s16 Speed_PI(s16, s16);
void Store_Rotor_Speed(s16);

//function of MC_Clarke_Park.c
//Trig_Components Trig_Functions(s16);

/*******************************************************************************
* Function Name : STO_Calc_Rotor_Angle
* Description : This function implements the State Observer of a
*       	PMSM back-emfs. Thus, motor speed and rotor angle are calculated
*	        and stored in the variables hRotor_Speed,hRotor_El_Angle.
* Input : Stator voltage reference values Valpha, Vbeta (as provided by RevPark
*         through variable Stat_Volt_alfa_beta),
*	  Stator current values Ialpha,Ibeta (as provided by Clark trasformation
*	  through variable Stat_Curr_alfa_beta),  DC bus voltage.
* Output : None
* Return : None
*******************************************************************************/
s16 hRotor_Speed;
s16 leewBemf_alfa_est1=0;
s16 leewBemf_alfa_est2=0;
s16 leewBemf_beta_est1=0;
s16 leewBemf_beta_est2=0;
s16 leewIalfa_est1=0;
s16 leewIalfa_est2=0;
s16 leewIbeta_est1=0;
s16 leewIbeta_est2=0;
s32 bDirection_out = 1;
void STO_Calc_Rotor_Angle(Volt_Components Stat_Volt_alfa_beta,
                         Curr_Components Stat_Curr_alfa_beta,
                         u16 hBusVoltage)
{
  s32 wIalfa_est_Next, wIbeta_est_Next;
  s32 wBemf_alfa_est_Next, wBemf_beta_est_Next;
  
  s16 hValfa, hVbeta;
  s16 hIalfa_err, hIbeta_err;

  s32 bDirection;
  s32 temp;
 
    
  hIalfa_err = (s16)((wIalfa_est/hF1)-Stat_Curr_alfa_beta.qI_Component1);
  hIbeta_err = (s16)((wIbeta_est/hF1)-Stat_Curr_alfa_beta.qI_Component2);
  
//  hValfa = (s16)((Stat_Volt_alfa_beta.qV_Component1*hBusVoltage)/32768);
//  hVbeta = (s16)((Stat_Volt_alfa_beta.qV_Component2*hBusVoltage)/32768);
  temp = (s32)(Stat_Volt_alfa_beta.qV_Component1*(hBusVoltage>>1));
  hValfa=  temp>>15;
  temp = (s32)(Stat_Volt_alfa_beta.qV_Component2*(hBusVoltage>>1));
  hVbeta=  temp>>15;
  /*alfa axes observer*/
  wIalfa_est_Next = (s32)(wIalfa_est-(s32)(hC1*(s16)(wIalfa_est/hF1))+
                          (s32)(hC2*hIalfa_err)+
                            (s32)(hC5*hValfa)-
                              (s32)(hC3*(s16)(wBemf_alfa_est/hF2)));
  
  wBemf_alfa_est_Next = (s32)(wBemf_alfa_est+(s32)(hC4*hIalfa_err)+
                              (s32)(hC6*hRotor_Speed_dpp*(wBemf_beta_est/(hF2*hF3))));                              

  
  /*beta axes observer*/
  wIbeta_est_Next = (s32)(wIbeta_est-(s32)(hC1*(s16)(wIbeta_est/hF1))+
                          (s32)(hC2*hIbeta_err)+
                            (s32)(hC5*hVbeta)-
                              (s32)(hC3*(s16)(wBemf_beta_est/hF2)));
  
  wBemf_beta_est_Next = (s32)(wBemf_beta_est+(s32)(hC4*hIbeta_err)-
                              (s32)(hC6*hRotor_Speed_dpp*(wBemf_alfa_est/(hF2*hF3))));                              
     
    //Extrapolation of present rotation direction, necessary for PLL 
/* 
    if (hRotor_Speed_dpp >= 0)
    {
        bDirection = 1;
        bDirection_out = 1;        
    }
    else
    {
        bDirection = -1;
        bDirection_out = -1;           
    }  
*/
    bDirection = 1;
  /*Calls the PLL blockset*/

  hBemf_alfa_est = wBemf_alfa_est / hF2;
  hBemf_beta_est = wBemf_beta_est / hF2;
    
  hRotor_Speed = Calc_Rotor_Speed((s16)(hBemf_alfa_est * bDirection),
                                  (s16)(-hBemf_beta_est * bDirection));
  
  Store_Rotor_Speed(hRotor_Speed);
      
  hRotor_El_Angle = (s16)(hRotor_El_Angle + hRotor_Speed);
    
  /*storing previous values of currents and bemfs*/
  wIalfa_est = wIalfa_est_Next;
  wBemf_alfa_est = wBemf_alfa_est_Next;
  
  wIbeta_est = wIbeta_est_Next;
  wBemf_beta_est = wBemf_beta_est_Next;
		
  if (wBemf_alfa_est > (s32)(S16_MAX*hF2))
  {
    wBemf_alfa_est = S16_MAX*hF2;
    leewBemf_alfa_est1++;
  }
  else if (wBemf_alfa_est <= (s32)(S16_MIN*hF2))
  {
    wBemf_alfa_est = -S16_MAX * hF2;
    leewBemf_alfa_est2++;
  }
    
  if (wBemf_beta_est > (s32)(S16_MAX * hF2))
  {
    wBemf_beta_est = S16_MAX * hF2;
    leewBemf_beta_est1 ++;
  }
  else if (wBemf_beta_est <= (s32)(S16_MIN * hF2))
  {
    wBemf_beta_est = -S16_MAX * hF2;
    leewBemf_beta_est2++;
  }

  if (wIalfa_est > (s32)(S16_MAX * hF1))
  {
    wIalfa_est = S16_MAX * hF1;
    leewIalfa_est1 ++;
  }
  else if (wIalfa_est <= (s32)(S16_MIN * hF1))
  {
    wIalfa_est = -S16_MAX * hF1;
    leewIalfa_est2++;
  }
    
  if (wIbeta_est > S16_MAX * hF1)
  {
    wIbeta_est = S16_MAX * hF1;
    leewIbeta_est1 ++;
  }
  else if (wIbeta_est <= S16_MIN * hF1)
  {
    wIbeta_est = -S16_MAX * hF1;
    leewIbeta_est2 ++;
  }   
}

/*******************************************************************************
* Function Name : STO_Calc_Speed
* Description : It averages the values of hRotor_Speed contained in the speed
*               buffer; motor speed is stored in the variable hRotor_Speed_dpp
* Input : None.
* Output : None.
* Return : None.
*******************************************************************************/
void STO_Calc_Speed(void)
{ 
    s32 wAverage_Speed = 0;
    s32 wError;
    s32 wAverageQuadraticError = 0;
    u8 i;
    
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        wAverage_Speed += hSpeed_Buffer[i];
    }
  
    wAverage_Speed /= BUFFER_SIZE;

    hRotor_Speed_dpp = (s16)(wAverage_Speed);

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        wError = hSpeed_Buffer[i] - wAverage_Speed;
        wError = (wError * wError);
        wAverageQuadraticError += (u32)(wError);
    }
  
      //It computes the measurement variance   
    wAverageQuadraticError /= BUFFER_SIZE;
  
    //The maximum variance acceptable is here calculated as ratio of average speed 
    wAverage_Speed = (s32)(wAverage_Speed * wAverage_Speed);
    wAverage_Speed = (wAverage_Speed / 128) * hPercentageFactor;

    if (wAverageQuadraticError > wAverage_Speed)
    {
        Is_Speed_Reliable = FALSE;
    }
    else
    {
        Is_Speed_Reliable = TRUE;
    }
}

/*******************************************************************************
* Function Name : STO_IsSpeed_Reliable
* Description : Return latest value of variable Is_Speed_Reliable
* Input : None
* Output : None
* Return : boolean value: TRUE if speed is reliable, FALSE otherwise.
*******************************************************************************/
bool STO_IsSpeed_Reliable(void)
{
  return(Is_Speed_Reliable);
}

/*******************************************************************************
* Function Name : STO_Get_Speed
* Description : It returns the motor electrical speed (dpp)
* Input : None.
* Output : None.
* Return : hRotor_Speed_dpp.
*******************************************************************************/
s16 STO_Get_Speed(void)
{
  return (hRotor_Speed_dpp);
}

/*******************************************************************************
* Function Name : STO_Get_Electrical_Angle
* Description : It returns the rotor position (electrical angle,s16) 
* Input : None.
* Output : None.
* Return : hRotor_El_Angle.
*******************************************************************************/
s16 STO_Get_Electrical_Angle(void)
{
  return (hRotor_El_Angle);
}

extern Curr_Components Bemf_alphabeta, Bemf_qd;
extern s16 hBemf_alfa_est, hBemf_beta_est;
extern s32 Ed_sum;
extern s16 Ed_ave;
/*******************************************************************************
* Function Name : STO_Init
* Description : It initializes all the State Observer related variables to
*               suitable values.
* Input : None.
* Output : None.
* Return : None.
*******************************************************************************/
void STO_Init(void)
{  
    wIalfa_est = 0;
    wIbeta_est = 0;
    wBemf_alfa_est = 0;
    wBemf_beta_est = 0;
    Is_Speed_Reliable = FALSE;

    wSpeed_PI_integral_sum = 0;
    Max_Speed_Out = FALSE;
    Min_Speed_Out = FALSE;

    hRotor_El_Angle = 0;            //could be used for start-up procedure
    hRotor_Speed_dpp = 0;

    STO_InitSpeedBuffer();

    Bemf_alphabeta.qI_Component1 = 0;
    Bemf_alphabeta.qI_Component2 = 0; 
    hBemf_alfa_est = 0;
    hBemf_beta_est = 0; 
    Bemf_qd.qI_Component1 = 0;
    Bemf_qd.qI_Component2 = 0;       
    Ed_ave = 0;
    Ed_sum = 0; 
}

/*******************************************************************************
* Function Name : STO_Init
* Description : It initializes all the State Observer related variables to
*               suitable values.
* Input : None.
* Output : None.
* Return : None.
*******************************************************************************/
void STO_Gains_Init(void)
{
    s16 htempk;
    s32 wAux= (int32_t)1;
    htempk = (s16)(C6_COMP_CONST1/(F2));//((100*65536)/(F2*2*PI));
    while (htempk != 0)
    {
      htempk /=(int16_t)2;
      wAux *= (int32_t)2;
      //(int32_t)hF3 *=(int32_t)2;
    }
    hF3 = (int16_t)wAux;
    wAux = (int32_t)F2*hF3;
    hC6 = (s16)(wAux/C6_COMP_CONST2);//((F2*hF3*2* PI)/65536);
  
  hC1 = C1;//StateObserver_ConstStruct->hC1;
  hC2 = C2; //StateObserver_ConstStruct->hC2;
  hC3 = C3;//C3;//StateObserver_ConstStruct->hC3;
  hC4 = C4;// StateObserver_ConstStruct->hC4;
  hC5 = C5;//StateObserver_ConstStruct->hC5;
  hC6 = hC6;//C6;//StateObserver_ConstStruct->hC6;
  hF1 = F1;//StateObserver_ConstStruct->hF1;
  hF2 = F2;//StateObserver_ConstStruct->hF2;
  hF3 = hF3;//StateObserver_ConstStruct->hF3;
  hSpeed_P_Gain = PLL_KP_GAIN;//StateObserver_ConstStruct->PLL_P;
  hSpeed_I_Gain = PLL_KI_GAIN;//StateObserver_ConstStruct->PLL_I;
  
  wMotorMaxSpeed_dpp = MOTOR_MAX_SPEED_DPP;// StateObserver_ConstStruct->wMotorMaxSpeed_dpp;
  hPercentageFactor = PERCENTAGE_FACTOR;// StateObserver_ConstStruct->hPercentageFactor;
}
/*******************************************************************************
* Function Name : Calc_Rotor_Speed
* Description : This function implements a PLL; it receives the motor back-emfs
*				and calculates motor speed.
* Input : Motor back-emfs hBemf_alpha,hBemf_beta.
* Output : None.
* Return : Motor speed (dpp, digit-per-pwm)
*******************************************************************************/
s16 Calc_Rotor_Speed(s16 hBemf_alfa, s16 hBemf_beta)
{
    s32 wAlfa_Sin_tmp, wBeta_Cos_tmp;
    s16 hOutput;
    Trig_Components Local_Components;
/*
    if (Start_Up_State == LOOPRUN)
    {
        Local_Components = MCM_Trig_Functions(hRotor_El_Angle);    
    }
    else
    {
        Local_Components = MCM_Trig_Functions(hForceElAngle);    
    }
*/    
    Local_Components = MCM_Trig_Functions(hRotor_El_Angle);

    /* Alfa & Beta BEMF multiplied by hRotor_El_Angle Cos & Sin*/
    wAlfa_Sin_tmp = (s32)(hBemf_alfa * Local_Components.hSin);
    wBeta_Cos_tmp = (s32)(hBemf_beta * Local_Components.hCos);

    /* Speed PI regulator */
    hOutput = Speed_PI((s16)(wAlfa_Sin_tmp / 32768), (s16)(wBeta_Cos_tmp / 32768));  

    return (hOutput);
}

/*******************************************************************************
* Function Name : Speed_PI
* Description : It implements the PLL PI regulator.
* Input : (B-emf alpha)*sin(theta),(B-emf beta)*cos(theta).
* Output : None.
* Return : Motor speed (dpp, digit-per-pwm).
*******************************************************************************/
s16 Speed_PI(s16 hAlfa_Sin, s16 hBeta_Cos)
{
  s32 wSpeed_PI_error, wOutput;
  s32 wSpeed_PI_proportional_term, wSpeed_PI_integral_term;
  
  wSpeed_PI_error = hBeta_Cos - hAlfa_Sin;
  
  wSpeed_PI_proportional_term = hSpeed_P_Gain * wSpeed_PI_error;  // !!!
  wSpeed_PI_integral_term = hSpeed_I_Gain * wSpeed_PI_error;      // !!!
  
  if ( (wSpeed_PI_integral_sum >= 0) && (wSpeed_PI_integral_term >= 0) 
        && (Max_Speed_Out == FALSE) )
  {
    if ( (s32)(wSpeed_PI_integral_sum + wSpeed_PI_integral_term) < 0)
    {
      wSpeed_PI_integral_sum = S32_MAX;
    }
    else    
    {
      wSpeed_PI_integral_sum += wSpeed_PI_integral_term;  
    }
  }
  else if ( (wSpeed_PI_integral_sum <= 0) && (wSpeed_PI_integral_term <= 0)
             && (Min_Speed_Out == FALSE) )
  {
    if ( (s32)(wSpeed_PI_integral_sum + wSpeed_PI_integral_term) > 0)
    {
      wSpeed_PI_integral_sum = -S32_MAX;
    }
    else 
    {
      wSpeed_PI_integral_sum += wSpeed_PI_integral_term;  
    }
  }
  else
  {
    wSpeed_PI_integral_sum += wSpeed_PI_integral_term;  
  }
  
  wOutput = (wSpeed_PI_proportional_term / 16384 + wSpeed_PI_integral_sum / 262144);
  
  if (wOutput > wMotorMaxSpeed_dpp)
  {
    Max_Speed_Out = TRUE;
    wOutput = wMotorMaxSpeed_dpp;		  			 	
  }
  else if (wOutput < (-wMotorMaxSpeed_dpp))
  {
    Min_Speed_Out = TRUE;
    wOutput = -wMotorMaxSpeed_dpp;
  } 
  else 
  {
    Max_Speed_Out = FALSE;
    Min_Speed_Out = FALSE;
  }  
  
  return ((s16)wOutput);
}

/*******************************************************************************
* Function Name : Store_Rotor_Speed
* Description : This function stores in a buffer, whose dimension is BUFFER_SIZE,
*               the last calculated value of the motor speed.
* Input : hRotor_Speed (dpp, digit-per-pwm)
* Output : None.
* Return : None.
*******************************************************************************/
void Store_Rotor_Speed(s16 hRotor_Speed)
{
  bSpeed_Buffer_Index++;
  if (bSpeed_Buffer_Index == BUFFER_SIZE) 
  {
    bSpeed_Buffer_Index = 0;
  }  
  hSpeed_Buffer[bSpeed_Buffer_Index] = hRotor_Speed;  
}
/*******************************************************************************
* Function Name : STO_InitSpeedBuffer
* Description : This function initializes the buffer for speed measurament 
* Input : None
* Output : None
* Return : None
*******************************************************************************/
void STO_InitSpeedBuffer(void)
{
  u8 i;
  
  /*init speed buffer*/
  for (i=0;i<BUFFER_SIZE;i++)
  {
    hSpeed_Buffer[i] = 0x00;
  }
  bSpeed_Buffer_Index = 0;
}

/*******************************************************************************
* Function Name : STO_Gains_Update
* Description : This function allows to write the private variables containing 
*               both observer and PLL gains;
* Input : Pointer to Sensorless_Gains type variable
* Output : None
* Return : None
*******************************************************************************/
void STO_Gains_Update(StateObserver_GainsUpdate* STO_GainsUpdateStruct)
{
  hC2 = STO_GainsUpdateStruct->hC2; 
  hC4 = STO_GainsUpdateStruct->hC4; 
  hSpeed_P_Gain = STO_GainsUpdateStruct->PLL_P;    
  hSpeed_I_Gain = STO_GainsUpdateStruct->PLL_I; 
}

/*******************************************************************************
* Function Name : STO_Get_wIalfa_est
* Description : This function allows to export the variables containing the 
*               observed current Ialfa in s16 format
* Input : None
* Output : None
* Return : observed Ialfa in s16 format
*******************************************************************************/
s16 STO_Get_wIalfa_est(void)
{
 return((s16)(wIalfa_est/hF1));
}
/*******************************************************************************
* Function Name : STO_Get_wIbeta_est
* Description : This function allows to export the variables containing the 
*               observed current Ibeta in s16 format
* Input : None
* Output : None
* Return : observed Ibeta in s16 format
*******************************************************************************/
s16 STO_Get_wIbeta_est(void)
{
 return((s16)(wIbeta_est/hF1));
}

/*******************************************************************************
* Function Name : STO_Get_wBemf_alfa_est
* Description : This function allows to export the variables containing the 
*               observed Bemf alfa in s16 format
* Input : None
* Output : None
* Return : observed Bemf alfa in s16 format
*******************************************************************************/
s16 STO_Get_wBemf_alfa_est(void)
{
 return((s16)(hBemf_alfa_est));
}

/*******************************************************************************
* Function Name : STO_Get_wBemf_beta_est
* Description : This function allows to export the variables containing the 
*               observed Bemf beta in s16 format
* Input : None
* Output : None
* Return : observed Bemf alfa in s16 format
*******************************************************************************/
s16 STO_Get_wBemf_beta_est(void)
{
 return((s16)(hBemf_beta_est));
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
