/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		driver.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-24
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-24
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "config.h"
#include "log.h"
#include "libusb.h"
#include "ParamConfig.h"
#include "usbcommon.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************************************
 Function:		DriverInit
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
PDriverHandle DriverInit(void);

/*************************************************
 Function:		DriverDeInit
 Descroption:	
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void DriverDeInit(PDriverHandle Handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _DRIVER_H_ Edit By Txl 2018-6-24 */

