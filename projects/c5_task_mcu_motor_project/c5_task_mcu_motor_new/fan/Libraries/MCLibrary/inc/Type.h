/******************************************************************************
* File Name         :  Type.h
* Author            :  ���ŷ� 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  ���ͼ����ֳ��������ļ�                       
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* ���ŷ�             19/01/14      1.0               ����   
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPE_H
#define __TYPE_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


#define SINGLE_DRIVER 
//#define DOUBLE_DRIVER 


#define SPEED_MODE 
#define Sensorless
#define JlinkSCope

#define OBSERVER_GAIN_TUNING
typedef enum {FALSE,TRUE} bool;



#define HALLMATCH1 1
#define HALLMATCH2 2
#define HALLMATCH3 3
#define HALLMATCH4 4
#define HALLMATCH5 5
#define HALLMATCH6 6

#define PHASEMATCH1 1
#define PHASEMATCH2 2
#define PHASEMATCH3 3
#define PHASEMATCH4 4
#define PHASEMATCH5 5
#define PHASEMATCH6 6

//#define LowEffect
//-----------------LINEAR_HALL//ƥ�����Ի������ʱ�������º궨��//
#define HALLMATCH HALLMATCH1    //�д�������ٺ�˶�� HALLMATCH5 PHASEMATCH2(������)  ����,���д�һ������HALLMATCH6 PHASEMATCH2(������) ���д��������  �»���(��ڻ�,�׺��)HALLMATCH6 PHASEMATCH1   �д�������ٴ�ת����(�׻ƺ�)(HALLMATCH5 PHASEMATCH2 M1 -20390  M2 10100)
#define PHASEMATCH PHASEMATCH1  // ��������(������)һ������ HALLMATCH4  PHASEMATCH2
#define HALLMATCHS HALLMATCH1 

//#define HALLMATCH HALLMATCH6    //�д�������ٺ�˶�� HALLMATCH5 PHASEMATCH2(������)  ����,���д�һ������HALLMATCH6 PHASEMATCH2(������) ���д��������  �»���(��ڻ�,�׺��)HALLMATCH6 PHASEMATCH1   �д�������ٴ�ת����(�׻ƺ�)(HALLMATCH5 PHASEMATCH2 M1 -20390  M2 10100)
//#define PHASEMATCH PHASEMATCH1  // ��������(������)һ������ HALLMATCH4  PHASEMATCH2
//#define HALLMATCHS HALLMATCH6  // nidec,whill��� HALLMATCH5 PHASEMATCH1(�����)   ������������HALLMATCH6 PHASEMATCH2(������) (M1 -9200, M2 18300) nidecȫ��E����  HALLMATCH6 PHASEMATCH1(��׺�)  HALLMATCH5 PHASEMATCH1 �ݺ͵��(�׺��,����)
#define PHASE_SHIFT_M1 (int16_t)(-9200)//-9400�����Ƕ�ƫ��   -9000 �д����(-9200)  �д����2(-8330 ,-8430)                             �д����3(-9250,-8700) �д����1(-9300����ӷ�������)  �������д�(-8550)  23767 ˶��   23367(��������) -9650(�»��� )  �д�һ��(-14500) ���з��д���(-12150)
#define PHASE_SHIFT_M2 (int16_t)(18300)//20500�����Ƕ�ƫ��   19500 �д����(20200)  �д����2(-9320  -7320����ӷ��̻���)   �д����3(20300, 19920) �д����1(20420)               �������д�(20350)             53267(��������)      22000(�»���)      �д�һ��(21650)  ���з��д���(17350)
//-----------------LINEAR_HALL

#ifdef LINEAR_HALL//190822,LinearHall   //-------------GPIO����
#define PRECHARGE_GPIO_Pin GPIO_Pin_2  //�ɰ� GPIO_Pin_6  �°� GPIO_Pin_2
#define PRECHARGE_GPIO_Port GPIOB
#define RELAY_GPIO_Pin GPIO_Pin_7
#define RELAY_GPIO_Port GPIOB
#else
#define PRECHARGE_GPIO_Pin GPIO_Pin_2   // 
#define PRECHARGE_GPIO_Port GPIOB
#define RELAY_GPIO_Pin GPIO_Pin_5
#define RELAY_GPIO_Port GPIOB
#endif

#define SWVERSION "0.0.7"   

//#define HALLMATCH HALLMATCH5    //XC��������ï������ת�Ӻ���ͷ//������룬���÷���
//#define PHASEMATCH PHASEMATCH2
//#define HALLMATCHS HALLMATCH5
//#define PHASE_SHIFT_M1 (int16_t)(29600)//�����Ƕ�ƫ��
//#define PHASE_SHIFT_M2 (int16_t)(-31500)//�����Ƕ�ƫ��
////-------------------���º궨�壬���������ã�����������------------------//
#define MOTOR_DIR_IS_CW_M1//����
//#define MOTOR_DIR_IS_CW_M2//����


//#define HALLMATCH HALLMATCH4    //XC��������ï������ת�Ӻ���ͷ//������룬���÷���
//#define PHASEMATCH PHASEMATCH2
//#define HALLMATCHS HALLMATCH4

//#define HALLMATCH HALLMATCH3//HALLMATCH5    //�ɰ壬ï�������к���ͷ//29600,-31500//������룬���÷���//NO
//#define PHASEMATCH PHASEMATCH1//PHASEMATCH2
//#define HALLMATCHS HALLMATCH3//HALLMATCH5
//#define PHASE_SHIFT_M1 (int16_t)(29600)//�����Ƕ�ƫ��
//#define PHASE_SHIFT_M2 (int16_t)(-31500)//�����Ƕ�ƫ��
//-------------------���º궨�壬���������ã�����������------------------//
//#define MOTOR_DIR_IS_CW_M1//����
//#define MOTOR_DIR_IS_CW_M2//����


//#define HALLMATCH HALLMATCH5//HALLMATCH5    //�°壬�д��µ�����޺���ͷ
//#define PHASEMATCH PHASEMATCH2//PHASEMATCH2
//#define HALLMATCHS HALLMATCH5//HALLMATCH5
//#define PHASE_SHIFT_M1 (int16_t)(-9000)//�����Ƕ�ƫ��
//#define PHASE_SHIFT_M2 (int16_t)(19500)//�ҵ���Ƕ�ƫ��
//-------------------���º궨�壬���������ã�����������------------------//
//#define MOTOR_DIR_IS_CW_M1//����
//#define MOTOR_DIR_IS_CW_M2//����


//#define HALLMATCH HALLMATCH3//HALLMATCH5    //�ɰ壬ï�������к���ͷ//-21500,-21500//������룬��������
//#define PHASEMATCH PHASEMATCH1//PHASEMATCH2
//#define HALLMATCHS HALLMATCH3//HALLMATCH5


//#define HALLMATCH HALLMATCH5//HALLMATCH5    //�д������к���ͷ//12530,-20200
//#define PHASEMATCH PHASEMATCH2//PHASEMATCH2
//#define HALLMATCHS HALLMATCH5//HALLMATCH5


//#define HALLMATCH HALLMATCH5//HALLMATCH5    //�д��µ�����޺���ͷ//����,-20200,12530
//#define PHASEMATCH PHASEMATCH4//PHASEMATCH2
//#define HALLMATCHS HALLMATCH5//HALLMATCH5


//#define HALLMATCH HALLMATCH6//HALLMATCH5    //�д��µ�����޺���ͷ2
//#define PHASEMATCH PHASEMATCH4//PHASEMATCH2
//#define HALLMATCHS HALLMATCH6//HALLMATCH5



#define EI() __set_PRIMASK(0)
#define DI() __set_PRIMASK(1)

//#define EI() taskEXIT_CRITICAL()
//#define DI() taskENTER_CRITICAL()


//#define GPIO_SetBits(GPIOB,GPIO_Pin_8)         		(GPIOB->BSRR = GPIO_Pin_8)//
//#define GPIO_ResetBits(GPIOB,GPIO_Pin_8)           (GPIOB->BRR = GPIO_Pin_8)
//#define GPO_StaLed_CHG()       (GPIOB->ODR^= GPIO_Pin_8)



#define UART_DEBUG//190702


#define MC_NULL    (uint16_t)(0x0000u)

#ifdef DOUBLE_DRIVER 
#define NBR_OF_MOTORS 2
#else
#define NBR_OF_MOTORS 1
#endif

#define M1      (uint8_t)(0x0)  /*!< Motor 1.*/
#define M2      (uint8_t)(0x1)  /*!< Motor 2.*/
#define M_NONE  (uint8_t)(0xFF) /*!< None motor.*/


#define U8_MAX      ((uint8_t)255)
#define S8_MAX      ((int8_t)127)
#define S8_MIN      ((int8_t)-127)
#define U16_MAX     ((uint16_t)65535u)
#define S16_MAX     ((int16_t)32767)
#define S16_MIN     ((int16_t)-32767)
#define S16_STD_MIN ((int16_t)-32768)
#define U32_MAX     ((uint32_t)4294967295uL)
#define S32_MAX     ((int32_t)2147483647)
#define S32_MIN     ((int32_t)-2147483647)


//#define IQMAX        16000//25322
#define IQMAX               32767  //10000 25000  12000(41A)  14000(48A)  18000(50A����)   14634(50A) 3658  7317
#define SATURATED_TIME      200  //10000(10S)  2000(2s)
#define POWERDOWNLEVEL 		38400  //  45392对应48V；38400对应40.5，手册给的欠压值
//#define  DefTemperatureMax  60  //�����¶��趨���ֵ STM32���ڲ��¶ȴ�����֧�ֵ��¶ȷ�ΧΪ��-40~125��
#define  TEMP_THRESHOLD     5   //�¶���ֵ
#define OVSREF              57300  //��ѹ��ֵ  57300(32V)    53600(30V)    60900(34V)  62800(35V)  50000(28V) 44800(25V) 48200(27V) 42900(24V) 35800(20V)  37500(21V)   34600(19.5V)
#define OVSREF2             56740 //60/48*45392=56740,60V是电池手册提供参考值
/* �ɰ忨
#define OVSREF     62660  //��ѹ��ֵ  59510(29V)    61562(30V)    65536(31.9V)  51302(25V)  55406(27V) 63615(31V) 57458(28V) 62660(30.5V)
#define OVSREF2     64714 //64714(31.5V)
*/
#define VSREF   	14600 //34600(19.5V) �°忨 Ƿѹ��ֵ 

#define BIT0 0x0001
#define BIT1 0x0002
#define BIT2 0x0004
#define BIT3 0x0008
#define BIT4 0x0010
#define BIT5 0x0020
#define BIT6 0x0040
#define BIT7 0x0080
#define BIT8 0x0100
#define BIT9 0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000


#define GetMaxVal(a,b,c) (a>b?(a>c?a:c):(b>c?b:c))
#define GetMinVal(a,b,c) (a<b?(c<a?c:a):(c<b?c:b))
#define ABS(x)            (((x)>=0)?(x):(-(x)))



#define PI              3.1416

#define SQRT_2  1.4142
#define SQRT_3  1.732051

#define MAGNETICBRAKE_DELAY_LOCKING_TIME     10  //1200  35
#define EabsStop_DELAY_TIME                  10  //1200  35
#define DELAY_LOCKING_TIME                   600 //10s

#define can_err_step_cnt  10  //CANͨ�Ŵ���ʱ�¿ظ����Ĳ���ֵ 2019 jimianhao

#ifdef LINEAR_HALL
#define unlock_spd_ref_max  7500  //3500  5500  4500
#define forward_max_spd   22000   //7km/h(22800)   6.5km/h(21000) 6.2km/h(20315)  7.5km/h(24500) 
#define backward_max_spd  11500   //3.5km/h(9000)    
#define ManualTrolleyMaxSpd3km    11000 // 3km/h(10000)   
#else
#define unlock_spd_ref_max  7500  //3500  5500  4500
#define forward_max_spd   25000   //7km/h(22800)   6.5km/h(21000) 6.2km/h(20315)  7.5km/h(24500) 
#define backward_max_spd  15496   //3.5km/h(15496)    3.75km/h(16603)
#define ManualTrolleyMaxSpd3km    15000 // 3km/h(13000)   3.5km/h(15000) 
#endif

#define  BREAK_PWM_Duty_Cycle  40 //40  12  ���ɲ��PWMռ�ձ�(60%)
#define  BREAK_PWM_Count  50 //50  20  ���ɲ��PWM��������(20ms)
#define  BREAK_PWM_Period  3600 //���ɲ������Ƶ�� 3200(20K)   6400(10K)  
#define  MagneticBrakeStaCheckMaxNum 600 //2000(2s)  1000(1s)

#define M1_spd_rate   100
#define M2_spd_rate   100    //�ٶȱ�������ֵ 92        96 100  102
#define spd0_ref_time  3000  //3000  2000

#define CH1_PWM   BIT6|BIT5 //PWM1ģʽ
#define CH1_INACT  BIT6  //ǿ�Ʊ�Ϊ�͵�ƽģʽ
#define CH1_ACT  BIT6|BIT4 //ǿ�Ʊ�Ϊ�ߵ�ƽģʽ

#define CH2_PWM   BIT14|BIT13
#define CH2_INACT  BIT14
#define CH2_ACT  BIT14|BIT12

#define CH3_PWM   BIT6|BIT5
#define CH3_INACT  BIT6
#define CH3_ACT  BIT6|BIT4

#define CH1_HON   BIT0  //����ʹ��
#define CH1_LON   BIT2  //����ʹ��

#define CH2_HON   BIT4
#define CH2_LON   BIT6

#define CH3_HON   BIT8
#define CH3_LON   BIT10

#define CCMR1_MASK   (BIT15|BIT11|BIT10|BIT9|BIT8|BIT7|BIT3|BIT2|BIT1|BIT0)
#define CCMR2_MASK   (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT3|BIT2|BIT1|BIT0)
#define CCER_MASK    (BIT13|BIT12|BIT11|BIT9|BIT7|BIT5|BIT3|BIT1)

typedef struct 
{
  uint8_t bStatus;
  uint8_t bFail;
  uint8_t bCommand;
  uint16_t hCounter;
  uint8_t bFail_lv;
  uint8_t bReady;
  uint8_t bMosCheck;
} Relay_t;



extern uint16_t hVs;
extern uint16_t hVrelay;
extern uint8_t PowerEnableFlag;
extern uint8_t bMcDriverErrFlag;
extern uint8_t bThermalShutdownFlag;
extern  int16_t hMCUTemp1;
extern  int16_t hMCUTemp2;

typedef struct 
{
  int16_t qI_Component1;
  int16_t qI_Component2;
} Curr_Components;



typedef struct 
{
  int16_t qV_Component1;
  int16_t qV_Component2;
} Volt_Components;



typedef struct
{
    Curr_Components Iab;         /*!< Stator current on stator reference frame abc */
    Curr_Components Ialphabeta;  /*!< Stator current on stator reference frame 
                                    alfa-beta*/
    Curr_Components IqdHF;       /*!< Stator current on stator reference frame 
                                    alfa-beta*/                                     
    Curr_Components Iqd;         /*!< Stator current on rotor reference frame qd */ 
    Curr_Components Iqdref;      /*!< Stator current on rotor reference frame qd */ 
    Volt_Components Vqd;         /*!< Phase voltage on rotor reference frame qd */ 
    Volt_Components Valphabeta;  /*!< Phase voltage on stator reference frame 
                                   alpha-beta*/ 
    int16_t hTeref;              /*!< Reference torque */ 
    int16_t hElAngle;            /*!< Electrical angle used for reference frame 
                                    transformation  */
                                        
    uint16_t hCodeError;         /*!< error message */
    
	uint8_t bStartCount;
	
    uint8_t bRunning;	
   
    uint8_t bBlockageCurrent;

	uint8_t bBlockageStartFlag;
		
	uint8_t bMotorStop;

	uint8_t bMotorEabsStop;
	
	uint8_t bSlopeStaticFlag;
	
	uint8_t bSpdStaticFlag;

	uint8_t bSpdRef0Flage;
	
    uint16_t hRegConv[3];   
    
    uint16_t hPwmA;
    uint16_t hPwmB;
    uint16_t hPwmC;
	uint16_t hPwmD;
	uint16_t hSampPoint;
    int16_t Speed_ref;
	uint16_t hSpdRef0Cnt;
  uint16_t hDollySpdLimit;
	uint8_t bCanMagneticBrakeState;
	uint8_t bRunMagneticBrakeState;
	uint8_t bStopMagneticBrakeState;

	
} FOCVars_t, *pFOCVars_t;

typedef struct
{
 s16 hC1;
 s16 hC2;
 s16 hC3;
 s16 hC4;
 s16 hC5;
 s16 hC6;
 s16 hF1;
 s16 hF2;
 s16 hF3;
 s16 PLL_P;
 s16 PLL_I;
 s32 wMotorMaxSpeed_dpp;
 u16 hPercentageFactor;
} StateObserver_Const;

typedef struct
{
  s16 PLL_P;
  s16 PLL_I;
  s16 hC2;
  s16 hC4;
} StateObserver_GainsUpdate;
typedef enum
{
IDLE, INIT, START, RUN, STOP, BRAKE, WAIT, FAULT
} SystStatus_t;
typedef enum
{
	MOTORSTOP      = 0,            //
	MOTORSTART      = 1,            //	
	MOTOROPEN      = 2,            //
	MOTORSWITCH    =3,
	MOTORLOOP      = 4,            //
	MOTORBRAKE      = 5,            //
	MOTORSLEEP      = 6,            //
}MOTERMODE_TypeDef;			
	#ifdef JlinkSCope
typedef  struct {
        signed int Reveal1;
        signed int Reveal2;
     //   signed int Reveal3;
		//	  signed int Reveal4;
    //    signed int Reveal5;
    //    signed int Reveal6;
	//signed int Reveal7;
      } ValBufferType;

			
extern char JS_RTT_UpBuffer[4096]; // J-Scope RTT Buffer
extern int JS_RTT_Channel;     // J-Scope RTT Channel		
extern ValBufferType acValBuffer;;     // J-Scope RTT Channel				
#endif
typedef enum
{
 S_INIT, ALIGNMENT, RAMP_UP,LOOPRUN
} Start_upStatus_t;
extern volatile int16_t  hForceElAngle;
extern volatile int32_t  hAvrMecSpeed01Hz;
extern volatile int16_t  hForceTorque_Reference;
extern Start_upStatus_t  Start_Up_State;
extern MOTERMODE_TypeDef motormode;	
#define TIMx_UP_PRE_EMPTION_PRIORITY 0u

#define TIMx_UP_SUB_PRIORITY 0u

#define DMAx_TC_PRE_EMPTION_PRIORITY 0u

#define DMAx_TC_SUB_PRIORITY 0u

#define ADC_PRE_EMPTION_PRIORITY 0u

#define ADC_SUB_PRIORITY 0u

#define TIMx_PRE_EMPTION_PRIORITY 1u

#define TIMx_SUB_PRIORITY 0u

#define USART_PRE_EMPTION_PRIORITY 3u

#define USART_SUB_PRIORITY 1u

#define CAN_PRE_EMPTION_PRIORITY 3u

#define CAN_SUB_PRIORITY 1u

#define SYSTICK_PRE_EMPTION_PRIORITY 4u

#define SYSTICK_SUB_PRIORITY 0u

#define TIMx_BRK_PRE_EMPTION_PRIORITY 1u

#define TIMx_BRK_SUB_PRIORITY 0u

#define PENDSV_PRE_EMPTION_PRIORITY 5u

#define PENDSV_SUB_PRIORITY 0u

#define SYSTICK_PRIORITY (((SYSTICK_PRE_EMPTION_PRIORITY & 0x07) << 1) | (SYSTICK_SUB_PRIORITY & 0x01))
#define PENDSV_PRIORITY  (((PENDSV_PRE_EMPTION_PRIORITY & 0x07)  << 1) | (PENDSV_SUB_PRIORITY & 0x01))  

#endif /* __MC_TYPE_H */


/**************************************************************END OF FILE****/
