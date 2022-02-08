/*****************************************************************
* 包含头文件
******************************************************************/
#include "devices.h"
#include "define.h"
#include "tim_core.h"
#include "gpio_core.h"
#include <stddef.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal.h"
#include "dig_usound_S12.h"
#include "delay.h"
#include "log.h"
extern int led_handle;
/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_GPIO_CORE  - GPIO_CORE
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
uint32_t capture_Buf[2] = {0};
 uint8_t capture_Cnt  = 0;
/*****************************************************************
* 私有结构体/共用体/枚举定义
******************************************************************/

/*****************************************************************
* 全局变量定义
******************************************************************/


/*****************************************************************
* 私有全局变量定义
******************************************************************/

/*****************************************************************
* 外部变量声明
******************************************************************/

/*****************************************************************
* 私有函数原型声明
******************************************************************/

/*****************************************************************
* 函数定义
******************************************************************/
int32_t dig_usound_S12_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t dig_usound_S12_close(const struct ca_device *dev)
{
	return 0;
}

/*****************************************************************/
/**
 * Function:       gpio_read
 * Description:    
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - -1 表示失败
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
int32_t dig_usound_S12_read(const struct ca_device *dev, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

/*****************************************************************/
/**
 * Function:       gpio_write
 * Description:    
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - 0 表示成功
 *  - -1 表示失败
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
int32_t dig_usound_S12_write(const struct ca_device *dev, void *buffer, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == dev)||(NULL == dev->bus)||(NULL == dev->bus->handler)||(NULL == buffer))
	{
		return -1;
	}

	return ret;

}

int32_t dig_usound_S12_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
    int32_t ret = -1;
	if(NULL == dev)
	{
		return -1;
	}
	
	switch(cmd)
	{
		case 0:
			dig_usound_S12_send_B(dev);
			ret = dig_usound_S12_read_distance(dev);
			break;
		default:
			break;
	}
	
	return 0;
}

void dig_usound_S12_ic_capture_callback(const struct ca_device *dev,void *handler)
{
    struct dig_usound_S12_user_argv *dig_usound_static = NULL;
    dig_usound_static  = (struct dig_usound_S12_user_argv *)dev->user_args;
	switch(capture_Cnt)
    {
			case 1:
				capture_Buf[0] = Get_sys_time_us();//获取当前的捕获.
				capture_Cnt++;
				break;
			case 2:
				capture_Buf[1] = Get_sys_time_us();//获取当前的捕获.
				tim_capture_stop(dev->bus); 
				capture_Cnt++;    
		}
}

int32_t dig_usound_S12_driver_init(struct ca_device *dev)
{
	if(NULL == dev)
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = dig_usound_S12_open;
	dev->ops.close = dig_usound_S12_close;
	dev->ops.read = dig_usound_S12_read;
	dev->ops.write = dig_usound_S12_write;
	dev->ops.ioctl = dig_usound_S12_ioctl;

    struct ca_bus *bus = NULL;
    if(bus_find_name(dev->device_args, &bus))
	{
		return -1;
	}
    dev->device_args = bus;

	tim_ic_capture_register_callback(dev,NULL,dig_usound_S12_ic_capture_callback);
	
	return 0;
}

//发送sendB命令
void dig_usound_S12_send_B(const struct ca_device *dev)
{
    struct ca_bus *dig_usound_gpio = NULL;
    dig_usound_gpio = (struct ca_bus *)dev->device_args;
    
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_HIGH, NULL);
    my_delay(200);
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_LOW, NULL);
    my_delay(50);
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_HIGH, NULL);
    my_delay(50);
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_LOW, NULL);
    my_delay(100);
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_HIGH, NULL);
    my_delay(100);
    gpio_ioctl(dig_usound_gpio, GPIO_ACTIVE_LOW, NULL);

}


//读取距离
int32_t dig_usound_S12_read_distance(const struct ca_device *dev)
{
    struct dig_usound_S12_user_argv *dig_usound_static = NULL;
    dig_usound_static  = (struct dig_usound_S12_user_argv *)dev->user_args;

    uint32_t read_start_ostime = Get_sys_time_us();//读取当前微妙级时间辍 为判断超时作准备
    uint32_t using_time = 0;
    uint16_t high_time = 0;
    int i = 0;
    while (1)
    {
        //读取两次下降沿之间的时间 根据此时间计算出距离
        switch (capture_Cnt)
        {
        case 0:
            capture_Cnt++;
            tim_capture_start(dev->bus,TIM_IC_FALLING);//开启下降沿捕获中断    
            break;
        case 3:
            high_time = capture_Buf[1] - capture_Buf[0]; //两次下降沿之间的电平时间 high_time这个变量的取名有问题 大家不用理会
            i++;
            capture_Cnt = 0; //清空标志位
            break;
        }

        //若命令发送后15ms未收到回波 则超时
        if (Get_sys_time_us() >= read_start_ostime + 15000)
        {
            capture_Cnt = 0;                            //清空标志位
            tim_capture_stop(dev->bus);                 //停止捕获
            return 0;
        }
        //读取五个低高电平后跳出循环
        if (i == 1)
            break;
     }
    using_time = Get_sys_time_us()- read_start_ostime;//读取一次超声的总用时
    //第一次时间偏移修正
    uint16_t time_delay_fix = high_time - 160;
    uint16_t time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;

    //第二次时间偏移修正
    if (time_delay_fix_distance >= 585)
    {
        time_delay_fix = high_time  - 120;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    //第三次时间偏移修正
    if (time_delay_fix_distance >= 785)
    {
        time_delay_fix = high_time  - 100;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    //第四次时间偏移修正
    if (time_delay_fix_distance >= 950)
    {
        time_delay_fix = high_time  - 70;
        time_delay_fix_distance = time_delay_fix * 340 / 1000 / 2;
    }
    LOG_DEBUG("du:%d\r\n",time_delay_fix_distance);
    return time_delay_fix_distance;
}


DEVICE_DRIVER_INIT(du_S12,tim, dig_usound_S12_driver_init);


#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */
/* @} Robot_DEVICES */


