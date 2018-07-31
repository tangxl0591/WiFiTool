/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		device.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-7-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-7-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <semaphore.h>
#include <dlfcn.h>

#include "list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum
{
    IOCTRL_NONE = 0x00;
}MODULE_CMD;

typedef enum
{
    MODULE_USB         = 0x01,
}MODULE_ID;

typedef struct
{
    const char *name;
    MODULE_ID Id;
    char device[200];
    int count;
    pthread_mutex_t mutex;
    void *handle;
    void *private;
    
    
    int (* probe)(void*);
    int (* open)(void *, void* Param);
    int (* close)(void *, void* Param);
    int (* read)(void *, unsigned char* Buf, int Size, unsigned int Timeout);
    int (* write)(void *, unsigned  char* Buf, int Size);
    int (* ioctrl)(void *, MODULE_CMD Cmd, int Param1, void* Param2);
    int (* release)(void*);

}DeviceDescription, *PDeviceDescription;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _DEVICE_H_ Edit By Txl 2018-7-29 */

