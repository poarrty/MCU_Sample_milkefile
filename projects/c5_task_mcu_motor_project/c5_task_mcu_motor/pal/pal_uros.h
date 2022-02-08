/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_uros.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Protocol abstraction layer: microros instance
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef _PAL_UROS_H_
#define _PAL_UROS_H_
#include "board.h"
#include "pal.h"
#include <std_msgs/msg/header.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/publisher.h>
#include <rclc/executor.h>
#include "cmsis_os2.h"
#include "cmsis_os.h"
#include "queue.h"

typedef enum { REQ = 0, RES } msgtype_enum_t;

typedef enum { BEST = 0, DEFAULT } qos_enum_t;

typedef enum { OVERWRITE = 0, SEND } msgqueue_enum_t;

typedef struct service {
    rcl_service_t                        service_t;
    const rosidl_service_type_support_t *type_support;
    char *                               service_name;
    void *                               request_msg;
    void *                               response_msg;
    qos_enum_t                           qos;
    rclc_service_callback_t              callback;
    struct service *                     ptnext;
} service, *pservice;

typedef enum {
    SYNC_STATUS_WAIT = 1,
    SYNC_STATUS_SUCCEED,
    SYNC_STATUS_FAIL,
    SYNC_STATUS_RECONNECT,
} SyncStatus;

extern SyncStatus syncStatus;

void service_init(pservice ptservice, const rosidl_service_type_support_t *type_support, char *service_name, void *request_msg,
                  void *response_msg, qos_enum_t qos, rclc_service_callback_t callback);

typedef struct publisher {
    rcl_publisher_t                      publisher_t;
    const rosidl_message_type_support_t *type_support;
    char *                               topic_name;
    void *                               ros_message;
    qos_enum_t                           qos;
    msgqueue_enum_t                      msgqueue;
    osMessageQueueId_t                   mq_id;
    rcl_ret_t                            publish_ret;
    struct publisher *                   ptnext;
} publisher, *ppublisher;

void publisher_init(ppublisher ptpublisher, const rosidl_message_type_support_t *type_support, char *topic_name, void *ros_message,
                    qos_enum_t qos, msgqueue_enum_t msgqueue, UBaseType_t uxItemSize);

typedef struct subscrption {
    rcl_subscription_t                   subscrption_t;
    const rosidl_message_type_support_t *type_support;
    char *                               topic_name;
    void *                               ros_message;
    qos_enum_t                           qos;
    rclc_subscription_callback_t         callback;
    struct subscrption *                 ptnext;
} subscrption, *psubscrption;
void subscrption_init(psubscrption ptsubscrption, const rosidl_message_type_support_t *type_support, char *topic_name, void *ros_message,
                      qos_enum_t qos, rclc_subscription_callback_t callback);

bool MallocString(rosidl_runtime_c__String *str, msgtype_enum_t msgtype, int space);
bool FreeString(rosidl_runtime_c__String *str);

bool micro_connect_flag;

void pal_uros(void *argument);
void message_publish(ppublisher ptpublisher);
void pal_uros_msg_set_timestamp(builtin_interfaces__msg__Time *ts);

#endif  // _MAIN_H_
