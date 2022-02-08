/*************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:      sys_list.c
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

#include "sys_list.h"

/**
 * @addtogroup Robot-NAV_407
 * @{
 */

/**
 * @defgroup Robot_SYSLIST
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


/*****************************************************************
 * 全局变量定义
 ******************************************************************/

/*****************************************************************
 * 私有全局变量定义
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
 * Function:       hal_init
 * Description:    初始化HAL层
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
void list_head_init(struct list_struct *list)
 {
     list->next = list;
     list->prev = list;
 }


 void list_add(struct list_struct *new_entry,
               struct list_struct *prev,
               struct list_struct *next)
 {
        next->prev = new_entry;
        new_entry->next = next;
        new_entry->prev = prev;
        prev->next = new_entry;
 }


 void list_head_add(struct list_struct *new_entry, struct list_struct *head)
 {
      list_add(new_entry, head, head->next);
 }

 
 void list_add_tail(struct list_struct *new_entry, struct list_struct *head)
 {
     list_add(new_entry, head->prev, head);
 }

 
 void list_del(struct list_struct * prev, struct list_struct * next)
 {
    next->prev = prev;
    prev->next = next;
 }

 void list_del_node(struct list_struct *entry)
 {
      list_del(entry->prev, entry->next);
 }


 void list_replace(struct list_struct *old_entry,
                              struct list_struct *new_entry)
{
         new_entry->next = old_entry->next;
         new_entry->next->prev = new_entry;
         new_entry->prev = old_entry->prev;
         new_entry->prev->next = new_entry;
}


 int list_is_empty(const struct list_struct *head)
 {
	if(head->next == head)
	{
		return(1);
	}
    return (0);
 }


 int list_is_last(const struct list_struct *list, const struct list_struct *head)
 {
	 if(list->next == head)
	 {
		 return(1);
	 }
     return(0);
 }
 


#ifdef __cplusplus
}
#endif

/* @} Robot_SYSLIST */ 
/* @} Robot-NAV_407 */

