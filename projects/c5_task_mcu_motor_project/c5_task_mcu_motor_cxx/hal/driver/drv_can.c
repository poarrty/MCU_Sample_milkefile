/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-05     Xeon Xu      the first version
 * 2019-01-22     YLZ          port from stm324xx-HAL to bsp stm3210x-HAL
 * 2019-02-19     YLZ          add support EXTID RTR Frame. modify send, recv functions.
 *                             fix bug.port to BSP [stm32]
 * 2019-03-27     YLZ          support double can channels, support stm32F4xx (only Legacy mode).
 * 2019-06-17     YLZ          port to new STM32F1xx HAL V1.1.3.
 * 2021-02-02     YuZhe XU     fix bug in filter config
 */
#include "drv_can.h"
#include "stm32f1xx_it.h"
#define LOG_TAG "drv_can"
#include "elog.h"

#ifdef BSP_USING_CAN

extern CAN_HandleTypeDef hcan;
#define canHandle1 hcan
#define canHandle2 hcan

#define used_cubemx_init

/* attention !!! baud calculation example: Tclk / ((ss + bs1 + bs2) * brp)  36 / ((1 + 8 + 3) * 3) = 1MHz*/
#if defined(SOC_SERIES_STM32F1) /* APB1 36MHz(max) */
#ifdef SOC_GD32F303RCT6
static const struct stm32_baud_rate_tab can_baud_rate_tab[] = {{CAN1MBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_5TQ | 6)},
                                                               {CAN800kBaud, (CAN_SJW_1TQ | CAN_BS1_8TQ | CAN_BS2_6TQ | 5)},
                                                               {CAN500kBaud, (CAN_SJW_1TQ | CAN_BS1_9TQ | CAN_BS2_8TQ | 4)},  // 72M div2--36M
                                                               //{CAN500kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ  | CAN_BS2_4TQ | 12)},
                                                               {CAN250kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 24)},
                                                               {CAN125kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 48)},
                                                               {CAN100kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 60)},
                                                               {CAN50kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 120)},
                                                               {CAN20kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 300)},
                                                               {CAN10kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_4TQ | 600)}};
#else
static const struct stm32_baud_rate_tab can_baud_rate_tab[] = {
    {CAN1MBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 3)},    {CAN800kBaud, (CAN_SJW_1TQ | CAN_BS1_4TQ | CAN_BS2_4TQ | 5)},
    {CAN500kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 6)},  {CAN250kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 12)},
    {CAN125kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 24)}, {CAN100kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 30)},
    {CAN50kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 60)},  {CAN20kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 150)},
    {CAN10kBaud, (CAN_SJW_1TQ | CAN_BS1_5TQ | CAN_BS2_6TQ | 300)}};
#endif
#elif defined(SOC_SERIES_STM32F4) /* APB1 42MHz(max) */
static const struct stm32_baud_rate_tab can_baud_rate_tab[] = {
    {CAN1MBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 3)},    {CAN800kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_6TQ | 4)},
    {CAN500kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 6)},  {CAN250kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 12)},
    {CAN125kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 24)}, {CAN100kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 30)},
    {CAN50kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 60)},  {CAN20kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 150)},
    {CAN10kBaud, (CAN_SJW_1TQ | CAN_BS1_6TQ | CAN_BS2_7TQ | 300)}};
#elif defined(SOC_SERIES_STM32F7) /* APB1 54MHz(max) */
static const struct stm32_baud_rate_tab can_baud_rate_tab[] = {
    {CAN1MBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 3)},    {CAN800kBaud, (CAN_SJW_2TQ | CAN_BS1_9TQ | CAN_BS2_7TQ | 4)},
    {CAN500kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 6)},  {CAN250kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 12)},
    {CAN125kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 24)}, {CAN100kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 30)},
    {CAN50kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 60)},  {CAN20kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 150)},
    {CAN10kBaud, (CAN_SJW_2TQ | CAN_BS1_10TQ | CAN_BS2_7TQ | 300)}};
#elif defined(SOC_SERIES_STM32L4) /* APB1 80MHz(max) */
static const struct stm32_baud_rate_tab can_baud_rate_tab[] = {
    {CAN1MBaud, (CAN_SJW_2TQ | CAN_BS1_5TQ | CAN_BS2_2TQ | 10)},    {CAN800kBaud, (CAN_SJW_2TQ | CAN_BS1_14TQ | CAN_BS2_5TQ | 5)},
    {CAN500kBaud, (CAN_SJW_2TQ | CAN_BS1_7TQ | CAN_BS2_2TQ | 16)},  {CAN250kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 20)},
    {CAN125kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 40)}, {CAN100kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 50)},
    {CAN50kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 100)}, {CAN20kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 250)},
    {CAN10kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | 500)}};
#endif

#ifdef BSP_USING_CAN1
struct stm32_can drv_can1 = {
    .name               = "can1",
    .CanHandle.Instance = CAN1,
};
#endif

#ifdef BSP_USING_CAN2
struct stm32_can drv_can2 = {
    "can2",
    .CanHandle.Instance = CAN2,
};
#endif

/**
 * rt_container_of - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define rt_container_of(ptr, type, member) ((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#define rt_list_entry(node, type, member) rt_container_of(node, type, member)

rt_inline void rt_list_init(rt_list_t *l) {
    l->next = l->prev = l;
}

rt_inline int rt_list_isempty(const rt_list_t *l) {
    return l->next == l;
}

rt_inline void rt_list_remove(rt_list_t *n) {
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

rt_inline void rt_list_insert_after(rt_list_t *l, rt_list_t *n) {
    l->next->prev = n;
    n->next       = l->next;

    l->next = n;
    n->prev = l;
}

rt_inline void rt_list_insert_before(rt_list_t *l, rt_list_t *n) {
    l->prev->next = n;
    n->prev       = l->prev;

    l->prev = n;
    n->next = l;
}

rt_inline unsigned int rt_list_len(const rt_list_t *l) {
    unsigned int     len = 0;
    const rt_list_t *p   = l;

    while (p->next != l) {
        p = p->next;
        len++;
    }

    return len;
}

rt_uint32_t get_can_baud_index(rt_uint32_t baud) {
    rt_uint32_t len, index;

    len = sizeof(can_baud_rate_tab) / sizeof(can_baud_rate_tab[0]);

    for (index = 0; index < len; index++) {
        if (can_baud_rate_tab[index].baud_rate == baud) {
            return index;
        }
    }

    return 0; /* default baud is CAN1MBaud */
}

static rt_err_t _can_config(struct rt_can_device *can, struct can_configure *cfg) {
    struct stm32_can *drv_can;

    RT_ASSERT(can);
    RT_ASSERT(cfg);
    drv_can = (struct stm32_can *) can->parent.user_data;
    RT_ASSERT(drv_can);

#ifndef used_cubemx_init
    rt_uint32_t baud_index;

    drv_can->CanHandle.Init.TimeTriggeredMode    = DISABLE;
    drv_can->CanHandle.Init.AutoBusOff           = ENABLE;
    drv_can->CanHandle.Init.AutoWakeUp           = DISABLE;
    drv_can->CanHandle.Init.AutoRetransmission   = DISABLE;
    drv_can->CanHandle.Init.ReceiveFifoLocked    = DISABLE;
    drv_can->CanHandle.Init.TransmitFifoPriority = ENABLE;

    switch (cfg->mode) {
        case RT_CAN_MODE_NORMAL:
            drv_can->CanHandle.Init.Mode = CAN_MODE_NORMAL;
            break;

        case RT_CAN_MODE_LISEN:
            drv_can->CanHandle.Init.Mode = CAN_MODE_SILENT;
            break;

        case RT_CAN_MODE_LOOPBACK:
            drv_can->CanHandle.Init.Mode = CAN_MODE_LOOPBACK;
            break;

        case RT_CAN_MODE_LOOPBACKANLISEN:
            drv_can->CanHandle.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
            break;
    }

    baud_index                            = get_can_baud_index(cfg->baud_rate);
    drv_can->CanHandle.Init.SyncJumpWidth = BAUD_DATA(SJW, baud_index);
    drv_can->CanHandle.Init.TimeSeg1      = BAUD_DATA(BS1, baud_index);
    drv_can->CanHandle.Init.TimeSeg2      = BAUD_DATA(BS2, baud_index);
    drv_can->CanHandle.Init.Prescaler     = BAUD_DATA(RRESCL, baud_index);

    /* init can */
    if (HAL_CAN_Init(&drv_can->CanHandle) != HAL_OK) {
        return -RT_ERROR;
    }

#endif

    /* default filter config */
    HAL_CAN_ConfigFilter(&drv_can->CanHandle, &drv_can->FilterConfig);
    /* can start */
    HAL_CAN_Start(&drv_can->CanHandle);

    return RT_EOK;
}

static rt_err_t _can_control(struct rt_can_device *can, int cmd, void *arg) {
    rt_uint32_t                  argval;
    struct stm32_can *           drv_can;
    struct rt_can_filter_config *filter_cfg;

    RT_ASSERT(can != RT_NULL);
    drv_can = (struct stm32_can *) can->parent.user_data;
    RT_ASSERT(drv_can != RT_NULL);

    switch (cmd) {
        case RT_DEVICE_CTRL_CLR_INT:
            argval = (rt_uint32_t) arg;

            if (argval == RT_DEVICE_FLAG_INT_RX) {
                if (CAN1 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
                    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
                }

#ifdef CAN2

                if (CAN2 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
                    HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
                }

#endif
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_FULL);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_OVERRUN);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_MSG_PENDING);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_FULL);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_OVERRUN);
            } else if (argval == RT_DEVICE_CAN_INT_ERR) {
                if (CAN1 == drv_can->CanHandle.Instance) {
                    NVIC_DisableIRQ(CAN1_SCE_IRQn);
                }

#ifdef CAN2

                if (CAN2 == drv_can->CanHandle.Instance) {
                    NVIC_DisableIRQ(CAN2_SCE_IRQn);
                }

#endif
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR_WARNING);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR_PASSIVE);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_BUSOFF);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_LAST_ERROR_CODE);
                __HAL_CAN_DISABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR);
            }

            break;

        case RT_DEVICE_CTRL_SET_INT:
            argval = (rt_uint32_t) arg;

            if (argval == RT_DEVICE_FLAG_INT_RX) {
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_FULL);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO0_OVERRUN);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_MSG_PENDING);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_FULL);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_RX_FIFO1_OVERRUN);

                if (CAN1 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
                    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
                }

#ifdef CAN2

                if (CAN2 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
                    HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
                }

#endif
            } else if (argval == RT_DEVICE_CAN_INT_ERR) {
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR_WARNING);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR_PASSIVE);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_BUSOFF);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_LAST_ERROR_CODE);
                __HAL_CAN_ENABLE_IT(&drv_can->CanHandle, CAN_IT_ERROR);

                if (CAN1 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
                }

#ifdef CAN2

                if (CAN2 == drv_can->CanHandle.Instance) {
                    HAL_NVIC_SetPriority(CAN2_SCE_IRQn, 1, 0);
                    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
                }

#endif
            }

            break;

        case RT_CAN_CMD_SET_FILTER:
            if (RT_NULL == arg) {
                /* default filter config */
                HAL_CAN_ConfigFilter(&drv_can->CanHandle, &drv_can->FilterConfig);
            } else {
                filter_cfg = (struct rt_can_filter_config *) arg;

                /* get default filter */
                for (int i = 0; i < filter_cfg->count; i++) {
                    drv_can->FilterConfig.FilterBank   = filter_cfg->items[i].hdr;
                    drv_can->FilterConfig.FilterIdHigh = (filter_cfg->items[i].id >> 13) & 0xFFFF;
                    drv_can->FilterConfig.FilterIdLow =
                        ((filter_cfg->items[i].id << 3) | (filter_cfg->items[i].ide << 2) | (filter_cfg->items[i].rtr << 1)) & 0xFFFF;
                    drv_can->FilterConfig.FilterMaskIdHigh = (filter_cfg->items[i].mask >> 16) & 0xFFFF;
                    drv_can->FilterConfig.FilterMaskIdLow  = filter_cfg->items[i].mask & 0xFFFF;
                    drv_can->FilterConfig.FilterMode       = filter_cfg->items[i].mode;
                    /* Filter conf */
                    HAL_CAN_ConfigFilter(&drv_can->CanHandle, &drv_can->FilterConfig);
                }
            }

            break;

        case RT_CAN_CMD_SET_MODE:
            argval = (rt_uint32_t) arg;

            if (argval != RT_CAN_MODE_NORMAL && argval != RT_CAN_MODE_LISEN && argval != RT_CAN_MODE_LOOPBACK &&
                argval != RT_CAN_MODE_LOOPBACKANLISEN) {
                return -RT_ERROR;
            }

            if (argval != drv_can->device.config.mode) {
                drv_can->device.config.mode = argval;
                return _can_config(&drv_can->device, &drv_can->device.config);
            }

            break;

        case RT_CAN_CMD_SET_BAUD:
            argval = (rt_uint32_t) arg;

            if (argval != CAN1MBaud && argval != CAN800kBaud && argval != CAN500kBaud && argval != CAN250kBaud && argval != CAN125kBaud &&
                argval != CAN100kBaud && argval != CAN50kBaud && argval != CAN20kBaud && argval != CAN10kBaud) {
                return -RT_ERROR;
            }

            if (argval != drv_can->device.config.baud_rate) {
                drv_can->device.config.baud_rate = argval;
                return _can_config(&drv_can->device, &drv_can->device.config);
            }

            break;

        case RT_CAN_CMD_SET_PRIV:
            argval = (rt_uint32_t) arg;

            if (argval != RT_CAN_MODE_PRIV && argval != RT_CAN_MODE_NOPRIV) {
                return -RT_ERROR;
            }

            if (argval != drv_can->device.config.privmode) {
                drv_can->device.config.privmode = argval;
                return _can_config(&drv_can->device, &drv_can->device.config);
            }

            break;

        case RT_CAN_CMD_GET_STATUS: {
            rt_uint32_t errtype;
            errtype                            = drv_can->CanHandle.Instance->ESR;
            drv_can->device.status.rcverrcnt   = errtype >> 24;
            drv_can->device.status.snderrcnt   = (errtype >> 16 & 0xFF);
            drv_can->device.status.lasterrtype = errtype & 0x70;
            drv_can->device.status.errcode     = errtype & 0x07;

            memcpy(arg, &drv_can->device.status, sizeof(drv_can->device.status));
        } break;
    }

    return RT_EOK;
}

static int _can_sendmsg(struct rt_can_device *can, const void *buf, rt_uint32_t box_num) {
    CAN_HandleTypeDef *hcan;
    hcan                          = &((struct stm32_can *) can->parent.user_data)->CanHandle;
    struct rt_can_msg * pmsg      = (struct rt_can_msg *) buf;
    CAN_TxHeaderTypeDef txheader  = {0};
    uint8_t             data[8]   = {0};
    uint32_t            TxMailbox = 0;

    if (RT_CAN_STDID == pmsg->ide) {
        txheader.IDE = CAN_ID_STD;
        RT_ASSERT(IS_CAN_STDID(pmsg->id));
        txheader.StdId = pmsg->id;
    } else {
        txheader.IDE = CAN_ID_EXT;
        RT_ASSERT(IS_CAN_EXTID(pmsg->id));
        txheader.ExtId = pmsg->id;
    }

    if (RT_CAN_DTR == pmsg->rtr) {
        txheader.RTR = CAN_RTR_DATA;
    } else {
        txheader.RTR = CAN_RTR_REMOTE;
    }

    txheader.DLC = pmsg->len;
    memcpy(data, pmsg->data, pmsg->len);

    while (!HAL_CAN_GetTxMailboxesFreeLevel(hcan)) {
        HAL_Delay(1);
    }

    if (HAL_CAN_AddTxMessage(hcan, &txheader, data, &TxMailbox) != HAL_OK) {
        return -RT_ERROR;
    }

    return box_num;
}

static int _can_recvmsg(struct rt_can_device *can, void *buf, rt_uint32_t fifo) {
    HAL_StatusTypeDef   status;
    CAN_HandleTypeDef * hcan;
    struct rt_can_msg * pmsg;
    CAN_RxHeaderTypeDef rxheader = {0};

    RT_ASSERT(can);

    hcan = &((struct stm32_can *) can->parent.user_data)->CanHandle;
    pmsg = (struct rt_can_msg *) buf;

    /* get data */
    status = HAL_CAN_GetRxMessage(hcan, fifo, &rxheader, pmsg->data);

    if (HAL_OK != status) {
        return -RT_ERROR;
    }

    /* get id */
    if (CAN_ID_STD == rxheader.IDE) {
        pmsg->ide = RT_CAN_STDID;
        pmsg->id  = rxheader.StdId;
    } else {
        pmsg->ide = RT_CAN_EXTID;
        pmsg->id  = rxheader.ExtId;
    }

    /* get type */
    if (CAN_RTR_DATA == rxheader.RTR) {
        pmsg->rtr = RT_CAN_DTR;
    } else {
        pmsg->rtr = RT_CAN_RTR;
    }

    /* get len */
    pmsg->len = rxheader.DLC;

    /* get hdr */
    if (hcan->Instance == CAN1) {
        pmsg->hdr = (rxheader.FilterMatchIndex + 1) >> 1;
    }
#ifdef CAN2
    else if (hcan->Instance == CAN2) {
        pmsg->hdr = (rxheader.FilterMatchIndex >> 1) + 14;
    }

#endif

    return RT_EOK;
}

const struct rt_can_ops _can_ops = {
    _can_config,
    _can_control,
    _can_sendmsg,
    _can_recvmsg,
};

int _can_int_rx(struct rt_can_device *can, struct rt_can_msg *data, int msgs) {
    int                    size;
    struct rt_can_rx_fifo *rx_fifo;
    RT_ASSERT(can != RT_NULL);
    size = msgs;

    rx_fifo = (struct rt_can_rx_fifo *) can->can_rx;
    RT_ASSERT(rx_fifo != RT_NULL);

    /* read from software FIFO */
    while (msgs) {
#ifdef RT_CAN_USING_HDR
        rt_int8_t hdr;
#endif /*RT_CAN_USING_HDR*/
        struct rt_can_msg_list *listmsg = RT_NULL;

        /* disable interrupt */
        taskDISABLE_INTERRUPTS();
#ifdef RT_CAN_USING_HDR
        hdr = data->hdr;

        if (hdr >= 0 && can->hdr && hdr < can->config.maxhdr && !rt_list_isempty(&can->hdr[hdr].list)) {
            listmsg = rt_list_entry(can->hdr[hdr].list.next, struct rt_can_msg_list, hdrlist);
            rt_list_remove(&listmsg->list);
            rt_list_remove(&listmsg->hdrlist);

            if (can->hdr[hdr].msgs) {
                can->hdr[hdr].msgs--;
            }

            listmsg->owner = RT_NULL;
        } else if (hdr == -1)
#endif /*RT_CAN_USING_HDR*/
        {
            if (!rt_list_isempty(&rx_fifo->uselist)) {
                listmsg = rt_list_entry(rx_fifo->uselist.next, struct rt_can_msg_list, list);
                rt_list_remove(&listmsg->list);
#ifdef RT_CAN_USING_HDR
                rt_list_remove(&listmsg->hdrlist);

                if (listmsg->owner != RT_NULL && listmsg->owner->msgs) {
                    listmsg->owner->msgs--;
                }

                listmsg->owner = RT_NULL;
#endif /*RT_CAN_USING_HDR*/
            } else {
                /* no data, enable interrupt and break out */
                taskENABLE_INTERRUPTS();
                break;
            }
        }

        /* enable interrupt */
        taskENABLE_INTERRUPTS();

        if (listmsg != RT_NULL) {
            // log_d("listmsg->data.id:%x", listmsg->data.id);
            // log_d("listmsg->data.ide:%x", listmsg->data.ide);
            // log_d("listmsg->data.rtr:%x", listmsg->data.rtr);
            // log_d("listmsg->data.rsv:%x", listmsg->data.rsv);
            // log_d("listmsg->data.len:%x", listmsg->data.len);
            // log_d("listmsg->data.hdr:%x", listmsg->data.hdr);
            // log_d("listmsg->data.data[0]:%x", listmsg->data.data[0]);
            // log_d("listmsg->data.data[1]:%x", listmsg->data.data[1]);
            // log_d("listmsg->data.data[2]:%x", listmsg->data.data[2]);
            // log_d("listmsg->data.data[3]:%x", listmsg->data.data[3]);

            memcpy(data, &listmsg->data, sizeof(struct rt_can_msg));

            taskDISABLE_INTERRUPTS();
            rt_list_insert_before(&rx_fifo->freelist, &listmsg->list);
            rx_fifo->freenumbers++;
            RT_ASSERT(rx_fifo->freenumbers <= can->config.msgboxsz);
            taskENABLE_INTERRUPTS();

            listmsg = RT_NULL;
        } else {
            break;
        }

        data++;
        msgs -= sizeof(struct rt_can_msg);
    }

    return (size - msgs);
}

static void _can_rx_isr(struct rt_can_device *can, rt_uint32_t fifo) {
    CAN_HandleTypeDef *hcan;
    RT_ASSERT(can);
    hcan = &((struct stm32_can *) can->parent.user_data)->CanHandle;

    switch (fifo) {
        case CAN_RX_FIFO0:

            /* save to user list */
            if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO0_MSG_PENDING)) {
                rt_hw_can_isr(can, RT_CAN_EVENT_RX_IND | fifo << 8);
            }

            /* Check FULL flag for FIFO0 */
            if (__HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FF0) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO0_FULL)) {
                /* Clear FIFO0 FULL Flag */
                __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FF0);
            }

            /* Check Overrun flag for FIFO0 */
            if (__HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FOV0) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO0_OVERRUN)) {
                /* Clear FIFO0 Overrun Flag */
                __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV0);
                rt_hw_can_isr(can, RT_CAN_EVENT_RXOF_IND | fifo << 8);
            }

            break;

        case CAN_RX_FIFO1:

            /* save to user list */
            if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO1) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO1_MSG_PENDING)) {
                rt_hw_can_isr(can, RT_CAN_EVENT_RX_IND | fifo << 8);
            }

            /* Check FULL flag for FIFO1 */
            if (__HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FF1) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO1_FULL)) {
                /* Clear FIFO1 FULL Flag */
                __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FF1);
            }

            /* Check Overrun flag for FIFO1 */
            if (__HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FOV1) && __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_RX_FIFO1_OVERRUN)) {
                /* Clear FIFO1 Overrun Flag */
                __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV1);
                rt_hw_can_isr(can, RT_CAN_EVENT_RXOF_IND | fifo << 8);
            }

            break;
    }
}

/* ISR for can interrupt */
void rt_hw_can_isr(struct rt_can_device *can, int event) {
    switch (event & 0xff) {
        case RT_CAN_EVENT_RXOF_IND: {
            portDISABLE_INTERRUPTS();
            can->status.dropedrcvpkg++;
            portENABLE_INTERRUPTS();
        }

        case RT_CAN_EVENT_RX_IND: {
            struct rt_can_msg       tmpmsg;
            struct rt_can_rx_fifo * rx_fifo;
            struct rt_can_msg_list *listmsg = RT_NULL;
#ifdef RT_CAN_USING_HDR
            rt_int8_t hdr;
#endif
            int         ch = -1;
            rt_uint32_t no;

            rx_fifo = (struct rt_can_rx_fifo *) can->can_rx;
            RT_ASSERT(rx_fifo != RT_NULL);
            /* interrupt mode receive */
            // RT_ASSERT(can->parent.open_flag & RT_DEVICE_FLAG_INT_RX);

            no = event >> 8;
            ch = can->ops->recvmsg(can, &tmpmsg, no);

            if (ch == -1) {
                break;
            }

            /* disable interrupt */
            portDISABLE_INTERRUPTS();
            can->status.rcvpkg++;
            can->status.rcvchange = 1;

            if (!rt_list_isempty(&rx_fifo->freelist)) {
                listmsg = rt_list_entry(rx_fifo->freelist.next, struct rt_can_msg_list, list);
                rt_list_remove(&listmsg->list);
#ifdef RT_CAN_USING_HDR
                rt_list_remove(&listmsg->hdrlist);

                if (listmsg->owner != RT_NULL && listmsg->owner->msgs) {
                    listmsg->owner->msgs--;
                }

                listmsg->owner = RT_NULL;
#endif /*RT_CAN_USING_HDR*/
                RT_ASSERT(rx_fifo->freenumbers > 0);
                rx_fifo->freenumbers--;
            } else if (!rt_list_isempty(&rx_fifo->uselist)) {
                listmsg = rt_list_entry(rx_fifo->uselist.next, struct rt_can_msg_list, list);
                can->status.dropedrcvpkg++;
                rt_list_remove(&listmsg->list);
#ifdef RT_CAN_USING_HDR
                rt_list_remove(&listmsg->hdrlist);

                if (listmsg->owner != RT_NULL && listmsg->owner->msgs) {
                    listmsg->owner->msgs--;
                }

                listmsg->owner = RT_NULL;
#endif
            }

            /* enable interrupt */
            portENABLE_INTERRUPTS();

            if (listmsg != RT_NULL) {
                memcpy(&listmsg->data, &tmpmsg, sizeof(struct rt_can_msg));
                portDISABLE_INTERRUPTS();
                rt_list_insert_before(&rx_fifo->uselist, &listmsg->list);
#ifdef RT_CAN_USING_HDR
                hdr = tmpmsg.hdr;

                if (can->hdr != RT_NULL) {
                    RT_ASSERT(hdr < can->config.maxhdr && hdr >= 0);

                    if (can->hdr[hdr].connected) {
                        rt_list_insert_before(&can->hdr[hdr].list, &listmsg->hdrlist);
                        listmsg->owner = &can->hdr[hdr];
                        can->hdr[hdr].msgs++;
                    }
                }

#endif
                portENABLE_INTERRUPTS();
            }

            /* invoke callback */
#ifdef RT_CAN_USING_HDR

            if (can->hdr != RT_NULL && can->hdr[hdr].connected && can->hdr[hdr].filter.ind) {
                rt_size_t rx_length;
                RT_ASSERT(hdr < can->config.maxhdr && hdr >= 0);

                portDISABLE_INTERRUPTS();
                rx_length = can->hdr[hdr].msgs * sizeof(struct rt_can_msg);
                portENABLE_INTERRUPTS();

                if (rx_length) {
                    can->hdr[hdr].filter.ind(&can->parent, can->hdr[hdr].filter.args, hdr, rx_length);
                }
            } else
#endif
            {
                if (can->parent.rx_indicate != RT_NULL) {
                    rt_size_t rx_length;

                    portDISABLE_INTERRUPTS();
                    /* get rx length */
                    rx_length = rt_list_len(&rx_fifo->uselist) * sizeof(struct rt_can_msg);
                    portENABLE_INTERRUPTS();

                    if (rx_length) {
                        can->parent.rx_indicate(&can->parent, rx_length);
                    }
                }
            }

            break;
        }

        case RT_CAN_EVENT_TX_DONE:
        case RT_CAN_EVENT_TX_FAIL:
            break;
    }
}

#ifdef BSP_USING_CAN1
/**
 * @brief This function handles CAN1 RX0 interrupts.
 */
void CAN1_RX0_IRQHandler(void) {
    // rt_interrupt_enter();
    _can_rx_isr(&drv_can1.device, CAN_RX_FIFO0);
    // rt_interrupt_leave();
}

/**
 * @brief This function handles CAN1 RX1 interrupts.
 */
void CAN1_RX1_IRQHandler(void) {
    // rt_interrupt_enter();
    _can_rx_isr(&drv_can1.device, CAN_RX_FIFO1);
    // rt_interrupt_leave();
}

/**
 * @brief This function handles CAN1 SCE interrupts.
 */
void CAN1_SCE_IRQHandler(void) {
    rt_uint32_t        errtype;
    CAN_HandleTypeDef *hcan;

    hcan    = &drv_can1.CanHandle;
    errtype = hcan->Instance->ESR;

    // rt_interrupt_enter();
    HAL_CAN_IRQHandler(hcan);

    switch ((errtype & 0x70) >> 4) {
        case RT_CAN_BUS_BIT_PAD_ERR:
            drv_can1.device.status.bitpaderrcnt++;
            break;

        case RT_CAN_BUS_FORMAT_ERR:
            drv_can1.device.status.formaterrcnt++;
            break;

        case RT_CAN_BUS_ACK_ERR: /* attention !!! test ack err's unit is transmit unit */
            drv_can1.device.status.ackerrcnt++;

            if (!READ_BIT(drv_can1.CanHandle.Instance->TSR, CAN_FLAG_TXOK0)) {
                rt_hw_can_isr(&drv_can1.device, RT_CAN_EVENT_TX_FAIL | 0 << 8);
            } else if (!READ_BIT(drv_can1.CanHandle.Instance->TSR, CAN_FLAG_TXOK1)) {
                rt_hw_can_isr(&drv_can1.device, RT_CAN_EVENT_TX_FAIL | 1 << 8);
            } else if (!READ_BIT(drv_can1.CanHandle.Instance->TSR, CAN_FLAG_TXOK2)) {
                rt_hw_can_isr(&drv_can1.device, RT_CAN_EVENT_TX_FAIL | 2 << 8);
            }

            break;

        case RT_CAN_BUS_IMPLICIT_BIT_ERR:
        case RT_CAN_BUS_EXPLICIT_BIT_ERR:
            drv_can1.device.status.biterrcnt++;
            break;

        case RT_CAN_BUS_CRC_ERR:
            drv_can1.device.status.crcerrcnt++;
            break;
    }

    drv_can1.device.status.lasterrtype = errtype & 0x70;
    drv_can1.device.status.rcverrcnt   = errtype >> 24;
    drv_can1.device.status.snderrcnt   = (errtype >> 16 & 0xFF);
    drv_can1.device.status.errcode     = errtype & 0x07;
    hcan->Instance->MSR |= CAN_MSR_ERRI;
    // rt_interrupt_leave();
}
#endif /* BSP_USING_CAN1 */

#ifdef BSP_USING_CAN2
/**
 * @brief This function handles CAN2 RX0 interrupts.
 */
void CAN2_RX0_IRQHandler(void) {
    // rt_interrupt_enter();
    _can_rx_isr(&drv_can2.device, CAN_RX_FIFO0);
    // rt_interrupt_leave();
}

/**
 * @brief This function handles CAN2 RX1 interrupts.
 */
void CAN2_RX1_IRQHandler(void) {
    // rt_interrupt_enter();
    _can_rx_isr(&drv_can2.device, CAN_RX_FIFO1);
    // rt_interrupt_leave();
}

/**
 * @brief This function handles CAN2 SCE interrupts.
 */
void CAN2_SCE_IRQHandler(void) {
    rt_uint32_t        errtype;
    CAN_HandleTypeDef *hcan;

    hcan    = &drv_can2.CanHandle;
    errtype = hcan->Instance->ESR;

    // rt_interrupt_enter();
    HAL_CAN_IRQHandler(hcan);

    switch ((errtype & 0x70) >> 4) {
        case RT_CAN_BUS_BIT_PAD_ERR:
            drv_can2.device.status.bitpaderrcnt++;
            break;

        case RT_CAN_BUS_FORMAT_ERR:
            drv_can2.device.status.formaterrcnt++;
            break;

        case RT_CAN_BUS_ACK_ERR:
            drv_can2.device.status.ackerrcnt++;

            if (!READ_BIT(drv_can2.CanHandle.Instance->TSR, CAN_FLAG_TXOK0)) {
                rt_hw_can_isr(&drv_can2.device, RT_CAN_EVENT_TX_FAIL | 0 << 8);
            } else if (!READ_BIT(drv_can2.CanHandle.Instance->TSR, CAN_FLAG_TXOK1)) {
                rt_hw_can_isr(&drv_can2.device, RT_CAN_EVENT_TX_FAIL | 1 << 8);
            } else if (!READ_BIT(drv_can2.CanHandle.Instance->TSR, CAN_FLAG_TXOK2)) {
                rt_hw_can_isr(&drv_can2.device, RT_CAN_EVENT_TX_FAIL | 2 << 8);
            }

            break;

        case RT_CAN_BUS_IMPLICIT_BIT_ERR:
        case RT_CAN_BUS_EXPLICIT_BIT_ERR:
            drv_can2.device.status.biterrcnt++;
            break;

        case RT_CAN_BUS_CRC_ERR:
            drv_can2.device.status.crcerrcnt++;
            break;
    }

    drv_can2.device.status.lasterrtype = errtype & 0x70;
    drv_can2.device.status.rcverrcnt   = errtype >> 24;
    drv_can2.device.status.snderrcnt   = (errtype >> 16 & 0xFF);
    drv_can2.device.status.errcode     = errtype & 0x07;
    hcan->Instance->MSR |= CAN_MSR_ERRI;
    // rt_interrupt_leave();
}
#endif /* BSP_USING_CAN2 */

/**
 * @brief  Error CAN callback.
 * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
 *         the configuration information for the specified CAN.
 * @retval None
 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_ERROR_WARNING | CAN_IT_ERROR_PASSIVE | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR |
                                  CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_OVERRUN | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO1_MSG_PENDING |
                                  CAN_IT_RX_FIFO1_OVERRUN | CAN_IT_RX_FIFO1_FULL | CAN_IT_TX_MAILBOX_EMPTY);
}

int rt_hw_can_init(void) {
    struct can_configure config = CANDEFAULTCONFIG;
    config.privmode             = RT_CAN_MODE_NOPRIV;
    config.ticks                = 50;
#ifdef RT_CAN_USING_HDR
    config.maxhdr = 14;
#ifdef CAN2
    config.maxhdr = 28;
#endif
#endif
    /* config default filter */
    CAN_FilterTypeDef filterConf    = {0};
    filterConf.FilterIdHigh         = 0x0000;
    filterConf.FilterIdLow          = 0x0000;
    filterConf.FilterMaskIdHigh     = 0x0000;
    filterConf.FilterMaskIdLow      = 0x0000;
    filterConf.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filterConf.FilterBank           = 0;
    filterConf.FilterMode           = CAN_FILTERMODE_IDMASK;
    filterConf.FilterScale          = CAN_FILTERSCALE_32BIT;
    filterConf.FilterActivation     = ENABLE;
    filterConf.SlaveStartFilterBank = 14;

#ifdef BSP_USING_CAN1
    filterConf.FilterBank = 0;

    drv_can1.FilterConfig  = filterConf;
    drv_can1.device.config = config;

    drv_can1.device.parent.user_data = &drv_can1;
    drv_can1.CanHandle               = canHandle1;

    drv_can1.device.ops = &_can_ops;

    _can_control(&drv_can1.device, RT_CAN_CMD_SET_FILTER, RT_NULL);
    _can_control(&drv_can1.device, RT_DEVICE_CTRL_SET_INT, (void *) RT_DEVICE_FLAG_INT_RX);
    _can_config(&drv_can1.device, &drv_can1.device.config);

#endif /* BSP_USING_CAN1 */

#ifdef BSP_USING_CAN2
    filterConf.FilterBank = filterConf.SlaveStartFilterBank;

    drv_can2.FilterConfig  = filterConf;
    drv_can2.device.config = config;

    drv_can2.device.parent.user_data = &drv_can2;
    drv_can2.CanHandle               = canHandle2;

    drv_can2.device.ops = &_can_ops;

    _can_control(&drv_can2.device, RT_CAN_CMD_SET_FILTER, RT_NULL);
    _can_control(&drv_can2.device, RT_DEVICE_CTRL_SET_INT, RT_DEVICE_FLAG_INT_RX);
    _can_config(&drv_can2.device, &drv_can2.device.config);

#endif /* BSP_USING_CAN2 */

    /* fifo+list creat */
#ifdef BSP_USING_CAN1
    struct rt_can_device *can = &drv_can1.device;
#elif defined BSP_USING_CAN2
    struct rt_can_device *can = &drv_can2.device;
#endif

    int                    i = 0;
    struct rt_can_rx_fifo *rx_fifo;

    rx_fifo = (struct rt_can_rx_fifo *) malloc(sizeof(struct rt_can_rx_fifo) + can->config.msgboxsz * sizeof(struct rt_can_msg_list));
    RT_ASSERT(rx_fifo != RT_NULL);

    rx_fifo->buffer = (struct rt_can_msg_list *) (rx_fifo + 1);
    memset(rx_fifo->buffer, 0, can->config.msgboxsz * sizeof(struct rt_can_msg_list));
    rt_list_init(&rx_fifo->freelist);
    rt_list_init(&rx_fifo->uselist);
    rx_fifo->freenumbers = can->config.msgboxsz;

    for (i = 0; i < can->config.msgboxsz; i++) {
        rt_list_insert_before(&rx_fifo->freelist, &rx_fifo->buffer[i].list);
#ifdef RT_CAN_USING_HDR
        rt_list_init(&rx_fifo->buffer[i].hdrlist);
        rx_fifo->buffer[i].owner = RT_NULL;
#endif
    }

    can->can_rx = rx_fifo;

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rt_hw_can_init, rt_hw_can_init, rt_hw_can_init);

#endif /* BSP_USING_CAN */

/************************** end of file ******************/
