/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		log.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-22
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-22
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _LOG_H_
#define _LOG_H_

#include "platform.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef   LOG_TAG  
#define  LOG_TAG        "WIFI"
   
#define _LOG_NONE_		 0x00
#define _LOG_COMMON_     0x01
#define _LOG_DRIVER_     0x02
#define _LOG_LOGIC_	     0x04
#define _LOG_RESULT_     0x08
#define _LOG_CORE_       0x10


#define COLOR_NONE                 "\e[0m"
#define COLOR_BLACK                "\e[0;30m"
#define COLOR_L_BLACK              "\e[1;30m"
#define COLOR_RED                  "\e[0;31m"
#define COLOR_L_RED                "\e[1;31m"
#define COLOR_GREEN                "\e[0;32m"
#define COLOR_L_GREEN              "\e[1;32m"
#define COLOR_BROWN                "\e[0;33m"
#define COLOR_YELLOW               "\e[1;33m"
#define COLOR_BLUE                 "\e[0;34m"
#define COLOR_L_BLUE               "\e[1;34m"
#define COLOR_PURPLE               "\e[0;35m"
#define COLOR_L_PURPLE             "\e[1;35m"
#define COLOR_CYAN                 "\e[0;36m"
#define COLOR_L_CYAN               "\e[1;36m"
#define COLOR_GRAY                 "\e[0;37m"
#define COLOR_WHITE                "\e[1;37m"

#define COLOR_BOLD                 "\e[1m"
#define COLOR_UNDERLINE            "\e[4m"
#define COLOR_BLINK                "\e[5m"
#define COLOR_REVERSE              "\e[7m"
#define COLOR_HIDE                 "\e[8m"
#define COLOR_CLEAR                "\e[2J"
#define COLOR_CLRLINE              "\r\e[K" //or "\e[1K\r"

/*************************************************
 Function:		log_get_level
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int log_get_level(void);

/*************************************************
 Function:		log_set_level
 Descroption:	 
 Input: 
	1.int level
 Output: 
 Return: 	
 Other:  
*************************************************/
void log_set_level(unsigned int level);

/*************************************************
 Function:		log_set_filepath
 Descroption:	 
 Input: 
	1.path
 Output: 
 Return: 	
 Other:  
*************************************************/
void log_set_filepath(char* path);

/*************************************************
 Function:		log_write
 Descroption:	 
 Input: 
	1.char *fmt
	2.
 Output: 
 Return: 	
 Other:  
*************************************************/
void log_write(unsigned int level,const char *fmt, ...);


#if defined(_LINUX_)
#include <sys/time.h>

#define LOG(level,color,...) \
        log_write(level,__VA_ARGS__);\
        if(log_get_level()&level) { \
			struct timeval _log_time; \
			printf(color"["LOG_TAG"]: "); \
			printf(__VA_ARGS__);printf("\n");\
            printf(COLOR_WHITE);}   

#define LOGI(level,...) LOG(level,COLOR_YELLOW,__VA_ARGS__)
#define LOGD(level,...) LOG(level,COLOR_L_BLUE,__VA_ARGS__)
#define LOGE(level,...) LOG(level,COLOR_RED,__VA_ARGS__)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _LOG_H_ Edit By Txl 2018-6-22 */

