/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name	     : MC_FOC_Methods.c
* Author 	     : IMS Systems Lab
* Date First Issued  : 29/05/08
* Description 	     : This module provides additional functionalities to  
*                      SM-PMSM/IPMSM FOC: flux weakening, feed-forward
*		       currents regulation, IPMSM MTPA control.
********************************************************************************
* History:
* 29/05/08 v2.0
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
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
#define SEGMNUM (u8)7 //coeff no. -1

/* Private macro -------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
s32 Square_root32 (s32);

/* Private variable ----------------------------------------------------------*/
static MTPA_Const MTPA_InitStructure;
static PID_Struct_t PI_Stat_Volt_InitStructure;
static s32 wNominalCurrent;
static s16 hIdDemag;
static s16 hIdRefOld;
static volatile s32 wVMagn;
static s32 wConstant_1Q;
static s32 wConstant_1D;
static s32 wConstant_2;

/*******************************************************************************
* Function Name : FOC_MTPA
* Description   : This function implements the IPMSM MTPA optimized drive.
* Input         : Reference current iq* (signed, 16 bits).
* Output        : Reference current id* (signed, 16 bits).
* Return        : None.
*******************************************************************************/
s16 FOC_MTPA(s16 hIqRef)
{
  s32 wIdRef;
  u8 bSegment;
  
  hIqRef = (hIqRef < 0 ? (-hIqRef) : (hIqRef));   //abs value
  
  bSegment = (u8)(hIqRef/MTPA_InitStructure.hsegdiv);
  if (bSegment > SEGMNUM)
  {
    bSegment = SEGMNUM;
  }
  
  wIdRef = (s16)((MTPA_InitStructure.wangc[bSegment]*hIqRef)/32768 +
  			MTPA_InitStructure.wofst[bSegment]);
  
  if (wIdRef < hIdDemag)
  {
    wIdRef = hIdDemag;
  }
  
  return ((s16)(wIdRef));  
}

/*******************************************************************************
* Function Name : FOC_FluxRegulator
* Description   : This function implements the Flux Weakening algorithm
* Input         : Reference currents (iq*,id*), reference voltages (vq*,vd*),
*		  stator voltage amplitude to be kept as reference level.
* Output        : Reference currents (iqsat**,id**).
* Return        : None.
*******************************************************************************/
Curr_Components FOC_FluxRegulator (Curr_Components Stat_Curr_qd_ref,
                                   Volt_Components Stat_Volt_qd,
                                   s16 hVoltLevel)
{
  s32 wIdRef;
  s32 wVMagnSq;
  
  s32 wIqSatSq;
  s32 wIqSat;
  
  s16 hId_fw;
  
  wVMagnSq = (s32)(Stat_Volt_qd.qV_Component1*Stat_Volt_qd.qV_Component1 +
              Stat_Volt_qd.qV_Component2*Stat_Volt_qd.qV_Component2);
  
  wVMagn = Square_root32(wVMagnSq);
  
  if (wVMagn > S16_MAX)
  {
    wVMagn = S16_MAX;
  }

  hId_fw = PID_Regulator(hVoltLevel, (s16)wVMagn, &PI_Stat_Volt_InitStructure);

  //Avoiding interaction between mtpa & fw, 
  if (hId_fw >= 0)
  {
    hIdRefOld = Stat_Curr_qd_ref.qI_Component2;
    wIdRef = Stat_Curr_qd_ref.qI_Component2;
  }
  else
  {
    wIdRef = hIdRefOld + hId_fw;
  }
  
  if (wIdRef < hIdDemag)
  {
    wIdRef = hIdDemag;
  }
  
  //Iqref saturation
  wIqSatSq = (s32)(wNominalCurrent - wIdRef*wIdRef);
  wIqSat = Square_root32(wIqSatSq);

  Stat_Curr_qd_ref.qI_Component1 = (s16)wIqSat;
  Stat_Curr_qd_ref.qI_Component2 = (s16)wIdRef;
  return Stat_Curr_qd_ref;
}

/*******************************************************************************
* Function Name : FOC_MTPA_Init
* Description   : This function is called by FOC_MTPAInterface_Init to
*		  initialize the MTPA algorithm according to default parameters
*		  defined in MC_PMSM_motor_param.h
* Input         : MTPA initialization structure, maximum allowed reference
*		  current id*
* Output        : None.
* Return        : None.
*******************************************************************************/
void FOC_MTPA_Init(MTPA_Const MTPA_InitStructure_in, s16 hIdDemag_in)
{
  MTPA_InitStructure = MTPA_InitStructure_in;
  hIdDemag = hIdDemag_in;
}

/*******************************************************************************
* Function Name : FOC_FluxRegulator_Init
* Description   : This function is called by FOC_FluxRegulatorInterface_Init
*		  to initialize the flux-weakening algorithm according to
*		  default parameters defined in MC_PMSM_motor_param.h
* Input         : Pointer to a PID instance structure (PID_Struct_t structure),
*		  motor nominal current (positive signed 16 bits)
* Output        : None.
* Return        : None.
*******************************************************************************/
void FOC_FluxRegulator_Init(PID_Struct_t *PI_Stat_Volt_InitStructure_in,
							s16 hNominalCurrent_in)
{
  wNominalCurrent = (s32)(hNominalCurrent_in*hNominalCurrent_in);
  hIdDemag = PI_Stat_Volt_InitStructure_in->hLower_Limit_Output;
  //PI_Stat_Volt_InitStructure = PI_Stat_Volt_InitStructure_in;
  PI_Stat_Volt_InitStructure.hKp_Gain = PI_Stat_Volt_InitStructure_in->hKp_Gain;
  PI_Stat_Volt_InitStructure.hKp_Divisor = 
                                    PI_Stat_Volt_InitStructure_in->hKp_Divisor;
  PI_Stat_Volt_InitStructure.hKi_Gain = PI_Stat_Volt_InitStructure_in->hKi_Gain;
  PI_Stat_Volt_InitStructure.hKi_Divisor = 
                                    PI_Stat_Volt_InitStructure_in->hKi_Divisor; 
  //Lower Limit for Output limitation
  PI_Stat_Volt_InitStructure.hLower_Limit_Output = 
                            PI_Stat_Volt_InitStructure_in->hLower_Limit_Output;
  //Upper Limit for Output limitation
  PI_Stat_Volt_InitStructure.hUpper_Limit_Output = 
                            PI_Stat_Volt_InitStructure_in->hUpper_Limit_Output;
  PI_Stat_Volt_InitStructure.wLower_Limit_Integral = 
                              PI_Stat_Volt_InitStructure.hLower_Limit_Output *
                              PI_Stat_Volt_InitStructure.hKi_Divisor;   //Lower Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wUpper_Limit_Integral = 0;   //Upper Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wIntegral = 0;
  
  PI_Stat_Volt_InitStructure.hKd_Gain = PI_Stat_Volt_InitStructure_in->hKd_Gain;
  PI_Stat_Volt_InitStructure.hKd_Divisor = 
                                     PI_Stat_Volt_InitStructure_in->hKd_Divisor; 
  PI_Stat_Volt_InitStructure.wPreviousError = 
                                  PI_Stat_Volt_InitStructure_in->wPreviousError;
  
  
  hIdRefOld = 0;
  wVMagn = 0;
}

/*******************************************************************************
* Function Name : FOC_FluxRegulator_Update
* Description   : According to the user input, it modifies the proportional and
*		  integral gains of the PI regulator implemented in the
*		  flux-weakening block
* Input         : Proportional gain (positive signed 16 bits),
*		  integral gain (positive signed 16 bits)
* Output        : Stator voltage amplitude (positive signed 16 bits)
* Return        : None.
*******************************************************************************/
s16 FOC_FluxRegulator_Update(s16 hKpGain, s16 hKiGain)
{
  PI_Stat_Volt_InitStructure.hKp_Gain = hKpGain;
  PI_Stat_Volt_InitStructure.hKi_Gain = hKiGain;
  
  return ((s16)(wVMagn));
}

/*******************************************************************************
* Function Name  : FF_CurReg_Init
* Description    : According to the used motor and to the parameters written
*		   in the feed-forward section of MC_PMSM_motor_param.h, it
*		   initializes all the variables related to feed-forward
*		   operations to proper values (FOC_FF_CurrReg function). It has
*		   to be called at least once before the first motor startup.
* Input          : Signed 32 bits parameters from the related section in
*		   MC_PMSM_motor_param.h                   
* Output         : None
* Return         : None
*******************************************************************************/
void FOC_FF_CurrReg_Init(s32 wConstant1Q, s32 wConstant1D, s32 wConstant2)
{
  wConstant_1Q = wConstant1Q;
  wConstant_1D = wConstant1D;
  wConstant_2 = wConstant2;
}

/*******************************************************************************
* Function Name  : FF_CurReg
* Description    : This function implements the Feed-Forward current regulation
*		   functionality
* Input          : Reference currents (iq**,id**), PI reference voltages
*		   (vq*,vd*), rotor electrical speed in dpp, DC bus voltage(s16)
* Output         : Reference voltages (vq*,vd*)
* Return         : None
*******************************************************************************/
Volt_Components FOC_FF_CurrReg(Curr_Components Stat_Curr_qdref,
                          Volt_Components PI_correction,s16 hspeed,s16 hvbus)
{
  Volt_Components Stat_Voltage_qd_ff;  
  s32 wtemp1;
  s32 wtemp2;
  //q-axes ff voltage calculation
  wtemp1 = (s32)(((s32)(hspeed) * (s32)(Stat_Curr_qdref.qI_Component2))/(s32)(32768));
  wtemp2 = (s32)(((wtemp1 * wConstant_1D)/(s32)(hvbus))*2);
  
  wtemp1 = (s32)(((wConstant_2*(s32)(hspeed))/hvbus)*16);
  
  wtemp2 = wtemp1 + wtemp2 + PI_correction.qV_Component1;
  
  if (wtemp2 > S16_MAX)
  {
    wtemp2 = S16_MAX;
  }
  else if (wtemp2 < -S16_MAX)
  {
    wtemp2 = -S16_MAX;
  }
  
  Stat_Voltage_qd_ff.qV_Component1 = (s16)(wtemp2);
  
  //d-axes ff voltage calculation
  wtemp1 = (s32)(((s32)(hspeed) * (s32)(Stat_Curr_qdref.qI_Component1))/(s32)(32768));
  wtemp2 = (s32)((wtemp1 * wConstant_1Q)/(s32)(hvbus)*2);
  
  wtemp2 = PI_correction.qV_Component2 - wtemp2;
  
  if (wtemp2 > S16_MAX)
  {
    wtemp2 = S16_MAX;
  }
  else if (wtemp2 < -S16_MAX)
  {
    wtemp2 = -S16_MAX;
  }  
  
  Stat_Voltage_qd_ff.qV_Component2 = (s16)(wtemp2);
 
  return (Stat_Voltage_qd_ff);
}

/*******************************************************************************
* Function Name : Square_root32
* Description   : this function calculates the square root of a non-negative s32.
*		  It returns 0 for negative s32.
* Input         : signed int32 (s32).
* Output        : Square root of the input.
* Return        : None.
*******************************************************************************/
s32 Square_root32 (s32 w_in)
{
  u8 biter = 0;
  s32 wtemproot;
  s32 wtemprootnew;

  if (w_in > 0)
  {
    
    if (w_in <= 2097152)
    {
      wtemproot = 128;
    }
    else
    {
      wtemproot = 8192;
    }
    
    do
    {
      wtemprootnew = (wtemproot + w_in/wtemproot)/2;
      if (wtemprootnew == wtemproot)
      {
        biter = 6;
      }
      else
      {
        biter ++;
        wtemproot = wtemprootnew;
      }
    }
    while (biter < 6);
  }
  else
  {
    wtemprootnew = 0;
  }
  
  return (wtemprootnew);
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
