/**
 ******************************************************************************
 * @file    zigbee_core_wb.c
 * @author  MCD Application Team
 * @brief   Contains STM32WB specificities requested to control the Zigbee stack
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

#include "zigbee_errors.h"
#include "zigbee_config.h"
#include "zigbee_types.h"
#include "zigbee_interface.h"
#include "zigbee_core.h"
#include "tl_zigbee_hci.h"

#include "dbg_trace.h"
#include "stm_logging.h"
#include "stm32wbxx_core_interface_def.h"



ScanChangedCbType scanChangedCb = NULL;


#if defined ( __CC_ARM ) || defined(__GNUC__)  /* KEIL MDK-ARM or SW4STM32 */
/* Use local buffer to avoid non-aligned accesses with LDM instruction with KEIL */
MAC_802_15_4_Notification_t notification_local;
#endif /* ( __CC_ARM ) || defined(__GNUC__)  */

/**
* @brief  This function is used to handle a MAC MLME Scan REQ as described
 *         in IEEE Std 802.15.4™-2011 standard
 *
 * @param  MAC_scanReq_t ref
 * @retval MAC_Status_t
*/


ZIGBEE_Status_t Zigbee_ScanReq( const  ZIGBEE_scanReq_t * pScanReq )
{
   ZIGBEE_Status_t status = MAC_NOT_IMPLEMENTED_STATUS;
   
    Pre_ZigbeeCmdProcessing();

  /* prepare buffer */
   Zigbee_Cmd_Request_t * p_ot_req = ZIGBEE_Get_OTCmdPayloadBuffer();
   
     p_ot_req->ID = MSG_M4TOM0_ZIGBEE_SCAN;

   p_ot_req->Size=1;
   p_ot_req->Data[0] = (uint32_t)pScanReq;

   ZIGBEE_CmdTransfer();
   
      p_ot_req = ZIGBEE_Get_OTCmdRspPayloadBuffer();
  
   return (ZIGBEE_Status_t) p_ot_req->Data[0];

}



void Zigbee_RegisterScanCb(ScanChangedCbType  scanResponseCallback )
{
    scanChangedCb = scanResponseCallback;
}
/**
  * @brief  This function is used to manage all the callbacks used by the
  *         OpenThread interface. These callbacks are used for example to
  *         notify the application as soon as the state of a device has been
  *         modified.
  *
  *         Important Note: This function must be called each time a message
  *         is sent from the M0 to the M4.
  *
  * @param  None
  * @retval None
  */

HAL_StatusTypeDef Zigbee_CallBackProcessing(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    /* Get pointer on received event buffer from M0 */
    Zigbee_Cmd_Request_t* p_notification = ZIGBEE_Get_NotificationPayloadBuffer();

    switch(p_notification->ID)
    {
    case MSG_M0TOM4_ZIGBEE_SCAN_NOTIF:
        if (scanChangedCb != NULL)
        {
            scanChangedCb((ZIGBEE_scanReq_t * ) p_notification->Data[0]);
        }
        break;
    default:
        status = HAL_ERROR;
        break;
    }

    TL_ZIGBEE_SendAckAfterAppliNotifFromM0();
    return status;

}


 



