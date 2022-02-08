/******************************************************************************
* File Name         :  Math.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  FOC变换过程中所需的数学运算函数                        
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_MATH_H
#define __MC_MATH_H

/* Includes ------------------------------------------------------------------*/
#include "Type.h"





#define LOG2(x) \
((x) == 65535 ? 16 : \
((x) == 2*2*2*2*2*2*2*2*2*2*2*2*2*2*2 ? 15 : \
((x) == 2*2*2*2*2*2*2*2*2*2*2*2*2*2 ? 14 : \
((x) == 2*2*2*2*2*2*2*2*2*2*2*2*2 ? 13 : \
((x) == 2*2*2*2*2*2*2*2*2*2*2*2 ? 12 : \
((x) == 2*2*2*2*2*2*2*2*2*2*2 ? 11 : \
((x) == 2*2*2*2*2*2*2*2*2*2 ? 10 : \
((x) == 2*2*2*2*2*2*2*2*2 ? 9 : \
((x) == 2*2*2*2*2*2*2*2 ? 8 : \
((x) == 2*2*2*2*2*2*2 ? 7 : \
((x) == 2*2*2*2*2*2 ? 6 : \
((x) == 2*2*2*2*2 ? 5 : \
((x) == 2*2*2*2 ? 4 : \
((x) == 2*2*2 ? 3 : \
((x) == 2*2 ? 2 : \
((x) == 2 ? 1 : \
((x) == 1 ? 0 : -1)))))))))))))))))

#define IS_THET_30     (int16_t)(5461)
#define IS_THET_60     (int16_t)(10922)
#define IS_THET_90     (int16_t)(16384)	
#define IS_THET_120    (int16_t)(21845)
#define IS_THET_150    (int16_t)(27306)
#define IS_THET_180    (int16_t)(32767)
#define IS_THET_210    (int16_t)(38229)
#define IS_THET_240    (int16_t)(-21846)//(43690)
#define IS_THET_270    (int16_t)(-16385)//(49151)
#define IS_THET_300    (int16_t)(-10924)//(54612)
#define IS_THET_330    (int16_t)(-5462)//(60074)
#define IS_THET_360    (int16_t)(-1)//(65535)

  typedef struct
  {
    int16_t hCos;
    int16_t hSin;
  } Trig_Components;



Curr_Components MCM_Clarke(Curr_Components Curr_Input);

Curr_Components MCM_Park(Curr_Components Curr_Input, int16_t Theta);

Volt_Components MCM_Rev_Park(Volt_Components Volt_Input, int16_t Theta);

Trig_Components MCM_Trig_Functions(int16_t hAngle);

int32_t MCM_Sqrt(int32_t wInput);

uint32_t MCM_floatToIntBit(float x);

s16 MCM_Arctan2(s32 y1,s32 x1);


#endif /*__MC_MATH_H*/

/**************************************************************END OF FILE****/
