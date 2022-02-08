#include "uxr/client/transport.h"

#include <rmw_microxrcedds_c/config.h>

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmsis_os.h"
#include "log.h"

#define LOG_TAG "microros_transports"
#include "elog.h"

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM

// --- micro-ROS Transports ---
#define UART_DMA_BUFFER_SIZE 2048

static uint8_t  dma_buffer[UART_DMA_BUFFER_SIZE];
static size_t   dma_head = 0, dma_tail = 0;
osSemaphoreId_t microros_tx_empty_id = NULL;

bool freertos_serial_open(struct uxrCustomTransport *transport) {
    if (microros_tx_empty_id == NULL) {
        microros_tx_empty_id = osSemaphoreNew(1, 0, NULL);
        if (microros_tx_empty_id == NULL) {
            log_e("microros_tx_empty_id Semaphore new error");
        }
    }

    UART_HandleTypeDef *uart = (UART_HandleTypeDef *) transport->args;
    HAL_UART_Receive_DMA(uart, dma_buffer, UART_DMA_BUFFER_SIZE);
    return true;
}

bool freertos_serial_close(struct uxrCustomTransport *transport) {
    /*connect失败时会调用两次close函数，避免连续两次释放信号量*/
    if (microros_tx_empty_id != NULL) {
        if (osSemaphoreDelete(microros_tx_empty_id) == osOK) {
            microros_tx_empty_id = NULL;
        } else {
            log_e("microros_tx_empty_id Semaphore delete error\r\n");
        }
    }

    UART_HandleTypeDef *uart = (UART_HandleTypeDef *) transport->args;
    HAL_UART_DMAStop(uart);
    return true;
}

size_t freertos_serial_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *err) {
    UART_HandleTypeDef *uart = (UART_HandleTypeDef *) transport->args;
    HAL_StatusTypeDef   ret;

    if (uart->gState == HAL_UART_STATE_READY) {
        ret = HAL_UART_Transmit_DMA(uart, (uint8_t *) buf, len);
        while (ret == HAL_OK && uart->gState != HAL_UART_STATE_READY) {
            osSemaphoreAcquire(microros_tx_empty_id, 1);
            // osDelay(1);
        }

        return (ret == HAL_OK) ? len : 0;
    } else {
        return 0;
    }
}

size_t freertos_serial_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *err) {
    UART_HandleTypeDef *uart = (UART_HandleTypeDef *) transport->args;

    __disable_irq();
    dma_tail = UART_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(uart->hdmarx);
    __enable_irq();

    size_t wrote = 0;
    while ((dma_head != dma_tail) && (wrote < len)) {
        buf[wrote] = dma_buffer[dma_head];
        dma_head   = (dma_head + 1) % UART_DMA_BUFFER_SIZE;
        wrote++;
    }

    return wrote;
}

#endif  // RMW_UXRCE_TRANSPORT_CUSTOM
