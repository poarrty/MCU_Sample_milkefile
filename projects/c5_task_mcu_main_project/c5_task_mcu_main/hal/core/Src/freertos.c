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
typedef StaticTask_t      osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
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
    .stack_size = 1024 * 4,
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for task_led */
osThreadId_t         task_ledHandle;
uint32_t             task_ledBuffer[256];
osStaticThreadDef_t  task_ledControlBlock;
const osThreadAttr_t task_led_attributes = {
    .name       = "task_led",
    .cb_mem     = &task_ledControlBlock,
    .cb_size    = sizeof(task_ledControlBlock),
    .stack_mem  = &task_ledBuffer[0],
    .stack_size = sizeof(task_ledBuffer),
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for task_shell */
osThreadId_t         task_shellHandle;
uint32_t             task_shellBuffer[512];
osStaticThreadDef_t  task_shellControlBlock;
const osThreadAttr_t task_shell_attributes = {
    .name       = "task_shell",
    .cb_mem     = &task_shellControlBlock,
    .cb_size    = sizeof(task_shellControlBlock),
    .stack_mem  = &task_shellBuffer[0],
    .stack_size = sizeof(task_shellBuffer),
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for task_key */
osThreadId_t         task_keyHandle;
uint32_t             task_keyBuffer[512];
osStaticThreadDef_t  task_keyControlBlock;
const osThreadAttr_t task_key_attributes = {
    .name       = "task_key",
    .cb_mem     = &task_keyControlBlock,
    .cb_size    = sizeof(task_keyControlBlock),
    .stack_mem  = &task_keyBuffer[0],
    .stack_size = sizeof(task_keyBuffer),
    .priority   = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for task_clean_ctrl */
osThreadId_t         task_clean_ctrlHandle;
uint32_t             task_clean_ctrlBuffer[512];
osStaticThreadDef_t  task_clean_ctrlControlBlock;
const osThreadAttr_t task_clean_ctrl_attributes = {
    .name       = "task_clean_ctrl",
    .cb_mem     = &task_clean_ctrlControlBlock,
    .cb_size    = sizeof(task_clean_ctrlControlBlock),
    .stack_mem  = &task_clean_ctrlBuffer[0],
    .stack_size = sizeof(task_clean_ctrlBuffer),
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for task_clean_manage */
osThreadId_t         task_clean_manageHandle;
uint32_t             task_clean_manageBuffer[1024];
osStaticThreadDef_t  task_clean_manageControlBlock;
const osThreadAttr_t task_clean_manage_attributes = {
    .name       = "task_clean_manage",
    .cb_mem     = &task_clean_manageControlBlock,
    .cb_size    = sizeof(task_clean_manageControlBlock),
    .stack_mem  = &task_clean_manageBuffer[0],
    .stack_size = sizeof(task_clean_manageBuffer),
    .priority   = (osPriority_t) osPriorityLow,
};
/* Definitions for mutex_i2c2 */
osMutexId_t         mutex_i2c2Handle;
osStaticMutexDef_t  mutex_i2c2ControlBlock;
const osMutexAttr_t mutex_i2c2_attributes = {
    .name    = "mutex_i2c2",
    .cb_mem  = &mutex_i2c2ControlBlock,
    .cb_size = sizeof(mutex_i2c2ControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* Definitions for task_workstation_control_run */
osThreadId_t         task_workstaion_control_runHandle;
uint32_t             task_workstation_control_runBuffer[1024];
osStaticThreadDef_t  task_workstaion_control_runControlBlock;
const osThreadAttr_t task_workstation_control_run_attributes = {
    .name       = "task_workstation_control_run",
    .cb_mem     = &task_workstaion_control_runControlBlock,
    .cb_size    = sizeof(task_workstaion_control_runControlBlock),
    .stack_mem  = &task_workstation_control_runBuffer[0],
    .stack_size = sizeof(task_workstation_control_runBuffer),
    .priority   = (osPriority_t) osPriorityBelowNormal,
};

/* Definitions for task_workstation_control_run */
osThreadId_t         task_log_runHandle;
uint32_t             task_log_runBuffer[256];
osStaticThreadDef_t  task_log_runControlBlock;
const osThreadAttr_t task_log_run_attributes = {
    .name       = "task_log_run",
    .cb_mem     = &task_log_runControlBlock,
    .cb_size    = sizeof(task_log_runControlBlock),
    .stack_mem  = &task_log_runBuffer[0],
    .stack_size = sizeof(task_log_runBuffer),
    .priority   = (osPriority_t) osPriorityBelowNormal,
};
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void task_led_run(void *argument);
void task_shell_run(void *argument);
void task_key_run(void *argument);
void task_clean_ctrl_det_run(void *argument);
void task_clean_manage_run(void *argument);
void task_fal_log_run(void *argument);

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

void task_workstation_control_run(void *argument);
/* USER CODE END 1 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */
    /* Create the mutex(es) */
    /* creation of mutex_i2c2 */
    mutex_i2c2Handle = osMutexNew(&mutex_i2c2_attributes);

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
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of task_led */
    task_ledHandle = osThreadNew(task_led_run, NULL, &task_led_attributes);

    /* creation of task_shell */
    task_shellHandle = osThreadNew(task_shell_run, NULL, &task_shell_attributes);

    /* creation of task_key */
    task_keyHandle = osThreadNew(task_key_run, NULL, &task_key_attributes);

    /* creation of task_clean_ctrl */
    task_clean_ctrlHandle = osThreadNew(task_clean_ctrl_det_run, NULL, &task_clean_ctrl_attributes);

    /* creation of task_clean_manage */
    task_clean_manageHandle = osThreadNew(task_clean_manage_run, NULL, &task_clean_manage_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* creation of task_workstation_control_run */
    task_workstaion_control_runHandle = osThreadNew(task_workstation_control_run, NULL, &task_workstation_control_run_attributes);

    /* creation of task_workstation_control_run */
    task_log_runHandle = osThreadNew(task_fal_log_run, NULL, &task_log_run_attributes);

    /* USER CODE END RTOS_THREADS */

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

/* USER CODE BEGIN Header_task_led_run */
/**
 * @brief Function implementing the task_led thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_led_run */
__weak void task_led_run(void *argument) {
    /* USER CODE BEGIN task_led_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_led_run */
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
        /* feed dog */
        // iwdg_feed();
        /* handle letter shell */
        extern void letter_shell_recv_handler();
        letter_shell_recv_handler();
        osDelay(10);
    }
    /* USER CODE END task_shell_run */
}

/* USER CODE BEGIN Header_task_key_run */
/**
 * @brief Function implementing the task_key thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_key_run */
__weak void task_key_run(void *argument) {
    /* USER CODE BEGIN task_key_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_key_run */
}

/* USER CODE BEGIN Header_task_clean_ctrl_det_run */
/**
 * @brief Function implementing the clean_ctrl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_clean_ctrl_det_run */
__weak void task_clean_ctrl_det_run(void *argument) {
    /* USER CODE BEGIN task_clean_ctrl_det_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_clean_ctrl_det_run */
}

/* USER CODE BEGIN Header_task_clean_manage_run */
/**
 * @brief Function implementing the task_clean_manage thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_task_clean_manage_run */
__weak void task_clean_manage_run(void *argument) {
    /* USER CODE BEGIN task_clean_manage_run */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END task_clean_manage_run */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
