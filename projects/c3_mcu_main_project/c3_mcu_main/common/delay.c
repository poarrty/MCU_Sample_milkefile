/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      common.c
 ** Author:
 ** Version:        V1.0
 ** Date:           2021-01-
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-01 robot创建
 ** <time>   <author>    <version >   <desc>
 **
 *************************************************/

/*****************************************************************
 * 包含头文件
 ******************************************************************/
#include "delay.h"
#include "main.h"
#include "cmsis_os.h"
// #include "tim.h"

/**
 * @addtogroup Robot_COMMON
 * @{
 */

/**
 * @defgroup COMMON_DELAY 延时功能 - DELAY
 *
 * @brief
 * \n
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

/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 外部变量声明（如果全局变量没有在其它的H文件声明，引用时需在此处声明，
 *如果已在其它H文件声明，则只需包含此H文件即可）
 ******************************************************************/

/*****************************************************************
 * 静态变量定义
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/

/*****************************************************************
 * 函数定义
 *注意，编写函数需首先定义所有的局部变量等，不允许在
 *函数的中间出现新的变量的定义。
 ******************************************************************/

/*****************************************************************/
/**
 * Function:       delay_init
 * Description:    初始化 delay
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示失败
 *  - 0  表示成功
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
int delay_init(void) {
    return 0;
}

/*****************************************************************/
/**
 * Function:       delay_Deinit
 * Description:    释放 delay 资源
 * Calls:
 * Called By:
 * @param[in]
 * @param[out]
 * @return
 *  - -1 表示打开文件失败
 *  - 0  表示打开文件成功
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
int delay_deInit(void) {
    return 0;
}

//延时nus
// nus:要延时的us数.
// nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
// void delay_us(unsigned int nus) {
//     /*
//     u32 fac_us = (u32)168;
//     u32 ticks;
//     u32 told,tnow,tcnt=0;
//     u32 reload=SysTick->LOAD;               //LOAD的值
//     ticks=nus*fac_us;                       //需要的节拍数
//     //delay_osschedlock();                  //阻止OS调度，防止打断us延时
//     //__set_PRIMASK(1);
//     osKernelLock();
//     told=SysTick->VAL;                      //刚进入时的计数器值
//     while(1)
//     {
//         tnow=SysTick->VAL;
//         if(tnow!=told)
//         {
//             if(tnow<told)tcnt+=told-tnow;
//     //这里注意一下SYSTICK是一个递减的计数器就可以了. else
//     tcnt+=reload-tnow+told; told=tnow; if(tcnt>=ticks)break;
//     //时间超过/等于要延迟的时间,则退出.
//         }
//     };
//     //delay_osschedunlock();                    //恢复OS调度
//     //__set_PRIMASK(0);
//     osKernelUnlock();
//     */

//     uint32_t i, j;

//     osKernelLock();

//     for (i = 0; i < nus; i++) {
//         j = 0;

//         while (j++ < 30)
//             ;
//     }

//     osKernelUnlock();
uint32_t Get_sys_time_us(void)
{
  uint32_t sys_us = 1000*HAL_GetTick()+TIM6->CNT;
  return sys_us;
}
uint32_t Get_sys_time_ms(void)
{
	return HAL_GetTick();
}

int my_delay(uint32_t us)
{
	int curtime = Get_sys_time_us();
	while(1)
	{
		if(Get_sys_time_us() >= curtime + us )
				break;
	}
	
	return 0;
}

#ifdef __cplusplus
}
#endif

/* @} COMMON_DELAY */
/* @} Robot_COMMON */

