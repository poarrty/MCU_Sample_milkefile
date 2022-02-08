/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: bsp_adc.c
 * @Author: Chen Yuliang(chenyuliang@cvte.com)
 * @Version: V1.0
 * @Date: 2021-11-10
 * @Description: board support package: adc operation
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/
#include "bsp_adc.h"
#define LOG_TAG "bsp_adc"
#include "elog.h"
#ifdef USED_BSP_ADC

#define PUSH_ROD_MOTOR_1_HANDLE &hadc2
#define PUSH_ROD_MOTOR_2_HANDLE &hadc2

#define PUSH_ROD_MOTOR_1_CHANNEL ADC_CHANNEL_6
#define PUSH_ROD_MOTOR_2_CHANNEL ADC_CHANNEL_8

// extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

int adc_read(uint8_t adc_obj_id, uint16_t *val) {
    uint32_t               ADC_VAL = 0;
    ADC_HandleTypeDef *    handler;
    ADC_ChannelConfTypeDef sConfig = {0};

    switch (adc_obj_id) {
        case ID_PUSH_ROD_MOTOR_1_ADC:
            handler         = PUSH_ROD_MOTOR_1_HANDLE;
            sConfig.Channel = PUSH_ROD_MOTOR_1_CHANNEL;
            break;

        case ID_PUSH_ROD_MOTOR_2_ADC:
            handler         = PUSH_ROD_MOTOR_2_HANDLE;
            sConfig.Channel = PUSH_ROD_MOTOR_2_CHANNEL;
            break;

        default:
            break;
    }

    sConfig.Rank         = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    HAL_ADC_ConfigChannel(handler, &sConfig);

    /* start ADC */
    HAL_ADC_Start(handler);

    /* Wait for the ADC to convert */
    HAL_ADC_PollForConversion(handler, 100);

    /* get ADC value */
    ADC_VAL = (uint32_t) HAL_ADC_GetValue(handler);

    HAL_ADC_Stop(handler);

    *val = (ADC_VAL & 0x0fff);
    return RT_EOK;
}

int adc_enable(uint8_t adc_obj_id) {
    ADC_HandleTypeDef *handler;

    switch (adc_obj_id) {
        case ID_PUSH_ROD_MOTOR_1_ADC:
            handler = PUSH_ROD_MOTOR_1_HANDLE;
            break;

        case ID_PUSH_ROD_MOTOR_2_ADC:
            handler = PUSH_ROD_MOTOR_2_HANDLE;
            break;

        default:
            break;
    }

    __HAL_ADC_ENABLE(handler);

    return HAL_OK;
}

int adc_disable(uint8_t adc_obj_id) {
    ADC_HandleTypeDef *handler;

    switch (adc_obj_id) {
        case ID_PUSH_ROD_MOTOR_1_ADC:
            handler = PUSH_ROD_MOTOR_1_HANDLE;
            break;

        case ID_PUSH_ROD_MOTOR_2_ADC:
            handler = PUSH_ROD_MOTOR_2_HANDLE;
            break;

        default:
            break;
    }

    __HAL_ADC_DISABLE(handler);

    return RT_EOK;
}

#ifdef RT_USING_FINSH
static void adc_debug(int argc, char **argv) {
    uint16_t value         = 0;
    uint32_t convert_value = 0;
    uint8_t  channal       = 0;

    if (argc < 2) {
        log_w("%s input parm error\r\n", __FUNCTION__);
        return;
    }

    channal = atoi(argv[1]);

    if (adc_enable(channal) != RT_EOK) {
        log_w("%s enable error\r\n", __FUNCTION__);
    }

    if (adc_read(channal, &value) != RT_EOK) {
        log_w("%s read error\r\n", __FUNCTION__);
    }

    convert_value = value * REFER_VOLTAGE / CONVERT_BITS;
    log_d("the vol: %d ,%ld mv \n", value, convert_value);
    return;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), adc_debug, adc_debug, adc read debug);
#endif

#endif