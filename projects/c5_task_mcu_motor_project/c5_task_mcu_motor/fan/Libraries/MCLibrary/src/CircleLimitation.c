/******************************************************************************
* File Name         :  CircleLimitation.c
* Author            :  陈雅枫 
* Version           :  1.0 
* Date              :  19/01/14
* Description       :  限制电压矢量不大于32767，避免PWM值溢出                         
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



CLM_t CLMpool[NBR_OF_MOTORS];



CLMParams_t CircleLimitationParams =
{
  MAX_MODULE,                           /*!< Circle limitation maximum allowed module */
  MMITABLE,                             /*!< Circle limitation table */
  START_INDEX,                          /*!< Circle limitation table indexing 
                                           start */
};


/*******************************************************************************
* Function Name  : CLM_Init
* Description    : 变量初始化
* parameters     : None
* Return         : None
*******************************************************************************/
void CLM_Init(void)
{
    oCLM[0] = &CLMpool[0];    
    oCLM[0]->pParams = &CircleLimitationParams;

	#ifdef DOUBLE_DRIVER
    oCLM[1] = &CLMpool[1];
    oCLM[1]->pParams = &CircleLimitationParams;    
    #endif
}


/*******************************************************************************
* Function Name  : Circle_Limitation
* Description    : 限制电压矢量不大于32767，通过同比例缩小,使得失真最小。
* parameters     : this  CLMpool结构指针      Vqd: FOC 变换后的 Vq,Vd。
* Return         : None
*******************************************************************************/
Volt_Components Circle_Limitation(pCLM_t this, Volt_Components Vqd)
{
  uint32_t uw_temp;
  int32_t sw_temp; 
  Volt_Components Local_Vqd=Vqd;
  pCLMParams_t pParams = this->pParams; 
  
  sw_temp =(int32_t)(Vqd.qV_Component1) * Vqd.qV_Component1 + 
    (int32_t)(Vqd.qV_Component2) * Vqd.qV_Component2;
  
  uw_temp =(uint32_t) sw_temp;
  /* uw_temp min value 0, max value 2*32767*32767 */
  
  if (uw_temp > (uint32_t)(pParams->hMaxModule) * pParams->hMaxModule) 
  {
    uint16_t hTable_Element;
    
    uw_temp /= (uint32_t)(16777216); 
    /* wtemp min value pParams->bStart_index, max value 127 */
    uw_temp -= pParams->bStart_index;
    
    /* uw_temp min value 0, max value 127 - pParams->bStart_index */   
    hTable_Element = pParams->hCircle_limit_table[(uint8_t)uw_temp];
    
    sw_temp = Vqd.qV_Component1 * (int32_t)hTable_Element; 
    Local_Vqd.qV_Component1 = (int16_t)(sw_temp/32768);  
    
    sw_temp = Vqd.qV_Component2 * (int32_t)(hTable_Element); 
    Local_Vqd.qV_Component2 = (int16_t)(sw_temp/32768);  
  }
  
  return(Local_Vqd);
}

/**************************************************************END OF FILE****/
