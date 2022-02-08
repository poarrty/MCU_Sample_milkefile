/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal_uros.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Protocol abstraction layer: microros instance
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw/rmw.h>
#include <rmw/init_options.h>
#include <rmw_microros/rmw_microros.h>
#include <stdio.h>
#include <std_msgs/msg/header.h>
#include <std_msgs/msg/u_int8.h>
#include <std_srvs/srv/set_bool.h>
#include <time.h>
#include <unistd.h>

#include "allocators.h"
#include "cmsis_os.h"
#include "log.h"
#include "microros_transports.h"
#include "pal_uros.h"
#include "shell.h"
#include "usart.h"
#include "fal.h"
#include "fal_ota.h"
#include "fal_workstation.h"
#include "clean_manage.h"

#define LOG_TAG "pal_uros"
#include "elog.h"
// #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc !=
// RCL_RET_OK)){printf("Failed status on line %d: %d.
// Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCCHECK(fn)                                                                       \
    {                                                                                     \
        rcl_ret_t temp_rc = fn;                                                           \
        if ((temp_rc != RCL_RET_OK)) {                                                    \
            printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int) temp_rc); \
        }                                                                                 \
    }
#define RCSOFTCHECK(fn)                                                                     \
    {                                                                                       \
        rcl_ret_t temp_rc = fn;                                                             \
        if ((temp_rc != RCL_RET_OK)) {                                                      \
            printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int) temp_rc); \
        }                                                                                   \
    }

#define IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#define IS_IRQ_MODE()   (__get_IPSR() != 0U)
#define IS_IRQ()        (IS_IRQ_MODE() || (IS_IRQ_MASKED() && (osKernelGetState() == osKernelRunning)))

SyncStatus      syncStatus = SYNC_STATUS_RECONNECT;
rcl_allocator_t g_allocator;
rclc_support_t  g_support;
rcl_node_t      g_node;
rclc_executor_t g_executor;
bool            micro_connect_flag;
uint8_t         publisher_register_count   = 0;
uint8_t         subscrption_register_count = 0;
uint8_t         service_register_count     = 0;
uint8_t         client_register_count      = 0;

volatile uint16_t ros2_send_remain_count = 0;

static pservice     g_service     = NULL;
static pclient      g_client      = NULL;
static ppublisher   g_publisher   = NULL;
static psubscrption g_subscrption = NULL;

static publisher     g_main_heartbeat;
std_msgs__msg__UInt8 main_heartbeat;

/*test client define ***************************************/
client                          g_client_test;
std_srvs__srv__SetBool_Request  g_client_test_request;
std_srvs__srv__SetBool_Response g_client_test_response;

void client_test_callback(const void *msg) {
    static uint32_t i = 0;
    i++;
    std_srvs__srv__SetBool_Response *msgin = (std_srvs__srv__SetBool_Response *) msg;
    printf("Test Client . Received service response (%lu) %d  \n", i, msgin->success);
}
/*end test client define ***********************************/

void service_init(pservice ptservice, const rosidl_service_type_support_t *type_support, char *service_name, void *request_msg,
                  void *response_msg, qos_enum_t qos, rclc_service_callback_t callback) {
    ptservice->type_support = type_support;
    ptservice->service_name = service_name;
    ptservice->request_msg  = request_msg;
    ptservice->response_msg = response_msg;
    ptservice->callback     = callback;
    ptservice->qos          = qos;

    ptservice->ptnext = g_service;
    g_service         = ptservice;
}
void client_init(pclient ptclient, const rosidl_service_type_support_t *type_support, char *client_name, void *request_msg, void *response_msg,
                 qos_enum_t qos, rclc_client_callback_t callback) {
    ptclient->type_support = type_support;
    ptclient->client_name  = client_name;
    ptclient->request_msg  = request_msg;
    ptclient->response_msg = response_msg;
    ptclient->callback     = callback;
    ptclient->qos          = qos;

    ptclient->ptnext = g_client;
    g_client         = ptclient;
}
void publisher_init(ppublisher ptpublisher, const rosidl_message_type_support_t *type_support, char *topic_name, void *ros_message,
                    qos_enum_t qos, msgqueue_enum_t msgqueue, UBaseType_t uxItemSize) {
    ptpublisher->type_support = type_support;
    ptpublisher->topic_name   = topic_name;
    ptpublisher->ros_message  = ros_message;
    ptpublisher->qos          = qos;
    ptpublisher->msgqueue     = msgqueue;

    ptpublisher->ptnext = g_publisher;
    g_publisher         = ptpublisher;

    if (ptpublisher->msgqueue == OVERWRITE) {
        ptpublisher->mq_id = osMessageQueueNew(1, uxItemSize, NULL);
    } else {
        ptpublisher->mq_id = osMessageQueueNew(5, uxItemSize, NULL);
    }
}

void subscrption_init(psubscrption ptsubscrption, const rosidl_message_type_support_t *type_support, char *topic_name, void *ros_message,
                      qos_enum_t qos, rclc_subscription_callback_t callback) {
    ptsubscrption->type_support = type_support;
    ptsubscrption->topic_name   = topic_name;
    ptsubscrption->ros_message  = ros_message;
    ptsubscrption->callback     = callback;

    ptsubscrption->ptnext = g_subscrption;
    g_subscrption         = ptsubscrption;
}

void message_publish(ppublisher ptpublisher) {
    /*单线程发布*/
#if 0
    if (ptpublisher->mq_id == NULL) {
        log_w("some topic no mq_id or no init");
        return;
    }

    if (ptpublisher->msgqueue == OVERWRITE) {
        if (IS_IRQ()) {
            BaseType_t yield = pdFALSE;
            xQueueOverwriteFromISR((QueueHandle_t) ptpublisher->mq_id,
                                   ptpublisher->ros_message, &yield);

            if (yield == pdTRUE) {
                portYIELD_FROM_ISR(yield);
            }
        } else {
            xQueueOverwrite((QueueHandle_t) ptpublisher->mq_id,
                            ptpublisher->ros_message);
        }
    } else {
        osMessageQueuePut(ptpublisher->mq_id, ptpublisher->ros_message, 0, 0);
    }
#else
    /*多线程发布*/
    if ((syncStatus != SYNC_STATUS_SUCCEED) || IS_IRQ()) {
        return;
    }
    ros2_send_remain_count++;
    log_d("%s topic ready pub", ptpublisher->topic_name);
    ptpublisher->publish_ret = rcl_publish(&ptpublisher->publisher_t, (const void *) ptpublisher->ros_message, NULL);
    if (ptpublisher->publish_ret == RCL_RET_OK) {
        log_i("%s pub ok", ptpublisher->topic_name);
    } else {
        log_w("%s pub fail", ptpublisher->topic_name);
    }
    ros2_send_remain_count--;
#endif
}

bool client_message_send(pclient ptclient) {
    /*多线程发布*/
    if ((syncStatus != SYNC_STATUS_SUCCEED) || IS_IRQ()) {
        return false;
    }
    ros2_send_remain_count++;
    log_d("%s client ready send", ptclient->client_name);
    int64_t sequence_number;
    ptclient->client_ret = rcl_send_request(&ptclient->client_t, ptclient->request_msg, &sequence_number);
    if (ptclient->client_ret == RCL_RET_OK) {
        log_i("%s client send  ok", ptclient->client_name);
        ros2_send_remain_count--;
        return true;
    } else {
        log_w("%s client send  fail", ptclient->client_name);
        ros2_send_remain_count--;
        return false;
    }
}

bool MallocString(rosidl_runtime_c__String *str, msgtype_enum_t msgtype, int space) {
    if (space == 0) {
        return true;
    }

    if (msgtype == REQ) {
        str->size     = 0;
        str->capacity = space;
    } else if (msgtype == RES) {
        str->size     = space;
        str->capacity = 0;
    } else {
        return false;
    }

    str->data = pvPortMalloc(space);

    if (str->data == NULL) {
        return false;
    }

    return true;
}

bool FreeString(rosidl_runtime_c__String *str) {
    str->size     = 0;
    str->capacity = 0;

    if (str->data != NULL) {
        vPortFree(str->data);
        str->data = NULL;
    }

    return true;
}

/// set timestamp of topic
void pal_uros_msg_set_timestamp(builtin_interfaces__msg__Time *ts) {
    int64_t nanos = rmw_uros_epoch_nanos();
    ts->sec       = (int32_t)(nanos / 1000000000);
    ts->nanosec   = (uint32_t)(nanos % 1000000000);
}

rcl_ret_t InitializePublisher() {
    ppublisher p_pubtmp = g_publisher;
    rcl_ret_t  ret      = RCL_RET_OK;
    while (p_pubtmp) {
        int count = 4;

        while (--count) {
            if (p_pubtmp->qos == BEST) {
                ret = rclc_publisher_init_best_effort(&p_pubtmp->publisher_t, &g_node, p_pubtmp->type_support, p_pubtmp->topic_name);
            } else {
                ret = rclc_publisher_init_default(&p_pubtmp->publisher_t, &g_node, p_pubtmp->type_support, p_pubtmp->topic_name);
            }
            log_d("topic_name: %s , count = %d , ret = %ld", p_pubtmp->topic_name, count, ret);

            if (RCL_RET_OK == ret) {
                break;
            }

            if (count == 1) {
                return ret;
            }
        }

        p_pubtmp = p_pubtmp->ptnext;

        if (++publisher_register_count > RMW_UXRCE_MAX_PUBLISHERS) {
            log_w("the max publishers is %d, publish create error", RMW_UXRCE_MAX_PUBLISHERS);
        }
    }

    return RCL_RET_OK;
}

rcl_ret_t InitializeSubscriber() {
    psubscrption p_subtmp = g_subscrption;
    rcl_ret_t    ret      = RCL_RET_OK;

    while (p_subtmp) {
        int count = 4;

        while (--count) {
            if (p_subtmp->qos == BEST) {
                ret = rclc_subscription_init_best_effort(&p_subtmp->subscrption_t, &g_node, p_subtmp->type_support, p_subtmp->topic_name);
            } else {
                ret = rclc_subscription_init_default(&p_subtmp->subscrption_t, &g_node, p_subtmp->type_support, p_subtmp->topic_name);
            }
            log_d("topic_name: %s , count = %d , ret = %ld", p_subtmp->topic_name, count, ret);

            if (RCL_RET_OK == ret) {
                break;
            }

            if (count == 1) {
                return ret;
            }
        }

        p_subtmp = p_subtmp->ptnext;

        if (++subscrption_register_count > RMW_UXRCE_MAX_SUBSCRIPTIONS) {
            log_w("the max subscriptions is %d, subscription create error", RMW_UXRCE_MAX_SUBSCRIPTIONS);
        }
    }

    return RCL_RET_OK;
}

rcl_ret_t initializeSubscriberExecutor() {
    psubscrption p_subtmp = g_subscrption;
    rcl_ret_t    ret      = RCL_RET_OK;

    while (p_subtmp) {
        if (p_subtmp->callback) {
            ret = rclc_executor_add_subscription(&g_executor, &p_subtmp->subscrption_t, p_subtmp->ros_message, p_subtmp->callback, ON_NEW_DATA);
            log_d("%s executor ret : %ld", p_subtmp->topic_name, ret);
        }

        p_subtmp = p_subtmp->ptnext;
    }

    return ret;
}

void MicroRosChannelConfig(void) {
    rmw_uros_set_custom_transport(true, (void *) &huart2, freertos_serial_open, freertos_serial_close, freertos_serial_write,
                                  freertos_serial_read);

    // Launch app thread when IP configured
    rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate        = __freertos_allocate;
    freeRTOS_allocator.deallocate      = __freertos_deallocate;
    freeRTOS_allocator.reallocate      = __freertos_reallocate;
    freeRTOS_allocator.zero_allocate   = __freertos_zero_allocate;

    if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
        // log_e("Error on default allocators (line %d)", __LINE__);
    } else {
        // log_i("uros init ok.");
    }

    g_allocator = rcl_get_default_allocator();
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), MicroRosChannelConfig, MicroRosChannelConfig,
                 MicroRosChannelConfig);

rcl_ret_t InitializeService() {
    pservice  p_srvtmp = g_service;
    rcl_ret_t ret      = RCL_RET_OK;
    while (p_srvtmp) {
        int count = 4;

        while (--count) {
            if (p_srvtmp->qos == BEST) {
                ret = rclc_service_init_best_effort(&p_srvtmp->service_t, &g_node, p_srvtmp->type_support, p_srvtmp->service_name);
            } else {
                ret = rclc_service_init_default(&p_srvtmp->service_t, &g_node, p_srvtmp->type_support, p_srvtmp->service_name);
            }
            log_d("service_name: %s , count = %d , ret = %ld", p_srvtmp->service_name, count, ret);

            if (RCL_RET_OK == ret) {
                break;
            }

            if (count == 1) {
                return ret;
            }
        }

        p_srvtmp = p_srvtmp->ptnext;

        if (++service_register_count > RMW_UXRCE_MAX_SERVICES) {
            log_w("the max services is %d, service create error", RMW_UXRCE_MAX_SERVICES);
        }
    }

    return RCL_RET_OK;
}

rcl_ret_t initializeServiceExecutor() {
    pservice  p_srvtmp = g_service;
    rcl_ret_t ret      = RCL_RET_OK;

    while (p_srvtmp) {
        if (p_srvtmp->callback) {
            ret =
                rclc_executor_add_service(&g_executor, &p_srvtmp->service_t, p_srvtmp->request_msg, p_srvtmp->response_msg, p_srvtmp->callback);
            log_d("%s executor ret : %ld", p_srvtmp->service_name, ret);
        }

        p_srvtmp = p_srvtmp->ptnext;
    }

    return ret;
}

rcl_ret_t InitializeClient() {
    pclient   p_srvtmp = g_client;
    rcl_ret_t ret      = RCL_RET_OK;
    while (p_srvtmp) {
        int count = 4;

        while (--count) {
            if (p_srvtmp->qos == BEST) {
                ret = rclc_client_init_best_effort(&p_srvtmp->client_t, &g_node, p_srvtmp->type_support, p_srvtmp->client_name);
            } else {
                ret = rclc_client_init_default(&p_srvtmp->client_t, &g_node, p_srvtmp->type_support, p_srvtmp->client_name);
            }
            log_d("client_name: %s , count = %d , ret = %ld", p_srvtmp->client_name, count, ret);

            if (RCL_RET_OK == ret) {
                break;
            }

            if (count == 1) {
                return ret;
            }
        }

        p_srvtmp = p_srvtmp->ptnext;

        if (++client_register_count > RMW_UXRCE_MAX_CLIENTS) {
            log_w("the max client is %d, service create error", RMW_UXRCE_MAX_CLIENTS);
        }
    }

    return RCL_RET_OK;
}

rcl_ret_t initializeClientExecutor() {
    pclient   p_srvtmp = g_client;
    rcl_ret_t ret      = RCL_RET_OK;

    while (p_srvtmp) {
        if (p_srvtmp->callback) {
            ret = rclc_executor_add_client(&g_executor, &p_srvtmp->client_t, p_srvtmp->response_msg, p_srvtmp->callback);
            log_d("%s executor ret : %ld", p_srvtmp->client_name, ret);
        }

        p_srvtmp = p_srvtmp->ptnext;
    }
    return ret;
}

bool MicroRosIsConnect(SyncStatus status) {
    static int errorCount = 0;
    bool       ret        = true;

    switch (status) {
        case SYNC_STATUS_SUCCEED:
            errorCount = 0;
            break;

        case SYNC_STATUS_FAIL:
            if (++errorCount > 2) {
                errorCount = 0;
                ret        = false;
            }

            break;

        case SYNC_STATUS_RECONNECT:
            ret = false;
            break;

        case SYNC_STATUS_WAIT:
            break;

        default:
            break;
    }

    return ret;
}

bool MicroRosFreeResource(bool *freeFlag) {
    while (ros2_send_remain_count) {
        osDelay(500);
        log_d("wait remain ros2 send fail");
    }

    publisher_register_count   = 0;
    subscrption_register_count = 0;
    service_register_count     = 0;
    client_register_count      = 0;

    if (!(*freeFlag)) {
        return true;
    }

    *freeFlag = false;

    // int i = 0;
    rcl_ret_t ret = RCL_RET_OK;

    // log_d("fini=%d", ++i);
    ret += rclc_executor_fini(&g_executor);

    pclient p_clienttemp = g_client;
    while (p_clienttemp) {
        // log_d("fini=%d", ++i);
        ret += rcl_client_fini(&p_clienttemp->client_t, &g_node);

        p_clienttemp = p_clienttemp->ptnext;
    }

    pservice p_srvtmp = g_service;

    while (p_srvtmp) {
        // log_d("fini=%d", ++i);
        ret += rcl_service_fini(&p_srvtmp->service_t, &g_node);

        p_srvtmp = p_srvtmp->ptnext;
    }

    ppublisher p_pubtmp = g_publisher;

    while (p_pubtmp) {
        // log_d("fini=%d", ++i);
        ret += rcl_publisher_fini(&p_pubtmp->publisher_t, &g_node);

        p_pubtmp = p_pubtmp->ptnext;
    }

    psubscrption p_subtmp = g_subscrption;

    while (p_subtmp) {
        // log_d("fini=%d", ++i);
        ret += rcl_subscription_fini(&p_subtmp->subscrption_t, &g_node);

        p_subtmp = p_subtmp->ptnext;
    }

    // log_d("fini=%d", ++i);
    ret += rcl_node_fini(&g_node);
    // log_d("fini=%d", ++i);
    ret += rclc_support_fini(&g_support);
    // log_d("fini=%d", ++i);
    return !ret;
}

bool MicroRosConnect() {
    return rclc_support_init(&g_support, 0, NULL, &g_allocator) == RCL_RET_OK ? true : false;
}

bool MicroRosBuildResource() {
    int ret;
    /* node *******************************************************************/
    // create a node
    log_d("rclc_node_init_default");
    ret = rclc_node_init_default(&g_node, "KavabotMainMCU", "", &g_support) == RCL_RET_OK ? true : false;

    if (!ret) {
        log_d("rclc_node_init_default fail");
        return false;
    }

    /* publisher **************************************************************/
    log_d("InitializePublisher");

    if (InitializePublisher() != RCL_RET_OK) {
        log_d("InitializePublisher fail");
        return false;
    }

    /* subscriber *************************************************************/
    log_d("InitializeSubscriber");

    if (InitializeSubscriber() != RCL_RET_OK) {
        log_d("InitializeSubscriber fail");
        return false;
    }

    /* service *************************************************************/
    log_d("InitializeService");

    if (InitializeService() != RCL_RET_OK) {
        log_d("InitializeService fail");
        return false;
    }

    /* client **************************************************************/
    log_d("InitializeClient");
    if (InitializeClient() != RCL_RET_OK) {
        log_d("InitializeClient fail");
        return false;
    }
    /* timer *****************************************************************/
    // RCCHECK(rclc_timer_init_default(&ipc2uros_timer, &g_support,
    // RCL_MS_TO_NS(1), ipc_timer_callback));

    /* executor **************************************************************/
    // Create a executor
    log_d("rclc_executor_init");
    log_d("==== EXECUTOR NUM: %d ", publisher_register_count + subscrption_register_count + service_register_count + client_register_count);
    log_d("==== Pub NUM: %d  Sub NUM: %d  Srv NUM: %d  Clt NUM: %d", publisher_register_count, subscrption_register_count,
          service_register_count, client_register_count);

    if (rclc_executor_init(&g_executor, &g_support.context, subscrption_register_count + service_register_count + client_register_count,
                           &g_allocator) != RCL_RET_OK) {
        log_d("rclc_executor_init fail, ret=%d", ret);
        return false;
    }

    /* add timer to executor *************************************************/
    // RCCHECK(rclc_executor_add_timer(&executor, &ipc2uros_timer));

    /* add subcriber to executor *********************************************/
    log_d("initializeSubscriberExecutor");

    if (initializeSubscriberExecutor() != RCL_RET_OK) {
        log_d("initializeSubscriberExecutor fail");
        return false;
    }

    /* add service to executor ***********************************************/
    log_d("initializeServiceExecutor");

    if (initializeServiceExecutor() != RCL_RET_OK) {
        log_d("initializeServiceExecutor fail");
        return false;
    }

    /* add client to executor ***********************************************/

    log_d("initializeClientExecutor");

    if (initializeClientExecutor() != RCL_RET_OK) {
        log_d("initializeClientExecutor fail");
        return false;
    }

    /* start executor ********************************************************/
    return true;
}

bool IsSyncTime() {
    static unsigned int sync_ts = 0;
    bool                ret     = osKernelGetTickCount() >= sync_ts;
    sync_ts                     = ret ? osKernelGetTickCount() + 120 * 1000 : sync_ts;
    return ret;
}

bool IsHeartBeat() {
    static unsigned int sync_ts = 0;
    bool                ret     = osKernelGetTickCount() >= sync_ts;
    sync_ts                     = ret ? osKernelGetTickCount() + 1 * 1000 : sync_ts;
    return ret;
}

SyncStatus MicroRosSyncTime() {
    if (rmw_uros_sync_session(500) != RMW_RET_OK) {
        log_d("rmw_uros_sync_session error");
    }

    int64_t nanos;
    nanos = rmw_uros_epoch_nanos();

    struct timespec ts;
    ts.tv_sec  = (int32_t)(nanos / 1000000000) + 8 * 3600;
    ts.tv_nsec = (uint32_t)(nanos % 1000000000);

    /// set rtc
    const time_t time_second = ts.tv_sec;
    struct tm *  time_utc    = gmtime(&time_second);

    set_local_rtc(time_utc->tm_year, time_utc->tm_mon, time_utc->tm_mday, time_utc->tm_hour, time_utc->tm_min, time_utc->tm_sec,
                  (uint16_t)((nanos % 1000000000) / 1000000));

    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    log_d("MCU: microros time sync ok, current time: %s.%03ld", buff, ts.tv_nsec / 1000000);

    return SYNC_STATUS_SUCCEED;
}

void MicroRosPublish() {
    ppublisher p_pubtmp = g_publisher;

    while (p_pubtmp) {
        if (osMessageQueueGet(p_pubtmp->mq_id, p_pubtmp->ros_message, NULL, 0) == osOK) {
            p_pubtmp->publish_ret = rcl_publish(&p_pubtmp->publisher_t, (const void *) p_pubtmp->ros_message, NULL);

            if (p_pubtmp->publish_ret != RCL_RET_OK) {
                log_w("topic_name: %s publish ERROR", p_pubtmp->topic_name);
            } else {
                log_d("topic_name: %s publish OK", p_pubtmp->topic_name);
            }
        }

        p_pubtmp = p_pubtmp->ptnext;
    }
}

void StartDefaultTask(void *argument) {
    uint8_t heartbeat_error_count = 0;
    bool    freeResourceFlag      = false;
    bool    pingFlag              = false;  // true ping ,false no ping.
    MicroRosChannelConfig();

    publisher_init(&g_main_heartbeat, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/c5_task_mcu_main_heartbeat", &main_heartbeat,
                   DEFAULT, OVERWRITE, sizeof(std_msgs__msg__UInt8));

    /* init */
    // g_publisher_init();
    // g_subscrption_init();

    /*******************************************************************
            client_init(&g_client_test,
                               ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv,
    SetBool),
                               "/c5_task_mcu_main_client_test",
                               &g_client_test_request,
                               &g_client_test_response,
                               DEFAULT,
                               client_test_callback
                              );
    *********************************************************************/
    clean_manage_pal_init();
    fal_workstation_init();
    fal_ota_init();
    while (1) {
        osDelay(5);

        if (syncStatus != SYNC_STATUS_SUCCEED && !pingFlag) {
            if (rmw_uros_ping_agent(50, 1) != RMW_RET_OK) {
                log_d("ping error!");
                osDelay(200);
                continue;
            }
            log_d("ping ok!");
            pingFlag = true;
        }

        if (!MicroRosIsConnect(syncStatus)) {
            syncStatus = SYNC_STATUS_RECONNECT;
            log_d("MicroRosFreeResource!");
            MicroRosFreeResource(&freeResourceFlag);
            osDelay(2000);

            log_d("MicroRosConnect!");

            if (!MicroRosConnect()) {
                log_d("MicroRosConnect error!");
                rclc_support_fini(&g_support);
                continue;
            }

            log_d("MicroRosBuildResource!");

            if (!MicroRosBuildResource()) {
                syncStatus = SYNC_STATUS_RECONNECT;
                log_d("MicroRosBuildResource fail!");
            } else {
                syncStatus = SYNC_STATUS_SUCCEED;
                log_d("MicroRosBuildResource finish!");
                g_main_heartbeat.publish_ret = RCL_RET_OK;
                heartbeat_error_count        = 0;
            }

            freeResourceFlag = true;
            continue;
        }

        // MicroRosPublish();

        if (IsHeartBeat()) {
            if (g_main_heartbeat.publish_ret != RCL_RET_OK) {
                log_w("heartbeat error, count = %d", heartbeat_error_count + 1);
                if (++heartbeat_error_count >= 2) {
                    heartbeat_error_count = 0;
                    syncStatus            = SYNC_STATUS_RECONNECT;
                    pingFlag              = false;
                }
            } else {
                heartbeat_error_count = 0;
                syncStatus            = SYNC_STATUS_SUCCEED;
                main_heartbeat.data++;
            }

            message_publish(&g_main_heartbeat);
        }

        if (kv_ota_config.ready_flag == 1) {
            __set_FAULTMASK(1);  // 关闭所有中断
            NVIC_SystemReset();  // 复位
        }

        syncStatus = IsSyncTime() ? MicroRosSyncTime() : syncStatus;

        rclc_executor_spin_some(&g_executor, RCL_MS_TO_NS(1));
    }
}

SyncStatus get_microros_sync_state(void) {
    return syncStatus;
}