/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		driver.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-24
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-24
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "driver.h"

/*************************************************
 Function:		DriverInit
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
PDriverHandle DriverInit(void)
{
    PDriverHandle Handle = (PDriverHandle)malloc(sizeof(DriverHandle));
    if(NULL == Handle)
    {
        return NULL;
    }

    Handle->usbdevs = NULL;
    
    LibUsb_Init(Handle);

    return Handle;
}

/*************************************************
 Function:		DriverDeInit
 Descroption:	
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void DriverDeInit(PDriverHandle Handle)
{
    if(Handle != NULL)
    {
        LibUsb_DeInit(Handle);
        free(Handle);
        Handle = NULL;
    }
}

