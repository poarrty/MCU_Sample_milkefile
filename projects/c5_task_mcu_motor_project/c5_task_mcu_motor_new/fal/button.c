/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: button.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: button function
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#include "button.h"

#define LOG_TAG "button"
#include "elog.h"
#ifdef USED_BUTTON

/*******************************************************************
 *                      Variable declaration
 *******************************************************************/
static struct button *button_head = NULL;

/*******************************************************************
 *                      Function declaration
 *******************************************************************/
// static void button_info_print(button_t *btn);
static void button_add(button_t *btn);

/************************************************************
 * @brief   button create
 * @param   name : button name
 * @param   btn : button instance
 * @param   read_btn_level : button status read function:user interface
 * @param   btn_trigger_level : button trigger logic level
 * @return  NULL
 ***********************************************************/
void button_create(const char *name, button_t *btn, int (*read_btn_level)(void), uint8_t btn_trigger_level) {
    struct button *pass_btn;
    uint8_t        need_del = FALSE;

    if (btn == NULL) {
        log_e("struct button is null!");
    }

    for (pass_btn = button_head; pass_btn != NULL; pass_btn = pass_btn->next) {
        if (pass_btn == btn) {
            need_del = TRUE;
            break;
        }
    }

    if (need_del == TRUE) {
        button_delete(btn);
    }

    memset(btn, 0, sizeof(struct button));           // clear struct
    strncpy(btn->name, name, BTN_NAME_MAX);          // name
    btn->button_state         = NONE_TRIGGER;        // btn status
    btn->button_trigger_event = NONE_TRIGGER;        // btn trigger event
    btn->read_button_level    = read_btn_level;      // read btn function
    btn->button_trigger_level = btn_trigger_level;   // btn trig logic level
    btn->button_last_level    = !btn_trigger_level;  // idle logic level
    btn->debounce_time        = 0;
    button_add(btn);  // adding button link list
    // button_info_print(btn);                           //print message
}

/************************************************************
 * @brief   Link trigger event to callback function
 * @param   btn : button struct
 * @param   btn_event : button event
 * @param   btn_callback : button callback function:user interface
 * @return  NULL
 ***********************************************************/
void button_attach(button_t *btn, button_event btn_event, button_callBack btn_callback) {
    if (btn == NULL) {
        log_e("struct button is null!");
    }

    /* Add callback function for triggering event */
    if (BUTTON_ALL_RIGGER == btn_event) {
        for (uint8_t i = 0; i < NUMBER_OF_EVENT - 1; i++) {
            btn->callBack_function[i] = btn_callback;
        }
    } else {
        btn->callBack_function[btn_event] = btn_callback;
    }
}

/************************************************************
 * @brief   Delete a created button
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
 * @brief   Gets the event triggered by the button
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_event_info_read(button_t *btn) {
    /* print button event */
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
 * @brief   get button trigger event
 * @param   NULL
 * @return  NULL
 ***********************************************************/
uint8_t button_state_read(button_t *btn) {
    return (uint8_t)(btn->button_state);
}

/************************************************************
 * @brief   button process function
 * @param   btn:the process button
 * @return  NULL
 ***********************************************************/
void button_cycle_process(button_t *btn) {
    /* get btn current level */
    uint8_t current_level = (uint8_t) btn->read_button_level();

    if (current_level > 1) {
        return;
    }

    /* button level change,Debounce */
    if (current_level != btn->button_last_level) {
        if (++(btn->debounce_time) >= BUTTON_DEBOUNCE_TIME) {
            /* update button level */
            btn->button_last_level = current_level;
            /* button level stabel,clear cnt*/
            btn->debounce_time = 0;
            btn->long_time     = 0;
            btn->button_cycle  = 0;

            /* The press level is the same as the trigger level,The key is pressed */
            if (btn->button_last_level == btn->button_trigger_level) {
                btn->button_state = BUTTON_DOWM;
            } else {
                /* release */
                btn->button_state = BUTTON_UP;
                /* trigger button up event calls callback function */
                TRIGGER_CB(BUTTON_UP);
                // log_d("btn release");
            }
        }
    } else {
        btn->debounce_time = 0;
    }

    /* Key state machine processing */
    switch (btn->button_state) {
        case BUTTON_DOWM:

            /* pressed */
            if (btn->button_last_level == btn->button_trigger_level) {
#if CONTINUOS_TRIGGER

                /* continues trigger */
                if (++(btn->button_cycle) >= BUTTON_CONTINUOS_CYCLE) {
                    btn->button_cycle         = 0;
                    btn->button_trigger_event = BUTTON_CONTINUOS;
                    /* conintue pressed callback */
                    TRIGGER_CB(BUTTON_CONTINUOS);
                    log_d("long press");
                }

#else
                btn->button_trigger_event = BUTTON_DOWM;

                /* long press entry trigger */
                if (++(btn->long_time) >= BUTTON_LONG_TIME) {
#if LONG_FREE_TRIGGER
                    btn->button_trigger_event = BUTTON_LONG;
#else

                    /* long press event trigger */
                    if (++(btn->button_cycle) >= BUTTON_LONG_CYCLE) {
                        btn->button_cycle = 0;

                        if (btn->callBack_function[BUTTON_LONG] != NULL) {
                            btn->button_trigger_event = BUTTON_LONG;
                            /* long press event callback */
                            TRIGGER_CB(BUTTON_LONG);
                            btn->button_state = NONE_TRIGGER;
                        } else {
                            btn->button_trigger_event = BUTTON_DOWM;
                        }
                    }

#endif

                    if (btn->long_time == 0xFF) {
                        /* update long time overflow */
                        btn->long_time = BUTTON_LONG_TIME;
                    }

                    // log_d("long press");
                }

#endif
            }

            break;

        case BUTTON_UP:

            /* release status */
            if (btn->button_trigger_event == BUTTON_DOWM)  // trigger single click
            {
                btn->long_time = 0;       // clear status
                TRIGGER_CB(BUTTON_DOWM);  // single click event callback
                btn->button_state = NONE_TRIGGER;
            } else if (btn->button_trigger_event == BUTTON_LONG) {
#if LONG_FREE_TRIGGER
                TRIGGER_CB(BUTTON_LONG);  // long press event callback
#else
                TRIGGER_CB(BUTTON_LONG_FREE);  // long press and release callback
#endif
                btn->long_time    = 0;
                btn->button_state = NONE_TRIGGER;
            }
#if CONTINUOS_TRIGGER
            else if (btn->button_trigger_event == BUTTON_CONTINUOS)  // continue press
            {
                btn->long_time = 0;
                TRIGGER_CB(BUTTON_CONTINUOS_FREE);  // continue press and release event callback
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
 * @brief   process all button
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
 * @brief   print exist button
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_search(void) {
    struct button *pass_btn;

    for (pass_btn = button_head; pass_btn != NULL; pass_btn = pass_btn->next) {
        log_d("button node have %s", pass_btn->name);
    }
}

/************************************************************
 * @brief   process all button event
 * @param   NULL
 * @return  NULL
 ***********************************************************/
void button_Process_CallBack(void *btn) {
    uint8_t btn_event = button_event_read(btn);

    switch (btn_event) {
        case BUTTON_DOWM:
            break;

        case BUTTON_UP:
            break;

        case BUTTON_LONG:
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
 * @brief   print button message
 * @param   NULL
 * @return  NULL
 ***********************************************************/
/*
static void button_info_print(button_t *btn)
{
 log_d("button struct information:\n\
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
 * @brief   add button by link list
 * @param   NULL
 * @return  NULL
 ***********************************************************/
static void button_add(button_t *btn) {
    taskENTER_CRITICAL();
    btn->next   = button_head;
    button_head = btn;
    taskEXIT_CRITICAL();
}

#endif