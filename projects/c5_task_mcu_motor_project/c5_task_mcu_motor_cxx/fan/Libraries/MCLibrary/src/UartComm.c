/******************************************************************************
* File Name         :  UartComm.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  电控驱动的任务调用                       
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

#define CR      (0x0D)
#define LF      (0x0A)

UartC_t  UARTCBuf;


uint8_t UARTC_ReadTxBuf (void);
void UARTC_InitUartBuf(void);

/*******************************************************************************
* Function Name  : UARTC_InitUartBuf
* Description    : UartBuf初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void UARTC_InitUartBuf(void)
{
    uint8_t i;

    for(i=0; i<UARTC_RXBUFSIZE; i++)
            UARTCBuf.RxBuf[i] = 0;

    for(i=0; i<UARTC_TXBUFSIZE; i++)
            UARTCBuf.TxBuf[i] = 0;

    UARTCBuf.RxHeadPoint=0;
    UARTCBuf.RxTailPoint=0;

    UARTCBuf.TxHeadPoint=0;
    UARTCBuf.TxTailPoint=0;

    UARTCBuf.RxCount=0;
    UARTCBuf.TxCount=0;
}

/*******************************************************************************
* Function Name  : UARTC_Init
* Description    : UART 外设初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void UARTC_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
//    GPIO_PinRemapConfig(GPIO_PartialRemap_USART4, ENABLE);
    //tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    //rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 256000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_PRE_EMPTION_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART5, ENABLE);
	UARTC_InitUartBuf();
}

/*******************************************************************************
* Function Name  : UARTC_ReadTxBuf
* Description    : 读取写缓冲区数据
* parameters     : None
* Return         : ch 读出的数据
*******************************************************************************/
uint8_t UARTC_ReadTxBuf (void)
{
	uint8_t ch;

	ch = UARTCBuf.TxBuf[UARTCBuf.TxHeadPoint];
	UARTCBuf.TxHeadPoint++;
	if (UARTCBuf.TxHeadPoint >= UARTC_TXBUFSIZE) UARTCBuf.TxHeadPoint = 0;
	UARTCBuf.TxCount--;

	return ch;
}

/*******************************************************************************
* Function Name  : UARTC_WriteRxBuf
* Description    : 往读缓冲区写入数据
* parameters     : ch 需要写入的数据
* Return         : 0: 缓冲区已满           1: 正常
*******************************************************************************/
static uint8_t UARTC_WriteRxBuf (uint8_t ch)
{
	UARTCBuf.RxBuf[UARTCBuf.RxTailPoint] = ch;
	UARTCBuf.RxTailPoint++;
	if (UARTCBuf.RxTailPoint >= UARTC_RXBUFSIZE) UARTCBuf.RxTailPoint = 0;

	// overflow
	if (++UARTCBuf.RxCount >= UARTC_RXBUFSIZE)
		return 0;

	return 1;
}

/*******************************************************************************
* Function Name  : UARTC_HandleTxd
* Description    : 将写缓冲区的数据写入串口数据寄存器
* parameters     : None
* Return         : None
*******************************************************************************/
void UARTC_HandleTxd(void)
{
    if(USART_GetFlagStatus(UART5, USART_FLAG_TXE) != RESET)
    {
        if (UARTCBuf.TxCount)
        {
            UART5->DR = UARTC_ReadTxBuf();
        }
    }
}


/*******************************************************************************
* Function Name  : UARTC_PutChar
* Description    : 写一个字节数据到写缓冲区
* parameters     : ch 要写入的数据
* Return         : 0:缓冲区已满      	1:正常
*******************************************************************************/
uint8_t UARTC_PutChar(uint8_t ch)
{
	if (UARTCBuf.TxCount >= UARTC_TXBUFSIZE) return 0;

	UARTCBuf.TxBuf[UARTCBuf.TxTailPoint] = ch;
	UARTCBuf.TxTailPoint++;
	if (UARTCBuf.TxTailPoint >= UARTC_TXBUFSIZE) UARTCBuf.TxTailPoint = 0;
	UARTCBuf.TxCount++;

	return 1;
}


/*******************************************************************************
* Function Name  : UARTC_QueryChar
* Description    : 读取 读缓冲区数据
* parameters     : *ch 读取的数据
* Return         : 0：缓冲区为空         1:正常
*******************************************************************************/
uint8_t UARTC_QueryChar(uint8_t *ch)
{
    if (UARTCBuf.RxCount == 0)
        return 0;

    *ch = UARTCBuf.RxBuf[UARTCBuf.RxHeadPoint];
    UARTCBuf.RxHeadPoint++;
    if (UARTCBuf.RxHeadPoint >= UARTC_RXBUFSIZE) UARTCBuf.RxHeadPoint = 0;

    DI();
    UARTCBuf.RxCount--;
    EI();
    return 1;

}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : USART3 中断
* parameters     : None
* Return         : None
*******************************************************************************/
//void USART3_IRQHandler(void)
//{
//    if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET)
//    {
//        UARTC_WriteRxBuf (USART3->DR);
//    }
//
//}
//

//void UART5_IRQHandler(void)
//{
//    if(USART_GetFlagStatus(UART5, USART_FLAG_RXNE) != RESET)
//    {
//        UARTC_WriteRxBuf (UART5->DR);
//    }
//    
//}
    
/**************************************************************END OF FILE****/
