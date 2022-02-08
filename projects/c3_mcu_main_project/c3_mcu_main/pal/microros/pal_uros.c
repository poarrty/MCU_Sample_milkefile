#include "pal_uros.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "SEGGER_RTT.h"
#include <rmw/rmw.h>
#include <rmw/init_options.h>
#include <rmw_microros/rmw_microros.h>
#include <microros_transports.h>
#include <allocators.h>
#include "devices.h"
#include "log.h"
#include "define_motor.h"
#include "gpio.h"
extern int bldcm_handle;
#include "usart.h"
//#include "bsp_rtc.h"
#include "std_msgs/msg/u_int8.h"

#define LOG_TAG "pal_uros"
extern int dig_usound_S12_handle;
extern int clean_water_pump_handle;
extern int wheel_handle;
extern int up_down_push_rod_handle;
extern int side_brush_handle;
extern int roller_brush_handle;
extern int roller_tube_handle;
extern int clean_water_pump_handle;
extern int sewage_water_pump_handle;
extern int water_valve_handle;
extern int fan_motor_handle;
//#include "elog.h"

#define RCCHECK(fn)                                                       \
    {                                                                     \
        rcl_ret_t temp_rc = fn;                                           \
        if ((temp_rc != RCL_RET_OK)) {                                    \
            is_uros_rclc_error_happened = true;                           \
            LOG_DEBUG("fail on line :%d\r\n", __LINE__);       \
        }                                                                 \
    }
#define RCDESTORYCHECK(fn)                                              \
    {                                                                   \
        rcl_ret_t temp_rc = fn;                                         \
        if ((temp_rc != RCL_RET_OK)) {                                  \
        } else {                                                        \
        }                                                               \
    }

#define IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#define IS_IRQ_MODE() (__get_IPSR() != 0U)
#define IS_IRQ()      \
    (IS_IRQ_MODE() || \
     (IS_IRQ_MASKED() && (osKernelGetState() == osKernelRunning)))

#define UROS_PING_PERIOD_MS 1000
#define UROS_SYNC_PERIOD_MS 60000
uint32_t uros_ping_ts = 0;
uint32_t uros_sync_ts = 0;
uint32_t uros_heartbeat_ts = 0;

rcl_allocator_t allocator = {0};
rclc_support_t support = {0};
rcl_node_t node = {0};
rclc_executor_t executor = {0};

static pservice g_service = NULL;
static ppublisher g_publisher = NULL;
static psubscrption g_subscrption = NULL;

static publisher g_nav_heartbeat;
static std_msgs__msg__UInt8 nav_heartbeat;

///< ipc timer
rcl_timer_t ipc2uros_timer = {0};

bool is_uros_init_successful = false;
bool is_uros_rclc_error_happened = false;

static void ipc_handle(void);

int pal_uros_init(void);
void pal_uros_create_entities(void);
void pal_uros_destroy_entities();
void pal_uros_sync(void);

void service_init(pservice ptservice,
                  const rosidl_service_type_support_t *type_support,
                  char *service_name, void *request_msg, void *response_msg,
                  qos_enum_t qos, rclc_service_callback_t callback) {
    ptservice->type_support = type_support;
    ptservice->service_name = service_name;
    ptservice->request_msg = request_msg;
    ptservice->response_msg = response_msg;
    ptservice->callback = callback;
    ptservice->qos = qos;

    ptservice->ptnext = g_service;
    g_service = ptservice;
}

void publisher_init(ppublisher ptpublisher,
                    const rosidl_message_type_support_t *type_support,
                    char *topic_name, void *ros_message, qos_enum_t qos,
                    msgqueue_enum_t msgqueue, UBaseType_t uxItemSize) {
    ptpublisher->type_support = type_support;
    ptpublisher->topic_name = topic_name;
    ptpublisher->ros_message = ros_message;
    ptpublisher->qos = qos;
    ptpublisher->msgqueue = msgqueue;

    ptpublisher->ptnext = g_publisher;
    g_publisher = ptpublisher;

    if (ptpublisher->msgqueue == OVERWRITE) {
        ptpublisher->mq_id = osMessageQueueNew(1, uxItemSize, NULL);
    } else {
        ptpublisher->mq_id = osMessageQueueNew(5, uxItemSize, NULL);
    }
}

void subscrption_init(psubscrption ptsubscrption,
                      const rosidl_message_type_support_t *type_support,
                      char *topic_name, void *ros_message, qos_enum_t qos,
                      rclc_subscription_callback_t callback) {
    ptsubscrption->type_support = type_support;
    ptsubscrption->topic_name = topic_name;
    ptsubscrption->ros_message = ros_message;
    ptsubscrption->callback = callback;
    ptsubscrption->qos = qos;

    ptsubscrption->ptnext = g_subscrption;
    g_subscrption = ptsubscrption;
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
    if ((is_uros_init_successful == false) || IS_IRQ()) {
        return;
    }
    /*多线程发布*/
    ptpublisher->publish_ret =
        rcl_publish(&ptpublisher->publisher_t,
                    (const void *) ptpublisher->ros_message, NULL);
    if (ptpublisher->publish_ret == RCL_RET_OK) {
        LOG_DEBUG("%s pub ok\r\n", ptpublisher->topic_name);
    } else {
        LOG_DEBUG("%s pub fail\r\n", ptpublisher->topic_name);
    }
#endif
}

bool MallocString(rosidl_runtime_c__String *str, msgtype_enum_t msgtype,
                  int space) {
    if (space == 0) {
        return true;
    }

    if (msgtype == REQ) {
        str->size = 0;
        str->capacity = space;
    } else if (msgtype == RES) {
        str->size = space;
        str->capacity = 0;
    } else {
        return false;
    }

    str->data = malloc(space);

    if (str->data == NULL) {
        return false;
    }

    return true;
}

bool FreeString(rosidl_runtime_c__String *str) {
    str->size = 0;
    str->capacity = 0;

    if (str->data != NULL) {
        free(str->data);
        str->data = NULL;
    }

    return true;
}


void task_uros_run(void *argument) {
    // while (1)
    // {
    //     //HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2);
    //     osDelay(500);
    //     //LOG_DEBUG("hhhhhhhhhhhhhhhhhhhhhh");
    //     //device_ioctl(dig_usound_S12_handle,0,NULL);
    // }
    rmw_ret_t ping_ret = RMW_RET_ERROR;
    uint8_t heartbeat_error_count = 0;

    publisher_init(&g_nav_heartbeat,
                   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
                   "/nav_heartbeat", &nav_heartbeat, DEFAULT, OVERWRITE,
                   sizeof(std_msgs__msg__UInt8));
    osDelay(1000);
    if (pal_uros_init() != 0) {
          
        while (1) {
//            log_e("pal_uros init error, pal uros stop.");
            // osDelay(1000);
        }
    }

    // while (1)
    // {
    //     // LOG_DEBUG("hhhhhhhhhhhhhhhhhhhhhh");
    //     osDelay(1000);
    //     device_ioctl(dig_usound_S12_handle,0,NULL);
    // }

//    log_i("pal_uros main loop start.");

    uros_ping_ts = 0;
    uros_sync_ts = 0;
    uros_heartbeat_ts = 0;
    static uint32_t uros_count = 0;
    /* uros main loop ********************************************************/
    while (1) {

           if (osKernelGetTickCount() >= uros_ping_ts) {
               ping_ret = rmw_uros_ping_agent(50, 5);

               if (RMW_RET_OK == ping_ret) {
                //    log_i("ping agent ok.");
                    LOG_DEBUG("ping agent ok\r\n"); 
                   uros_ping_ts = osWaitForever;
               } else {
                //    log_i("ping agent error.");
                    LOG_DEBUG("ping agent error, %ld\r\n", uros_count++); 
                   uros_ping_ts = osKernelGetTickCount() + UROS_PING_PERIOD_MS;
                   continue;
               }
           }

            if (RMW_RET_OK == ping_ret) {
                if (is_uros_init_successful) {
                    if (osKernelGetTickCount() >= uros_heartbeat_ts) {
                        if (g_nav_heartbeat.publish_ret != RCL_RET_OK) {
                            if (++heartbeat_error_count >= 2) {
                                heartbeat_error_count = 0;
                                ping_ret = RMW_RET_ERROR;
                            }
                        } else {
                            heartbeat_error_count = 0;
                        }

                        message_publish(&g_nav_heartbeat);
                        uros_heartbeat_ts = osKernelGetTickCount() + 1000;
                    }

                    ///< Doesn't work !! single thread topic handle
                    ipc_handle();
                    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1));
                    osDelay(1);
                } else {
                    LOG_DEBUG("pal_uros_create_entities\r\n"); 
                    pal_uros_create_entities();
                }

                if (osKernelGetTickCount() >= uros_sync_ts) {
                    pal_uros_sync();
                    uros_sync_ts = osKernelGetTickCount() + UROS_SYNC_PERIOD_MS;
                }
            } else {
                if (is_uros_init_successful) {    
                    LOG_DEBUG("pal_uros_destroy_entities.\r\n");
                    pal_uros_destroy_entities();
                    LOG_DEBUG("pal_uros_destroy_entities finish\r\n");
                }

                osDelay(1000);
            }
//        } else {
//            log_i("pal_uros wait sys on.");
//
//            if (is_uros_init_successful) {
//                log_i("pal_uros_destroy_entities.");
//                pal_uros_destroy_entities();
//            }
//
//            osDelay(1000);
//        }
    }
}

int pal_uros_init(void) {
    int ret = 1;

    rmw_uros_set_custom_transport(true, (void *) &huart3, freertos_serial_open,
                                  freertos_serial_close, freertos_serial_write,
                                  freertos_serial_read);

    rcl_allocator_t freeRTOS_allocator =
        rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate = __freertos_allocate;
    freeRTOS_allocator.deallocate = __freertos_deallocate;
    freeRTOS_allocator.reallocate = __freertos_reallocate;
    freeRTOS_allocator.zero_allocate = __freertos_zero_allocate;

    if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
        LOG_DEBUG("pal_uros_init error: set default allocators.\r\n");
        ret = 1;
    } else {
        LOG_DEBUG("pal_uros_init ok.\r\n");
        ret = 0;
    }

    is_uros_init_successful = false;
    pal_all_init();
    return ret;
}

int pal_uros_deInit(void) {
    return 0;
}

void pal_uros_create_entities(void) {
    ppublisher p_pubtmp = g_publisher;
    psubscrption p_subtmp = g_subscrption;
    pservice p_srvtmp = g_service;

    uint8_t publisher_register_count = 0;
    uint8_t subscrption_register_count = 0;
    uint8_t service_register_count = 0;
    /* allocator **************************************************************/
    // get allocator
    allocator = rcl_get_default_allocator();

    while (1) {
        if (rclc_support_init(&support, 0, NULL, &allocator) == RCL_RET_OK) {
            LOG_DEBUG("uros agent connect ok.\r\n");
            break;
        } else {
            LOG_DEBUG("uros agent connect error, retry...\r\n");
        }

        osDelay(1000);
    }

    is_uros_rclc_error_happened = false;

    /* node *******************************************************************/
    // create a pingpong_node
    // RCCHECK(rclc_node_init_default(&node, "12345", "", &support));
    if (rclc_node_init_default(&node, "MCU_429", "", &support) == RCL_RET_OK) {
            LOG_DEBUG("rclc_node_init_default ok.\r\n");
        } else {
            LOG_DEBUG("rclc_node_init_default error\r\n");
        }
    /* publisher **************************************************************/
    while (p_pubtmp) {
        if (p_pubtmp->qos == BEST) {
            RCCHECK(rclc_publisher_init_best_effort(
                &p_pubtmp->publisher_t, &node, p_pubtmp->type_support,
                p_pubtmp->topic_name));
        } else if (p_pubtmp->qos == DEFAULT) {
            RCCHECK(rclc_publisher_init_default(&p_pubtmp->publisher_t, &node,
                                                p_pubtmp->type_support,
                                                p_pubtmp->topic_name));
        } else {
//            log_w("no qos, use default publish");
            RCCHECK(rclc_publisher_init_default(&p_pubtmp->publisher_t, &node,
                                                p_pubtmp->type_support,
                                                p_pubtmp->topic_name));
        }

        p_pubtmp = p_pubtmp->ptnext;

        if (++publisher_register_count > RMW_UXRCE_MAX_PUBLISHERS) {
//            log_w(
//                "the max publishers is %d, publish create error, please fix "
//                "meta "
//                "file",
//                RMW_UXRCE_MAX_PUBLISHERS);
        }
    }
    LOG_DEBUG( "publisher_register %d , the max is %d\r\n", publisher_register_count, RMW_UXRCE_MAX_PUBLISHERS);

    /* subscriber *************************************************************/
    while (p_subtmp) {
        if (p_subtmp->qos == BEST) {
            RCCHECK(rclc_subscription_init_best_effort(
                &p_subtmp->subscrption_t, &node, p_subtmp->type_support,
                p_subtmp->topic_name));
        } else if (p_subtmp->qos == DEFAULT) {
            RCCHECK(rclc_subscription_init_default(
                &p_subtmp->subscrption_t, &node, p_subtmp->type_support,
                p_subtmp->topic_name));
        } else {
//            log_w("no qos, use default subscrption");
            RCCHECK(rclc_subscription_init_default(
                &p_subtmp->subscrption_t, &node, p_subtmp->type_support,
                p_subtmp->topic_name));
        }

        p_subtmp = p_subtmp->ptnext;

        if (++subscrption_register_count > RMW_UXRCE_MAX_SUBSCRIPTIONS) {
//            log_w("the max subscriptions is %d, subscription create error",
//                  RMW_UXRCE_MAX_SUBSCRIPTIONS);
        }
    }
    LOG_DEBUG("subscrption_register %d , the max is %d\r\n", subscrption_register_count, RMW_UXRCE_MAX_SUBSCRIPTIONS);

    /* service *************************************************************/
    while (p_srvtmp) {
        if (p_srvtmp->qos == BEST) {
            RCCHECK(rclc_service_init_best_effort(&p_srvtmp->service_t, &node,
                                                  p_srvtmp->type_support,
                                                  p_srvtmp->service_name));
        } else if (p_srvtmp->qos == DEFAULT) {
            RCCHECK(rclc_service_init_default(&p_srvtmp->service_t, &node,
                                              p_srvtmp->type_support,
                                              p_srvtmp->service_name));
        } else {
//            log_w("no qos, use default service");
            RCCHECK(rclc_service_init_default(&p_srvtmp->service_t, &node,
                                              p_srvtmp->type_support,
                                              p_srvtmp->service_name));
        }

        p_srvtmp = p_srvtmp->ptnext;

        if (++service_register_count > RMW_UXRCE_MAX_SERVICES) {

        }
    }
    LOG_DEBUG( "service_register %d , the max is %d\r\n", service_register_count, RMW_UXRCE_MAX_SERVICES);

    /* executor **************************************************************/
    // Create a executor
    //服务加订阅
    RCCHECK(rclc_executor_init(
        &executor, &support.context,
        service_register_count + subscrption_register_count + 1, &allocator));

    /* add subcriber to executor *********************************************/
    p_subtmp = g_subscrption;

    while (p_subtmp) {
        RCCHECK(rclc_executor_add_subscription(
            &executor, &p_subtmp->subscrption_t, p_subtmp->ros_message,
            p_subtmp->callback, ON_NEW_DATA));

        p_subtmp = p_subtmp->ptnext;
    }

    /* add service to executor ***********************************************/
    p_srvtmp = g_service;

    while (p_srvtmp) {
        if (p_srvtmp->callback) {
            RCCHECK(rclc_executor_add_service(
                &executor, &p_srvtmp->service_t, p_srvtmp->request_msg,
                p_srvtmp->response_msg, p_srvtmp->callback));
        }

        p_srvtmp = p_srvtmp->ptnext;
    }

    if (is_uros_rclc_error_happened) {
        LOG_DEBUG("pal_uros_create_entities failed, destroy_entities..\r\n");
        pal_uros_destroy_entities();
        is_uros_init_successful = false;

    } else {
        LOG_DEBUG("pal_uros_create_entities success.\r\n");
        is_uros_init_successful = true;
    }
}

void pal_uros_destroy_entities(void) {
    is_uros_init_successful = false;
    osDelay(1500);

    RCDESTORYCHECK(rclc_executor_fini(&executor));

    ppublisher p_pubtmp = g_publisher;

    while (p_pubtmp) {
        RCDESTORYCHECK(rcl_publisher_fini(&p_pubtmp->publisher_t, &node));

        p_pubtmp = p_pubtmp->ptnext;
    }

    psubscrption p_subtmp = g_subscrption;

    while (p_subtmp) {
        RCDESTORYCHECK(rcl_subscription_fini(&p_subtmp->subscrption_t, &node));

        p_subtmp = p_subtmp->ptnext;
    }

    pservice p_srvtmp = g_service;

    while (p_srvtmp) {
        RCDESTORYCHECK(rcl_service_fini(&p_srvtmp->service_t, &node));

        p_srvtmp = p_srvtmp->ptnext;
    }

    RCDESTORYCHECK(rcl_timer_fini(&ipc2uros_timer));
    RCDESTORYCHECK(rcl_node_fini(&node));
    RCDESTORYCHECK(rclc_support_fini(&support));

    uros_ping_ts = osKernelGetTickCount() + UROS_PING_PERIOD_MS;
}

void pal_uros_sync(void) {
    int64_t nanos;
    struct timespec ts;
    time_t time_second;
    struct tm *time_utc;

    ///< 正常同步时间：5 ms
    if (rmw_uros_sync_session(100) == RMW_RET_OK) {
        nanos = rmw_uros_epoch_nanos();
        ts.tv_sec =
            (int32_t)(nanos / 1000000000) + 8 * 3600;  ///< 北京时间时区：东八区
        ts.tv_nsec = (uint32_t)(nanos % 1000000000);

        /// set rtc
        time_second = ts.tv_sec;
        time_utc = gmtime(&time_second);
//        bsp_rtc_set(time_utc->tm_year + 1900, time_utc->tm_mon + 1,
//                    time_utc->tm_mday, time_utc->tm_hour, time_utc->tm_min,
//                    time_utc->tm_sec,
//                    (uint16_t)((nanos % 1000000000) / 1000000));

//        log_i(
//            "pal_uros time sync ok, current time(beijing): %d-%d-%d "
//            "%02d:%02d:%02d.%03d",
//            time_utc->tm_year + 1900, time_utc->tm_mon + 1, time_utc->tm_mday,
//            time_utc->tm_hour, time_utc->tm_min, time_utc->tm_sec,
//            ts.tv_nsec / 1000000);
    } else {
//        log_i("pal_uros time sync error!");
    }
}

/// set timestamp of topic
void pal_uros_msg_set_timestamp(builtin_interfaces__msg__Time *ts) {
    int64_t nanos = rmw_uros_epoch_nanos();
    // ts->sec = (int32_t)(nanos / 1000000000) + 8 * 3600;       ///<
    // 北京时间时区：东八区
    ts->sec = (int32_t)(nanos / 1000000000);  ///< 消息发布使用 UTC时间
    ts->nanosec = (uint32_t)(nanos % 1000000000);
}

///< single thread topic handle
static void ipc_handle(void) {
    ppublisher p_pubtmp = g_publisher;
    while (p_pubtmp) {
        if (osMessageQueueGet(p_pubtmp->mq_id, p_pubtmp->ros_message, NULL,
                              0) == osOK) {
//            log_d("get topic %s ready to publish", p_pubtmp->topic_name);
            p_pubtmp->publish_ret =
                rcl_publish(&p_pubtmp->publisher_t,
                            (const void *) p_pubtmp->ros_message, NULL);

            if (p_pubtmp->publish_ret != RCL_RET_OK) {
//                log_w("topic_name: %s publish ERROR", p_pubtmp->topic_name);
            } else {
//                log_d("topic_name: %s publish OK", p_pubtmp->topic_name);
            }
        }

        p_pubtmp = p_pubtmp->ptnext;
    }
}

void pal_uros_print_ts(void) {
    int64_t nanos;
    struct timespec ts;

    time_t time_second;
    struct tm *time_utc;

    nanos = rmw_uros_epoch_nanos();
    ts.tv_sec =
        (int32_t)(nanos / 1000000000) + 8 * 3600;  ///< 北京时间时区：东八区
    ts.tv_nsec = (uint32_t)(nanos % 1000000000);

    time_second = ts.tv_sec;
    time_utc = gmtime(&time_second);

//    log_i("pal_uros_print_ts: %d-%d-%d %02d:%02d:%02d.%03d",
//          time_utc->tm_year + 1900, time_utc->tm_mon + 1, time_utc->tm_mday,
//          time_utc->tm_hour, time_utc->tm_min, time_utc->tm_sec,
//          ts.tv_nsec / 1000000);
}

void pal_all_init(){
	init_call *init_ptr = &_pal_start;
    for (; init_ptr < &_pal_end; init_ptr++) {
        (*init_ptr)();
    }
}