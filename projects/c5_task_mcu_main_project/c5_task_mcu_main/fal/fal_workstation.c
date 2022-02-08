#include "fal_workstation.h"
#include "md5.h"
#include "string.h"
#include "log.h"
#include "cmsis_os.h"
#include "queue.h"
#include "clean_ctrl.h"
#include "fal_key.h"
#include "pal_uros.h"
#include "clean_manage.h"
#include "shell.h"

#define LOG_TAG "fal_workstation"
#include "elog.h"

/************************************ TO IMPROVE
*************************************
1.可以使用电池的电量信息中电池状态来区分/代替充电中或充电暂停
*********************************** TO IMPROVE
*************************************/

/*enum define *****************************************************/
typedef enum {
    WC_STATE_INIT          = 0,
    WC_STATE_ADD_WATER_S   = 1,
    WC_STATE_ADD_WATER_W   = 2,
    WC_STATE_DRAIN_WATER_S = 3,
    WC_STATE_DRAIN_WATER_W = 4,
    WC_STATE_SELF_CLEAN_S  = 5,
    WC_STATE_SELF_CLEAN_W  = 6,
    WC_STATE_FINISH        = 7,
    WC_STATE_FAILED        = 8,
    WC_STATE_END           = 9,
} WaterControlState;
/*macro*************************************************************/
#define ELECTRODE_STATE_USED 1
#define ADD_WATER_TIMEOUT    (600)
#define DRAIN_WATER_TIMEOUT  (600)
#define SELF_CLEAN_TIMEOUT   (15)
/*varaible*********************************************************/
uint32_t drain_water_timeout = DRAIN_WATER_TIMEOUT;  // 600s
uint32_t add_water_timeout   = ADD_WATER_TIMEOUT;    // 600s

/*client ********************************************************/
static client                          g_client_add_water;
static std_srvs__srv__SetBool_Request  g_add_water_request;
static std_srvs__srv__SetBool_Response g_add_water_response;

static client                          g_client_drain_water;
static std_srvs__srv__SetBool_Request  g_drain_water_request;
static std_srvs__srv__SetBool_Response g_drain_water_response;

static client                          g_client_self_clean;
static std_srvs__srv__SetBool_Request  g_self_clean_request;
static std_srvs__srv__SetBool_Response g_self_clean_response;
/*client *********************************************************/

/*subscrption ****************************************************/
static subscrption                    g_sub_battery;
static sensor_msgs__msg__BatteryState g_battery_state;
static char                           battery_header_framid_data[16] = {0};

static subscrption           g_sub_charge_state;
static std_msgs__msg__String g_charge_state;
static char                  charge_state_[16] = {0};
static uint8_t               g_charge_state_reflesh;

// static subscrption g_sub_pipe_electrode_station;
static std_msgs__msg__Bool g_electrode_state;
// static uint8_t g_pipe_electrode_reflesh;

static subscrption          g_sub_transition_tank_status;
static std_msgs__msg__UInt8 g_transition_tank_status;

static subscrption          g_sub_pile_heartbeat;
static std_msgs__msg__UInt8 g_pile_heartbeat;

static subscrption          g_sub_autocharge_feedback;
static std_msgs__msg__UInt8 g_autocharge_feedback_state;

// static subscrption g_sub_imu;
// static sensor_msgs__msg__Imu g_Imu_msg;
/*subscrption ****************************************************/

/*publish *********************************************************/
static publisher           g_pub_charge_pause;
static std_msgs__msg__Bool g_charge_pause;

// static publisher g_pub_charge_start;
// static std_msgs__msg__Empty g_charge_start;

static publisher           g_pub_battery_update;
static std_msgs__msg__Empty g_battery_update;

/*publish *********************************************************/

/*water control state ********************************************/
static WaterControlState control_state = WC_STATE_END;
/*****************************************************************/

void client_add_water_callback(const void *msg) {
    static uint8_t i = 0;
    i++;
    g_add_water_response = *((std_srvs__srv__SetBool_Response *) msg);
    log_d("client_add_water . Received service response (%u) : %d  \n", i, g_add_water_response.success);
}

void client_drain_water_callback(const void *msg) {
    static uint8_t i = 0;
    i++;
    g_drain_water_response = *((std_srvs__srv__SetBool_Response *) msg);
    log_d("client_drain_water . Received service response (%u): %d  \n", i, g_drain_water_response.success);
}

void client_self_clean_callback(const void *msg) {
    static uint8_t i = 0;
    i++;
    g_self_clean_response = *((std_srvs__srv__SetBool_Response *) msg);
    log_d("client_self_clean . Received service response (%u) :%d  \n", i, g_self_clean_response.success);
}

void sub_battery_callback(const void *msgin) {
    static uint8_t i = 0;
    i++;
    // LOG_DEBUG("sub_battery_callback  ,  response (%lu) : %f\n", i,
    // g_battery_state.percentage);
}

void sub_charge_state_callback(const void *msgin) {
    // static uint8_t i = 0;
    // i++;
    g_charge_state_reflesh++;
    log_d("sub_charge_state_callback  ,  response (%u) state: %s\n", g_charge_state_reflesh, g_charge_state.data.data);
}

// void sub_electrode_state_callback(const void * msgin){
//     //static uint8_t i = 0;
// 	//i++;
//     g_pipe_electrode_reflesh++;
//     log_d("sub_electrode_state_callback  ,  response (%u): %d\n", g_pipe_electrode_reflesh, g_electrode_state.data);
// }

void sub_transition_tank_status_callback(const void *msgin) {
    static uint8_t i = 0;
    i++;
    log_d("sub_transition_tank_status_callback  ,  response (%u):  %d\n", i, g_transition_tank_status.data);
}

void sub_pile_heartbeat_callback(const void *msgin) {
    static uint8_t i = 0;
    i++;
    log_d("sub_pile_heartbeat_callback  ,  response (%u):  %d\n", i, g_pile_heartbeat.data);
}

void sub_imu_callback(const void *msgin) {
    static uint8_t i = 0;
    i++;
    // log_d("sub_imu_callback  ,  response (%u)\n", i);
}
void sub_autocharge_feekback_callback(const void *msgin) {
    static uint8_t i = 0;
    i++;
    log_d("sub_autocharge_feekback_callback  ,  response: %u  (%u)\n", g_autocharge_feedback_state.data, i);
    if (g_autocharge_feedback_state.data == 1 || g_autocharge_feedback_state.data == 2) {
        g_electrode_state.data = true;
        enable_mcu_hub_motor(true);
    } else {
        g_electrode_state.data = false;
    }
}

void fal_workstation_set_drain_water_timeout(uint32_t timeout) {
    drain_water_timeout = timeout;
    log_d("drain water timeout : %u\n", drain_water_timeout);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
                 fal_workstation_set_drain_water_timeout, fal_workstation_set_drain_water_timeout, set drain water timeout);
void fal_workstation_set_add_water_timeout(uint32_t timeout) {
    add_water_timeout = timeout;
    log_d("add water timeout : %u\n", add_water_timeout);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
                 fal_workstation_set_add_water_timeout, fal_workstation_set_add_water_timeout, set add water timeout);

void fal_workstation_init(void) {
    // init the service-client

    // 1.  service - client -- > workstation/add_clean_water
    client_init(&g_client_add_water, ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool), "/workstation/add_clean_water", &g_add_water_request,
                &g_add_water_response, BEST, client_add_water_callback);
    // 2.  service client -->  /workstation/sewage_water
    client_init(&g_client_drain_water, ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool), "/workstation/sewage_water", &g_drain_water_request,
                &g_drain_water_response, BEST, client_drain_water_callback);
    // 3.  service client --> /workstation/self_cleaning
    client_init(&g_client_self_clean, ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool), "/workstation/self_cleaning", &g_self_clean_request,
                &g_self_clean_response, BEST, client_self_clean_callback);

    // init the subscription
    // 1. subscription ---> /battery
    g_battery_state.percentage               = 0;
    g_battery_state.header.frame_id.data     = battery_header_framid_data;
    g_battery_state.header.frame_id.capacity = sizeof(battery_header_framid_data);
    g_battery_state.header.frame_id.size     = strlen(battery_header_framid_data);
    subscrption_init(&g_sub_battery, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "/battery", &g_battery_state, DEFAULT,
                     sub_battery_callback);

    // 2. subscription ---> /auto_charge/state
    g_charge_state.data.data = charge_state_;
    memcpy(g_charge_state.data.data, CHARGE_STATE_IDLE, sizeof(CHARGE_STATE_IDLE));
    g_charge_state.data.capacity = sizeof(charge_state_);
    subscrption_init(&g_sub_charge_state, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "/auto_charge/state", &g_charge_state, BEST,
                     sub_charge_state_callback);

    // 3. subscription ---> /auto_charge/pile_electrode_station
    g_electrode_state.data = false;
    // subscrption_init(&g_sub_pipe_electrode_station,
    // 				 ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    // 				 "/auto_charge/pile_electrode_station",
    // 				 &g_electrode_state,
    // 				 DEFAULT,
    // 				 sub_electrode_state_callback
    // 				);
    // 4. subscription ---> /workstation/transition_tank_status
    g_transition_tank_status.data = 1;
    subscrption_init(&g_sub_transition_tank_status, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/workstation/transition_tank_status",
                     &g_transition_tank_status, BEST, sub_transition_tank_status_callback);
    // 5. heart beat -- > /auto_charge/pile_heartbeat
    subscrption_init(&g_sub_pile_heartbeat, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/auto_charge/pile_heartbeat", &g_pile_heartbeat,
                     DEFAULT, sub_pile_heartbeat_callback);
    // 6. autocharge feedback
    subscrption_init(&g_sub_autocharge_feedback, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/auto_charge/charge_feedback",
                     &g_autocharge_feedback_state, DEFAULT, sub_autocharge_feekback_callback);

    // init the publish
    // 1.charge pause -- > /auto_charge/pause
    g_charge_pause.data = true;
    publisher_init(&g_pub_charge_pause, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/auto_charge/pause", &g_charge_pause, BEST,
                   OVERWRITE, sizeof(std_msgs__msg__Bool));
    // 2. battery_update --->/battery/update
    publisher_init(&g_pub_battery_update, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/battery/update", &g_battery_update, DEFAULT,
                   OVERWRITE, sizeof(std_msgs__msg__Bool));
}

#ifdef ELECTRODE_STATE_USED
static bool is_electrode_down(void) {
    return g_electrode_state.data;
}
#endif

bool is_in_charge_state(char *state_str) {
    if (g_charge_state.data.data != NULL && state_str != NULL) {
        if (strstr(g_charge_state.data.data, state_str) != NULL) {
            log_d("is_in_charge_state : %s == %s", g_charge_state.data.data, state_str);
            return true;
        }
    }
    return false;
}

// static bool is_in_charging_state(void){
//     if(g_battery_state.power_supply_status == 1 ||
//     g_battery_state.power_supply_status==5 ||
//     g_battery_state.power_supply_status==6){
//         return true;
//     }
//     return false;
// }

// static bool is_in_uncharging_state(void){
//     if(g_battery_state.power_supply_status == 3){
//         return true;
//     }
//     return false;
// }

// static char* get_charge_state(void){
//     return g_charge_state.data.data;
// }

static bool add_water_control(bool is_start) {
    // send message to workstation
    // int64_t sequence_number ;
    g_add_water_request.data = is_start;
    uint8_t i                = 3;
    // rcl_ret_t rt;
    bool rt = false;
    while (i--) {
        // Send request
        // rt = rcl_send_request(&g_client_add_water.client_t,
        // &g_add_water_request, &sequence_number);
        rt = client_message_send(&g_client_add_water);
        log_d("add_water_control (%u) : %d   rt:%d\n", i, g_add_water_request.data, rt);
        // if(rt == RCL_RET_OK)
        if (rt) {
            return true;
        }
        osDelay(10);
    }
    return false;
}

static bool drain_water_control(bool is_start) {
    // send message to workstation
    // int64_t sequence_number ;
    g_drain_water_request.data = is_start;
    // rcl_ret_t rt;
    bool    rt = false;
    uint8_t i  = 3;
    while (i--) {
        // Send request
        // rt = rcl_send_request(&g_client_drain_water.client_t,
        // &g_drain_water_request, &sequence_number);
        rt = client_message_send(&g_client_drain_water);
        log_d("drain_water_control (%u) : %d   rt:%d\n", i, g_drain_water_request.data, rt);
        // if(rt == RCL_RET_OK)
        if (rt) {
            return true;
        }
        osDelay(10);
    }
    return false;
}

static bool self_clean_control(bool is_start) {
    // send message to workstation
    // int64_t sequence_number;
    g_self_clean_request.data = is_start;
    uint8_t i                 = 3;
    // rcl_ret_t rt;
    bool rt = false;
    while (i--) {
        // Send request
        // rt = rcl_send_request(&g_client_self_clean.client_t,
        // &g_self_clean_request, &sequence_number);
        rt = client_message_send(&g_client_self_clean);
        log_d("self_clean_control (%u) : %d   rt:%d\n", i, g_self_clean_request.data, rt);
        // if(rt == RCL_RET_OK)
        if (rt) {
            return true;
        }
        osDelay(10);
    }
    return false;
}

// static bool send_water_percentage( uint8_t percentage){
//     //send message to workstation
//     return true;
// }

// static uint8_t get_pure_water_percentage(void){
//     uint8_t water_percentage =0;
//     //get percentage of pure
//     water_percentage = water_level_status_get(1);
//     if(0 >= water_percentage || water_percentage >= 100){
//         LOG_DEBUG("error water percentage  : %d ,set to zero",
//         water_percentage); water_percentage = 0;
//     }
//     return water_percentage;
// }

// static uint8_t get_drain_water_percentage(void){
//     uint8_t water_percentage =0;
//     //get percentage of drain
//     water_percentage = water_level_status_get(0);
//     if(0 >= water_percentage || water_percentage >= 100){
//         LOG_DEBUG("error water percentage  : %d ,set to zero",
//         water_percentage); water_percentage = 0;
//     }
//     return water_percentage;
// }

static bool is_clean_box_full(void) {
    if (g_transition_tank_status.data != 0) {
        return true;
    }
    return false;
}

static bool is_battery_low(float percentage_low) {
    return (g_battery_state.percentage * 100) < percentage_low ? true : false;
}

static bool set_charge_pause(void) {
    message_publish(&g_pub_charge_pause);
    return true;
}

static bool is_lose_heartbeat(uint8_t over_times) {
    static std_msgs__msg__UInt8 last_pile_hearbeat;
    static std_msgs__msg__UInt8 cur_pile_heartbeat;
    static uint8_t              overtime = 0;
    last_pile_hearbeat                   = cur_pile_heartbeat;
    cur_pile_heartbeat                   = g_pile_heartbeat;
    if (last_pile_hearbeat.data == cur_pile_heartbeat.data) {
        overtime++;
    } else {
        overtime = 0;
    }
    if (overtime > over_times) {
        return true;
    }
    return false;
}
#ifdef ELECTRODE_STATE_USED
// static bool is_lose_electrode_state(uint8_t over_times){
//     static uint8_t last;
//     static uint8_t cur;
//     static uint8_t overtime= 0;
//     last = cur;
//     cur = g_pipe_electrode_reflesh;
//     if(last == cur){
//         overtime++;
//     }else {
//        overtime = 0;
//     }
//     if(overtime > over_times){
//         return true;
//     }
//     return false;
// }

// static bool is_lose_charge_state(uint8_t over_times){
//     static uint8_t last;
//     static uint8_t cur;
//     static uint8_t overtime= 0;
//     last = cur;
//     cur = g_charge_state_reflesh;
//     if(last == cur){
//         overtime++;
//     }else {
//        overtime = 0;
//     }
//     if(overtime > over_times){
//         return true;
//     }
//     return false;
// }
#endif

// static bool enable_hub_motor(bool enable){
//     uint8_t i=2;
//     do{
//         enable_mcu_hub_motor(enable);
//         osDelay(10);
//     }while(--i);
//     return true;
// }

static bool open_drain_valve(bool open) {
    uint8_t operation = open ? 1 : 0;
    sewage_water_valve_set_data(operation);
    return true;
}

static bool is_stop_add_water() {
    WATER_STATE water_state = water_level_status_get(1);
    if (water_state == WATER_ERROR_HIGH) {
        return true;
    }
    return false;
}

static bool is_stop_drain_water() {
    WATER_STATE water_state = water_level_status_get(0);
    if (water_state == WATER_ERROR_LOW) {
        return true;
    }
    return false;
}

static bool is_add_water_timeout(uint32_t times) {
    if (times >= add_water_timeout) {
        return true;
    }
    return false;
}

static bool is_drain_water_timeout(uint32_t times) {
    if (times >= drain_water_timeout) {
        return true;
    }
    return false;
}

// static bool is_happend_electrol_down_event(const std_msgs__msg__Bool electrol_state){
//     static std_msgs__msg__Bool cur_electrol_state = {.data = false};
//     static std_msgs__msg__Bool last_electrol_state= {.data = false};
//     last_electrol_state = cur_electrol_state;
//     cur_electrol_state = electrol_state;
//     if(cur_electrol_state.data != last_electrol_state.data){
//         if(cur_electrol_state.data){
//             return true;
//         }
//     }
//     return false;
// }

// static bool is_actual_electrol_up(bool state){
//     #define OVER_TIME 3
//     bool cur_state = state;
//     static uint8_t i = 0;
//     if(!cur_state){
//         if(++i>OVER_TIME){
//             i = OVER_TIME;
//             return true;
//         }
//     }else{
//         i = 0;
//     }

//     return false;
// }
static bool is_battery_full(void) {
    return (g_battery_state.percentage + 0.005) * 100 >= 100 ? true : false;
}

void set_workstation_control_state(WaterControlState state) {
    control_state = state;
}

WaterControlState get_workstation_control_state() {
    return control_state;
}

void update_battery_once(void) {
    message_publish(&g_pub_battery_update);
}

bool is_emerg_happend(void) {
    return get_emerg_flag();
}

void workstation_control_process(void) {
    static uint8_t  self_clean_times  = 0;
    static uint32_t add_water_times   = 0;
    static uint32_t drain_water_times = 0;

    // 0.get charge info ,if charge Percentage is low , charge first
    static bool last_is_electrode_down = false;
    static bool cur_is_electrode_down  = false;

    log_d(
        "===== control_state: %d , charge_state: %s , battery: %f, electrode "
        "state: %d",
        control_state, g_charge_state.data.data, g_battery_state.percentage, g_electrode_state.data);
#ifdef ELECTRODE_STATE_USED
// if(is_lose_electrode_state(5)){
//     g_electrode_state.data = false;
//     //log_d("unknow workstaion electrode state\n");
// }
#endif
    if (get_microros_sync_state() != SYNC_STATUS_SUCCEED) {
        log_d("==== microros_sync_state: %d", get_microros_sync_state());
        return;
    }

    if (is_lose_heartbeat(5)) {
        log_d("lose pile heart beat\n");
        g_transition_tank_status.data = 1;
        last_is_electrode_down        = false;
        cur_is_electrode_down         = false;
        if (control_state == WC_STATE_END) {
            return;
        }
        // LOG_DEBUG("lose pile heart beat\n");
        control_state = WC_STATE_FAILED;
        // g_transition_tank_status.data = 1;
        // g_electrode_state.data = false;//comment out for lost the heart beart
        // memcpy(g_charge_state.data.data, CHARGE_STATE_IDLE, sizeof(CHARGE_STATE_IDLE));
        // #ifndef ELECTRODE_STATE_USED
        //     g_electrode_state.data = false;
        // #endif

        goto HANDLE;
    }

#ifdef ELECTRODE_STATE_USED
    last_is_electrode_down = cur_is_electrode_down;
    cur_is_electrode_down  = is_electrode_down();
    // if trigger to workstation control
    if (cur_is_electrode_down != last_is_electrode_down) {
        if (cur_is_electrode_down) {
            enable_mcu_hub_motor(true);
            update_battery_once();
            control_state = WC_STATE_INIT;
            log_d("set to WC_STATE_INIT");
            osDelay(150);
        } else {
            control_state = WC_STATE_FAILED;
            goto HANDLE;
            // open_drain_valve(false);
        }
    }

    uint8_t i = 1;
    while (i--) {
        if (false == is_electrode_down() /*is_actual_electrol_up(g_electrode_state.data)*/) {
            // control_state = WC_STATE_FAILED;
            log_d("electrode state : !< up >!");
            return;
        }
        osDelay(10);
    }
#else
    last_is_electrode_down = cur_is_electrode_down;
    cur_is_electrode_down  = is_lose_electrode_state(5);
    if (!last_is_electrode_down) {
        if (control_state == WC_STATE_END) {
            control_state = WC_STATE_INIT;
            log_d("set control_state to WC_STATE_INIT");
        }
    }
#endif

    if ((is_in_charge_state(CHARGE_STATE_CHARGING) || is_battery_full()) /*is_in_charging_state()*/
        && control_state == WC_STATE_INIT) {
        // set tcharge psuse \ enable motor and  is heartbreat loss ?
        if (is_battery_low(20.0f)) {
            log_d("battery low ,charge first");
            return;
        }
        control_state = WC_STATE_ADD_WATER_S;
        log_d("now it is ready to workstation control");
    }

    if (WC_STATE_FAILED >= control_state && control_state >= WC_STATE_ADD_WATER_S) {
        // enable_hub_motor(true);
        set_charge_pause();
    }

    if (!is_in_charge_state(CHARGE_STATE_PAUSECHARGE) /*is_in_uncharging_state()*/) {
        log_d("no in pause state");
        // if(is_in_charge_state(CHARGE_STATE_FAILED)){
        //     control_state = WC_STATE_FAILED;
        //     goto HANDLE;
        // }
        return;
    }
HANDLE:
    switch (control_state) {
        case WC_STATE_ADD_WATER_S:
            if (is_stop_add_water()) {
                control_state = WC_STATE_DRAIN_WATER_S;
                log_d("no need add water , stop_add_water ");
            } else {
                if (false == add_water_control(true)) {
                    log_d("send add water control service error");
                    control_state = WC_STATE_FAILED;
                } else {
                    control_state = WC_STATE_ADD_WATER_W;
                    log_d("send add water control service ok");
                }
            }
            break;

        case WC_STATE_ADD_WATER_W:
            if (is_add_water_timeout(++add_water_times)) {
                log_d("add water timeou : %ld", add_water_times);
            }
            if (is_stop_add_water() || is_add_water_timeout(add_water_times)) {
                add_water_times = 0;
                if (true == add_water_control(false)) {
                    log_d("stop add water ok");
                    control_state = WC_STATE_DRAIN_WATER_S;
                } else {
                    log_d("stop add water failed ");
                    control_state = WC_STATE_FAILED;
                }
            }
            break;

        case WC_STATE_DRAIN_WATER_S:
            if (is_stop_drain_water()) {
                control_state = WC_STATE_SELF_CLEAN_S;
                log_d("no need drain water , stop drain water");
            } else {
                if (false == drain_water_control(true)) {
                    log_d("send drain water control service  OPEN error");
                    control_state = WC_STATE_FAILED;
                } else {
                    control_state = WC_STATE_DRAIN_WATER_W;
                }
            }
            break;

        case WC_STATE_DRAIN_WATER_W:
            if (is_drain_water_timeout(++drain_water_times)) {
                log_d("add water timeout : %ld", drain_water_times);
            }
            if (is_stop_drain_water() || is_drain_water_timeout(drain_water_times)) {
                drain_water_times = 0;
                open_drain_valve(false);

                if (false == drain_water_control(false)) {
                    log_d("send drain water control service  CLOSE error");
                    control_state = WC_STATE_FAILED;
                } else {
                    control_state = WC_STATE_SELF_CLEAN_S;
                }
            } else {
                if (is_clean_box_full() || is_emerg_happend()) {
                    open_drain_valve(false);
                } else {
                    open_drain_valve(true);
                }
            }
            break;

        case WC_STATE_SELF_CLEAN_S:
            if (false == self_clean_control(true)) {
                log_d("send self clean control service  OPEN error");
                control_state = WC_STATE_FAILED;
            } else {
                control_state = WC_STATE_SELF_CLEAN_W;
                log_d("send self clean control service  OPEN ok");
            }
            break;

        case WC_STATE_SELF_CLEAN_W:
            if (++self_clean_times > SELF_CLEAN_TIMEOUT) {
                if (false == self_clean_control(false)) {
                    log_d("send self clean control service  CLOSE error");
                    control_state = WC_STATE_FAILED;
                } else {
                    log_d("self clean control finish CLOSE ok ");
                    control_state = WC_STATE_FINISH;
                }
                self_clean_times = 0;
            }
            break;

        case WC_STATE_FINISH:
            // enable_hub_motor(true);
            log_d("workstation control finish");
            control_state = WC_STATE_END;
            break;

        case WC_STATE_FAILED:
            drain_water_times = 0;
            add_water_times   = 0;
            self_clean_times  = 0;
            // enable_hub_motor(true);
            open_drain_valve(false);
            log_d("workstation control failed");
            control_state = WC_STATE_END;
            break;

        case WC_STATE_END:
            log_d("workstation control end");
            break;

        default:
            break;
    }
}

// void start_worksation_control(void){
//     uint8_t i =5;
//     while(i--){
//         message_publish(&g_pub_charge_start);
//           osDelay(10);
//     }
//     set_workstation_control_state(WC_STATE_INIT);
// }

void task_workstation_control_run(void *argument) {
    uint8_t printf_div = 0;
    // uint32_t start_time = 0;
    // uint8_t i=0;
    osDelay(5000);
    while (1) {
        osDelay(1000);
        // start_time = clock_cpu_gettime();
        // if(is_happend_electrol_down_event(g_electrode_state)){
        //     enable_hub_motor(true);
        // }
        workstation_control_process();
        if (printf_div++ % 5 == 0) {
            // LOG_DEBUG("workstation control used time :%d us, i:%lu \r\n", clock_cpu_microsecond_diff(start_time, clock_cpu_gettime()), i++ );
        }
    }
}
