/******************************************************************************
* File Name         :  LinearHallSensor.c
* Author            :  徐欣
* Version           :  1.0 
* Date              :  19/12/05
* Description       :  霍尔及电角度处理函数                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 徐欣              19/12/05      1.0               创建   
* 陈雅枫             20/02/05      1.1               修改代码风格，并将线性霍尔部分独立为一个文件。
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
#include "UserInclude.h"
#include "MCInclude.h"

#ifdef LINEAR_HALL


HALLParams_t HALLParamsM1 =   
{
	(8200),//-25300  -24900 -24600(nidec第二次打样) 反转:3300   反转:7250(全新nidec  6950   D版本6950 E版本-30850 -31000 -28840 -28900 -29500  -30200(140rpm)) -29300(SPI为6M,12M)  -28700(SPI为3M)
	0,
	2,
	3,
	M1,
	
};

HALLParams_t HALLParamsM2 =  
{
	(-29300),//3600  3400 3100(nidec第二次打样) 2400(nidec第一次打样) 反转:40767       反转:36500(全新nidec 37250 37400 D版本37250 E版本36520,速度环取1 8100,8250,6350 7620速度环取-1)  8100(SPI为6M,12M)  7500(SPI为3M)
	1,
	2,
	4,
	M2,
	
};

HALLVars_t HALLVarsInt =
{
	0,
	0,
	0,
	0,
	0,
};



HALL_t HALLpool[NBR_OF_MOTORS]; 


#define LOCKED_TIME         ((int16_t)311) //堵转时间 256*187 PWM cycle =  6s(187)        16s(500)  311(10S)




#define SPI1_DR_REG (SPI1_BASE + 0x0C)
#define SPI3_DR_REG (SPI3_BASE + 0x0C)

#define ANGLE_BUFFER (8)

#define ANGLE_FILTER (2)

uint16_t hDataReceived[7];//接收到的数据，包括角度、故障信息和通信计数等

uint8_t g_bSPICommError = 0;//通信故障标志
uint8_t g_bSPISeqNum = 0;
uint8_t g_bSPICommErrorCnt = 0;

uint8_t g_bSPI_Loss = 0;
uint8_t g_bSPI_CRCerr = 0;


void SPI_Config(void);

/*******************************************************************************
* Function Name  : LINHALL_Init
* Description    : 上电HALL初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void LINHALL_Init(void)
{
    oHall[0] = &HALLpool[0];
    oHall[1] = &HALLpool[1];

    oHall[0]->pParams = &HALLParamsM1;
    oHall[1]->pParams = &HALLParamsM2;

	oHall[0]->Vars = HALLVarsInt;
	oHall[1]->Vars = HALLVarsInt;

	SPI_Config();

}


/*******************************************************************************
* Function Name  : SPI_Config
* Description    : 上电SPI初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void SPI_Config(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    EXTI_InitTypeDef EXTI_InitStruct;//1111
    NVIC_InitTypeDef NVIC_InitStructure;//1111

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* Only PA15 and PB3 are released JTAG-DP Disabled and SW-DP Enabled */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

    //PA15-NSS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PB3-SCK,PB4-MISO,PB5-MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB,GPIO_Pin_4);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_StructInit(&SPI_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 0x1021;

    SPI_Init(SPI1, &SPI_InitStructure);

//    SPI_CalculateCRC(SPI1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    CRC_ResetDR();

//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
//    /* DMA2 channel1 configuration */
//    DMA_DeInit(DMA2_Channel1);
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI1_DR_REG;
//    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&hDataReceived[0]);
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//    DMA_InitStructure.DMA_BufferSize = 3u;//1u;
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//    DMA_Init(DMA2_Channel1, &DMA_InitStructure);
//    /* Enable DMA2 Channel1 */
//    DMA_Cmd(DMA2_Channel1, ENABLE);

//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* DMA1 channel2 configuration */
    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI1_DR_REG;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&hDataReceived[0]);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 7u;//1u;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    /* Enable DMA1 Channel2 */
    DMA_Cmd(DMA1_Channel2, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);

//    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);

    SPI_Cmd(SPI1, ENABLE);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14); // GPIO_PinSource13  新板卡PC14做NIDEC的IO输入中断
    //1111
    EXTI_InitStruct.EXTI_Line = EXTI_Line14; // EXTI_Line13  新板卡PC14做NIDEC的IO输入中断
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
    //1111
    EXTI_ClearITPendingBit(EXTI_Line14); //	EXTI_Line13   新板卡PC14做NIDEC的IO输入中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;  //选择中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //使能中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                //优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                //优先级为1
    NVIC_Init(&NVIC_InitStructure);

}


/*******************************************************************************
* Function Name  : LINHALL_GetElAngle
* Description    : 获取当前电角度
* parameters     : this HALLpool结构体指针
* Return         : pVars->hElAngle 当前电角度
*******************************************************************************/
int16_t LINHALL_GetElAngle(pHALL_t this)
{
	pHALLVars_t pVars = &(this->Vars);
	pHALLParams_t pParams = this->pParams;
	 
	return (-(pVars->hElAngle) + (pParams->hAngleShift));
}




/*******************************************************************************
* Function Name  : LINHALL_HandleHallParam
* Description    : 获取Hall相关参数
* parameters     : this HALLpool结构体指针
* Return         : None
*******************************************************************************/
void LINHALL_HandleHallParam(pHALL_t this, FOCVars_t *Global)
{
	pHALLVars_t pVars = &(this->Vars);
	pHALLParams_t pParams = this->pParams;

	ComplexVars_t ComplexVars;
	
   // static uint8_t bSPISeqNum = 0;//通信计数
   // static uint8_t bSPICommErrorCnt = 0;//通信错误计数

	uint8_t bAngleIndex = pParams->bAngleIndex;
	uint8_t bAuxIndex = pParams->bAuxIndex;
	uint8_t bSpeedIndex = pParams->bSpeedIndex;
	uint8_t bSeqNum = 0;

	
	if(pVars->hHallTime<1000)
		pVars->hHallTime++;
	else
		pVars->hElSpeedDpp=0;
	
	if((Global->bRunning != 0)&&(Global->bBlockageCurrent!=0))
	{
		if(++ pVars->bHallTimeDivisor == 0)
		{
			if(pVars->hHallATime<10000)
				pVars->hHallATime++;

			if(pVars->hHallBTime<10000)
				pVars->hHallBTime++;

			if(pVars->hHallCTime<10000)
				pVars->hHallCTime++;

			if((pVars->hHallATime>LOCKED_TIME)||
				(pVars->hHallBTime>LOCKED_TIME)||
				(pVars->hHallCTime>LOCKED_TIME))
			{
				pVars->bBlockageFault = 1;
			}
		}
	}

    if((pVars->bSPIFla == 0)&&(g_bSPICommError == 0))
    {
        pVars->bSPIFla = 1;
        pVars->hElAngleBak = pVars->hElAngle;
    }
    else if(g_bSPICommError == 1)
    {
        pVars->bSPIFla = 0;
    }

    if((g_bSPI_Loss == 0)
            &&(g_bSPI_CRCerr == 0))//通信校验ok，采用新的数据
    {

        pVars->hElAngle = (int16_t)hDataReceived[bAngleIndex]; //电角度 堵转测试把该值设为常数
        ComplexVars.HalfWord = hDataReceived[bAuxIndex];

        if(pParams->bMotor == M1)
        {             	
			pVars->bHallFault = (uint8_t)ComplexVars.Block.HallFaultM1;//故障标志        	
            pVars->bHallState =  (uint8_t)ComplexVars.Block.HallStateM1;//转换为开关HALL
          	
        }
        else
        {                 	
			pVars->bHallFault = (uint8_t)ComplexVars.Block.HallFaultM2;//故障标志          	
            pVars->bHallState =  (uint8_t)ComplexVars.Block.HallStateM2;//转换为开关HALL
           	
        }

        bSeqNum = ComplexVars.Block.SeqNum;//通信计数器
        pVars->hElSpeedDpp = (int16_t)hDataReceived[bSpeedIndex];//角速度

        //------------通信故障判断--start
        if(bSeqNum != g_bSPISeqNum)
        {
            if(g_bSPICommErrorCnt > 0)
                g_bSPICommErrorCnt--;
        }
        else
        {
            if(g_bSPICommErrorCnt < 10)
                g_bSPICommErrorCnt ++;
        }

        if(g_bSPICommErrorCnt > 3)
        {
            g_bSPICommError = 1;
        }
        else if(g_bSPICommErrorCnt == 0)
        {
            g_bSPICommError = 0;
        }
        //------------通信故障判断--end

        g_bSPISeqNum = bSeqNum;
    }

//	pVars->wElSpeedSum += pVars->hElSpeedDpp ;// pVars->hElSpeedDpp
//	pVars->wElSpeedSum -= pVars->hElSpeedBuf[pVars->hElSpeedCnt];
//	pVars->hElSpeedBuf[pVars->hElSpeedCnt] = pVars->hElSpeedDpp;
//	if(pVars->hElSpeedCnt < SPEEDNUM)
//		pVars->hElSpeedCnt++;
//	else
//		pVars->hElSpeedCnt = 0;

	if(pVars->hElSpeedCnt < SPEEDNUM)// SPEEDNUM
	{
      pVars->hElSpeedCnt++;
	  pVars->wElSpeedSum += pVars->hElSpeedDpp ;
	}
	else
	{
		pVars->wElSpeedAvrg = (pVars->wElSpeedSum>>9)*25;//*25(SPI为6M和12M) *17(SPI为3M)   9
		pVars->wElSpeedSum = 0;
		pVars->hElSpeedCnt = 0; 
	}

	if(pVars->bHallPreState != pVars->bHallState)
	{
		uint8_t err;

        err = pVars->bHallPreState ^ pVars->bHallState;
        
        if(err & 0x01)
            pVars->hHallATime=0;
        else if(err & 0x02)
            pVars->hHallBTime=0;
        else if(err & 0x04)
            pVars->hHallCTime=0;

		pVars->bHallPreState = pVars->bHallState;
		pVars->hLastHallTime = pVars->hHallTime;
        pVars->hHallTime = 0;

	}
	
}



/*******************************************************************************
* Function Name  : HALL_GetAvrgMecSpeed01Hz
* Description    : 获取当前电机本体转速（不考虑减速器）
* parameters     : this HALLpool结构体指针
* Return         : hMecSpeed01Hz 当前电机本体转速
*******************************************************************************/
int16_t LINHALL_GetAvrgMecSpeed01Hz(pHALL_t this)
{
	pHALLVars_t pVars = &(this->Vars);
    int16_t hMecSpeed01Hz;

	//hMecSpeed01Hz = (int16_t)(pVars->wElSpeedSum>>4);
    hMecSpeed01Hz = (int16_t)pVars->wElSpeedAvrg ;
	if(pVars->hElSpeedDpp<=5 && pVars->hElSpeedDpp>=-5) 
	{
	  hMecSpeed01Hz =0;
	}
	return(hMecSpeed01Hz);
}


/*******************************************************************************
* Function Name  : HALL_MeasureInit
* Description    : 变量初始化 用于电机启动
* parameters     : this  HALLpool结构指针
* Return         : None
*******************************************************************************/
void LINHALL_MeasureInit(pHALL_t this, int16_t hDesCmd)   
{
	pHALLVars_t pVars =&(this->Vars);

	uint16_t i;

	
	//for(i=0; i<SPEEDNUM; i++)
	//{
	//	pVars->hElSpeedBuf[i] = 0;
	//}      
      
    pVars->hHallATime = 0;
    pVars->hHallBTime = 0;
    pVars->hHallCTime = 0;

	pVars->hElSpeedDpp = 0;
	
    pVars->bHallInitFault = 0;

    
   if((pVars->bHallState ==0)||(pVars->bHallState ==7))
    {
        pVars->bHallInitFault = 1;
        return ;
    }

}




void EXTI15_10_IRQHandler(void)//耗时约2.36us--相对CS上升沿延后约1.1us
{
    uint32_t CRCtemp1,CRCtemp2;

    if(EXTI_GetITStatus(EXTI_Line14) != RESET) // EXTI_Line13  新板卡PC14做NIDEC的IO输入中断
    {
//        GPIOC->BSRR = GPIO_Pin_14;//1111指示

        CRC->DR = (uint32_t)hDataReceived[0];
        CRC->DR = (uint32_t)hDataReceived[1];
        CRC->DR = (uint32_t)hDataReceived[2];
        CRC->DR = (uint32_t)hDataReceived[3];
        CRC->DR = (uint32_t)hDataReceived[4];

        if(((GPIOC->IDR & GPIO_Pin_14) != (uint32_t)Bit_RESET)//CS  GPIO_Pin_13  新板卡PC14做NIDEC的IO输入中断
               // &&((SPI1->SR & SPI_I2S_FLAG_BSY) != (uint16_t)RESET)//SPI_BUSY
                &&(DMA1_Channel2->CNDTR != 7))//DMA_count
        {
            g_bSPI_Loss = 1;
//            SPI1->CR1 &= CR1_SPE_Reset;//SPI_disable
//            RCC->APB2ENR &= ~RCC_APB2Periph_SPI1;//SPI clock disable

            DMA1_Channel2->CCR &= (uint16_t)(~DMA_CCR1_EN);/* Disable the selected DMAy Channelx */

            DMA1_Channel2->CNDTR = 7;//DMA_count clear
            DMA1_Channel2->CCR |= DMA_CCR1_EN;/* Enable the selected DMAy Channelx */

//            RCC->APB2ENR |= RCC_APB2Periph_SPI1;//SPI clock enable
//            SPI1->CR1 |= CR1_SPE_Set;//SPI enable
        }
        else
            g_bSPI_Loss = 0;

        CRCtemp1 = CRC->DR;//取CRC结果
        CRC->CR = CRC_CR_RESET;//CRC_ResetDR();//Resets the CRC Data register (DR).

        CRCtemp2 = ((uint32_t)hDataReceived[5]<<16) + (uint32_t)hDataReceived[6];
        if(CRCtemp1 != CRCtemp2)
            g_bSPI_CRCerr = 1;
        else
            g_bSPI_CRCerr = 0;

//        GPIOC->BRR = GPIO_Pin_14;//1111指示

//        UARTC_PutChar(hDataReceived[0]>>8);
//        UARTC_PutChar(hDataReceived[0]);
//        UARTC_PutChar(hDataReceived[1]>>8);
//        UARTC_PutChar(hDataReceived[1]);
//        UARTC_PutChar(hDataReceived[2]>>8);
//        UARTC_PutChar(hDataReceived[2]);
//        UARTC_PutChar(hDataReceived[3]>>8);
//        UARTC_PutChar(hDataReceived[3]);
//        UARTC_PutChar(hDataReceived[4]>>8);
//        UARTC_PutChar(hDataReceived[4]);

//        UARTC_PutChar(CRCtemp1>>24);
//        UARTC_PutChar(CRCtemp1>>16);
//        UARTC_PutChar(CRCtemp1>>8);
//        UARTC_PutChar(CRCtemp1);

//        UARTC_PutChar((g_bSPI_Loss<<4)+g_bSPI_CRCerr);

        EXTI_ClearITPendingBit(EXTI_Line14); // EXTI_Line13  新板卡PC14做NIDEC的IO输入中断
    }
}


#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
