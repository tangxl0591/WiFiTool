/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		net_utils.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-10-17
 Description: 		
 History: 			
 					
   1.Date:	 		2018-10-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

#include "utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HIGHEST_CHANNEL 220
#define LOWEST_CHANNEL -16

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
 Function:		getFrequencyFromChannel
 Descroption:	 
 Input: 
	1.channel
 Output: 
 Return: 	
 Other:  
*************************************************/
int getFrequencyFromChannel(int channel);

/*************************************************
 Function:		getChannelFromFrequency
 Descroption:	 
 Input: 
	1.frequency
 Output: 
 Return: 	
 Other:  
*************************************************/
int getChannelFromFrequency(int frequency);

/*************************************************
 Function:		net_phy_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int net_phy_lookup_index(char *name);

/*************************************************
 Function:		net_name_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int net_name_lookup_index(char *name);

/*************************************************
 Function:		net_get_priv
 Descroption:	 
 Input: 
	1.net* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * net_get_priv(void* filter);

/*************************************************
 Function:		net_get_nl80211
 Descroption:	 
 Input: 
	1.net* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * net_get_nl80211(void* filter);

/*************************************************
 Function:		net_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_ifname(void* filter); 

/*************************************************
 Function:		net_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_monname(void* filter);

/*************************************************
 Function:		net_get_name
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_name(void* filter);

/*************************************************
 Function:		calc_crc_osdep
 Descroption:	 
 Input: 
	1.char * buf
	2.len
 Output: 
 Return: 	
 Other:  
*************************************************/
unsigned long calc_crc_osdep(unsigned char * buf, int len);

/*************************************************
 Function:		check_crc_buf_osdep
 Descroption:	 
 Input: 
	1.char * buf
	2.len
 Output: 
 Return: 	
 Other:  
*************************************************/
int check_crc_buf_osdep(unsigned char * buf, int len);

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

/*************************************************
 Function:		find_monitor_mon
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int find_monitor_mon(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _NET_UTILS_H_ Edit By Txl 2018-10-17 */

