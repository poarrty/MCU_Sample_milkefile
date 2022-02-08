#include "bsp_gpio_in.h"
#include "bsp_gpio_out.h"
#include "fal_key.h"
#include "string.h"
#include "cmsis_os.h"
#include "main.h"
#include "button.h"
#include "gpio.h"
#include "drv_pca9539.h"
#include "bsp_iwdg.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#include "bsp_cputime.h"
#include "shell.h"
#include "log.h"
#include "i2c.h"

#define LOG_TAG "fal_key"
#include "elog.h"

#define THREAD_YIELD_TIME      17
enum { WASTE_WATER_HIGH = 0, WASTE_WATER_LOW, CLEAN_WATER_HIGH, CLEAN_WATER_LOW, WATER_LEVEL_SENSOR_MAX };

button_t        water_level_sensor[WATER_LEVEL_SENSOR_MAX];
uint8_t         water_level_status[WATER_LEVEL_SENSOR_MAX] = {0};
static button_t func_button[BUTTON_DEFAULT_MAX]            = {0};              //存储多个按键
uint8_t         func_button_status[BUTTON_DEFAULT_MAX]     = {0, 0, 1, 1, 0};  //默认自动，自动手动模式开启
static void     return_home_button_init(void);
static void     auto_manual_button_init(void);
static void     wash_floor_button_init(void);
static void     dust_push_button_init(void);

uint32_t water_level_status_get(uint8_t id) {
    uint32_t status = 0;
    if (id >= 2)
        return 0;

    //水位传感器高异常
    if (water_level_status[2 * id]) {
        //水位传感低异常
        if (water_level_status[2 * id + 1]) {
            status = WATER_ERROR_HIGH;
        } else {
            //满水
            status = WATER_ERROR_HIGH;
        }
    } else {
        //水位传感器低异常，缺水
        if (water_level_status[2 * id + 1]) {
            status = WATER_ERROR_LOW;
        } else {
            //正常
            status = WATER_OK;
        }
    }
    return status;
}

static int waste_water_levle_high_status_read(void) {
    gpio_out_off(ID_IO_HUB_CTL_0);
    gpio_out_on(ID_IO_HUB_CTL_1);
    return gpio_in_status_read(ID_WATER_LEVEL);
}

static void waste_water_levle_high_press_handler(void *param) {
    //污水箱高异常
    water_level_status[WASTE_WATER_HIGH] = 1;
    log_d("waste water high status error");
}

static void waste_water_levle_high_release_handler(void *param) {
    //污水箱高正常
    water_level_status[WASTE_WATER_HIGH] = 0;
    log_d("waste water high status ok");
}

static int waste_water_levle_low_status_read(void) {
    gpio_out_on(ID_IO_HUB_CTL_0);
    gpio_out_on(ID_IO_HUB_CTL_1);
    return gpio_in_status_read(ID_WATER_LEVEL);
}

static void waste_water_levle_low_press_handler(void *param) {
    //污水箱低异常
    water_level_status[WASTE_WATER_LOW] = 1;
    log_d("waste water low status error");
}

static void waste_water_levle_low_release_handler(void *param) {
    //污水箱低正常
    water_level_status[WASTE_WATER_LOW] = 0;
    log_d("waste water low status ok");
}

static int clean_water_level_high_status_read(void) {
    gpio_out_off(ID_IO_HUB_CTL_0);
    gpio_out_off(ID_IO_HUB_CTL_1);
    return gpio_in_status_read(ID_WATER_LEVEL);
}

static void clean_water_level_high_press_handler(void *param) {
    //清水箱高异常
    water_level_status[CLEAN_WATER_HIGH] = 1;
    log_d("clean water high status error");
}

static void clean_water_level_high_release_handler(void *param) {
    //清水箱低正常
    water_level_status[CLEAN_WATER_HIGH] = 0;
    log_d("clean water high status ok");
}

static int clean_water_level_low_status_read(void) {
    gpio_out_on(ID_IO_HUB_CTL_0);
    gpio_out_off(ID_IO_HUB_CTL_1);
    return gpio_in_status_read(ID_WATER_LEVEL);
}

static void clean_water_level_low_press_handler(void *param) {
    //清水箱低异常
    water_level_status[CLEAN_WATER_LOW] = 1;
    log_d("clean water low status error");
}

static void clean_water_level_low_release_handler(void *param) {
    //清水箱低正常
    water_level_status[CLEAN_WATER_LOW] = 0;
    log_d("clean water low status ok");
}

static void waste_water_level_sensor_init(void) {
    button_create("waste water", &water_level_sensor[WASTE_WATER_HIGH], waste_water_levle_high_status_read, PIN_HIGH);
    button_attach(&water_level_sensor[WASTE_WATER_HIGH], BUTTON_LONG, waste_water_levle_high_press_handler);
    button_attach(&water_level_sensor[WASTE_WATER_HIGH], BUTTON_UP, waste_water_levle_high_release_handler);
    button_set_debounce_time(&water_level_sensor[WASTE_WATER_HIGH], 200);

    button_create("waste water", &water_level_sensor[WASTE_WATER_LOW], waste_water_levle_low_status_read, PIN_LOW);
    button_attach(&water_level_sensor[WASTE_WATER_LOW], BUTTON_LONG, waste_water_levle_low_press_handler);
    button_attach(&water_level_sensor[WASTE_WATER_LOW], BUTTON_UP, waste_water_levle_low_release_handler);
    button_set_debounce_time(&water_level_sensor[WASTE_WATER_LOW], 200);
}

static void clean_water_level_sensor_init(void) {
    button_create("c_water_h", &water_level_sensor[CLEAN_WATER_HIGH], clean_water_level_high_status_read, PIN_HIGH);
    button_attach(&water_level_sensor[CLEAN_WATER_HIGH], BUTTON_LONG, clean_water_level_high_press_handler);
    button_attach(&water_level_sensor[CLEAN_WATER_HIGH], BUTTON_UP, clean_water_level_high_release_handler);
    button_set_debounce_time(&water_level_sensor[CLEAN_WATER_HIGH], 200);

    button_create("c_water_l", &water_level_sensor[CLEAN_WATER_LOW], clean_water_level_low_status_read, PIN_LOW);
    button_attach(&water_level_sensor[CLEAN_WATER_LOW], BUTTON_LONG, clean_water_level_low_press_handler);
    button_attach(&water_level_sensor[CLEAN_WATER_LOW], BUTTON_UP, clean_water_level_low_release_handler);
    button_set_debounce_time(&water_level_sensor[CLEAN_WATER_LOW], 200);
}

void show_water_level_info(void) {
    log_i("waste io high: %d", waste_water_levle_high_status_read());
    log_i("waste io low: %d", waste_water_levle_low_status_read());
    log_i("clean io high :%d", clean_water_level_high_status_read());
    log_i("clean io low :%d", clean_water_level_low_status_read());
    log_i("waste water :%ld", water_level_status_get(0));
    log_i("clean water :%ld", water_level_status_get(1));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, show_water_level_info,
                 show_water_level_info, water level info show);

uint32_t function_button_status_get(uint8_t button_id) {
    if (button_id >= sizeof(func_button_status) / sizeof(func_button_status[0])) {
        log_e("%s input args error", __FUNCTION__);
        return 0;
    }

    // LOG_DEBUG("fbs get :%d, %d", button_id, func_button_status[button_id]);
    return func_button_status[button_id];
}

#ifdef USED_FUNCTION_KEY
/*返航按键*/
static int return_home_button_status_read(void) {
    return gpio_in_status_read(ID_KEY_RETURN_HOME);
}

void button_and_led_linkage_update(uint8_t button_id) {
#if 1
    switch (button_id) {
        case RETURN_HOME_BUTTON:
            if (func_button_status[RETURN_HOME_BUTTON]) {
                gpio_out_on(ID_KEY_RETURN_HOME_LED);
            } else {
                gpio_out_off(ID_KEY_RETURN_HOME_LED);
            }
            break;
        case AUTO_MANUAL_BUTTON:
            if (func_button_status[AUTO_MANUAL_BUTTON]) {
                gpio_out_off(ID_KEY_AUTO_MODE_LED);
                gpio_out_on(ID_KEY_MANUAL_MODE_LED);
            } else {
                gpio_out_on(ID_KEY_AUTO_MODE_LED);
                gpio_out_off(ID_KEY_MANUAL_MODE_LED);
            }
            break;
        case WASH_FLOOR_BUTTON:
            if (func_button_status[WASH_FLOOR_BUTTON]) {
                gpio_out_on(ID_KEY_WASH_FLOOR_BLUE_LED);
                gpio_out_on(ID_KEY_WASH_FLOOR_RED_LED);
            } else {
                gpio_out_off(ID_KEY_WASH_FLOOR_BLUE_LED);
                gpio_out_off(ID_KEY_WASH_FLOOR_RED_LED);
            }
            break;
        case DUST_PUSH_BUTTON:
            if (func_button_status[DUST_PUSH_BUTTON]) {
                gpio_out_on(ID_KEY_DUST_PUSH_BLUE_LED);
                gpio_out_on(ID_KEY_DUST_PUSH_RED_LED);
            } else {
                gpio_out_off(ID_KEY_DUST_PUSH_BLUE_LED);
                gpio_out_off(ID_KEY_DUST_PUSH_RED_LED);
            }
            break;
        default:
            break;
    }
#endif
}

static void return_home_button_press_handler(void *param) {
    // struct key_status_info key_info = {0};

    taskENTER_CRITICAL();
    func_button_status[RETURN_HOME_BUTTON] = !func_button_status[RETURN_HOME_BUTTON];
    func_button_status[AUTO_MANUAL_BUTTON] = 0;  //自动手动
    func_button_status[WASH_FLOOR_BUTTON]  = 1;  //洗地
    func_button_status[DUST_PUSH_BUTTON]   = 1;  //尘推
    taskEXIT_CRITICAL();

    /* 按键LED灯联动 */
    button_and_led_linkage_update(RETURN_HOME_BUTTON);
    button_and_led_linkage_update(AUTO_MANUAL_BUTTON);
    button_and_led_linkage_update(WASH_FLOOR_BUTTON);
    button_and_led_linkage_update(DUST_PUSH_BUTTON);
    log_d("return home : %s", (func_button_status[RETURN_HOME_BUTTON] ? "yes" : "no"));
}

static void return_home_button_init(void) {
    button_create("return home", &func_button[RETURN_HOME_BUTTON], return_home_button_status_read, PIN_LOW);
    button_attach(&func_button[RETURN_HOME_BUTTON], BUTTON_DOWM, return_home_button_press_handler);
}

/*自动手动按键*/
static int auto_manual_button_status_read(void) {
    return gpio_in_status_read(ID_KEY_AUTO_MANUAL_MODE);
}

static void auto_manual_button_press_handler(void *param) {
    // struct key_status_info key_info = {0};

    taskENTER_CRITICAL();
    func_button_status[AUTO_MANUAL_BUTTON] = !func_button_status[AUTO_MANUAL_BUTTON];
    if (func_button_status[AUTO_MANUAL_BUTTON]) {
        func_button_status[RETURN_HOME_BUTTON] = 0;  //返航
        func_button_status[WASH_FLOOR_BUTTON]  = 0;  //洗地
        func_button_status[DUST_PUSH_BUTTON]   = 0;  //尘推
    } else {
        func_button_status[RETURN_HOME_BUTTON] = 0;  //返航
        func_button_status[WASH_FLOOR_BUTTON]  = 1;  //洗地
        func_button_status[DUST_PUSH_BUTTON]   = 1;  //尘推
    }
    taskEXIT_CRITICAL();

    /* 按键LED灯联动 */
    button_and_led_linkage_update(RETURN_HOME_BUTTON);
    button_and_led_linkage_update(AUTO_MANUAL_BUTTON);
    button_and_led_linkage_update(WASH_FLOOR_BUTTON);
    button_and_led_linkage_update(DUST_PUSH_BUTTON);
    log_d("auto manual mode : %s", (func_button_status[AUTO_MANUAL_BUTTON] ? "manual" : "auto"));
}
static void auto_manual_button_init(void) {
    button_create("auto manual", &func_button[AUTO_MANUAL_BUTTON], auto_manual_button_status_read, PIN_LOW);
    button_attach(&func_button[AUTO_MANUAL_BUTTON], BUTTON_DOWM, auto_manual_button_press_handler);
}

/*洗地按键*/
static int wash_floor_button_status_read(void) {
    return gpio_in_status_read(ID_KEY_WASH_FLOOR);
}
static void wash_floor_button_press_handler(void *param) {
    if (func_button_status[AUTO_MANUAL_BUTTON]) {
        taskENTER_CRITICAL();
        func_button_status[WASH_FLOOR_BUTTON] = !func_button_status[WASH_FLOOR_BUTTON];
        /* 如果洗地按键有效，当尘推处于有效时，应更改洗地状态为无效状态*/
        if (func_button_status[DUST_PUSH_BUTTON] && func_button_status[WASH_FLOOR_BUTTON]) {
            func_button_status[DUST_PUSH_BUTTON] = !func_button_status[DUST_PUSH_BUTTON];
        }
        taskEXIT_CRITICAL();

        /* 按键LED灯联动 */
        button_and_led_linkage_update(WASH_FLOOR_BUTTON);
        button_and_led_linkage_update(DUST_PUSH_BUTTON);
        log_d("clean mode : %s", (func_button_status[WASH_FLOOR_BUTTON] ? "yes" : "not"));
    }
}

static void wash_floor_button_init(void) {
    button_create("wash floor", &func_button[WASH_FLOOR_BUTTON], wash_floor_button_status_read, PIN_LOW);
    button_attach(&func_button[WASH_FLOOR_BUTTON], BUTTON_DOWM, wash_floor_button_press_handler);
}

/*尘推按键*/
static int dust_push_button_status_read(void) {
    return gpio_in_status_read(ID_KEY_DUST_PUSH);
}

static void dust_push_button_press_handler(void *param) {
    if (func_button_status[AUTO_MANUAL_BUTTON]) {
        taskENTER_CRITICAL();
        func_button_status[DUST_PUSH_BUTTON] = !func_button_status[DUST_PUSH_BUTTON];
        /* 如果尘推按键有效，当洗地处于有效时，应更改洗地状态为无效状态*/
        if (func_button_status[WASH_FLOOR_BUTTON] && func_button_status[DUST_PUSH_BUTTON]) {
            func_button_status[WASH_FLOOR_BUTTON] = !func_button_status[WASH_FLOOR_BUTTON];
        }
        taskEXIT_CRITICAL();
        /* 按键LED灯联动 */
        button_and_led_linkage_update(WASH_FLOOR_BUTTON);
        button_and_led_linkage_update(DUST_PUSH_BUTTON);
        log_d("dust push: %s", (func_button_status[DUST_PUSH_BUTTON] ? "yes" : "no"));
    }
}

static void dust_push_button_init(void) {
    button_create("dust push", &func_button[DUST_PUSH_BUTTON], dust_push_button_status_read, PIN_LOW);
    button_attach(&func_button[DUST_PUSH_BUTTON], BUTTON_DOWM, dust_push_button_press_handler);
}

static int strainer_status_read(void) {
    return gpio_in_status_read(ID_KEY_STRAINER);
}

static void strainer_press_handler(void *param) {
    func_button_status[STRAINER_BUTTON] = 1;
    log_d("strainer ok");
}

static void strainer_release_handler(void *param) {
    func_button_status[STRAINER_BUTTON] = 0;
    log_d("strainer error");
}

static void strainer_button_init(void) {
    /* 过滤网检测 */
    button_create("strainer", &func_button[STRAINER_BUTTON], strainer_status_read, PIN_LOW);
    button_attach(&func_button[STRAINER_BUTTON], BUTTON_LONG, strainer_press_handler);
    button_attach(&func_button[STRAINER_BUTTON], BUTTON_UP, strainer_release_handler);
}

uint8_t function_button_status_set(uint8_t button_id, uint8_t value) {
    if (button_id > BUTTON_DEFAULT_MAX)
        return 0xff;

    if (value)
        value = 1;

    if (func_button_status[button_id] != value) {
        if (func_button[button_id].callBack_function[BUTTON_DOWM] != NULL)
            func_button[button_id].callBack_function[BUTTON_DOWM](NULL);
    }

    log_d("fbs set :%d, %d", button_id, func_button_status[button_id]);
    return value;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, function_button_status_set,
                 function_button_status_set, function button status set);

uint8_t auto_manual_button_status_set(uint8_t data) {
    return function_button_status_set(AUTO_MANUAL_BUTTON, data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, auto_manual_button_status_set,
                 auto_manual_button_status_set, auto and manual key status set);
/*按键任务入口函数*/
void task_key_run(void *argument) {
    uint8_t last_water_level_stauts[2] = {0};

    waste_water_level_sensor_init();
    clean_water_level_sensor_init();

#if 1
    /*新按键*/
    return_home_button_init();
    auto_manual_button_init();
    wash_floor_button_init();
    dust_push_button_init();
    strainer_button_init();

    osDelay(200);
    /* 按键LED灯联动 */
    button_and_led_linkage_update(RETURN_HOME_BUTTON);
    button_and_led_linkage_update(AUTO_MANUAL_BUTTON);
    button_and_led_linkage_update(WASH_FLOOR_BUTTON);
    button_and_led_linkage_update(DUST_PUSH_BUTTON);
#endif
    uint8_t printf_div = 0;

    while (1) {
        osDelay(THREAD_YIELD_TIME);
        if (get_i2c_error_status()) {
            log_d("****** key again init **********");
            /* 给按键板断电 */
            gpio_out_off(ID_KEY_BOARD_POWER_EN);
            osDelay(20);

            /* 重新初始化i2c*/
            MX_I2C2_Init();

            /* 给按键板供电 */
            gpio_out_on(ID_KEY_BOARD_POWER_EN);
            osDelay(20);

            /* 按键板IO初始化 */
            gpio_out_init_for_pca9539();
            gpio_in_init_for_pca9539();

            /* 按键LED灯联动 */
            button_and_led_linkage_update(RETURN_HOME_BUTTON);
            button_and_led_linkage_update(AUTO_MANUAL_BUTTON);
            button_and_led_linkage_update(WASH_FLOOR_BUTTON);
            button_and_led_linkage_update(DUST_PUSH_BUTTON);

            clear_i2c_error_status();

            /* 处理水位状态 */
            for (uint8_t i = 0; i < 2; i++) {
                if (i == 1) {
                    if (water_level_status_get(i) == WATER_ERROR_LOW) {
                        gpio_out_on(ID_CLEAN_WATER_LEVEL_ERROR_LED);
                    } else {
                        gpio_out_off(ID_CLEAN_WATER_LEVEL_ERROR_LED);
                    }
                } else if (i == 0) {
                    if (water_level_status_get(i) == WATER_ERROR_HIGH) {
                        gpio_out_on(ID_WASTE_WATER_LEVEL_ERROR_LED);
                    } else {
                        gpio_out_off(ID_WASTE_WATER_LEVEL_ERROR_LED);
                    }
                }
            }
        }
        // uint32_t start_time = clock_cpu_gettime();
        button_process();

        /* 处理水位状态 */
        for (uint8_t i = 0; i < 2; i++) {
            if (last_water_level_stauts[i] != water_level_status_get(i)) {
                last_water_level_stauts[i] = water_level_status_get(i);
                if (i == 1) {
                    if (water_level_status_get(i) == WATER_ERROR_LOW) {
                        gpio_out_on(ID_CLEAN_WATER_LEVEL_ERROR_LED);
                    } else {
                        gpio_out_off(ID_CLEAN_WATER_LEVEL_ERROR_LED);
                    }
                } else if (i == 0) {
                    if (water_level_status_get(i) == WATER_ERROR_HIGH) {
                        gpio_out_on(ID_WASTE_WATER_LEVEL_ERROR_LED);
                    } else {
                        gpio_out_off(ID_WASTE_WATER_LEVEL_ERROR_LED);
                    }
                }
            }
        }
        if (printf_div++ % 1000 == 0) {
             //log_d("used time :%ld us\r\n", clock_cpu_microsecond_diff(start_time, clock_cpu_gettime()));
        }
    }
}

#endif
