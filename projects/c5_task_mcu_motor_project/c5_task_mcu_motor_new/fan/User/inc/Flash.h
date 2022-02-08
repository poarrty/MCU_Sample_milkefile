/******************************************************************************
* File Name         :  Flash.c
* Author            :  暨绵浩
* Version           :  1.0 
* Date              :  19/10/09
* Description       :  霍尔及电角度处理函数                         
********************************************************************************
* History: 
* <author>        <time>        <version >        <desc>
* 暨绵浩             19/10/09      1.0               创建   
* 陈雅枫             20/02/05      1.1               修改代码风格，并将线性霍尔部分独立为一个文件。
* 
*
* 
* 
*
* 
*
*
*
*
*
*******************************************************************************/
#include "UserInclude.h"
#include "MCInclude.h"

#ifndef __FLASH_H__
#define __FLASH_H__


//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 512 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////  


#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 




//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址

#define FLASH_SAVE_ADDR  0X08020004  	//设置FLASH 保存地址(128K)(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机. 0X0800C004(49K)
                                        //设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)


uint16_t FLASH_ReadHalfWord(uint32_t addr);		  //读出半字  
void FLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void FLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据

#endif










							   














