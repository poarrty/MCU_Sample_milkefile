#include "main.h"
#include "ymodem.h"
#include "rtt_fal.h"
#include "fota_port.h"

extern UART_HandleTypeDef FOTA_YMODEM_HUART;
uint8_t temp_data;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == FOTA_YMODEM_UART) {
        HAL_UART_Receive_IT(huart, (uint8_t *) &temp_data, 1);
        ymodem_data_recv(&temp_data, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    /*在 RXNE = 1 的情况下，当移位寄存器中当前正在接收的字准备好传输到 RDR
     *寄存器时，该 位由硬件置 1。如果 USART_CR1 寄存器中 RXNEIE =
     *1，则会生成中断。该位由软件序列清 零（读入 USART_SR 寄存器，然后读入
     *USART_DR 寄存器）。 0：无上溢错误 1：检测到上溢错误*/
    if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE) {
        /*清除硬件ORE标志位*/
        __HAL_UART_CLEAR_OREFLAG(huart);
        /*DMA模式可以通过HAL_UART_Receive_DMA再次开启接受，会丢失一个字节*/
        if (huart->hdmarx != NULL) {
            HAL_UART_Receive_DMA(huart, (uint8_t *) &temp_data, 1);
        }
        /*中断模式可以通过HAL_UART_Receive_IT再次开启接受，会丢失一个字节*/
        else {
            HAL_UART_Receive_IT(huart, (uint8_t *) &temp_data, 1);
        }
    }
}

/**
 * @brief   获取毫秒时间戳.
 * @param   void
 * @return  时间戳
 */
y_uint32_t y_get_tick(void) {
    return HAL_GetTick();
}

/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int y_transmit_ch(y_uint8_t ch) {
    Y_UNUSED(ch);
    HAL_UART_Transmit(&FOTA_YMODEM_HUART, &ch, 1, 100);
    return 0;
}

/**
 * @brief   文件名和大小接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_nanme_size_callback(void *ptr, char *file_name,
                                y_uint32_t file_size) {
    Y_UNUSED(ptr);
    Y_UNUSED(file_name);
    Y_UNUSED(file_size);

    /* 用户应该在外部实现这个函数 */
    const struct fal_partition *part;
    part = fal_partition_find(FOTA_FM_PART_NAME);

    if (part == NULL) {
        printf("Partition[%s] not found.\r\n", FOTA_FM_PART_NAME);
        return -1;
    }

    if (fal_partition_erase_all(part) < 0) {
        printf("Erase Partition[%s] failed.\r\n", FOTA_FM_PART_NAME);
        return -1;
    }

    return 0;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_data: 文件数据.
 * @param   w_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, y_uint32_t w_size) {
    Y_UNUSED(ptr);
    Y_UNUSED(file_data);
    Y_UNUSED(w_size);

    /* 用户应该在外部实现这个函数 */
    static uint32_t writen_size = 0;
    const struct fal_partition *part;
    part = fal_partition_find(FOTA_FM_PART_NAME);

    if (part == NULL) {
        printf("Partition[%s] not found.\r\n", FOTA_FM_PART_NAME);
        return -1;
    }

    if (fal_partition_write(part, writen_size, (const uint8_t *) file_data,
                            w_size) < 0) {
        printf("Write Partition[%s] failed.\r\n", FOTA_FM_PART_NAME);
        return -1;
    }

    writen_size += w_size;

    return 0;
}

/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr) {
    Y_UNUSED(ptr);

    /* 用户应该在外部实现这个函数 */
    // __set_FAULTMASK(1);  // 关闭所有中断
    // NVIC_SystemReset();  // 复位

    return 0;
}