/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_i2c.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: i2c operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include <board.h>

#define i2c1_devAddr 0xA0
#define i2c2_devAddr 0xA0
enum { hw_i2c1 = 0, hw_i2c2 };
int i2c_read_regs(uint8_t i2c_obj_id, uint8_t reg_addr, uint8_t *buf, uint8_t len);
int i2c_write_regs(uint8_t i2c_obj_id, uint8_t reg_addr, uint8_t *buf, uint8_t len);
#endif
