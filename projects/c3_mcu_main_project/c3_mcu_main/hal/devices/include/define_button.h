/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-24
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-3-25       sulikang    0.0.1         创建文件
******************************************************************/

#ifndef _DEFINE_BUTTON_H
#define _DEFINE_BUTTON_H

/*****************************************************************
* 包含头文件
******************************************************************/

/*****************************************************************
* 宏定义
******************************************************************/

/**
 * @ingroup Robot_DEVICES
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
* 宏定义
******************************************************************/
#define BUTTON_LONG_PRESS_HOLD_SINGLE_TRIGGER      0      //长按单次触发
#define BUTTON_LONG_PRESS_HOLD_CONTINUE_TRIGGER    1      //长按以固定频率触发

/*****************************************************************
* 结构定义
******************************************************************/

typedef enum 
{
    BUTTON_CMD_SET_ADD_BUTTON,          //初始化按键对象
    BUTTON_CMD_SET_ADD_ATTACH,          //注册按键事件    
    BUTTON_CMD_GET_PIN_STATE,           //获取IO引脚状态

	BUTTON_CMD_MAX                      // 最多模式类型
} BUTTON_CMD_E;                         // button ioctl命令类型

typedef enum 
{
    GPIO_TRIGGER_LOW,                   //低电平触发
    GPIO_TRIGGER_HIGH,                  //高电平触发

	GPIO_TRIGGER_MAX                    
} GPIO_TRIGGER_CONDITION_E;             //按键触发条件

typedef enum{
    BUTTON_PRESS_DOWN = 0,     //按键按下，每次按下都触发
	BUTTON_PRESS_UP,           //按键弹起，每次松开都触发
	BUTTON_PRESS_REPEAT,       //重复按下触发，变量repeat计数连击次数
	BUTTON_SINGLE_CLICK,       //单击按键事件
	BUTTON_DOUBLE_CLICK,       //双击按键事件
	BUTTON_LONG_PRESS_START,   //两种情况，通过宏进行选择，一、达到长按时间阈值时触发一次，二、长按时以固定频率触发
	BUTTON_LONG_PRESS_HOLD,    //长按期间一直触发
	BUTTON_EXIT,               //满足中断条件触发
	BUTTON_EVENT_MAX,
}BUTTON_STATUS_TYPE;           //此框架支持的按键事件类型

#pragma pack(push,1)
typedef struct
{
    uint8_t button_press_down:1;	   //按键按下，每次按下都触发
    uint8_t button_press_up:1;	       //按键弹起，每次松开都触发
    uint8_t button_press_repeat:1;	   //重复按下触发，变量repeat计数连击次数
    uint8_t button_single_click:1;	   //单击按键事件	
    uint8_t button_double_click:1;     //双击按键事件
    uint8_t button_long_press_start:1; //两种情况，通过宏进行选择，一、达到长按时间阈值时触发一次，二、长按时以固定频率触发
    uint8_t button_long_press_hold:1;  //长按期间一直触发
    uint8_t button_exit:1;             //满足中断条件触发
} BUTTON_ATTACH_T;                     //注册按键支持的事件类型
#pragma pack(pop)

typedef enum
{
    BUTTON_PUB_DATA_TYPE_BOOL,          //bool类型
    BUTTON_PUB_DATA_TYPE_STRING,        //string类型

    BUTTON_PUB_DATA_TYPE_MAX

}BUTTON_PUB_DATA_TYPE_E;                //按键发布的数据类型

typedef struct 
{
    struct bus_info *gpio_info;                  //按键PIN脚信息
    uint16_t period;                             //按键长按触发周期
    GPIO_TRIGGER_CONDITION_E trigger_condition;  //按键触发条件
    uint8_t long_press_hold_type;                //按键长按触发模式
    uint8_t index;                               //按键对象索引值（每组按键有不同的值，如pal_clean.c为一组按键事件）
        
} BUTTON_ATTR_T, *BUTTON_ATTR_P;                  //按键对象属性信息
typedef struct
{
    struct bus_info *gpio_info;                            //按键PIN脚信息
    BUTTON_ATTACH_T attach;                                //按键支持的事件类型
    int (*button_callback)(uint8_t, BUTTON_STATUS_TYPE);   //非中断事件处理函数
    void (*gpio_exti_callback)(const struct ca_device *dev, uint16_t GPIO_Pin);   //中断事件处理函数

}BUTTON_ATTACH_ATTR_T, *BUTTON_ATTACH_ATTR_P;              //按键对象事件处理信息


typedef struct button_ctrl_pin_state_attr
{
    struct bus_info gpio_info;              //按键PIN脚信息
    uint8_t state;                          //PIN脚状态               
}BUTTON_PIN_ATTR_T, *BUTTON_PIN_ATTR_P;     //按键PIN脚属性信息


/*****************************************************************
* 全局变量声明
******************************************************************/

/*****************************************************************
* 函数原型声明
******************************************************************/


/*****************************************************************
* 函数说明
******************************************************************/
typedef void (*multi_button_callBack)(uint8_t state);
typedef void (*interruput_callBack)(uint8_t state);

#ifdef __cplusplus
}
#endif

/* @} Robot_DEVICES */


#endif
