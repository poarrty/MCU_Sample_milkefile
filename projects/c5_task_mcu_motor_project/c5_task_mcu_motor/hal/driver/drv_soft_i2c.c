/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-08     balanceTWK   first version
 */

#include "drv_soft_i2c.h"
#include "i2c-bit-ops.h"
#define LOG_TAG "drv_soft_i2c"
#include "elog.h"

#ifdef RT_USING_I2C
#ifdef BSP_USING_SOFT_I2C

#if !defined(BSP_USING_I2C1) && !defined(BSP_USING_I2C2) && !defined(BSP_USING_I2C3) && !defined(BSP_USING_I2C4)
#error "Please define at least one BSP_USING_I2Cx"
/* this driver can be disabled at menuconfig → RT-Thread Components → Device Drivers */
#endif

static const struct stm32_soft_i2c_config soft_i2c_config[] = {
#ifdef BSP_USING_I2C1
    I2C1_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C2
    I2C2_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C3
    I2C3_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C4
    I2C4_BUS_CONFIG,
#endif
};

/* stm32 i2c dirver class */
struct stm32_i2c {
    struct rt_i2c_bit_ops    ops;
    struct rt_i2c_bus_device i2c2_bus;
};

static struct stm32_i2c i2c_obj[sizeof(soft_i2c_config) / sizeof(soft_i2c_config[0])];

/**
 * This function initializes the i2c pin.
 *
 * @param Stm32 i2c dirver class.
 */
static void stm32_i2c_gpio_init(struct stm32_i2c *i2c) {
    struct stm32_soft_i2c_config *cfg = (struct stm32_soft_i2c_config *) i2c->ops.data;

    _stm32_pin_ops.pin_mode(0, cfg->scl, PIN_MODE_OUTPUT_OD);
    _stm32_pin_ops.pin_mode(0, cfg->sda, PIN_MODE_OUTPUT_OD);

    _stm32_pin_ops.pin_write(0, cfg->scl, PIN_HIGH);
    _stm32_pin_ops.pin_write(0, cfg->sda, PIN_HIGH);
}

/**
 * This function sets the sda pin.
 *
 * @param Stm32 config class.
 * @param The sda pin state.
 */
static void stm32_set_sda(void *data, rt_int32_t state) {
    struct stm32_soft_i2c_config *cfg = (struct stm32_soft_i2c_config *) data;

    if (state) {
        _stm32_pin_ops.pin_write(0, cfg->sda, PIN_HIGH);
    } else {
        _stm32_pin_ops.pin_write(0, cfg->sda, PIN_LOW);
    }
}

/**
 * This function sets the scl pin.
 *
 * @param Stm32 config class.
 * @param The scl pin state.
 */
static void stm32_set_scl(void *data, rt_int32_t state) {
    struct stm32_soft_i2c_config *cfg = (struct stm32_soft_i2c_config *) data;

    if (state) {
        _stm32_pin_ops.pin_write(0, cfg->scl, PIN_HIGH);
    } else {
        _stm32_pin_ops.pin_write(0, cfg->scl, PIN_LOW);
    }
}

/**
 * This function gets the sda pin state.
 *
 * @param The sda pin state.
 */
static rt_int32_t stm32_get_sda(void *data) {
    struct stm32_soft_i2c_config *cfg = (struct stm32_soft_i2c_config *) data;
    return _stm32_pin_ops.pin_read(0, cfg->sda);
}

/**
 * This function gets the scl pin state.
 *
 * @param The scl pin state.
 */
static rt_int32_t stm32_get_scl(void *data) {
    struct stm32_soft_i2c_config *cfg = (struct stm32_soft_i2c_config *) data;
    return _stm32_pin_ops.pin_read(0, cfg->scl);
}
/**
 * The time delay function.
 *
 * @param microseconds.
 */
static void stm32_udelay(rt_uint32_t us) {
    if (us == 0) {
        return;
    }

    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    told  = SysTick->VAL;

    while (1) {
        tnow = SysTick->VAL;

        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks) {
                break;
            }
        }
    }
}

static const struct rt_i2c_bit_ops stm32_bit_ops_default = {.data     = RT_NULL,
                                                            .set_sda  = stm32_set_sda,
                                                            .set_scl  = stm32_set_scl,
                                                            .get_sda  = stm32_get_sda,
                                                            .get_scl  = stm32_get_scl,
                                                            .udelay   = stm32_udelay,
                                                            .delay_us = 1,
                                                            .timeout  = 100};

/**
 * if i2c is locked, this function will unlock it
 *
 * @param stm32 config class
 *
 * @return RT_EOK indicates successful unlock.
 */
static rt_err_t stm32_i2c_bus_unlock(const struct stm32_soft_i2c_config *cfg) {
    rt_int32_t i = 0;

    if (PIN_LOW == _stm32_pin_ops.pin_read(0, cfg->sda)) {
        while (i++ < 9) {
            _stm32_pin_ops.pin_write(0, cfg->scl, PIN_HIGH);
            stm32_udelay(100);
            _stm32_pin_ops.pin_write(0, cfg->scl, PIN_LOW);
            stm32_udelay(100);
        }
    }

    if (PIN_LOW == _stm32_pin_ops.pin_read(0, cfg->sda)) {
        return -RT_ERROR;
    }

    return RT_EOK;
}

/* I2C initialization function */
int rt_hw_i2c_init(void) {
    rt_size_t obj_num = sizeof(i2c_obj) / sizeof(struct stm32_i2c);
    rt_err_t  result;

    for (int i = 0; i < obj_num; i++) {
        i2c_obj[i].ops           = stm32_bit_ops_default;
        i2c_obj[i].ops.data      = (void *) &soft_i2c_config[i];
        i2c_obj[i].i2c2_bus.priv = &i2c_obj[i].ops;
        i2c_obj[i].i2c2_bus.ops  = &i2c_bit_bus_ops;
        stm32_i2c_gpio_init(&i2c_obj[i]);
        stm32_i2c_bus_unlock(&soft_i2c_config[i]);
        log_d("software simulation %s init done, pin scl: %d, pin sda %d", soft_i2c_config[i].bus_name, soft_i2c_config[i].scl,
              soft_i2c_config[i].sda);
    }

    return RT_EOK;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rt_hw_i2c_init, rt_hw_i2c_init, rt_hw_i2c_init);
#endif
#endif /* RT_USING_I2C */
