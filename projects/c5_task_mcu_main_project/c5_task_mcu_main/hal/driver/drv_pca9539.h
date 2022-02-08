#ifndef __DRV_PCA9539_H
#define __DRV_PCA9539_H

#include "stdint.h"

#define PCA9539_DEFAULT_I2C_ADDR    0x74
#define PCA9539_W                   (PCA9539_DEFAULT_I2C_ADDR << 1) | 0x00
#define PCA9539_R                   (PCA9539_DEFAULT_I2C_ADDR << 1) | 0x01
#define PORT_PER_PIN                (8)
#define PCA9539_GET_PIN(PORTx, PIN) (uint8_t)(PORT_PER_PIN * (PORTx) + (PIN))

#define PORT_0_INPUT_CMD    0x00
#define PORT_1_INPUT_CMD    0x01
#define PORT_0_OUTPUT_CMD   0x02
#define PORT_1_OUTPUT_CMD   0x03
#define PORT_0_POLARITY_CMD 0x04
#define PORT_1_POLARITY_CMD 0x05
#define PORT_0_CONFIG_CMD   0x06
#define PORT_1_CONFIG_CMD   0x07

#define PIN_MODE_OUTPUT 0x00
#define PIN_MODE_INPUT  0x01

enum { PCA9539_PORT0 = 0, PCA9539_PORT1 };

enum { PIN0 = 0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7 };

//封装写寄存器函数
int     pca9539_write_regs(uint8_t reg, uint8_t const *val, uint8_t val_size);
int     pca9539_read_regs(uint8_t reg, uint8_t const *val, uint8_t val_size);
void    pca9539_mode(uint8_t reg, uint8_t mode, uint8_t pin);
void    pca9539_pin_write(uint8_t port, uint8_t pin, uint8_t pin_val);
int     pca9539_pin_read(uint8_t port, uint8_t pin);
void    pca9539_read(uint8_t port, uint8_t pin);
void    clear_i2c_error_status(void);
uint8_t get_i2c_error_status(void);
//配置KJ_B 为输出模式
//配置KEY1 为输入模式

#endif
