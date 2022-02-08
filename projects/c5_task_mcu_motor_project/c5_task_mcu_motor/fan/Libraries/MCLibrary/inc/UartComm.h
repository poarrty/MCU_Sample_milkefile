/******************************************************************************
* File Name         :  UartComm.h
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UARTCOMM_H
#define __UARTCOMM_H


//#include "Type.h"
//#include "MCTasks.h"



#define  _PUTCHAR_EN   


#define UARTC_RXBUFSIZE  (16)
#define UARTC_TXBUFSIZE  (32)
//#define	CR		(0x0D)
//#define	LF		(0x0A)



typedef struct {
	uint8_t RxBuf[UARTC_RXBUFSIZE];
	uint8_t TxBuf[UARTC_TXBUFSIZE];

	uint8_t RxHeadPoint;
	uint8_t RxTailPoint;

	uint8_t TxHeadPoint;
	uint8_t TxTailPoint;

	uint8_t RxCount;
	volatile uint8_t TxCount;

}UartC_t;

uint8_t UARTC_QueryChar(uint8_t *ch);
void UARTC_Init(void);
void UARTC_HandleTxd(void);
//void USART1_IRQHandler(void);

void UARTC_DEBUG(void);


uint8_t UARTC_PutChar(uint8_t ch);


#endif 

/**************************************************************END OF FILE****/
