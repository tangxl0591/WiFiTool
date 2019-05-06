/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		main.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-6-24
 Description: 		
 History: 			
 					
   1.Date:	 		2018-6-24
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "config.h"
#include "log.h"
#include "netfilter.h"


/*************************************************
 Function:		main
 Descroption:	 
 Input: 
	1.argc
	2.*argv[]
 Output:  
 Return: 	
 Other:  
*************************************************/
int main(int argc,char *argv[])  
{ 
    int i = 10;
    LOGI(_LOG_LOGIC_, "**************************************************");
    LOGI(_LOG_LOGIC_, "Build Version: %s",_BUILD_VERSION_);
    LOGI(_LOG_LOGIC_, "Build Time   : %s",_BUILD_TIME_);
    LOGI(_LOG_LOGIC_, "**************************************************");    

    netfilter_init();
    
    return 0;
}
