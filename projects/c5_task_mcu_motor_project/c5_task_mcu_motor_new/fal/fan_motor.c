/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fan_motor.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: fan motor operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "board.h"

#define LOG_TAG "fan_motor"
#include "elog.h"
#define USED_FAN_MOTOR
#ifdef USED_FAN_MOTOR

void    MediumFrequencyTask(void);
void    LowFrequencyTask(void);
void    MCboot(void);
void    STO_StateObserverInterface_Init(void);
void    STO_Init(void);
uint8_t UARTC_PutChar(uint8_t ch);
// void GPIO_Configuration(void);

typedef struct {
    uint16_t b0 : 1;
    uint16_t b1 : 1;
    uint16_t b2 : 1;
    uint16_t b3 : 1;
    uint16_t b4 : 1;
    uint16_t b5 : 1;
    uint16_t b6 : 1;
    uint16_t b7 : 1;
    uint16_t b8 : 1;
    uint16_t b9 : 1;
    uint16_t b10 : 1;
    uint16_t b11 : 1;
    uint16_t b12 : 1;
    uint16_t b13 : 1;
    uint16_t b14 : 1;
    uint16_t b15 : 1;
} Bit_t;

typedef union {
    uint16_t halfword;
    Bit_t    bit;
} Flag_t;

typedef struct {
    Flag_t  hDriverFlag;
    int16_t hSpeedM1;
    int16_t hSpeedM2;
    int8_t  bData1;
    int8_t  bData2;
} DriverMsg_t;

typedef struct {
    int16_t hSpeedM1;
    int16_t hSpeedM2;

} ControlCmd_t;

uint32_t            adc1_jdr1                = 0;   // test used,read sample register value
uint32_t            adc2_jdr1                = 0;   // test used
uint16_t            fan_motor_speed_fbk_pub  = 20;  // test used,pushlish feedback speed
uint16_t            fan_motor_error_code_pub = 10;  // test used,pushlish error code
uint16_t            pub_count                = 0;
uint16_t            pub_countset             = 1000;  // 1000 * 1ms = 1000ms the time of pushlish speed and error code,temporary 1000ms
extern u16          hMecSpeedRPM;
extern DriverMsg_t  DriverMsg;
extern ControlCmd_t ControlCommand;

extern bool ping_ok;

// int16_t plotx[] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
// int16_t ploty[] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};

void fan_motor_thread(void *parameter) {
    // uint32_t fan_motor_speed_set_sub = 0;
    clean_module_ctrl_st fan_motor_info = {0};  // Used to store the speed instruction information subscribed to

    // GPIO_Configuration();
    MCboot();
    STO_StateObserverInterface_Init();
    STO_Init();
    // UARTC_PutChar(0x99);
    // UARTC_PutChar(0x55);

    uint16_t start_delay     = 0;
    uint16_t start_delay_set = 3000;

    uint32_t sub_evt = osFlagsError;

    // ControlCommand.hSpeedM1 = 0;

    /* waiting for RK,beacause of hardware requirements */
    int time = 0;
    while (1) {
        if (ping_ok || time > 150)
            break;
        osDelay(100);
        log_d("waiting for RK %d00 ms", time++);
    }

    while (1) {
        sub_evt = osEventFlagsWait(sys_suber_fan_motorHandle, SYS_EVT_FAN_MOTOR_SPEED_SET, osFlagsWaitAny, 0);

        if (sub_evt > osFlagsError) {
            sub_evt = 0;
        }

        // pushlish fan speed and error code

        fan_motor_speed_fbk_pub  = hMecSpeedRPM;
        fan_motor_error_code_pub = DriverMsg.hDriverFlag.halfword;

        if (pub_count >= pub_countset)  //
        {
            pub_count = 0;

            if (fan_motor_info.set_value) {
                fan_motor_speed_fbk_ops(0, fan_motor_speed_fbk_pub / 150);  // pushlish fan speed
            } else {
                fan_motor_speed_fbk_ops(0, 0);  // pushlish fan speed
            }

            fan_motor_error_code_ops(0, fan_motor_error_code_pub);  // pushlish error code
        } else {
            pub_count++;
        }

        // Subscribe to fan speed command
        if (sub_evt & SYS_EVT_FAN_MOTOR_SPEED_SET) {
            if (xQueuePeek(topic_fan_motor_speed_setHandle, &fan_motor_info, 0) != errQUEUE_EMPTY) {
                // utils_truncate_number_uint(&fan_motor_info.set_value, 3000, MOTOR_MAX_SPEED_RPM); //limit max speed
                fan_motor_info.set_value *= 150;

                if (fan_motor_info.set_value == 0) {
                    utils_truncate_number_int(&fan_motor_info.set_value, 0, 0);  // limit max speed
                } else {
                    utils_truncate_number_int(&fan_motor_info.set_value, 3000, 15000);  // limit max speed
                }

                log_d("fan motor set :%d", fan_motor_info.set_value);
                // log_d("fan motor speed :%d", hMecSpeedRPM);
                // log_d("fan motor error code:%d", DriverMsg.hDriverFlag.halfword);
            }
        }

        // utils_step_towards_int((int *)&ControlCommand.hSpeedM1, (int)(fan_motor_info.set_value / 6), 1); //change unit:rpm -> 0.1hz
        ControlCommand.hSpeedM1 = fan_motor_info.set_value;
        // ControlCommand.hSpeedM2 = plotx[0] + ploty[0];

        if (start_delay >= start_delay_set) {
            // ControlCommand.hSpeedM1 = 5000;
        } else {
            start_delay++;
        }

        MediumFrequencyTask();
        LowFrequencyTask();
        // CAN_CommunicationTask();

        osDelay(1);
    }
}

// void GPIO_Configuration(void)
// {
//     GPIO_InitTypeDef  GPIO_InitStructure;

//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     __HAL_RCC_GPIOB_CLK_ENABLE();
//     __HAL_RCC_GPIOC_CLK_ENABLE();
//     __HAL_RCC_AFIO_CLK_ENABLE();

//     //Precharge and Relay control, out 48V
//     GPIO_InitStructure.Pin = Precharge_Pin | Relay_control_Pin;
//     GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
// 	GPIO_InitStructure.Pull = GPIO_PULLUP;
//     GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(Relay_control_GPIO_Port, &GPIO_InitStructure);

// 	HAL_GPIO_WritePin(Precharge_GPIO_Port, Relay_control_Pin|Precharge_Pin, GPIO_PIN_RESET);

//     //power(48V) and battery fk
//     GPIO_InitStructure.Pin = V_POWER_Pin | V_BATTERY_Pin;
//     GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//     HAL_GPIO_Init(V_POWER_GPIO_Port, &GPIO_InitStructure);

// 	//test the code run time as IO
//     GPIO_InitStructure.Pin =  JTDO_3_Pin;
//     GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
// 	GPIO_InitStructure.Pull = GPIO_PULLUP;
//     GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(JTDO_3_GPIO_Port, &GPIO_InitStructure);
// }

void fan_status(void) {
    log_d("fan_motor_speed_fbk_pub: %d", fan_motor_speed_fbk_pub);
    log_d("fan_motor_error_code_pub: %d", fan_motor_error_code_pub);
    if (fan_motor_error_code_pub) {
        if (fan_motor_error_code_pub & 0x01)
            log_e("Hardware overcurrent fault!");
        else if ((fan_motor_error_code_pub >> 1) & 0x01)
            log_e("Locked-Rotor fault!");
        else if ((fan_motor_error_code_pub >> 2) & 0x01)
            log_e("Mosfet-H fault!");
        else if ((fan_motor_error_code_pub >> 3) & 0x01)
            log_e("Mosfet-L fault!");
        else if ((fan_motor_error_code_pub >> 4) & 0x01)
            log_e("Start-up fault:low voltage!");
        else if ((fan_motor_error_code_pub >> 5) & 0x01)
            log_e("prechage fault!");
        else if ((fan_motor_error_code_pub >> 6) & 0x01)
            log_e("over voltage fault!");
        else if ((fan_motor_error_code_pub >> 7) & 0x01)
            log_e("low voltage fault!");
        else if ((fan_motor_error_code_pub >> 8) & 0x01)
            log_e("soft overcurrent fault!");
        else if ((fan_motor_error_code_pub >> 9) & 0x01)
            log_e("phase fault!");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fan_status, fan_status, fan_status);

uint32_t fan_speed_get(uint8_t id) {
    // log_d("fan get: %ld", fan_motor_speed_fbk_pub);
    return fan_motor_speed_fbk_pub;
}

uint32_t fan_error_status_get(uint8_t clean_dev_id) {
    return fan_motor_error_code_pub;
}
#endif
