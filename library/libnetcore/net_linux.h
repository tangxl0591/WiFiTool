/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		net_linux.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-12-27
 Description: 		
 History: 			
 					
   1.Date:	 		2018-12-27
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _NET_LINUX_H_
#define _NET_LINUX_H_

#include "net_core.h"
#include "net_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* nl80211 */
#ifdef CONFIG_LIBNL
struct nl80211_state
{
	struct nl_sock * nl_sock;
	struct nl_cache * nl_cache;
	struct genl_family * nl80211;
};
#endif

struct priv_linux
{
	int fd_in,arptype_in;

	int channel;
	int freq;
	int rate;
	int tx_power;
	unsigned char mac[6];

    int drivertype;
};

#ifndef ETH_P_80211_RAW
#define ETH_P_80211_RAW 25
#endif

#define ARPHRD_ETHERNET 1
#define ARPHRD_IEEE80211 801
#define ARPHRD_IEEE80211_PRISM 802
#define ARPHRD_IEEE80211_FULL 803

#ifndef NULL_MAC
#define NULL_MAC "\x00\x00\x00\x00\x00\x00"
#endif

/*************************************************
 Function:		linux_open
 Descroption:	 
 Input: 
	1.iface
 Output: 
 Return: 	
 Other:  
*************************************************/
struct netcore * linux_open(char * iface);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _NET_LINUX_H_ Edit By Txl 2018-12-27 */

