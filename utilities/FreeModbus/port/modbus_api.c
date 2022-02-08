/*************************************************************************************
Copyright (C), CVTE Electronics CO.Ltd 2020.
File name	: task_modbus.c
Author		: SuLikang
Version		: 0.1
Date		: 2020-11-25
Description : Modbus 总线数据处理相关函数文件
Others		: 
Functions	: 
History		: 
<time> 		<author> 	<version > 		<desc>
2020-11-25 	SuLikang 	V0.1 			创建文件
*************************************************************************************/
/* Includes ------------------------------------------------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "modbus_api.h"
#include "string.h"

/* Private define ------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------------*/

/* Functions ------------------------------------------------------------------------*/

static eMBErrorCode eMbregGet(USHORT usAddress, USHORT usNRegs,UCHAR * pucRegBuffer, MODBUS_REG *reg_list,int count)
{
        eMBErrorCode    eStatus = MB_ENOERR;
        USHORT offset = 0;
        while(offset <  usNRegs)
        {
            int i;
            for(i = 0;i<count;i++)
            {
               // printf("usAddress +offset :%x\n",usAddress +offset );
               // printf("reg_list[%d].reg_start %x\n",i,reg_list[i].reg_start );
               // printf("reg_list[%d].reg_start + reg_list[%d].u16len:%x\n",i,i,reg_list[i].reg_start + reg_list[i].u16len);
                if(usAddress +offset  >= reg_list[i].reg_start && usAddress +offset <  reg_list[i].reg_start + reg_list[i].u16len )
                {
                    UCHAR *val = NULL;
                    int len = 0,add_offset = 0;
                    if( reg_list[i].get)
                    {
                        val =  reg_list[i].get();
                    }
                    else
                    {
                        return MB_ENOREG;
                    }
                    if(usAddress +offset  > reg_list[i].reg_start)
                    {
                        add_offset = usAddress +offset  - reg_list[i].reg_start;
                    }
                    
                    len = usNRegs - offset > reg_list[i].u16len - add_offset? reg_list[i].u16len - add_offset:  usNRegs - offset;
                    
                    if(val)
                    {
                        memcpy(pucRegBuffer + offset*2,val + add_offset*2,len*2);
                        offset += len;
                        goto END;
                    }
                    else
                    {
                        return MB_ENOREG;
                    }
                }
                
            }
            eStatus = MB_ENOREG;
            break;
    END:
        ;
        }
        
        ///< 大小端转换
        if(eStatus == MB_ENOERR)
        {
            USHORT reg_num = usNRegs;
            USHORT *data = (USHORT *)pucRegBuffer;
            USHORT temp;
            while(reg_num--)
            {
                temp = *data;
                *data = temp>>8;
                *data |= temp<<8;
                data++;
            }
        }
        
        return eStatus;

}

static eMBErrorCode eMbregSet(USHORT usAddress, USHORT usNRegs,UCHAR * pucRegBuffer, MODBUS_REG *reg_list,int count)
{
        eMBErrorCode    eStatus = MB_ENOERR;
        USHORT offset = 0;
        
        ///< 大小端转换
        USHORT reg_num = usNRegs;
        USHORT *data = (USHORT *)pucRegBuffer;
        USHORT temp;
        while(reg_num--)
        {
            temp = *data;
            *data = temp>>8;
            *data |= temp<<8;
            data++;
        }
    
        while(offset <  usNRegs)
        {
            int i;
            for(i = 0;i<count;i++)
            {
                if(usAddress +offset  >= reg_list[i].reg_start && usAddress +offset <  reg_list[i].reg_start + reg_list[i].u16len )
                {
                    int len = 0,add_offset = 0;

                    if(usAddress +offset  > reg_list[i].reg_start)
                    {
                        add_offset = usAddress +offset  - reg_list[i].reg_start;
                    }
                    
                    len = usNRegs - offset > reg_list[i].u16len - add_offset? reg_list[i].u16len - add_offset:  usNRegs - offset;

                    if(reg_list[i].set)
                    {
                        if (reg_list[i].set(pucRegBuffer,add_offset*2,len*2) != 0)
                        {
                            return MB_ENOREG;
                        }
                        
                        offset += len;
                        goto END;
                    }
                    else
                    {
                        return MB_ENOREG;
                    }
                }
                
            }
            eStatus = MB_ENOREG;
            break;
    END:
        ;
        }
        
        ///< 大小端转换  写结束要将大小端转换回去，因为写1个寄存器（功能码06）的时候要返回值
        if(eStatus == MB_ENOERR && usNRegs == 1)
        {
            USHORT reg_num = usNRegs;
            USHORT *data = (USHORT *)pucRegBuffer;
            USHORT temp;
            while(reg_num--)
            {
                temp = *data;
                *data = temp>>8;
                *data |= temp<<8;
                data++;
            }
        }
        
        return eStatus;

}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{

    usAddress--;
    //printf("eMBRegInputCB:%x\n",usAddress);
    return eMbregGet(usAddress,usNRegs,pucRegBuffer,modebus_read_reg_list,modebus_read_reg_list_count);
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    usAddress--;
    //printf("eMBRegHoldingCB:%x\n",usAddress);

    if(eMode == MB_REG_READ)
    {
        return eMbregGet(usAddress,usNRegs,pucRegBuffer,modebus_rw_reg_list,modebus_rw_reg_list_count);
    }
    else  if(eMode == MB_REG_WRITE)
    {
        return eMbregSet(usAddress,usNRegs,pucRegBuffer,modebus_rw_reg_list,modebus_rw_reg_list_count);
    }
    else
    {
        return MB_ENOREG;
    }
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    //printf("eMBRegCoilsCB\n");
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    //printf("eMBRegDiscreteCB\n");
    return MB_ENOREG;
}
