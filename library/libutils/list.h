/*********************************************************
 Copyright (C),2012-2018,Electronic Technology Co.,Ltd.
 File name: 		List.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2015-7-8
 Description: 		
 History: 			
 					
   1.Date:	 		2015-7-8
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

struct listnode
{
    struct listnode *next;
    struct listnode *prev;
};

#define node_to_item(node, container, member) \
    (container *) (((char*) (node)) - offsetof(container, member))

#define list_declare(name) \
    struct listnode name = { \
        .next = &name, \
        .prev = &name, \
    }

#define list_for_each(node, list) \
    for (node = (list)->next; node != (list); node = node->next)

#define list_for_each_reverse(node, list) \
    for (node = (list)->prev; node != (list); node = node->prev)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/*************************************************
 Function:		list_init
 Descroption:	 
 Input: 
	1.listnode *node
 Output: 
 Return: 	
 Other:  
*************************************************/
void _list_init(struct listnode *node);

/*************************************************
 Function:		list_add
 Descroption:	 
 Input: 
	1.listnode *new
	2.listnode *head
 Output: 
 Return: 	
 Other:  
*************************************************/
void _list_add(struct listnode *newnode, struct listnode *head);

/*************************************************
 Function:		list_add_tail
 Descroption:	 
 Input: 
	1.listnode *head
	2.listnode *item
 Output: 
 Return: 	
 Other:  
*************************************************/
void list_add_tail(struct listnode *head, struct listnode *item);

/*************************************************
 Function:		list_del
 Descroption:	 
 Input: 
	1.listnode *item
 Output: 
 Return: 	
 Other:  
*************************************************/
void _list_del(struct listnode *item);


#define list_empty(list) ((list) == (list)->next)
#define list_head(list) ((list)->next)
#define list_tail(list) ((list)->prev)

#define LIST_INIT(node)             _list_init(node)
#define LIST_ADD(newnode, head)     _list_add(newnode, head)
#define LIST_DEL(item)              _list_del(item)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _LIST_H_ Edit By Txl 2015-7-8 */

