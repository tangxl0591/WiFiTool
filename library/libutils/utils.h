/*********************************************************
 Copyright (C),2016-2022,Electronic Technology Co.,Ltd.
 File name: 		utils.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2019-9-16
 Description: 		
 History: 			
 					
   1.Date:	 		2019-9-16
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#include "platform.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************************************
 Function:		is_dir_exit
 Descroption:	 
 Input: 
	1.*path
 Output: 
 Return: 	
 Other:  
*************************************************/
int is_dir_exit(char *path);

/*************************************************
 Function:		u_fileexist
 Descroption:	 
 Input: 
	1.*filename
 Output: 
 Return: 	
 Other:  
*************************************************/
int u_fileexist(char *filename);

/*************************************************
 Function:		find_wifi_classname
 Descroption:	 
 Input: 
	1.path
 Output: 
 Return: 	
 Other:  
*************************************************/
int find_wifi_classname(char* path, char *name);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _UTILS_H_ Edit By Txl 2019-9-16 */

