/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		netfilter_utils.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-10-17
 Description: 		
 History: 			
 					
   1.Date:	 		2018-10-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _NETFILTER_UTILS_H_
#define _NETFILTER_UTILS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HIGHEST_CHANNEL 220
#define LOWEST_CHANNEL -16

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
 Function:		netfilter_phy_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int netfilter_phy_lookup_index(char *name);

/*************************************************
 Function:		netfilter_name_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int netfilter_name_lookup_index(char *name);

/*************************************************
 Function:		netfilter_get_priv
 Descroption:	 
 Input: 
	1.netfilter* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * netfilter_get_priv(void* filter);

/*************************************************
 Function:		netfilter_get_nl80211
 Descroption:	 
 Input: 
	1.netfilter* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * netfilter_get_nl80211(void* filter);

/*************************************************
 Function:		netfilter_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * netfilter_get_ifname(void* filter); 

/*************************************************
 Function:		netfilter_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * netfilter_get_monname(void* filter);

/*************************************************
 Function:		netfilter_get_name
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * netfilter_get_name(void* filter);

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


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _NETFILTER_UTILS_H_ Edit By Txl 2018-10-17 */

