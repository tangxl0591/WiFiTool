/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		device_usb.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-7-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-7-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "device_usb.h"

#define _DEBUG_PRINT_       1

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
 Function:		Usb_Probe
 Descroption:	 
 Input: 
	1.handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Probe(void* handle)
{
    int i,j,k;
    int num_devs = 0;
    int ret = -1;
    libusb_device **list = NULL;
    PDeviceUSBParam mUsbParam = NULL;
    libusb_device_handle* device_handle = NULL;
    struct libusb_device_descriptor desc;
	struct libusb_config_descriptor* conf;
    int config= 0;
    PDeviceDescription mDeviceUsb  = (PDeviceDescription)handle;
    
    if(mDeviceUsb == NULL)
    {
        return -1;
    }

    mUsbParam = (PDeviceUSBParam)malloc(sizeof(DeviceUSBParam));
    if(NULL == mUsbParam)
    {
        return -1;
    }
    memset(mUsbParam, 0, sizeof(DeviceUSBParam));

    libusb_init(&mUsbParam->usbctx);

    num_devs = libusb_get_device_list(mUsbParam->usbctx, &list); //get the list of devices
	if(num_devs < 0) {
		goto error_exit;
	}

    for(i = 0; i < num_devs; i++)
    {
		libusb_device *dev = list[i];
		libusb_open(dev,&device_handle);
 
		libusb_get_configuration(device_handle,&config);
		libusb_get_device_descriptor(dev, &desc);
        #if _DEBUG_PRINT_
		LOGE(_LOG_DRIVER_,"device:%04x:%04x\n",desc.idVendor,desc.idProduct);
		LOGE(_LOG_DRIVER_,"bDeviceSubClass = %5u\n",desc.bDeviceSubClass);
		LOGE(_LOG_DRIVER_,"bDeviceClass    = %5u\n",desc.bDeviceClass);
		LOGE(_LOG_DRIVER_,"bDeviceProtocol    = %5u\n",desc.bDeviceProtocol);
		for( j = 0; j < desc.bNumConfigurations; ++j) 
        {
            ret = libusb_get_config_descriptor(dev, j, &conf);
            if (ret) 
            {
                LOGE(_LOG_DRIVER_,"Couldn't get configuration descriptor %lu, some information will be missing\n", j);
            }
            else 
            {
                LOGE(_LOG_DRIVER_,"bNumberInterfaces = %5u\n",conf->bNumInterfaces);
                LOGE(_LOG_DRIVER_,"bConfigurationValue = %5u\n",conf->bConfigurationValue);
                for (k = 0 ; k < conf->bNumInterfaces ; k++)
                {
                	dump_interface(device_handle, &conf->interface[k]);
                }
                libusb_free_config_descriptor(conf);
            }
        }
        #endif
        for(j = 0; j < (sizeof(mProbeUsbID)/sizeof(UsbID)); j++)
        {
            if(desc.idVendor == mProbeUsbID[j].VendorID && desc.idProduct == mProbeUsbID[j].ProductID)
            {
                mUsbParam->mUsbID.ProductID = desc.idProduct;
                mUsbParam->mUsbID.VendorID = desc.idVendor;
                ret = 0;
                break;
            }
        }

        libusb_close(device_handle);
        
	}

    libusb_free_device_list(list, 1);
    if(ret != 0)
    {
        goto error_exit;
    }
    pthread_mutex_init(&(mDeviceUsb->mutex),NULL);
    return ret;
    
error_exit:

    if(mUsbParam)
    {
        libusb_exit(mUsbParam->usbctx);
        free(mDeviceUsb->private);
        mDeviceUsb->private = NULL;
    }
   
    return ret;
}

/*************************************************
 Function:		Usb_Release
 Descroption:	 
 Input: 
	1.handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Release(void* handle)
{
    int ret = -1;
    PDeviceUSBParam mUsbParam = NULL;
    PDeviceDescription mDeviceUsb  = (PDeviceDescription)handle;
    if(mDeviceUsb == NULL)
    {
        return -1;
    }

    mUsbParam = (PDeviceUSBParam)mDeviceUsb->private;
    if(mUsbParam == NULL)
    {
        return -1;
    }
    

    libusb_exit(mUsbParam->usbctx);
    
    free(mDeviceUsb->private);
    mDeviceUsb->private = NULL;

    pthread_mutex_destroy(&(mDeviceUsb->mutex));
    return 0;
}

/*************************************************
 Function:		Usb_Open
 Descroption:	 
 Input: 
	1.handle
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Open(void* handle)
{
    int ret = -1;
    PDeviceUSBParam mUsbParam = NULL;
    PDeviceDescription mDeviceUsb  = (PDeviceDescription)handle;
    if(mDeviceUsb == NULL)
    {
        return -1;
    }

    mUsbParam = (PDeviceUSBParam)mDeviceUsb->private;
    if(mUsbParam == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&(mDeviceUsb->mutex));
    if(NULL == mUsbParam->device_handle)
    {
        mUsbParam->device_handle = libusb_open_device_with_vid_pid(mUsbParam->usbctx, mUsbParam.mUsbID.VendorID, mUsbParam.mUsbID.ProductID);     
        if(mUsbParam->device_handle)
        {
            mDeviceUsb->count = 1;
            ret = 0;

            if(libusb_kernel_driver_active(mUsbParam->device_handle, 0) == 1) 
            {
			    LOGD(_LOG_DRIVER_,"Kernel Driver Active");
    			if(libusb_detach_kernel_driver(mUsbParam->device_handle, 0) == 0) 
                {
                    LOGD(_LOG_DRIVER_,"Kernel Driver Detached");
                }         
		    }
    		if(libusb_claim_interface(mUsbParam->device_handle, 0) < 0 )
            {
                LOGD(_LOG_DRIVER_,"Cannot Claim Interface");
                mDeviceUsb->count = 0;
                ret = -1;                
                goto error_open;
                
            }              
        }
    }
    else
    {
        mDeviceUsb->count++;
        ret = 0;
    }
    

error_open:
    pthread_mutex_unlock(&(mDeviceUsb->mutex));
    if(-1 == ret)
    {
        libusb_close(mUsbParam->device_handle);
        mUsbParam->device_handle = NULL;
    }
    return ret;
}

/*************************************************
 Function:		Usb_Close
 Descroption:	 
 Input: 
	1.handle
	2.param
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Close(void* handle, void* param)
{
    int ret = -1;
    PDeviceUSBParam mUsbParam = NULL;
    PDeviceDescription mDeviceUsb  = (PDeviceDescription)handle;
    if(mDeviceUsb == NULL)
    {
        return -1;
    }

    mUsbParam = (PDeviceUSBParam)mDeviceUsb->private;
    if(mUsbParam == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&(mDeviceUsb->mutex));
    if(mDeviceUsb->count > 0)
    {
        mDeviceUsb->count--;
    }

    if(mDeviceUsb->count == 0 && NULL != mUsbParam->device_handle)
    {
        libusb_close(mUsbParam->device_handle);
        mUsbParam->device_handle = NULL;
     }
    
    pthread_mutex_unlock(&(mDeviceUsb->mutex));    
    ret = 0;
    return ret;
}


/*************************************************
 Function:		Usb_Read
 Descroption:	 
 Input: 
	1.handle
	2.char* Buf
	3.Size
	4.int Timeout
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Read(void* handle, unsigned char* Buf, int Size, unsigned int Timeout)
{

}

/*************************************************
 Function:		Usb_Write
 Descroption:	 
 Input: 
	1.handle
	2.char* Buf
	3.Size
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Write(void* handle, unsigned char* Buf, int Size)
{

}

/*************************************************
 Function:		Usb_Ioctrl
 Descroption:	 
 Input: 
	1.handle
	2.Cmd
	3.Param1
	4.Param2
 Output: 
 Return: 	
 Other:  
*************************************************/
int Usb_Ioctrl(void* handle, MODULE_CMD Cmd, int Param1, void* Param2)
{

}

DeviceDescription DeviceUsb =
{
    .name = "LibUsb",
    .Id = MODULE_USB,
    .handle = NULL,
    .private = NULL,
    .count = 0,
    .probe = Usb_Probe,
    .open = Usb_Open,
    .close = Usb_Close,
    .read = Usb_Read,
    .write = Usb_Write,
    .ioctrl = Usb_Ioctrl,   
    .release = Usb_Release,
};

