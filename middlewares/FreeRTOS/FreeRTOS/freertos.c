/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
void system_init(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for task_default */
osThreadId_t task_defaultHandle;
const osThreadAttr_t task_default_attributes = {
  .name = "task_default",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_shell */
osThreadId_t task_fal_shellHandle;
uint32_t task_fal_shellBuffer[ 256 ];
osStaticThreadDef_t task_fal_shellControlBlock;
const osThreadAttr_t task_fal_shell_attributes = {
  .name = "task_fal_shell",
  .cb_mem = &task_fal_shellControlBlock,
  .cb_size = sizeof(task_fal_shellControlBlock),
  .stack_mem = &task_fal_shellBuffer[0],
  .stack_size = sizeof(task_fal_shellBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_log */
osThreadId_t task_fal_logHandle;
uint32_t task_fal_logBuffer[ 256 ];
osStaticThreadDef_t task_fal_logControlBlock;
const osThreadAttr_t task_fal_log_attributes = {
  .name = "task_fal_log",
  .cb_mem = &task_fal_logControlBlock,
  .cb_size = sizeof(task_fal_logControlBlock),
  .stack_mem = &task_fal_logBuffer[0],
  .stack_size = sizeof(task_fal_logBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_update */
osThreadId_t task_fal_updateHandle;
uint32_t task_fal_updateBuffer[ 256 ];
osStaticThreadDef_t task_fal_updateControlBlock;
const osThreadAttr_t task_fal_update_attributes = {
  .name = "task_fal_update",
  .cb_mem = &task_fal_updateControlBlock,
  .cb_size = sizeof(task_fal_updateControlBlock),
  .stack_mem = &task_fal_updateBuffer[0],
  .stack_size = sizeof(task_fal_updateBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_button */
osThreadId_t task_fal_buttonHandle;
uint32_t task_fal_buttonBuffer[ 256 ];
osStaticThreadDef_t task_fal_buttonControlBlock;
const osThreadAttr_t task_fal_button_attributes = {
  .name = "task_fal_button",
  .cb_mem = &task_fal_buttonControlBlock,
  .cb_size = sizeof(task_fal_buttonControlBlock),
  .stack_mem = &task_fal_buttonBuffer[0],
  .stack_size = sizeof(task_fal_buttonBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_led */
osThreadId_t task_fal_ledHandle;
uint32_t task_fal_ledBuffer[ 256 ];
osStaticThreadDef_t task_fal_ledControlBlock;
const osThreadAttr_t task_fal_led_attributes = {
  .name = "task_fal_led",
  .cb_mem = &task_fal_ledControlBlock,
  .cb_size = sizeof(task_fal_ledControlBlock),
  .stack_mem = &task_fal_ledBuffer[0],
  .stack_size = sizeof(task_fal_ledBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_state */
osThreadId_t task_fal_stateHandle;
uint32_t task_fal_stateBuffer[ 256 ];
osStaticThreadDef_t task_fal_stateControlBlock;
const osThreadAttr_t task_fal_state_attributes = {
  .name = "task_fal_state",
  .cb_mem = &task_fal_stateControlBlock,
  .cb_size = sizeof(task_fal_stateControlBlock),
  .stack_mem = &task_fal_stateBuffer[0],
  .stack_size = sizeof(task_fal_stateBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_battery */
osThreadId_t task_fal_batteryHandle;
uint32_t task_fal_batteryBuffer[ 256 ];
osStaticThreadDef_t task_fal_batteryControlBlock;
const osThreadAttr_t task_fal_battery_attributes = {
  .name = "task_fal_battery",
  .cb_mem = &task_fal_batteryControlBlock,
  .cb_size = sizeof(task_fal_batteryControlBlock),
  .stack_mem = &task_fal_batteryBuffer[0],
  .stack_size = sizeof(task_fal_batteryBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_power */
osThreadId_t task_fal_powerHandle;
uint32_t task_fal_powerBuffer[ 256 ];
osStaticThreadDef_t task_fal_powerControlBlock;
const osThreadAttr_t task_fal_power_attributes = {
  .name = "task_fal_power",
  .cb_mem = &task_fal_powerControlBlock,
  .cb_size = sizeof(task_fal_powerControlBlock),
  .stack_mem = &task_fal_powerBuffer[0],
  .stack_size = sizeof(task_fal_powerBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_pubsub */
osThreadId_t task_fal_pubsubHandle;
uint32_t task_fal_pubsubBuffer[ 256 ];
osStaticThreadDef_t task_fal_pubsubControlBlock;
const osThreadAttr_t task_fal_pubsub_attributes = {
  .name = "task_fal_pubsub",
  .cb_mem = &task_fal_pubsubControlBlock,
  .cb_size = sizeof(task_fal_pubsubControlBlock),
  .stack_mem = &task_fal_pubsubBuffer[0],
  .stack_size = sizeof(task_fal_pubsubBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for task_fal_display */
osThreadId_t task_fal_displayHandle;
uint32_t task_fal_displayBuffer[ 256 ];
osStaticThreadDef_t task_fal_displayControlBlock;
const osThreadAttr_t task_fal_display_attributes = {
  .name = "task_fal_display",
  .cb_mem = &task_fal_displayControlBlock,
  .cb_size = sizeof(task_fal_displayControlBlock),
  .stack_mem = &task_fal_displayBuffer[0],
  .stack_size = sizeof(task_fal_displayBuffer),
  .priority = (osPriority_t) osPriorityNormal5,
};
/* Definitions for task_pal_modbus */
osThreadId_t task_pal_modbusHandle;
uint32_t task_pal_modbusBuffer[ 512 ];
osStaticThreadDef_t task_pal_modbusControlBlock;
const osThreadAttr_t task_pal_modbus_attributes = {
  .name = "task_pal_modbus",
  .cb_mem = &task_pal_modbusControlBlock,
  .cb_size = sizeof(task_pal_modbusControlBlock),
  .stack_mem = &task_pal_modbusBuffer[0],
  .stack_size = sizeof(task_pal_modbusBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_security_mng */
osThreadId_t task_fal_security_mngHandle;
uint32_t task_fal_security_mngBuffer[ 256 ];
osStaticThreadDef_t task_fal_security_mngControlBlock;
const osThreadAttr_t task_fal_security_mng_attributes = {
  .name = "task_fal_security_mng",
  .cb_mem = &task_fal_security_mngControlBlock,
  .cb_size = sizeof(task_fal_security_mngControlBlock),
  .stack_mem = &task_fal_security_mngBuffer[0],
  .stack_size = sizeof(task_fal_security_mngBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_imu */
osThreadId_t task_fal_imuHandle;
uint32_t task_fal_imuBuffer[ 256 ];
osStaticThreadDef_t task_fal_imuControlBlock;
const osThreadAttr_t task_fal_imu_attributes = {
  .name = "task_fal_imu",
  .cb_mem = &task_fal_imuControlBlock,
  .cb_size = sizeof(task_fal_imuControlBlock),
  .stack_mem = &task_fal_imuBuffer[0],
  .stack_size = sizeof(task_fal_imuBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_motor */
osThreadId_t task_fal_motorHandle;
uint32_t task_fal_motorBuffer[ 256 ];
osStaticThreadDef_t task_fal_motorControlBlock;
const osThreadAttr_t task_fal_motor_attributes = {
  .name = "task_fal_motor",
  .cb_mem = &task_fal_motorControlBlock,
  .cb_size = sizeof(task_fal_motorControlBlock),
  .stack_mem = &task_fal_motorBuffer[0],
  .stack_size = sizeof(task_fal_motorBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_misc */
osThreadId_t task_fal_miscHandle;
uint32_t task_fal_miscBuffer[ 256 ];
osStaticThreadDef_t task_fal_miscControlBlock;
const osThreadAttr_t task_fal_misc_attributes = {
  .name = "task_fal_misc",
  .cb_mem = &task_fal_miscControlBlock,
  .cb_size = sizeof(task_fal_miscControlBlock),
  .stack_mem = &task_fal_miscBuffer[0],
  .stack_size = sizeof(task_fal_miscBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_cliff */
osThreadId_t task_fal_cliffHandle;
uint32_t task_fal_cliffBuffer[ 256 ];
osStaticThreadDef_t task_fal_cliffControlBlock;
const osThreadAttr_t task_fal_cliff_attributes = {
  .name = "task_fal_cliff",
  .cb_mem = &task_fal_cliffControlBlock,
  .cb_size = sizeof(task_fal_cliffControlBlock),
  .stack_mem = &task_fal_cliffBuffer[0],
  .stack_size = sizeof(task_fal_cliffBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_usound */
osThreadId_t task_fal_usoundHandle;
uint32_t task_fal_usoundBuffer[ 256 ];
osStaticThreadDef_t task_fal_usoundControlBlock;
const osThreadAttr_t task_fal_usound_attributes = {
  .name = "task_fal_usound",
  .cb_mem = &task_fal_usoundControlBlock,
  .cb_size = sizeof(task_fal_usoundControlBlock),
  .stack_mem = &task_fal_usoundBuffer[0],
  .stack_size = sizeof(task_fal_usoundBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_fal_charge */
osThreadId_t task_fal_chargeHandle;
uint32_t task_fal_chargeBuffer[ 256 ];
osStaticThreadDef_t task_fal_chargeControlBlock;
const osThreadAttr_t task_fal_charge_attributes = {
  .name = "task_fal_charge",
  .cb_mem = &task_fal_chargeControlBlock,
  .cb_size = sizeof(task_fal_chargeControlBlock),
  .stack_mem = &task_fal_chargeBuffer[0],
  .stack_size = sizeof(task_fal_chargeBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for mutex_test */
osMutexId_t mutex_testHandle;
const osMutexAttr_t mutex_test_attributes = {
  .name = "mutex_test"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task_default_run(void *argument);
void task_fal_shell_run(void *argument);
void task_fal_log_run(void *argument);
void task_fal_update_run(void *argument);
void task_fal_button_run(void *argument);
void task_fal_led_run(void *argument);
void task_fal_state_run(void *argument);
void task_fal_battery_run(void *argument);
void task_fal_power_run(void *argument);
void task_fal_pubsub_run(void *argument);
void task_fal_display_run(void *argument);
void task_pal_modbus_run(void *argument);
void task_fal_security_mng_run(void *argument);
void task_fal_imu_run(void *argument);
void task_fal_motor_run(void *argument);
void task_fal_misc_run(void *argument);
void task_fal_cliff_run(void *argument);
void task_fal_usound_run(void *argument);
void task_fal_charge_run(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  system_init();
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of mutex_test */
  mutex_testHandle = osMutexNew(&mutex_test_attributes);	//作用是什么？貌似没用到

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of task_default */
  task_defaultHandle = osThreadNew(task_default_run, NULL, &task_default_attributes);

  /* creation of task_fal_shell */
  task_fal_shellHandle = osThreadNew(task_fal_shell_run, NULL, &task_fal_shell_attributes);

  /* creation of task_fal_log */
  task_fal_logHandle = osThreadNew(task_fal_log_run, NULL, &task_fal_log_attributes);

  /* creation of task_fal_update */
  //task_fal_updateHandle = osThreadNew(task_fal_update_run, NULL, &task_fal_update_attributes);

  /* creation of task_fal_button */
  task_fal_buttonHandle = osThreadNew(task_fal_button_run, NULL, &task_fal_button_attributes);

  /* creation of task_fal_led */
  task_fal_ledHandle = osThreadNew(task_fal_led_run, NULL, &task_fal_led_attributes);

  /* creation of task_fal_state */
  //task_fal_stateHandle = osThreadNew(task_fal_state_run, NULL, &task_fal_state_attributes);

  /* creation of task_fal_battery */
  task_fal_batteryHandle = osThreadNew(task_fal_battery_run, NULL, &task_fal_battery_attributes);

  /* creation of task_fal_power */
  task_fal_powerHandle = osThreadNew(task_fal_power_run, NULL, &task_fal_power_attributes);

  /* creation of task_fal_pubsub */
  task_fal_pubsubHandle = osThreadNew(task_fal_pubsub_run, NULL, &task_fal_pubsub_attributes);

  /* creation of task_fal_display */
  task_fal_displayHandle = osThreadNew(task_fal_display_run, NULL, &task_fal_display_attributes);

  /* creation of task_pal_modbus */
  //task_pal_modbusHandle = osThreadNew(task_pal_modbus_run, NULL, &task_pal_modbus_attributes);

  /* creation of task_fal_security_mng */
  //task_fal_security_mngHandle = osThreadNew(task_fal_security_mng_run, NULL, &task_fal_security_mng_attributes);

  /* creation of task_fal_imu */
  task_fal_imuHandle = osThreadNew(task_fal_imu_run, NULL, &task_fal_imu_attributes);

  /* creation of task_fal_motor */
  task_fal_motorHandle = osThreadNew(task_fal_motor_run, NULL, &task_fal_motor_attributes);

  /* creation of task_fal_misc */
  task_fal_miscHandle = osThreadNew(task_fal_misc_run, NULL, &task_fal_misc_attributes);

  /* creation of task_fal_cliff */
  task_fal_cliffHandle = osThreadNew(task_fal_cliff_run, NULL, &task_fal_cliff_attributes);

  /* creation of task_fal_usound */
  //task_fal_usoundHandle = osThreadNew(task_fal_usound_run, NULL, &task_fal_usound_attributes);

  /* creation of task_fal_charge */
  task_fal_chargeHandle = osThreadNew(task_fal_charge_run, NULL, &task_fal_charge_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_task_default_run */
/**
  * @brief  Function implementing the task_default thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_task_default_run */
__weak void task_default_run(void *argument)
{
  /* USER CODE BEGIN task_default_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_default_run */
}

/* USER CODE BEGIN Header_task_fal_shell_run */
/**
* @brief Function implementing the task_fal_shell thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_shell_run */
__weak void task_fal_shell_run(void *argument)
{
  /* USER CODE BEGIN task_fal_shell_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_shell_run */
}

/* USER CODE BEGIN Header_task_fal_log_run */
/**
* @brief Function implementing the task_fal_log thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_log_run */
__weak void task_fal_log_run(void *argument)
{
  /* USER CODE BEGIN task_fal_log_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_log_run */
}

/* USER CODE BEGIN Header_task_fal_update_run */
/**
* @brief Function implementing the task_fal_update thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_update_run */
__weak void task_fal_update_run(void *argument)
{
  /* USER CODE BEGIN task_fal_update_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_update_run */
}

/* USER CODE BEGIN Header_task_fal_button_run */
/**
* @brief Function implementing the task_fal_button thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_button_run */
__weak void task_fal_button_run(void *argument)
{
  /* USER CODE BEGIN task_fal_button_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_button_run */
}

/* USER CODE BEGIN Header_task_fal_led_run */
/**
* @brief Function implementing the task_fal_led thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_led_run */
__weak void task_fal_led_run(void *argument)
{
  /* USER CODE BEGIN task_fal_led_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_led_run */
}

/* USER CODE BEGIN Header_task_fal_state_run */
/**
* @brief Function implementing the task_fal_state thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_state_run */
__weak void task_fal_state_run(void *argument)
{
  /* USER CODE BEGIN task_fal_state_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_state_run */
}

/* USER CODE BEGIN Header_task_fal_battery_run */
/**
* @brief Function implementing the task_fal_battery thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_battery_run */
__weak void task_fal_battery_run(void *argument)
{
  /* USER CODE BEGIN task_fal_battery_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_battery_run */
}

/* USER CODE BEGIN Header_task_fal_power_run */
/**
* @brief Function implementing the task_fal_power thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_power_run */
__weak void task_fal_power_run(void *argument)
{
  /* USER CODE BEGIN task_fal_power_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_power_run */
}

/* USER CODE BEGIN Header_task_fal_pubsub_run */
/**
* @brief Function implementing the task_fal_pubsub thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_pubsub_run */
__weak void task_fal_pubsub_run(void *argument)
{
  /* USER CODE BEGIN task_fal_pubsub_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_pubsub_run */
}

/* USER CODE BEGIN Header_task_fal_display_run */
/**
* @brief Function implementing the task_fal_display thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_display_run */
__weak void task_fal_display_run(void *argument)
{
  /* USER CODE BEGIN task_fal_display_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_display_run */
}

/* USER CODE BEGIN Header_task_pal_modbus_run */
/**
* @brief Function implementing the task_pal_modbus thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_pal_modbus_run */
__weak void task_pal_modbus_run(void *argument)
{
  /* USER CODE BEGIN task_pal_modbus_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_pal_modbus_run */
}

/* USER CODE BEGIN Header_task_fal_security_mng_run */
/**
* @brief Function implementing the task_fal_security_mng thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_security_mng_run */
__weak void task_fal_security_mng_run(void *argument)
{
  /* USER CODE BEGIN task_fal_security_mng_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_security_mng_run */
}

/* USER CODE BEGIN Header_task_fal_imu_run */
/**
* @brief Function implementing the task_fal_imu thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_imu_run */
__weak void task_fal_imu_run(void *argument)
{
  /* USER CODE BEGIN task_fal_imu_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_imu_run */
}

/* USER CODE BEGIN Header_task_fal_motor_run */
/**
* @brief Function implementing the task_fal_motor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_motor_run */
__weak void task_fal_motor_run(void *argument)
{
  /* USER CODE BEGIN task_fal_motor_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_motor_run */
}

/* USER CODE BEGIN Header_task_fal_misc_run */
/**
* @brief Function implementing the task_fal_misc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_misc_run */
__weak void task_fal_misc_run(void *argument)
{
  /* USER CODE BEGIN task_fal_misc_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_misc_run */
}

/* USER CODE BEGIN Header_task_fal_cliff_run */
/**
* @brief Function implementing the task_fal_cliff thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_cliff_run */
__weak void task_fal_cliff_run(void *argument)
{
  /* USER CODE BEGIN task_fal_cliff_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_cliff_run */
}

/* USER CODE BEGIN Header_task_fal_usound_run */
/**
* @brief Function implementing the task_fal_usound thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_usound_run */
__weak void task_fal_usound_run(void *argument)
{
  /* USER CODE BEGIN task_fal_usound_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_usound_run */
}

/* USER CODE BEGIN Header_task_fal_charge_run */
/**
* @brief Function implementing the task_fal_charge thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_fal_charge_run */
__weak void task_fal_charge_run(void *argument)
{
  /* USER CODE BEGIN task_fal_charge_run */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_fal_charge_run */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
