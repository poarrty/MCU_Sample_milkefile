/**
 ******************************************************************************
 * @file    Zigbee_interface.h
 * @author
 * @brief   Header for zigbee interface API (M4 side)
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
#ifndef _ZIGBEE_INTERFACE
#define _ZIGBEE_INTERFACE

/* Includes ------------------------------------------------------------------*/
#include "zigbee_types.h"
#include "stm32wbxx_core_interface_def.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* general */


/* Request */

ZIGBEE_Status_t Zigbee_ScanReq( const  ZIGBEE_scanReq_t * pScanReq );



#endif /* _ZIGBEE_INTERFACE */
