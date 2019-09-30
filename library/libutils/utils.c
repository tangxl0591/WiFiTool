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
#include "utils.h"

/*************************************************
 Function:		is_dir_exit
 Descroption:	 
 Input: 
	1.*path
 Output: 
 Return: 	
 Other:  
*************************************************/
int is_dir_exit(char *path)
{
	if (path == NULL)
	{
		return -1;
	}
	if (opendir(path) == NULL)
	{
		return -1;
	}
	
	return 0;
}

/*************************************************
 Function:		u_fileexist
 Descroption:	 
 Input: 
	1.*filename
 Output: 
 Return: 	
 Other:  
*************************************************/
int u_fileexist(char *filename)
{
    if(access(filename, F_OK) == 0)
        return 0;
    else 
        return -1;
}


/*************************************************
 Function:		find_wifi_classname
 Descroption:	 
 Input: 
	1.path
 Output: 
 Return: 	
 Other:  
*************************************************/
int find_wifi_classname(char* path, char *name)

{
    DIR *dir;
    struct dirent *ptr;
    int found = 0;

    if(name == NULL || path == NULL)
    {
        return -1;
    }
    
    if ((dir=opendir(path)) == NULL)
    {
        return -1;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0 || strcmp(ptr->d_name,"enp2s0")==0
            || strcmp(ptr->d_name,"lo")==0 || strcmp(ptr->d_name,"Newland")==0)
        {
            continue;
        }
        else 
        {
            if(name != NULL)
            {
                sprintf(name,"%s",ptr->d_name);
                found = 1;
            }
            LOGI(_LOG_COMMON_,"%s = [%s]\n",__func__,ptr->d_name);
            
        }
    }
    closedir(dir);
    if(found)
    {
        return 0;
    }
    return -1;
}

