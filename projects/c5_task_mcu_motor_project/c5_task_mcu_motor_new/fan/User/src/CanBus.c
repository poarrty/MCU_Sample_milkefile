/******************************************************************************
* File Name         :  CanBus.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  Can总线处理程序                      
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 陈雅枫             19/01/14      1.0               创建   
* 陈雅枫             20/02/07      1.1               修改代码格式 
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

#define NEEDRC6ENCRYPTION

CanTxMsg TxMessage;
CanRxMsg RxMessage;

__IO int16_t hCanSeqNum = 0;
__IO int32_t wCanErrorNum = 0;

uint8_t  bCanCommFault = 0;
uint8_t  bCanMagneticBrakeRelease = 0;
uint8_t  bCanMagneticBrakeStaM1 = 0;
uint8_t  bCanMagneticBrakeStaM2 = 0;
uint8_t  bNeedEncryption = 0;

uint16_t hCanMessageID = 0;



CANProtocol_t 	CanProcotolData;
CANVersion_t  	JoyModVersionData;
CANVersion_t  	MCModVersionData = {1,{0,0,1},{0,0,2}};
ControlCmd_t 	ControlCommand = {0,0}; //鍗曚綅0.1hz   *6 = rpm
MotorParam_t	MotorParamData;
DriverMsg_t 	DriverMsg = {0,0,0,0};
DriverMsg_t 	DriverMsg1 = {0,0,0,0};
HALL_LR_DEF     HALL_Msg = {0,0}; 

UserParam_t 	UserParamData;


extern uint8_t bOvAbnormalFlag;
extern uint8_t bcompeletFlag;
extern uint8_t bMagneticBrakeRelease;
extern uint8_t bPowerDownFlag;
extern uint8_t g_bSPI_CRCerr;
extern uint8_t g_bSPI_Loss;

uint8_t CAN_Buff[64] = {0};

extern uint8_t  bBatteryChargeSta;
extern Relay_t Relay;


void CAN_Reply_Handle(uint16_t CMD, uint8_t  *data, uint16_t count);
void CAN_HandleUpdate(void);

int32_t hHandlebarValue=0;
/*******************************************************************************
* Function Name  : CAN_HandleUpdate
* Description    : 处理升级，重启到Boot
* parameters     : void               
* Return         : void
********************************************************************************/
void CAN_HandleUpdate(void)
{
	u8 mcustatus[8]={3,1,1,1,1,1,1,1};
	u8 Byte_Num = 0;
	u16 HalfWord = 0;
	u32 addr = 0x8004000;
//	FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STOP_NoRST, OB_STDBY_NoRST);
//	FLASH_Unlock();
//	FLASH_ErasePage(addr);
	while (Byte_Num < 8)
	{
		HalfWord = mcustatus[Byte_Num];
		HalfWord |= mcustatus[Byte_Num+1] << 8;
//		FLASH_ProgramHalfWord(addr, HalfWord);
		addr += 2;
		Byte_Num +=2;
	}
//	FLASH_Lock();
	 __disable_irq();
	NVIC_SystemReset();
}
/*******************************************************************************
* Function Name  : CAN_Init
* Description    : CAN 外设初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void CAN_Config(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	/* GPIO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
		
	/* Configure CAN TX pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// CAN RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* CANx Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;//DISABLE
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;//ENABLE
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/* CAN Baudrate = 1MBps*/  //36M/((1+4+4)*4)=1M    内部晶振最高64M  CAN时钟为64/2=32M 32M/((1+3+4)*8)=0.5M  外部晶振最高72M  CAN时钟为72/2=36M 36M/((1+3+2)*12)=0.5M
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  // CAN_SJW_1tq
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;  // CAN_BS1_12tq  CAN_BS1_4tq
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;  //CAN_BS2_5tq   CAN_BS2_4tq    CAN_BS2_2tq
	CAN_InitStructure.CAN_Prescaler = CAN_Baudrate_Prescaler; // 12  2  4  CAN_Baudrate_Prescaler  8
	CAN_Init(CAN1,&CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = (CAN_FILTER_ID>>13)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterIdLow = (((CAN_FILTER_ID<<3)&0xFFFF)|(0x01 << 2)|(0x00 << 1))&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (CAN_MASK_ID>>13)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (((CAN_MASK_ID<<3)&0xFFFF)|(0x01 << 2)|(0x01 << 1))&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =CAN_PRE_EMPTION_PRIORITY;//11
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ENABLE
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	//  /* Transmit */
	//  TxMessage.StdId = 0x07FF;
	//  TxMessage.ExtId = 0x1FFFFFF0;
	//  TxMessage.RTR = CAN_RTR_DATA;
	//  TxMessage.IDE = CAN_Id_Extended;
	//  TxMessage.DLC = 7;

	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);//接收中断允许

	CAN_ITConfig(CAN1,CAN_IT_ERR|CAN_IT_LEC,ENABLE); 
	//CAN_ITConfig(CAN1,CAN_IT_ERR,DISABLE);//禁止错误中断

	rc4_init(&rc4_st ,rc4_key,8);

//	bCanMagneticBrakeRelease = 0;
//	bCanCommFault = 0;
//	bCanMagneticBrakeStaM1 = 0;
//    bCanMagneticBrakeStaM2 = 0;
//    bNeedEncryption = 0;
//    hCanMessageID = 0;
	
}


/*******************************************************************************
* Function Name  : CanBusCommunicationTask
* Description    : CAN 通讯处理函数
* parameters     : None
* Return         : None
*******************************************************************************/
void CAN_CommunicationTask(void)
{
	uint8_t decryMsg[8]={0};	
	if(hCanMessageID)
	{			
#ifdef NEEDRC6ENCRYPTION	
	rc4_run(&rc4_st, decryMsg, CanProcotolData.pdata, CanProcotolData.curMsgLen);//加密
#else
	memcpy(decryMsg,CanProcotolData.pdata,	CanProcotolData.curMsgLen);//不加密
#endif

		if(hCanMessageID == DSP_CONNET_TEST )//can connect test
		{
#ifdef NEEDRC6ENCRYPTION
			if(bcompeletFlag == 1)
			{
			
			
			uint8_t msgdata[8] ={0};
			if(decryMsg[0] == 0)
			{	
				char * verchr  = SWVERSION;
				MCModVersionData.bSWvserion[0] =  verchr[0] -'0';
				MCModVersionData.bSWvserion[1] =  verchr[2] -'0';
				MCModVersionData.bSWvserion[2] =  verchr[4] -'0';
				memcpy(&msgdata[1],&MCModVersionData,sizeof(CANVersion_t));
				CAN_Reply_Handle(DSP_CONNET_TEST,msgdata,8);
			}else if(decryMsg[0] == 1)
			{
				 msgdata[0] = 0x1;
				 
				 CAN_Reply_Handle(DSP_CONNET_TEST,msgdata,8);
				 bNeedEncryption = 1;
			}
		   }
			else
			{
				 uint8_t msgdata[8] ={0};
			  msgdata[0] = 0xFF;				 
				 CAN_Reply_Handle(DSP_CONNET_TEST,msgdata,8);	
			}
#endif
		}
		else if(hCanMessageID == DSP_CONTROL_PARA_CMD)//motor control command
		{
			uint8_t bxor = (decryMsg[0] + decryMsg[1]+decryMsg[2]+decryMsg[3] )&0xFF;
#ifdef NEEDRC6ENCRYPTION		
			if( bNeedEncryption == 1   && bxor == decryMsg[4])		//当加密协议完成
			{
				memcpy(&ControlCommand,decryMsg,sizeof(ControlCmd_t));
				CAN_Reply_Handle(DSP_CONTROL_PARA_CMD,(uint8_t *)&DriverMsg,sizeof(DriverMsg_t));
			}else
			{		//加密未完成，但接收到速度指令，这种情况发生在下控重启，上控未重启情况
				uint8_t msgdata[8] ={0};
				memcpy(&msgdata[1],&MCModVersionData,sizeof(CANVersion_t));
				CAN_Reply_Handle(DSP_CONNET_TEST,msgdata,8);
			}
#else
			memcpy(&ControlCommand,decryMsg,sizeof(ControlCmd_t));
			CAN_Reply_Handle(DSP_CONTROL_PARA_CMD,(uint8_t *)&DriverMsg,sizeof(DriverMsg_t));
#endif
		}
		else if(hCanMessageID == DSP_MOTOR_PARA_SET)
		{
			memcpy(&MotorParamData,decryMsg,sizeof(MotorParam_t));
			CAN_Reply_Handle(DSP_MOTOR_PARA_SET,(uint8_t *)0,0);
		}
		else if(hCanMessageID == DSP_USER_PARA_SET)
		{
			memcpy(&UserParamData,decryMsg,sizeof(UserParam_t));
			CAN_Reply_Handle(DSP_USER_PARA_SET,(uint8_t *)0,0);
		}
		else if(hCanMessageID == DSP_GET_ELECTRONIC_INFO && CanProcotolData.wFuncID==7)//&& CanProcotolData.wFuncID==7
		{
			memcpy(&bCanMagneticBrakeRelease,decryMsg,sizeof(bCanMagneticBrakeRelease));
			CAN_Reply_Handle(DSP_GET_ELECTRONIC_INFO,(uint8_t *)&bCanMagneticBrakeRelease,1);
		} 
		else if(hCanMessageID == DSP_UPATE_MOTOR_CMD)
		{  		
			CAN_HandleUpdate();
		}

		hCanMessageID = 0;

        ControlCommand.hSpeedM1 =hHandlebarValue ;
/*		
		if(hCanMessageID == DSP_CONNET_TEST)//can connect test
		{
			memcpy(&JoyModVersionData,CanProcotolData.pdata,sizeof(CANVersion_t));
			CAN_Reply_Handle(DSP_CONNET_TEST,(uint8_t *)&MCModVersionData,sizeof(CANVersion_t));
		}
		else if(hCanMessageID == DSP_CONTROL_PARA_CMD)//motor control command
		{
			memcpy(&ControlCommand,CanProcotolData.pdata,sizeof(ControlCmd_t));
			
			//ControlCommand.hSpeedM1 = ControlCommand1.hSpeedM2;
			//ControlCommand.hSpeedM2 = ControlCommand1.hSpeedM1;		
			
			CAN_Reply_Handle(DSP_CONTROL_PARA_CMD,(uint8_t *)&DriverMsg1,sizeof(DriverMsg_t));	
			CAN_Reply_Handle(DSP_GET_ROBOT,(uint8_t *)&HALL_Msg,sizeof(HALL_LR_DEF)); 
		}
		else if(hCanMessageID == DSP_MOTOR_PARA_SET)
		{
			memcpy(&MotorParamData,CanProcotolData.pdata,sizeof(MotorParam_t));
			CAN_Reply_Handle(DSP_MOTOR_PARA_SET,(uint8_t *)0,0);
		}
		else if(hCanMessageID == DSP_USER_PARA_SET)
		{
			memcpy(&UserParamData,CanProcotolData.pdata,sizeof(UserParam_t));
			CAN_Reply_Handle(DSP_USER_PARA_SET,(uint8_t *)0,0);
		}
		else if(hCanMessageID == DSP_GET_ELECTRONIC_INFO && CanProcotolData.wFuncID==7)
		{
			memcpy(&bCanMagneticBrakeRelease,CanProcotolData.pdata,sizeof(bCanMagneticBrakeRelease));
			CAN_Reply_Handle(DSP_GET_ELECTRONIC_INFO,(uint8_t *)&bCanMagneticBrakeRelease,1);
		} 
		else if(hCanMessageID == DSP_UPATE_MOTOR_CMD)
		{  		
			CAN_HandleUpdate();
		}

		hCanMessageID = 0;		
*/						
	}
	
	DriverMsg.hDriverFlag.halfword = 0;
	#ifdef DOUBLE_DRIVER
	if(oPwmc[M2]->Vars.bOverCurrentFault !=0 )
		DriverMsg.hDriverFlag.bit.b0 = 1;
	#endif
	if(oPwmc[M1]->Vars.bOverCurrentFault !=0 )
		DriverMsg.hDriverFlag.bit.b1 = 1;	

#ifdef LINEAR_HALL	
	if(oHall[M1]->Vars.bHallFault !=0 || g_bSPI_CRCerr==1 || g_bSPI_Loss==1) //线性霍尔或SPI通信故障
		DriverMsg.hDriverFlag.bit.b2 = 1;
	
	if(oHall[M2]->Vars.bHallFault !=0 || g_bSPI_CRCerr==1 || g_bSPI_Loss==1) //线性霍尔或SPI通信故障  
		DriverMsg.hDriverFlag.bit.b3 = 1;
	#endif
#ifdef SWITCH_HALL
	if(oHall[M1]->Vars.bHallFault !=0 ) //开关霍尔故障
		DriverMsg.hDriverFlag.bit.b2 = 1;
	#ifdef DOUBLE_DRIVER
	if(oHall[M2]->Vars.bHallFault !=0 ) //开关霍尔故障   
		DriverMsg.hDriverFlag.bit.b3 = 1;
	#endif
	#endif
#ifdef ENCODER_SENSOR
	if(oEncoder[M1]->Vars.bEncoderFault !=0 || oEncoder[M1]->Vars.bPWMStartErr !=0 || oEncoder[M1]->Vars.bInitFault != 0 || oEncoder[M1]->Vars.bEncoderDletaCntFault!=0
		 || oEncoder[M1]->Vars.bEncoderZFault!=0 || oEncoder[M1]->Vars.bEncoderA0Fault!=0 || oEncoder[M1]->Vars.bEncoderB0Fault!=0 
	   || oEncoder[M1]->Vars.bEncoderA1Fault !=0 || oEncoder[M1]->Vars.bEncoderB1Fault !=0) //编码器故障(PWM错误和断线)
		DriverMsg.hDriverFlag.bit.b2 = 1;

	if(oEncoder[M2]->Vars.bEncoderFault !=0 || oEncoder[M2]->Vars.bPWMStartErr !=0 || oEncoder[M2]->Vars.bInitFault != 0 || oEncoder[M2]->Vars.bEncoderDletaCntFault!=0
		 || oEncoder[M2]->Vars.bEncoderZFault!=0 || oEncoder[M2]->Vars.bEncoderA0Fault!=0 || oEncoder[M2]->Vars.bEncoderB0Fault!=0 
	   || oEncoder[M2]->Vars.bEncoderA1Fault !=0 || oEncoder[M2]->Vars.bEncoderB1Fault !=0) //编码器故障  
		DriverMsg.hDriverFlag.bit.b3 = 1;


#endif
	
//	if(FOCVars[M1].bCanMagneticBrakeState != 0) //M1静止和运行中手动刹车检测
//		DriverMsg.hDriverFlag.bit.b4 = 1;

//	if(FOCVars[M2].bCanMagneticBrakeState != 0) //M2静止和运行中手动刹车检测
//		DriverMsg.hDriverFlag.bit.b5 = 1;

	if(Relay.bFail != 0)
		DriverMsg.hDriverFlag.bit.b6 = 1;

	if(Relay.bFail_lv != 0)
		DriverMsg.hDriverFlag.bit.b7 = 1;

//	if(oHall[M2]->Vars.bBlockageFault != 0)
//		DriverMsg.hDriverFlag.bit.b8 = 1;

//	if(oHall[M1]->Vars.bBlockageFault != 0)
//		DriverMsg.hDriverFlag.bit.b9 = 1;
	#ifdef DOUBLE_DRIVER
	if(oPosSen[M2]->Vars.bBlockageFault != 0)
		DriverMsg.hDriverFlag.bit.b8 = 1;
	if(oPwmc[M2]->Vars.bMosfetHFault != 0)
		DriverMsg.hDriverFlag.bit.b10 = 1;
		
	if(oPwmc[M2]->Vars.bMosfetLFault != 0)
		DriverMsg.hDriverFlag.bit.b12 = 1;
	#endif
	
	if(oPosSen[M1]->Vars.bBlockageFault != 0)
		DriverMsg.hDriverFlag.bit.b9 = 1;



	if(oPwmc[M1]->Vars.bMosfetHFault != 0)
		DriverMsg.hDriverFlag.bit.b11 = 1;

	
	if(oPwmc[M1]->Vars.bMosfetLFault != 0)
		DriverMsg.hDriverFlag.bit.b13 = 1;

	if(bBatteryChargeSta != 0)
		DriverMsg.hDriverFlag.bit.b14 = 1;

	if(bThermalShutdownFlag != 0) 
		DriverMsg.hDriverFlag.bit.b15 = 1;	//下控MCU过热故障

	DriverMsg1.hDriverFlag = DriverMsg.hDriverFlag;
    //if(bOvAbnormalFlag != 0) 
	  //DriverMsg.hDriverFlag.bit.b15 = 1;	//下控电压异常(过压)故障

//	if(Relay.bReady != 0) 
//		DriverMsg.hDriverFlag.bit.b15 = 1;	//下控上电继电器吸合准备好标志位给上控

#ifdef LINEAR_HALL	
	DriverMsg.hSpeedM1 = (int) -HALL_GetAvrgMecSpeed01Hz(oHall[M1]); // /253(减速比是23 中大电机)  173(减速比是19 nidec电机)    /140 (减速比是17 联谊电机 /(8.2*17)  
	#ifdef DOUBLE_DRIVER
	DriverMsg.hSpeedM2 = (int) HALL_GetAvrgMecSpeed01Hz(oHall[M2]) ; //    /253  *27 /190    /209  *12/2090 
#endif
	#endif
	#ifdef SWITCH_HALL
    DriverMsg.hSpeedM1 = (int) HALL_GetAvrgMecSpeed01Hz(oHall[M1]);//187
	#ifdef DOUBLE_DRIVER 
	DriverMsg.hSpeedM2 = (int) HALL_GetAvrgMecSpeed01Hz(oHall[M2]);//187
		#endif
#endif
	#ifdef ENCODER_SENSOR
	  DriverMsg.hSpeedM1 = (int) ENC_GetAvrgMecSpeed01Hz(oEncoder[M1]);//187
    DriverMsg.hSpeedM2 = (int) -ENC_GetAvrgMecSpeed01Hz(oEncoder[M2]);//187
		DriverMsg1.hSpeedM2 = (int) ENC_GetAvrgMecSpeed01Hz(oEncoder[M1]);//187
    DriverMsg1.hSpeedM1 = (int) -ENC_GetAvrgMecSpeed01Hz(oEncoder[M2]);//187
#endif
	#ifdef DOUBLE_DRIVER 
  		HALL_Msg.hLeftHall=  16384 - TIM_GetCounter(TIM4);//反馈左电机霍尔计数值 
#endif
HALL_Msg.hRightHall = TIM_GetCounter(TIM2);;//反馈右电机霍尔计数值 
	
	//DriverMsg.bData1 = bCanMagneticBrakeRelease;// bCanMagneticBrakeRelease   Relay.bReady 0x20000119
	//DriverMsg.bData2 = bOvAbnormalFlag|( (bMagneticBrakeRelease)<<1) | (bCanCommFault<<2) | (bMcDriverErrFlag<<3) | (PowerEnableFlag<<4) |(bPowerDownFlag<<5);	//(oHall[M2]->Vars.hElAngle)>>8
	  //DriverMsg.bData1 =  hVs>>8;
   // DriverMsg.bData2 =hVs;
//	DriverMsg.driver_err_flag = (oPwmc[M2]->Vars.bOverCurrentFault)
//								|(oPwmc[M1]->Vars.bOverCurrentFault<<1)
//								|((oHall[M1]->Vars).bHallFault<<2)
//								|((oHall[M2]->Vars).bHallFault<<3)
//								|(FAULTN_Relay_FB_L_err<<4)
//	                            |(FAULTN_Relay_FB_R_err<<5) 
//	                            |(Relay.bFail<<6) | (Relay.bFail_lv<<7) 
//	                            |(oHall[M2]->Vars.bBlockageFault<<8)|(oHall[M1]->Vars.bBlockageFault<<9) | (oPwmc[M2]->Vars.bMosfetHFault<<10) | (oPwmc[M1]->Vars.bMosfetHFault<<11) | 
//	                             (oPwmc[M2]->Vars.bMosfetLFault<<12) | (oPwmc[M1]->Vars.bMosfetLFault<<13) | (bat_charge<<14) ; //   (bat_charge<<14)		(oPwmc[M2]->Vars.bPhaseOffsetFault<<6)  (oPwmc[M1]->Vars.bPhaseOffsetFault<<7)                     	

	

}


/*******************************************************************************
* Function Name  : CAN_Reply_Handle
* Description    : CAN 应答函数
* parameters     : 
* Return         : None
*******************************************************************************/
void CAN_Reply_Handle(uint16_t CMD, uint8_t  *data, uint16_t count)
{
	uint8_t bSegpolo = 0;
	uint8_t bSegnum = 0;
	uint8_t EndPackBit = 0;
	uint16_t hTxMsgSendCnt = 0;
	bSegnum = (count>>3) + ((count&0x07)?1:0);
	
	if(bSegnum <= 1) 
		bSegnum = 0;
	else 
		bSegnum --;
	
	TxMessage.StdId = 0;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_EXT;
	
	for(bSegpolo = 0; bSegpolo < (bSegnum+1); bSegpolo++)
	{
        if(bSegpolo == bSegnum)     //结束标志
            EndPackBit = 1;
        TxMessage.ExtId = CAN_SEND_ID | (CMD << CAN_CMD_BITPOS) | (EndPackBit << CAN_END_BITPOS) |(bSegpolo);

		if(count >= 8) 
		{
			TxMessage.DLC = 8;
			count = count - 8;
		}
		else TxMessage.DLC = count;

		if(TxMessage.DLC) 
		{
			#ifdef NEEDRC6ENCRYPTION
			rc4_run(&rc4_st, TxMessage.Data, &data[bSegpolo<<3], TxMessage.DLC);

			#else
			memcpy(TxMessage.Data, &data[bSegpolo<<3], TxMessage.DLC);
			#endif
			
			hTxMsgSendCnt ++;
		}
		//while(CAN_Transmit(CAN1,&TxMessage) == CAN_NO_MB);
		while(CAN_Transmit(CAN1,&TxMessage) == CAN_NO_MB && hTxMsgSendCnt < 2048);
	}
}





/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : CAN 中断
* parameters     : None
* Return         : None
*******************************************************************************/
/*
void USB_LP_CAN1_RX0_IRQHandler(void)
{

    if(CAN_GetFlagStatus(CAN1 ,CAN_FLAG_FMP0) ==  SET)
    {
        uint8_t bSegPolo = 0;
        uint8_t bEndPack = 0;
       // uint8_t bLength = 0;

        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

        bSegPolo = RxMessage.ExtId&0x3F;
        bEndPack = (RxMessage.ExtId>>CAN_END_BITPOS) & 0x1;

        if(bSegPolo == 0)
        {
            CanProcotolData.wFuncID = 0;
            CanProcotolData.hDataCount = 0;
        }

        CanProcotolData.wFuncID = (RxMessage.ExtId >> CAN_CMD_BITPOS)&0xFF;

        CanProcotolData.curMsgLen = RxMessage.DLC;

		memcpy(&CAN_Buff[CanProcotolData.hDataCount],RxMessage.Data, RxMessage.DLC);					//3us
		
        CanProcotolData.hDataCount += CanProcotolData.curMsgLen;

        if(bEndPack)
        {
                hCanMessageID = CanProcotolData.wFuncID;
                CanProcotolData.pdata = CAN_Buff;
        }

         hCanSeqNum ++;
    }

}
*/

/*******************************************************************************
* Function Name  : CAN1_SCE_IRQHandler
* Description    : CAN 出错中断
* parameters     : None
* Return         : None
*******************************************************************************/
/*
void CAN1_SCE_IRQHandler()
{
	CAN_ClearITPendingBit(CAN1,CAN_IT_EWG);
	CAN_ClearITPendingBit(CAN1,CAN_IT_EPV);
	CAN_ClearITPendingBit(CAN1,CAN_IT_BOF);
	CAN_ClearITPendingBit(CAN1,CAN_IT_ERR);
    CAN_ClearITPendingBit(CAN1,CAN_IT_LEC);
	//SETERRORFLAG(Error_can_err_flag);
	wCanErrorNum++;
}
*/
/*******************************************************************************
* Function Name  : CAN_CommFaultCheck
* Description    : CAN 通信故障判断
* parameters     : None
* Return         : None
*******************************************************************************/
void CAN_CommFaultCheck(void) //CAN 通信故障判断 20190929  jimianhao
{	
	static int16_t hCanPreSeqNum = 0;
	static int16_t hErrorCnt1 = 0;
	static int16_t hErrorCnt2 = 0;
	
	if( hCanSeqNum == hCanPreSeqNum ) 
	{ 
		hErrorCnt1++;
		if( hErrorCnt1>=80 )
		{
			bCanCommFault = 0;//lee
			hErrorCnt1=81;
		//	CAN_Config();
		}
	}
	else
	{	
		hErrorCnt1 = 0;
		hCanPreSeqNum = hCanSeqNum;
		
		if( bCanCommFault ==1 ) //通信正常后,清除故障标志	
		{
			hErrorCnt2++;
			if( hErrorCnt2>=20 ) 
			{
				bCanCommFault = 0;
				hErrorCnt2 = 0;   
			}
		}	  
	}
}

/**************************************************************END OF FILE****/
