/*********************************************************
 Copyright (C),2016-2022,Electronic Technology Co.,Ltd.
 File name: 		filter.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2019-9-17
 Description: 		
 History: 			
 					
   1.Date:	 		2019-9-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "filter_core.h"
#include "filter_net.h"

extern MSFilterDesc ms_netcore_desc;

static MSFilterDesc * ms_module_descs[]={
    &ms_netcore_desc,
	NULL,
};

/*************************************************
  Function:		ms_module_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_module_init(void)
{
	int i;
	for (i = 0; ms_module_descs[i] != NULL; i++)
	{
		ms_module_register(ms_module_descs[i]);
	}
}

/*************************************************
 Function:		module_init
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
void module_init(void)
{
    ms_module_init();
    MSFilterDesc * mNetCore = ms_filter_new(MS_NET_CORE_ID);
    ms_filter_call_method_noarg(mNetCore,MS_NET_CORE_OPEN);
}

/*************************************************
 Function:		module_exit
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
void module_exit(void)
{
    ms_module_unregister_all();
}