#include "fal.h"
#include "devices.h"
#include "define.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "pal_uros.h"
#include "log.h"
#include "shell.h"
#include "define_motor.h"
#include "gpio.h"
#include "cputime.h"
#include "fal_motor.h"
#include "fal.h"
/************************** input ************************************************************/
/* handle */
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

#define motor_message_get               clean_ctrl_comp_info
#define motor_message_get_mode          motor_message_get.mode
#define motor_message_get_ctrl_time     motor_message_get.last_recv_time
#define motor_recv_timeout              5000
#define motor_pwm_set(handle,rpm)       do{device_ioctl(handle, MOTOR_CMD_SET_RPM , (void*)&rpm);}while(0)//rpm type:int
#define motor_io_set(port,pin,status)   do{HAL_GPIO_WritePin(port,pin,status);}while(0)

/* device */
#define push_rod_ctrl(rpm)              motor_pwm_set(up_down_push_rod_handle,rpm) 
#define side_brush_ctrl(rpm)            motor_pwm_set(side_brush_handle,rpm)
#define roller_brush_ctrl(rpm)          motor_pwm_set(roller_brush_handle,rpm)
#define roller_tube_ctrl(rpm)           motor_pwm_set(roller_tube_handle,rpm)
#define clean_water_pump_ctrl(rpm)      motor_pwm_set(clean_water_pump_handle,rpm)
#if 0
#define sewage_water_pump_ctrl(rpm)     motor_pwm_set(sewage_water_pump_handle,rpm)
#else
#define sewage_water_pump_ctrl(rpm)     motor_io_set(GPIOF, GPIO_PIN_14,rpm);
#endif
#define fan_ctrl(rpm)                   motor_pwm_set(fan_motor_handle,rpm)
#if 0
#define clena_water_valve(rpm)          motor_pwm_set(clean_water_valve_handle,rpm)
#else
#define clean_water_valve(rpm)          motor_io_set(GPIOD, GPIO_PIN_3,rpm)
#endif
#if 0
#define sewage_water_valve(rpm)         motor_pwm_set(sewage_water_valve_handle,rpm)
#else
#define sewage_water_valve(rpm)         HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,rpm);
#endif

/************************** input end ********************************************************/

/************************** output ************************************************************/
/************************** output end ********************************************************/
#define SIDE_BRUSH_MOTOR_DEFAULT_VALUE		0.30	/* side brush default value */ //0.60
#define ROLL_BRUSH_MOTOR_DEFAULT_VALUE		0.71 	/* roll brush default value */
#define ROLL_TUBE_MOTOR_DEFAULT_VALUE       0.80    /* roll tube default value */
#define CLEAN_WATER_PUMP_DEFAULT_VALUE		0.42	/* clean water pump default value */
#define FAN_DEFAULT_VALUE					0.80	/* fan default value */

clean_ctrl_component_t clean_ctrl_comp_info = {0};

char *mcu_dev_name[CLEAN_MODULE_TYPE_MAX] = {
    "push_rod_wash", "side_brush",        "roll_brush",
    "roll_tube",     "clean_water_pump",  "filter_water_pump",
    "fan",           "clean_water_state", "waste_water_state"};

typedef struct {
    float            speed;
    bool             overflag;
    bool             ops_again;
    uint8_t          ops_again_num;
} motor_ctrl_message;

typedef struct _MotorHandle {
    void *parent;
    void *this;

    int synchronization;
    int prior;
    int triggerFlag;

    /* init */
    int (*super)();
    int (*init)();
    /* thread */
    osThreadId_t* motorHandle;
    void (*motorThread)(void *argument);

} MotorHandle;

typedef struct _MotorControl {
    MotorHandle *parent;
    void *this;

    /* init */
    int (*super)();
    int (*init)();

    /* single ops */
    // motor_ack (*sideBrushOps)(float data);
    // motor_ack (*rollBrushOps)(float data);
    // motor_ack (*fanMotorOps)(float data);
    // motor_ack (*cleanWaterPumpOps)(float data);
    // motor_ack (*cleanPutterOps)(float data);
    // motor_ack (*dustPutterOps)(float data);
    // motor_ack (*cleanValveOps)(float data);
    /* multi ops */
    //uint8_t (*allImmediatelyStopOps)(clean_ctrl_component_t *object);
    void (*allImmediatelyStopOps)(void);
    void (*allNormalStopOps)(void);
    void (*dustOps)(void);
    void (*washOps)(void);
    void (*waterInitOps)(void);
    void (*waterLoopOps)(void);
    int (*clearErrorOps)(void);
} MotorControl;

typedef struct _MotorConfig {
    MotorHandle *parent;
    void *this;

    // float sideBrushSpeed;
    // float rollBrushSpeed;
    // float fanBrushSpeed;
    // float cleanWaterPumpSpeed;
    // float cleanPutterStatus;
    // float dustPutterStatus;
    // float cleanValveStatus;
    uint8_t mode;						
    motor_ctrl_message motorMessage[CLEAN_MODULE_TYPE_MAX];

    /* init */
    int (*super)();
    int (*init)();

    void (*paramConfig)(void);

} MotorConfig;

typedef struct _MotorFeedback {
    MotorHandle *parent;
    void *this;

    int sideBrushErr;
    int rollBrushErr;
    int fanErr;
    int cleanWaterPumpErr;
    int cleanPutterErr;
    int dustPutterErr;
    int cleanValveErr;

    /* init */
    int (*super)();
    int (*init)();

    int (*errorHandle)();

} MotorFeedback;

typedef struct MotorLogicCenter {
    int parent;
    int this;

    int emergFlag;
    int touchEdgeFlag;
    int velocityFlag;

    int (*init)();
    int (*super)();

    int (*waterOffCtrl)();
    int (*openWaterCtrl)();

    int (*sideBrushErrHandle)();
    int (*rollBrushErrHandle)();
    int (*fanErrHandle)();
    int (*cleanWaterPumpErrHandle)();
    int (*cleanPutterErrHandle)();
    int (*dustPutterErrHandle)();
    int (*cleanValveErrHandle)();
    int (*motorErrHandle)();

    int (*emergHandle)();
    int (*touchEdgeHandle)();
    int (*velocityHandle)();
    int (*eventHandle)();

    int (*pubMotorMessage)();

} MotorLogicCenter;

typedef void (*MotorOps)(void);

void clean_ctrl_dust_start(void);
void clean_ctrl_wash_start(void);
void clean_ctrl_normal_stop(void);
void clean_ctrl_immediately_stop(void);
void clean_motor_run(void *argument);
void motor_param_update(void);
int motor_param_init(void);
void water_loop_ctrl(void);

/* Definitions for microros */
osThreadId_t motorHandle_t;
const osThreadAttr_t motor_run_attributes = {
    .name       = "clean_motor_run",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t) osPriorityNormal
};


/* object instance */
MotorHandle motor = {
    .this = &motor, 
    .motorHandle = &motorHandle_t,
    .motorThread = clean_motor_run
}; 

MotorControl motor_operator = {
    .parent = &motor,           
    .this = &motor_operator,
    .allImmediatelyStopOps = clean_ctrl_immediately_stop, 
    .allNormalStopOps = clean_ctrl_normal_stop,
    .washOps = clean_ctrl_wash_start, 
    .dustOps = clean_ctrl_dust_start,
    .waterLoopOps = water_loop_ctrl
};

MotorConfig motor_configure = {
    .parent = &motor,
    .this = &motor_configure,
    .init = motor_param_init,
    .paramConfig = motor_param_update
};

MotorFeedback motor_fb_message = {0};
/* the center for processing logic */
MotorLogicCenter motor_cpu = {0};



void clean_motor_run(void *argument)
{
    MotorOps motor_run = motor_configure.paramConfig;
    for(;;)
    {
        /* check recv time. if timeout,stop the device */
        if (clock_cpu_millisecond_diff(motor_message_get_ctrl_time, clock_cpu_gettime()) > motor_recv_timeout) {
            motor_message_get_mode = ID_MODE_NULL;
            motor_message_get_ctrl_time = clock_cpu_gettime();
            LOG_DEBUG("recv mode timeout");
        }
        /* if mode is change,execute appropriate actions */
        if(motor_configure.mode != motor_message_get_mode){
            motor_run = motor_configure.paramConfig;
            motor_run();
            if(motor_configure.mode == ID_MODE_NULL)
                motor_run = motor_operator.allNormalStopOps;
            else if(motor_configure.mode == ID_MODE_MOPPING)
                motor_run = motor_operator.washOps;
            else if(motor_configure.mode == ID_MODE_DEDUSTING)
                motor_run = motor_operator.dustOps;
            motor_run();
        }
        else{
            if(motor_configure.mode == ID_MODE_MOPPING){
                motor_run = motor_operator.waterLoopOps;
                motor_run();
            }
            else{
                osDelay(10);
            }
        }
    }
}


int fal_motor_init(void) {
    /* init */
    motor_param_init();
    /* fal motor thread create */
    motorHandle_t = osThreadNew(clean_motor_run, NULL, &motor_run_attributes);
    return 0;
}
FAL_MODULE_INIT(fal_motor_init);


void clean_ctrl_immediately_stop(void){
    int speed = 0;
    /* close sewage water pump */
    sewage_water_pump_ctrl(speed);
    /* close clean water pump */
    clean_water_pump_ctrl(speed);
    /* waiting 200ms */
    osDelay(200);
    /* close sewage water valve */
    sewage_water_valve(0);
    /* close clean water valve */
    clean_water_valve(0);
    /* close roll tube */
    roller_tube_ctrl(speed);
    /* close roll brush */
    roller_brush_ctrl(speed);
    /* close side brush */
    side_brush_ctrl(speed);
    /* close fan */
    fan_ctrl(speed);
    /* pull up the push rod */
    push_rod_ctrl(speed);
}

void clean_ctrl_normal_stop(void){
    int speed = 0;
    /* close sewage water pump */
    sewage_water_pump_ctrl(0);
    /* waiting 200ms */
    osDelay(200);
    /* close clean water pump */
    clean_water_pump_ctrl(speed);
    /* waiting 200ms */
    osDelay(200);
    /* close roll tube */
    roller_tube_ctrl(speed);
    /* close roll brush */
    roller_brush_ctrl(speed);
    /* close side brush */
    side_brush_ctrl(speed);
    /* close sewage water valve */
    sewage_water_valve(0);
    /* close clean water valve */
    clean_water_valve(0);
    /* waiting 200ms */
    osDelay(200);
    /* close fan */
    fan_ctrl(speed);
    /* pull up the push rod */
    push_rod_ctrl(speed);
}

void clean_ctrl_wash_start(void){
    int rpm = 0;
    /* open roll tube */
    rpm = (int)(motor_configure.motorMessage[ROLLER_TUBE].speed * 10.0);
    roller_tube_ctrl(rpm);
    /* water ops */
    /* open clean water valve */
    clean_water_valve(1);
    /* waiting 5s */
    osDelay(5000);
    /* open clean water pump */
    rpm = (int)(motor_configure.motorMessage[CLEAN_WATER_PUMP].speed * 10.0);
    clean_water_pump_ctrl(rpm);
    /* open sewage water pump */
    sewage_water_pump_ctrl(1);
    /* waiting 10s */
    osDelay(10000);
    /* close clean water valve */
    clean_water_valve(0);
    /* waiting 10s */
    osDelay(10000);
    /* close clean water pump */
    clean_water_pump_ctrl(rpm);
    /* close sewage water pump */
    sewage_water_pump_ctrl(0);
    /* waiting 5s */
    osDelay(5000);
}

void clean_ctrl_dust_start(void){
    int rpm = 0;
    /* open fan */
    rpm = (int)(motor_configure.motorMessage[FAN_MOTOR].speed * 10.0);
    fan_ctrl(rpm);
    /* waiting 500ms */
    osDelay(500);
    /* open roll brush */
    rpm = (int)(motor_configure.motorMessage[ROLLER_BRUSH].speed * 10.0);
    roller_brush_ctrl(rpm);
    /* open side brush */
    rpm = (int)(motor_configure.motorMessage[SIDE_BRUSH].speed * 10.0);
    side_brush_ctrl(rpm);
}

void water_loop_ctrl(void){
    int rpm = 0;
    /* open clean water valve */
    clean_water_valve(1);
    /* open clean water pump */
    rpm = (int)(motor_configure.motorMessage[CLEAN_WATER_PUMP].speed * 10.0);
    clean_water_pump_ctrl(rpm);
    /* open sewage water pump */
    sewage_water_pump_ctrl(1);
    /* waiting 2s */
    osDelay(2000);
    /* close clean water valve */
    clean_water_valve(0);
    /* waiting 3s */
    osDelay(3000);
    /* close clean water pump */
    clean_water_pump_ctrl(rpm);
    /* close sewage water pump */
    sewage_water_pump_ctrl(0);
    /* waiting 10s */
    osDelay(10000);
}

void motor_param_update(void){
    /*update motor parameter */
    motor_configure.mode = clean_ctrl_comp_info.mode;
    /* update speed */
    for(int i=0;i<CLEAN_MODULE_TYPE_MAX;i++){
        motor_configure.motorMessage[i].speed = clean_ctrl_comp_info.dev_set_value[i];
    }
}

int motor_param_init(void){
    /* set motor config init */
    motor_configure.mode = ID_MODE_NULL;
    motor_configure.motorMessage[UP_DOWM_PUSH_ROD].speed = 0;
    motor_configure.motorMessage[SIDE_BRUSH].speed = SIDE_BRUSH_MOTOR_DEFAULT_VALUE;
    motor_configure.motorMessage[ROLLER_BRUSH].speed = ROLL_BRUSH_MOTOR_DEFAULT_VALUE;
    motor_configure.motorMessage[ROLLER_TUBE].speed = ROLL_TUBE_MOTOR_DEFAULT_VALUE;
    motor_configure.motorMessage[CLEAN_WATER_PUMP].speed = CLEAN_WATER_PUMP_DEFAULT_VALUE;
    motor_configure.motorMessage[SEWAGE_WATER_PUMP].speed = 0;
    motor_configure.motorMessage[FAN_MOTOR].speed = FAN_DEFAULT_VALUE;
    motor_configure.motorMessage[CLEAN_WATER_VALVE].speed = 0;
    motor_configure.motorMessage[SEWAGE_WATER_VALVE].speed = 0;
    return 0;
}

void wash_mode_test(void){
    motor_param_init();
    clean_ctrl_comp_info.mode = ID_MODE_MOPPING;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),wash_mode_test, wash_mode_test, open wash mode for default parameter);

void dust_mode_test(void){
    motor_param_init();
    clean_ctrl_comp_info.mode = ID_MODE_DEDUSTING;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),dust_mode_test, dust_mode_test, open dust mode for default parameter);