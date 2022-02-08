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
#include "board.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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
/* Definitions for defaultTask */
osThreadId_t         defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 1536 * 4,
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for task_shell */
osThreadId_t         task_shellHandle;
const osThreadAttr_t task_shell_attributes = {
    .name       = "task_shell",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for device_run */
osThreadId_t         device_runHandle;
const osThreadAttr_t device_run_attributes = {
    .name       = "device_run",
    .stack_size = 512 * 4,
    .priority   = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for det_thread */
osThreadId_t         det_threadHandle;
const osThreadAttr_t det_thread_attributes = {
    .name       = "det_thread",
    .stack_size = 512 * 4,
    .priority   = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for can_rx */
osThreadId_t         can_rxHandle;
const osThreadAttr_t can_rx_attributes = {
    .name       = "can_rx",
    .stack_size = 512 * 4,
    .priority   = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for fan_motor_speed__set */
osThreadId_t         fan_motor_speed__setHandle;
const osThreadAttr_t fan_motor_speed__set_attributes = {
    .name       = "fan_motor_speed__set",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for easylog_thread */
osThreadId_t         easylog_threadHandle;
const osThreadAttr_t easylog_thread_attributes = {
    .name       = "easylog_thread",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for task_fal_pubsub */
osThreadId_t         task_fal_pubsubHandle;
const osThreadAttr_t task_fal_pubsub_attributes = {
    .name       = "task_fal_pubsub",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t) osPriorityHigh,
};
/* Definitions for pal_ctl */
osThreadId_t         pal_ctlHandle;
const osThreadAttr_t pal_ctl_attributes = {
    .name       = "pal_ctl",
    .stack_size = 1024 * 4,
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for topic_sunction_motor */
osMessageQueueId_t         topic_sunction_motorHandle;
const osMessageQueueAttr_t topic_sunction_motor_attributes = {.name = "topic_sunction_motor"};
/* Definitions for topic_roll_brush_motor */
osMessageQueueId_t         topic_roll_brush_motorHandle;
const osMessageQueueAttr_t topic_roll_brush_motor_attributes = {.name = "topic_roll_brush_motor"};
/* Definitions for topic_water_dist_pump */
osMessageQueueId_t         topic_water_dist_pumpHandle;
const osMessageQueueAttr_t topic_water_dist_pump_attributes = {.name = "topic_water_dist_pump"};
/* Definitions for topic_side_brush_motor */
osMessageQueueId_t         topic_side_brush_motorHandle;
const osMessageQueueAttr_t topic_side_brush_motor_attributes = {.name = "topic_side_brush_motor"};
/* Definitions for topic_push_rod_motor_1 */
osMessageQueueId_t         topic_push_rod_motor_1Handle;
const osMessageQueueAttr_t topic_push_rod_motor_1_attributes = {.name = "topic_push_rod_motor_1"};
/* Definitions for topic_push_rod_motor_2 */
osMessageQueueId_t         topic_push_rod_motor_2Handle;
const osMessageQueueAttr_t topic_push_rod_motor_2_attributes = {.name = "topic_push_rod_motor_2"};
/* Definitions for topic_fan_motor_speed_set */
osMessageQueueId_t         topic_fan_motor_speed_setHandle;
const osMessageQueueAttr_t topic_fan_motor_speed_set_attributes = {.name = "topic_fan_motor_speed_set"};
/* Definitions for topic_fan_motor_speed_fbk */
osMessageQueueId_t         topic_fan_motor_speed_fbkHandle;
const osMessageQueueAttr_t topic_fan_motor_speed_fbk_attributes = {.name = "topic_fan_motor_speed_fbk"};
/* Definitions for topic_fan_motor_error_code */
osMessageQueueId_t         topic_fan_motor_error_codeHandle;
const osMessageQueueAttr_t topic_fan_motor_error_code_attributes = {.name = "topic_fan_motor_error_code"};
/* Definitions for topic_update_file_recv */
osMessageQueueId_t         topic_update_file_recvHandle;
const osMessageQueueAttr_t topic_update_file_recv_attributes = {.name = "topic_update_file_recv"};
/* Definitions for topic_update_enter_boot */
osMessageQueueId_t         topic_update_enter_bootHandle;
const osMessageQueueAttr_t topic_update_enter_boot_attributes = {.name = "topic_update_enter_boot"};
/* Definitions for topic_water_valve_clean */
osMessageQueueId_t         topic_water_valve_cleanHandle;
const osMessageQueueAttr_t topic_water_valve_clean_attributes = {.name = "topic_water_valve_clean"};
/* Definitions for sys_suber_sunction_motor */
osEventFlagsId_t         sys_suber_sunction_motorHandle;
const osEventFlagsAttr_t sys_suber_sunction_motor_attributes = {.name = "sys_suber_sunction_motor"};
/* Definitions for sys_suber_fan_motor */
osEventFlagsId_t         sys_suber_fan_motorHandle;
const osEventFlagsAttr_t sys_suber_fan_motor_attributes = {.name = "sys_suber_fan_motor"};
/* Definitions for sys_suber_detection */
osEventFlagsId_t         sys_suber_detectionHandle;
const osEventFlagsAttr_t sys_suber_detection_attributes = {.name = "sys_suber_detection"};
/* Definitions for sys_suber_clean_ctrl */
osEventFlagsId_t         sys_suber_clean_ctrlHandle;
const osEventFlagsAttr_t sys_suber_clean_ctrl_attributes = {.name = "sys_suber_clean_ctrl"};
/* Definitions for sys_suber_roll_motor */
osEventFlagsId_t         sys_suber_roll_motorHandle;
const osEventFlagsAttr_t sys_suber_roll_motor_attributes = {.name = "sys_suber_roll_motor"};
/* Definitions for sys_suber_test */
osEventFlagsId_t         sys_suber_testHandle;
const osEventFlagsAttr_t sys_suber_test_attributes = {.name = "sys_suber_test"};
/* Definitions for sys_suber_modbus_ota */
osEventFlagsId_t         sys_suber_modbus_otaHandle;
const osEventFlagsAttr_t sys_suber_modbus_ota_attributes = {.name = "sys_suber_modbus_ota"};
/* Definitions for sys_pub */
osEventFlagsId_t         sys_pubHandle;
const osEventFlagsAttr_t sys_pub_attributes = {.name = "sys_pub"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void task_shell_run(void *argument);
void device_run_thread_entry(void *argument);
void detection_device_thread_entry(void *argument);
void can_rx_thread(void *argument);
void fan_motor_thread(void *argument);
void task_fal_log_run(void *argument);
void task_fal_pubsub_run(void *argument);
void pal_ctl_thread(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void          configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void) {}

__weak unsigned long getRunTimeCounterValue(void) {
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

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of topic_sunction_motor */
    topic_sunction_motorHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_sunction_motor_attributes);

    /* creation of topic_roll_brush_motor */
    topic_roll_brush_motorHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_roll_brush_motor_attributes);

    /* creation of topic_water_dist_pump */
    topic_water_dist_pumpHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_water_dist_pump_attributes);

    /* creation of topic_side_brush_motor */
    topic_side_brush_motorHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_side_brush_motor_attributes);

    /* creation of topic_push_rod_motor_1 */
    topic_push_rod_motor_1Handle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_push_rod_motor_1_attributes);

    /* creation of topic_push_rod_motor_2 */
    topic_push_rod_motor_2Handle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_push_rod_motor_2_attributes);

    /* creation of topic_fan_motor_speed_set */
    topic_fan_motor_speed_setHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_fan_motor_speed_set_attributes);

    /* creation of topic_fan_motor_speed_fbk */
    topic_fan_motor_speed_fbkHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_fan_motor_speed_fbk_attributes);

    /* creation of topic_fan_motor_error_code */
    topic_fan_motor_error_codeHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_fan_motor_error_code_attributes);

    /* creation of topic_update_file_recv */
    topic_update_file_recvHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_update_file_recv_attributes);

    /* creation of topic_update_enter_boot */
    topic_update_enter_bootHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_update_enter_boot_attributes);

    /* creation of topic_water_valve_clean */
    topic_water_valve_cleanHandle = osMessageQueueNew(1, sizeof(clean_module_ctrl_st), &topic_water_valve_clean_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of task_shell */
    task_shellHandle = osThreadNew(task_shell_run, NULL, &task_shell_attributes);

    /* creation of device_run */
    device_runHandle = osThreadNew(device_run_thread_entry, NULL, &device_run_attributes);

    /* creation of det_thread */
    det_threadHandle = osThreadNew(detection_device_thread_entry, NULL, &det_thread_attributes);

    /* creation of can_rx */
    can_rxHandle = osThreadNew(can_rx_thread, NULL, &can_rx_attributes);

    /* creation of fan_motor_speed__set */
    fan_motor_speed__setHandle = osThreadNew(fan_motor_thread, NULL, &fan_motor_speed__set_attributes);

    /* creation of easylog_thread */
    easylog_threadHandle = osThreadNew(task_fal_log_run, NULL, &easylog_thread_attributes);

    /* creation of task_fal_pubsub */
    task_fal_pubsubHandle = osThreadNew(task_fal_pubsub_run, NULL, &task_fal_pubsub_attributes);

    /* creation of pal_ctl */
    pal_ctlHandle = osThreadNew(pal_ctl_thread, NULL, &pal_ctl_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* Create the event(s) */
    /* creation of sys_suber_sunction_motor */
    sys_suber_sunction_motorHandle = osEventFlagsNew(&sys_suber_sunction_motor_attributes);

    /* creation of sys_suber_fan_motor */
    sys_suber_fan_motorHandle = osEventFlagsNew(&sys_suber_fan_motor_attributes);

    /* creation of sys_suber_detection */
    sys_suber_detectionHandle = osEventFlagsNew(&sys_suber_detection_attributes);

    /* creation of sys_suber_clean_ctrl */
    sys_suber_clean_ctrlHandle = osEventFlagsNew(&sys_suber_clean_ctrl_attributes);

    /* creation of sys_suber_roll_motor */
    sys_suber_roll_motorHandle = osEventFlagsNew(&sys_suber_roll_motor_attributes);

    /* creation of sys_suber_test */
    sys_suber_testHandle = osEventFlagsNew(&sys_suber_test_attributes);

    /* creation of sys_suber_modbus_ota */
    sys_suber_modbus_otaHandle = osEventFlagsNew(&sys_suber_modbus_ota_attributes);

    /* creation of sys_pub */
    sys_pubHandle = osEventFlagsNew(&sys_pub_attributes);

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument) {
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_task_shell_run */
/**
 * @brief Function implementing the task_shell thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_shell_run */
__weak void task_shell_run(void *argument) {
    /* USER CODE BEGIN task_shell_run */
    /* Infinite loop */
    for (;;) {
        /* handle letter shell */
        letter_shell_recv_handler();
        osDelay(10);
    }
    /* USER CODE END task_shell_run */
}

/* USER CODE BEGIN Header_device_run_thread_entry */
/**
 * @brief Function implementing the device_run thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_device_run_thread_entry */
__weak void device_run_thread_entry(void *argument) {
    /* USER CODE BEGIN device_run_thread_entry */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END device_run_thread_entry */
}

/* USER CODE BEGIN Header_detection_device_thread_entry */
/**
 * @brief Function implementing the det_thread thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_detection_device_thread_entry */
__weak void detection_device_thread_entry(void *argument) {
    /* USER CODE BEGIN detection_device_thread_entry */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END detection_device_thread_entry */
}

/* USER CODE BEGIN Header_can_rx_thread */
/**
 * @brief Function implementing the can_rx thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_can_rx_thread */
__weak void can_rx_thread(void *argument) {
    /* USER CODE BEGIN can_rx_thread */
    /* Infinite loop */
    for (;;) {
        /* feed dog */
        iwdg_feed();
        osDelay(100);
    }
    /* USER CODE END can_rx_thread */
}

/* USER CODE BEGIN Header_fan_motor_thread */
/**
 * @brief Function implementing the fan_motor_speed__set thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_fan_motor_thread */
__weak void fan_motor_thread(void *argument) {
    /* USER CODE BEGIN fan_motor_thread */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END fan_motor_thread */
}

/* USER CODE BEGIN Header_task_fal_log_run */
/**
 * @brief Function implementing the easylog_thread thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_fal_log_run */
__weak void task_fal_log_run(void *argument) {
    /* USER CODE BEGIN task_fal_log_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_fal_log_run */
}

/* USER CODE BEGIN Header_task_fal_pubsub_run */
/**
 * @brief Function implementing the task_fal_pubsub thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_fal_pubsub_run */
__weak void task_fal_pubsub_run(void *argument) {
    /* USER CODE BEGIN task_fal_pubsub_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_fal_pubsub_run */
}

/* USER CODE BEGIN Header_pal_ctl_thread */
/**
 * @brief Function implementing the pal_ctl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_pal_ctl_thread */
__weak void pal_ctl_thread(void *argument) {
    /* USER CODE BEGIN pal_ctl_thread */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END pal_ctl_thread */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
