#ifndef BUTTON_H
#define BUTTON_H

#include "stdint.h"

/* 名字最大为32字节 */
#define BTN_NAME_MAX 16

/* 按键消抖时间40ms, 建议调用周期为20ms
 * 只有连续检测到40ms状态不变才认为有效，包括弹起和按下两种事件
 **/

#define CONTINUOS_TRIGGER 0  //是否支持连续触发，连发的话就不要检测单双击与长按了

/* 是否支持长按释放才触发，如果打开这个宏定义，那么长按释放之后才触发单次长按，
   否则在长按指定时间就一直触发长按，触发周期由 BUTTON_LONG_CYCLE 决定 */
#define LONG_FREE_TRIGGER 0

#define BUTTON_DEBOUNCE_TIME   5   //消抖时间      (n-1)*调用周期
#define BUTTON_CONTINUOS_CYCLE 1    //连按触发周期时间  (n-1)*调用周期
#define BUTTON_LONG_CYCLE      1    //长按触发周期时间  (n-1)*调用周期
#define BUTTON_LONG_TIME       50  // 持续n秒((n-1)*调用周期 ms)，认为长按事件

#define TRIGGER_CB(event)              \
    if (btn->callBack_function[event]) \
    btn->callBack_function[event]((button_t *) btn)

/* 按键触发回调函数，需要用户实现 */
typedef void (*button_callBack)(void *);

typedef enum {
    BUTTON_DOWM = 0,
    BUTTON_UP,
    BUTTON_LONG,
    BUTTON_LONG_FREE,
    BUTTON_CONTINUOS,
    BUTTON_CONTINUOS_FREE,
    BUTTON_ALL_RIGGER,
    NUMBER_OF_EVENT, /* 触发回调的事件 */
    NONE_TRIGGER
} button_event;

/*
    每个按键对应1个全局的结构体变量。
    其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct button {
    /* 下面是一个函数指针，指向判断按键手否按下的函数 */
    int (*read_button_level)(void); /* 读取按键电平函数，需要用户实现 */
    char            name[BTN_NAME_MAX];
    uint8_t         button_state : 4;                   /* 按键当前状态（按下还是弹起） */
    uint8_t         button_trigger_level : 2;           /* 按键触发电平 */
    uint8_t         button_last_level : 2;              /* 按键当前电平 */
    uint8_t         button_trigger_event;               /* 按键触发事件，单击，双击，长按等 */
    button_callBack callBack_function[NUMBER_OF_EVENT]; /*元素为函数指针，回调函数*/
    uint16_t        button_cycle;                       /* 连续按键周期 */
    uint16_t        debounce_time;                      /* 消抖时间 */
    uint16_t        debounce_set_time;                  /* 设定的消抖时间 */
    uint16_t        long_time;                         /* 按键按下持续时间 */
    uint16_t        long_set_time;                      /* 长按设定时间 */
    struct button * next;
} button_t;

/* 供外部调用的函数声明 */
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
void    button_set_debounce_time(button_t *btn, uint16_t data);
void    button_set_long_time(button_t *btn, uint16_t data);

#endif
