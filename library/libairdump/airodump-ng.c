/*********************************************************
 Copyright (C),2016-2022,Electronic Technology Co.,Ltd.
 File name: 		airodump-ng.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2019-1-4
 Description: 		
 History: 			
 					
   1.Date:	 		2019-1-4
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/

#include "netfilter_linux.h"
#include "airodump-ng.h"

const unsigned char llcnull[4] = {0, 0, 0, 0};
char * f_ext[NB_EXTENSIONS] = {AIRODUMP_NG_CSV_EXT,
							   AIRODUMP_NG_GPS_EXT,
							   AIRODUMP_NG_CAP_EXT,
							   IVS2_EXTENSION,
							   KISMET_CSV_EXT,
							   KISMET_NETXML_EXT};

/*************************************************
 Function:		is_filtered_netmask
 Descroption:	 
 Input: 
	1.Globals G
	2.char * bssid
 Output: 
 Return: 	
 Other:  
*************************************************/
static int is_filtered_netmask(struct Globals* G,unsigned char * bssid)
{
	unsigned char mac1[6];
	unsigned char mac2[6];
	int i;

	for (i = 0; i < 6; i++)
	{
		mac1[i] = bssid[i] & G->f_netmask[i];
		mac2[i] = G->f_bssid[i] & G->f_netmask[i];
	}

	if (memcmp(mac1, mac2, 6) != 0)
	{
		return (1);
	}

	return (0);
}

/*************************************************
 Function:		is_filtered_essid
 Descroption:	 
 Input: 
	1.Globals G
	2.char * essid
 Output: 
 Return: 	
 Other:  
*************************************************/
int is_filtered_essid(struct Globals* G,unsigned char * essid)
{
	int ret = 0;
	int i;

	if (G->f_essid)
	{
		for (i = 0; i < G->f_essid_count; i++)
		{
			if (strncmp((char *) essid, G->f_essid[i], MAX_IE_ELEMENT_SIZE) == 0)
			{
				return (0);
			}
		}

		ret = 1;
	}
	return (ret);
}

/*************************************************
 Function:		dump_add_packet
 Descroption:	 
 Input: 
	1.char * h80211
	2.caplen
	3.rx_info * ri
 Output: 
 Return: 	
 Other:  
*************************************************/
int dump_add_packet(unsigned char * h80211, int caplen,  struct rx_info * ri)
{
    int ret = -1;
    int seq;
    unsigned char bssid[6];
	unsigned char stmac[6];
	unsigned char namac[6];
    
    /* skip packets smaller than a 802.11 header */
	if (caplen < 24) goto write_exit;

    /* skip (uninteresting) control frames */
	if ((h80211[0] & 0x0C) == 0x04) goto write_exit;

    /* if it's a LLC null packet, just forget it (may change in the future) */
    if (caplen > 28)
	{
	    if (memcmp(h80211 + 24, llcnull, 4) == 0) return (0);
    }

    /* grab the sequence number */
	seq = ((h80211[22] >> 4) + (h80211[23] << 4));

	/* locate the access point's MAC address */

	switch (h80211[1] & 3)
	{
		case 0:
			memcpy(bssid, h80211 + 16, 6);
			break; // Adhoc
		case 1:
			memcpy(bssid, h80211 + 4, 6);
			break; // ToDS
		case 2:
			memcpy(bssid, h80211 + 10, 6);
			break; // FromDS
		case 3:
			memcpy(bssid, h80211 + 10, 6);
			break; // WDS -> Transmitter taken as BSSID
	}

	LOGD(_LOG_DRIVER_,"freq [%d] power [%d] bssid [%x-%x-%x-%x-%x-%x]",ri->ri_freq,ri->ri_power,bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5]);

write_exit:

    return ret;
}


int analysis_add_packet(struct Globals* G,unsigned char * h80211, int caplen,  struct rx_info * ri)
{
    int ret = -1;
    int seq;
    unsigned char bssid[6];
	unsigned char stmac[6];
	unsigned char namac[6];
    
    /* skip packets smaller than a 802.11 header */
	if (caplen < 24) goto write_exit;

    /* skip (uninteresting) control frames */
	if ((h80211[0] & 0x0C) == 0x04) goto write_exit;

    /* if it's a LLC null packet, just forget it (may change in the future) */
    if (caplen > 28)
	{
	    if (memcmp(h80211 + 24, llcnull, 4) == 0) return (0);
    }

    /* grab the sequence number */
	seq = ((h80211[22] >> 4) + (h80211[23] << 4));

	/* locate the access point's MAC address */

	switch (h80211[1] & 3)
	{
		case 0:
			memcpy(bssid, h80211 + 16, 6);
			break; // Adhoc
		case 1:
			memcpy(bssid, h80211 + 4, 6);
			break; // ToDS
		case 2:
			memcpy(bssid, h80211 + 10, 6);
			break; // FromDS
		case 3:
			memcpy(bssid, h80211 + 10, 6);
			break; // WDS -> Transmitter taken as BSSID
	}

    	if (memcmp(G->f_bssid, NULL_MAC, 6) != 0)
	{
		if (memcmp(G->f_netmask, NULL_MAC, 6) != 0)
		{
			if (is_filtered_netmask(G,bssid)) return (1);
		}
		else
		{
			if (memcmp(G->f_bssid, bssid, 6) != 0) return (1);
		}
	}
    
    
write_exit:

    return ret;
}



