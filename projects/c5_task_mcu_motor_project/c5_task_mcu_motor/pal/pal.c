/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: pal.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Protocol abstraction layer interface entry
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "pal.h"
#include <rcl/rcl.h>
#include "pal_uros.h"
#include "std_msgs/msg/u_int8.h"
#include "std_msgs/msg/u_int32.h"
#include "std_msgs/msg/bool.h"
#include "std_msgs/msg/string.h"
#include "cJSON.h"

#define LOG_TAG "pal"
#include "elog.h"

#define pub_err_code
#define log_microros_sub
#define log_cjson
//#define mode_detection
#define microros_response
//#define check_and_repair
#define ops_again_after_err
#define disconnect_detection

#define push_rod_wash     0
#define push_rod_dust_box 1
#define side_brush        2
#define roll_brush        3
#define clean_water_pump  4
#define clean_water_valve 5
#define fan               6
#define dev_max_num       7

#define wash_mode      0
#define dust_push_mode 1

#define side_brush_ops_again_max 15

typedef struct {
    uint16_t Hardware_overcurrent : 1;
    uint16_t Locked_Rotor : 1;
    uint16_t Mosfet_H : 1;
    uint16_t Mosfet_L : 1;
    uint16_t StartUp_Low_voltage : 1;
    uint16_t prechage : 1;
    uint16_t over_voltage : 1;
    uint16_t low_voltage : 1;
    uint16_t soft_overcurrent : 1;
    uint16_t phase : 1;
    uint16_t b10 : 1;
    uint16_t b11 : 1;
    uint16_t b12 : 1;
    uint16_t b13 : 1;
    uint16_t b14 : 1;
    uint16_t b15 : 1;
} fan_error_code;

typedef union {
    uint16_t       error_code;
    fan_error_code fan_error_message;
} motor_error_code;

typedef union {
    int  speed;
    bool status;
} motor_value;

typedef struct {
    motor_value      set;
    motor_value      get;
    motor_error_code err_code;
    bool             overflag;
    bool             ops_again;
    uint8_t          ops_again_num;
} motor_message;

typedef struct {
    char *      name;
    motor_value value;
    bool        overflag;
} json_recv_object;

typedef struct {
    char *name;
    char *string;
    bool  overflag;
} json_sent_object;

typedef struct {
    void *           parent;
    json_recv_object message_get;
    json_sent_object message_sent;
    uint8_t          connection_interval;
    uint32_t         task_main_heartbeat_curr_time;
} Notifier;

typedef struct {
    void *        parent;
    motor_message motor[dev_max_num];
    const char *(*motor_name_index);
    uint8_t motor_curr_index;
    uint8_t mode;
} Controller;

const char *mator_name[dev_max_num] = {"push_rod_wash",    "push_rod_dust_box", "side_brush", "roll_brush",
                                       "clean_water_pump", "clean_water_state", "fan"};
Notifier    microros_proxy;
Controller  motor_proxy = {
    .motor_name_index = mator_name,
};

cJSON_Hooks cjson_hooks;

uint32_t fan_speed_get(uint8_t id);
uint32_t fan_error_status_get(uint8_t clean_dev_id);
void     update_motor_status(void);
void     update_motor_err_code(void);

static publisher      g_task_motor_get;
std_msgs__msg__String task_motor_get;

/* subscrption ops */
static subscrption    g_task_motor_set;
std_msgs__msg__String task_motor_set;
void                  task_motor_set_info(const void *);

static subscrption   g_task_main_heartbeat_set;
std_msgs__msg__UInt8 task_main_heartbeat_set;
void                 task_main_heartbeat_set_info(const void *);

void g_subscrption_init(void) {
    /* subscrption ops */
    subscrption_init(&g_task_motor_set, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/task_motor/set", &task_motor_set, DEFAULT,
                     task_motor_set_info);
    /* creat molloc */
    MallocString(&task_motor_set.data, REQ, 256);

    subscrption_init(&g_task_main_heartbeat_set, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/c5_task_mcu_main_heartbeat",
                     &task_main_heartbeat_set, BEST, task_main_heartbeat_set_info);
}

void g_publisher_init(void) {
    /* publisher */
    publisher_init(&g_task_motor_get, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/task_motor/get", &task_motor_get, BEST, SEND,
                   sizeof(std_msgs__msg__String));
}

void pal_ctl_thread(void *parameter) {
    uint8_t        i;
    static uint8_t time         = 0;
    uint8_t        side_errCode = 0;

    /* init cjson */
    cjson_hooks.malloc_fn = pvPortMalloc;
    cjson_hooks.free_fn   = vPortFree;
    cJSON_InitHooks(&cjson_hooks);

    while (1) {
        /* entry per second */
        if (time % 10 == 0) {
#ifdef disconnect_detection
#if 0
        /* calculation connection interval */
        if (microros_proxy.connection_interval) {
            microros_proxy.connection_interval--;
        } else {
            /* disconnection,check if device is running */
            for (i = 0; i < dev_max_num; i++)
                if (motor_proxy.motor[i].get.speed)
                    clean_ctrl_all_reset();
        }
#else
            if (syncStatus != SYNC_STATUS_SUCCEED ||
                clock_cpu_millisecond_diff(microros_proxy.task_main_heartbeat_curr_time, clock_cpu_gettime()) > 5000) {
                if (clock_cpu_millisecond_diff(microros_proxy.task_main_heartbeat_curr_time, clock_cpu_gettime()) > 5000) {
                    microros_proxy.task_main_heartbeat_curr_time = clock_cpu_gettime();
                    log_e("task main heartbeat receive timeout!");
                }
                /* disconnection,check if device is running */
                for (i = 0; i < dev_max_num; i++) {
                    if (motor_proxy.motor[i].get.speed) {
                        log_e("microros failed,stop all device!");
                        clean_ctrl_all_reset();
                        osDelay(8000);
                    }
                }
            }
#endif
#endif
            /* get speed and status */
            update_motor_status();
#ifdef log_device_get
            log_d("push_rod_wash:%d,\tpush_rod_dust_box:%d,\tside_brush:%d,\troll_brush:%d", motor_proxy.motor[push_rod_wash].get.speed,
                  motor_proxy.motor[push_rod_dust_box].get.speed, motor_proxy.motor[side_brush].get.speed,
                  motor_proxy.motor[roll_brush].get.speed);
            log_d("clean_water_pump:%d,\tclean_water_valve:%d,\tfan:%d", motor_proxy.motor[clean_water_pump].get.speed,
                  motor_proxy.motor[clean_water_valve].get.speed, motor_proxy.motor[fan].get.speed);
#endif
            /* get err */
            update_motor_err_code();
#ifdef log_err_code
            log_d("push_rod_wash_err:%d,\tpush_rod_dust_box_err:%d,\tside_brush_err:%d,\troll_brush_err:%d",
                  motor_proxy.motor[push_rod_wash].err_code.error_code, motor_proxy.motor[push_rod_dust_box].err_code.error_code,
                  motor_proxy.motor[side_brush].err_code.error_code, motor_proxy.motor[roll_brush].err_code.error_code);
            log_d("clean_water_pump_err:%d,\tclean_water_valve_err:%d,\tfan_err:%d", motor_proxy.motor[clean_water_pump].err_code.error_code,
                  motor_proxy.motor[clean_water_valve].err_code.error_code, motor_proxy.motor[fan].err_code.error_code);
#endif

#ifdef ops_again_after_err
            /* logic */
            if (time % 20 == 0) {
                /* get err code */
                side_errCode = (clean_device_error_status_get(ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA) << 1) |
                               clean_device_error_status_get(ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA);

                /* error resume */
                if (!side_errCode) {
                    motor_proxy.motor[side_brush].ops_again           = 1;
                    motor_proxy.motor[side_brush].ops_again_num       = side_brush_ops_again_max;
                    motor_proxy.motor[side_brush].err_code.error_code = 0;
                }

                if (!motor_proxy.motor[side_brush].ops_again_num) {
                    motor_proxy.motor[side_brush].ops_again = 0;
                }
                /* motor handle */
                for (i = 0; i < dev_max_num; i++) {
                    if (side_errCode && motor_proxy.motor[i].ops_again) {
                        clean_ops_test(i, motor_proxy.motor[i].set.speed);
                        motor_proxy.motor[i].ops_again_num--;
                        log_d("remain ops again num:%d", motor_proxy.motor[i].ops_again_num);
                    }
                }
            }
#endif

#ifdef check_and_repair
            /* check and repair */
            for (i = 0; i < dev_max_num; i++)
                if (motor_proxy.motor[motor_proxy.motor_curr_index].set.speed != motor_proxy.motor[motor_proxy.motor_curr_index].get.speed)
                    clean_ops_test(motor_proxy.motor_curr_index, motor_proxy.motor[motor_proxy.motor_curr_index].set.speed);
#endif

#ifdef pub_err_code
            /* creat json */
            cJSON *root;
            root = cJSON_CreateObject();
            if (root != NULL) {
                cJSON_AddNumberToObject(root, "push_rod_wash_err", motor_proxy.motor[0].err_code.error_code);
                cJSON_AddNumberToObject(root, "push_rod_dust_box_err", motor_proxy.motor[1].err_code.error_code);
                cJSON_AddNumberToObject(root, "side_brush_err", motor_proxy.motor[2].err_code.error_code);
                cJSON_AddNumberToObject(root, "roll_brush_err", motor_proxy.motor[3].err_code.error_code);
                cJSON_AddNumberToObject(root, "clean_water_pump_err", motor_proxy.motor[4].err_code.error_code);
                cJSON_AddNumberToObject(root, "clean_water_state_err", motor_proxy.motor[5].err_code.error_code);
                cJSON_AddNumberToObject(root, "fan_err", motor_proxy.motor[6].err_code.error_code);
                char *rendered = cJSON_Print(root);
                if (rendered != NULL) {
                    // log_d("json object:%s", rendered);
                    /* publisher */
                    task_motor_get.data.data = rendered;
                    task_motor_get.data.size = strlen(rendered);
                    message_publish(&g_task_motor_get);
                    cJSON_free(rendered);
                }
                cJSON_Delete(root);
            }
#endif
        }
        time++;
        osDelay(100);
    }
}
/******************************************************************************
 * @Function: pal_init
 * @Description: Protocol abstraction layer init function
 * @Input: void
 * @Output: None
 * @Return: int
 * @Others: None
 *******************************************************************************/
int pal_init(void) {
    return 0;
}

/******************************************************************************
 * @Function: pal_DeInit
 * @Description: Protocol abstraction layer Deinitialization function
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 *******************************************************************************/
int pal_DeInit(void) {
    return 0;
}

void task_motor_set_info(const void *msgin) {
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *) msgin;
    /* print the get json */
#ifdef log_cjson
    log_d("Yes,I get your message");
    log_d("json:%s", msg->data.data);
#endif
    /* parse json */
    // char* s = "{\n\t\"name\":\t\"side_brush\",\n\t\"value\":\t50\n}";
    if (msg->data.data == NULL)
        return;
    cJSON *root = cJSON_Parse(msg->data.data);
    if (root != NULL) {
        cJSON *json_name = cJSON_GetObjectItemCaseSensitive(root, "name");
        if (json_name != NULL) {
            microros_proxy.message_get.name = json_name->valuestring;
        } else {
            /* free root object */
            cJSON_Delete(root);
            return;
        }

        cJSON *json_value = cJSON_GetObjectItemCaseSensitive(root, "value");
        if (json_value != NULL) {
            microros_proxy.message_get.value.speed = (int) ((json_value->valuedouble) * 100.0);
        } else {
            /* free root object */
            cJSON_Delete(root);
            return;
        }

#ifdef log_microros_sub
        log_d("type:%x", cJSON_GetObjectItem(root, "name")->type);
        log_d("name:%s", microros_proxy.message_get.name);
        log_d("value:%d", microros_proxy.message_get.value.speed);
#endif
        /* index motor */
        motor_proxy.motor_name_index = mator_name;
        while ((motor_proxy.motor_name_index <= &mator_name[dev_max_num - 1]) &&
               strcmp(microros_proxy.message_get.name, *motor_proxy.motor_name_index) != 0)
            motor_proxy.motor_name_index++;
        /* device is not exist */
        if (motor_proxy.motor_name_index > &mator_name[dev_max_num - 1]) {
#ifdef log_microros_sub
            log_e("device is not exist!");
#endif
            /* free root object */
            cJSON_Delete(root);
            return;
        }
        /* get the device id */
        motor_proxy.motor_curr_index = motor_proxy.motor_name_index - mator_name;

#ifdef log_microros_sub
        log_d("device name:%s", mator_name[motor_proxy.motor_curr_index]);
#endif
        /* update status */
        update_motor_status();
#ifdef mode_detection
        /* mode handle */
        if (motor_proxy.motor_curr_index == push_rod_dust_box) {
            /* is wash component running? */
            for (uint8_t i = 0; i < dev_max_num; i++) {
                /* ignore push rod dust box */
                if (i == push_rod_dust_box)
                    continue;

                if (motor_proxy.motor[i].set.speed) {
                    /* wash component running,keep wash mode */
                    motor_proxy.mode = wash_mode;
                    break;
                } else {
                    if (i == (dev_max_num - 1)) {
                        /* no cleaning components are running,change to dust push mode */
                        motor_proxy.mode = dust_push_mode;
                    }
                }
            }
        } else if (motor_proxy.motor_curr_index != push_rod_dust_box) {
            /* is push rod dust box running ? */
            if (motor_proxy.motor[push_rod_dust_box].set.status)
                motor_proxy.mode = dust_push_mode;  // is running,keep dust push mode
            else
                motor_proxy.mode = wash_mode;  // is close,change to wash mode
        }
        /* mode judge */
        if (motor_proxy.mode == wash_mode) {
            /* It is wash mode,It is not allowed to open the washing mode component */
            if (motor_proxy.motor_curr_index == push_rod_dust_box) {
                log_d("device name:%s is ignored!", mator_name[motor_proxy.motor_curr_index]);
                return;
            }
        } else if (motor_proxy.mode == push_rod_dust_box) {
            /*It is dust push mode,It is not allowed to open washing mode component*/
            if (motor_proxy.motor_curr_index != push_rod_dust_box) {
                log_d("device name:%s is ignored!", mator_name[motor_proxy.motor_curr_index]);
                return;
            }
        }
#endif

#ifdef disconnect_detection
        /* set connect interval */
        microros_proxy.connection_interval = 10;  // set connect interval 10s
#endif
        if (motor_proxy.motor_curr_index == push_rod_wash || motor_proxy.motor_curr_index == push_rod_dust_box) {
            /* save to motor_proxy */
            if (motor_proxy.motor[motor_proxy.motor_curr_index].set.speed != microros_proxy.message_get.value.speed) {
                motor_proxy.motor[motor_proxy.motor_curr_index].set.speed = microros_proxy.message_get.value.speed;
                clean_ops_test(motor_proxy.motor_curr_index, motor_proxy.motor[motor_proxy.motor_curr_index].set.speed);
            } else if (motor_proxy.motor[motor_proxy.motor_curr_index].set.speed != motor_proxy.motor[motor_proxy.motor_curr_index].get.speed) {
                clean_ops_test(motor_proxy.motor_curr_index, motor_proxy.motor[motor_proxy.motor_curr_index].set.speed);
            }
        } else {
#ifdef ops_again_after_err
            if (motor_proxy.motor_curr_index == side_brush) {
                if (microros_proxy.message_get.value.speed) {
                    motor_proxy.motor[side_brush].ops_again           = 1;
                    motor_proxy.motor[side_brush].ops_again_num       = side_brush_ops_again_max;
                    motor_proxy.motor[side_brush].err_code.error_code = 0;
                } else {
                    motor_proxy.motor[side_brush].ops_again     = 0;
                    motor_proxy.motor[side_brush].ops_again_num = 0;
                }
            }
#endif
            motor_proxy.motor[motor_proxy.motor_curr_index].set.speed = microros_proxy.message_get.value.speed;
            clean_ops_test(motor_proxy.motor_curr_index, motor_proxy.motor[motor_proxy.motor_curr_index].set.speed);
        }

#ifdef microros_response
        /* response same json */
        char *rendered = cJSON_Print(root);
        if (rendered != NULL) {
            task_motor_get.data.data = rendered;
            task_motor_get.data.size = strlen(rendered);
            message_publish(&g_task_motor_get);
            cJSON_free(rendered);
        }

#endif
        /* free root object */
        cJSON_Delete(root);
    }
    return;
}

void task_main_heartbeat_set_info(const void *msgin) {
    const std_msgs__msg__UInt8 *msg = (const std_msgs__msg__UInt8 *) msgin;
    (void) msg;
    // log_d("task main heartbeat...");
    microros_proxy.task_main_heartbeat_curr_time = clock_cpu_gettime();
}

void update_motor_status(void) {
    motor_proxy.motor[push_rod_wash].get.speed     = push_rod_motor_status_get(0);
    motor_proxy.motor[push_rod_dust_box].get.speed = push_rod_motor_status_get(1);
    motor_proxy.motor[side_brush].get.speed        = side_brush_motor_status_get(0);
    motor_proxy.motor[roll_brush].get.speed        = roll_brush_motor_speed_get(0);
    motor_proxy.motor[clean_water_pump].get.speed  = water_distribution_pump_speed_get(0);
    motor_proxy.motor[clean_water_valve].get.speed = water_valve_clean_status_get(0);
    motor_proxy.motor[fan].get.speed               = fan_speed_get(0);
}
void update_motor_err_code(void) {
    /* get err */
    motor_proxy.motor[push_rod_wash].err_code.error_code     = clean_device_error_status_get(ID_PUSH_ROD_MOTOR_1_ERR_STA);
    motor_proxy.motor[push_rod_dust_box].err_code.error_code = clean_device_error_status_get(ID_PUSH_ROD_MOTOR_2_ERR_STA);
#ifdef ops_again_after_err
    if (!motor_proxy.motor[side_brush].ops_again)
        motor_proxy.motor[side_brush].err_code.error_code = (clean_device_error_status_get(ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA) << 1) |
                                                            clean_device_error_status_get(ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA);
#else
    motor_proxy.motor[side_brush].err_code.error_code = (clean_device_error_status_get(ID_SIDE_BURSH_MOTOR_LEFT_ERR_STA) << 1) |
                                                        clean_device_error_status_get(ID_SIDE_BRUSH_MOTOR_RIGHT_ERR_STA);
#endif
    motor_proxy.motor[roll_brush].err_code.error_code        = clean_device_error_status_get(ID_ROLL_MOTOR_ERR_STA);
    motor_proxy.motor[clean_water_pump].err_code.error_code  = clean_device_error_status_get(ID_WATER_DIST_ERR_STA);
    motor_proxy.motor[clean_water_valve].err_code.error_code = clean_device_error_status_get(ID_WATER_VALVE_CLEAN_ERR_STA);
    motor_proxy.motor[fan].err_code.error_code               = fan_error_status_get(0);
}

void ops_again_test(void) {}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ops_again_test, ops_again_test, ops_again_test);