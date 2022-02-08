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





#ifdef UI_DEBUG

#define UI_CHANNELPOOL_BUFSIZE 1024


UiChannel_t UI_ChannelPool;

/*******************************************************************************
* Function Name  : UI_InitChannelPool
* Description    : 各通道Pool初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void UI_InitChannelPool(void)
{
	u16 i;
	
	for(i=0; i<UI_CHANNELPOOL_BUFSIZE; i++)
	{
        UI_ChannelPool.Buf[0][i] = 0;
		UI_ChannelPool.Buf[1][i] = 0;
		UI_ChannelPool.Buf[2][i] = 0;
		UI_ChannelPool.Buf[3][i] = 0;
	}
	UI_ChannelPool.HeadPoint=0;
	UI_ChannelPool.TailPoint=0;
	UI_ChannelPool.Count=0;
	UI_ChannelPool.RecvFlag = 1;
	UI_ChannelPool.ReFillCnt = 0;
	
}

/*******************************************************************************
* Function Name  : UI_FillChannelPoolWithData
* Description    : 填充各通道数据
* parameters     : 各通道数据
* Return         : 若Buf数据已满则返回0 否则返回1
*******************************************************************************/
u8 UI_FillChannelPoolWithData(u8 data1,u8 data2,u8 data3,u8 data4)
{
	if((UI_ChannelPool.Count >= UI_CHANNELPOOL_BUFSIZE)||(UI_ChannelPool.RecvFlag==0)) 
	{
		UI_ChannelPool.RecvFlag=0;	
		UI_ChannelPool.ReFillCnt = 4;
		return 0;
	}
		
	UI_ChannelPool.Buf[0][UI_ChannelPool.TailPoint] = data1;
	UI_ChannelPool.Buf[1][UI_ChannelPool.TailPoint] = data2;
	UI_ChannelPool.Buf[2][UI_ChannelPool.TailPoint] = data3;
	UI_ChannelPool.Buf[3][UI_ChannelPool.TailPoint] = data4;
	UI_ChannelPool.TailPoint++;
	
	if (UI_ChannelPool.TailPoint >= UI_CHANNELPOOL_BUFSIZE) 
		UI_ChannelPool.TailPoint = 0;
	UI_ChannelPool.Count++;

	return 1;
}

/*******************************************************************************
* Function Name  : UI_LoadChannelPoolWithData
* Description    : 
* parameters     : channel 导出通道
* Return         : Buf为空返回 0 否则为1
*******************************************************************************/
u8 UI_LoadChannelPoolWithData(u8 channel)
{
	u8 data;
	
	static u8 ChlFlag;

	if(UI_ChannelPool.Count==0)
	{
		UI_ChannelPool.RecvFlag=1;		
		return 0;
	}

	ChlFlag |= (1<<channel);

	data = UI_ChannelPool.Buf[channel][UI_ChannelPool.HeadPoint];
	if((ChlFlag&0x0f)==0x0f)
	{
		UI_ChannelPool.HeadPoint++;
		if (UI_ChannelPool.HeadPoint >= UI_CHANNELPOOL_BUFSIZE) 
			UI_ChannelPool.HeadPoint = 0;
		UI_ChannelPool.Count--;		
		ChlFlag = 0;
		
	}

	return data;
}


/*******************************************************************************
* Function Name  : UI_DrawWaveformView
* Description    : 
* parameters     : None
* Return         : None
*******************************************************************************/
void UI_DrawWaveformView(void)
{

	u8 data=0;
	static volatile u8 step=0;
	static volatile u16 cnt=0;

	if((USART_GetFlagStatus(USART3, USART_FLAG_TXE) != RESET) &&((UI_ChannelPool.RecvFlag==0)||(UI_ChannelPool.ReFillCnt>0)))   
    {  
	
		switch(step)
		{
			case 0 : 
				
				step=1;				
				data = 0x55;
				break;

			case 1 :
				step=2;
				data = 0x5A;							
				break;
				
			case 2 : 
				step=3;
				data = 0xA5;
				break;

			case 3 :
				step=4;
				data = 0xAA;							
				break;
				
			case 4 : 
//				if(UI_ChannelPool.Count>0)
					data = UI_LoadChannelPoolWithData(0);
				step=5;
				break;

			case 5 :
//				if(UI_ChannelPool.Count>0)
					data = UI_LoadChannelPoolWithData(1);
				step=6;				
				break;

			case 6 : 
//				if(UI_ChannelPool.Count>0)
					data = UI_LoadChannelPoolWithData(2);
				step=7;	
				break;

			case 7 :
//				if(UI_ChannelPool.Count>0)
					data = UI_LoadChannelPoolWithData(3);
				if(cnt<1023)
				{
					cnt++;
					step=4;	
				}
				else
				{
					cnt=0;
					step=0;	
				}	
				break;			

			default:
				step=0;
				cnt=0;
				break;

		}
		
		if(UI_ChannelPool.RecvFlag==0)
			USART3->DR = data;
		else if(UI_ChannelPool.ReFillCnt>0)
		{
			USART3->DR = 0;
			UI_ChannelPool.ReFillCnt--;

		}
		
    }
	else if(UI_ChannelPool.RecvFlag==1)
	{
		cnt=0;
		step=0;	
	}
}


u16 UI_SampPoint;
u8 UI_HallThetaShift;



/*******************************************************************************
* Function Name  : UI_WindowsWithTimer1
* Description    : 
* parameters     : None
* Return         : None
*******************************************************************************/
void UI_WindowsWithTimer1(void)
{
	u8 dat0,dat1,dat2,dat3;
//	UI_FillChannelPoolWithData(0,(uint8_t)(FOCVars[M1].Iqdref.qI_Component1>>8));
//	UI_FillChannelPoolWithData(0,(uint8_t)((control_data.Left_motor_spd*10)>>8));
//	UI_FillChannelPoolWithData(1,(uint8_t)(FOCVars[M1].Vqd.qV_Component1>>8));
//	UI_FillChannelPoolWithData(0,(uint8_t)(FOCVars[M1].Iab.qI_Component1>>8));
//	UI_FillChannelPoolWithData(1,(uint8_t)(FOCVars[M1].hSampPoint>>4));

	dat0 = (uint8_t)(oHall[M1]->Vars.hElAngle>>8);
	dat1 = (uint8_t)(FOCVars[M1].Iqdref.qI_Component1*10>>8);
	dat2 = (uint8_t)(FOCVars[M1].Vqd.qV_Component1>>8);
	dat3 = (uint8_t)((control_data.Left_motor_spd*10)>>8);	
	

	UI_FillChannelPoolWithData(dat0,dat1,dat2,dat3);
}


/*******************************************************************************
* Function Name  : UI_WindowsWithTimer8
* Description    : 
* parameters     : None
* Return         : None
*******************************************************************************/
void UI_WindowsWithTimer8(void)
{

//	UI_FillChannelPoolWithData(2,(uint8_t)(oHall[M1]->Vars.hMeasuredElAngle>>8));
//	UI_FillChannelPoolWithData(2,(uint8_t)(FOCVars[M1].Iqdref.qI_Component1>>8));
//	if(bHallSta!=oHall[M1]->Vars.bHallRunState)
//	{
//		bHallSta=oHall[M1]->Vars.bHallRunState;
//		hVal = bHallSta*10;
//	}
//	else
//		hVal = oHall[M1]->Vars.hElAngle>>8;
//	UI_FillChannelPoolWithData(3,(uint8_t)(hVal));
//	UI_FillChannelPoolWithData(2,(uint8_t)(oHall[M2]->Vars.hElAngle>>8));
//	UI_FillChannelPoolWithData(3,(uint8_t)(oHall[M2]->Vars.hMeasuredElAngle>>8));

//	UI_FillChannelPoolWithData(2,(uint8_t)(FOCVars[M1].Ialphabeta.qI_Component1>>8));
//	UI_FillChannelPoolWithData(3,(uint8_t)(FOCVars[M1].Vqd.qV_Component1>>8));

}


#endif


/**************************************************************END OF FILE****/
