/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     balanceTWK        first version
 * 2020-06-16     thread-liu        add stm32mp1
 * 2020-09-01     thread-liu        add GPIOZ
 * 2020-09-18     geniusgogo        optimization design pin-index algorithm
 */

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <board.h>

#ifdef drv_gpio_module
#define drv_gpio_global
#else
#define drv_gpio_global extern
#endif  // !drv_gpio_module

#ifdef __cplusplus
extern "C" {
#endif

#define __STM32_PORT(port) GPIO##port##_BASE

#if defined(SOC_SERIES_STM32MP1)
#define GET_PIN(PORTx, PIN) \
    (GPIO##PORTx == GPIOZ) ? (176 + PIN) : ((rt_base_t)((16 * (((rt_base_t) __STM32_PORT(PORTx) - (rt_base_t) GPIOA_BASE) / (0x1000UL))) + PIN))
#else
#define GET_PIN(PORTx, PIN) (rt_base_t)((16 * (((rt_base_t) __STM32_PORT(PORTx) - (rt_base_t) GPIOA_BASE) / (0x0400UL))) + PIN)
#endif
#define PIN_NUM(port, no) (((((port) &0xFu) << 4) | ((no) &0xFu)))
#define PIN_PORT(pin)     ((uint8_t)(((pin) >> 4) & 0xFu))
#define PIN_NO(pin)       ((uint8_t)((pin) &0xFu))

#if defined(SOC_SERIES_STM32MP1)
#if defined(GPIOZ)
#define gpioz_port_base (175) /* PIN_STPORT_MAX * 16 - 16 */
#define PIN_STPORT(pin) \
    ((pin > gpioz_port_base) ? ((GPIO_TypeDef *) (GPIOZ_BASE)) : ((GPIO_TypeDef *) (GPIOA_BASE + (0x1000u * PIN_PORT(pin)))))
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *) (GPIOA_BASE + (0x1000u * PIN_PORT(pin))))
#endif /* GPIOZ */
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *) (GPIOA_BASE + (0x400u * PIN_PORT(pin))))
#endif /* SOC_SERIES_STM32MP1 */

#define PIN_STPIN(pin) ((uint16_t)(1u << PIN_NO(pin)))

#if defined(GPIOZ)
#define __STM32_PORT_MAX 12u
#elif defined(GPIOK)
#define __STM32_PORT_MAX 11u
#elif defined(GPIOJ)
#define __STM32_PORT_MAX 10u
#elif defined(GPIOI)
#define __STM32_PORT_MAX 9u
#elif defined(GPIOH)
#define __STM32_PORT_MAX 8u
#elif defined(GPIOG)
#define __STM32_PORT_MAX 7u
#elif defined(GPIOF)
#define __STM32_PORT_MAX 6u
#elif defined(GPIOE)
#define __STM32_PORT_MAX 5u
#elif defined(GPIOD)
#define __STM32_PORT_MAX 4u
#elif defined(GPIOC)
#define __STM32_PORT_MAX 3u
#elif defined(GPIOB)
#define __STM32_PORT_MAX 2u
#elif defined(GPIOA)
#define __STM32_PORT_MAX 1u
#else
#define __STM32_PORT_MAX 0u
#error Unsupported STM32 GPIO peripheral.
#endif

#define PIN_STPORT_MAX __STM32_PORT_MAX

struct pin_irq_map {
    rt_uint16_t pinbit;
    IRQn_Type   irqno;
};

int                            rt_hw_pin_init(void);
rt_size_t                      rt_strlen(const char *s);
extern const struct rt_pin_ops _stm32_pin_ops;

/* author:chenyuliang@cvte.com */
#define REG32(addr)                 (*(volatile uint32_t *) (uint32_t)(addr))
#define GPIO_ISTAT(gpiox)           REG32((gpiox) + 0x08U) /*!< GPIO port input status register */
#define GPIO_OCTL(gpiox)            REG32((gpiox) + 0x0CU) /*!< GPIO port output control register */
#define GPIO_BOP(gpiox)             REG32((gpiox) + 0x10U) /*!< GPIO port bit operation register */
#define GPIO_BC(gpiox)              REG32((gpiox) + 0x14U) /*!< GPIO bit clear register */
#define GPIO_BOP_H(gpiox, pinx)     (GPIO_BOP(gpiox) = pinx)
#define GPIO_BOP_L(gpiox, pinx)     (GPIO_BOP(gpiox) = pinx << 16)
#define GPIO_W(gpiox, pinx, status) (status ? GPIO_BOP_H(gpiox, pinx) : GPIO_BOP_L(gpiox, pinx))
#define GPIO_R(gpiox, pinx)         (GPIO_OCTL(gpiox) & pinx ? 1 : 0)

/* 位域---------------------------------------------------------------------> */
#ifndef GPIO_READ_ERROR
#define GPIO_READ_ERROR -1
#endif

// enum {FALSE = 0, TRUE = !FALSE};

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#ifndef NULL
#define NULL 0
#endif

typedef struct _16_Bits_Struct {
    u16 bit0 : 1;
    u16 bit1 : 1;
    u16 bit2 : 1;
    u16 bit3 : 1;
    u16 bit4 : 1;
    u16 bit5 : 1;
    u16 bit6 : 1;
    u16 bit7 : 1;
    u16 bit8 : 1;
    u16 bit9 : 1;
    u16 bit10 : 1;
    u16 bit11 : 1;
    u16 bit12 : 1;
    u16 bit13 : 1;
    u16 bit14 : 1;
    u16 bit15 : 1;
} Bits_16_TypeDef;

#define PORT_OUT(IO_PORT)            ((Bits_16_TypeDef *) (&(IO_PORT->ODR)))
#define PORT_IN(IO_PORT)             ((Bits_16_TypeDef *) (&(IO_PORT->IDR)))
#define PIN_BIT(x)                   bit##x
#define PORT_BIT(x)                  GPIO##x
#define BIT_ADDR(PORT_INOUT, BITPIN) (PORT_INOUT->BITPIN)

////IO口操作,只对单一的IO口!
////确保n的值小于16!
#define PINout(port, n) BIT_ADDR(PORT_OUT(port), PIN_BIT(n))  //输出
#define PINin(port, n)  BIT_ADDR(PORT_IN(port), PIN_BIT(n))   //输入

#define PnOut(port, n) BIT_ADDR(PORT_OUT(PORT_BIT(port)), PIN_BIT(n))  //输出
#define PnIn(port, n)  BIT_ADDR(PORT_IN(PORT_BIT(port)), PIN_BIT(n))   //输入

#define PAout(n) BIT_ADDR(PORT_OUT(GPIOA), PIN_BIT(n))  //输出
#define PAin(n)  BIT_ADDR(PORT_IN(GPIOA), PIN_BIT(n))   //输入

#define PBout(n) BIT_ADDR(PORT_OUT(GPIOB), PIN_BIT(n))  //输出
#define PBin(n)  BIT_ADDR(PORT_IN(GPIOB), PIN_BIT(n))   //输入

#define PCout(n) BIT_ADDR(PORT_OUT(GPIOC), PIN_BIT(n))  //输出
#define PCin(n)  BIT_ADDR(PORT_IN(GPIOC), PIN_BIT(n))   //输入

#define PDout(n) BIT_ADDR(PORT_OUT(GPIOD), PIN_BIT(n))  //输出
#define PDin(n)  BIT_ADDR(PORT_IN(GPIOD), PIN_BIT(n))   //输入

#define PEout(n) BIT_ADDR(PORT_OUT(GPIOE), PIN_BIT(n))  //输出
#define PEin(n)  BIT_ADDR(PORT_IN(GPIOE), PIN_BIT(n))   //输入

#define PFout(n) BIT_ADDR(PORT_OUT(GPIOF), PIN_BIT(n))  //输出
#define PFin(n)  BIT_ADDR(PORT_IN(GPIOF), PIN_BIT(n))   //输入

#define PGout(n) BIT_ADDR(PORT_OUT(GPIOG), PIN_BIT(n))  //输出
#define PGin(n)  BIT_ADDR(PORT_IN(GPIOG), PIN_BIT(n))   //输入

/*--------------------- gpio in ---------------------*/
#define GPIOin_IDR(port, pin)          ((port->IDR & pin) ? PIN_HIGH : PIN_LOW)
#define GPIOin_byte(port, pin, status) (*status = GPIOin_IDR(port, pin))

#define GPIOin(port, pin, val)      \
    do {                            \
        if (pin & GPIO_PIN_0)       \
            *val = PINin(port, 0);  \
        else if (pin & GPIO_PIN_1)  \
            *val = PINin(port, 1);  \
        else if (pin & GPIO_PIN_2)  \
            *val = PINin(port, 2);  \
        else if (pin & GPIO_PIN_3)  \
            *val = PINin(port, 3);  \
        else if (pin & GPIO_PIN_4)  \
            *val = PINin(port, 4);  \
        else if (pin & GPIO_PIN_5)  \
            *val = PINin(port, 5);  \
        else if (pin & GPIO_PIN_6)  \
            *val = PINin(port, 6);  \
        else if (pin & GPIO_PIN_7)  \
            *val = PINin(port, 7);  \
        else if (pin & GPIO_PIN_8)  \
            *val = PINin(port, 8);  \
        else if (pin & GPIO_PIN_9)  \
            *val = PINin(port, 9);  \
        else if (pin & GPIO_PIN_10) \
            *val = PINin(port, 10); \
        else if (pin & GPIO_PIN_11) \
            *val = PINin(port, 11); \
        else if (pin & GPIO_PIN_12) \
            *val = PINin(port, 12); \
        else if (pin & GPIO_PIN_13) \
            *val = PINin(port, 13); \
        else if (pin & GPIO_PIN_14) \
            *val = PINin(port, 14); \
        else if (pin & GPIO_PIN_15) \
            *val = PINin(port, 15); \
    } while (0)

#define GPIOin_num(port, n, val)    \
    do {                            \
        if (n == 0)                 \
            *val = PINin(port, 0);  \
        else if (n == 1)            \
            *val = PINin(port, 1);  \
        else if (n == 2)            \
            *val = PINin(port, 2);  \
        else if (n == 3)            \
            *val = PINin(port, 3);  \
        else if (n == 4)            \
            *val = PINin(port, 4);  \
        else if (n == 5)            \
            *val = PINin(port, 5);  \
        else if (n == 6)            \
            *val = PINin(port, 6);  \
        else if (n == 7)            \
            *val = PINin(port, 7);  \
        else if (n == 8)            \
            *val = PINin(port, 8);  \
        else if (n == 9)            \
            *val = PINin(port, 9);  \
        else if (n == 10)           \
            *val = PINin(port, 10); \
        else if (n == 11)           \
            *val = PINin(port, 11); \
        else if (n == 12)           \
            *val = PINin(port, 12); \
        else if (n == 13)           \
            *val = PINin(port, 13); \
        else if (n == 14)           \
            *val = PINin(port, 14); \
        else if (n == 15)           \
            *val = PINin(port, 15); \
    } while (0)

#define GPIOin_char(port, n, val)   \
    do {                            \
        if (n == '0')               \
            *val = PINin(port, 0);  \
        else if (n == '1')          \
            *val = PINin(port, 1);  \
        else if (n == '2')          \
            *val = PINin(port, 2);  \
        else if (n == '3')          \
            *val = PINin(port, 3);  \
        else if (n == '4')          \
            *val = PINin(port, 4);  \
        else if (n == '5')          \
            *val = PINin(port, 5);  \
        else if (n == '6')          \
            *val = PINin(port, 6);  \
        else if (n == '7')          \
            *val = PINin(port, 7);  \
        else if (n == '8')          \
            *val = PINin(port, 8);  \
        else if (n == '9')          \
            *val = PINin(port, 9);  \
        else if (n == '10')         \
            *val = PINin(port, 10); \
        else if (n == '11')         \
            *val = PINin(port, 11); \
        else if (n == '12')         \
            *val = PINin(port, 12); \
        else if (n == '13')         \
            *val = PINin(port, 13); \
        else if (n == '14')         \
            *val = PINin(port, 14); \
        else if (n == '15')         \
            *val = PINin(port, 15); \
    } while (0)

/*--------------------- gpio out ---------------------*/
#define GPIOout_byte(port, pin, status) (status ? port->ODR |= pin : port->ODR &= ~pin)

#define GPIOout(port, pin, val)     \
    do {                            \
        if (pin & GPIO_PIN_0)       \
            PINout(port, 0) = val;  \
        else if (pin & GPIO_PIN_1)  \
            PINout(port, 1) = val;  \
        else if (pin & GPIO_PIN_2)  \
            PINout(port, 2) = val;  \
        else if (pin & GPIO_PIN_3)  \
            PINout(port, 3) = val;  \
        else if (pin & GPIO_PIN_4)  \
            PINout(port, 4) = val;  \
        else if (pin & GPIO_PIN_5)  \
            PINout(port, 5) = val;  \
        else if (pin & GPIO_PIN_6)  \
            PINout(port, 6) = val;  \
        else if (pin & GPIO_PIN_7)  \
            PINout(port, 7) = val;  \
        else if (pin & GPIO_PIN_8)  \
            PINout(port, 8) = val;  \
        else if (pin & GPIO_PIN_9)  \
            PINout(port, 9) = val;  \
        else if (pin & GPIO_PIN_10) \
            PINout(port, 10) = val; \
        else if (pin & GPIO_PIN_11) \
            PINout(port, 11) = val; \
        else if (pin & GPIO_PIN_12) \
            PINout(port, 12) = val; \
        else if (pin & GPIO_PIN_13) \
            PINout(port, 13) = val; \
        else if (pin & GPIO_PIN_14) \
            PINout(port, 14) = val; \
        else if (pin & GPIO_PIN_15) \
            PINout(port, 15) = val; \
    } while (0)

#define GPIOout_num(port, n, val)   \
    do {                            \
        if (n == 0)                 \
            PINout(port, 0) = val;  \
        else if (n == 1)            \
            PINout(port, 1) = val;  \
        else if (n == 2)            \
            PINout(port, 2) = val;  \
        else if (n == 3)            \
            PINout(port, 3) = val;  \
        else if (n == 4)            \
            PINout(port, 4) = val;  \
        else if (n == 5)            \
            PINout(port, 5) = val;  \
        else if (n == 6)            \
            PINout(port, 6) = val;  \
        else if (n == 7)            \
            PINout(port, 7) = val;  \
        else if (n == 8)            \
            PINout(port, 8) = val;  \
        else if (n == 9)            \
            PINout(port, 9) = val;  \
        else if (n == 10)           \
            PINout(port, 10) = val; \
        else if (n == 11)           \
            PINout(port, 11) = val; \
        else if (n == 12)           \
            PINout(port, 12) = val; \
        else if (n == 13)           \
            PINout(port, 13) = val; \
        else if (n == 14)           \
            PINout(port, 14) = val; \
        else if (n == 15)           \
            PINout(port, 15) = val; \
    } while (0)

#define GPIOout_char(port, n, val)  \
    do {                            \
        if (n == '0')               \
            PINout(port, 0) = val;  \
        else if (n == '1')          \
            PINout(port, 1) = val;  \
        else if (n == '2')          \
            PINout(port, 2) = val;  \
        else if (n == '3')          \
            PINout(port, 3) = val;  \
        else if (n == '4')          \
            PINout(port, 4) = val;  \
        else if (n == '5')          \
            PINout(port, 5) = val;  \
        else if (n == '6')          \
            PINout(port, 6) = val;  \
        else if (n == '7')          \
            PINout(port, 7) = val;  \
        else if (n == '8')          \
            PINout(port, 8) = val;  \
        else if (n == '9')          \
            PINout(port, 9) = val;  \
        else if (n == '10')         \
            PINout(port, 10) = val; \
        else if (n == '11')         \
            PINout(port, 11) = val; \
        else if (n == '12')         \
            PINout(port, 12) = val; \
        else if (n == '13')         \
            PINout(port, 13) = val; \
        else if (n == '14')         \
            PINout(port, 14) = val; \
        else if (n == '15')         \
            PINout(port, 15) = val; \
    } while (0)

/* get pin def:portNo<<4 | pinNo */
#define pinNo(x)                                                                             \
    (x & GPIO_PIN_0                                                                          \
         ? 0                                                                                 \
         : x & GPIO_PIN_1                                                                    \
               ? 1                                                                           \
               : x & GPIO_PIN_2                                                              \
                     ? 2                                                                     \
                     : x & GPIO_PIN_3                                                        \
                           ? 3                                                               \
                           : x & GPIO_PIN_4                                                  \
                                 ? 4                                                         \
                                 : x & GPIO_PIN_5                                            \
                                       ? 5                                                   \
                                       : x & GPIO_PIN_6                                      \
                                             ? 6                                             \
                                             : x & GPIO_PIN_7                                \
                                                   ? 7                                       \
                                                   : x & GPIO_PIN_8                          \
                                                         ? 8                                 \
                                                         : x & GPIO_PIN_9                    \
                                                               ? 9                           \
                                                               : x & GPIO_PIN_10             \
                                                                     ? 10                    \
                                                                     : x & GPIO_PIN_11       \
                                                                           ? 11              \
                                                                           : x & GPIO_PIN_12 \
                                                                                 ? 12        \
                                                                                 : x & GPIO_PIN_13 ? 13 : x & GPIO_PIN_14 ? 14 : 15)

#define portNo(x) (x == GPIOA ? 0 : x == GPIOB ? 1 : x == GPIOC ? 2 : x == GPIOD ? 3 : x == GPIOE ? 4 : x == GPIOF ? 5 : 6)

#define GET_PINno(port, pin) (portNo(port) * 16 + pinNo(pin))

#ifdef __cplusplus
}
#endif

#endif /* __DRV_GPIO_H__ */
