/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		net_utils.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-10-17
 Description: 		
 History: 			
 					
   1.Date:	 		2018-10-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "net_core.h"
#include "net_utils.h"
#include "crctable_osdep.h"

/*************************************************
 Function:		getFrequencyFromChannel
 Descroption:	 
 Input: 
	1.channel
 Output: 
 Return: 	
 Other:  
*************************************************/
int getFrequencyFromChannel(int channel)
{
	static int frequencies[] = {
		-1, // No channel 0
		2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462, 2467,
		2472, 2484, -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		-1, // Nothing from channel 15 to 34 (exclusive)
		5170, 5175, 5180, 5185, 5190, 5195, 5200, 5205, 5210, 5215, 5220, 5225,
		5230, 5235, 5240, 5245, 5250, 5255, 5260, 5265, 5270, 5275, 5280, 5285,
		5290, 5295, 5300, 5305, 5310, 5315, 5320, 5325, 5330, 5335, 5340, 5345,
		5350, 5355, 5360, 5365, 5370, 5375, 5380, 5385, 5390, 5395, 5400, 5405,
		5410, 5415, 5420, 5425, 5430, 5435, 5440, 5445, 5450, 5455, 5460, 5465,
		5470, 5475, 5480, 5485, 5490, 5495, 5500, 5505, 5510, 5515, 5520, 5525,
		5530, 5535, 5540, 5545, 5550, 5555, 5560, 5565, 5570, 5575, 5580, 5585,
		5590, 5595, 5600, 5605, 5610, 5615, 5620, 5625, 5630, 5635, 5640, 5645,
		5650, 5655, 5660, 5665, 5670, 5675, 5680, 5685, 5690, 5695, 5700, 5705,
		5710, 5715, 5720, 5725, 5730, 5735, 5740, 5745, 5750, 5755, 5760, 5765,
		5770, 5775, 5780, 5785, 5790, 5795, 5800, 5805, 5810, 5815, 5820, 5825,
		5830, 5835, 5840, 5845, 5850, 5855, 5860, 5865, 5870, 5875, 5880, 5885,
		5890, 5895, 5900, 5905, 5910, 5915, 5920, 5925, 5930, 5935, 5940, 5945,
		5950, 5955, 5960, 5965, 5970, 5975, 5980, 5985, 5990, 5995, 6000, 6005,
		6010, 6015, 6020, 6025, 6030, 6035, 6040, 6045, 6050, 6055, 6060, 6065,
		6070, 6075, 6080, 6085, 6090, 6095, 6100};

	return (channel > 0 && channel <= HIGHEST_CHANNEL)
			   ? frequencies[channel]
			   : (channel >= LOWEST_CHANNEL && channel <= -4)
					 ? 5000 - (channel * 5)
					 : -1;
}

/*************************************************
 Function:		getChannelFromFrequency
 Descroption:	 
 Input: 
	1.frequency
 Output: 
 Return: 	
 Other:  
*************************************************/
int getChannelFromFrequency(int frequency)
{
	if (frequency >= 2412 && frequency <= 2472)
		return (frequency - 2407) / 5;
	else if (frequency == 2484)
		return 14;

	else if (frequency >= 4920 && frequency <= 6100)
		return (frequency - 5000) / 5;
	else
		return -1;
}


/*************************************************
 Function:		net_phy_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int net_phy_lookup_index(char *name)
{
	char buf[200];
	int fd, pos;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);

	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return -1;
	pos = read(fd, buf, sizeof(buf) - 1);
	if (pos < 0) {
		close(fd);
		return -1;
	}
	buf[pos] = '\0';
	close(fd);
	return atoi(buf);
}

/*************************************************
 Function:		net_name_lookup_index
 Descroption:	 
 Input: 
	1.*name
 Output: 
 Return: 	
 Other:  
*************************************************/
int net_name_lookup_index(char *name)
{
    return if_nametoindex(name);
}

/*************************************************
 Function:		net_get_priv
 Descroption:	 
 Input: 
	1.net* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * net_get_priv(void* filter) 
{   
    struct netcore* handler = (struct netcore*)filter;
    return handler->priv; 
}

/*************************************************
 Function:		net_get_nl80211
 Descroption:	 
 Input: 
	1.net* filter
 Output: 
 Return: 	
 Other:  
*************************************************/
void * net_get_nl80211(void* filter) 
{
    struct netcore* handler = (struct netcore*)filter;
    return handler->n80211; 
}

/*************************************************
 Function:		net_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_ifname(void* filter) 
{
    struct netcore* handler = (struct netcore*)filter;
    return handler->interface;
}

/*************************************************
 Function:		net_get_ifname
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_monname(void* filter) 
{
    return MONITOR_NAME;
}

/*************************************************
 Function:		net_get_name
 Descroption:	 
 Input: 
	1.filter
 Output: 
 Return: 	
 Other:  
*************************************************/
char * net_get_name(void* filter) 
{
    struct netcore* handler = (struct netcore*)filter;
    if(handler->monitor)
    {
        return net_get_monname(filter);
    }
    
    return net_get_ifname(filter);
}

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
unsigned long calc_crc_osdep(unsigned char * buf, int len)
{
	unsigned long crc = 0xFFFFFFFF;

	for (; len > 0; len--, buf++)
		crc = crc_tbl_osdep[(crc ^ *buf) & 0xFF] ^ (crc >> 8);

	return (~crc);
}

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
int check_crc_buf_osdep(unsigned char * buf, int len)
{
	unsigned long crc;

	if (len < 0) return 0;

	crc = calc_crc_osdep(buf, len);
	buf += len;
	return (((crc) &0xFF) == buf[0] && ((crc >> 8) & 0xFF) == buf[1]
			&& ((crc >> 16) & 0xFF) == buf[2]
			&& ((crc >> 24) & 0xFF) == buf[3]);
}


/*************************************************
 Function:		find_monitor_mon
 Descroption:	 
 Input: 		None
 Output: 
 Return: 	
 Other:  
*************************************************/
int find_monitor_mon(void)
{
    return u_fileexist("/sys/class/net/Newland");    
}

