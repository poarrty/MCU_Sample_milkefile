/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: rtconfig.h
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: global define
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __EXTERN_INIT_H__
#define __EXTERN_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*init_fn_t)(void);

/* board init routines will be called in board_init() function */
#define INIT_BOARD__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("board"))) = fn

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define INIT_PREV__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("prev"))) = fn
/* device initialization */
#define INIT_DEVICE__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("device"))) = fn
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("component"))) = fn
/* environment initialization (mount disk, ...) */
#define INIT_ENV__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("env"))) = fn
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP__EXPORT(fn) const init_fn_t _init_##fn __attribute__((section("app"))) = fn

#define INIT__EXPORT(s, e)             \
    do {                               \
        uint32_t *p = (uint32_t *) &s; \
        while (p != (uint32_t *) &e)   \
            ((init_fn_t) *p++)();      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */