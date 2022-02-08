/******************************************************************************
* File Name         :  Math.c
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

/* Includes ------------------------------------------------------------------*/
#include "MCInclude.h"

/* Private macro -------------------------------------------------------------*/
#define SIN_COS_TABLE {\
0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,\
0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,\
0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,\
0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,\
0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,\
0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,\
0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,\
0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,\
0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,\
0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,\
0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,\
0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,\
0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,\
0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,\
0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,\
0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,\
0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,\
0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,\
0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,\
0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,\
0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,\
0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,\
0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,\
0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,\
0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,\
0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,\
0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,\
0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,\
0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,\
0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,\
0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,\
0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE}

#define SIN_MASK        0x0300u
#define U0_90           0x0200u
#define U90_180         0x0300u
#define U180_270        0x0000u
#define U270_360        0x0100u
#define divSQRT_3	(int32_t)0x49E6    /* 1/sqrt(3) in q1.15 format=0.5773315*/

/* Private variables ---------------------------------------------------------*/
const int16_t hSin_Cos_Table[256] = SIN_COS_TABLE;


/*******************************************************************************
* Function Name  : MCM_Clarke
* Description    : Clarke变换: Ialpha = Ia   Ibeta = -(2*Ib+Ia)/sqrt(3)
* parameters     : 三相静止坐标中的电流 Ia,Ib
* Return         : 两相静止坐标中的电流 Ialpha Ibeta
*******************************************************************************/
Curr_Components MCM_Clarke(Curr_Components Curr_Input)
{
	Curr_Components Curr_Output;

	int32_t qIa_divSQRT3_tmp, qIb_divSQRT3_tmp ;
	int32_t wIbeta_tmp;
	int16_t hIbeta_tmp;

	/* qIalpha = qIas*/
	Curr_Output.qI_Component1= Curr_Input.qI_Component1;  

	qIa_divSQRT3_tmp = divSQRT_3 * (int32_t)Curr_Input.qI_Component1;

	qIb_divSQRT3_tmp = divSQRT_3 * (int32_t)Curr_Input.qI_Component2;

	/*qIbeta = -(2*qIbs+qIas)/sqrt(3)*/   

	wIbeta_tmp = (-(qIa_divSQRT3_tmp)-(qIb_divSQRT3_tmp)-(qIb_divSQRT3_tmp))>> 15;

	/* Check saturation of Ibeta */
	if (wIbeta_tmp > S16_MAX)
	{
		hIbeta_tmp = S16_MAX;
	}
	else if (wIbeta_tmp < (-32767))
	{
		hIbeta_tmp = (-32767);
	}
	else
	{
		hIbeta_tmp = (int16_t)(wIbeta_tmp);
	}

	Curr_Output.qI_Component2 = hIbeta_tmp;

	return(Curr_Output); 
}


/*******************************************************************************
* Function Name  : MCM_Park
* Description    : Park变换: Id= Ialpha *sin(theta)+qIbeta *cos(Theta)   
*                :           Iq=qIalpha *cos(Theta)-qIbeta *sin(Theta)
* parameters     : 两相静止坐标中的电流 Ialpha Ibeta   Theta 旋转角度
* Return         : 两相旋转坐标中的电流 Id Iq
*******************************************************************************/
Curr_Components MCM_Park(Curr_Components Curr_Input, int16_t Theta)
{
	Curr_Components Curr_Output;
	int32_t qId_tmp_1, qId_tmp_2, qIq_tmp_1, qIq_tmp_2;     
	Trig_Components Local_Vector_Components;
	int32_t wIqd_tmp;
	int16_t hIqd_tmp;

	Local_Vector_Components = MCM_Trig_Functions(Theta);

	/*No overflow guaranteed*/
	qIq_tmp_1 = Curr_Input.qI_Component1 * (int32_t)Local_Vector_Components.hCos;

	/*No overflow guaranteed*/
	qIq_tmp_2 = Curr_Input.qI_Component2 * (int32_t)Local_Vector_Components.hSin;

	/*Iq component in Q1.15 Format */


	wIqd_tmp = (qIq_tmp_1-qIq_tmp_2)>>15;


	/* Check saturation of Iq */
	if (wIqd_tmp > S16_MAX)
	{
		hIqd_tmp = S16_MAX;
	}
	else if (wIqd_tmp < (-32767))
	{
		hIqd_tmp = (-32767);
	}
	else
	{
		hIqd_tmp = (int16_t)(wIqd_tmp);
	}

	Curr_Output.qI_Component1 = hIqd_tmp;

	/*No overflow guaranteed*/
	qId_tmp_1 = Curr_Input.qI_Component1 * (int32_t)Local_Vector_Components.hSin;

	/*No overflow guaranteed*/
	qId_tmp_2 = Curr_Input.qI_Component2 * (int32_t)Local_Vector_Components.hCos;

	/*Id component in Q1.15 Format */

	wIqd_tmp = (qId_tmp_1+qId_tmp_2) >>15;


	/* Check saturation of Id */
	if (wIqd_tmp > S16_MAX)
	{
		hIqd_tmp = S16_MAX;
	}
	else if (wIqd_tmp < (-32767))
	{
		hIqd_tmp = (-32767);
	}
	else
	{
		hIqd_tmp = (int16_t)(wIqd_tmp);
	}

	Curr_Output.qI_Component2 = hIqd_tmp;

	return (Curr_Output);
}


/*******************************************************************************
* Function Name  : MCM_Rev_Park
* Description    : 反Park变换: Valfa= Vq*Cos(theta)+ Vd*Sin(theta)  
*                :             Vbeta=-Vq*Sin(theta)+ Vd*Cos(theta)
* parameters     : 两相旋转坐标中的电压 Vq Vd   Theta 旋转角度
* Return         : 两相静止坐标中的电压 Valfa Vbeta
*******************************************************************************/
Volt_Components MCM_Rev_Park(Volt_Components Volt_Input, int16_t Theta)
{ 	
	int32_t qValpha_tmp1,qValpha_tmp2,qVbeta_tmp1,qVbeta_tmp2;
	Trig_Components Local_Vector_Components;
	Volt_Components Volt_Output;

	Local_Vector_Components = MCM_Trig_Functions(Theta);

	/*No overflow guaranteed*/
	qValpha_tmp1 = Volt_Input.qV_Component1 * (int32_t)Local_Vector_Components.hCos;  
	qValpha_tmp2 = Volt_Input.qV_Component2 * (int32_t)Local_Vector_Components.hSin;

	Volt_Output.qV_Component1 = (int16_t)(((qValpha_tmp1)+(qValpha_tmp2))>>15);


	qVbeta_tmp1 = Volt_Input.qV_Component1 * (int32_t)Local_Vector_Components.hSin;
	qVbeta_tmp2 = Volt_Input.qV_Component2 * (int32_t)Local_Vector_Components.hCos;

	Volt_Output.qV_Component2 = (int16_t)((qVbeta_tmp2-qVbeta_tmp1) >>15);

	return(Volt_Output);
}


/*******************************************************************************
* Function Name  : MCM_Trig_Functions
* Description    : 三角函数变换: Sin(angle)         Cos(angle)
* parameters     : hAngle 三角变换需要输入的角度
* Return         : 转换后的三角函数值
*******************************************************************************/
Trig_Components MCM_Trig_Functions(int16_t hAngle)
{
	int32_t shindex;
	uint16_t uhindex;

	Trig_Components Local_Components;

	/* 10 bit index computation  */  
	shindex =((int32_t)32768 + (int32_t)hAngle); 
	uhindex = (uint16_t)shindex;
	uhindex /= (uint16_t)64;      


	switch ((uint16_t)(uhindex) & SIN_MASK) 
	{
		case U0_90:
		Local_Components.hSin = hSin_Cos_Table[(uint8_t)(uhindex)];
		Local_Components.hCos = hSin_Cos_Table[(uint8_t)(0xFFu-(uint8_t)(uhindex))];
		break;

		case U90_180:  
		 Local_Components.hSin = hSin_Cos_Table[(uint8_t)(0xFFu-(uint8_t)(uhindex))];
		 Local_Components.hCos = -hSin_Cos_Table[(uint8_t)(uhindex)];
		break;

		case U180_270:
		 Local_Components.hSin = -hSin_Cos_Table[(uint8_t)(uhindex)];
		 Local_Components.hCos = -hSin_Cos_Table[(uint8_t)(0xFFu-(uint8_t)(uhindex))];
		break;

		case U270_360:
		 Local_Components.hSin =  -hSin_Cos_Table[(uint8_t)(0xFFu-(uint8_t)(uhindex))];
		 Local_Components.hCos =  hSin_Cos_Table[(uint8_t)(uhindex)]; 
		break;
		
		default:
		break;
	}
	return (Local_Components);
}


/*******************************************************************************
* Function Name  : MCM_Sqrt
* Description    : 开平方根
* parameters     : 一个无符号的32位变量
* Return         : 输入量的平方根
*******************************************************************************/
int32_t MCM_Sqrt(int32_t wInput)
{
  uint8_t biter = 0u;
  int32_t wtemproot;
  int32_t wtemprootnew;

  if (wInput > 0)
  {
    
    if (wInput <= (int32_t)2097152)
    {
      wtemproot = (int32_t)128;
    }
    else
    {
      wtemproot = (int32_t)8192;
    }
    
    do
    {
      wtemprootnew = (wtemproot + wInput/wtemproot)/(int32_t)2;
      if (wtemprootnew == wtemproot)
      {
        biter = 6u;
      }
      else
      {
        biter ++;
        wtemproot = wtemprootnew;
      }
    }
    while (biter < 6u);
  }
  else
  {
    wtemprootnew = (int32_t)0;
  }
  
  return (wtemprootnew); 
}

/*******************************************************************************
* Function Name  : MCM_floatToIntBit
* Description    : 该函数将浮点数转换为相对的32位整数
* parameters     : 浮点数
* Return         : 32位整数
*******************************************************************************/
uint32_t MCM_floatToIntBit(float x)
{
  uint32_t *pInt;
  pInt = (uint32_t*)(&x);
  return *pInt;
}


/*******************************************************************************
* Function Name  : MCM_Arctan2
* Description    : 反正切函数
* parameters     : x轴分量，y轴分量
* Return         : 角度
*******************************************************************************/
s16 MCM_Arctan2(s32 y1,s32 x1)
{
	s16 a;
	s32 ay,ax;//s16//161217-1
	u16 x;
	s16 result;
	u16 sw;
	
	sw=0;
	ay=ABS(y1);
	ax=ABS(x1);
	
	//161217-1
	if(ay > 32767)
	{
		ay = 32767;
	}
	
	if(ax > 32767)
	{
		ax = 32767;
	}		
	//161217-1
	
	if(ay>ax)
	{
		a=ay;
		ay=ax;
		ax=a;
		sw=1;
	}
	
	if(ax==0)
	{
		return(0);
	}
	else
	{
		x=(((s32)ay<<15)/ax); 
		 
		if(x==0x8000)
			result=0x2000;
		else
		{
			a=(((s32)0x118b<<13)-(s32)x*0x96)>>13;
			a=(((s32)-0x4306<<14)+(s32)a*x)>>16;
			a=(((s32)0x364<<12)+(s32)a*x)>>15;
			a=(((s32)0x5179<<14)+(s32)a*x)>>15;
			result=((s32)a*x)>>15;
		}
		
		if(sw==1)
			result=0x4000-result;
		
		if(x1>=0)
		{
			if(y1<0)     //第4相限
			result=-result;
		}
		else
		{
			if(y1>=0)   //第2相限
				result=0x8000-result;
			else
				result=-(0x8000-result);
		}
		
		return(result);    
	}
}

/**************************************************************END OF FILE****/
