/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         chenyuliang
 ** Version:        V0.0.1
 ** Date:           2021-12-20
 ** Description:		
 ** Others:
 ** Function List:
 ** History:        2021-12 chenyulaing establish
 ** <time>          <author>    <version >    <desc>
 ** 2021-12-20      chenyuliang					1.0         establish
 ******************************************************************/


#ifndef _FAL_MOTOR_H
#define _FAL_MOTOR_H

/**
 * @ingroup Robot_PAL Protocol abstraction layer
 *
 * @brief \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	UP_DOWM_PUSH_ROD,         //升降推杆电机
	SIDE_BRUSH,               //边刷
	ROLLER_BRUSH,             //滚刷
	ROLLER_TUBE,              //滚筒
	CLEAN_WATER_PUMP,         //清水泵
	SEWAGE_WATER_PUMP,        //污水泵
	FAN_MOTOR,                //风机
	CLEAN_WATER_VALVE,		  //清水阀
	SEWAGE_WATER_VALVE,		  //污水阀
	CLEAN_MODULE_TYPE_MAX,
	ID_MODE_NULL,
	ID_MODE_MOPPING,
	ID_MODE_DEDUSTING,
}CLEAN_MODULE_TYPE_E;         //组建类型

typedef struct clean_ctrl_component
{
	char *model;						/* 清洁组件模式名称 */
	uint8_t mode;						/* 控制模式，洗地或尘推，充电等*/
	uint8_t last_mode;					/* 记录任务上一次下发的工作模式 */
	uint32_t last_recv_time;			/* 记录上一次接受时间，用于超时计算*/
	float dev_set_value[CLEAN_MODULE_TYPE_MAX];	/* 组件控制设定值 */
	float dev_fb_value[CLEAN_MODULE_TYPE_MAX];		/* 组件反馈控制值 */
	int dev_err_value[CLEAN_MODULE_TYPE_MAX];		/* 组件错误码 */
	uint32_t last_fb_time; 				/* 记录上一次电控反馈时间 */

}clean_ctrl_component_t;

/* 组件控制信息 */
extern clean_ctrl_component_t clean_ctrl_comp_info;
/* 组件名 */
extern char *mcu_dev_name[CLEAN_MODULE_TYPE_MAX];

#ifdef __cplusplus
}
#endif

/* @} Robot_PAL */

#endif