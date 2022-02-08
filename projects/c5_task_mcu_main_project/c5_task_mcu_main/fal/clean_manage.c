#include "log.h"
#include "clean_ctrl.h"
#include "app_led.h"
#include "fal_key.h"
#include "shell.h"
#include "bsp_cputime.h"
#include "string.h"
#include "clean_def.h"

#include "cJSON.h"
#include <rcl/rcl.h>
#include "pal_uros.h"
#include "std_msgs/msg/string.h"
#include "std_msgs/msg/bool.h"
#include "geometry_msgs/msg/twist.h"
#include "std_msgs/msg/u_int8.h"
#include "std_msgs/msg/empty.h"
#include "chassis_interfaces/msg/navi_work_status.h"
#include "fal_workstation.h"
#include <math.h>
#define LOG_TAG "clean_manage"
#include "elog.h"

#define SYS_SET_BIT(val, bit) ((val) |= (1 << bit))    /* 设定数据bit */
#define SYS_GET_BIT(val, bit) ((val >> bit) & 1)       /* 获取数据bit */
#define SYS_CLR_BIT(val, bit) ((val) &= (~(1 << bit))) /* 清除数据bit */
#define CM_RCCHECK(ret, Target)                                                      \
    {                                                                                \
        if ((temp_rc != Target)) {                                                   \
            log_d("Failed status on line %d: %d. Aborting.\n", __LINE__, (int) ret); \
        }                                                                            \
    }
#define WATER_OPEN_THRESHOLD_VALUE     18                         /* 开水阈值，已乘100 */
#define WATER_CLOSE_THRESHOLD_VALUE    14                         /* 关水阈值，已乘100 */               
#define FLOAT_EQUAL_DIFF_VALUE         0.005              
#define ROLL_BRUSH_MOTOR_DEFAULT_VALUE 0.71                         /* 滚刷默认设定值 */
#define FAN_DEFAULT_VALUE              0.80                         /* 风机默认设定值 */
#define SIDE_BRUSH_MOTOR_DEFAULT_VALUE 0.30                         /* 边刷默认设定值0.60 */
#define CLOTH_WATER_PUMP_DEFAULT_VALUE 0.42                         /* 布水水泵默认设定值 */
#define THREAD_DELAY_TIME              10                           /* 线程切出时间 */
#define STOP_DELAY_TIME_ZERO           (200 / THREAD_DELAY_TIME)    /* 过滤水泵与布水水泵开关间隔*/
#define STOP_DELAY_TIME_ONE            (400 / THREAD_DELAY_TIME)    /* 布水水泵与水阀关闭延迟200*/
#define STOP_DELAY_TIME_TWO            (5000 / THREAD_DELAY_TIME)   /* 边刷与风机关闭延迟5000 */
#define STOP_DELAY_TIME_THREE          (10000 / THREAD_DELAY_TIME)
#define START_DELAY_TIME_ONE           (100 / THREAD_DELAY_TIME)    /* 风机与清水阀启动间隔 */
#define START_DELAY_TIME_TWO           (400 / THREAD_DELAY_TIME)    /* 清水阀与布水水泵启动间隔*/   
#define START_DELAY_TIME_THREE         (200 / THREAD_DELAY_TIME)    /* 布水水泵与过滤水泵启动间隔*/ 
#define START_DELAY_TIME_FOUR          (3000 / THREAD_DELAY_TIME)   /* 布水水泵与洗地推杆启动间隔*/ 
#define START_DELAY_TIME_FIVE          (2000 / THREAD_DELAY_TIME)   /* 洗地推杆与边刷启动间隔 */  

#define OPS_WATER_DELAY_TIME           ((500 + START_DELAY_TIME_FOUR) / THREAD_DELAY_TIME) /* 收水操作延迟*/ 
#define WATER_LEVLE_DET_DEBOUNCE_COUNT 3  /* 水循环系统开启次数统计*/
#define WAIT_ACK_TIME                  10 /* cyl:unit:ms */
#define WAIT_ACK_TIME_MAX              10 /* cyl:unit:10ms */
//#define DUST_MODE_OPEN_MOTOR            /* 尘推模式是否滚刷跟边刷 */
//#define soft_key	/* cyl:key funct test */
#define drop_tolerance_max  15                            /* cyl:resume times */
#define drop_log                                          /* cyl:drop log */
#define wheel_vel           (clean_ctrl_comp_info.linear) /* cyl:wheel speed */
#define rotationl_speed     (clean_ctrl_comp_info.angular)
#define cur_mode            (clean_ctrl_comp_info.state) /* cyl:current mode */
#define water_ctl_st_status 10
#define CLEAN_KEYBOARD_STATE_FEEKBACK
//#define send_fb_cmp_log /* cyl:send feedback compare logger */
typedef uint8_t (*clean_ctrl_fun)(void *object, uint8_t state); /* 状态机转变控制函数 */

/* 电控Mcu设备枚举 */
enum {
    ID_CLEAN_PUTTER_MOTOR = 0,
    ID_DUST_PUTTER_MOTOR,
    ID_SIDE_BRUSH_MOTOR,
    ID_ROLL_BURSH_MOTOR,
    ID_CLOTH_WATER_PUMP,
    ID_CLEAR_WATER_VALVE,
    ID_FAN_MOTOR,
    ID_FILTER_PUMP,
    ID_WASTE_WATER_VALVE,
    ID_SEWAGE_WATER_VALVE,
    ID_DEV_MAX,
    ID_MODE_NULL,
    ID_MODE_MOPPING,
    ID_MODE_DEDUSTING,
};

/* 组件控制状态 */
enum {
    STATE_INIT = 0,
    STATE_DUST_START,
    STATE_WASH_START,
    STATE_IMMEDIATELY_STOP,
    STATE_NORMAL_STOP,
    STATE_WATER_OPEN,
    STATE_WATER_CLOSE,
    STATE_MAX
};

/* 组件控制状态 */
enum {
    DEV_OPS_STEP_0 = 0,
    DEV_OPS_STEP_1,
    DEV_OPS_STEP_1_1,
    DEV_OPS_STEP_1_2,
    DEV_OPS_STEP_2,
    DEV_OPS_STEP_3,
    DEV_OPS_STEP_4,
    DEV_OPS_STEP_5,
    DEV_OPS_STEP_6,
    DEV_OPS_STEP_7,
    DEV_OPS_STEP_8,
    DEV_OPS_STEP_9,
    DEV_OPS_STEP_10,
    DEV_OPS_STEP_11,
    DEV_OPS_STEP_12,
    DEV_OPS_STEP_13,
    DEV_OPS_STEP_14,
    DEV_OPS_STEP_15,
    DEV_OPS_STEP_16,
    DEV_OPS_STEP_17,
    DEV_OPS_STEP_18
};

/* 控制操作状态 */
enum { OPS_WAIT_COMPLETION = 0, OPS_COMPLETION };

/* 安全状态标志 */
enum { EMERG_ERROR_FLAG = 0, TOUCH_EDGE_0_FLAG };

/* 导航工作状态 */
enum { NAV_STATUS_IDLE = 0, NAV_STATUS_TASK, NAV_STATUS_CHARGE };

/* cyl:ack detection enum */
typedef enum { ACK_LOSS, ACK_OK } motor_ack;
enum { LED_TRIGLE_NONE = 0, LED_TRIGLE_EMERG = 1, LED_TRIGLE_WORK = 2, LED_TRIGLE_CHARGE = 3, LED_TRIGLE_IDLE = 4 };
extern uint8_t func_button_status[BUTTON_DEFAULT_MAX];

/* 清洁组件设备控制信息 */
typedef struct clean_ctrl_component {
    char *   model;                          /* 清洁组件模式名称 */
    uint8_t  mode;                           /* 控制模式，洗地或尘推，充电等*/
    uint8_t  last_mode;                      /* 记录任务上一次下发的工作模式 */
    uint32_t last_recv_time;                 /* 记录上一次接受时间，用于超时计算*/
    float    last_dev_set_value[ID_DEV_MAX]; /* 上一次设定组件控制值 */
    float    dev_set_value[ID_DEV_MAX];      /* 平台组件控制设定值 */
    float    mcu_set_dev_value[ID_DEV_MAX];  /* 组件自身设定值*/
    float    dev_fb_value[ID_DEV_MAX];       /* 组件反馈控制值 */
    int      dev_err_value[ID_DEV_MAX];      /* 组件错误码 */
    int      last_dev_err_value[ID_DEV_MAX]; /* 上一次清洁组件错误信息 */
    uint32_t last_fb_time;                   /* 记录上一次电控反馈时间 */

    /* cyl:ack detection */
    int  drop_num;           /*cyl:packet loss num */
    bool reconnect_for_drop; /* cyl:agent reconnect,resent */
    /*控制状态机及对应函数*/
    uint8_t        state;                     /* 控制功能状态机 */
    uint8_t        last_state;                /* 控制功能状态机上一次状态记录 */
    uint8_t        last_last_state;
    uint8_t        is_enable[STATE_MAX];      /* 控制功能启动标志，是否使能有效*/
    clean_ctrl_fun run_fun[STATE_MAX];        /* 控制功能函数 */
    uint32_t       start_or_stop_delay_count; /* 状态时间计数 */
    uint32_t       ops_water_delay_count;     /* 开水跟收水延迟时间计数 */
    uint8_t        dev_switch_state;          /* 每种状态控制设备状态切换 */

    /* 按键值及状态是否改变 */
    uint8_t button_value[BUTTON_DEFAULT_MAX];

    uint8_t status_changed; /* 只有状态改变时才触清洁功能 */

    uint32_t water_level_info[2];  /* 水位状态 */
    uint8_t  enable_filter_system; /* 使能过滤系统水循环 */

    uint32_t secure_error_flag; /* 安全错误标志 */
    bool     emerg_flag;        /* 急停标志 */
    bool     touch_edge_0_flag; /* 安全出边标志 */

    /* 速度信息 */
    float linear;  /* 线速度 */
    float angular; /* 角速度 */

    uint8_t nav_work_status; /* 记录导航工作状态 */

    uint8_t water_system_state; /*当前水系统状态*/
    uint8_t last_water_system_state;   /* 上一次水系统状态 */
} clean_ctrl_component_t;

/* 电控Mcu设备JSON名 */
static char *mcu_dev_name[ID_DEV_MAX] = {"push_rod_wash",     "push_rod_dust_box", "side_brush", "roll_brush",
                                         "clean_water_pump",  "clean_water_state", "fan",        "filter_water_pump",
                                         "waste_water_state", "sewage_water_valve"};
/* Mcu设备错误JSON名 */
static char *mcu_dev_name_error_zh[ID_DEV_MAX] = {"洗地推杆异常", "尘推推杆异常", "边刷异常",     "滚刷异常",     "布水水泵异常",
                                                  "清水水阀异常", "风机异常",     "过滤水泵异常", "过滤水阀异常", "排污球阀异常"};
/* Mcu设备恢复正常JSON名 */
static char *mcu_dev_name_zh[ID_DEV_MAX] = {"洗地推杆恢复正常", "尘推推杆恢复正常", "边刷恢复正常",     "滚刷恢复正常",     "布水水泵恢复正常",
                                            "清水水阀恢复正常", "风机恢复正常",     "过滤水泵恢复正常", "过滤水阀恢复正常", "排污球阀恢复正常"};

/* 电控Mcu设备错误反馈JSON名 */
static char *mcu_fb_error_name[ID_DEV_MAX] = {"push_rod_wash_err",     "push_rod_dust_box_err", "side_brush_err", "roll_brush_err",
                                              "clean_water_pump_err",  "clean_water_state_err", "fan_err",        "filter_water_pump_err",
                                              "waste_water_state_err", "sewage_water_valve_err"};
/* 组件控制信息 */
static clean_ctrl_component_t clean_ctrl_comp_info = {0};

/* 清洁控制信息 */
static subscrption    g_clean_component_ctrl;
std_msgs__msg__String clean_component_ctrl_info;
void                  get_clean_component_ctrl_info(const void *msgin);
/*速度 */
static subscrption        g_robot_vel;
geometry_msgs__msg__Twist robot_vel_info;
void                      get_robot_vel_info(const void *msgin);

/* 急停 */
static subscrption  g_emerg_info;
std_msgs__msg__Bool emerg_info;
void                get_emerg_info(const void *msgin);

/* 安全触边 */
static subscrption  g_touch_edge_0;
std_msgs__msg__Bool touch_edge_0_info;
void                get_touch_edge_0_info(const void *msgin);

/* 回冲状态 */
// static subscrption                      g_nav_work_status;
// chassis_interfaces__msg__NaviWorkStatus nav_work_status_info;
// void                                    get_nav_work_info_status(const void *msgin);

/* 电控MCU通讯控制指令 */
static publisher      g_motor_mcu_ctrl;
std_msgs__msg__String motor_mcu_ctrl_info;

/* 自动主动发布主题 */
static publisher     g_work_mode;
std_msgs__msg__UInt8 work_mode;
static subscrption   g_work_mode_update;
std_msgs__msg__Empty work_mode_update;
void                 get_work_mode_update(const void *msgin);

/*  过滤网主动发布主题 */
static publisher    g_strainer;
std_msgs__msg__Bool strainer_info;

/*  充电命令发布 */
static publisher     g_cmd_charge_start;
std_msgs__msg__Empty cmd_charge_start_info;
static publisher     g_cmd_charge_cancel;
std_msgs__msg__Empty cmd_charge_cancel_info;

/* 水位信息发布 */
static publisher      g_water_level_info;
std_msgs__msg__String water_level_info;
static subscrption    g_water_level_info_update;
std_msgs__msg__Empty  water_level_info_update;
void                  get_water_level_info_update(const void *msgin);

/* 电控MCU错误码反馈 */
static subscrption    g_motor_mcu_feedback;
std_msgs__msg__String motor_mcu_feedback_info;
void                  get_motor_mcu_feedback_info(const void *msgin);

/* 清洁设备状态 */
static publisher      g_clean_dev_status;
std_msgs__msg__String clean_dev_status_info;

#ifdef CLEAN_KEYBOARD_STATE_FEEKBACK
/* pub */
static publisher      g_pub_clean_keyboard_state;
std_msgs__msg__String g_clean_keyboard_state;
static char           keyboard_state[128] = {0};
void                  publish_clean_keyboard_state(void);
/* sub */
static subscrption  g_sub_clean_keyboard_update;
std_msgs__msg__Empty g_clean_keyboard_update;
void                get_clean_keyboard_state_update_info(const void *msgin);
#endif

int vel_tmp;

cJSON_Hooks cjson_hooks;

static void clean_water(clean_ctrl_component_t *object);
static void clean_led_control(void);

void motor_mcu_ctrl(uint8_t dev_id, float data) {
    cJSON *root       = NULL;
    char * string_msg = NULL;

    if (dev_id >= ID_DEV_MAX)
        return;
    clean_ctrl_comp_info.mcu_set_dev_value[dev_id] = data;    
    root = cJSON_CreateObject();
    if (root != NULL) {
        cJSON_AddStringToObject(root, "name", mcu_dev_name[dev_id]);
        cJSON_AddNumberToObject(root, "value", data);
        string_msg = cJSON_PrintUnformatted(root);
        if (string_msg == NULL) {
            cJSON_Delete(root);
            log_d("can't create publisher msg on line :%d", __LINE__);
            return;
        }

        motor_mcu_ctrl_info.data.data     = string_msg;
        motor_mcu_ctrl_info.data.size     = strlen(string_msg);
        motor_mcu_ctrl_info.data.capacity = 0;
        message_publish(&g_motor_mcu_ctrl);

        log_d("----->%s", string_msg);
        cJSON_free(string_msg);
        cJSON_Delete(root);
    } else {
        log_d("creat json obj failed on line %d\r\n", __LINE__);
    }
    return;
}

void water_level_info_pub(uint32_t *value, uint8_t len) {
    cJSON *root         = NULL;
    char * string_msg   = NULL;
    float  temp_data[2] = {0};

    if (value == NULL || len < 2)
        return;

    root = cJSON_CreateObject();
    if (root != NULL) {
        temp_data[0] = ((float) value[0]) / 100;
        temp_data[1] = ((float) value[1]) / 100;
        cJSON_AddNumberToObject(root, "clean_status", temp_data[1]);
        cJSON_AddNumberToObject(root, "clean_waring_threshold", 1.0);
        cJSON_AddNumberToObject(root, "waste_status", temp_data[0]);
        cJSON_AddNumberToObject(root, "waste_waring_threshold", 0.0);
        string_msg = cJSON_PrintUnformatted(root);
        if (string_msg != NULL) {
            water_level_info.data.data     = string_msg;
            water_level_info.data.size     = strlen(string_msg);
            water_level_info.data.capacity = 0;
            message_publish(&g_water_level_info);
        }
        if (root != NULL) {
            cJSON_Delete(root);
        }
        if (string_msg != NULL) {
            log_d("----->%s", string_msg);
            cJSON_free(string_msg);
        }
    } else {
        log_d("creat json obj failed on line %d\r\n", __LINE__);
    }
    return;
}

void clean_dev_status_info_pub(uint8_t dev_id, int error_code, uint8_t len) {
    cJSON *root       = NULL;
    char * string_msg = NULL;

    if ((dev_id >= ID_DEV_MAX) || (len == 0 || len > ID_DEV_MAX))
        return;

    root = cJSON_CreateObject();
    if (root != NULL) {
        /* error_num */
        if (error_code) {
            /* log type */
            cJSON_AddStringToObject(root, "log_type", "warm");
            /* log value */
            cJSON_AddStringToObject(root, "log_value", mcu_dev_name_error_zh[dev_id]);
            /* application name*/
            cJSON_AddStringToObject(root, "application_name", "clean");

        } else {
            /* log type */
            cJSON_AddStringToObject(root, "log_type", "info");
            /* log value */
            cJSON_AddStringToObject(root, "log_value", mcu_dev_name_zh[dev_id]);
            /* application name*/
            cJSON_AddStringToObject(root, "application_name", "clean");
        }

        string_msg = cJSON_PrintUnformatted(root);
        if (string_msg != NULL) {
            clean_dev_status_info.data.data     = string_msg;
            clean_dev_status_info.data.size     = strlen(string_msg);
            clean_dev_status_info.data.capacity = 0;
            message_publish(&g_clean_dev_status);
        }
        if (root != NULL) {
            cJSON_Delete(root);
        }
        if (string_msg != NULL) {
            log_d("----->%s", string_msg);
            cJSON_free(string_msg);
        }
    } else {
        log_d("creat json obj failed on line %d\r\n", __LINE__);
    }
    return;
}

void clean_work_mode_pub(uint8_t data) {
    /* 手动自动发布 */
    work_mode.data = data;
    message_publish(&g_work_mode);
}

void strainer_info_pub(uint8_t data) {
    if (data > 0) {
        strainer_info.data = 0;
    } else {
        strainer_info.data = 1;
    }
    message_publish(&g_strainer);
}

void cmd_charge_start_pub(uint8_t data) {
    /* 自动充电主题发布 */
    cmd_charge_start_info.structure_needs_at_least_one_member = data;
    message_publish(&g_cmd_charge_start);
}

void cmd_charge_cancel_pub(uint8_t data) {
    /* 取消充电主题发布 */
    cmd_charge_cancel_info.structure_needs_at_least_one_member = data;
    message_publish(&g_cmd_charge_cancel);
}

motor_ack side_brush_motor_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_SIDE_BRUSH_MOTOR, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_SIDE_BRUSH_MOTOR] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack roll_brush_motor_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_ROLL_BURSH_MOTOR, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_ROLL_BURSH_MOTOR] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not*/
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack fan_motor_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_FAN_MOTOR, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_FAN_MOTOR] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack cloth_water_pump_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_CLOTH_WATER_PUMP, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_CLOTH_WATER_PUMP] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack clean_putter_motor_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_CLEAN_PUTTER_MOTOR, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_CLEAN_PUTTER_MOTOR] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack dust_putter_motor_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_DUST_PUTTER_MOTOR, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_DUST_PUTTER_MOTOR] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

motor_ack clear_valve_set(float data) {
    int send, fb;
    int time = 0;
    /* cyl:run */
    motor_mcu_ctrl(ID_CLEAR_WATER_VALVE, data);
    /* cyl:waiting for ack */
    while (1) {
        send = (int) (data * 100.0 + 0.5);
        fb   = (int) (clean_ctrl_comp_info.dev_fb_value[ID_CLEAR_WATER_VALVE] * 100.0 + 0.5);
#ifdef send_fb_cmp_log
        log_d("send:%d,fb:%d", send, fb);
#endif
        osDelay(WAIT_ACK_TIME);
        if (send == fb || time++ > WAIT_ACK_TIME_MAX)
            break;
    }
    /* cyl:is motor ack?if not */
    if (send != fb) {
        /* cyl:drop count */
        clean_ctrl_comp_info.drop_num++;
        /* cyl:return loss flag */
        return ACK_LOSS;
    }
    /* cyl:return normal ack flag */
    return ACK_OK;
}

/* 立即停止 */
static uint8_t clean_ctrl_immediately_stop(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
                /* 关尘推推杆*/
                ack &= dust_putter_motor_set(0);
                /* 关滚刷 */
                ack &= roll_brush_motor_set(0);
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_2;
                }
                break;
            case DEV_OPS_STEP_2:
                /* 关过滤水泵 */
                fliter_pmup_set_data(0);
                dev_switch_state = DEV_OPS_STEP_3;
                break;
            case DEV_OPS_STEP_3:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ZERO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_4;
                }
                break;
            case DEV_OPS_STEP_4:
                /* 关布水水泵 */
                ack &= cloth_water_pump_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
                break;
            case DEV_OPS_STEP_5:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_6:
                /* 关清水阀，过滤系统水阀*/
                waste_water_valve_set_data(0);
                ack &= clear_valve_set(0);
                /* 关边刷 */
                ack &= side_brush_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_7;
                }
                break;
            case DEV_OPS_STEP_7:
                /* 关风机 */
                ack &= fan_motor_set(0);
                /* 关洗地推杆 */
                ack &= clean_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_8;
                }
                break;
            case DEV_OPS_STEP_8:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_9;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}

static uint8_t run_clean_ctrl_immediately_stop(void *object, uint8_t state) {
    uint8_t temp_state = state;

    if (temp_state) {
        return clean_ctrl_immediately_stop((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

/* 正常停止 */
static uint8_t clean_ctrl_normal_stop(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
                /* 关尘推推杆*/
                ack &= dust_putter_motor_set(0);
#ifdef WASH_MODE_OPEN_DUST            
                /* 洗地模式开尘推推杆 */
                if(clean_ctrl_comp_info.last_last_state == STATE_WASH_START)
                    ack &= dust_putter_motor_set(1);
#endif
                /* 关滚刷 */
                ack &= roll_brush_motor_set(0);
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 3) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_2;
                }
                break;
            case DEV_OPS_STEP_2:
                /* 关过滤水泵 */
                fliter_pmup_set_data(0);
                dev_switch_state = DEV_OPS_STEP_3;
                break;
            case DEV_OPS_STEP_3:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ZERO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_4;
                }
                break;
            case DEV_OPS_STEP_4:
                /* 关布水水泵 */
                ack &= cloth_water_pump_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
                break;
            case DEV_OPS_STEP_5:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_6:
                /* 关清水阀，过滤系统水阀*/
                waste_water_valve_set_data(0);
                ack &= clear_valve_set(0);
                /* 关边刷 */
                ack &= side_brush_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_7;
                }
                break;
            case DEV_OPS_STEP_7:
                if (start_or_stop_delay_count++ >= STOP_DELAY_TIME_TWO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_8;
                }
                break;
            case DEV_OPS_STEP_8:
                /* 关风机 */
                ack &= fan_motor_set(0);
                /* 关洗地推杆 */
                ack &= clean_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
#ifdef WASH_MODE_OPEN_DUST
                    if(clean_ctrl_comp_info.last_last_state == STATE_WASH_START)
                    	dev_switch_state = DEV_OPS_STEP_9;
                    else
                        dev_switch_state = DEV_OPS_STEP_11;
#else
                    dev_switch_state = DEV_OPS_STEP_11;
#endif
                }
                break;
            case DEV_OPS_STEP_9:
                if (start_or_stop_delay_count++ >= STOP_DELAY_TIME_THREE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_10;
                }
                break;
            case DEV_OPS_STEP_10:
                /* 关尘推推杆 */
                ack &= dust_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_11;
                }
                break;
            case DEV_OPS_STEP_11:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_12;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}

static uint8_t run_clean_ctrl_normal_stop(void *object, uint8_t state) {
    uint8_t temp_state = state;
    if (temp_state) {
        return clean_ctrl_normal_stop((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

static uint8_t clean_ctrl_dust_start(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
                /* 关尘推推杆*/
                ack &= dust_putter_motor_set(0);
                /* 关滚刷 */
                ack &= roll_brush_motor_set(0);
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_2;
                }
                break;
            case DEV_OPS_STEP_2:
                /* 关过滤水泵 */
                fliter_pmup_set_data(0);
                dev_switch_state = DEV_OPS_STEP_3;
                break;
            case DEV_OPS_STEP_3:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ZERO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_4;
                }
                break;
            case DEV_OPS_STEP_4:
                /* 关布水水泵 */
                ack &= cloth_water_pump_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
                break;
            case DEV_OPS_STEP_5:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_6:
                /* 关清水阀，过滤系统水阀*/
                waste_water_valve_set_data(0);
                ack &= clear_valve_set(0);
                /* 关边刷 */
                ack &= side_brush_motor_set(0);
                /* 关风机 */
                ack &= fan_motor_set(0);
                /* 关洗地推杆 */
                ack &= clean_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 4) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_7;
                }
                break;
            case DEV_OPS_STEP_7:
                /* 开启尘推 */
                ack &= dust_putter_motor_set(1);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
#ifdef DUST_MODE_OPEN_MOTOR
                    dev_switch_state = DEV_OPS_STEP_8;
#else 
                    dev_switch_state = DEV_OPS_STEP_11;
#endif
                }
                break;
            case DEV_OPS_STEP_8:
                /* 开滚刷 */
                ack &= roll_brush_motor_set(clean_ctrl_comp_info.dev_set_value[ID_ROLL_BURSH_MOTOR]);
                /* 开洗地推杆 */
                ack &= clean_putter_motor_set(1);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_9;
                }
                break;
            case DEV_OPS_STEP_9:
                /*延迟*/
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_FOUR) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_10;
                }
                break;
            case DEV_OPS_STEP_10:
                /* 开边刷 */
                ack &= side_brush_motor_set(clean_ctrl_comp_info.dev_set_value[ID_SIDE_BRUSH_MOTOR]);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_11;
                }
                break;
            case DEV_OPS_STEP_11:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_12;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}

static uint8_t run_clean_ctrl_dust_start(void *object, uint8_t state) {
    uint8_t temp_state = state;
    if (temp_state) {
        return clean_ctrl_dust_start((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

static uint8_t clean_ctrl_wash_start(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    uint8_t waste_value = 0;
    uint8_t clean_value = 0;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
             	/* 关过滤水泵 */
                fliter_pmup_set_data(0);
                dev_switch_state = DEV_OPS_STEP_2;
                break;
            case DEV_OPS_STEP_2:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ZERO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_3;
                }
                break;
            case DEV_OPS_STEP_3:
                /* 关布水水泵 */
                ack &= cloth_water_pump_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_4;
                }
                break;
            case DEV_OPS_STEP_4:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
                break;
            case DEV_OPS_STEP_5:
                /* 关清水阀，过滤系统水阀*/
                waste_water_valve_set_data(0);
                ack &= clear_valve_set(0);
                /*关尘推推杆 */
                ack &= dust_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_6:
                /* 开滚刷 */
                ack &= roll_brush_motor_set(clean_ctrl_comp_info.dev_set_value[ID_ROLL_BURSH_MOTOR]);
                /* 开风机 */
                ack &= fan_motor_set(clean_ctrl_comp_info.dev_set_value[ID_FAN_MOTOR]);

                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_13;
                }
                break;
            case DEV_OPS_STEP_7:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_8;
                }
            case DEV_OPS_STEP_8:
                if (clean_ctrl_comp_info.enable_filter_system) {
                    /*使用过滤水循环，开启污水阀 */
                    waste_value = 1;
                    clean_value = 0;
                } else {
                    waste_value = 0;
                    clean_value = 1;
                }
                waste_water_valve_set_data(waste_value);
                /* 开清水阀 */
                ack &= clear_valve_set(clean_value);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_9;
                }
                break;
            case DEV_OPS_STEP_9:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_TWO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_10;
                }
                break;
            case DEV_OPS_STEP_10:
                /* 布水水泵 */
                ack &= cloth_water_pump_set(clean_ctrl_comp_info.dev_set_value[ID_CLOTH_WATER_PUMP]);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    if(clean_ctrl_comp_info.enable_filter_system)
                        dev_switch_state = DEV_OPS_STEP_11;
                    else
                        dev_switch_state = DEV_OPS_STEP_17;
                }
                break;
            case DEV_OPS_STEP_11:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_THREE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_12;
                }
            case DEV_OPS_STEP_12:
                /*开启了过滤系统，打开过滤系统水阀 */
                fliter_pmup_set_data(1);
                dev_switch_state = DEV_OPS_STEP_17;
                break;
            case DEV_OPS_STEP_13:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_FOUR) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_14;
                }
                break;
            case DEV_OPS_STEP_14:
                /* 洗地推杆 */
                ack &= clean_putter_motor_set(1);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_15;
                }
                break;
            case DEV_OPS_STEP_15:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_FIVE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_16;
                }
                break;
            case DEV_OPS_STEP_16:
                /* 边刷 */
                ack &= side_brush_motor_set(clean_ctrl_comp_info.dev_set_value[ID_SIDE_BRUSH_MOTOR]);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_7;
                }
                break;
            case DEV_OPS_STEP_17:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_18;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, clean_ctrl_wash_start,
                 clean_ctrl_wash_start, clean wash);

static uint8_t run_clean_ctrl_wash_start(void *object, uint8_t state) {
    uint8_t temp_state = state;
    if (temp_state) {
        return clean_ctrl_wash_start((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

static uint8_t clean_ctrl_water_open(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    uint8_t waste_value = 0;
    uint8_t clean_value = 0;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
                if (clean_ctrl_comp_info.enable_filter_system) {
                    /*使用过滤水循环，开启污水阀 */
                    waste_value = 1;
                    clean_value = 0;
                } else {
                    waste_value = 0;
                    clean_value = 1;
                }
                waste_water_valve_set_data(waste_value);
                /* 开清水阀 */
                ack &= clear_valve_set(clean_value);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_2;
                }
                break;
            case DEV_OPS_STEP_2:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_TWO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_3;
                }
                break;
            case DEV_OPS_STEP_3:
                /* 布水水泵 */
                ack &= cloth_water_pump_set(clean_ctrl_comp_info.dev_set_value[ID_CLOTH_WATER_PUMP]);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    if(clean_ctrl_comp_info.enable_filter_system)
                        dev_switch_state = DEV_OPS_STEP_4;
                    else
                        dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_4:
                if (start_or_stop_delay_count++ >= START_DELAY_TIME_THREE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
            case DEV_OPS_STEP_5:
                /*开启了过滤系统，打开过滤系统水阀 */
                fliter_pmup_set_data(1);
                dev_switch_state = DEV_OPS_STEP_6;
                break;
            case DEV_OPS_STEP_6:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_7;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}

static uint8_t run_clean_ctrl_water_open(void *object, uint8_t state) {
    uint8_t temp_state = state;
    if (temp_state) {
        return clean_ctrl_water_open((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

/* 正常停止 */
static uint8_t clean_ctrl_water_close(clean_ctrl_component_t *object) {
    uint8_t   ret                       = OPS_WAIT_COMPLETION;
    static uint32_t  start_or_stop_delay_count = 0;
    uint8_t   dev_switch_state          = 0;
    motor_ack ack                       = ACK_OK;  // cyl:ack flag
    if (object != NULL) {
        dev_switch_state          = object->dev_switch_state;
        switch (dev_switch_state) {
            case DEV_OPS_STEP_0:
                start_or_stop_delay_count = 0;
                dev_switch_state = DEV_OPS_STEP_1;
                break;
            case DEV_OPS_STEP_1:
             	/* 关过滤水泵 */
                fliter_pmup_set_data(0);
                dev_switch_state = DEV_OPS_STEP_2;
                break;
            case DEV_OPS_STEP_2:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ZERO) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_3;
                }
                break;
            case DEV_OPS_STEP_3:
                /* 关布水水泵 */
                ack &= cloth_water_pump_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_4;
                }
                break;
            case DEV_OPS_STEP_4:
                if(start_or_stop_delay_count++ >= STOP_DELAY_TIME_ONE) {
                    start_or_stop_delay_count = 0;
                    dev_switch_state = DEV_OPS_STEP_5;
                }
                break;
            case DEV_OPS_STEP_5:
                /* 关清水阀，过滤系统水阀*/
                waste_water_valve_set_data(0);
                ack &= clear_valve_set(0);
                /*关尘推推杆 */
                ack &= dust_putter_motor_set(0);
                /* cyl:again or next state */
                if (ack == ACK_OK || clean_ctrl_comp_info.drop_num > drop_tolerance_max * 2) {
#ifdef drop_log
                    log_d("ACK:%d,drop:%d", ack, clean_ctrl_comp_info.drop_num);
#endif
                    clean_ctrl_comp_info.drop_num = 0;
                    if (clean_ctrl_comp_info.drop_num > drop_tolerance_max)
                        clean_ctrl_comp_info.reconnect_for_drop = true;
                    dev_switch_state = DEV_OPS_STEP_6;
                }
                break;
            case DEV_OPS_STEP_6:
                ret = OPS_COMPLETION;
                dev_switch_state = DEV_OPS_STEP_7;
            default:
                start_or_stop_delay_count = 0;
                break;
        }
    }
    object->dev_switch_state          = dev_switch_state;
    return ret;
}

static uint8_t run_clean_ctrl_water_close(void *object, uint8_t state) {
    uint8_t temp_state = state;
    if (temp_state) {
        return clean_ctrl_water_close((clean_ctrl_component_t *) object);
    }
    return OPS_COMPLETION;
}

void get_motor_mcu_feedback_info(const void *msgin) {
    cJSON *json_msg   = NULL;
    cJSON *json_temp  = NULL;
    cJSON *json_value = NULL;
    if (msgin == NULL)
        return;

    const std_msgs__msg__String *string_msg = (const std_msgs__msg__String *) msgin;
    if (string_msg->data.data != NULL) {
        /* cyl:print origin json message */
        // log_d("motor feedback json:%s", string_msg->data.data);

        json_msg = cJSON_Parse(string_msg->data.data);
        if (json_msg == NULL)
            return;

        json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, "name");
        if (json_temp != NULL) {
            json_value = cJSON_GetObjectItemCaseSensitive(json_msg, "value");
            if (json_value != NULL) {
                for (uint8_t i = 0; i < ID_FILTER_PUMP; i++) {
                    if (!strncasecmp(json_temp->valuestring, mcu_dev_name[i], strlen(mcu_dev_name[i]))) {
                        clean_ctrl_comp_info.dev_fb_value[i] = json_value->valuedouble;
                        //log_d("<-----%s:%f", mcu_dev_name[i], clean_ctrl_comp_info.dev_fb_value[i]);
                        cJSON_Delete(json_msg);
                        clean_ctrl_comp_info.last_fb_time = clock_cpu_gettime();
                        return;
                    }
                }
            }
        }
        for (uint8_t i = 0; i < ID_FILTER_PUMP; i++) {
            json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, mcu_fb_error_name[i]);
            if (json_temp != NULL) {
                clean_ctrl_comp_info.dev_err_value[i] = json_temp->valueint;
                clean_ctrl_comp_info.last_fb_time     = clock_cpu_gettime();
                if (clean_ctrl_comp_info.dev_err_value[i])
                    log_d("%s:%d", mcu_fb_error_name[i], clean_ctrl_comp_info.dev_err_value[i]);
            }
        }
        cJSON_Delete(json_msg);
    }
    return;
}

void get_robot_vel_info(const void *msgin) {
    if (msgin == NULL)
        return;

    const geometry_msgs__msg__Twist *vel_msg = (const geometry_msgs__msg__Twist *) msgin;
    clean_ctrl_comp_info.linear              = vel_msg->linear.x;
    clean_ctrl_comp_info.angular             = vel_msg->angular.z;
    // log_d("wheel speed:%f",clean_ctrl_comp_info.linear);
}

void get_water_level_info_update(const void *msgin) {
    if (msgin == NULL)
        return;

    water_level_info_pub(clean_ctrl_comp_info.water_level_info, 2);
}

void get_work_mode_update(const void *msgin) {
    if (msgin == NULL)
        return;

    clean_work_mode_pub(clean_ctrl_comp_info.button_value[AUTO_MANUAL_BUTTON]);
}

void get_emerg_info(const void *msgin) {
    if (msgin == NULL)
        return;
    const std_msgs__msg__Bool *temp_msg = (const std_msgs__msg__Bool *) msgin;
    clean_ctrl_comp_info.emerg_flag     = temp_msg->data;
    log_d("emerg flag :%d", clean_ctrl_comp_info.emerg_flag);
}

void get_touch_edge_0_info(const void *msgin) {
    if (msgin == NULL)
        return;
    const std_msgs__msg__Bool *temp_msg    = (const std_msgs__msg__Bool *) msgin;
    clean_ctrl_comp_info.touch_edge_0_flag = temp_msg->data;
    log_d("touch edge 0 flag :%d", clean_ctrl_comp_info.touch_edge_0_flag);
}

void get_nav_work_info_status(const void *msgin) {
    // if (msgin == NULL)
    //     return;

    // const chassis_interfaces__msg__NaviWorkStatus *pnavi_state_msg = (const chassis_interfaces__msg__NaviWorkStatus *) msgin;
    // if (!strncasecmp(pnavi_state_msg->work_status.data, "idle", strlen("idle"))) {
    //     clean_ctrl_comp_info.nav_work_status = NAV_STATUS_IDLE;
    // }
    // else if (!strncasecmp(pnavi_state_msg->work_status.data, "task_navigating", strlen("task_navigating"))) {
    //     clean_ctrl_comp_info.nav_work_status = NAV_STATUS_TASK;
    // } else if (!strncasecmp(pnavi_state_msg->work_status.data, "charge_navigating", strlen("charge_navigating"))) {
    //     clean_ctrl_comp_info.nav_work_status = NAV_STATUS_CHARGE;
    // }
    // log_d("<-----nav work status: %d", clean_ctrl_comp_info.nav_work_status);
    // return;
}
void get_clean_component_ctrl_info(const void *msgin) {
    cJSON *json_msg  = NULL;
    cJSON *json_comp = NULL;
    cJSON *json_temp = NULL;
    if (msgin == NULL)
        return;

    const std_msgs__msg__String *string_msg = (const std_msgs__msg__String *) msgin;
    if (string_msg->data.data != NULL) {
        /* cyl:print origin json message */
        log_d("clean component ctrl json:%s", string_msg->data.data);

        json_msg = cJSON_Parse(string_msg->data.data);
        if (json_msg == NULL)
            return;
        json_temp = cJSON_GetObjectItemCaseSensitive(json_msg, "model");
        if (json_temp != NULL) {
            clean_ctrl_comp_info.model = cJSON_GetStringValue(json_temp);
            if (!strncasecmp(clean_ctrl_comp_info.model, "mopping", sizeof("mopping"))) {
                clean_ctrl_comp_info.mode = ID_MODE_MOPPING;
            } else if (!strncasecmp(clean_ctrl_comp_info.model, "dedusting", sizeof("dedusting"))) {
                clean_ctrl_comp_info.mode = ID_MODE_DEDUSTING;
            } else {
                clean_ctrl_comp_info.mode = ID_MODE_NULL;
            }
            log_d("current mode :%s---%d", clean_ctrl_comp_info.model, clean_ctrl_comp_info.mode);
        }
        json_comp = cJSON_GetObjectItemCaseSensitive(json_msg, "component");
        if (json_comp != NULL) {
            for (uint8_t i = 0; i < ID_DEV_MAX; i++) {
                json_temp = cJSON_GetObjectItemCaseSensitive(json_comp, mcu_dev_name[i]);
                if (json_temp != NULL) {
                    clean_ctrl_comp_info.dev_set_value[i] = json_temp->valuedouble;
                    //log_d("%d, %s:%f", clean_ctrl_comp_info.mode, mcu_dev_name[i], clean_ctrl_comp_info.dev_set_value[i]);
                } else {
                    //log_d("%s get error", mcu_dev_name[i]);
                }
            }
        }
        clean_ctrl_comp_info.last_recv_time = clock_cpu_gettime();
        cJSON_Delete(json_msg);
    }
}

void clean_ctrl_comp_info_default_init(clean_ctrl_component_t *object) {
    if (object == NULL)
        return;
    object->mode                                 = ID_MODE_NULL;
    object->dev_set_value[ID_ROLL_BURSH_MOTOR]   = ROLL_BRUSH_MOTOR_DEFAULT_VALUE;
    object->dev_set_value[ID_FAN_MOTOR]          = FAN_DEFAULT_VALUE;
    object->dev_set_value[ID_SIDE_BRUSH_MOTOR]   = SIDE_BRUSH_MOTOR_DEFAULT_VALUE;
    object->dev_set_value[ID_CLOTH_WATER_PUMP]   = CLOTH_WATER_PUMP_DEFAULT_VALUE;
    object->dev_set_value[ID_FILTER_PUMP]        = 0;
    object->dev_set_value[ID_CLEAR_WATER_VALVE]  = 1;
    object->dev_set_value[ID_WASTE_WATER_VALVE]  = 0;
    object->dev_set_value[ID_CLEAN_PUTTER_MOTOR] = 1;
    object->dev_set_value[ID_DUST_PUTTER_MOTOR]  = 0;
}

void clean_ctrl_comp_info_default_func(clean_ctrl_component_t *object) {
    if (object == NULL)
        return;

    object->run_fun[STATE_INIT]             = NULL;
    object->run_fun[STATE_IMMEDIATELY_STOP] = run_clean_ctrl_immediately_stop;
    object->run_fun[STATE_NORMAL_STOP]      = run_clean_ctrl_normal_stop;
    object->run_fun[STATE_WASH_START]       = run_clean_ctrl_wash_start;
    object->run_fun[STATE_DUST_START]       = run_clean_ctrl_dust_start;
    object->run_fun[STATE_WATER_OPEN]       = run_clean_ctrl_water_open;
    object->run_fun[STATE_WATER_CLOSE]      = run_clean_ctrl_water_close;
}
#ifdef CLEAN_KEYBOARD_STATE_FEEKBACK
void get_clean_keyboard_state_update_info(const void *msgin) {
    if (msgin == NULL)
        return;
    publish_clean_keyboard_state();
    log_i("keyboard state update");
}
#endif
void clean_manage_pal_init(void) {
    /* 订阅清洁控制信息 */
    subscrption_init(&g_clean_component_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/clean_component/control",
                     &clean_component_ctrl_info, BEST, get_clean_component_ctrl_info);
    MallocString(&clean_component_ctrl_info.data, REQ, 384);

    /* 订阅机器速度信息 */
    subscrption_init(&g_robot_vel, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/odom_vel", &robot_vel_info, BEST,
                     get_robot_vel_info);

    /* 订阅急停信息 */
    subscrption_init(&g_emerg_info, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/emerg", &emerg_info, BEST, get_emerg_info);

    /* 订阅触边信息 */
    subscrption_init(&g_touch_edge_0, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/touch_edge_0", &touch_edge_0_info, BEST,
                     get_touch_edge_0_info);

    /* 订阅导航信息 */
    // subscrption_init(&g_nav_work_status, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, NaviWorkStatus),
    //                  "/navi_manager/state_publisher/work_status", &nav_work_status_info, BEST, get_nav_work_info_status);
    // MallocString(&nav_work_status_info.work_status, REQ, 32);
    // memcpy(nav_work_status_info.work_status.data, "idle", sizeof("idle"));
    // MallocString(&nav_work_status_info.state, REQ, 32);
    // MallocString(&nav_work_status_info.map_id, REQ, 128);

    /* 向电控MCU发布主题 */
    publisher_init(&g_motor_mcu_ctrl, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/task_motor/set", &motor_mcu_ctrl_info, DEFAULT,
                   OVERWRITE, sizeof(std_msgs__msg__String));

    /* 发布水位信息 */
    publisher_init(&g_water_level_info, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/water_position", &water_level_info, DEFAULT,
                   OVERWRITE, sizeof(std_msgs__msg__String));

    /* 订阅水位更新信息 */
    subscrption_init(&g_water_level_info_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/water_position/update",
                     &water_level_info_update, BEST, get_water_level_info_update);

    /* 工作模式发布主题 */
    publisher_init(&g_work_mode, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/clean_work_mode", &work_mode, DEFAULT, OVERWRITE,
                   sizeof(std_msgs__msg__UInt8));

    /* 发布开始充电命令 */
    publisher_init(&g_cmd_charge_start, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/navi_manager/start_charge", &cmd_charge_start_info,
                   BEST, OVERWRITE, sizeof(std_msgs__msg__Empty));

    /* 发布取消充电命令 */
    publisher_init(&g_cmd_charge_cancel, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty), "/navi_manager/cancel_charge",
                   &cmd_charge_cancel_info, BEST, OVERWRITE, sizeof(std_msgs__msg__Empty));

#ifdef CLEAN_KEYBOARD_STATE_FEEKBACK
    g_clean_keyboard_state.data.data     = keyboard_state;
    g_clean_keyboard_state.data.capacity = sizeof(keyboard_state);
    g_clean_keyboard_state.data.size     = strlen(keyboard_state);
    publisher_init(&g_pub_clean_keyboard_state, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/clean_keyboard_state",
                   &g_clean_keyboard_state, DEFAULT, OVERWRITE, sizeof(std_msgs__msg__String));
#endif

    /* 订阅模式跟新主题 */
    subscrption_init(&g_work_mode_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/clean_work_mode/update", &work_mode_update, BEST,
                     get_work_mode_update);

    /* 过滤网信息发布 */
    publisher_init(&g_strainer, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/strainer", &strainer_info, BEST, OVERWRITE,
                   sizeof(std_msgs__msg__Bool));

    /* 订阅清洁控制信息 */
    subscrption_init(&g_motor_mcu_feedback, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/task_motor/get", &motor_mcu_feedback_info,
                     BEST, get_motor_mcu_feedback_info);
    MallocString(&motor_mcu_feedback_info.data, REQ, 256);

    /* 发布清洁设备状态 */
    publisher_init(&g_clean_dev_status, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/clean_component/status", &clean_dev_status_info,
                   DEFAULT, OVERWRITE, sizeof(std_msgs__msg__String));
#ifdef CLEAN_KEYBOARD_STATE_FEEKBACK
    /* sub the clean_keyboard_state update */
    subscrption_init(&g_sub_clean_keyboard_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/clean_keyboard_state/update",
                     &g_clean_keyboard_update, BEST, get_clean_keyboard_state_update_info);
#endif

    cjson_hooks.malloc_fn = pvPortMalloc;
    cjson_hooks.free_fn   = vPortFree;

    cJSON_InitHooks(&cjson_hooks);

    return;
}
void enable_mcu_hub_motor(bool flag) {
    work_mode.data = flag ? 0 : 1;
    auto_manual_button_status_set(work_mode.data);
    message_publish(&g_work_mode);
}
#ifdef CLEAN_KEYBOARD_STATE_FEEKBACK
void publish_clean_keyboard_state(void) {
    // publish the keyboard state
    cJSON *root        = NULL;
    char * pstring_msg = NULL;
    memset(g_clean_keyboard_state.data.data, 0, sizeof(keyboard_state));
    root = cJSON_CreateObject();
    if (root == NULL) {
        log_d("creat json obj failed on line %d\r\n", __LINE__);
        return;
    }

    cJSON_AddBoolToObject(root, "return_charge", clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON] ? true : false);
    cJSON_AddNumberToObject(root, "work_mode", clean_ctrl_comp_info.button_value[AUTO_MANUAL_BUTTON] ? 1 : 0);
    cJSON_AddBoolToObject(root, "mopping", clean_ctrl_comp_info.button_value[WASH_FLOOR_BUTTON] ? true : false);
    cJSON_AddBoolToObject(root, "dedusting", clean_ctrl_comp_info.button_value[DUST_PUSH_BUTTON] ? true : false);
    pstring_msg = cJSON_PrintUnformatted(root);
    if (pstring_msg == NULL) {
        log_d("line %d  error create msg of keyboard", __LINE__);
        cJSON_Delete(root);
        return;
    }
    g_clean_keyboard_state.data.size = strlen(pstring_msg) > sizeof(keyboard_state) ? sizeof(keyboard_state) : strlen(pstring_msg);
    memcpy(g_clean_keyboard_state.data.data, pstring_msg, g_clean_keyboard_state.data.size);
    message_publish(&g_pub_clean_keyboard_state);
    log_i("----->%s", pstring_msg);
    cJSON_free(pstring_msg);
    cJSON_Delete(root);
}

void clean_keyboard_state_reflesh(void) {
    static uint8_t cur_return_charge_key  = 0;
    static uint8_t last_return_charge_key = 0;
    static uint8_t cur_auto_manual_key    = 0;
    static uint8_t last_auto_manual_key   = 0;
    static uint8_t cur_wash_floor_key     = 0;
    static uint8_t last_wash_floor_key    = 0;
    static uint8_t cur_dust_push_key      = 0;
    static uint8_t last_dust_push_key     = 0;
    static uint8_t times                  = 0;

    for (uint8_t i = 0; i < BUTTON_DEFAULT_MAX; i++) {
        if (clean_ctrl_comp_info.button_value[i] != function_button_status_get(i)) {
            clean_ctrl_comp_info.button_value[i] = function_button_status_get(i);
            if (i == AUTO_MANUAL_BUTTON) {
                clean_work_mode_pub(clean_ctrl_comp_info.button_value[AUTO_MANUAL_BUTTON]);
            } else if (i == STRAINER_BUTTON) {
                strainer_info_pub(clean_ctrl_comp_info.button_value[STRAINER_BUTTON]);
            } else if (i == RETURN_HOME_BUTTON) {
                /* 一键返航按键被按下 */
                if (clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]) {
                    clean_ctrl_comp_info.nav_work_status = NAV_STATUS_CHARGE;
                    cmd_charge_start_pub(clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]);
                } else {
                    clean_ctrl_comp_info.nav_work_status = NAV_STATUS_IDLE;
                    cmd_charge_cancel_pub(clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]);
                }
            }
            log_i("key changed :%d, %d,", i, clean_ctrl_comp_info.button_value[i]);
        }
    }

    if (times++ % 100 == 0) {
        last_return_charge_key = cur_return_charge_key;
        cur_return_charge_key  = clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON];

        last_auto_manual_key = cur_auto_manual_key;
        cur_auto_manual_key  = clean_ctrl_comp_info.button_value[AUTO_MANUAL_BUTTON];

        last_wash_floor_key = cur_wash_floor_key;
        cur_wash_floor_key  = clean_ctrl_comp_info.button_value[WASH_FLOOR_BUTTON];

        last_dust_push_key = cur_dust_push_key;
        cur_dust_push_key  = clean_ctrl_comp_info.button_value[DUST_PUSH_BUTTON];
        if (last_return_charge_key != cur_return_charge_key || last_auto_manual_key != cur_auto_manual_key ||
            last_wash_floor_key != cur_wash_floor_key || last_dust_push_key != cur_dust_push_key) {
            publish_clean_keyboard_state();
        }
    }
}
#endif

void task_clean_manage_run(void *argument) {
    uint32_t printf_div = 0;
    /* 清洁组件百分比默认参数设定 */
    clean_ctrl_comp_info_default_init(&clean_ctrl_comp_info);
    /* 清洁组件执行函数初始化 */
    clean_ctrl_comp_info_default_func(&clean_ctrl_comp_info);
    /* 清洁订阅发布服务初始化 */
    // clean_manage_pal_init();
    clean_ctrl_comp_info.state = STATE_INIT;
    osDelay(5000);
    while (1) {
        osDelay(THREAD_DELAY_TIME);
        /* 更新按键值 */
#ifndef CLEAN_KEYBOARD_STATE_FEEKBACK
        for (uint8_t i = 0; i < BUTTON_DEFAULT_MAX; i++) {
            if (clean_ctrl_comp_info.button_value[i] != function_button_status_get(i)) {
                clean_ctrl_comp_info.button_value[i] = function_button_status_get(i);
                if (i == AUTO_MANUAL_BUTTON) {
                    clean_work_mode_pub(clean_ctrl_comp_info.button_value[AUTO_MANUAL_BUTTON]);
                } else if (i == STRAINER_BUTTON) {
                    strainer_info_pub(clean_ctrl_comp_info.button_value[STRAINER_BUTTON]);
                } else if (i == RETURN_HOME_BUTTON) {
                    /* 一键返航按键被按下 */
                    if (clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]) {
                        clean_ctrl_comp_info.nav_work_status = NAV_STATUS_CHARGE;
                        cmd_charge_start_pub(clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]);
                    } else {
                        clean_ctrl_comp_info.nav_work_status = NAV_STATUS_IDLE;
                        cmd_charge_cancel_pub(clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]);
                    }
                }
                log_i("key changed :%d, %d,", i, clean_ctrl_comp_info.button_value[i]);
            }
        }
#else
        clean_keyboard_state_reflesh();
#endif
        for (uint8_t i = 0; i < 2; i++) {
            if (clean_ctrl_comp_info.water_level_info[i] != water_level_status_get(i)) {
                for (uint8_t j = 0; j < 2; j++) {
                    clean_ctrl_comp_info.water_level_info[i] = water_level_status_get(i);
                }
                water_level_info_pub(clean_ctrl_comp_info.water_level_info, 2);
                break;
            }
        }
        /* 更新回冲状态 */
        if (clean_ctrl_comp_info.nav_work_status == NAV_STATUS_CHARGE) {
            if (!clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]) {
                function_button_status_set(RETURN_HOME_BUTTON, 1);
                clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON] = function_button_status_get(RETURN_HOME_BUTTON);
            }
        } else {
            if (clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON]) {
                function_button_status_set(RETURN_HOME_BUTTON, 0);
                clean_ctrl_comp_info.button_value[RETURN_HOME_BUTTON] = function_button_status_get(RETURN_HOME_BUTTON);
            }
        }

        /* 更新主mcu设备错误码 */
        clean_ctrl_comp_info.dev_err_value[ID_FILTER_PUMP]        = clean_device_error_status_get(ID_FILTER_PUMP_DET);
        clean_ctrl_comp_info.dev_err_value[ID_SEWAGE_WATER_VALVE] = clean_device_error_status_get(ID_SEWAGE_WATER_VALVE_DET);
        clean_ctrl_comp_info.dev_err_value[ID_WASTE_WATER_VALVE]  = clean_device_error_status_get(ID_WASTE_WATER_VALVE_DET);
        clean_ctrl_comp_info.dev_set_value[ID_FILTER_PUMP]        = clean_device_status_get(ID_FILTER_PUMP_DET);
        clean_ctrl_comp_info.dev_set_value[ID_SEWAGE_WATER_VALVE] = clean_device_status_get(ID_SEWAGE_WATER_VALVE_DET);
        clean_ctrl_comp_info.dev_set_value[ID_WASTE_WATER_VALVE]  = clean_device_status_get(ID_WASTE_WATER_VALVE_DET);
        /* 对比MCU错误码,发布错误码消息 */
        // uint8_t index = 0;
        // uint8_t dev_id_table[ID_DEV_MAX] = {0};
        // int err_code[ID_DEV_MAX] = {0};
        for (uint8_t i = 0; i < ID_DEV_MAX; i++) {
            if (clean_ctrl_comp_info.dev_err_value[i] != clean_ctrl_comp_info.last_dev_err_value[i]) {
                // dev_id_table[index] = i;
                // err_code[index] = clean_ctrl_comp_info.dev_err_value[i];
                // index++;
                /* 发布组件变化消息 */
                clean_dev_status_info_pub(i, clean_ctrl_comp_info.dev_err_value[i], 1);
            }
        }
        // if(index > 0)
        //{
        // clean_dev_status_info_pub(dev_id_table, err_code, index);
        //}

        /* 查找急停安全触边等紧急状态 */
        if (clean_ctrl_comp_info.emerg_flag) {
            /* 急停按钮被按下 */
            SYS_SET_BIT(clean_ctrl_comp_info.secure_error_flag, EMERG_ERROR_FLAG);
        } else {
            SYS_CLR_BIT(clean_ctrl_comp_info.secure_error_flag, EMERG_ERROR_FLAG);
        }
        if (clean_ctrl_comp_info.touch_edge_0_flag) {
            /* 安全触边触发 */
            SYS_SET_BIT(clean_ctrl_comp_info.secure_error_flag, TOUCH_EDGE_0_FLAG);
        } else {
            SYS_CLR_BIT(clean_ctrl_comp_info.secure_error_flag, TOUCH_EDGE_0_FLAG);
        }

        /* 急停、安全触边触发：1.急停按下时候，
         * 全部设备依次按照顺序快速关闭,恢复，则继续运行，没有恢复就不打开*/
        if (clean_ctrl_comp_info.secure_error_flag) {
            clean_ctrl_comp_info.mode  = ID_MODE_NULL;
            clean_ctrl_comp_info.state = STATE_IMMEDIATELY_STOP;
            if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
            }
            /* 关闭洗地或者尘推功能按键 */
            // function_button_status_set(WASH_FLOOR_BUTTON, 0);
            // function_button_status_set(DUST_PUSH_BUTTON, 0);
        } else {
            /* 返航按键被按下，正常关闭清洁组件 */
            if (function_button_status_get(RETURN_HOME_BUTTON)) {
                clean_ctrl_comp_info.state = STATE_NORMAL_STOP;
                clean_ctrl_comp_info.mode  = ID_MODE_NULL;
                if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                    clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                }
            } else {
                /* 手动模式 */
                if (function_button_status_get(AUTO_MANUAL_BUTTON)) {
                    clean_ctrl_comp_info.mode = ID_MODE_NULL;
                    /* 清洁组件百分比默认参数设定 */
                    clean_ctrl_comp_info_default_init(&clean_ctrl_comp_info);

                    /* 洗地按钮被按下 */
                    if (function_button_status_get(WASH_FLOOR_BUTTON)) {
                        clean_ctrl_comp_info.state = STATE_WASH_START;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    } else if (function_button_status_get(DUST_PUSH_BUTTON)) {
                        /* 尘推被按下 */
                        clean_ctrl_comp_info.state = STATE_DUST_START;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    } else {
                        /* 洗地尘推都没被按下 */
                        clean_ctrl_comp_info.state = STATE_NORMAL_STOP;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    }
                } else {
                    /* 根据当前平台给定的模式进行操作 */
                    if (clean_ctrl_comp_info.mode == ID_MODE_MOPPING) {
                        /* 洗地模式 */
                        clean_ctrl_comp_info.state = STATE_WASH_START;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    } else if (clean_ctrl_comp_info.mode == ID_MODE_DEDUSTING) {
                        /* 尘推被按下 */
                        clean_ctrl_comp_info.state = STATE_DUST_START;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    } else {
                        clean_ctrl_comp_info.state = STATE_NORMAL_STOP;
                        if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                            clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                        }
                    }

                    /* 接收清洁命令超时，停止任务运行 */
                    if (clock_cpu_millisecond_diff(clean_ctrl_comp_info.last_recv_time, clock_cpu_gettime()) > 5000) {
                        clean_ctrl_comp_info.mode           = ID_MODE_NULL;
                        clean_ctrl_comp_info.state          = STATE_NORMAL_STOP;
                        clean_ctrl_comp_info.last_recv_time = clock_cpu_gettime();
                        log_d("recv mode timeout");
                    }
                }
            }
        }

        /* 过滤网不存在:1.如果开启了洗地模式，则不允许设备打开，提示错误*/
        if (clean_ctrl_comp_info.state == STATE_WASH_START) {
            if (clean_ctrl_comp_info.button_value[STRAINER_BUTTON] == 0) {
                clean_ctrl_comp_info.state = STATE_NORMAL_STOP;
                if (!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) {
                    clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state] = 1;
                }
                if (printf_div++ >= 200) {
                    printf_div = 0;
                    strainer_info_pub(clean_ctrl_comp_info.button_value[STRAINER_BUTTON]);
                    log_d("clean wash mode, not strainer");
                }
            } else {
                /*根据水位状态，判断是否使用过滤水系统,水循环，默认是清水循环：
                  1.当清水箱没水、污水箱有水的情况下，执行洗地模式时。使用污水循环过滤水进行洗地，即打开过滤水阀、打开过滤水泵、打开布水水泵进行布水
                  2.当清水箱有水时，正常利用清水洗地，即打开清水水阀、打开布水水泵进行洗地
                */
                if ((clean_ctrl_comp_info.water_level_info[1] != WATER_ERROR_LOW)) {
                    if (clean_ctrl_comp_info.enable_filter_system) {
                        clean_ctrl_comp_info.enable_filter_system = 0;
                        /* 模拟产生一次状态切换 */
                        clean_ctrl_comp_info.last_state = STATE_INIT;
                    }

                } else {
                    if (clean_ctrl_comp_info.water_level_info[0] != WATER_ERROR_LOW) {
                        /* 清水箱无水且使能过滤水系统，使用过滤系统 */
                        if(clean_ctrl_comp_info.dev_set_value[ID_FILTER_PUMP] && clean_ctrl_comp_info.dev_set_value[ID_WASTE_WATER_VALVE]) {
                            if (!clean_ctrl_comp_info.enable_filter_system) {
                                clean_ctrl_comp_info.enable_filter_system = 1;
                                /* 模拟产生一次状态切换 */
                                clean_ctrl_comp_info.last_state = STATE_INIT;
                            }
                        }
                    }
                }
                /*速度小于0.1m/s
                  1.速度小于0.1m/s的时候则认为设备停止或者转弯，则关闭布水水泵、关闭清水阀、关闭滚刷
                  2.当速度恢复大于0.1m/s后，恢复打开清水阀、打开布水水泵、打开滚刷*/

                /* 当设备全部启动完成之后，检测轮速， 将速度扩大100倍 */
                //	if(clean_ctrl_comp_info.ops_water_delay_count)
                //	{
                // 		uint8_t wheel_vel =
                // (uint8_t)(clean_ctrl_comp_info.linear * 100);
                // if(wheel_vel < 10)
                // 		{
                // 			clean_ctrl_comp_info.state =
                // STATE_WATER_CLOSE;
                // 			if(!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state])
                // 			{
                // 				clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]
                // = 1;
                // 			}
                // 		}
                // 		else
                // 		{
                // 			clean_ctrl_comp_info.state =
                // STATE_WATER_OPEN;
                // 			if(!clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state])
                // 			{
                // 				clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]
                // = 1;
                // 			}
                // 		}
                // 	}
            }
        }

        /* 水位:1.当污水箱满水的时候、触发回充命令，让机器人加排水,
         * 此策略由任务做*/
        if (((clean_ctrl_comp_info.state == STATE_IMMEDIATELY_STOP) || (clean_ctrl_comp_info.state == STATE_NORMAL_STOP) ||
             (clean_ctrl_comp_info.state == STATE_INIT)) &&
            (clean_ctrl_comp_info.water_level_info[0] == WATER_ERROR_HIGH)) {
            // cmd_charge_pub(1);
        }
        if ((clean_ctrl_comp_info.last_state != clean_ctrl_comp_info.state)) {
            log_d(
                "##### cur mode :%d, last mode :%d, cur state :%d last "
                "state:%d,enable %d ####",
                clean_ctrl_comp_info.mode, clean_ctrl_comp_info.last_mode, clean_ctrl_comp_info.state, clean_ctrl_comp_info.last_state,
                clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]);
            clean_ctrl_comp_info.dev_switch_state          = 0;
            clean_ctrl_comp_info.start_or_stop_delay_count = 0;
            clean_ctrl_comp_info.status_changed            = 1;
            clean_ctrl_comp_info.reconnect_for_drop        = false;
            clean_ctrl_comp_info.last_last_state = clean_ctrl_comp_info.last_state;
            // /* 收水，开水不重新计时间 */
            // if((clean_ctrl_comp_info.state < STATE_WATER_OPEN))
            // {
            // 	clean_ctrl_comp_info.ops_water_delay_count = 0;
            // }
            /* 任务变化抛过滤网信息 */
            strainer_info_pub(clean_ctrl_comp_info.button_value[STRAINER_BUTTON]);
        }
        /* 组件具体执行函数 */
        if (clean_ctrl_comp_info.status_changed) {
            if (clean_ctrl_comp_info.run_fun[clean_ctrl_comp_info.state] != NULL) {
                if (clean_ctrl_comp_info.run_fun[clean_ctrl_comp_info.state](
                        &clean_ctrl_comp_info, clean_ctrl_comp_info.is_enable[clean_ctrl_comp_info.state]) == OPS_COMPLETION) {
                    // if((clean_ctrl_comp_info.state == STATE_WASH_START) ||
                    // (clean_ctrl_comp_info.state == STATE_DUST_START))
                    // {
                    // 	clean_ctrl_comp_info.ops_water_delay_count = 1;
                    // }
                    if (clean_ctrl_comp_info.last_fb_time && (!clean_ctrl_comp_info.reconnect_for_drop)) {
                        clean_ctrl_comp_info.status_changed = 0;
                        vel_tmp                             = water_ctl_st_status;  // cyl:set water control
                                                                                    // start flag;
                    } else if (clean_ctrl_comp_info.reconnect_for_drop) {
                        clean_ctrl_comp_info.dev_switch_state          = 0;
                        clean_ctrl_comp_info.start_or_stop_delay_count = 0;
                        clean_ctrl_comp_info.status_changed            = 1;
                    }
                    clean_ctrl_comp_info.reconnect_for_drop = false;
                }
            }
        } else {
            /* Detect and implement specific single actions in the mode */
            /* water control */
            clean_water(&clean_ctrl_comp_info);
        }

        clean_led_control();

        /* 更新上一次状态 */
        clean_ctrl_comp_info.last_state = clean_ctrl_comp_info.state;
        clean_ctrl_comp_info.last_mode  = clean_ctrl_comp_info.mode;
        for (uint8_t i = 0; i < ID_DEV_MAX; i++) {
            clean_ctrl_comp_info.last_dev_err_value[i] = clean_ctrl_comp_info.dev_err_value[i];
        }
    }
}

void clean_ctrl_comp_info_printf(void) {
    LOG_DEBUG("-----------state and status---------------");
    LOG_DEBUG("current mode :%d, last mode :%d, last recv time :%ld", clean_ctrl_comp_info.mode, clean_ctrl_comp_info.last_mode,
              clean_ctrl_comp_info.last_recv_time);
    LOG_DEBUG("current state :%d, last state: %d, dev_switch_state :%d", clean_ctrl_comp_info.state, clean_ctrl_comp_info.last_state,
              clean_ctrl_comp_info.dev_switch_state);
    LOG_DEBUG("status_changed :%d", clean_ctrl_comp_info.status_changed);
    LOG_DEBUG("---------------safe flag------------------");
    LOG_DEBUG("emerg :%d, touch_edge_0 :%d", clean_ctrl_comp_info.emerg_flag, clean_ctrl_comp_info.touch_edge_0_flag);
    LOG_DEBUG("-------------clean device info------------");
    for (uint8_t i = 0; i < ID_DEV_MAX; i++) {
        LOG_DEBUG("%18s, set: %f, mcuset :%f, feedback: %f, error code :%d", mcu_dev_name[i], clean_ctrl_comp_info.dev_set_value[i],
                  clean_ctrl_comp_info.mcu_set_dev_value[i], clean_ctrl_comp_info.dev_fb_value[i], clean_ctrl_comp_info.dev_err_value[i]);
    }
    for (uint8_t i = 0; i < BUTTON_DEFAULT_MAX; i++) {
        LOG_DEBUG("%d value :%d", i, clean_ctrl_comp_info.button_value[i]);
    }
    LOG_DEBUG("clean water level: %ld, waste water level :%ld", clean_ctrl_comp_info.water_level_info[1],
              clean_ctrl_comp_info.water_level_info[0]);
    LOG_DEBUG("linear: %f, angular: %f", clean_ctrl_comp_info.linear, clean_ctrl_comp_info.angular);
    LOG_DEBUG("cur nav work status: %d", clean_ctrl_comp_info.nav_work_status);
    LOG_DEBUG("filter system :%d", clean_ctrl_comp_info.enable_filter_system);
    LOG_DEBUG("water system :%d, last water system :%d", clean_ctrl_comp_info.water_system_state, clean_ctrl_comp_info.last_water_system_state);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, clean_ctrl_comp_info_printf,
                 clean_ctrl_comp_info_printf, clean ctrl comp info printf);

bool get_emerg_flag(void) {
    return clean_ctrl_comp_info.emerg_flag;
}

void clear_clean_ctrl_error_flag(void) {
    clean_ctrl_comp_info.emerg_flag        = 0;
    clean_ctrl_comp_info.touch_edge_0_flag = 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, clear_clean_ctrl_error_flag,
                 clear_clean_ctrl_error_flag, clear clean ctrl error flag);

#ifdef soft_key
/* cyl:key function simulation */
void key_function_test(int button, int press) {
    if (button == RETURN_HOME_BUTTON) {
        press ? (func_button_status[RETURN_HOME_BUTTON] = 1) : (func_button_status[RETURN_HOME_BUTTON] = 0);
        button_and_led_linkage_update(RETURN_HOME_BUTTON);
    } else if (button == AUTO_MANUAL_BUTTON) {
        press ? (func_button_status[AUTO_MANUAL_BUTTON] = 1) : (func_button_status[AUTO_MANUAL_BUTTON] = 0);
        button_and_led_linkage_update(AUTO_MANUAL_BUTTON);
    } else if (button == WASH_FLOOR_BUTTON) {
        press ? (func_button_status[WASH_FLOOR_BUTTON] = 1) : (func_button_status[WASH_FLOOR_BUTTON] = 0);
        button_and_led_linkage_update(WASH_FLOOR_BUTTON);
    } else if (button == DUST_PUSH_BUTTON) {
        press ? (func_button_status[DUST_PUSH_BUTTON] = 1) : (func_button_status[DUST_PUSH_BUTTON] = 0);
        button_and_led_linkage_update(DUST_PUSH_BUTTON);
    } else if (button == STRAINER_BUTTON) {
        press ? (func_button_status[STRAINER_BUTTON] = 1) : (func_button_status[STRAINER_BUTTON] = 0);
        button_and_led_linkage_update(STRAINER_BUTTON);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_function_test, key_function_test,
                 key_function_test 1 1);

void key_return_home(int value) {
    key_function_test(RETURN_HOME_BUTTON, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_return_home, key_return_home,
                 key_return_home 1);

void key_auto_manual(int value) {
    key_function_test(AUTO_MANUAL_BUTTON, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_auto_manual, key_auto_manual,
                 key_auto_manual 1);

void key_wash_floor(int value) {
    key_function_test(WASH_FLOOR_BUTTON, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_wash_floor, key_wash_floor,
                 key_wash_floor 1);

void key_dust_push(int value) {
    key_function_test(DUST_PUSH_BUTTON, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_dust_push, key_dust_push,
                 key_dust_push 1);

void key_strainer(int value) {
    key_function_test(STRAINER_BUTTON, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, key_strainer, key_strainer,
                 key_strainer 1);

#endif

/************************************************************************************************/
/************************************************************************************************/
// void clean_motor_handle(void *argument) {}

static void clean_water(clean_ctrl_component_t *object) {
    uint32_t wheel_speed_temp = 0;
    static uint32_t debounce_time = 0;
    if(object == NULL)
        return;

    wheel_speed_temp =(uint32_t)(wheel_vel * 100);
    if(wheel_speed_temp <= WATER_CLOSE_THRESHOLD_VALUE) {
        object->water_system_state = STATE_WATER_CLOSE;
    }
    else if(wheel_speed_temp >= WATER_OPEN_THRESHOLD_VALUE) {
        object->water_system_state = STATE_WATER_OPEN;
    }

    if(cur_mode == STATE_WASH_START) {
        if(object->last_water_system_state != object->water_system_state) {
            if(debounce_time == 0) {
                debounce_time = clock_cpu_gettime();
            } else if(clock_cpu_millisecond_diff(debounce_time, clock_cpu_gettime()) >= 300) {
                debounce_time = 0;
                object->last_water_system_state = object->water_system_state;

                switch (object->water_system_state) {
                    case STATE_WATER_CLOSE:
                        fliter_pmup_set_data(0);
                        osDelay(200);
                        cloth_water_pump_set(0);
                        osDelay(400);
                        waste_water_valve_set_data(0);
                        clear_valve_set(0);
                        roll_brush_motor_set(0);
                        log_d("water off********");
                        break;
                    case STATE_WATER_OPEN:
                        if(object->enable_filter_system) {
                            waste_water_valve_set_data(1);
                            osDelay(200);
                            cloth_water_pump_set(object->dev_set_value[ID_CLOTH_WATER_PUMP]);
                            osDelay(100);
                            fliter_pmup_set_data(1);
                        } else {
                            clear_valve_set(1);
                            osDelay(200);
                            cloth_water_pump_set(object->dev_set_value[ID_CLOTH_WATER_PUMP]);
                        }
                        roll_brush_motor_set(object->dev_set_value[ID_ROLL_BURSH_MOTOR]);
                        log_d("water on**********");
                        break;
                default:
                    break;
                }
                if(object->reconnect_for_drop)
                {
                    object->reconnect_for_drop = false;
                    object->last_water_system_state = STATE_INIT;
                }
            }
        } else {
            debounce_time = 0;
        }
    } else {
        debounce_time = 0;
        object->last_water_system_state = STATE_INIT;
    }
}

void led_state_refresh(int32_t blink, int32_t brightness) {
    led_blink_data_set(blink);
    led_brightness_data_set(brightness);
}
static void clean_led_control(void) {
    static uint8_t i           = 0;
    static uint8_t trigle_flag = LED_TRIGLE_NONE;  // indicate the led trigle condition
    if (i++ % 200 != 0) {
        return;
    }
    // log_d("---- clean_led_control:i %d  trigle_flag:%d", i, trigle_flag);
    // log_d("---- clean_ctrl_comp_info.state: %d  wheel_vel:%f  rotationl_speed:%f", clean_ctrl_comp_info.state, wheel_vel, rotationl_speed);

    // 300ms blink when robot is emergency stop
    if (clean_ctrl_comp_info.emerg_flag) {
        if (trigle_flag != LED_TRIGLE_EMERG) {
            led_state_refresh(LED_BLINK_EMERGENCY, LED_DEFAULT_BRIGHTNESS);
            trigle_flag = LED_TRIGLE_EMERG;
            log_d("led state refresh --- > emerg_flag");
        }
    }
    // 750ms blink when robot is working
    else if (clean_ctrl_comp_info.state == STATE_DUST_START || clean_ctrl_comp_info.state == STATE_WASH_START ||
             (fabs(wheel_vel * 10) > 0.5f) || (fabs(rotationl_speed * 10) > 0.5f)) {
        if (trigle_flag != LED_TRIGLE_WORK) {
            led_state_refresh(LED_BLINK_NAVI_NORMAL, LED_DEFAULT_BRIGHTNESS);
            trigle_flag = LED_TRIGLE_WORK;
            log_d("led state refresh --- > work");
        }
    }
    // 500ms blink when robot is charging
    else if (!is_in_charge_state(CHARGE_STATE_IDLE)) {
        if (trigle_flag != LED_TRIGLE_CHARGE) {
            led_state_refresh(LED_BLINK_CHARGE, LED_DEFAULT_BRIGHTNESS);
            trigle_flag = LED_TRIGLE_CHARGE;
            log_d("led state refresh --- > charge");
        }
    }
    // close led when robot no working
    else {
        if (trigle_flag != LED_TRIGLE_IDLE) {
            led_state_refresh(LED_BLINK_NAVI_NORMAL, 0);
            trigle_flag = LED_TRIGLE_IDLE;
            log_d("led state refresh --- > idle");
        }
    }
}

// typedef struct _MotorHandle {
//     void *parent;
//     void *this;

//     int synchronization;
//     int prior;
//     int triggerFlag;

//     /* init */
//     int (*super)();
//     int (*init)();
//     /* thread */
//     void (*motorHandle)(void *argument);

// } MotorHandle;

// typedef struct _MotorControl {
//     MotorHandle *parent;
//     void *this;

//     /* init */
//     int (*super)();
//     int (*init)();

//     /* single ops */
//     motor_ack (*sideBrushOps)(float data);
//     motor_ack (*rollBrushOps)(float data);
//     motor_ack (*fanMotorOps)(float data);
//     motor_ack (*cleanWaterPumpOps)(float data);
//     motor_ack (*cleanPutterOps)(float data);
//     motor_ack (*dustPutterOps)(float data);
//     motor_ack (*cleanValveOps)(float data);
//     /* multi ops */
//     uint8_t (*allImmediatelyStopOps)(clean_ctrl_component_t *object);
//     uint8_t (*allNormalStopOps)(clean_ctrl_component_t *object);
//     uint8_t (*dustOps)(clean_ctrl_component_t *object);
//     uint8_t (*washOps)(clean_ctrl_component_t *object);
//     uint8_t (*waterOffOps)(clean_ctrl_component_t *object);
//     uint8_t (*waterOpenOps)(clean_ctrl_component_t *object);
//     int (*clearErrorOps)();
// } MotorControl;

// typedef struct _MotorConfig {
//     MotorHandle *parent;
//     void *this;

//     float sideBrushSpeed;
//     float rollBrushSpeed;
//     float fanBrushSpeed;
//     float cleanWaterPumpSpeed;
//     float cleanPutterStatus;
//     float dustPutterStatus;
//     float cleanValveStatus;

//     /* init */
//     int (*super)();
//     int (*init)();

//     int (*paramConfig)();

// } MotorConfig;

// typedef struct _MotorFeedback {
//     MotorHandle *parent;
//     void *this;

//     int sideBrushErr;
//     int rollBrushErr;
//     int fanErr;
//     int cleanWaterPumpErr;
//     int cleanPutterErr;
//     int dustPutterErr;
//     int cleanValveErr;

//     /* init */
//     int (*super)();
//     int (*init)();

//     int (*errorHandle)();

// } MotorFeedback;

// typedef struct MotorLogicCenter {
//     int parent;
//     int this;

//     int emergFlag;
//     int touchEdgeFlag;
//     int velocityFlag;

//     int (*init)();
//     int (*super)();

//     int (*waterOffCtrl)();
//     int (*openWaterCtrl)();

//     int (*sideBrushErrHandle)();
//     int (*rollBrushErrHandle)();
//     int (*fanErrHandle)();
//     int (*cleanWaterPumpErrHandle)();
//     int (*cleanPutterErrHandle)();
//     int (*dustPutterErrHandle)();
//     int (*cleanValveErrHandle)();
//     int (*motorErrHandle)();

//     int (*emergHandle)();
//     int (*touchEdgeHandle)();
//     int (*velocityHandle)();
//     int (*eventHandle)();

//     int (*pubMotorMessage)();

// } MotorLogicCenter;

// /* object instance */
// MotorHandle motor = {.this = &motor, .synchronization = 1, .prior = 0,
// .triggerFlag = 0, .motorHandle = clean_motor_handle}; MotorControl
// motor_operator = {.parent = &motor,
//                                .this = &motor_operator,
//                                .sideBrushOps = side_brush_motor_set,
//                                .rollBrushOps = roll_brush_motor_set,
//                                .fanMotorOps = fan_motor_set,
//                                .cleanWaterPumpOps = cloth_water_pump_set,
//                                .cleanValveOps = clear_valve_set,
//                                .cleanPutterOps = clean_putter_motor_set,
//                                .dustPutterOps = dust_putter_motor_set,
//                                .allImmediatelyStopOps =
//                                clean_ctrl_immediately_stop, .allNormalStopOps
//                                = clean_ctrl_normal_stop, .washOps =
//                                clean_ctrl_wash_start, .dustOps =
//                                clean_ctrl_dust_start};
// MotorConfig motor_configure = {
//     .parent = &motor,
//     .this = &motor_configure,
// };
// MotorFeedback motor_fb_message = {0};
// /* the center for processing logic */
// MotorLogicCenter motor_cpu = {0};
