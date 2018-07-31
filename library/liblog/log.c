/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		log.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-22
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-22
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "log.h"

#define LOG_BUF_SIZE     1024
#define _LOG_LEVEL_   	 0xff 


static unsigned int mLogLevel = _LOG_LEVEL_;
static int klog_fd = -1;
static char log_path[100];
static int log_path_flag = 0;

/*************************************************
 Function:		log_get_level
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int log_get_level(void)
{
    return mLogLevel;
}

/*************************************************
 Function:		log_set_level
 Descroption:	 
 Input: 
	1.int level
 Output: 
 Return: 	
 Other:  
*************************************************/
void log_set_level(unsigned int level)
{
    mLogLevel = level;
}

/*************************************************
 Function:		log_set_filepath
 Descroption:	 
 Input: 
	1.path
 Output: 
 Return: 	
 Other:  
*************************************************/
void log_set_filepath(char* path)
{
    memset(log_path,0,sizeof(log_path));
    sprintf(log_path,"%s",path);
    log_path_flag = 1;
}

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
void log_write(unsigned int level,const char *fmt, ...)
{
    if((level&mLogLevel) && log_path_flag)
    {
        va_list ap;
        char buf[LOG_BUF_SIZE];
        int len = 0;

        va_start(ap, fmt);
        vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
        len = strlen(buf);
        if(buf[len-1] != '\n')
        {
        buf[len++] = '\n';
        }
        va_end(ap);
        if(klog_fd == -1)
        {
            klog_fd = open(log_path, O_CREAT|O_RDWR|O_APPEND,0666);
        }
        if(klog_fd > 0)
        {
            write(klog_fd, buf, len);
        }
    }
}

