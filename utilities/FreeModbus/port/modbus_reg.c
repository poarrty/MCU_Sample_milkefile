#include "modbus_reg.h"
#include "modbus_api.h"
#include "stdint.h"
#include "stddef.h"
#include "sys_pubsub.h"
#include "sys_paras.h"
#include "bsp_led.h"
#include "string.h"
#include "elog.h"
#include "math.h"
#include "fal_cliff.h"
#include "fal_security_mng.h"
#include "fal_usound.h"
#include "fal_misc.h"
#include "rtc.h"

static bool is_speed_cmd_zero = false;

static void data_little2big(uint8_t *data, uint8_t size);
static void data_reg_change(uint8_t *data, uint8_t size);

extern uint8_t motor_speed_vw_set(int16_t speed_v, int16_t speed_w);
extern void led_lamp_set_freq(uint8_t Hz);
extern uint8_t sys_paras_set(PARAS_ITEM_E item, uint32_t val);
extern void nvg_soc_sta_set(uint8_t sta);

///< Input register --------------------------------------------------------------
static uint16_t modbus_input_reg_example = 0;

MODBUS_REG_MOTOR_DRIVE_PARAS_T modbus_input_reg_motor_drive_paras = {0};
MODBUS_REG_THTB_T modbus_input_reg_thtb = {0};
MODBUS_REG_TOUCH_EDGE_T modbus_input_reg_touch_edge = {0};
MODBUS_REG_BATTERY_T modbus_input_reg_batttery = {0};
MODBUS_REG_SOC_DOWN_CMD_T modbus_input_reg_soc_down_cmd = {0};
MODBUS_REG_USOUND_T modbus_input_reg_usound = {0};
MODBUS_REG_LUNA_T modbus_input_reg_luna = {0};
MODBUS_REG_ROBOT_STA_T modbus_input_reg_robot_sta = {0};
MODBUS_REG_VERSIFON_T modbus_input_reg_version = {0};
MODBUS_REG_RTC_TIME_T modbus_input_reg_rtc_time = {0};
MODBUS_PERP_STA_T modbus_input_reg_perp_sta = {0};

///< Holding register-------------------------------------------------------------
static uint8_t motor_holding_reg_example[4] = {0};

MODBUS_MOTOR_CTRL_SPEED_T modbus_holding_reg_motor_ctrl_speed = {0};
MODBUS_REG_MOTOR_CTRL_LOCK_T modbus_holding_reg_motor_ctrl_lock = {0};
MODBUS_REG_ROBOT_EXC_CLR_T modbus_holding_reg_exc_clr = {0};
MODBUS_REG_ROBOT_EXC_DET_T modbus_holding_reg_exc_det = {0X0001};
MODBUS_REG_LAMP_CTRL1_T modbus_holding_reg_lamp_ctrl1 = {0};
MODBUS_REG_DRV_LED_CTRL_T modbus_holding_reg_drv_led = {0};
MODBUS_REG_LAMP_CTRL2_T modbus_holding_reg_lamp_ctrl2 = {0};
MODBUS_REG_SOC_POWER_CTRL_T modbus_holding_reg_soc_power_ctrl = {0};
MODBUS_REG_SOC_POWER_STA_T modbus_holding_reg_soc_power_sta = {0};
MODBUS_REG_AUTO_CHARGE_LOC_T modbus_holding_reg_auto_charge_loc = {0};
MODBUS_REG_AUTO_CHARGE_STA_T modbus_holding_reg_auto_charge_sta = {0};
MODBUS_REG_AUTO_CHARGE_PARKING_REQ_T modbus_holding_reg_auto_charge_parking_req = {0};
MODBUS_REG_CHARGE_STA_T modbus_holding_reg_charge_sta = {0};
MODBUS_REG_AUTO_CHARGE_CMD_T modbus_holding_reg_auto_charge_cmd = {0};
MODBUS_REG_AUTO_CHARGE_PARKING_STA_T modbus_holding_reg_auto_charge_parking_sta = {0};
MODBUS_REG_SOC_DISPLAY_CTRL_T modbus_holding_reg_soc_display_ctrl = {0};
MODBUS_REG_RTC_TIME_SET_T modbus_holding_reg_rtc_time_set = {0};
MODBUS_REG_SELFTEST_STA_T modbus_holding_reg_selftest_sta = {0};
MODBUS_REG_IMU_RAW_T modbus_holding_reg_imu_raw = {0};

///< 04
uint8_t *modbus_input_reg_example_read(void);
uint8_t *modbus_input_reg_soc_down_cmd_read(void);
uint8_t *modbus_input_reg_version_read(void);
uint8_t *modbus_input_reg_version_407_read(void);
uint8_t *modbus_input_reg_motor_drive_paras_read(void);
uint8_t *modbus_input_reg_batttery_read(void);
uint8_t *modbus_input_reg_luna_read(void);
uint8_t *modbus_input_reg_thtb_read(void);
uint8_t *modbus_input_reg_tough_edge_read(void);
uint8_t *modbus_input_reg_usound_read(void);
uint8_t *modbus_input_reg_robot_sta_read(void);
uint8_t *modbus_input_reg_rtc_time_read(void);
uint8_t *modbus_input_reg_perp_sta_read(void);

///< 03 06/16
uint8_t *motor_holding_reg_example_read(void);
int motor_holding_reg_example_write(uint8_t *set_val,int start,int len);

int modbus_holding_reg_motor_ctrl_speed_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_soc_exc_clr_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_soc_exc_det_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_soc_power_sta_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_lamp_ctrl1_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_lamp_ctrl2_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_drv_led_write(uint8_t *set_val,int start,int len);

uint8_t *modbus_holding_reg_auto_charge_loc_read(void);
uint8_t *modbus_holding_reg_auto_charge_sta_read(void);
uint8_t *modbus_holding_reg_auto_charge_parking_req_read(void);
uint8_t *modbus_holding_reg_charge_sta_read(void);
uint8_t *modbus_holding_reg_selftest_sta_read(void);
uint8_t *modbus_holding_reg_motor_ctrl_lock_read(void);
uint8_t *modbus_holding_reg_imu_raw_read(void);
uint8_t *modbus_holding_reg_soc_exc_clr_read(void);
uint8_t *modbus_holding_reg_soc_exc_det_read(void);

int modbus_holding_reg_auto_charge_cmd_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_auto_charge_parking_sta_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_soc_display_ctrl_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_rtc_time_set_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_motor_ctrl_lock_write(uint8_t *set_val,int start,int len);

int modbus_holding_reg_heat_ctrl_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_charge_relay_ctrl_write(uint8_t *set_val,int start,int len);
int modbus_holding_reg_motor_pwr_ctrl_write(uint8_t *set_val,int start,int len);

///<------------------------------------------------------------------------------
///< 寄存器定义宏
#define MB_INPUT_REG(reg_addr_s,reg_addr_e,get) {(reg_addr_s), (reg_addr_e)-(reg_addr_s)+1, (get), NULL},
#define MB_RW_REG(reg_addr_s,reg_addr_e,get,set) {(reg_addr_s), (reg_addr_e)-(reg_addr_s)+1, (get),(set)},

///< 输入寄存器列表 读功能码: 04(dec)
MODBUS_REG modebus_read_reg_list[] = 
{
    MB_INPUT_REG(0x1000,0x1000, modbus_input_reg_example_read)
    MB_INPUT_REG(0x3002,0x3005, modbus_input_reg_thtb_read)
    MB_INPUT_REG(0x3015,0x3018, modbus_input_reg_tough_edge_read)
    MB_INPUT_REG(0x3020,0x302C, modbus_input_reg_batttery_read)
    MB_INPUT_REG(0x30A0,0x30A0, modbus_input_reg_soc_down_cmd_read)
    MB_INPUT_REG(0x30EC,0x30FE, modbus_input_reg_usound_read)
    MB_INPUT_REG(0x3130,0x3133, modbus_input_reg_luna_read)
    MB_INPUT_REG(0x3140,0x3148, modbus_input_reg_robot_sta_read)
    MB_INPUT_REG(0x3200,0x320D, modbus_input_reg_motor_drive_paras_read)
    MB_INPUT_REG(0x3300,0x3307, modbus_input_reg_version_read)
    MB_INPUT_REG(0x3308,0x3326, modbus_input_reg_version_407_read)
    MB_INPUT_REG(0x3400,0x3405, modbus_input_reg_rtc_time_read)
    MB_INPUT_REG(0x3500,0x3501, modbus_input_reg_perp_sta_read)
    
};

///< 保持寄存器列表 读功能码：03(dec) 写功能码: 单个寄存器06(dec) 多个寄存器16(dec)
MODBUS_REG modebus_rw_reg_list[] = 
{
    MB_RW_REG(0x2000,0x2001, motor_holding_reg_example_read, motor_holding_reg_example_write)
    MB_RW_REG(0x4000,0x4001, NULL, modbus_holding_reg_motor_ctrl_speed_write)
    MB_RW_REG(0x4100,0x4103, NULL, modbus_holding_reg_lamp_ctrl1_write)
    MB_RW_REG(0x4006,0x4006, modbus_holding_reg_motor_ctrl_lock_read, modbus_holding_reg_motor_ctrl_lock_write)
    MB_RW_REG(0x4008,0x400C, modbus_holding_reg_soc_exc_clr_read, modbus_holding_reg_soc_exc_clr_write)
    MB_RW_REG(0x400D,0x400D, modbus_holding_reg_soc_exc_det_read, modbus_holding_reg_soc_exc_det_write)
    MB_RW_REG(0x4108,0x4108, NULL, modbus_holding_reg_drv_led_write)
    MB_RW_REG(0x410A,0x410A, NULL, modbus_holding_reg_lamp_ctrl2_write)
    MB_RW_REG(0x4132,0x4132, NULL, modbus_holding_reg_soc_power_sta_write)
    MB_RW_REG(0x4200,0x4203, NULL, modbus_holding_reg_rtc_time_set_write)
    MB_RW_REG(0x4302,0x4302, modbus_holding_reg_selftest_sta_read, NULL)
    MB_RW_REG(0x4304,0x4304, NULL, modbus_holding_reg_heat_ctrl_write)
    MB_RW_REG(0x4306,0x4306, NULL, modbus_holding_reg_charge_relay_ctrl_write)
    MB_RW_REG(0x4308,0x4308, NULL, modbus_holding_reg_motor_pwr_ctrl_write)
    MB_RW_REG(0x4320,0x4351, modbus_holding_reg_imu_raw_read, NULL)
    MB_RW_REG(0x4400,0x4403, NULL, modbus_holding_reg_soc_display_ctrl_write)
    MB_RW_REG(0x5000,0x5000, modbus_holding_reg_auto_charge_loc_read, NULL)
    MB_RW_REG(0x5002,0x5002, modbus_holding_reg_auto_charge_sta_read, NULL)
    MB_RW_REG(0x5004,0x5004, modbus_holding_reg_auto_charge_parking_req_read, NULL)
    MB_RW_REG(0x5006,0x5006, modbus_holding_reg_charge_sta_read, NULL)
    MB_RW_REG(0x6000,0x6000, NULL, modbus_holding_reg_auto_charge_cmd_write)
    MB_RW_REG(0x6002,0x6002, NULL, modbus_holding_reg_auto_charge_parking_sta_write)

};

int modebus_read_reg_list_count = sizeof(modebus_read_reg_list)/sizeof(MODBUS_REG);
int modebus_rw_reg_list_count = sizeof(modebus_rw_reg_list)/sizeof(MODBUS_REG);
///<------------------------------------------------------------------------------

///< register init ---------------------------------------------------------------
void modbus_reg_init(void)
{
    char date[12];
    char time[10];
    char brach[8] =  "dev4.0_B";
    char commit[8] = {0};
    
    memset(date, 0, 12);
    memset(time, 0, 10);
    memcpy(date, __DATE__, 12);
    memcpy(time, __TIME__, 10);
    
    commit[0] = '-';
    strcpy(commit+1, SOFTWARE_VERSION);
    
    modbus_input_reg_version.MCU407 = 1;
    modbus_input_reg_version.MCU_103_NVG = 2;
    modbus_input_reg_version.motor_drive_soft = 3;
    modbus_input_reg_version.motor_drive_hard = 4;
    
    memcpy(modbus_input_reg_version.date,  date, sizeof(modbus_input_reg_version.date));
    memcpy(modbus_input_reg_version.time,  time, sizeof(modbus_input_reg_version.time));
    memcpy(modbus_input_reg_version.branch, brach, sizeof(modbus_input_reg_version.branch));
    memcpy(modbus_input_reg_version.commit, commit, sizeof(modbus_input_reg_version.commit));
    
    //< modbus api 在发送前会将寄存器大小端进行翻转，此处先翻转一次以消除影响
    data_reg_change(modbus_input_reg_version.date, sizeof(modbus_input_reg_version.date));
    data_reg_change(modbus_input_reg_version.time, sizeof(modbus_input_reg_version.time));
    data_reg_change(modbus_input_reg_version.branch, sizeof(modbus_input_reg_version.branch));
    data_reg_change(modbus_input_reg_version.commit, sizeof(modbus_input_reg_version.commit));
    
    modbus_holding_reg_auto_charge_loc.location = 0;                ///< 无效位置
    modbus_holding_reg_auto_charge_sta.sta = 1;                     ///< 未充电
    modbus_holding_reg_auto_charge_parking_req.parking_req = 1;     ///< 不需停车
    modbus_holding_reg_charge_sta.sta = 1;                          ///< 未充电
    
    modbus_holding_reg_motor_ctrl_lock.bit0_drv_enable = 1;
    
}


///< Input register 操作函数------------------------------------------------------

///< 示例
uint8_t *modbus_input_reg_example_read(void)
{
    modbus_input_reg_example++;

    return (uint8_t *)&modbus_input_reg_example;
}
///< 示例

extern uint16_t mod_nvg_updown;
uint8_t *modbus_input_reg_soc_down_cmd_read(void)
{
    return (uint8_t *)&mod_nvg_updown;
}

uint8_t *modbus_input_reg_version_read(void)
{
    return (uint8_t *)&modbus_input_reg_version;
}

uint8_t *modbus_input_reg_version_407_read(void)
{
    return (uint8_t *)&modbus_input_reg_version.date;
}

extern osMutexId_t mutex_motor_data_upload;
extern float euler_yaw_rad;          ///< 航向角
extern float euler_gyro_z_rad;       ///< 角速度
extern float speed_mps;              ///< 线速度
extern float odom_x_cm;              ///< X 轴方向位移
extern float odom_y_cm;              ///< Y 轴方向位移
extern float odom_total_cm;          ///< 总里程
uint8_t *modbus_input_reg_motor_drive_paras_read(void)
{
    osMutexAcquire(mutex_motor_data_upload, osWaitForever);
    
    modbus_input_reg_motor_drive_paras.eular_yaw = fabs(euler_yaw_rad)*100;
    if(euler_yaw_rad >= 0.0f)
        modbus_input_reg_motor_drive_paras.eular_yaw &= ~0x8000;
    else
        modbus_input_reg_motor_drive_paras.eular_yaw |= 0x8000;
    
    modbus_input_reg_motor_drive_paras.speed_w = fabs(euler_gyro_z_rad)*1000;
    if(euler_gyro_z_rad >= 0.0f)
        modbus_input_reg_motor_drive_paras.speed_w &= ~0x8000;
    else
        modbus_input_reg_motor_drive_paras.speed_w |= 0x8000;
    
    modbus_input_reg_motor_drive_paras.speed_v = fabs(speed_mps)*1000;
    if(speed_mps >= 0.0f)
        modbus_input_reg_motor_drive_paras.speed_v &= ~0x8000;
    else
        modbus_input_reg_motor_drive_paras.speed_v |= 0x8000;
    
    modbus_input_reg_motor_drive_paras.odom_x = fabs(odom_x_cm);
    if(odom_x_cm >= 0.0f)
        modbus_input_reg_motor_drive_paras.odom_x &= ~0x80000000;
    else
        modbus_input_reg_motor_drive_paras.odom_x |= 0x80000000;
    
    modbus_input_reg_motor_drive_paras.odom_y = fabs(odom_y_cm);
    if(odom_y_cm >= 0.0f)
        modbus_input_reg_motor_drive_paras.odom_y &= ~0x80000000;
    else
        modbus_input_reg_motor_drive_paras.odom_y |= 0x80000000;
    
    modbus_input_reg_motor_drive_paras.odom_total = fabs(odom_total_cm);
    if(odom_total_cm >= 0.0f)
        modbus_input_reg_motor_drive_paras.odom_total &= ~0x80000000;
    else
        modbus_input_reg_motor_drive_paras.odom_total |= 0x80000000;
    
    osMutexRelease(mutex_motor_data_upload);
    
    //log_i("odom %d %d %d %d", modbus_input_reg_motor_drive_paras.odom_x, modbus_input_reg_motor_drive_paras.odom_y, modbus_input_reg_motor_drive_paras.odom_total, modbus_input_reg_motor_drive_paras.speed_v);
    
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_x, sizeof(modbus_input_reg_motor_drive_paras.odom_x));
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_y, sizeof(modbus_input_reg_motor_drive_paras.odom_y));
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_total, sizeof(modbus_input_reg_motor_drive_paras.odom_total));

    ///< modbus api 在发送前会将寄存器大小端进行翻转，此处先翻转一次以消除影响
    data_reg_change((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_x, sizeof(modbus_input_reg_motor_drive_paras.odom_x));
    data_reg_change((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_y, sizeof(modbus_input_reg_motor_drive_paras.odom_y));
    data_reg_change((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_total, sizeof(modbus_input_reg_motor_drive_paras.odom_total));
    /*
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_x, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_x)+2, 2);
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_y, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_y)+2, 2);
    data_little2big((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_total, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_motor_drive_paras.odom_total)+2, 2);
    */
    
    return (uint8_t *)&modbus_input_reg_motor_drive_paras;
}

extern BATTERY_BMS_DATA_T   battery_bms;
uint8_t *modbus_input_reg_batttery_read(void)
{
    modbus_input_reg_batttery.voltage = battery_bms.voltage;
    modbus_input_reg_batttery.cap_remain = battery_bms.cap_remain;
    modbus_input_reg_batttery.current = battery_bms.current;
    modbus_input_reg_batttery.temp_env = battery_bms.temp_env;
    modbus_input_reg_batttery.cyc_time = battery_bms.cyc_time;
    modbus_input_reg_batttery.cap_full_in = battery_bms.cap_full_in;
    modbus_input_reg_batttery.volatage_percent = bsp_battery_volatage_percent_get();
    modbus_input_reg_batttery.protect = battery_bms.sta_protect;
    modbus_input_reg_batttery.warming = battery_bms.sta_warming;
    modbus_input_reg_batttery.temp_cell1 = battery_bms.temp_cell1;
    
    data_little2big((uint8_t*)&modbus_input_reg_batttery.cap_remain, sizeof(modbus_input_reg_batttery.cap_remain));
    data_little2big((uint8_t*)&modbus_input_reg_batttery.current, sizeof(modbus_input_reg_batttery.current));
    data_little2big((uint8_t*)&modbus_input_reg_batttery.cap_full_in, sizeof(modbus_input_reg_batttery.cap_full_in));

    ///< modbus api 在发送前会将寄存器大小端进行翻转，此处先翻转一次以消除影响
    data_reg_change((uint8_t*)&modbus_input_reg_batttery.cap_remain, sizeof(modbus_input_reg_batttery.cap_remain));
    data_reg_change((uint8_t*)&modbus_input_reg_batttery.current, sizeof(modbus_input_reg_batttery.current));
    data_reg_change((uint8_t*)&modbus_input_reg_batttery.cap_full_in, sizeof(modbus_input_reg_batttery.cap_full_in));
    /*
    data_little2big((uint8_t*)&modbus_input_reg_batttery.cap_remain, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_batttery.cap_remain)+2, 2);
    data_little2big((uint8_t*)&modbus_input_reg_batttery.current, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_batttery.current)+2, 2);
    data_little2big((uint8_t*)&modbus_input_reg_batttery.cap_full_in, 2);
    data_little2big(((uint8_t*)&modbus_input_reg_batttery.cap_full_in)+2, 2);
    */
    
    return (uint8_t *)&modbus_input_reg_batttery;
}

extern CLIFF_FRMAE_DATA_T cliff_data;
uint8_t *modbus_input_reg_luna_read(void)
{
    modbus_input_reg_luna.dis = cliff_data.dist;
    modbus_input_reg_luna.laser_strength = cliff_data.amp;
    modbus_input_reg_luna.temperature = cliff_data.temp;
    
    return (uint8_t *)&modbus_input_reg_luna;
}

extern ROBOT_THTB_T robot_thtb;
uint8_t *modbus_input_reg_thtb_read(void)
{
    modbus_input_reg_thtb.temperature_inner = robot_thtb.temperature_inner*100;
    modbus_input_reg_thtb.humidity_inner = robot_thtb.humidity_inner*100;
    modbus_input_reg_thtb.temperature_outside = robot_thtb.temperature_outside*10;
    modbus_input_reg_thtb.humidity_outside = robot_thtb.humidity_outside*10;
    
    //data_reg_change((uint8_t*)&modbus_input_reg_thtb, sizeof(modbus_input_reg_thtb));
    
    return (uint8_t *)&modbus_input_reg_thtb;
}

extern osMutexId_t mutex_touch_edge_data_upload;
extern ROBOT_TOUGH_EDGE_T robot_tough_edge;
uint8_t *modbus_input_reg_tough_edge_read(void)
{
    osMutexAcquire(mutex_touch_edge_data_upload, osWaitForever);
    
    modbus_input_reg_touch_edge.left_adc_val = robot_tough_edge.left_adc_val;
    modbus_input_reg_touch_edge.left_distance_val = robot_tough_edge.left_distance_val*100;
    modbus_input_reg_touch_edge.right_adc_val = robot_tough_edge.right_adc_val;
    modbus_input_reg_touch_edge.right_distance_val = robot_tough_edge.right_distance_val*100;
    
    //data_reg_change((uint8_t*)&modbus_input_reg_touch_edge, sizeof(modbus_input_reg_touch_edge));
    
    osMutexRelease(mutex_touch_edge_data_upload);
    
    return (uint8_t *)&modbus_input_reg_touch_edge;
}

extern NVG_USOUND_DATA_T nvg_usound_data;
extern TASK_USOUND_DATA_T task_usound_data;
extern osMutexId_t mutex_usound_data;

uint8_t *modbus_input_reg_usound_read(void)
{
    osMutexAcquire(mutex_usound_data, osWaitForever);
    
    modbus_input_reg_usound.dis_1_nvg = nvg_usound_data.dis1;
    modbus_input_reg_usound.dis_2_nvg = nvg_usound_data.dis2;
    modbus_input_reg_usound.dis_3_nvg = nvg_usound_data.dis3;
    modbus_input_reg_usound.dis_4_nvg = nvg_usound_data.dis4;
    modbus_input_reg_usound.dis_5_nvg = nvg_usound_data.dis5;
    modbus_input_reg_usound.dis_6_nvg = nvg_usound_data.dis6;
    modbus_input_reg_usound.dis_7_nvg = nvg_usound_data.dis7;
    modbus_input_reg_usound.dis_8_nvg = nvg_usound_data.dis8;
    modbus_input_reg_usound.dis_1_task = task_usound_data.dis1;
    modbus_input_reg_usound.dis_2_task = task_usound_data.dis2;
    modbus_input_reg_usound.dis_3_task = task_usound_data.dis3;
    modbus_input_reg_usound.dis_4_task = task_usound_data.dis4;
    modbus_input_reg_usound.usound_sta = 0;
    
    osMutexRelease(mutex_usound_data);
    
    return (uint8_t *)&modbus_input_reg_usound;
}


uint8_t *modbus_input_reg_robot_sta_read(void)
{
    modbus_input_reg_robot_sta.emerg = sys_exc_get(EXC65_EMERG);
    modbus_input_reg_robot_sta.crash = sys_exc_get(EXC67_CRASH);
    
    modbus_input_reg_robot_sta.motor_drv1.lock_l = sys_exc_get(EXC05_MOTOR1L_LOCK);
    modbus_input_reg_robot_sta.motor_drv1.lock_r = sys_exc_get(EXC06_MOTOR1R_LOCK);
    
    modbus_input_reg_robot_sta.io_sta.bit0_emerg = sys_exc_get(EXC65_EMERG);
    modbus_input_reg_robot_sta.io_sta.bit1_crash = sys_exc_get(EXC67_CRASH);
    
    if(sys_exc_get(EXC65_EMERG))
        modbus_input_reg_robot_sta.run_sta = 1;
    else if(sys_exc_get(EXC66_CLIFF))
        modbus_input_reg_robot_sta.run_sta = 2;
    else if(sys_exc_get(EXC67_CRASH))
        modbus_input_reg_robot_sta.run_sta = 3;
    else if(sys_exc_get(EXC68_FLOOD))
        modbus_input_reg_robot_sta.run_sta = 4;
    else if(sys_exc_get(EXC35_NVG_SOC_COM_TO))
        modbus_input_reg_robot_sta.run_sta = 5;
    else if(is_speed_cmd_zero)
        modbus_input_reg_robot_sta.run_sta = 6;
    else if(sys_exc_get(EXC05_MOTOR1L_LOCK) || sys_exc_get(EXC06_MOTOR1R_LOCK))
        modbus_input_reg_robot_sta.run_sta = 7;
    else if(sys_exc_get(EXC70_LOW_POWER))
        modbus_input_reg_robot_sta.run_sta = 9;
    else
        modbus_input_reg_robot_sta.run_sta = 0;
    
    modbus_input_reg_robot_sta.sensor_sta.bit0_flood = sys_exc_get(EXC68_FLOOD);
    modbus_input_reg_robot_sta.sensor_sta.bit1_rain = sys_exc_get(EXC69_RAIN);
    modbus_input_reg_robot_sta.sensor_sta.bit2_cliff = sys_exc_get(EXC66_CLIFF);
    modbus_input_reg_robot_sta.sensor_sta.bit3_emerg = sys_exc_get(EXC65_EMERG);
    modbus_input_reg_robot_sta.sensor_sta.bit4_crash = sys_exc_get(EXC67_CRASH);
    modbus_input_reg_robot_sta.sensor_sta.bit5_ultra_alow_power = sys_exc_get(EXC70_LOW_POWER);
    
    return (uint8_t *)&modbus_input_reg_robot_sta;
}

uint8_t *modbus_input_reg_rtc_time_read(void)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    
    modbus_input_reg_rtc_time.sta = 0;
    modbus_input_reg_rtc_time.ts.year = date.Year+2000;
    modbus_input_reg_rtc_time.ts.moon = date.Month;
    modbus_input_reg_rtc_time.ts.date = date.Date;
    modbus_input_reg_rtc_time.ts.hour = time.Hours;
    modbus_input_reg_rtc_time.ts.min = time.Minutes;
    modbus_input_reg_rtc_time.ts.second = time.Seconds;
    //modbus_input_reg_rtc_time.ts.millisecond = 1.0*(time.SecondFraction-time.SubSeconds)/time.SecondFraction*1000;
    modbus_input_reg_rtc_time.ts.millisecond = (time.SecondFraction-time.SubSeconds)*4;
    
    modbus_input_reg_rtc_time.ts.weekdate = 0;
    
    return (uint8_t *)&modbus_input_reg_rtc_time;
}

uint8_t *modbus_input_reg_perp_sta_read(void)
{

    
    return (uint8_t *)&modbus_input_reg_perp_sta;
}

///< Holding register 操作函数---------------------------------------------------

///< 示例
uint8_t *motor_holding_reg_example_read(void) 
{
    return motor_holding_reg_example;

}

int motor_holding_reg_example_write(uint8_t *set_val,int start,int len)
{
    if(len == 4)
    {
        motor_holding_reg_example[0] = set_val[0];
        motor_holding_reg_example[1] = set_val[1];
        motor_holding_reg_example[2] = set_val[2];
        motor_holding_reg_example[3] = set_val[3];
        
        return 0;
    }

    else
        return 1;

}
///< 示例

int modbus_holding_reg_soc_power_sta_write(uint8_t *set_val,int start,int len)
{
    
    if(len == 2)
    {
        uint16_t *p = (uint16_t *)set_val;
        
        nvg_soc_sta_set(*p);
        
        return 0;
    }

    else
        return 1;

}

int modbus_holding_reg_motor_ctrl_speed_write(uint8_t *set_val,int start,int len)
{
    int16_t temp_v, temp_w;
    
    if(start == 0 && len == sizeof(MODBUS_MOTOR_CTRL_SPEED_T))
    {
        MODBUS_MOTOR_CTRL_SPEED_P p = (MODBUS_MOTOR_CTRL_SPEED_P)set_val;
        
        ///< 老化导航板 SOC 的生存时间
        nvg_soc_live_update();
        
        if(p->speed_v == 0 && p->speed_w == 0)
            is_speed_cmd_zero = true;
        else
            is_speed_cmd_zero = false;
        
        if(p->speed_v & 0X8000)
            temp_v = -1*(p->speed_v&(~0X8000));
        else
            temp_v = p->speed_v;
        
        if(p->speed_w & 0X8000)
            temp_w = -1*(p->speed_w&(~0X8000));
        else
            temp_w = p->speed_w;
        
        motor_speed_vw_set(temp_v, temp_w);
        
        return 0;
    }

    else
        return 1;

}

int modbus_holding_reg_lamp_ctrl1_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_LAMP_CTRL1_T))
    {
        MODBUS_REG_LAMP_CTRL1_P p = (MODBUS_REG_LAMP_CTRL1_P)set_val;
        
        log_i("modbus lamp ctrl freq[%d]", p->lamp_shake_freq_lf);
        
        //elog_hexdump("Modbus data", len, set_val, len);
        
        led_lamp_set_freq(p->lamp_shake_freq_lf);
        
        return 0;
    }

    else
    {
        log_e("modbus lamp ctrl freq, address or size error.");
        
        return 1;
    }

}

int modbus_holding_reg_lamp_ctrl2_write(uint8_t *set_val,int start,int len)
{
    MODBUS_REG_LAMP_CTRL2_T data = *((MODBUS_REG_LAMP_CTRL2_P)set_val);
    
    if(start == 0 && len == sizeof(MODBUS_REG_LAMP_CTRL2_T))
    {
        MODBUS_REG_LAMP_CTRL2_P p = &data;
        
        /*
        __disable_irq();
        sys_paras_set(SYS_PARAS_LAMP_COLOR, p->color);
        __enable_irq();
        */
        
        sys_paras.lamp_color = p->color;
        
        log_i("modbus lamp ctrl color[%d]", sys_paras.lamp_color);
        
        /*
        if(p->color&0x01)
            bsp_lamp_color(LAMP_L, (LED_COLOR_E)sys_paras.lamp_color);
        else
            bsp_lamp_color(LAMP_L, COLOR_OFF);
        
        if(p->color&0x02)
            bsp_lamp_color(LAMP_R, (LED_COLOR_E)sys_paras.lamp_color);
        else
            bsp_lamp_color(LAMP_R, COLOR_OFF);
        */
        
        return 0;
    }

    else
    {
        log_e("modbus lamp ctrl color, address or size error.");
        return 1;
    }

}

int modbus_holding_reg_drv_led_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_DRV_LED_CTRL_T))
    {
        MODBUS_REG_DRV_LED_CTRL_P p = (MODBUS_REG_DRV_LED_CTRL_P)set_val;
        
        if(p->turn_on)
            bsp_led_on(LED_DRV);
        else
            bsp_led_off(LED_DRV);
        
        return 0;
    }

    else
        return 1;

}

uint8_t *modbus_holding_reg_auto_charge_loc_read(void)
{
    modbus_holding_reg_auto_charge_loc.location = fal_charge_get_location();
    return (uint8_t *)&modbus_holding_reg_auto_charge_loc;
}

extern uint16_t auto_charge_sta;            ///< 自动充电流程状态
uint8_t *modbus_holding_reg_auto_charge_sta_read(void)
{
    modbus_holding_reg_auto_charge_sta.sta = auto_charge_sta;
    return (uint8_t *)&modbus_holding_reg_auto_charge_sta;
}

uint8_t *modbus_holding_reg_auto_charge_parking_req_read(void)
{
    if(fal_charge_get_packing_req())
        modbus_holding_reg_auto_charge_parking_req.parking_req  = 0x02;
    else
        modbus_holding_reg_auto_charge_parking_req.parking_req  = 0x01;
    
    return (uint8_t *)&modbus_holding_reg_auto_charge_parking_req;
}

extern uint16_t charge_sta;                   ///< 机器当前充电状态
uint8_t *modbus_holding_reg_charge_sta_read(void)
{
    modbus_holding_reg_charge_sta.sta = charge_sta;
    
    return (uint8_t *)&modbus_holding_reg_charge_sta;
}

int modbus_holding_reg_auto_charge_cmd_write(uint8_t *set_val,int start,int len)
{
    CHARGE_CTRL_T ctrl;
    
    if(start == 0 && len == sizeof(MODBUS_REG_AUTO_CHARGE_PARKING_STA_T))
    {
        MODBUS_REG_AUTO_CHARGE_PARKING_STA_P p = (MODBUS_REG_AUTO_CHARGE_PARKING_STA_P)set_val;
        
        if(p->parking_sta == 0X01)
        {
            ctrl.cmd = AUTO_CHARGE_START;
            pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);
        }
        else if(p->parking_sta == 0X02)
        {
            ctrl.cmd = AUTO_CHARGE_ABORT;
            pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);
        }
        
        return 0;
    }

    else
        return 1;

}

int modbus_holding_reg_auto_charge_parking_sta_write(uint8_t *set_val,int start,int len)
{
    CHARGE_CTRL_T ctrl;
    
    if(start == 0 && len == sizeof(MODBUS_REG_AUTO_CHARGE_CMD_T))
    {
        MODBUS_REG_AUTO_CHARGE_CMD_P p = (MODBUS_REG_AUTO_CHARGE_CMD_P)set_val;
        
        if(p->cmd == 0X01)
        {
            ctrl.cmd = AUTO_CHARGE_PARKING_OK;
            pub_topic(SYS_EVT_AUTO_CHARGE, &ctrl);
        }
        
        return 0;
    }

    else
        return 1;

}

int modbus_holding_reg_soc_display_ctrl_write(uint8_t *set_val,int start,int len)
{
    DISPLAY_CTRL_T ctrl = {0};
    
    if(start == 0 && len == sizeof(MODBUS_REG_SOC_DISPLAY_CTRL_T))
    {
        MODBUS_REG_SOC_DISPLAY_CTRL_P p = (MODBUS_REG_SOC_DISPLAY_CTRL_P)set_val;
        
        if(p->cmd.display_mode == 0X00 || p->cmd.display_mode == 0X01)
        {
            log_i("soc display ctrl: mode[%d] %d %d %d", p->cmd.display_mode, p->cmd.hundred, p->cmd.ten, p->cmd.single);
            
            ctrl.cmd = p->cmd;
            
            pub_topic(SYS_EVT_DISPLAY_CTRL, &ctrl);
            
            return 0;
        }
        else
        {
            return 1;
        }
        
    }

    else
        return 1;

}

int modbus_holding_reg_rtc_time_set_write(uint8_t *set_val,int start,int len)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
    
    if(start == 0 && len == sizeof(MODBUS_REG_RTC_TIME_SET_T))
    {
        MODBUS_REG_RTC_TIME_SET_P p = (MODBUS_REG_RTC_TIME_SET_P)set_val;
        
        log_i("SOC set mcu rtc: %d-%d-%d %02d:%02d:%02d", p->year, p->moon, p->date, p->hour, p->min, p->sec);

        date.Year = p->year - 2000;
        date.Month = p->moon;
        date.WeekDay = p->weekdate;
        date.Date = p->date;
        
        time.Hours = p->hour;
        time.Minutes = p->min;
        time.Seconds = p->sec;
        
        HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
        HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
        
        ///< 时间同步功能没启用，先不通知 SOC 设置成功
        return 1;
        
    }

    else
        return 1;

}

int modbus_holding_reg_soc_exc_clr_write(uint8_t *set_val,int start,int len)
{
    if(start == 0 && len == sizeof(MODBUS_REG_ROBOT_EXC_CLR_T))
    {
        MODBUS_REG_ROBOT_EXC_CLR_P p = (MODBUS_REG_ROBOT_EXC_CLR_P)set_val;
        
        log_d("SOC Write mask [0X%02X 0X%02X 0X%02X 0X%02X 0X%02X 0X%02X 0X%02X 0X%02X 0X%02X 0X%02X]",
               p->mask[0], p->mask[1], p->mask[2], p->mask[3], p->mask[4], p->mask[5], p->mask[6], p->mask[7], p->mask[8], p->mask[9]);
        
        log_i("SOC set cliff mask[%d]", exc_get_item(p->mask, EXC66_CLIFF));
        log_i("SOC set crash mask[%d]", exc_get_item(p->mask, EXC67_CRASH));
        log_i("SOC set flood mask[%d]", exc_get_item(p->mask, EXC68_FLOOD));
        
        if(exc_get_item(p->mask, EXC66_CLIFF))
        {
            ///< 使能事件检测使能
            exc_set_item(sys_paras.sys_exc_mask, EXC66_CLIFF);
        }
        else
        {
            ///< 失能事件检测使能
            exc_clear_item(sys_paras.sys_exc_mask, EXC66_CLIFF);
            ///< 把对应已触发的系统异常清除
            sys_exc_clear(EXC66_CLIFF);
        }
        
        if(exc_get_item(p->mask, EXC67_CRASH))
        {
            ///< 使能事件检测使能
            exc_set_item(sys_paras.sys_exc_mask, EXC67_CRASH);
        }
        else
        {
            ///< 失能事件检测使能
            exc_clear_item(sys_paras.sys_exc_mask, EXC67_CRASH);
            ///< 把对应已触发的系统异常清除
            sys_exc_clear(EXC67_CRASH);
        }
        
        if(exc_get_item(p->mask, EXC68_FLOOD))
        {
            ///< 使能事件检测使能
            exc_set_item(sys_paras.sys_exc_mask, EXC68_FLOOD);
        }
        else
        {
            ///< 失能事件检测使能
            exc_clear_item(sys_paras.sys_exc_mask, EXC68_FLOOD);
            ///< 把对应已触发的系统异常清除
            sys_exc_clear(EXC68_FLOOD);
        }
        
        return 0;
        
    }

    else
        return 1;
    
}

uint8_t *modbus_holding_reg_soc_exc_clr_read(void)
{
    if( exc_get_item(sys_paras.sys_exc_mask, EXC66_CLIFF))
    {
        exc_set_item(modbus_holding_reg_exc_clr.mask, EXC66_CLIFF);
    }
    else
    {
        exc_clear_item(modbus_holding_reg_exc_clr.mask, EXC66_CLIFF);
    }
    
    if( exc_get_item(sys_paras.sys_exc_mask, EXC67_CRASH))
    {
        exc_set_item(modbus_holding_reg_exc_clr.mask, EXC67_CRASH);
    }
    else
    {
        exc_clear_item(modbus_holding_reg_exc_clr.mask, EXC67_CRASH);
    }
    
    if( exc_get_item(sys_paras.sys_exc_mask, EXC68_FLOOD))
    {
        exc_set_item(modbus_holding_reg_exc_clr.mask, EXC68_FLOOD);
    }
    else
    {
        exc_clear_item(modbus_holding_reg_exc_clr.mask, EXC68_FLOOD);
    }
    
    return (uint8_t *)&modbus_holding_reg_exc_clr;
}

int modbus_holding_reg_soc_exc_det_write(uint8_t *set_val,int start,int len)
{
    if(start == 0 && len == sizeof(MODBUS_REG_ROBOT_EXC_DET_T))
    {
        MODBUS_REG_ROBOT_EXC_DET_P p = (MODBUS_REG_ROBOT_EXC_DET_P)set_val;
        
        if(p->enable == 1)
        {
            log_i("SOC enable exc detect, old value is[%d].", modbus_holding_reg_exc_det.enable);

            modbus_holding_reg_exc_det.enable = 1;
            sys_paras.sys_exc_detect_enable = modbus_holding_reg_exc_det.enable;
            
        }
        else
        {
            log_i("SOC disable exc detect, old value is[%d].", modbus_holding_reg_exc_det.enable);
            
            modbus_holding_reg_exc_det.enable = 0;
            sys_paras.sys_exc_detect_enable = modbus_holding_reg_exc_det.enable;
            
            osDelay(20);
            
            sys_exc_clear_all();
            
        }
        
        return 0;
        
    }

    else
        return 1;
    
}

uint8_t *modbus_holding_reg_soc_exc_det_read(void)
{
    return (uint8_t *)&modbus_holding_reg_exc_det;
    
}

extern SELFTEST_STA_T selftest_sta;

uint8_t *modbus_holding_reg_selftest_sta_read(void)
{
    modbus_holding_reg_selftest_sta.selftest_sta = selftest_sta;
    return (uint8_t *)&modbus_holding_reg_selftest_sta;
}

extern uint8_t bsp_pmu_heat_on(void);
extern uint8_t bsp_pmu_heat_off(void);
    
int modbus_holding_reg_heat_ctrl_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_HEAT_CTRL_T))
    {
        MODBUS_REG_HEAT_CTRL_P p = (MODBUS_REG_HEAT_CTRL_P)set_val;
        
        if(p->turn_on)
            bsp_pmu_heat_on();
        else
            bsp_pmu_heat_off();
        
        return 0;
    }

    else
        return 1;

}

extern void fal_charge_relay_on(void);
extern void fal_charge_relay_off(void);
extern bool is_charge_relay_test_mode;

int modbus_holding_reg_charge_relay_ctrl_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_CHARGE_RELAY_CTRL_T))
    {
        MODBUS_REG_CHARGE_RELAY_CTRL_P p = (MODBUS_REG_CHARGE_RELAY_CTRL_P)set_val;
        
        if(p->turn_on)
        {
            is_charge_relay_test_mode = true;
            fal_charge_relay_on();
        }
        else
        {
            is_charge_relay_test_mode = false;
            fal_charge_relay_off();
        }
        
        return 0;
    }

    else
        return 1;

}

extern uint8_t bsp_pmu_motor_on(void);
extern uint8_t bsp_pmu_motor_off(void);

int modbus_holding_reg_motor_pwr_ctrl_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_MOTOR_PWR_CTRL_T))
    {
        MODBUS_REG_MOTOR_PWR_CTRL_P p = (MODBUS_REG_MOTOR_PWR_CTRL_P)set_val;
        
        if(p->turn_on)
            bsp_pmu_motor_on();
        else
             bsp_pmu_motor_off();
        
        return 0;
    }

    else
        return 1;

}

uint8_t *modbus_holding_reg_motor_ctrl_lock_read(void)
{
    return (uint8_t *)&modbus_holding_reg_motor_ctrl_lock;
}

extern osTimerId_t timer_motor_control;
extern void bsp_syntron_motor_enable(void);
extern void Motor_Disable(void);

int modbus_holding_reg_motor_ctrl_lock_write(uint8_t *set_val,int start,int len)
{
    
    if(start == 0 && len == sizeof(MODBUS_REG_MOTOR_CTRL_LOCK_T))
    {
        MODBUS_REG_MOTOR_CTRL_LOCK_P p = (MODBUS_REG_MOTOR_CTRL_LOCK_P)set_val;
        
        if(p->bit0_drv_enable)
        {
            osTimerStop(timer_motor_control);
            bsp_syntron_motor_enable();
            bsp_syntron_motor_enable();
            osTimerStart(timer_motor_control, 20);
            modbus_holding_reg_motor_ctrl_lock.bit0_drv_enable = 1;
        }
        else
        {
            osTimerStop(timer_motor_control);
            Motor_Disable();
            Motor_Disable();
            osTimerStart(timer_motor_control, 20);
            modbus_holding_reg_motor_ctrl_lock.bit0_drv_enable = 0;
        }
        
        modbus_holding_reg_motor_ctrl_lock.bit0_drv_enable = p->bit0_drv_enable;
        
        log_i("SOC set motor driver enable:[%d]", modbus_holding_reg_motor_ctrl_lock.bit0_drv_enable);
        
        return 0;
    }

    else
        return 1;

}

extern IMU_DATA_T imu_sensor_data;           ///< IMU 原始数据
extern MOTOR_POSITION_T position_t;          ///< 编码器位置，自研驱动器
extern int32_t pos_l_save, pos_r_save;       ///< 编码器位置，森创驱动器

uint8_t *modbus_holding_reg_imu_raw_read(void)
{
    osMutexAcquire(mutex_imu_data, osWaitForever);
    
    modbus_holding_reg_imu_raw.imu_sensor_data = imu_sensor_data;
    
    osMutexRelease(mutex_imu_data);
    
    osMutexAcquire(mutex_motor_data_upload, osWaitForever);
    
    if(sys_paras.motor_drive_type == MOTOR_DRIVE_SELF)
    {
        modbus_holding_reg_imu_raw.pos_l = position_t.position_l;
        modbus_holding_reg_imu_raw.pos_r = position_t.position_l;
        
    }
    else
    {
        modbus_holding_reg_imu_raw.pos_l = pos_l_save;
        modbus_holding_reg_imu_raw.pos_r = pos_r_save;
        
    }
    
    osMutexRelease(mutex_motor_data_upload);
    
    //data_little2big((uint8_t*)&modbus_holding_reg_imu_raw.pos_l, sizeof(modbus_holding_reg_imu_raw.pos_l));
    //data_little2big((uint8_t*)&modbus_holding_reg_imu_raw.pos_r, sizeof(modbus_holding_reg_imu_raw.pos_r));
    //data_reg_change((uint8_t*)&modbus_holding_reg_imu_raw.pos_l, sizeof(modbus_holding_reg_imu_raw.pos_l));
    //data_reg_change((uint8_t*)&modbus_holding_reg_imu_raw.pos_r, sizeof(modbus_holding_reg_imu_raw.pos_r));
    
    return (uint8_t *)&modbus_holding_reg_imu_raw;
}

static void data_little2big(uint8_t *data, uint8_t size)
{
    uint8_t temp[2];
    uint8_t i;
    
    for(i = 0; i < size/2; i++)
    {
        temp[0] = data[i];
        temp[1] = data[size-i-1];
        
        data[i] = temp[1];
        data[size-i-1] = temp[0];
        
    }
    
}

static void data_reg_change(uint8_t *data, uint8_t size)
{
    uint8_t i, temp;
    
    for(i = 0; i < size; i += 2)
    {
        temp = data[i];
        data[i] = data[i+1];
        data[i+1] = temp;
    }
    
}

