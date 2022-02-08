#ifndef __FAL_WORKSTATION_H__
#define __FAL_WORKSTATION_H__

#include "stdio.h"
#include <rcl/rcl.h>
#include "pal_uros.h"
#include "std_msgs/msg/u_int8.h"
#include "std_srvs/srv/set_bool.h"
#include "std_msgs/msg/string.h"
#include "std_msgs/msg/bool.h"
#include "sensor_msgs/msg/battery_state.h"
#include "std_msgs/msg/empty.h"
#include "sensor_msgs/msg/imu.h"

/*maro ************************************/
#define CHARGE_STATE_CHECKUP     "CheckUp"
#define CHARGE_STATE_CHARGING    "Charging"
#define CHARGE_STATE_FAILED      "Failed"
#define CHARGE_STATE_OBSTACLE    "Obstacle"
#define CHARGE_STATE_HEARTBREAK  "HeartBreak"
#define CHARGE_STATE_WAITCANCEL  "WaitCancel"
#define CHARGE_STATE_PAUSECHARGE "PauseCharge"
#define CHARGE_STATE_DOCKING     "Docking"
#define CHARGE_STATE_IDLE        "Idle"

void fal_workstation_init(void);
void task_workstation_control_run(void *argument);
void start_worksation_control(void);
bool is_in_charge_state(char *state_str);
#endif
