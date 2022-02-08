/******************************************************************************
* File Name         :  UITasks.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  串口输出图形程序                       
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







#ifdef J_SCOPE

int16_t JS_hElAngleM1;
int16_t JS_hElAngleM2;

int16_t JS_hMeasuredElAngleM1;
int16_t JS_hMeasuredElAngleM2;

int16_t JS_ValphaM1;
int16_t JS_ValphaM2;

int16_t JS_IaM1;
int16_t JS_IaM2;

int16_t JS_IqRefM1;
int16_t JS_IqRefM2;

int16_t JS_VqM1;
int16_t JS_VqM2;

int16_t JS_IqM1;
int16_t JS_IqM2;

int16_t JS_DiffM1;
int16_t JS_DiffM2;


uint8_t JS_HallNewStateM1;
uint8_t JS_HallNewStateM2;

uint16_t JS_HallTimeM1;
uint16_t JS_HallTimeM2;

uint16_t TIM1_TEST;
uint16_t TIM8_TEST;

int32_t M1_vq;
int32_t M2_vq;

int32_t M1_vd;
int32_t M2_vd;
/*******************************************************************************
* Function Name  : JS_WindowsWithTimer1
* Description    : 
* parameters     : None
* Return         : None
*******************************************************************************/
void JS_WindowsWithTimer1(void)
{
//	JS_hElAngleM1 = oHall[M1]->Vars.hElAngle;
	JS_ValphaM1 = FOCVars[M1].Valphabeta.qV_Component1;
	//JS_hMeasuredElAngleM1 = oHall[M1]->Vars.hMeasuredElAngle;
	JS_IaM1 = FOCVars[M1].Iab.qI_Component1;
	JS_VqM1 = FOCVars[M1].Vqd.qV_Component1;
	JS_IqRefM1 = FOCVars[M1].Iqdref.qI_Component1;
	JS_IqM1 = FOCVars[M1].Iqd.qI_Component1;
	//JS_HallNewStateM1 = oHall[M1]->Vars.bHallNewState;
//	JS_HallTimeM1 = oHall[M1]->Vars.hHallTime;
	//JS_DiffM1 = oHall[M1]->Vars.hDiff;
	
	TIM1_TEST = TIM1->CCR1;
	M1_vq= FOCVars[M1].Vqd.qV_Component1;
	M1_vd= FOCVars[M1].Vqd.qV_Component2;
}


/*******************************************************************************
* Function Name  : JS_WindowsWithTimer8
* Description    : 
* parameters     : None
* Return         : None
*******************************************************************************/
void JS_WindowsWithTimer8(void)
{
//	JS_hElAngleM2 = oHall[M2]->Vars.hElAngle;
	JS_ValphaM2 = FOCVars[M2].Valphabeta.qV_Component1;
	//JS_hMeasuredElAngleM2 = oHall[M2]->Vars.hMeasuredElAngle;
	JS_IaM2 = FOCVars[M2].Iab.qI_Component1;
	JS_VqM2 = FOCVars[M2].Vqd.qV_Component1;
	JS_IqRefM2 = FOCVars[M2].Iqdref.qI_Component1;
	JS_IqM2 = FOCVars[M2].Iqd.qI_Component1;
	//JS_HallNewStateM2 = oHall[M2]->Vars.bHallNewState;
//	JS_HallTimeM2 = oHall[M2]->Vars.hHallTime;
	//JS_DiffM2 = oHall[M2]->Vars.hDiff;
	TIM8_TEST= TIM8->CCR1;
	M2_vq= FOCVars[M2].Vqd.qV_Component1;
	M2_vd= FOCVars[M2].Vqd.qV_Component2;
}


#endif


/**************************************************************END OF FILE****/
