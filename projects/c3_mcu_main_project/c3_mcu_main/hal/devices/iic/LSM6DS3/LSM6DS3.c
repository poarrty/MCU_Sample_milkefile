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
#include "LSM6DS3.h"
#include "LSM6DS3_ACC_GYRO_driver.h"


/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_LSM6DS3  - LSM6DS3
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
int32_t LSM6DS3H_open(const struct ca_device *dev, int32_t flags)
{
	return 0;
}

int32_t LSM6DS3H_close(const struct ca_device *dev)
{
	return 0;
}

int32_t LSM6DS3H_read(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t LSM6DS3H_write(const struct ca_device *dev, void *buffer, uint32_t size)
{
	return 0;
}

int32_t LSM6DS3H_ioctl(const struct ca_device *dev,  uint32_t cmd, void *arg)
{
	return 0;
}

/*****************************************************************/
/**
 * Function:       LSM6DS3H_init
 * Description:    初始化 LSM6DS3H
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
int32_t LSM6DS3H_init(struct ca_device *dev)
{
	if(NULL == dev)
	{
		return -1;
	}

	memset(&dev->ops, 0, sizeof(struct ca_device_ops));

	dev->ops.open = LSM6DS3H_open;
	dev->ops.close = LSM6DS3H_close;
	dev->ops.read = LSM6DS3H_read;
	dev->ops.write = LSM6DS3H_write;
	dev->ops.ioctl = LSM6DS3H_ioctl;
	
	return 0;
}

/*****************************************************************/
/**
 * Function:       LSM6DS3H_deinit
 * Description:    去初始化 LSM6DS3H
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
int32_t LSM6DS3H_deinit(struct ca_device *dev)
{
	return 0;
}


#ifdef __cplusplus
}
#endif

/* @} Robot_LSM6DS3 */
/* @} Robot_DEVICES */

