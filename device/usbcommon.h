/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		usbcommon.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-7-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-7-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _USBCOMMON_H_
#define _USBCOMMON_H_

#include "config.h"
#include "log.h"
#include "libusb.h"
#include "ParamConfig.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************************************
 Function:		LibUsb_ListDevice
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int LibUsb_ListDevice(PDriverHandle Handle);

/*************************************************
 Function:		scan_device
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int LibUsb_ScanDevice(PDriverHandle Handle);

/*************************************************
 Function:		LibUsb_Init
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void LibUsb_Init(PDriverHandle Handle);

/*************************************************
 Function:		LibUsb_DeInit
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void LibUsb_DeInit(PDriverHandle Handle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _USBCOMMON_H_ Edit By Txl 2018-7-29 */

