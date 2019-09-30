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
#include "platform.h"
#include "net_core.h"
#include "airodump-ng.h"

const unsigned char llcnull[4] = {0, 0, 0, 0};
char * f_ext[NB_EXTENSIONS] = {AIRODUMP_NG_CSV_EXT,
							   AIRODUMP_NG_GPS_EXT,
							   AIRODUMP_NG_CAP_EXT,
							   IVS2_EXTENSION,
							   KISMET_CSV_EXT,
							   KISMET_NETXML_EXT};

const char * OUI_PATHS[] = {"./airodump-ng-oui.txt",
							"/etc/aircrack-ng/airodump-ng-oui.txt",
							"/usr/local/etc/aircrack-ng/airodump-ng-oui.txt",
							"/usr/share/aircrack-ng/airodump-ng-oui.txt",
							"/var/lib/misc/oui.txt",
							"/usr/share/misc/oui.txt",
							"/var/lib/ieee-data/oui.txt",
							"/usr/share/ieee-data/oui.txt",
							"/etc/manuf/oui.txt",
							"/usr/share/wireshark/wireshark/manuf/oui.txt",
							"/usr/share/wireshark/manuf/oui.txt",
							NULL};

int read_pkts = 0;
int abg_chans[]
	= {1,   7,   13,  2,   8,   3,   14,  9,   4,   10,  5,   11,  6,
	   12,  36,  38,  40,  42,  44,  46,  48,  50,  52,  54,  56,  58,
	   60,  62,  64,  100, 102, 104, 106, 108, 110, 112, 114, 116, 118,
	   120, 122, 124, 126, 128, 132, 134, 136, 138, 140, 142, 144, 149,
	   151, 153, 155, 157, 159, 161, 165, 169, 173, 0};
int bg_chans[] = {1, 7, 13, 2, 8, 3, 14, 9, 4, 10, 5, 11, 6, 12, 0};
int a_chans[] = {36,  38,  40,  42,  44,  46,  48,  50,  52,  54,  56,  58,
				 60,  62,  64,  100, 102, 104, 106, 108, 110, 112, 114, 116,
				 118, 120, 122, 124, 126, 128, 132, 134, 136, 138, 140, 142,
				 144, 149, 151, 153, 155, 157, 159, 161, 165, 169, 173, 0};
int * frequencies;

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
static int is_filtered_netmask(struct globals* G,unsigned char * bssid)
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
int is_filtered_essid(struct globals* G,unsigned char * essid)
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


/*************************************************
 Function:		analysis_add_packet
 Descroption:	 
 Input: 
	1.Globals* G
	2.char * h80211
	3.caplen
	4.rx_info * ri
 Output: 
 Return: 	
 Other:  
*************************************************/
int analysis_add_packet(struct globals* G,unsigned char * h80211, int caplen,  struct rx_info * ri)
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

/*************************************************
 Function:		resetSelection
 Descroption:	 
 Input: 
	1.Globals* G
 Output: 
 Return: 	
 Other:  
*************************************************/
static void resetSelection(struct globals* G)
{
	G->sort_by = SORT_BY_POWER;
	G->sort_inv = 1;

	G->start_print_ap = 1;
	G->start_print_sta = 1;
	G->selected_ap = 1;
	G->selected_sta = 1;
	G->selection_ap = 0;
	G->selection_sta = 0;
	G->mark_cur_ap = 0;
	G->skip_columns = 0;
	G->do_pause = 0;
	G->do_sort_always = 0;
	memset(G->selected_bssid, '\x00', 6);
}

/*************************************************
 Function:		analysis_init
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
struct globals* analysis_init(void)
{
	struct timeval tv0;
	struct timeval tv1;
	struct timeval tv2;
	struct timeval tv3;
	struct timeval tv4;
	struct tm * lt;

    struct globals* G = (struct globals*)malloc(sizeof(struct globals));
    if(G == NULL)
    {
        return NULL;
    }

    memset(G, 0, sizeof(struct globals));


	G->chanoption = 0;
	G->freqoption = 0;
	G->num_cards = 0;

	G->batt = NULL;
	G->chswitch = 0;
	G->usegpsd = 0;
	G->channels = bg_chans;
	G->one_beacon = 1;
	G->singlechan = 0;
	G->singlefreq = 0;
	G->dump_prefix = NULL;
	G->record_data = 0;
	G->f_cap = NULL;
	G->f_ivs = NULL;
	G->f_txt = NULL;
	G->f_kis = NULL;
	G->f_kis_xml = NULL;
	G->f_gps = NULL;
	G->keyout = NULL;
	G->f_xor = NULL;
	G->sk_len = 0;
	G->sk_len2 = 0;
	G->sk_start = 0;
	G->prefix = NULL;
	G->f_encrypt = 0;
	G->asso_client = 0;
	G->f_essid = NULL;
	G->f_essid_count = 0;
	G->active_scan_sim = 0;
	G->update_s = 0;
	G->decloak = 1;
	G->is_berlin = 0;
	G->numaps = 0;
	G->maxnumaps = 0;
	G->berlin = 120;
	G->show_ap = 1;
	G->show_sta = 1;
	G->show_ack = 0;
	G->hide_known = 0;
	G->maxsize_essid_seen = 5; // Initial value: length of "ESSID"
	G->show_manufacturer = 0;
	G->show_uptime = 0;
	G->hopfreq = DEFAULT_HOPFREQ;
	G->s_file = NULL;
	G->s_iface = NULL;
	G->f_cap_in = NULL;
	G->detect_anomaly = 0;
	G->airodump_start_time = NULL;
	G->manufList = NULL;

	G->output_format_pcap = 1;
	G->output_format_csv = 1;
	G->output_format_kismet_csv = 1;
	G->output_format_kismet_netxml = 1;
	G->file_write_interval = 5; // Write file every 5 seconds by default
	G->maxsize_wps_seen = 6;
	G->show_wps = 0;
	G->background_mode = -1;

    memset(G->sharedkey, '\x00', sizeof(G->sharedkey));
	memset(G->message, '\x00', sizeof(G->message));
	memset(&G->pfh_in, '\x00', sizeof(struct pcap_file_header));

	gettimeofday(&tv0, NULL);

	lt = localtime((time_t *) &tv0.tv_sec);

	G->keyout = (char *) malloc(512);
	memset(G->keyout, 0, 512);
	snprintf(G->keyout,
			 511,
			 "keyout-%02d%02d-%02d%02d%02d.keys",
			 lt->tm_mon + 1,
			 lt->tm_mday,
			 lt->tm_hour,
			 lt->tm_min,
			 lt->tm_sec);    

	memset(G->f_bssid, '\x00', 6);
	memset(G->f_netmask, '\x00', 6);
	memset(G->wpa_bssid, '\x00', 6);

    
    return G;
}

