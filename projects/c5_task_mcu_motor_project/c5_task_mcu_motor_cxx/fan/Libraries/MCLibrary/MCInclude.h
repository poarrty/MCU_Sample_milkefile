/******************************************************************************
* File Name         :  MCInclude.h
* Author            :  ���ŷ� 
* Version           :  1.0 
* Date              :  20/02/06
* Description       :  ������ؿ�����ͷ�ļ�                       
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* ���ŷ�             20/02/06      1.0               ����   
*
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MCINCLUDE_H
#define __MCINCLUDE_H

/* Includes ------------------------------------------------------------------*/




//#define ENCODER_SENSOR
#define SWITCH_HALL
//#define LINEAR_HALL




/* Includes ------------------------------------------------------------------*/
#include "Type.h"
#include "inc/Math.h"
//#include "MC_FOC_Methods.h"
#include "MC_State_Observer.h"
#include "MC_State_Observer_Interface.h"
#include "MC_State_Observer_param.h"
#ifdef LINEAR_HALL
#include "LinearHallSensor.h"
#define HALL_Init					LINHALL_Init
#define HALL_CalcElAngle			LINHALL_HandleHallParam    		
#define HALL_GetElAngle				LINHALL_GetElAngle				    
#define HALL_GetAvrgMecSpeed01Hz	LINHALL_GetAvrgMecSpeed01Hz		
#define HALL_MeasureInit			LINHALL_MeasureInit				
#define pPOS_t					    pHALL_t
#define oPosSen					    oHall	
#define pPOSVars_t				    pHALLVars_t
#endif
#ifdef SWITCH_HALL
#include "HallSensor.h"
#define POS_Init				    HALL_Init
#define POS_CalcElAngle			    HALL_CalcElAngle    		
#define POS_GetElAngle			    HALL_GetElAngle				    
#define POS_GetAvrgMecSpeed01Hz	    HALL_GetAvrgMecSpeed01Hz		
#define POS_MeasureInit			    HALL_MeasureInit	
#define pPOS_t					    pHALL_t
#define oPosSen					    oHall	
#define pPOSVars_t				    pHALLVars_t
#endif
#ifdef ENCODER_SENSOR
#include "EncoderSensor.h"
#define POS_Init				    ENC_Init
#define POS_CalcElAngle			    ENC_CalcElAngle    		
#define POS_GetElAngle			    ENC_GetElAngle				    
#define POS_GetAvrgMecSpeed01Hz	    ENC_GetAvrgMecSpeed01Hz		
#define POS_MeasureInit			    ENC_MeasureInit	
#define oPosSen					    oEncoder	
#define pPOS_t					    pENC_t
#define pPOSVars_t				    pENCVars_t

#endif

#include "CircleLimitation.h"
#include "PIRegulator.h"
#include "PWMnCurrentFdbk.h"
#include "CurrentLimitation.h"  
#include "NTCSensor.h"
#include "SpeednTorqCtrl.h"
#include "MTPAControl.h"

//#include "MCLibraryConf.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"




#endif 

/**************************************************************END OF FILE****/
