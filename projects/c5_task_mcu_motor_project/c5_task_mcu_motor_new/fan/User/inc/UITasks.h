/******************************************************************************
* File Name         :  UITasks.h
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  14/02/27
* Description       :  串口调试界面                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/04/04      1.0               创建   
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
#ifndef __UITASKS_H
#define __UITASKS_H


//#define UI_DEBUG




#ifdef UI_DEBUG

/* Includes ------------------------------------------------------------------*/
#include "Type.h"
#include "MCTasks.h"
#include "HallSensor.h"



typedef struct {
	u8 Buf[4][1024];
	u8 RecvFlag;
	u8 ReFillCnt;
	u16 HeadPoint;
	u16 TailPoint;
	volatile u16 Count;
}UiChannel_t;





extern u8 UI_HallThetaShift;


u8 UI_FillChannelPoolWithData(u8 data1,u8 data2,u8 data3,u8 data4);
void UI_InitChannelPool(void);
void UI_DrawWaveformView(void);
void UI_WindowsWithTimer1(void);
void UI_WindowsWithTimer8(void);

#endif



#endif 

/**************************************************************END OF FILE****/
