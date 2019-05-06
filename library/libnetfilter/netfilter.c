/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		netfilter.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-10-17
 Description: 		
 History: 			
 					
   1.Date:	 		2018-10-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "netfilter.h"
#ifdef _LINUX_
#include "netfilter_linux.h"
#endif


struct netfilter* mNetfilter = NULL;

/*************************************************
 Function:		netfilter_init
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int netfilter_init(void)
{
    mNetfilter = (struct netfilter *)netfilter_open("wlx30469af9f781");
    return 0;
}

