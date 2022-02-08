/**
  ******************************************************************************
  * @file    stm32wbxx_core_interface_def.h
  * @author  MCD Application Team
  * @brief   This file contains all the defines and structures used for the
  *          communication between the two core M0 and M4.
  *          This file is shared between the code running on M4 and the code
  *          running on M0.
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WBxx_CORE_INTERFACE_DEF_H
#define STM32WBxx_CORE_INTERFACE_DEF_H
 
#ifdef __cplusplus
extern "C" {
#endif

#include "zigbee_types.h"  
  
/* Structure of the messages exchanged between M0 and M4 */
#define OT_CMD_BUFFER_SIZE 20U

typedef struct
{
  uint32_t  ID;
  uint32_t  Size;
  uint32_t  Data[OT_CMD_BUFFER_SIZE];
}Zigbee_Cmd_Request_t;


/* List of messages sent by the M4 to the M0 */
typedef enum
{
  /* SCAN */
  MSG_M4TOM0_ZIGBEE_SCAN = 0x5,
  MSG_M0TOM4_ZIGBEE_SCAN_NOTIF = 0x15,
  MSG_M0TOM4_TRACE_SEND
} MsgId_M0toM4_Enum_t;



/* List of errors returned by the interface  */
typedef enum
{
  ERR_INTERFACE_IPCC_INIT_FAIL = 100U,
  ERR_INTERFACE_IPCC_REMOTE_FAIL = 101U,
  ERR_INTERFACE_IPCC_SEND_ACK_TO_M0 = 102U,
} Error_Interface_Id_Enum_t;


typedef struct {
    /*! The type of scan to be performed */
    uint8_t scan_type;
    /*! The time spent on scanning each channel */
    uint8_t scan_duration;
    /*! Channel page on which to perform the scan */
    uint8_t channel_page;
} ZIGBEE_scanReq_t;


typedef void (*ScanChangedCbType)(ZIGBEE_scanReq_t *scanNotif);


ZIGBEE_Status_t Zigbee_ScanReq( const  ZIGBEE_scanReq_t * pScanReq );
void Zigbee_RegisterScanCb(ScanChangedCbType  scanResponseCallback );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STM32WBxx_CORE_INTERFACE_DEF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
