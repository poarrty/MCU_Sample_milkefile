/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : Contains the PMSM State Observer related parameters
*                      (module MC_State_Observer_Interface.c)
*
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_STATE_OBSERVER_PARAM_H
#define __MC_STATE_OBSERVER_PARAM_H
#include "Type.h"
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define MAX_CURRENT 18.8//2.9             /* max current value, Amps */
#define MCU_SUPPLY_VOLTAGE      3.30
#define RSHUNT                  0.005  // 35mΩ 0.035  5.6mΩ-0.0056
#define AMPLIFICATION_GAIN      2.5     // 电流放大倍数2.5  
//#define MAX_CURRENT           18.8    // max current value, Amps 
#define MAX_CURRENT (MCU_SUPPLY_VOLTAGE / (2 * RSHUNT * AMPLIFICATION_GAIN))

#define MOTOR_FAN   0
#define MOTOR_LJL   1
#define MOTOR_TYPE  MOTOR_FAN

#if (MOTOR_TYPE == MOTOR_LJL)

#define RS              0.428     /* Stator resistance , ohm*/
#define LS              0.0005    /* Stator inductance , H */
#define POLE_PAIR_NUM 7
#define MOTOR_MAX_SPEED_RPM       3000    /*!< rpm, mechanical */
//#define MIN_APPLICATION_SPEED           0 /*!< rpm, mechanical,  */
#define MOTOR_VOLTAGE_CONSTANT    14.33//  Volts RMS ph-ph /kRPM

#elif (MOTOR_TYPE == MOTOR_FAN)

#define RS              0.43//375//16//48// 16//0.075//0.35   /* Stator resistance , ohm*/
#define LS              0.000240//0010//0.000180//0.00158//0.002805//0.00112//0.00155   0.091        /* Stator inductance , H */
#define POLE_PAIR_NUM   2   //4
#define MOTOR_MAX_SPEED_RPM         25000//  4000//16700//4000 /*!< rpm, mechanical */
//#define MIN_APPLICATION_SPEED           0 /*!< rpm, mechanical,  */
#define MOTOR_VOLTAGE_CONSTANT      3.8//2.69//4  //4 Volts RMS ph-ph /kRPM

#endif

#define BUS_ADC_CONV_RATIO  0.008       /* DC bus voltage partitioning ratio*/
#define MAX_VOLTAGE (s16)   69             //((3.3/2)/BUS_ADC_CONV_RATIO)
#define SAMPLING_FREQ       ((uint16_t)(PWM_FREQUENCY / REGULATION_EXECUTION_RATE))
#define MAX_BEMF_VOLTAGE    (u16)((MOTOR_MAX_SPEED_RPM*\
                            MOTOR_VOLTAGE_CONSTANT*SQRT_2)/(1000*SQRT_3))

// Values showed on LCD display must be here multiplied by 10 
#define K1 (s32) (-1500)//(-12000)             /* State Observer Gain 1 */
// Values showed on LCD display must be here multiplied by 100 
#define K2 (s32) (1500)//(+85200)           /* State Observer Gain 2 */

#define Gain1 (s32) (-23930) //(-23200)
#define Gain2 (s32) (27746)  //(13061)
#define PLL_KP_GAIN (s16)249 //1130 //532 //(532*MOTOR_MAX_SPEED_RPM*POLE_PAIR_NUM/SAMPLING_FREQ)  //532

#define PLL_KI_GAIN (s16)3  //50   //12  //(1506742*POLE_PAIR_NUM/SAMPLING_FREQ\

#if (MOTOR_TYPE == MOTOR_LJL) 

#define FREQ_START_UP_DURATION    (u16)200  // 1500 //in msec
#define FINAL_START_UP_SPEED      (u16)600  //1440//2700 //Rotor mechanical speed (rpm)
//Minimum Rotor speed to validate the start-up
#define MINIMUM_SPEED_RPM         (u16) 100//200//580

// With MB459 phase current = (X_I_START_UP * 0.64)/(32767 * Rshunt)
#define FIRST_I_STARTUP           (u16) 1500 //8000
#define FINAL_I_STARTUP           (u16) 1500 //8000
#define I_START_UP_DURATION       (u16) 750 //in msec   开环加速时间
#define I_LOCAL_DURATION          (u16) 250 //150        预定位时间

#elif (MOTOR_TYPE == MOTOR_FAN)  

#define FREQ_START_UP_DURATION    (u16)4400// 1500 //in msec
#define FINAL_START_UP_SPEED      (u16)2400//1440//2700 //Rotor mechanical speed (rpm)
//Minimum Rotor speed to validate the start-up
#define MINIMUM_SPEED_RPM         (u16) 580//200//580

// With MB459 phase current = (X_I_START_UP * 0.64)/(32767 * Rshunt)
#define I_LOCAL_STARTUP           (u16) 1000//  4500->0度 4000->±90度，4.5A左右
#define I_LOCAL_DURATION          (u16) 1250//  1250->0度和-90

#define FIRST_I_STARTUP           (u16) I_LOCAL_STARTUP//8000  2500 8000->55A的峰峰值
#define FINAL_I_STARTUP           (u16) 1000//8000  2500
#define I_START_UP_DURATION       (u16) 750 //in msec

#endif


/******************* START-UP PARAMETERS ***************************************

              Speed /|\
FINAL_START_UP_SPEED |              /
                     |            /
                     |          /          
                     |        /	      
                     |      /          
                     |    / 
                     |  / 
                     |/_______________________________________\                       
                     0          FREQ_START_UP_DURATION      t /               */

/*
                 |I|/|\
                     |
      FINAL_I_STARTUP|       __________________    
                     |     /          
                     |    /	      
                     |   /          
                     |  / 
                     | / 
      FIRST_I_STARTUP|/ 
                     |_______________________________________________\                      
                     0 I_START_UP_DURATION  FREQ_START_UP_DURATION t /        */
    

// Alignment settings 
#ifdef NO_SPEED_SENSORS_ALIGNMENT

//Alignemnt duration
#define SLESS_T_ALIGNMENT           (u16) 700    // Alignment time in ms

#define SLESS_ALIGNMENT_ANGLE       (u16) 90 //Degrees [0..359]  
//  90� <-> Ia = SLESS_I_ALIGNMENT, Ib = Ic =-SLESS_I_ALIGNMENT/2)

// With SLESS_ALIGNMENT_ANGLE equal to 90� final alignment
// phase current = (SLESS_I_ALIGNMENT * 1.65/ Av)/(32767 * Rshunt)  
// being Av the voltage gain between Rshunt and A/D input
#define SLESS_I_ALIGNMENT           (u16) 22000 

#endif

/**************************** STATISTIC PARAMETERS ****************************/
//Threshold for the speed measurement variance.   
#define VARIANCE_THRESHOLD        0.10//.15//0.0625  //Percentage of mean value

// Number of consecutive tests on speed variance to be passed before start-up is
// validated. Checked every PWM period
#define NB_CONSECUTIVE_TESTS      (u16) 60   //ST源码，这个值是2，差别挺大的，看来自己的代码比较严格
// Number of consecutive tests on speed variance before the variable containing
// speed reliability change status. Checked every SPEED_SAMPLING_TIME
#define RELIABILITY_HYSTERESYS    (u8)  3


#define F1 (s16)(16384)//(2048)
#define F2 (s16)(16384)//(8192)

/*************** PI divisor  ***************/
#define SP_KPDIV_LOG LOG2 (16)
#define SP_KIDIV_LOG LOG2 (256)
#define SP_KDDIV_LOG LOG2 (16)
#define TF_KPDIV_LOG LOG2 (1024)
#define TF_KIDIV_LOG LOG2 (16384)
#define TF_KDDIV_LOG LOG2 (8192)
#define FW_KPDIV_LOG LOG2 (32768)
#define FW_KIDIV_LOG LOG2 (32768)
#define PLL_KPDIV     16384
#define PLL_KPDIV_LOG LOG2 (PLL_KPDIV)
#define PLL_KIDIV     65535
#define PLL_KIDIV_LOG LOG2(PLL_KIDIV)
#define F1_LOG LOG2 (4096)
#define F2_LOG LOG2 (4096)
//The parameters below shouldn't be modified
/*max phase voltage, 0-peak Volts*/

#define C1 (s32)((F1*RS)/(LS*SAMPLING_FREQ))
#define C2 (s32)Gain1//((F1*K1)/SAMPLING_FREQ)
#define C3 (s32)((F1*MAX_BEMF_VOLTAGE)/(LS*MAX_CURRENT*SAMPLING_FREQ))
#define C4 (s32)Gain2//((((K2*MAX_CURRENT)/(MAX_BEMF_VOLTAGE))*F2)/(SAMPLING_FREQ))
#define C5 (s32)((F1*MAX_VOLTAGE)/(LS*MAX_CURRENT*SAMPLING_FREQ))
#define C6  (s16)((F2*F3*2*PI)/65536);

#define C6_COMP_CONST1  (int32_t) 1043038
#define C6_COMP_CONST2  (int32_t) 10430
#define MOTOR_MAX_SPEED_DPP (s32)((1.2 * MOTOR_MAX_SPEED_RPM * 65536 * POLE_PAIR_NUM)\
                                                            /(SAMPLING_FREQ * 60))

#define FREQ_STARTUP_PWM_STEPS (u32) ((FREQ_START_UP_DURATION * SAMPLING_FREQ)\
                                                                          /1000) 
#define FREQ_INC (u16) ((FINAL_START_UP_SPEED*POLE_PAIR_NUM*65536/60)\
                                                        /FREQ_STARTUP_PWM_STEPS)
#define I_STARTUP_PWM_STEPS (u32) ((I_START_UP_DURATION * SAMPLING_FREQ)/1000) 

#define I_LOCAL_PWM_STEPS       (u32)((I_LOCAL_DURATION * SAMPLING_FREQ) / 1000)
#define I_INC                   (u16)((FINAL_I_STARTUP - FIRST_I_STARTUP) * 1024 / I_STARTUP_PWM_STEPS)
#define PERCENTAGE_FACTOR       (u16)(VARIANCE_THRESHOLD * 128)      
#define MINIMUM_SPEED           (u16) (MINIMUM_SPEED_RPM / 6)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MC_STATE_OBSERVER_PARAM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
