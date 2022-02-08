
#include "stm32f10x_tim.h"
#include "math.h"
#include "stm32f10x.h"
#include "Type.h"


//#define BIT0 0X0001
//#define BIT1 0X0002
//#define BIT2 0X0004
//#define BIT3 0X0008
//#define BIT4 0X0010
//#define BIT5 0X0020
//#define BIT6 0X0040
//#define BIT7 0X0080
//
//#define BIT8 0X0100
//#define BIT9 0X0200
//#define BIT10 0X0400
//#define BIT11 0X0800
//#define BIT12 0X1000
//#define BIT13 0X2000
//#define BIT14 0X4000
//#define BIT15 0X8000

#define CH1_PWM   BIT6|BIT5 //PWM1模式
#define CH1_INACT  BIT6  //强制变为低电平模式
#define CH1_ACT  BIT6|BIT4 //强制变为高电平模式

#define CH2_PWM   BIT14|BIT13
#define CH2_INACT  BIT14
#define CH2_ACT  BIT14|BIT12

#define CH3_PWM   BIT6|BIT5
#define CH3_INACT  BIT6
#define CH3_ACT  BIT6|BIT4

#define CH1_HON   BIT0  //上桥使能
#define CH1_LON   BIT2  //下桥使能

#define CH2_HON   BIT4
#define CH2_LON   BIT6

#define CH3_HON   BIT8
#define CH3_LON   BIT10

#define CCMR1_MASK   (BIT15|BIT11|BIT10|BIT9|BIT8|BIT7|BIT3|BIT2|BIT1|BIT0)
#define CCMR2_MASK   (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT3|BIT2|BIT1|BIT0)
#define CCER_MASK    (BIT13|BIT12|BIT11|BIT9|BIT7|BIT5|BIT3|BIT1)

int MOTOR_PWM[] =
{
  (CH1_PWM|CH2_INACT),(CH3_INACT),(CH1_HON|CH1_LON|CH2_HON|CH2_LON|CH3_LON),//AB
  (CH1_PWM|CH2_INACT),(CH3_INACT),(CH1_HON|CH1_LON|CH2_LON|CH3_HON|CH3_LON ),//AC
  (CH1_INACT|CH2_PWM),(CH3_INACT),(CH1_LON|CH2_HON|CH2_LON|CH3_LON|CH3_HON),//BC
  (CH1_INACT|CH2_PWM),(CH3_INACT),(CH1_HON|CH1_LON|CH2_HON|CH2_LON|CH3_LON),//BA
  (CH1_INACT|CH2_INACT),(CH3_PWM),(CH1_HON|CH1_LON|CH2_LON|CH3_HON|CH3_LON),//CA
  (CH1_INACT|CH2_INACT),(CH3_PWM),(CH1_LON|CH2_HON|CH2_LON|CH3_HON|CH3_LON),//CB
};


void ChangePhase(uint8_t Hall_state,int16_t L_PWM,TIM_TypeDef* TIMx)
{
#if 0//HALLMATCH5，旧结构   //茂田电机
    switch(Hall_state)
    {
       case 4:     //6

       TIM_SetCompare1(TIMx,L_PWM);//AB

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
       TIMx->CCER =  (TIMx->CCER & CCER_MASK)  | MOTOR_PWM[2];
       break;
       case 5:  //4

       TIM_SetCompare1(TIMx,L_PWM);//AC

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[5];
       break;
       case 1:  //5

       TIM_SetCompare2(TIMx,L_PWM);//BC

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[8];
       break;

       case 3://1

       TIM_SetCompare2(TIMx,L_PWM); //BA

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[11];
       break;

       case 2: //3

        TIM_SetCompare3(TIMx,L_PWM); //CA

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[14];
      break;

      case 6:  //2

       TIM_SetCompare3(TIMx,L_PWM);//CB

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[17];
       break;

       default:

       break;
     }
#else//HALLMATCH5，XC新结构   //茂田电机
    switch(Hall_state)
    {
       case 5:     //6

       TIM_SetCompare1(TIMx,L_PWM);//AB

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
       TIMx->CCER =  (TIMx->CCER & CCER_MASK)  | MOTOR_PWM[2];
       break;
       case 4:  //4

       TIM_SetCompare1(TIMx,L_PWM);//AC

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[5];
       break;
       case 6:  //5

       TIM_SetCompare2(TIMx,L_PWM);//BC

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[8];
       break;

       case 2://1

       TIM_SetCompare2(TIMx,L_PWM); //BA

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[11];
       break;

       case 3: //3

        TIM_SetCompare3(TIMx,L_PWM); //CA

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[14];
      break;

      case 1:  //2

       TIM_SetCompare3(TIMx,L_PWM);//CB

       TIMx->CCMR1 = (TIMx->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
       TIMx->CCMR2 = (TIMx->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
       TIMx->CCER = (TIMx->CCER & CCER_MASK) | MOTOR_PWM[17];
       break;

       default:

       break;
     }
#endif
}



//void L_MOTOR_PWM(void)
//{
//
//  switch(motor2_type) //左电机类型
//   {
//	 case 6:   //捷安达（轮毂电机）
//
//	 switch(Hall_state_L_temp)
//	 {
//		case 1:     //6
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//
//		break;
//		case 3:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//
//		break;
//		case 2:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 6://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 4: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//
//	    break;
//
//	   case 5:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//
//	    break;
//
//		default:
//
//		break;
//	  }
//
//	  break;
//
//	  case 1:   //	联谊
//
//	 switch(Hall_state_L_temp)
//	 {
//		case 6:     //6
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//
//		case 2:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 3:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 1://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 5: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//
//	    break;
//
//	   case 4:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//
//	    break;
//
//		default:
//
//		break;
//	  }
//
//	  break;
//
//	  case 2:  // 康尼
//
//	 switch(Hall_state_L_temp)
//	 {
//		case 5:     //6
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//
//		case 4:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//
//		case 6:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[8];
//
//		break;
//
//		case 2://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//
//	    break;
//
//		case 3: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//	    break;
//
//	   case 1:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//	    break;
//
//		default:
//
//		break;
//	  }
//
//	  break;
//
//	  case 3:   //茂田 中大
//
//	 switch(Hall_state_L_temp)
//	 {
//		case 4:     //6
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 5:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 1:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 3://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 2: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//	   case 6:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//	    break;
//
//		default:
//
//		break;
//	  }
//
//	  break;
//
//	 case 4:   //平方
//
//	 switch(Hall_state_L_temp)
//	 {
//		case 4:     //6
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 6:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 2:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[8];
//		break;
//
//		case 3://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 1: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//	   case 5:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//	   break;
//
//		default:
//
//		break;
//	  }
//
//	  break;
//
//      case 5:   //硕阳
//
//      switch(Hall_state_L_temp)
//	  {
//
//		case 5:
//
//        TIM_SetCompare1(TIM1,L_PWM);//AB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM1->CCER =  (TIM1->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 1:  //4
//
//		TIM_SetCompare1(TIM1,L_PWM);//AC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 3:  //5
//
//		TIM_SetCompare2(TIM1,L_PWM);//BC
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 2://1
//
//		TIM_SetCompare2(TIM1,L_PWM); //BA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 6: //3
//
//		 TIM_SetCompare3(TIM1,L_PWM); //CA
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//	   case 4:  //2
//
//		 TIM_SetCompare3(TIM1,L_PWM);//CB
//
//		TIM1->CCMR1 = (TIM1->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM1->CCMR2 = (TIM1->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM1->CCER = (TIM1->CCER & CCER_MASK) | MOTOR_PWM[17];
//	   break;
//
//		default:
//		break;
//	 }
//        break;
//
//	    default:
//		break;
//    }
//}
//
//void R_MOTOR_PWM(void)
//{
//
//	switch(motor_type) //右电机类型
//	{
//      case 6:   //捷安达
//
//      switch(Hall_state_R_temp)
//	  {
//
//		case 1:
//
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 3:
//
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 2:
//
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 6:
//
//		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = ( TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 4:
//
//		 TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//		case 5:
//
//		 TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//	   break;
//
//		default:
//		break;
//	 }
//
//      break;
//
//	  case 1: //	联谊  熊大 吉庆
//
//      switch(Hall_state_R_temp)
//	  {
//
//		case 1:     //6
//
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 5:  //4
//
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 4:  //5
//
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 6://1
//
//		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 2: //3
//
//		 TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//		case 3:  //2
//
//		 TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//	   break;
//
//		default:
//		break;
//	 }
//	  break;
//
//	  case 2: //康尼
//
//     switch(Hall_state_R_temp)
//	  {
//
//		case 5:     //6
//
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 4:  //4
//
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 6:  //5
//
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 2://1
//
//		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 3: //3
//
//		 TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//		case 1:  //2
//
//		 TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//	   break;
//
//		default:
//		break;
//	 }
//
//	  break;
//
//	  case 3:  //茂田 中大
//
//      switch(Hall_state_R_temp)
//	  {
//
//		case 4:     //6
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 =  (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK )| MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 5:  //4
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = ( TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//
//		break;
//		case 1:  //5
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = ( TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//
//		break;
//
//		case 3://1
// 		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//
//	    break;
//
//		case 2: //3
//		TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//
//	   break;
//
//		case 6:  //2
// 		TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//
//	   break;
//
//		default:
//		break;
//	 }
//
//	  break;
//
//      case 4:   //平方
//
//      switch(Hall_state_R_temp)
//	  {
//
//		case 4:
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//
//		break;
//		case 6:
//
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 2:
//
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 3:
//
//		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = ( TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = ( TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 1:
//
//		TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//	    break;
//
//		case 5:
//
//		TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = ( TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//	    break;
//
//		default:
//		break;
//	 }
//
//      break;
//
//      case 5:   //硕阳
//
//      switch(Hall_state_R_temp)
//	  {
//
//		case 5:
//
//        TIM_SetCompare1(TIM8,R_PWM);//AB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[0];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[1];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK)  | MOTOR_PWM[2];
//		break;
//		case 1:
//
//		TIM_SetCompare1(TIM8,R_PWM);//AC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[3];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[4];
//		TIM8->CCER =  (TIM8->CCER & CCER_MASK) | MOTOR_PWM[5];
//		break;
//		case 3:
//
//		TIM_SetCompare2(TIM8,R_PWM);//BC
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[6];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[7];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[8];
//		break;
//
//		case 2:
//
//		TIM_SetCompare2(TIM8,R_PWM); //BA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[9];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[10];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[11];
//	    break;
//
//		case 6:
//
//		 TIM_SetCompare3(TIM8,R_PWM); //CA
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[12];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[13];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[14];
//	   break;
//
//		case 4:
//
//		 TIM_SetCompare3(TIM8,R_PWM);//CB
//
//		TIM8->CCMR1 = (TIM8->CCMR1 & CCMR1_MASK) | MOTOR_PWM[15];
//		TIM8->CCMR2 = (TIM8->CCMR2 & CCMR2_MASK) | MOTOR_PWM[16];
//		TIM8->CCER = (TIM8->CCER & CCER_MASK) | MOTOR_PWM[17];
//	    break;
//
//		default:
//		break;
//	 }
//
//      break;
//
//		default:
//		break;
//   }
//}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
