/******************************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:
  ** Author:         曾曼云
  ** Version:        V0.0.1
  ** Date:           2021-9-25
  ** Description:
  ** Others:
  ** Function List:
  ** History:        2021-09 曾曼云 创建
  ** <time>          <author>    <version >    <desc>
  ** 2021-9-25       曾曼云	     0.0.1         创建文件
******************************************************************/

/*****************************************************************
* 包含头文件
******************************************************************/
#include "devices.h"
#include "define.h"
#include "i2c_core.h"
#include <stddef.h>
#include <string.h>


/**
 * @addtogroup Robot_DEVICES
 * @{
 */

/**
 * @defgroup Robot_I2C_CORE  - I2C_CORE
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

/*****************************************************************/
/**
 * Function:       i2c_read_one_byte
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
int32_t i2c_read_one_byte(struct ca_bus *bus, uint16_t read_addr, uint8_t *data)
{
	if((NULL == bus)||(NULL == bus->handler)||(NULL == data))
	{
		return -1;
	}

	if(HAL_OK != HAL_I2C_Mem_Read((I2C_HandleTypeDef *)bus->handler, bus->bus_addr, read_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 100))
	{
		return -1;
	}

	return 0;

}

/*****************************************************************/
/**
 * Function:       i2c_write_one_byte
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
int32_t i2c_write_one_byte(struct ca_bus *bus, uint16_t write_addr, uint8_t *data)
{
	if((NULL == bus)||(NULL == bus->handler)||(NULL == data))
	{
		return -1;
	}

	if(HAL_OK != HAL_I2C_Mem_Write((I2C_HandleTypeDef *)bus->handler, bus->bus_addr, write_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 100))
	{
		return -1;
	}

	return 0;

}

/*****************************************************************/
/**
 * Function:       i2c_read
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
int32_t i2c_read(struct ca_bus *bus, uint16_t read_addr, uint8_t *data, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == bus->handler)||(NULL == data))
	{
		return -1;
	}

	while(len--)
	{
		ret = i2c_read_one_byte(bus, read_addr, data++);
		if(ret < 0)
		{
			break;
		}
	}

	return ret;

}

/*****************************************************************/
/**
 * Function:       i2c_write
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
int32_t i2c_write(struct ca_bus *bus, uint16_t write_addr, uint8_t *data, uint32_t len)
{
	int32_t ret = -1;
	
	if((NULL == bus->handler)||(NULL == data))
	{
		return -1;
	}

	while(len--)
	{
		ret = i2c_write_one_byte(bus, write_addr, data++);
		if(ret < 0)
		{
			break;
		}
	}

	return ret;

}


#ifdef __cplusplus
}
#endif

/* @} Robot_I2C_CORE */
/* @} Robot_DEVICES */

