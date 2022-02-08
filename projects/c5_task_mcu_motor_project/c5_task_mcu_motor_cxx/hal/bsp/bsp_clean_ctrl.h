/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_clean_ctrl.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: clean unit operation interface entry
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_CLEAN_CTRL_H
#define __BSP_CLEAN_CTRL_H
#include <board.h>

uint32_t clean_io_ctrl(uint8_t gpio_id, uint32_t value);
void     clean_ctrl_all_reset(void);
uint32_t side_brush_motor_ops(uint8_t id, uint32_t value);
uint32_t side_brush_motor_status_get(uint8_t id);
uint32_t sunction_motor_ops(uint8_t id, uint32_t value);
uint32_t sunction_motor_speed_get(uint8_t id);
uint32_t sunction_motor_status_get(uint8_t id);
uint32_t push_rod_motor_ops(uint8_t id, uint32_t value);
uint32_t push_rod_motor_status_get(uint8_t id);
uint32_t roll_brush_motor_ops(uint8_t id, uint32_t value);
uint32_t roll_brush_motor_speed_get(uint8_t id);
uint32_t roll_brush_motor_status_get(uint8_t id);
uint32_t water_distribution_pump_ops(uint8_t id, uint32_t value);
uint32_t water_distribution_pump_speed_get(uint8_t id);
uint32_t water_distribution_pump_status_get(uint8_t id);
uint32_t water_valve_clean_ops(uint8_t id, uint32_t value);
uint32_t water_valve_clean_status_get(uint8_t id);
uint32_t roll_brush_motor_report_speed_set(uint32_t speed);
uint32_t side_brush_motor_cw_ops(uint8_t id, uint32_t value);
uint32_t fan_motor_speed_set_ops(uint8_t id, uint32_t value);
uint32_t fan_motor_speed_fbk_ops(uint8_t id, uint32_t value);
uint32_t fan_motor_error_code_ops(uint8_t id, uint32_t value);
uint32_t app_version_info_get(uint8_t id);
uint32_t app_update_info_get(uint8_t id);
uint32_t app_update_file_recv_set(uint8_t id, uint32_t value);
uint32_t app_update_enter_boot_set(uint8_t id, uint32_t value);
int      clean_ops_test(uint32_t id, uint32_t value);
#endif
