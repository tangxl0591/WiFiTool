/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		platform.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-9-2
 Description: 		
 History: 			
 					
   1.Date:	 		2018-9-2
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#if defined(_LINUX_)
#include <pthread.h>
#include <fcntl.h>
#include <poll.h>
#include <semaphore.h>
#include <dlfcn.h>
#endif

#include "log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _PLATFORM_H_ Edit By Txl 2018-9-2 */

