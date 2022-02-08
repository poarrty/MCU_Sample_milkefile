/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-24
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-3-25       sulikang    0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "letter_shell.h"
#include "shell.h"
#include "uart_core.h"
#include "devices.h"
#include <stddef.h>
#include <string.h>
#include "lwrb.h"
#include "cmsis_os2.h"
#include "shell_port.h"
#include "SEGGER_RTT.h"
/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_LETTER_SHELL  - LETTER_SHELL
 *
 * @brief  \n
 * \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
* 私有宏定义
******************************************************************/
#define SHEEL_RECV_BUFF_SIZE 128
/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/
struct ca_device *p_dev = NULL;

static lwrb_t   shell_rbuff;

osSemaphoreId_t shell_empty_id  = NULL;
osSemaphoreId_t shell_filled_id = NULL;

osThreadId_t letter_shell_thread;
const osThreadAttr_t letter_shell_attributes = {
  .name = "letter_shell",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 384 * 4
};
/*****************************************************************
* 全局变量定义
******************************************************************/

/*****************************************************************
* 私有全局变量定义
******************************************************************/
static uint8_t shell_recv_buff[SHEEL_RECV_BUFF_SIZE];
/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/
static void letter_shell_run(void *argument);
/*****************************************************************
* 函数定义
******************************************************************/
int32_t letter_shell_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t letter_shell_close(const struct ca_device *dev)
{
	return 0;
}

int32_t letter_shell_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t letter_shell_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	shellWriteEndLine(&shell, buffer, size);

	return 0;
}

int32_t letter_shell_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	return 0;
}

void letter_shell_rx_callback(const struct ca_device *dev, uint8_t rx_buf)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return;
	}

	if (shell_empty_id != NULL && shell_filled_id != NULL) {
		if (osSemaphoreAcquire(shell_empty_id, 0) == osOK) {
			// produce data
			lwrb_write(&shell_rbuff, &rx_buf, 1);
		}

		osSemaphoreRelease(shell_filled_id);
	}
}

/*****************************************************************/
/**
 * Function:       letter_shell_init
 * Description:    初始化 letter_shell
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int32_t letter_shell_init(struct ca_device *dev)
{
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler))
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = letter_shell_open;
	dev->ops.close = letter_shell_close;
	dev->ops.read = letter_shell_read;
	dev->ops.write = letter_shell_write;
	dev->ops.ioctl = letter_shell_ioctl;

	// uart_rx_register_callback(dev, letter_shell_rx_callback);

	p_dev = dev;

	userShellInit();
	
	lwrb_init(&shell_rbuff, shell_recv_buff, sizeof(shell_recv_buff));
	shell_empty_id  = osSemaphoreNew(SHEEL_RECV_BUFF_SIZE, SHEEL_RECV_BUFF_SIZE, NULL);
    shell_filled_id = osSemaphoreNew(SHEEL_RECV_BUFF_SIZE, 0U, NULL);

	letter_shell_thread = osThreadNew(letter_shell_run, NULL, &letter_shell_attributes);

	return 0;
}

static void letter_shell_run(void *argument) {
	uint8_t data;

	for (;;) {
		// if (osSemaphoreAcquire(shell_filled_id, osWaitForever) == osOK) {
        //     // consume data
        //     if (lwrb_read(&shell_rbuff, &data, 1)) {
        //         ///< 若关闭调度，则shell的命令函数内不能阻塞等待信号量、互斥锁、消息队列
        //         // osKernelLock();
        //         shellHandler(&shell, data);
        //         // osKernelUnlock();
        //     }

        //     osSemaphoreRelease(shell_empty_id);
        // }

		if (SEGGER_RTT_Read(0, &data, 1)) {
			shellHandler(&shell, data);
		} else {
			osDelay(2);
		}
	}
}

/*****************************************************************/
/**
 * Function:       letter_shell_deinit
 * Description:    去初始化 letter_shell
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - 1 表示失败
 *
 * @author:
 * @date
 *
 * @par Modification History
 * @par Author:
 * @par Date:
 * @par Description:
 *
 * @see
 *
 */
/******************************************************************/
int32_t letter_shell_deinit(struct ca_device *dev)
{
	return 0;
}

DEVICE_DRIVER_INIT(letter_shell, uart, letter_shell_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_LETTER_SHELL */
/* @} Robot_DEVICES */

