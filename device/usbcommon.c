/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		usbcommon.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-7-3
 Description: 		
 History: 			
 					
   1.Date:	 		2018-7-3
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "driver.h"

static UsbID mProbeUsbID[] = 
{
    {0x0846,0x9010}
};

/*************************************************
 Function:		dump_altsetting
 Descroption:	 
 Input: 
	1.*dev
	2.struct libusb_interface_descriptor *interface
 Output: 
 Return: 	
 Other:  
*************************************************/
static void dump_altsetting(libusb_device_handle *dev, const struct libusb_interface_descriptor *interface)
{
	LOGD(_LOG_DRIVER_, " \n"
           "    Interface Descriptor:\n"
	       "      bLength             %5u\n"
	       "      bDescriptorType     %5u\n"
	       "      bInterfaceNumber    %5u\n"
	       "      bAlternateSetting   %5u\n"
	       "      bNumEndpoints       %5u\n"
	       "      bInterfaceClass     %5u\n"
	       "      bInterfaceSubClass  %5u\n"
	       "      bInterfaceProtocol  %5u\n",
	       interface->bLength, interface->bDescriptorType, interface->bInterfaceNumber,
	       interface->bAlternateSetting, interface->bNumEndpoints, interface->bInterfaceClass,
	       interface->bInterfaceSubClass,interface->bInterfaceProtocol);
}

/*************************************************
 Function:		dump_interface
 Descroption:	 
 Input: 
	1.*dev
	2.struct libusb_interface *interface
 Output: 
 Return: 	
 Other:  
*************************************************/
static void dump_interface(libusb_device_handle *dev, const struct libusb_interface *interface)
{
	int i;
 
	for (i = 0; i < interface->num_altsetting; i++)
    {   
		dump_altsetting(dev, &interface->altsetting[i]);
    }
}
 
/*************************************************
 Function:		LibUsb_ListDevice
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int LibUsb_ListDevice(PDriverHandle Handle)
{
	libusb_device **list = NULL;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor* conf;
 
	libusb_device_handle *  handle = NULL;
	int config= 0;
	int ret;
	int status = 1; /* 1 device not found, 0 device found */
	ssize_t num_devs, i, j, k;
    
    if(Handle == NULL)
    {
        return 1;
    }

    num_devs = libusb_get_device_list(Handle->usbctx, &list);
	if (num_devs < 0)
		goto error;
 
	for (i = 0; i < num_devs; ++i) {
		libusb_device *dev = list[i];
		libusb_open(dev,&handle);
 
		libusb_get_configuration(handle,&config);
 
 		uint8_t bnum = libusb_get_bus_number(dev);
		uint8_t dnum = libusb_get_device_address(dev);
 
		libusb_get_device_descriptor(dev, &desc);
		status = 0;
		LOGD(_LOG_DRIVER_,"device:%04x:%04x\n",desc.idVendor,desc.idProduct);
		LOGD(_LOG_DRIVER_,"bDeviceSubClass = %5u\n",desc.bDeviceSubClass);
		LOGD(_LOG_DRIVER_,"bDeviceClass    = %5u\n",desc.bDeviceClass);
		LOGD(_LOG_DRIVER_,"bDeviceProtocol    = %5u\n",desc.bDeviceProtocol);
		for( j = 0; j < desc.bNumConfigurations; ++j) 
        {
            ret = libusb_get_config_descriptor(dev, j, &conf);
            if (ret) 
            {
                LOGE(_LOG_DRIVER_, "Couldn't get configuration "
                                "descriptor %lu, some information will "
                                "be missing\n", j);
            }
            else 
            {
                LOGD(_LOG_DRIVER_,"bNumberInterfaces = %5u\n",conf->bNumInterfaces);
                LOGD(_LOG_DRIVER_,"bConfigurationValue = %5u\n",conf->bConfigurationValue);

                for (k = 0 ; k < conf->bNumInterfaces ; k++)
                {
                    dump_interface(handle, &conf->interface[k]);
                }
                libusb_free_config_descriptor(conf);
            }
        }
		
	}
 
	libusb_free_device_list(list, 0);
error:
	return status;
}


/*************************************************
 Function:		LibUsb_ScanDevice
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int LibUsb_GetDevice(PDriverHandle Handle)
{
    int i;
    if(Handle == NULL)
    {
        return 0;
    }

    if(Handle->usbdevs != NULL)
    {
        libusb_free_device_list(Handle->usbdevs, 1);
        Handle->usbdevs = NULL;
    }
    
    Handle->usbcnt = libusb_get_device_list(NULL, &Handle->usbdevs);    
    return Handle->usbcnt;
}

int LibUsb_ProbeDevice(PDriverHandle Handle)
{
    
}

/*************************************************
 Function:		LibUsb_Init
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void LibUsb_Init(PDriverHandle Handle)
{
    libusb_init(&Handle->usbctx);
}

/*************************************************
 Function:		LibUsb_DeInit
 Descroption:	 
 Input: 
	1.Handle
 Output: 
 Return: 	
 Other:  
*************************************************/
void LibUsb_DeInit(PDriverHandle Handle)
{
    if(Handle->usbdevs != NULL)
    {
        libusb_free_device_list(Handle->usbdevs, 1);
        Handle->usbdevs = NULL;
    }
    libusb_exit(Handle->usbctx);
}


