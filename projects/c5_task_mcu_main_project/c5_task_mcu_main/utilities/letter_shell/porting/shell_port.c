/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell_port.h"
#include "log.h"
#include "shell.h"

#define LOG_TAG "shell_port"
#include "elog.h"

#define shell_fifo_err    2
#define shell_fifo_full   1
#define shell_fifo_normal 0
#define shell_fifo_null   -1

/* shell logic */
#define pointer_set_limit(sp, ep, p) ((p == ep) ? p = sp : p++)
/* shell instance */
#define shell_fifo_instance       shell_fifo
#define shell_fifo_length         (shell_fifo_instance.capacity)
#define shell_fifo_current_length (shell_fifo_instance.size)
#define shell_fifo_start          (shell_fifo_instance.data)
#define shell_fifo_end            (shell_fifo_instance.data + shell_fifo_length - 1)
#define shell_fifo_in_p           (shell_fifo_instance.in)
#define shell_fifo_out_p          (shell_fifo_instance.out)
#define shell_fifo_in_pnext       pointer_set_limit(shell_fifo_start, shell_fifo_end, shell_fifo_in_p)
#define shell_fifo_out_pnext      pointer_set_limit(shell_fifo_start, shell_fifo_end, shell_fifo_out_p)
#define shell_fifo_is_full        (shell_fifo.size >= shell_fifo.capacity)
#define shell_fifo_is_null        (!shell_fifo.size)
#define shell_in_equal_out        (shell_fifo_in_p == shell_fifo_out_p)
/* shell simple fifo */
typedef struct {
    /// The pointer to the first character, the sequence ends with index
    /// capacity-1.
    uint8_t *data;
    /// The pointer to the in data,
    uint8_t *in;
    /// The pointer to the out data,
    uint8_t *out;
    /// The length of the effective data.
    size_t size;
    /// The capacity represents the number of allocated bytes.
    size_t capacity;
} shell_simple_fifo;

Shell   shell;
char    shellBuffer[512];
uint8_t letter_shell_recv_buff = 0;

uint8_t           shell_buff[64];
shell_simple_fifo shell_fifo = {shell_buff, shell_buff, shell_buff, 0, 64};

int shell_simple_fifo_in(uint8_t data) {
    if (shell_fifo_is_full) {
        /* shell fifo is full */
        if (shell_in_equal_out)
            return shell_fifo_full;
        else {
            log_e("shell fifo error");
            return shell_fifo_err;
        }
    }

    /* fifo in data */
    *shell_fifo_in_p = data;

    /* fifo in to next */
    shell_fifo_in_pnext;

    /* fifo size +1 */
    shell_fifo_current_length++;

    return shell_fifo_normal;
}

int shell_simple_fifo_out(uint8_t *data) {
    if (shell_fifo_is_null) {
        /* shell fifo is empty */
        if (shell_in_equal_out)
            return shell_fifo_null;
        else {
            log_e("shell fifo error");
            return shell_fifo_err;
        }
    }

    /* fifo out data */
    *data = *shell_fifo_out_p;

    /* fifo out to next */
    shell_fifo_out_pnext;

    /* fifo size -1 */
    shell_fifo_current_length--;

    return shell_fifo_normal;
}

/**
 * @brief 用户shell写
 *
 * @param data 数据
 */
void userShellWrite(char data) {
    HAL_UART_Transmit(&huart5, (uint8_t *) &data, 1, 1000);
}

#if 0
/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @return char 状态
 */
signed char userShellRead(char *data)
{
    if (serialReceive(&debugSerial, (uint8_t *)data, 1, 0) == 1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
#endif

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void) {
    HAL_UART_Receive_IT(&huart5, &letter_shell_recv_buff, 1);
    shell.write = userShellWrite;

#if 0
    shell.read = userShellRead;
#endif

    shellInit(&shell, shellBuffer, 512);
}

/******************************************************************************
 * @Function: letter_shell_recv_irq_handler
 * @Description: letter-shell接收数据处理
 * @Input: void
 * @Output: None
 * @Return: void
 * @Others: None
 * @param {UART_HandleTypeDef} *huart
 *******************************************************************************/
void letter_shell_recv_irq_handler(UART_HandleTypeDef *huart) {
#if 0
   shellHandler(&shell, letter_shell_recv_buff);
   HAL_UART_Receive_IT(huart, &letter_shell_recv_buff, 1);
#else
    shell_simple_fifo_in(letter_shell_recv_buff);
    HAL_UART_Receive_IT(huart, &letter_shell_recv_buff, 1);
#endif
}

void 

letter_shell_recv_handler(void) {
    uint8_t data = '\n';
    int     ret;
    ret = shell_simple_fifo_out(&data);
    while (ret != shell_fifo_null && ret != shell_fifo_err) {
        shellHandler(&shell, data);
        ret = shell_simple_fifo_out(&data);
    }
}
