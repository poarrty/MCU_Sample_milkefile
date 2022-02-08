/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: button.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: button function
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H

#include <board.h>

/* The maximum length of the button name is 32 bytes */
#define BTN_NAME_MAX 16

/* debounce time:40ms, advise detection cycle:20ms
 * Effective detection time length:40ms,press and release the button
 **/

#define CONTINUOS_TRIGGER 0

/* long depress detection */
#define LONG_FREE_TRIGGER 0

#define BUTTON_DEBOUNCE_TIME   10   //(n-1)*run cycle
#define BUTTON_CONTINUOS_CYCLE 1    //(n-1)*run cycle
#define BUTTON_LONG_CYCLE      1    //(n-1)*run cycle
#define BUTTON_LONG_TIME       120  // keep n second ((n-1)*run cycle ms),trigger long press event

#define TRIGGER_CB(event)              \
    if (btn->callBack_function[event]) \
    btn->callBack_function[event]((button_t *) btn)

/* button event trigger callback funct:user interface*/
typedef void (*button_callBack)(void *);

typedef enum {
    BUTTON_DOWM = 0,
    BUTTON_UP,
    BUTTON_LONG,
    BUTTON_LONG_FREE,
    BUTTON_CONTINUOS,
    BUTTON_CONTINUOS_FREE,
    BUTTON_ALL_RIGGER,
    NUMBER_OF_EVENT, /* trigger callback event */
    NONE_TRIGGER
} button_event;

/*
A key corresponds to a structure obj
*/
typedef struct button {
    int (*read_button_level)(void); /* read button status function:user interface */
    char            name[BTN_NAME_MAX];
    uint8_t         button_state : 4;         /* button status:depress or release */
    uint8_t         button_trigger_level : 2; /* button trigger voltage */
    uint8_t         button_last_level : 2;    /* Key current level */
    uint8_t         button_trigger_event;     /* button trigger event:single,double,long depress */
    button_callBack callBack_function[NUMBER_OF_EVENT];
    uint8_t         button_cycle;  /* continue key cycle */
    uint8_t         debounce_time; /* button debounce time*/
    uint8_t         long_time;     /* key depress continue time*/
    struct button * next;
} button_t;

/* Function declaration for external calls */
void    button_create(const char *name, button_t *btn, int (*read_btn_level)(void), uint8_t btn_trigger_level);
void    button_attach(button_t *btn, button_event btn_event, button_callBack btn_callback);
void    button_cycle_Process(button_t *btn);
void    button_process(void);
void    button_delete(button_t *btn);
void    button_search(void);
void    button_event_info_read(button_t *btn);
uint8_t button_event_read(button_t *btn);
uint8_t button_state_read(button_t *btn);
void    button_process_callBack(void *btn);

#endif
