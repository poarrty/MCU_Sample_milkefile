/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: fal_common.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: Functional abstraction layer: common called
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "fal_common.h"

#define LOG_TAG "fal_common"
#include "elog.h"
int utils_truncate_number_uint(uint32_t *number, uint32_t min, uint32_t max) {
    int did_trunc = 0;

    if (*number > max) {
        *number   = max;
        did_trunc = 1;
    } else if (*number < min) {
        *number   = min;
        did_trunc = 1;
    }

    return did_trunc;
}

void utils_step_towards_uint(uint32_t *value, uint32_t goal, uint32_t step) {
    if (*value < goal) {
        if ((*value + step) < goal) {
            *value += step;
        } else {
            *value = goal;
        }
    } else if (*value > goal) {
        if (*value > (goal + step)) {
            *value -= step;
        } else {
            *value = goal;
        }
    } else {
        *value = goal;
    }
}

int utils_truncate_number_int(int *number, int min, int max) {
    int did_trunc = 0;

    if (*number > max) {
        *number   = max;
        did_trunc = 1;
    } else if (*number < min) {
        *number   = min;
        did_trunc = 1;
    }

    return did_trunc;
}

void utils_step_towards_int(int *value, int goal, int step) {
    if (*value < goal) {
        if ((*value + step) < goal) {
            *value += step;
        } else {
            *value = goal;
        }
    } else if (*value > goal) {
        if ((*value - step) > goal) {
            *value -= step;
        } else {
            *value = goal;
        }
    }
}

int utils_truncate_number_float(float *number, float min, float max) {
    int did_trunc = 0;

    if (*number > max) {
        *number   = max;
        did_trunc = 1;
    } else if (*number < min) {
        *number   = min;
        did_trunc = 1;
    }

    return did_trunc;
}

void utils_step_towards_float(float *value, float goal, float step) {
    if (*value < goal) {
        if ((*value + step) < goal) {
            *value += step;
        } else {
            *value = goal;
        }
    } else if (*value > goal) {
        if ((*value - step) > goal) {
            *value -= step;
        } else {
            *value = goal;
        }
    }
}

// remove del_len head and tail values and get the average value
uint16_t average_get(uint16_t *in_data, uint8_t len, uint8_t del_len) {
    uint32_t temp_val = 0;
    uint8_t  i        = 0;

    if (len > del_len) {
        for (i = del_len; i < len - del_len; i++) {
            temp_val += in_data[i];
        }

        temp_val = temp_val / (len - 2 * del_len);
    }

    return temp_val;
}

// bubble sort
void bubble_sort(uint16_t *data, uint8_t n) {
    uint8_t  i    = 0;
    uint8_t  j    = 0;
    uint16_t temp = 0;
    int      flag;

    if (data != NULL) {
        for (i = 0; i < n - 1; i++) {
            flag = 1;

            for (j = 0; j < n - 1 - i; j++) {
                if (data[j] > data[j + 1]) {
                    temp        = data[j];
                    data[j]     = data[j + 1];
                    data[j + 1] = temp;
                    flag        = 0;
                }
            }

            if (flag == 1) {
                break;
            }
        }
    }
}
