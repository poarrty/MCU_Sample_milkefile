/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      memPool.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-10-
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-10 robot创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "mem_pool.h"
#include <stdint.h>
#include <stddef.h>
#include "cmsis_os.h"


/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_MEMPOOL
 *
 * @brief
 * \n \n
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * 宏定义
 ******************************************************************/

/*****************************************************************
 * 结构定义
 ******************************************************************/
typedef enum{
	OS_ERR_NONE 							= 0u,
	OS_ERR_MEM_CREATE_ISR 					= 22201u,
	OS_ERR_MEM_FULL 						= 22202u,
	OS_ERR_MEM_INVALID_P_ADDR				= 22203u,
	OS_ERR_MEM_INVALID_BLKS 				= 22204u,
	OS_ERR_MEM_INVALID_PART 				= 22205u,
	OS_ERR_MEM_INVALID_P_BLK				= 22206u,
	OS_ERR_MEM_INVALID_P_MEM				= 22207u,
	OS_ERR_MEM_INVALID_P_DATA 			 	= 22208u,
	OS_ERR_MEM_INVALID_SIZE 				= 22209u,
	OS_ERR_MEM_NO_FREE_BLKS 				= 22210u
}OS_ERR;


typedef struct {																						 /* MEMORY CONTROL BLOCK																	 */
		void								*AddrPtr; 													/* Pointer to beginning of memory partition 							*/
		char								*NamePtr;
		void								*FreeListPtr; 											/* Pointer to list of free memory blocks									*/
		int 								 BlkSize; 													/* Size (in bytes) of each block of memory								*/
		int 								 NbrMax;														/* Total number of blocks in this partition 							*/
		int 								 NbrFree; 													/* Number of memory blocks remaining in this partition		*/
}OS_MEM;


/*****************************************************************
 * 全局变量定义
 ******************************************************************/

OS_MEM mem_pool_array_[MEM_BLOCK_NUM]; 

uint8_t mem_buf_32_[MEM_BLOCK_SIZE_NUM_32][32] __attribute__((section(".IRAM2")));
uint8_t mem_buf_64_[MEM_BLOCK_SIZE_NUM_64][64] __attribute__((section(".IRAM2")));
uint8_t mem_buf_128_[MEM_BLOCK_SIZE_NUM_128][128] __attribute__((section(".IRAM2")));
uint8_t mem_buf_256_[MEM_BLOCK_SIZE_NUM_256][256] __attribute__((section(".IRAM2")));

osMutexId_t mutex_mem_pool_ = NULL;

const osMutexAttr_t mutex_mem_pool_attr_ = {
    "mutex_mem_pool",                       // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};


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
 * Function:       mem_create
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
void  mem_create (OS_MEM    *p_mem, char *p_name, void *p_addr, int n_blks, int blk_size, OS_ERR *p_err)
{
    int     i;
    int     loops;
    uint8_t    *p_blk;
    void         **p_link;

    if (p_addr == (void *)0) {                              /* Must pass a valid address for the memory part.         */
       *p_err   = OS_ERR_MEM_INVALID_P_ADDR;
        return;
    }
    if (n_blks < 2) {                           					 /* Must have at least 2 blocks per partition              */
       *p_err = OS_ERR_MEM_INVALID_BLKS;
        return;
    }
    if (blk_size < sizeof(void *)) {                        /* Must contain space for at least a pointer              */
       *p_err = OS_ERR_MEM_INVALID_SIZE;
        return;
    }

    p_link = (void **)p_addr;                               /* Create linked list of free memory blocks               */
    p_blk  = (uint8_t *)p_addr;
    loops  = n_blks - 1u;
    for (i = 0u; i < loops; i++) {
        p_blk +=  blk_size;
       *p_link = (void  *)p_blk;                            /* Save pointer to NEXT block in CURRENT block            */
        p_link = (void **)(void *)p_blk;                    /* Position     to NEXT block                             */
    }
   *p_link             = (void *)0;                         /* Last memory block points to NULL                       */

	osMutexAcquire(mutex_mem_pool_, osWaitForever);

    p_mem->NamePtr     = p_name;                            /* Save name of memory partition                          */
    p_mem->AddrPtr     = p_addr;                            /* Store start address of memory partition                */
    p_mem->FreeListPtr = p_addr;                            /* Initialize pointer to pool of free blocks              */
    p_mem->NbrFree     = n_blks;                            /* Store number of free blocks in MCB                     */
    p_mem->NbrMax      = n_blks;
    p_mem->BlkSize     = blk_size;                          /* Store block size of each memory blocks                 */

	osMutexRelease(mutex_mem_pool_);
		
    *p_err = OS_ERR_NONE;
}


void  *mem_get (OS_MEM  *p_mem, OS_ERR  *p_err)
{
    void    *p_blk;

    if (p_err == (OS_ERR *)0) {
        return ((void *)0);
    }

    if (p_mem == (OS_MEM *)0) {                             /* Must point to a valid memory partition                 */
       *p_err  = OS_ERR_MEM_INVALID_P_MEM;
        return ((void *)0);
    }

	osMutexAcquire(mutex_mem_pool_, osWaitForever);
		
    if (p_mem->NbrFree == 0) {                  /* See if there are any free memory blocks                */

		osMutexRelease(mutex_mem_pool_);
			 
       *p_err = OS_ERR_MEM_NO_FREE_BLKS;                    /* No,  Notify caller of empty memory partition           */
        return ((void *)0);                                 /*      Return NULL pointer to caller                     */
    }
    p_blk              = p_mem->FreeListPtr;                /* Yes, point to next free memory block                   */
    p_mem->FreeListPtr = *(void **)p_blk;                   /*      Adjust pointer to new free list                   */
    p_mem->NbrFree--;                                       /*      One less memory block in this partition           */

	osMutexRelease(mutex_mem_pool_);

   *p_err = OS_ERR_NONE;                                    /*      No error                                          */
    return (p_blk);                                         /*      Return memory block to caller                     */
}

void  mem_put (OS_MEM  *p_mem, void *p_blk, OS_ERR *p_err)
{
	osMutexAcquire(mutex_mem_pool_, osWaitForever);
		
    if (p_mem->NbrFree >= p_mem->NbrMax) {                  /* Make sure all blocks not already returned              */

		osMutexRelease(mutex_mem_pool_);
		
		*p_err = OS_ERR_MEM_FULL;
		return;
    }
		
    *(void **)p_blk    = p_mem->FreeListPtr;                /* Insert released block into free block list             */
    p_mem->FreeListPtr = p_blk;
    p_mem->NbrFree++;                                       /* One more memory block in this partition                */

	osMutexRelease(mutex_mem_pool_);

   *p_err              = OS_ERR_NONE;                       /* Notify caller that memory block was released           */
}


void * mem_block_alloc(int uiSize)
{
     int iIndex;
     OS_ERR err;
     uint32_t *pBuf;

     iIndex = 0;
     
     while( iIndex < MEM_BLOCK_NUM )
     {    
         if(uiSize <= mem_pool_array_[iIndex].BlkSize)
         {
             break;
         }
         iIndex++;
     }

     pBuf = (uint32_t *)mem_get(&mem_pool_array_[iIndex], &err);
     if(err != OS_ERR_NONE)
     {
        return NULL;
     }

     return pBuf;
     
}

int mem_block_free(void *ptr)
{      
      OS_ERR err;
      int iIndex = 0;
      
      if( NULL == ptr )
      {
          return -1;
      }

      while( iIndex < MEM_BLOCK_NUM )
     {    
         if(ptr < mem_pool_array_[iIndex].AddrPtr)     		 
						break;
         iIndex++;
     }

      if(iIndex > MEM_BLOCK_NUM)
      {
//      	printf("--------nRet= -1: iIndex=%d \r\n",iIndex);
        return -1;
       }

     mem_put(&mem_pool_array_[iIndex - 1], ptr, &err);
     if(err != OS_ERR_NONE)
     {
//     	printf("--------nRet= -1: err=%d \r\n",err);
        return -1;
     }

      ptr = NULL;

      return 0;
    
}

int mem_pool_init(void)
{
     OS_ERR err;

		 mutex_mem_pool_ = osMutexNew(&mutex_mem_pool_attr_);

		 mem_create(&mem_pool_array_[0],"memPool_32", mem_buf_32_, MEM_BLOCK_SIZE_NUM_32,32,&err);
     if( err != OS_ERR_NONE )
     {
         return -1;
     }
    
     mem_create(&mem_pool_array_[1],"memPool_64", mem_buf_64_, MEM_BLOCK_SIZE_NUM_64,64,&err);
     if( err != OS_ERR_NONE )
     {
         return -1;
     }

     mem_create(&mem_pool_array_[2],"memPool_128",mem_buf_128_,MEM_BLOCK_SIZE_NUM_128,128,&err);
     if( err != OS_ERR_NONE )
     {
         return -1;
     }

     mem_create(&mem_pool_array_[3],"memPool_256",mem_buf_256_,MEM_BLOCK_SIZE_NUM_256,256,&err);
     if( err != OS_ERR_NONE )
     {
         return -1;
     }

     return 0;
}

int  mem_pool_deinit(void)
{
     return 0;
}

/* @} Robot_MEMPOOL */ 
/* @} Robot-NAV_407 */
