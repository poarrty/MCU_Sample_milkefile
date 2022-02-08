/*************************************************
  ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
  ** File name:      sys_list.h
  ** Author:
  ** Version:
  ** Date:
  ** Description:
  ** Others:
  ** Function List:
  ** History:

  ** <time>   <author>    <version >   <desc>
  **
*************************************************/
#ifndef _SYS_LIST_H
#define _SYS_LIST_H

/*****************************************************************
 * 包含头文件(如非特殊需要，H文件不建议包含其它H文件)
 ******************************************************************/

/**
 * @ingroup Robot_SYSLIST
 *
 * @brief \n
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
struct list_struct 
{
	 struct list_struct *next, *prev;
};

/*****************************************************************
 * 全局变量声明
 ******************************************************************/

/*****************************************************************
 * 函数原型声明
 ******************************************************************/
void list_head_init(struct list_struct *list);
void list_head_add(struct list_struct *new_entry, struct list_struct *head);
void list_add_tail(struct list_struct *new_entry, struct list_struct *head);
void list_del_node(struct list_struct *entry);
int list_is_empty(const struct list_struct *head);
int list_is_last(const struct list_struct *list, const struct list_struct *head);
void list_add(struct list_struct *new_entry,  struct list_struct *prev, struct list_struct *next);


/*****************************************************************
 * 函数说明
 ******************************************************************/

#ifdef __cplusplus
}
#endif

/* @} Robot_SYSLIST */

#endif

