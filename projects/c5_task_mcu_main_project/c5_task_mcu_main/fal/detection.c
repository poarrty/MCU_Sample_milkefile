#include "detection.h"
#include "button.h"
#include "bsp_gpio_in.h"
//#include "rtthread.h"
#include "FreeRTOS.h"
//#include "finsh.h"
#include "bsp_clean_ctrl.h"
//#include "uorb.h"
#include "clean_def.h"
// #include "orb_def.h"
#include "bsp_gpio_out.h"
#include "main.h"

#define LOG_TAG "detection"
#include "elog.h"

#define KEEP_TIME_MS               4500
#define FILTER_PUMP_DET_DELAY_TIME 1000
#define DET_THREAD_YIELD_TIME      5

button_t detection_device[ID_DET_DEFAULT_MAX];
uint8_t  clean_dev_err_status[ID_DET_DEFAULT_MAX] = {0};

uint32_t clean_device_error_status_get(uint8_t clean_dev_id) {
    if (clean_dev_id < ID_DET_DEFAULT_MAX) {
        // log_d("cdes get :%d, %d", clean_dev_id,
        // clean_dev_err_status[clean_dev_id]);
        return clean_dev_err_status[clean_dev_id];
    }

    return 0;
}

uint32_t clean_device_error_status_clear(uint8_t clean_dev_id, uint32_t value) {
    // rt_enter_critical();
    portENTER_CRITICAL();

    if (value) {
        clean_dev_err_status[clean_dev_id] = 0;
    }

    // rt_exit_critical();
    portENTER_CRITICAL();
    return RT_EOK;
}

static int filter_pump_det_status_read(void) {
    return gpio_in_status_read(ID_FILTER_PUMP_OC);
}

static void filter_pump_det_press_handler(void *param) {
    //�쳣
    clean_dev_err_status[ID_FILTER_PUMP_DET] = RT_ERROR;
    filter_pump_ops(FILTER_PUMP_NUM, DET_DISABLE);
    log_e("fp oc error");
}

static int sewage_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_SEWAGE_WATER_VALVE_OC);
}

static void sewage_water_valve_det_press_handler(void *param) {
    //�쳣
    clean_dev_err_status[ID_SEWAGE_WATER_VALVE_DET] = RT_ERROR;
    water_valve_ops(SEWAGE_WATER_VALVE_NUM, DET_DISABLE);
    log_e("swv oc error");
}

static int clean_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_CLEAN_WATER_VALVE_OC);
}

static void clean_water_valve_det_press_handler(void *param) {
    //�쳣
    clean_dev_err_status[ID_CLEAN_WATER_VALVE_DET] = RT_ERROR;
    water_valve_ops(CLEAN_WATER_VALVE_NUM, DET_DISABLE);
    log_e("cwv oc error");
}

static int waste_water_valve_det_status_read(void) {
    return gpio_in_status_read(ID_WASTE_WATER_VALVE_OC);
}

static void waste_water_valve_det_press_handler(void *param) {
    //�쳣
    clean_dev_err_status[ID_WASTE_WATER_VALVE_DET] = 1;
    water_valve_ops(WASTE_WATER_VALVE_NUM, DET_DISABLE);
    log_e("wwv oc error");
}

static void filter_pump_det_init(void) {
    button_create("pump det", &detection_device[ID_FILTER_PUMP_DET], filter_pump_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_FILTER_PUMP_DET], BUTTON_LONG, filter_pump_det_press_handler);
}

static void filter_pump_det_deinit(void) {
    button_delete(&detection_device[ID_FILTER_PUMP_DET]);
    rt_memset(&detection_device[ID_FILTER_PUMP_DET], 0, sizeof(button_t));
}

static void sewage_water_valve_det_init(void) {
    button_create("sw water", &detection_device[ID_SEWAGE_WATER_VALVE_DET], sewage_water_valve_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_SEWAGE_WATER_VALVE_DET], BUTTON_LONG, sewage_water_valve_det_press_handler);
}

static void sewage_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_SEWAGE_WATER_VALVE_DET]);
    rt_memset(&detection_device[ID_SEWAGE_WATER_VALVE_DET], 0, sizeof(button_t));
}

static void clean_water_valve_det_init(void) {
    button_create("clean water", &detection_device[ID_CLEAN_WATER_VALVE_DET], clean_water_valve_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_CLEAN_WATER_VALVE_DET], BUTTON_LONG, clean_water_valve_det_press_handler);
}

static void clean_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_CLEAN_WATER_VALVE_DET]);
    rt_memset(&detection_device[ID_CLEAN_WATER_VALVE_DET], 0, sizeof(button_t));
}

static void waste_water_valve_det_init(void) {
    button_create("waste water", &detection_device[ID_WASTE_WATER_VALVE_DET], waste_water_valve_det_status_read, PIN_LOW);
    button_attach(&detection_device[ID_WASTE_WATER_VALVE_DET], BUTTON_LONG, waste_water_valve_det_press_handler);
}

static void waste_water_valve_det_deinit(void) {
    button_delete(&detection_device[ID_WASTE_WATER_VALVE_DET]);
    rt_memset(&detection_device[ID_WASTE_WATER_VALVE_DET], 0, sizeof(button_t));
}

// #ifdef RT_USING_FINSH
// #include "finsh.h"
// void detection_device_info(int argc, char **argv)
// {
//     LOG_D("fp sta :%d\r\n", clean_dev_err_status[ID_FILTER_PUMP_DET]);
//     lod_d("swv sta :%d\r\n",
//     clean_dev_err_status[ID_SEWAGE_WATER_VALVE_DET]); lod_d("cwv sta
//     :%d\r\n", clean_dev_err_status[ID_CLEAN_WATER_VALVE_DET]); lod_d("wwv sta
//     :%d\r\n", clean_dev_err_status[ID_WASTE_WATER_VALVE_DET]);
// }
// MSH_CMD_EXPORT_ALIAS(detection_device_info, det_info, show water level info);
// #endif

void detection_device_thread_entry(void *param) {
#ifdef USED_FILTER_PUMP_DETECTION
    uint8_t              filter_pump_det_enable = DET_DISABLE;
    uint8_t              start_once             = pdFALSE;
    uint32_t             filter_pump_sub        = 0;
    uint32_t             filter_pump_det_count  = 0;
    clean_module_ctrl_st filter_pump_data;
    // if (orb_subscribe_auto(ORB_ID(filter_pump), &filter_pump_sub, 10) !=
    // RT_EOK) //���Ĺ���ˮ������
    // {
    //     LOG_E("sub fp failed");
    // }
#endif

#ifdef USED_SEWAGE_WATER_DETECTION
    uint8_t              sewage_water_valve_det_enable = DET_DISABLE;
    uint32_t             sewage_water_valve_start_time = 0;
    uint32_t             sewage_water_valve_sub        = 0;
    clean_module_ctrl_st sewage_water_valve_data;
    // if (orb_subscribe_auto(ORB_ID(sewage_water_valve),
    // &sewage_water_valve_sub, 10) != RT_EOK)//��ˮ����
    // {
    //     LOG_E("sub swv failed");
    // }
#endif

#ifdef USED_CLEAN_WATER_DETECTION
    uint8_t              clean_water_valve_det_enable = DET_DISABLE;
    uint32_t             clean_water_valve_sub        = 0;
    clean_module_ctrl_st clean_water_valve_data;
    // if (orb_subscribe_auto(ORB_ID(clean_water_valve), &clean_water_valve_sub,
    // 10) != RT_EOK)
    // {
    //     LOG_E("sub cwv failed");
    // }
#endif

#ifdef USED_WASTE_WATER_DETECTION
    uint8_t              waste_water_valve_det_enable = DET_DISABLE;
    uint32_t             waste_water_valve_sub        = 0;
    clean_module_ctrl_st waste_water_valve_data;
    // if (orb_subscribe_auto(ORB_ID(waste_water_valve), &waste_water_valve_sub,
    // 10) != RT_EOK)
    // {
    //     LOG_E("sub wwv failed");
    // }
#endif

    while (1) {
        /* �ȵȴ��������������ȳ�ʼ��ִ�� */
        // rt_thread_delay(DET_THREAD_YIELD_TIME);
        osDelay(DET_THREAD_YIELD_TIME);
        // printf("detection task\r\n");
#ifdef USED_FILTER_PUMP_DETECTION
        // if (orb_check(&filter_pump_sub, 0) == RT_EOK)
        // {
        //     orb_copy(ORB_ID(filter_pump), &filter_pump_data);

        //     if(filter_pump_data.set_value)
        //     {
        //         if(filter_pump_det_enable != DET_ENABLE)
        //         {
        //             filter_pump_det_count = 0;
        //             start_once = RT_TRUE;
        //             filter_pump_det_enable = DET_ENABLE;
        //         }
        //     }
        //     else
        //     {
        //         if(filter_pump_det_enable != DET_DISABLE)
        //         {
        //             start_once = RT_FALSE;
        //             filter_pump_det_enable = DET_DISABLE;
        //             filter_pump_det_deinit();
        //         }
        //     }
        // }
        // if (filter_pump_det_enable == DET_ENABLE && (++filter_pump_det_count
        // >= FILTER_PUMP_DET_DELAY_TIME / DET_THREAD_YIELD_TIME))
        // {
        //  filter_pump_det_count = 0;

        //  if (start_once == pdTRUE)
        //  {
        //      start_once = pdFALSE;
        //      filter_pump_det_init();
        //  }
        // }

#endif

#ifdef USED_SEWAGE_WATER_DETECTION
        // if (orb_check(&sewage_water_valve_sub, 0) == RT_EOK)
        // {
        //     orb_copy(ORB_ID(sewage_water_valve), &sewage_water_valve_data);
        //     if (sewage_water_valve_data.set_value)
        //     {
        //         if (sewage_water_valve_det_enable != DET_ENABLE)
        //         {
        //             sewage_water_valve_det_enable = DET_ENABLE;
        //             sewage_water_valve_start_time = rt_tick_get();
        //             sewage_water_valve_det_init();
        //         }
        //     }
        //     else
        //     {
        //         if (sewage_water_valve_det_enable != DET_DISABLE)
        //         {
        //             sewage_water_valve_det_enable = DET_DISABLE;
        //             sewage_water_valve_det_deinit();
        //         }
        //     }
        // }

        // if (sewage_water_valve_det_enable == DET_ENABLE)
        // {
        //  uint64_t time_int = rt_tick_get();
        //  time_int = (time_int + 0xffffffff - sewage_water_valve_start_time) %
        //  0xffffffff;

        //  if (time_int >= KEEP_TIME_MS)
        //  {
        //      sewage_water_valve_det_enable = DET_DISABLE;
        //      sewage_water_valve_det_deinit();
        //  }
        // }

#endif

#ifdef USED_CLEAN_WATER_DETECTION
        // if (orb_check(&clean_water_valve_sub, 0) == RT_EOK)
        // {
        //     orb_copy(ORB_ID(clean_water_valve), &clean_water_valve_data);
        //     if (clean_water_valve_data.set_value)
        //     {
        //         if (clean_water_valve_det_enable != DET_ENABLE)
        //         {
        //             clean_water_valve_det_enable = DET_ENABLE;
        //             clean_water_valve_det_init();
        //         }
        //     }
        //     else
        //     {
        //         if (clean_water_valve_det_enable != DET_DISABLE)
        //         {
        //             clean_water_valve_det_enable = DET_DISABLE;
        //             clean_water_valve_det_deinit();
        //         }
        //     }
        // }
#endif

#ifdef USED_WASTE_WATER_DETECTION
        // if (orb_check(&waste_water_valve_sub, 0) == RT_EOK)
        // {
        //     orb_copy(ORB_ID(waste_water_valve), &waste_water_valve_data);
        //     if (waste_water_valve_data.set_value)
        //     {
        //         if (waste_water_valve_det_enable != DET_ENABLE)
        //         {
        //             waste_water_valve_det_enable = DET_ENABLE;
        //             waste_water_valve_det_init();
        //         }
        //     }
        //     else
        //     {
        //         if (waste_water_valve_det_enable != DET_DISABLE)
        //         {
        //             waste_water_valve_det_enable = DET_DISABLE;
        //             waste_water_valve_det_deinit();
        //         }
        //     }
        // }
#endif
    }
}

// #ifdef USED_CLEAN_DETECTION
// static int detection_device_thread_init(void)
// {
//     rt_thread_t tid = RT_NULL;
//     tid = rt_thread_create("det thread", detection_device_thread_entry,
//     RT_NULL, 1024, 10, 10); if (tid == RT_NULL)
//     {
//         LOG_E("detection creat error");
//         return RT_ERROR;
//     }
//     rt_thread_startup(tid);
//     return RT_EOK;
// }
// INIT_APP_EXPORT(detection_device_thread_init);
// #endif
