/*************************************************************************************
Copyright (C), CVTE Electronics CO.Ltd 2020.
File name	: 
Author		: SuLikang
Version		: 0.1
Date		: 
Description :   
Others		: 
Functions	: 
History		: 
<time> 		<author> 	<version > 		<desc>
2020-11-27  SuLikang 	V0.1 			创建文件
**************************************************************************************/
#ifndef __MODBUS_API_H
#define __MODBUS_API_H
#include "stdint.h"



/* Extern define --------------------------------------------------------------------*/
typedef unsigned short USHORT;
typedef unsigned char UCHAR;

/* Extern typdefine -----------------------------------------------------------------*/
typedef struct
{
    USHORT reg_start;
    USHORT u16len;
    UCHAR *(*get)();
    int (*set)(UCHAR *set_val,int start,int len);
}MODBUS_REG;

/* Extern variables -----------------------------------------------------------------*/
extern MODBUS_REG modebus_read_reg_list[];
extern int modebus_read_reg_list_count;
extern MODBUS_REG modebus_rw_reg_list[];
extern int modebus_rw_reg_list_count;

/* External function ----------------------------------------------------------------*/

#endif
