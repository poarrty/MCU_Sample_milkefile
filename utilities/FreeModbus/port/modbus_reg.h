#ifndef _MODBUS_REG_H
#define _MODBUS_REG_H
#include "typedef.h"
#include "bsp_battery.h"
#include "bsp_motor.h"
#include "fal_display.h"
#include "fal_misc.h"
#include "fal_imu.h"
#include "bsp_imu.h"

#pragma pack ( push )
#pragma pack (1)
///< 时间戳信息
typedef struct
{
    uint16_t year;
    uint8_t moon;
    uint8_t date;
    uint8_t weekdate;
    uint8_t hour;
    uint8_t min;
    uint8_t second;
    uint16_t millisecond;
    
}MODBUS_TIMESTAMP_T, *MODBUS_TIMESTAMP_P;

///< 外设通信 状态
typedef struct
{
    uint16_t bit0_can1_motor_drive1 : 1;
    uint16_t bit1_can2_battery : 1;
    uint16_t bit2_soc_uart1 : 1;
    uint16_t bit3_task_mcu103_uart2: 1;
    uint16_t bit4_luna_uart3: 1;
    uint16_t bit5_nvg_mcu103_uart4: 1;
    uint16_t bit6_imu_uart5: 1;
    uint16_t bit7_flood_io: 1;
    uint16_t bit8_thtb_inner: 1;
    uint16_t bit9_thtb_outside: 1;
    uint16_t bit10_can1_motor_drive2: 1;
    uint16_t bit11: 1;
    uint16_t bit12: 1;
    uint16_t bit13: 1;
    uint16_t bit14: 1;
    uint16_t bit15: 1;

}MODBUS_PERIPHERAL_CONNECT_STA_T, *MODBUS_PERIPHERAL_CONNECT_STA_P;

///< IO 状态
typedef struct
{
    uint16_t bit0_emerg : 1;                 ///< 急停按钮 IO
    uint16_t bit1_crash : 1;                 ///< 防撞触边 IO
    uint16_t bit2_power : 1;                 ///< 软开关机 IO
    uint16_t bit3: 1;
    uint16_t bit4: 1;
    uint16_t bit5: 1;
    uint16_t bit6: 1;
    uint16_t bit7: 1;
    uint16_t bit8: 1;
    uint16_t bit9: 1;
    uint16_t bit10: 1;
    uint16_t bit11: 1;
    uint16_t bit12: 1;
    uint16_t bit13: 1;
    uint16_t bit14: 1;
    uint16_t bit15: 1;

}MODBUS_IO_STA_T, *MODBUS_IO_STA_P;

///< 运行状态
typedef struct
{
    uint16_t bit0_normal : 1;
    uint16_t bit1_emerg : 1;
    uint16_t bit2_cliff : 1;
    uint16_t bit3_crash: 1;
    uint16_t bit4_flood: 1;
    uint16_t bit5_soc_connect: 1;
    uint16_t bit6_speed_zero: 1;
    uint16_t bit7_motor_drive_error_handle: 1;
    uint16_t bit8_emerg_handle: 1;
    uint16_t bit9_low_power: 1;
    uint16_t bit10: 1;
    uint16_t bit11: 1;
    uint16_t bit12: 1;
    uint16_t bit13: 1;
    uint16_t bit14: 1;
    uint16_t bit15: 1;

}MODBUS_RUN_STA_T, *MODBUS_RUN_STA_P;

///< 传感器状态
typedef struct
{
    uint16_t bit0_flood : 1;
    uint16_t bit1_rain : 1;
    uint16_t bit2_cliff : 1;
    uint16_t bit3_emerg: 1;
    uint16_t bit4_crash: 1;
    uint16_t bit5_ultra_alow_power: 1;
    uint16_t bit6: 1;
    uint16_t bit7: 1;
    uint16_t bit8: 1;
    uint16_t bit9: 1;
    uint16_t bit10: 1;
    uint16_t bit11: 1;
    uint16_t bit12: 1;
    uint16_t bit13: 1;
    uint16_t bit14: 1;
    uint16_t bit15: 1;

}MODBUS_SENSOR_STA_T, *MODBUS_SENSOR_STA_P;

///< 驱动器读相关
///< Modbus Addr: 0X3200
///< Modbus Code: 04(dec)
typedef struct
{
    uint16_t eular_yaw;             ///< 航偏角              单位 0.01 rad，最高位是符号位
    uint16_t speed_w;               ///< 角速度(gyro_z)      单位 mrad/s，最高位是符号位
    uint16_t speed_v;               ///< 线速度              单位 mm/s，最高位是符号位
    uint32_t odom_x;                ///< X轴方向位移         单位 cm，最高位是符号位
    uint32_t odom_y;                ///< Y轴方向位移         单位 cm，最高位是符号位
    uint32_t odom_total;            ///< 总里程              单位 cm，最高位是符号位
    MODBUS_TIMESTAMP_T ts;          ///< 时间戳
    
}MODBUS_REG_MOTOR_DRIVE_PARAS_T, *MODBUS_REG_MOTOR_DRIVE_PARAS_P;

///< 温湿度相关
///< Modbus Addr: 0X3002
///< Modbus Code: 04
typedef struct
{
    //float temperature_inner;        ///< 内部温度          单位 （需要跟应用确定单位）
    //float humidity_inner;           ///< 内部湿度          单位 （需要跟应用确定单位）
    //float temperature_outside;      ///< 外部温度          单位 （需要跟应用确定单位）
    //float humidity_outside;         ///< 外部湿度          单位 （需要跟应用确定单位）
    int16_t temperature_inner;        ///< 内部温度          单位 实际值*100
    int16_t humidity_inner;           ///< 内部湿度          单位 实际值*100
    int16_t temperature_outside;      ///< 外部温度          单位 实际值*10
    int16_t humidity_outside;         ///< 外部湿度          单位 实际值*10

}MODBUS_REG_THTB_T, *THTB_P;

///< 触边传感器相关相关
///< Modbus Addr: 0X3015
///< Modbus Code: 04
typedef struct
{
    uint16_t left_adc_val;             ///< 触边传感器1AD值   单位   实际值
    uint16_t left_distance_val;        ///< 触边传感器1距离   单位mm 实际值
    uint16_t right_adc_val;            ///< 触边传感器2AD值   单位   实际值
    uint16_t right_distance_val;       ///< 触边传感器2距离   单位mm 实际值

}MODBUS_REG_TOUCH_EDGE_T, *TOUCH_EDGE_P;

///< 电池相关
///< Modbus Addr: 0X3020
///< Modbus Code: 04
typedef struct
{
    uint16_t voltage;               ///< 电压                单位 mV
    uint32_t cap_remain;            ///< 剩余容量            单位 mAh
    uint32_t current;               ///< 电流                单位 mA
    uint16_t temp_env;              ///< 温度                单位 待定
    uint16_t cyc_time;              ///< 充电次数            即循环次数
    uint32_t cap_full_in;           ///< 满充容量            单位 mAh
    uint16_t volatage_percent;      ///< 电压百分比          0-100
    STA_PROTECT_BITS protect;       ///< 保护状态
    STA_WARMING_BITS warming;       ///< 警告状态
    int16_t temp_cell1;             ///< 电芯温度1

}MODBUS_REG_BATTERY_T, *MODBUS_REG_BATTERY_P;

///< MCU通知SOC关机相关
///< Modbus Addr: 0X30A0
///< Modbus Code: 04
typedef struct
{
    uint16_t down_cmd;              ///< 关机指令            0X01:不关机 0X02:关机

}MODBUS_REG_SOC_DOWN_CMD_T, *MODBUS_REG_SOC_DOWN_CMD_P;

///< 超声测距相关
///< Modbus Addr: 0X30EC
///< Modbus Code: 04
typedef struct
{
    uint16_t dis_1_nvg;             ///< 导航板超声测距1     单位：mm
    uint16_t dis_2_nvg;             ///< 导航板超声测距2     单位：mm
    uint16_t dis_3_nvg;             ///< 导航板超声测距3     单位：mm
    uint16_t dis_4_nvg;             ///< 导航板超声测距4     单位：mm
    uint16_t dis_5_nvg;             ///< 导航板超声测距5     单位：mm
    uint16_t dis_6_nvg;             ///< 导航板超声测距6     单位：mm
    uint16_t dis_7_nvg;             ///< 导航板超声测距7     单位：mm
    uint16_t dis_8_nvg;             ///< 导航板超声测距8     单位：mm
    uint16_t dis_1_task;            ///< 任务板超声测距1     单位：mm
    uint16_t dis_2_task;            ///< 任务板超声测距2     单位：mm
    uint16_t dis_3_task;            ///< 任务板超声测距3     单位：mm
    uint16_t dis_4_task;            ///< 任务板超声测距4     单位：mm
    uint16_t usound_sta;            ///< 超声测距状态        bit0 : bit11 十二位对应十二个探头  1：故障 0：正常

}MODBUS_REG_USOUND_T, *MODBUS_REG_USOUND_P;

///< 单点激光测距相关
///< Modbus Addr: 0X3130
///< Modbus Code: 04
typedef struct
{
    uint16_t dis;                   ///< 距离                单位：cm
    uint16_t laser_strength;        ///< 激光强度
    uint16_t temperature;           ///< 温度                单位: ℃

}MODBUS_REG_LUNA_T, *MODBUS_REG_LUNA_P;

///< 机器人状态相关
///< Modbus Addr: 0X3140
///< Modbus Code: 04
typedef struct
{
    uint16_t emerg;                                 ///< 急停开关            0：非紧急停止 1：紧急停止
    uint16_t crash;                                 ///< 触边                0：非触边 1：触边
    MOTOR_ERROR_BITS_T motor_drv1;                  ///< 驱动器1错误码
    MOTOR_ERROR_BITS_T motor_drv2;                  ///< 驱动器2错误码（四驱预留）
    MODBUS_PERIPHERAL_CONNECT_STA_T perph_con1;     ///< 外设通信状态1
    uint16_t perph_con2;                            ///< 外设通信状态2（预留）
    MODBUS_IO_STA_T io_sta;                         ///< IO 状态
    uint16_t run_sta;                               ///< 运行状态
    MODBUS_SENSOR_STA_T sensor_sta;                 ///< 传感器状态

}MODBUS_REG_ROBOT_STA_T, *MODBUS_REG_ROBOT_STA_P;

///< 软件版本相关
///< Modbus Addr: 0X3300
///< Modbus Code: 04
typedef struct
{
    uint16_t MCU407;
    uint16_t MCU_103_NVG;
    uint16_t motor_drive_soft;
    uint16_t motor_drive_hard;
    uint8_t date[12];
    uint8_t time[10];
    uint8_t branch[8];
    uint8_t commit[8];

}MODBUS_REG_VERSIFON_T, *MODBUS_REG_VERSION_P;

///< MCU RTC 时间
///< Modbus Addr: 0X3400
///< Modbus Code: 04
typedef struct
{
    uint16_t sta;           ///< 状态              0：正常 1：故障
    MODBUS_TIMESTAMP_T ts;

}MODBUS_REG_RTC_TIME_T, *MODBUS_REG_RTC_TIME_P;

///< 外设状态相关
///< Modbus Addr: 0X3500
///< Modbus Code: 04
typedef struct
{
    uint16_t sta_bits;
    uint16_t reserve;

}MODBUS_PERP_STA_T, *MODBUS_PERP_STA_P;

///< 电机速度控制
///< Modbus Addr: 0X4000
///< Modbus Code: 03 06/16
typedef struct
{
    uint16_t speed_v;       ///< 线速度              单位：mm/s 最高位为符号位   注意文档有误，v 和 w 相反
    uint16_t speed_w;       ///< 角速度              单位：mrad/s 最高位为符号位

}MODBUS_MOTOR_CTRL_SPEED_T, *MODBUS_MOTOR_CTRL_SPEED_P;

///< 电机刹车控制
///< Modbus Addr: 0X4006
///< Modbus Code: 03 06
typedef struct
{
    uint16_t bit0_drv_enable : 1;
    uint16_t bit1_clear_error : 1;
    uint16_t bit2_emerg_lock : 1;
    uint16_t bit3_normal_lock: 1;
    uint16_t bit4: 1;
    uint16_t bit5: 1;
    uint16_t bit6: 1;
    uint16_t bit7: 1;
    uint16_t bit8: 1;
    uint16_t bit9: 1;
    uint16_t bit10: 1;
    uint16_t bit11: 1;
    uint16_t bit12: 1;
    uint16_t bit13: 1;
    uint16_t bit14: 1;
    uint16_t bit15: 1;

}MODBUS_REG_MOTOR_CTRL_LOCK_T, *MODBUS_REG_MOTOR_CTRL_LOCK_P;

///< 机器人异常事件处理
///< Modbus Addr: 0X4008
///< Modbus Code: 03 06
typedef struct
{
    uint8_t mask[10];                //事件检测屏蔽寄存器

}MODBUS_REG_ROBOT_EXC_CLR_T, *MODBUS_REG_ROBOT_EXC_CLR_P;

///< 机器人异常检测总开关
///< Modbus Addr: 0X400D
///< Modbus Code: 03 06
typedef struct
{
    uint16_t enable;                //事件检测总开关

}MODBUS_REG_ROBOT_EXC_DET_T, *MODBUS_REG_ROBOT_EXC_DET_P;

///< 示廓灯控制1
///< Modbus Addr: 0X4100
///< Modbus Code: 03 06/16
typedef struct
{
    uint8_t lamp_strength_lf;        ///< 左前示廓灯亮度          0：不亮 ；1~15：越来越暗
    uint8_t lamp_shake_freq_lf;      ///< 左前示廓灯闪烁频率      0：不闪烁；1~30：频率越来越低
    uint8_t lamp_strength_lb;        ///< 左后示廓灯亮度          0：不亮 ；1~15：越来越暗
    uint8_t lamp_shake_freq_lb;      ///< 左后示廓灯闪烁频率      0：不闪烁；1~30：频率越来越低
    uint8_t lamp_strength_rf;        ///< 右前示廓灯亮度          0：不亮 ；1~15：越来越暗
    uint8_t lamp_shake_freq_rf;      ///< 右前示廓灯闪烁频率      0：不闪烁；1~30：频率越来越低
    uint8_t lamp_strength_rb;        ///< 右前示廓灯亮度          0：不亮 ；1~15：越来越暗
    uint8_t lamp_shake_freq_rb;      ///< 右前示廓灯闪烁频率      0：不闪烁；1~30：频率越来越低
    
}MODBUS_REG_LAMP_CTRL1_T, *MODBUS_REG_LAMP_CTRL1_P;

///< 行车灯控制
///< Modbus Addr: 0X4108
///< Modbus Code: 03 06
typedef struct
{
    uint16_t turn_on;                ///< 开关                    0：关闭；1：打开
    
}MODBUS_REG_DRV_LED_CTRL_T, *MODBUS_REG_DRV_LED_CTRL_P;

///< 示廓灯控制2
///< Modbus Addr: 0X410A
///< Modbus Code: 03 06
typedef struct
{
    uint8_t color;                  ///< 颜色控制
    uint8_t turn_on;                ///< 开关控制                 0：不亮；bit0: 左亮 bit1: 右亮
    
}MODBUS_REG_LAMP_CTRL2_T, *MODBUS_REG_LAMP_CTRL2_P;

///< SOC 向 MCU 发送开关控制
///< Modbus Addr: 0X4130
///< Modbus Code: 06
typedef struct
{
    uint16_t cmd;                   ///< 开关控制                 0X01：关机；0X02：重启
    
}MODBUS_REG_SOC_POWER_CTRL_T, *MODBUS_REG_SOC_POWER_CTRL_P;

///< SOC 向 MCU 发送开关状态
///< Modbus Addr: 0X4132
///< Modbus Code: 06
typedef struct
{
    uint16_t sta;                   ///< 开关状态                 0X01：关机完成；0X02：开机完成
    
}MODBUS_REG_SOC_POWER_STA_T, *MODBUS_REG_SOC_POWER_STA_P;

///< MCU RTC 时间
///< Modbus Addr: 0X4200
///< Modbus Code: 16
typedef struct
{
    /*
    uint16_t year;
    uint8_t moon;
    uint8_t date;
    uint8_t weekdate;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    */
    uint16_t year;
    uint8_t date;
    uint8_t moon;
    uint8_t hour;
    uint8_t weekdate;
    uint8_t sec;
    uint8_t min;
    
    
}MODBUS_REG_RTC_TIME_SET_T, *MODBUS_REG_RTC_TIME_SET_P;


///< SOC 向 MCU 设置数码管显示模式
///< Modbus Addr: 0X4400
///< Modbus Code: 16
typedef struct
{
    DISPLAY_CMD_T cmd;
    
}MODBUS_REG_SOC_DISPLAY_CTRL_T, *MODBUS_REG_SOC_DISPLAY_CTRL_P;

///< 自动充电相关：充电位置信息
///< Modbus Addr: 0X5000
///< Modbus Code: 03
typedef struct
{
    uint16_t location;              ///< 充电位置
    
}MODBUS_REG_AUTO_CHARGE_LOC_T, *MODBUS_REG_AUTO_CHARGE_LOC_P;

///< 自动充电相关：自动充电状态（结果）
///< Modbus Addr: 0X5002
///< Modbus Code: 03
typedef struct
{
    uint16_t sta;                   ///< 自动充电状态             0X01：未充电；0X02：充电中 0X03:充电失败
    
}MODBUS_REG_AUTO_CHARGE_STA_T, *MODBUS_REG_AUTO_CHARGE_STA_P;

///< 自动充电相关：请求充电停车
///< Modbus Addr: 0X5004
///< Modbus Code: 03
typedef struct
{
    uint16_t parking_req;           ///< 自动充电停车请求         0X01：不停；0X02：停车
    
}MODBUS_REG_AUTO_CHARGE_PARKING_REQ_T, *MODBUS_REG_AUTO_CHARGE_PARKING_REQ_P;

///< 机器人充电状态
///< Modbus Addr: 0X5006
///< Modbus Code: 03
typedef struct
{
    uint16_t sta;                   ///< 自动充电状态             0X01：未充电；0X02：自动充电中 0X03:手推充电中 0X04:有线充电中
    
}MODBUS_REG_CHARGE_STA_T, *MODBUS_REG_CHARGE_STA_P;

///< 自动充电相关：自动充电指令
///< Modbus Addr: 0X6000
///< Modbus Code: 06
typedef struct
{
    uint16_t cmd;                   ///< 自动充电指令             0X01：开始自动充电；0X02：取消自动充电
    
}MODBUS_REG_AUTO_CHARGE_CMD_T, *MODBUS_REG_AUTO_CHARGE_CMD_P;

///< 自动充电相关：充电停车状态
///< Modbus Addr: 0X6002
///< Modbus Code: 06
typedef struct
{
    uint16_t parking_sta;          ///< 自动充电停车请求           0X01：停车完成
    
}MODBUS_REG_AUTO_CHARGE_PARKING_STA_T, *MODBUS_REG_AUTO_CHARGE_PARKING_STA_P;

/********************************* 以下仅供板卡测试使用 *************************/
///< 设备自检状态
///< Modbus Addr: 0X4302
///< Modbus Code: 
typedef struct
{
    SELFTEST_STA_T selftest_sta;
    
}MODBUS_REG_SELFTEST_STA_T, *MODBUS_REG_SELFTEST_STA_P;

///< 加热控制
///< Modbus Addr: 0X4304
///< Modbus Code: 
typedef struct
{
    uint16_t turn_on;                ///< 开关                    0：关闭；1：打开
    
}MODBUS_REG_HEAT_CTRL_T, *MODBUS_REG_HEAT_CTRL_P;

///< 充电继电器控制
///< Modbus Addr: 0X4306
///< Modbus Code: 
typedef struct
{
    uint16_t turn_on;                ///< 开关                    0：关闭；1：打开
    
}MODBUS_REG_CHARGE_RELAY_CTRL_T, *MODBUS_REG_CHARGE_RELAY_CTRL_P;

///< 驱动器电源控制
///< Modbus Addr: 0X4308
///< Modbus Code: 
typedef struct
{
    uint16_t turn_on;                ///< 开关                    0：关闭；1：打开
    
}MODBUS_REG_MOTOR_PWR_CTRL_T, *MODBUS_REG_MOTOR_PWR_CTRL_P;

///< IMU 原始数据
///< Modbus Addr: 0X4320
///< Modbus Code: 
typedef struct
{
    IMU_DATA_T imu_sensor_data;
    uint32_t pos_l;                 ///< 左轮编码器位置      转动一圈增量 自研：600 森创: 5000
    uint32_t pos_r;                 ///< 右轮编码器位置      转动一圈增量 安保：600 森创: 5000
    
}MODBUS_REG_IMU_RAW_T, *MODBUS_REG_IMU_RAW_P;

#pragma pack ( pop )

/********************************************************************************/

void modbus_reg_init(void);


#endif

