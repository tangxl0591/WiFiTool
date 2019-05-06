/*********************************************************
 Copyright (C),2012-2018,Electronic Technology Co.,Ltd.
 File name: 		list.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2015-7-8
 Description: 		
 History: 			
 					
   1.Date:	 		2015-7-8
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include <stdio.h>
#include "list.h"


/*************************************************
 Function:		_list_init
 Descroption:	 
 Input: 
	1.listnode *node
 Output: 
 Return: 	
 Other:  
*************************************************/
void _list_init(struct listnode *node)
{
    node->next = node;
    node->prev = node;
}

/*************************************************
 Function:		__list_add
 Descroption:	 
 Input: 
	1.listnode *new
	2.listnode *prev
	3.listnode *next
 Output: 
 Return: 	
 Other:  
*************************************************/
void __list_add(struct listnode *newnode,
			      struct listnode *prev,
			      struct listnode *next)
{
	next->prev = newnode;
	newnode->next = next;
	newnode->prev = prev;
	prev->next = newnode;
}

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
void _list_add(struct listnode *newnode, struct listnode *head)
{
	__list_add(newnode, head, head->next);
}


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
void list_add_tail(struct listnode *head, struct listnode *item)
{
    __list_add(item, head->prev, head);
}


/*************************************************
 Function:		__list_del
 Descroption:	 
 Input: 
	1.listnode * prev
	2.listnode * next
 Output: 
 Return: 	
 Other:  
*************************************************/
void __list_del(struct listnode * prev, struct listnode * next)
{
    next->prev = prev;
    prev->next = next;
}

/*************************************************
 Function:		list_remove
 Descroption:	 
 Input: 
	1.listnode *item
 Output: 
 Return: 	
 Other:  
*************************************************/
void _list_del(struct listnode *item)
{
	__list_del(item->prev, item->next);
	item->next = 0;
	item->prev = 0;
}


