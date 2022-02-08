/******************************************************************************
 * @Copyright (C), CVTE Electronics CO.Ltd 2021.
 * @File name: crc16.h
 * @Author: Gu Chunqi(guchunqi@cvte.com)
 * @Version: V1.0
 * @Date: 2021-05-15 10:24:45
 * @Description: CRC16接口定义头文件
 * @Others: None
 * @History: <time>   <author>    <version >   <desc>
 *******************************************************************************/

#ifndef __CRC16_H__
#define __CRC16_H__

#include <stdint.h>

uint16_t calculate_crc16(uint8_t *crc_buf, const uint8_t crc_count);

#endif
