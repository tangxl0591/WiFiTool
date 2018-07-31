/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		device_usb.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-7-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-7-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _DEVICE_USB_H_
#define _DEVICE_USB_H_

#include "libusb.h"
#include "Device.h"

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
    UsbID mUsbID;
    libusb_context *usbctx;
    libusb_device_handle* device_handle;
}DeviceUSBParam,*PDeviceUSBParam;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _DEVICE_USB_H_ Edit By Txl 2018-7-29 */

