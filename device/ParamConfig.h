/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		ParamConfig.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-24
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-24
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _PARAMCONFIG_H_
#define _PARAMCONFIG_H_

#include "libusb.h"
#include "pcap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct
{
    int ProductID;
    int VendorID;
}UsbID;

typedef struct
{
    int usbcnt;
    libusb_context *usbctx;
    libusb_device **usbdevs;
}DriverHandle,*PDriverHandle;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _PARAMCONFIG_H_ Edit By Txl 2018-6-24 */

