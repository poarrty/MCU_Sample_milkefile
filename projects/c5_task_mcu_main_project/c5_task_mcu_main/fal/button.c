#include "button.h"
#include "fal_key.h"
#include "stdbool.h"
#include "string.h"
#include "cmsis_os.h"
#include "main.h"
#include "FreeRTOS.h"
#include "log.h"

#define LOG_TAG "button"
#include "elog.h"

/*******************************************************************
 *                          变量声明
 *******************************************************************/
static struct button *button_head = NULL;

/*******************************************************************
 *                         函数声明
 *******************************************************************/
// static void button_info_print(button_t *btn);
static void button_add(button_t *btn);

/************************************************************
 * @brief   按键创建
 * @param   name : 按键名称
 * @param   btn : 按键结构体
 * @param   read_btn_level :
 *按键电平读取函数，需要用户自己实现返回uint8_t类型的电平
 * @param   btn_trigger_level : 按键触发电平
 * @return  NULL
 ***********************************************************/
void button_create(const char *name, button_t *btn, int (*read_btn_level)(void), uint8_t btn_trigger_level) {
    struct button *pass_btn;
    uint8_t        need_del = pdFALSE;

    if (btn == NULL) {
        log_e("struct button is null!");
    }

    for (pass_btn = button_head; pass_btn != NULL; pass_btn = pass_btn->next) {
        if (pass_btn == btn) {
            need_del = pdTRUE;
            break;
        }
    }

    if (need_del == pdTRUE) {
        button_delete(btn);
    }

    memset(btn, 0, sizeof(struct button));           //清除结构体信息，建议用户在之前清除
    strncpy(btn->name, name, BTN_NAME_MAX);          //创建按键名称
    btn->button_state         = NONE_TRIGGER;        //按键状态
    btn->button_trigger_event = NONE_TRIGGER;        //按键触发事件
    btn->read_button_level    = read_btn_level;      //按键读电平函数
    btn->button_trigger_level = btn_trigger_level;   //按键触发电平
    btn->button_last_level    = !btn_trigger_level;  //按键当前电平,默认为没有被按下的电平
    btn->debounce_time        = 0;                   //消抖时间
    btn->debounce_set_time    = 0;                   //消抖设定时间
    btn->long_set_time        = 0;                   //长按设定时间
    button_add(btn);                                 //创建的时候添加到单链表中
    // button_info_print(btn);                           //打印信息
}

/************************************************************
 * @brief   按键触发事件与回调函数映射链接起来
 * @param   btn : 按键结构体
 * @param   btn_event : 按键触发事件
 * @param   btn_callback : 按键触发之后的回调处理函数。需要用户实现
 * @return  NULL
 ***********************************************************/
void button_attach(button_t *btn, button_event btn_event, button_callBack btn_callback) {
    if (btn == NULL) {
        log_e("struct button is null!");
    }

    /* 按键事件触发的回调函数，用于处理按键事件 */
    if (BUTTON_ALL_RIGGER == btn_event) {
        for (uint8_t i = 0; i < NUMBER_OF_EVENT - 1; i++) {
            btn->callBack_function[i] = btn_callback;
        }
    } else {
        btn->callBack_function[btn_event] = btn_callback;
    }
}

/************************************************************
 * @brief   删除一个已经创建的按键
 * @param NULL
 * @return  NULL
 ***********************************************************/
void button_delete(button_t *btn) {
    struct button **curr;
    taskENTER_CRITICAL();

    for (curr = &button_head; *curr;) {
        struct button *entry = *curr;

        if (entry == btn) {
            *curr = entry->next;
        } else {
            curr = &entry->next;
        }
    }

    taskEXIT_CRITICAL();
}

/************************************************************
 * @brief   获取按键触发的事件
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_event_info_read(button_t *btn) {
    /* 按键事件触发的回调函数，用于处理按键事件 */
    for (uint8_t i = 0; i < NUMBER_OF_EVENT - 1; i++) {
        if (btn->callBack_function[i] != 0) {
            log_d("Button_Event:%d", i);
        }
    }
}

uint8_t button_event_read(button_t *btn) {
    return (uint8_t)(btn->button_trigger_event);
}

/************************************************************
 * @brief   获取按键触发的事件
 * @param   NULL
 * @return  NULL
 ***********************************************************/
uint8_t button_state_read(button_t *btn) {
    return (uint8_t)(btn->button_state);
}

/************************************************************
 * @brief   设定消抖时间
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_set_debounce_time(button_t *btn, uint16_t data)
{
    taskENTER_CRITICAL();
    btn->debounce_set_time = data;
    taskEXIT_CRITICAL();
}

/************************************************************
 * @brief   设定长按时间
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_set_long_time(button_t *btn, uint16_t data)
{
    taskENTER_CRITICAL();
    btn->long_set_time = data;
    taskEXIT_CRITICAL();
}

/************************************************************
 * @brief   按键周期处理函数
 * @param   btn:处理的按键
 * @return  NULL
 ***********************************************************/
void button_cycle_process(button_t *btn) {
    /* 获取当前按键电平 */
    if(btn->read_button_level == NULL)
        return;
    
    uint8_t current_level = (uint8_t) btn->read_button_level();
    // printf("current_level = %d\r\n", current_level);

    if (current_level > 1) {
        return;
    }
    
    uint16_t temp_debounce_time = btn->debounce_set_time ? btn->debounce_set_time : BUTTON_DEBOUNCE_TIME;
    uint16_t temp_long_time = btn->long_set_time ? btn->long_set_time : BUTTON_LONG_CYCLE;
    /* 按键电平发生变化，消抖 */
    if (current_level != btn->button_last_level) {
        if (++(btn->debounce_time) >= temp_debounce_time) {
            /* 更新当前按键电平 */
            btn->button_last_level = current_level;
            /* 电平稳定后,消抖计数清零 */
            btn->debounce_time = 0;
            btn->long_time     = 0;
            btn->button_cycle  = 0;

            /* 按下电平与触发电平一致，表明按下按下 */
            if (btn->button_last_level == btn->button_trigger_level) {
                btn->button_state = BUTTON_DOWM;
            } else {
                /* 释放按键 */
                btn->button_state = BUTTON_UP;
                /* 触发按键抬起回调 */
                TRIGGER_CB(BUTTON_UP);
                // LOG_D("btn release");
            }
        }
    } else {
        btn->debounce_time = 0;
    }

    /* 按键状态机处理 */
    switch (btn->button_state) {
        case BUTTON_DOWM:

            /* 按键按下 */
            if (btn->button_last_level == btn->button_trigger_level) {
#if CONTINUOS_TRIGGER

                /* 支持连续触发 */
                if (++(btn->button_cycle) >= BUTTON_CONTINUOS_CYCLE) {
                    btn->button_cycle         = 0;
                    btn->button_trigger_event = BUTTON_CONTINUOS;
                    /* 按键连按回调处理 */
                    TRIGGER_CB(BUTTON_CONTINUOS);
                    log_d("long press");
                }

#else
                btn->button_trigger_event = BUTTON_DOWM;

                /* 释放按键前更新触发事件为长按 */
                if (++(btn->long_time) >= temp_long_time) {
#if LONG_FREE_TRIGGER
                    btn->button_trigger_event = BUTTON_LONG;
#else

                    /* 连续触发长按的周期 */
                    if (++(btn->button_cycle) >= BUTTON_LONG_CYCLE) {
                        btn->button_cycle = 0;

                        if (btn->callBack_function[BUTTON_LONG] != NULL) {
                            btn->button_trigger_event = BUTTON_LONG;
                            /* 按键长按回调处理 */
                            TRIGGER_CB(BUTTON_LONG);
                            btn->button_state = NONE_TRIGGER;
                        } else {
                            btn->button_trigger_event = BUTTON_DOWM;
                        }
                    }

#endif

                    if (btn->long_time == 0xFF) {
                        /* 更新时间溢出 */
                        btn->long_time = BUTTON_LONG_TIME;
                    }

                    // LOG_D("long press");
                }

#endif
            }

            break;

        case BUTTON_UP:

            /* 弹起状态 */
            if (btn->button_trigger_event == BUTTON_DOWM)  //触发单击
            {
                btn->long_time = 0;       //检测长按失败，清0
                TRIGGER_CB(BUTTON_DOWM);  //单击
                btn->button_state = NONE_TRIGGER;
            } else if (btn->button_trigger_event == BUTTON_LONG) {
#if LONG_FREE_TRIGGER
                TRIGGER_CB(BUTTON_LONG);  //长按
#else
                TRIGGER_CB(BUTTON_LONG_FREE);  //长按释放
#endif
                btn->long_time    = 0;
                btn->button_state = NONE_TRIGGER;
            }
#if CONTINUOS_TRIGGER
            else if (btn->button_trigger_event == BUTTON_CONTINUOS)  //连按
            {
                btn->long_time = 0;
                TRIGGER_CB(BUTTON_CONTINUOS_FREE);  //连发释放
                btn->button_state      = NONE_TRIGGER;
                btn->button_last_state = BUTTON_CONTINUOS;
            }

#endif
            break;

        default:
            // btn->debounce_time = 0;
            btn->long_time    = 0;
            btn->button_cycle = 0;
            break;
    }
}

/************************************************************
 * @brief   遍历的方式扫描按键，不会丢失每个按键
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_process(void) {
    struct button *pass_btn;

    for (pass_btn = button_head; pass_btn != NULL; pass_btn = pass_btn->next) {
        button_cycle_process(pass_btn);
    }
}

/************************************************************
 * @brief   遍历按键
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_search(void) {
    struct button *pass_btn;

    for (pass_btn = button_head; pass_btn != NULL; pass_btn = pass_btn->next) {
        LOG_DEBUG("button node have %s", pass_btn->name);
    }
}

/************************************************************
 * @brief   处理所有按键回调函数
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_Process_CallBack(void *btn) {
    uint8_t btn_event = button_event_read(btn);

    switch (btn_event) {
        case BUTTON_DOWM:
            log_d("WUPA KEY BUTTON DOWN!!\r\n");
            break;

        case BUTTON_UP:
            log_d("WUPA KEY BUTTON UP!!\r\n");
            break;

        case BUTTON_LONG:
            log_d("WUPA KEY BUTTON LONG!!\r\n");
            break;

        case BUTTON_LONG_FREE:
            break;

        case BUTTON_CONTINUOS:
            break;

        case BUTTON_CONTINUOS_FREE:
            break;
    }
}

/************************************************************
 * @brief   打印按键相关信息
 * @param   NULL
 * @return  NULL
 ***********************************************************/
/*
static void button_info_print(button_t *btn)
{
        LOG_DEBUG("button struct information:\n\
                          btn->Name:%s \n\
                          btn->button_state:%d \n\
                          btn->button_trigger_event:%d \n\
                          btn->button_trigger_Level:%d \n\
                          btn->button_last_level:%d \n\
                          ",
                          btn->name,
                          btn->button_state,
                          btn->button_trigger_event,
                          btn->button_trigger_level,
                          btn->button_last_level);

        button_search();
}
*/
/************************************************************
 * @brief   使用单链表将按键连接起来
 * @param   NULL
 * @return  NULL
 ***********************************************************/
static void button_add(button_t *btn) {
    taskENTER_CRITICAL();
    btn->next   = button_head;
    button_head = btn;
    taskEXIT_CRITICAL();
}
