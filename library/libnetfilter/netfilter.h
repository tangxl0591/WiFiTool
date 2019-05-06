/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		netfilter.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-10-17
 Description: 		
 History: 			
 					
   1.Date:	 		2018-10-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _NETFILTER_H_
#define _NETFILTER_H_

#include <stdio.h>

#ifdef CONFIG_LIBNL
#include <linux/nl80211.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <linux/genetlink.h>
#endif // CONFIG_LIBNL

#include "byteorder.h"
#include "packed.h"
#include "netfilter_utils.h"
#include "nl80211.h"
#include "log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef IFNAMSIZ 
/** Maximum length of a interface name */
#define IFNAMSIZ 16
#endif

/* patch 2.4.x if_arp */
#ifndef ARPHRD_INFINIBAND
#define ARPHRD_INFINIBAND 32
#endif

#define MONITOR_NAME    "Newland"

struct tx_info
{
	unsigned int ti_rate;
};

struct rx_info
{
	uint64_t ri_mactime;
	int32_t ri_power;
	int32_t ri_noise;
	uint32_t ri_channel;
	uint32_t ri_freq;
	uint32_t ri_rate;
	uint32_t ri_antenna;
} __packed;

struct netfilter
{   
    int debug;
    int wi_id;
    int monitor;
    void * n80211;
	void * priv;
	char interface[IFNAMSIZ];  

   	int (*wi_read)(struct netfilter * filter,
				   unsigned char * h80211,
				   int len,
				   struct rx_info * ri);
	int (*wi_write)(struct netfilter * filter,
					unsigned char * h80211,
					int len,
					struct tx_info * ti);
	int (*wi_set_ht_channel)(struct netfilter * filter, int chan, unsigned int htval);
	int (*wi_set_channel)(struct netfilter * filter, int chan);
	int (*wi_get_channel)(struct netfilter * filter);
	int (*wi_set_freq)(struct netfilter * filter, int freq);
	int (*wi_get_freq)(struct netfilter * filter);
	void (*wi_close)(struct netfilter * filter);
	int (*wi_get_mac)(struct netfilter * filter, unsigned char * mac);
	int (*wi_set_mac)(struct netfilter * filter, unsigned char * mac);
	int (*wi_set_rate)(struct netfilter * filter, int rate);
	int (*wi_get_rate)(struct netfilter * filter);
	int (*wi_set_txpower)(struct netfilter * filter, int rate);
	int (*wi_get_txpower)(struct netfilter * filter);    
 	int (*wi_set_mtu)(struct netfilter * filter, int mtu);
	int (*wi_get_mtu)(struct netfilter * filter);

	int (*wi_get_monitor)(struct netfilter * filter);
    int (*wi_set_monitor)(struct netfilter * filter);

};

/*************************************************
 Function:		netfilter_init
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int netfilter_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _NETFILTER_H_ Edit By Txl 2018-10-17 */

