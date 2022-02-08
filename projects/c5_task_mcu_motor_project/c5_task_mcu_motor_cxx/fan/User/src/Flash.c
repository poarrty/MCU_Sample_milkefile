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





uint16_t hFlashBuf[STM_SECTOR_SIZE/2];//最多是2K字节









/*******************************************************************************
* Function Name  : FLASH_ReadHalfWord
* Description    : 读取指定地址的半字(16位数据)
* parameters     : 读地址(此地址必须为2的倍数!!
* Return         : 对应数据
*******************************************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t addr)
{
	return *(__IO uint16_t*)addr; 
}

   
/*******************************************************************************
* Function Name  : FLASH_Write_NoCheck
* Description    : 
* parameters     : WriteAddr:起始地址     , pBuffer:数据指针 ,Num:半字(16位)数 
*                : 读地址(此地址必须为2的倍数!!
* Return         : 对应数据
*******************************************************************************/
void FLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t Num)   
{ 			 		 
	uint16_t i;
	
	for(i=0;i<Num;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 


/*******************************************************************************
* Function Name  : FLASH_Write
* Description    : 从指定地址开始写入指定长度的数据
* parameters     : WriteAddr:起始地址(此地址必须为2的倍数!!) ,pBuffer:数据指针
*                : Num:半字(16位)数(就是要写入的16位数据的个数.)
* Return         : 对应数据
*******************************************************************************/
void FLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t Num)	
{
	uint32_t wSecpos;	   //扇区地址
	uint16_t hSecoff;	   //扇区内偏移地址(16位字计算)
	uint16_t hSecremain; //扇区内剩余地址(16位字计算)	  	  
	uint32_t wOffaddr;   //去掉0X08000000后的地址
	uint32_t i;  
	
	if((WriteAddr<STM32_FLASH_BASE)||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//非法地址
		
	FLASH_Unlock();						//解锁
	
	wOffaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	wSecpos=wOffaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	hSecoff=(wOffaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	hSecremain=STM_SECTOR_SIZE/2-hSecoff;		//扇区剩余空间大小  
	
	if(Num<=hSecremain)
	{
		hSecremain=Num;//不大于该扇区范围
	}
	
	while(1) 
	{	
		FLASH_Read((wSecpos*STM_SECTOR_SIZE+STM32_FLASH_BASE),hFlashBuf,(STM_SECTOR_SIZE/2));//读出整个扇区的内容
		
		for(i=0;i<hSecremain;i++)//校验数据
		{
			if(hFlashBuf[hSecoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		
		if(i<hSecremain)//需要擦除
		{
			FLASH_ErasePage(wSecpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			
			for(i=0;i<hSecremain;i++)//复制
			{
				hFlashBuf[i+hSecoff]=pBuffer[i];	  
			}
			
			FLASH_Write_NoCheck(wSecpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,hFlashBuf,STM_SECTOR_SIZE/2);//写入整个扇区  
			
		}
		else 
			FLASH_Write_NoCheck(WriteAddr,pBuffer,hSecremain);//写已经擦除了的,直接写入扇区剩余区间. 			
			
		if(Num==hSecremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			wSecpos++;				//扇区地址增1
			hSecoff=0;				//偏移位置为0 	 
		   	pBuffer+=hSecremain;  	//指针偏移
			WriteAddr+=hSecremain;	//写地址偏移	   
		   	Num-=hSecremain;	//字节(16位)数递减
		   	
			if(Num>(STM_SECTOR_SIZE/2))
				hSecremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
				
			else hSecremain=Num;//下一个扇区可以写完了
		}	 
	};	
		
	FLASH_Lock();//上锁
}

/*******************************************************************************
* Function Name  : FLASH_Read
* Description    : 从指定地址开始读出指定长度的数据
* parameters     : ReadAddr:起始地址,pBuffer:数据指针,Num:半字(16位)数                
* Return         : none
*******************************************************************************/
void FLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t Num)   	
{
	uint16_t i;
	
	for(i=0;i<Num;i++)
	{
		pBuffer[i]=FLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}









