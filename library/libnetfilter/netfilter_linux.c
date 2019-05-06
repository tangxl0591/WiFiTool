/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		netfilter_linux.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-12-27
 Description: 		
 History: 			
 					
   1.Date:	 		2018-12-27
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netpacket/packet.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <netinet/in.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <net/if_arp.h>
#include <limits.h>

#include "netfilter_linux.h"
#include "radiotap.h"
#include "radiotap_iter.h"
#include "airodump-ng.h"

/*************************************************
 Function:		netfilter_error_handler
 Descroption:	 
 Input: 
	1.sockaddr_nl *nla
	2.nlmsgerr *err
	3.*arg
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netfilter_error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
			 void *arg)
{
	int *ret = arg;
    LOGD(_LOG_DRIVER_,"%s===========================",__func__);      
	*ret = err->error;
	return NL_STOP;
}

/*************************************************
 Function:		netfilter_finish_handler
 Descroption:	 
 Input: 
	1.nl_msg *msg
	2.*arg
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netfilter_finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
    LOGD(_LOG_DRIVER_,"%s===========================",__func__);  
	*ret = 0;
	return NL_SKIP;
}

/*************************************************
 Function:		netfilter_ack_handler
 Descroption:	 
 Input: 
	1.nl_msg *msg
	2.*arg
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netfilter_ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
    LOGD(_LOG_DRIVER_,"%s===========================",__func__);  
    return NL_STOP;
}

/*************************************************
 Function:		netfilter_valid_handler
 Descroption:	 
 Input: 
	1.nl_msg *msg
	2.*arg
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netfilter_valid_handler(struct nl_msg *msg, void *arg)
{
	LOGD(_LOG_DRIVER_,"%s===========================",__func__);  
	return NL_OK;
}

/*************************************************
 Function:		nl80211_init
 Descroption:	 
 Input: 
	1.nl80211_state * state
 Output: 
 Return: 	
 Other:  
*************************************************/
static int nl80211_init(struct nl80211_state * state)
{
	int err;

	state->nl_sock = nl_socket_alloc();

	if (!state->nl_sock)
	{
		LOGE(_LOG_DRIVER_, "Failed to allocate netlink socket.\n");
		return -ENOMEM;
	}

	if (genl_connect(state->nl_sock))
	{
		LOGE(_LOG_DRIVER_, "Failed to connect to generic netlink.\n");
		err = -ENOLINK;
		goto out_handle_destroy;
	}

	if (genl_ctrl_alloc_cache(state->nl_sock, &state->nl_cache))
	{
		LOGE(_LOG_DRIVER_, "Failed to allocate generic netlink cache.\n");
		err = -ENOMEM;
		goto out_handle_destroy;
	}

	state->nl80211 = genl_ctrl_search_by_name(state->nl_cache, "nl80211");
	if (!state->nl80211)
	{
		LOGE(_LOG_DRIVER_, "nl80211 not found.\n");
		err = -ENOENT;
		goto out_cache_free;
	}

	return 0;

out_cache_free:
	nl_cache_free(state->nl_cache);
out_handle_destroy:
	nl_socket_free(state->nl_sock);
	return err;
}

/*************************************************
 Function:		nl80211_cleanup
 Descroption:	 
 Input: 
	1.nl80211_state * state
 Output: 
 Return: 	
 Other:  
*************************************************/
static void nl80211_cleanup(struct nl80211_state * state)
{
	genl_family_put(state->nl80211);
	nl_cache_free(state->nl_cache);
	nl_socket_free(state->nl_sock);
}

/*************************************************
 Function:		linux_fd
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_fd(struct netfilter * filter)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	return pl->fd_in;
}

/*************************************************
 Function:		linux_get_freq
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_freq(struct netfilter * filter)
{
	struct priv_linux * dev = netfilter_get_priv(filter);
	struct iwreq wrq;
	int fd, frequency;

	memset(&wrq, 0, sizeof(struct iwreq));

	strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	fd = dev->fd_in;
	if (ioctl(fd, SIOCGIWFREQ, &wrq) < 0) return (-1);

	frequency = wrq.u.freq.m;
	if (frequency > 100000000)
		frequency /= 100000;
	else if (frequency > 1000000)
		frequency /= 1000;

	if (frequency < 500) 
		frequency = getFrequencyFromChannel(frequency);

	return frequency;
}

/*************************************************
 Function:		linux_set_freq
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.freq
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_set_freq(struct netfilter * filter, int freq)
{
	struct priv_linux * dev = netfilter_get_priv(filter);
	char s[32];
	int pid, status, unused;
	struct iwreq wrq;

	memset(s, 0, sizeof(s));

	memset(&wrq, 0, sizeof(struct iwreq));
	strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	wrq.u.freq.m = (double) freq * 100000;
	wrq.u.freq.e = (double) 1;

	if (ioctl(dev->fd_in, SIOCSIWFREQ, &wrq) < 0)
	{
		usleep(10000); /* madwifi needs a second chance */

		if (ioctl(dev->fd_in, SIOCSIWFREQ, &wrq) < 0)
		{
			LOGE(_LOG_DRIVER_, "ioctl(SIOCSIWFREQ) failed" );
			return (1);
		}
	}

	dev->freq = freq;
	return (0);
}


/*************************************************
 Function:		linux_get_mac
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.char * mac
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_mac(struct netfilter * filter, unsigned char * mac)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct ifreq ifr;
	int fd;

	fd = pl->fd_in;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, netfilter_get_ifname(filter), sizeof(ifr.ifr_name) - 1);

	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFINDEX) failed");
		return (1);
	}

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFHWADDR) failed");
		return (1);
	}

	memcpy(pl->mac, (unsigned char *) ifr.ifr_hwaddr.sa_data, 6);
    if(NULL != mac)
    {
	    memcpy(mac, pl->mac, 6);
    }
	return 0;
}

/*************************************************
 Function:		linux_set_mac
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.char * mac
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_set_mac(struct netfilter * filter, unsigned char * mac)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct ifreq ifr;
	int fd, ret;

	fd = linux_fd(filter);

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, netfilter_get_ifname(filter), sizeof(ifr.ifr_name) - 1);

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFHWADDR) failed");
		return (1);
	}

	ifr.ifr_flags &= ~(IFF_UP | IFF_BROADCAST | IFF_RUNNING);

	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCSIFFLAGS) failed");
		return (1);
	}

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);
	memcpy(pl->mac, mac, 6);

	// set mac
	ret = ioctl(fd, SIOCSIFHWADDR, &ifr);

	// if up
	ifr.ifr_flags |= IFF_UP | IFF_BROADCAST | IFF_RUNNING;

	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCSIFFLAGS) failed");
		return (1);
	}

	return ret;
}

/*************************************************
 Function:		linux_get_monitor
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_monitor(struct netfilter * filter)
{
	struct priv_linux * dev = netfilter_get_priv(filter);
	struct ifreq ifr;
	struct iwreq wrq;

	memset(&ifr, 0, sizeof(ifr));
    if(filter->monitor)
    {
	    strncpy(ifr.ifr_name, netfilter_get_monname(filter), sizeof(ifr.ifr_name) - 1);
    }
    else
    {
        strncpy(ifr.ifr_name, netfilter_get_ifname(filter), sizeof(ifr.ifr_name) - 1);
    }
    if( ioctl(linux_fd(filter), SIOCGIFINDEX, &ifr ) < 0 )
    {
        LOGE(_LOG_DRIVER_, "ioctl(SIOCGIFINDEX) failed" );
        return(-1);
    }

	if (ioctl(linux_fd(filter), SIOCGIFHWADDR, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFHWADDR) failed");
		return (-1);
	}

	memset(&wrq, 0, sizeof(struct iwreq));
    if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(linux_fd(filter), SIOCGIWMODE, &wrq) < 0)
	{
		wrq.u.mode = IW_MODE_MONITOR;
	}

	if ((ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211
		 && ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_PRISM
		 && ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_FULL)
		|| (wrq.u.mode != IW_MODE_MONITOR))
	{
		return (-1);
	}

	return (0);
}

/*************************************************
 Function:		nl80211_set_monitor
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int nl80211_set_monitor(struct netfilter * filter)
{
    struct nl_cb *cb;
    struct nl_cb *s_cb;
    int err;
    int ret;
    int flags = 0;
    struct nl80211_state * nl80211 = NULL;
    enum nl80211_commands cmd = NL80211_CMD_NEW_INTERFACE; 

    if(filter == NULL)
    {
        return -1;
    }

    nl80211 = filter->n80211;
    if(NULL == nl80211)
    {
        return -1;
    }
    
    struct nl_msg* msg = nlmsg_alloc();
    if(filter->debug)
    {
        cb = nl_cb_alloc(NL_CB_DEBUG);
        s_cb = nl_cb_alloc(NL_CB_DEBUG);
        nl_socket_set_cb(nl80211->nl_sock, s_cb);
    }
 
    int ifIndex = netfilter_name_lookup_index(filter->interface);

    genlmsg_put(msg, 0, 0, genl_family_get_id(nl80211->nl80211), 0, flags, cmd, 0);
    NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);
    NLA_PUT_STRING(msg, NL80211_ATTR_IFNAME, MONITOR_NAME);
    NLA_PUT_U32(msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_MONITOR);

    ret = nl_send_auto_complete(nl80211->nl_sock, msg);
    if(filter->debug)
    {
        err = 1;
    	nl_cb_err(cb, NL_CB_CUSTOM, netfilter_error_handler, &err);
    	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, netfilter_finish_handler, &err);
    	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, netfilter_ack_handler, &err);
    	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, netfilter_valid_handler, NULL);
    }

	while (err > 0)
		nl_recvmsgs(nl80211->nl_sock, cb);

    filter->monitor = 1;
    
 out:
	nl_cb_put(cb);
	nlmsg_free(msg);

    return 0;

nla_put_failure:
    nlmsg_free(msg);
    return 1;    
}

/*************************************************
 Function:		linux_set_rate
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.rate
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_set_rate(struct netfilter * filter, int rate)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct ifreq ifr;
	struct iwreq wrq;
    int fd = linux_fd(filter);

	memset(&wrq, 0, sizeof(struct iwreq));

	if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	wrq.u.bitrate.value = rate;
	wrq.u.bitrate.fixed = 1;

	if (ioctl(fd, SIOCSIWRATE, &wrq) < 0)
	{
		return (-1);
	}

	return 0;
}

/*************************************************
 Function:		linux_get_rate
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_rate(struct netfilter * filter)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct iwreq wrq;
    int fd = linux_fd(filter);

	memset(&wrq, 0, sizeof(struct iwreq));

	if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }	
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(fd, SIOCGIWRATE, &wrq) < 0)
	{
		return (-1);
	}

	return wrq.u.bitrate.value;
}

/*************************************************
 Function:		linux_set_txpower
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.power
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_set_txpower(struct netfilter * filter, int power)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct iwreq wrq;
    int ret = -1;
    int fd = linux_fd(filter);

	memset(&wrq, 0, sizeof(struct iwreq));

    if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }

	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	wrq.u.txpower.value = power;
	wrq.u.txpower.fixed = 0;

    ret = ioctl(fd, SIOCSIWTXPOW, &wrq);
	if (ret < 0)
	{
	    LOGE(_LOG_DRIVER_,"ioctl(SIOCSIWTXPOW) failed");
		return (-1);
	}
    pl->tx_power = power;
	return 0;
}

/*************************************************
 Function:		linux_get_txpower
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_txpower(struct netfilter * filter)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct iwreq wrq;
    int fd = linux_fd(filter);

	memset(&wrq, 0, sizeof(struct iwreq));

    if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(fd, SIOCGIWTXPOW, &wrq) < 0)
	{
		return (-1);
	}

	return wrq.u.txpower.value;
}

/*************************************************
 Function:		linux_get_channel
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_get_channel(struct netfilter * filter)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct iwreq wrq;
	int fd, frequency;
	int chan = 0;

	memset(&wrq, 0, sizeof(struct iwreq));

	if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	fd = linux_fd(filter);

	if (ioctl(fd, SIOCGIWFREQ, &wrq) < 0) 
    {
        return (-1);
    }

	frequency = wrq.u.freq.m;
	if (frequency > 100000000)
		frequency /= 100000;
	else if (frequency > 1000000)
		frequency /= 1000;

	if (frequency > 1000)
		chan = getChannelFromFrequency(frequency);
	else
		chan = frequency;

	return chan;
}


/*************************************************
 Function:		linux_set_channel
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.channel
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_set_channel(struct netfilter * filter, int channel)
{
	struct priv_linux * pl = netfilter_get_priv(filter);
	struct iwreq wrq;
    int fd = linux_fd(filter);

	memset(&wrq, 0, sizeof(struct iwreq));
	if(filter->monitor)
	{
	    strncpy(wrq.ifr_name, netfilter_get_monname(filter), IFNAMSIZ);
    }
    else
    {
        strncpy(wrq.ifr_name, netfilter_get_ifname(filter), IFNAMSIZ);
    }
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	wrq.u.freq.m = (double) channel;
	wrq.u.freq.e = (double) 0;

	if (ioctl(fd, SIOCSIWFREQ, &wrq) < 0)
	{
		usleep(10000); /* madwifi needs a second chance */

		if (ioctl(fd, SIOCSIWFREQ, &wrq) < 0)
		{
			LOGE(_LOG_DRIVER_, "ioctl(SIOCSIWFREQ) failed" );
			return (1);
		}
	}
	pl->channel = channel;
	return (0);
}


/*************************************************
 Function:		linux_read
 Descroption:	 
 Input: 
	1.netfilter * filter
	2.char * buf
	3.count
	4.rx_info * ri
 Output: 
 Return: 	
 Other:  
*************************************************/
static int linux_read(struct netfilter * filter, unsigned char * buf, int count, struct rx_info * ri)
{
	struct priv_linux * dev = netfilter_get_priv(filter);
	unsigned char tmpbuf[4096];

	int caplen, n, got_signal, got_noise, got_channel, fcs_removed;

	caplen = n = got_signal = got_noise = got_channel = fcs_removed = 0;

	if ((unsigned) count > sizeof(tmpbuf)) return (-1);

	if ((caplen = read(dev->fd_in, tmpbuf, count)) < 0)
	{
		if (errno == EAGAIN) return (0);

		LOGE(_LOG_DRIVER_,"read failed");
		return (-1);
	}

	memset(buf, 0, count);

	if (ri) 
    {
        memset(ri, 0, sizeof(*ri));
    }
    
	if (dev->arptype_in == ARPHRD_IEEE80211_PRISM)
	{
		/* skip the prism header */
		if (tmpbuf[7] == 0x40)
		{
			/* prism54 uses a different format */
			if (ri)
			{
				ri->ri_power = tmpbuf[0x33];
				ri->ri_noise = *(unsigned int *) (tmpbuf + 0x33 + 12);
				ri->ri_rate = (*(unsigned int *) (tmpbuf + 0x33 + 24)) * 500000;

				got_signal = 1;
				got_noise = 1;
			}

			n = 0x40;
		}
		else
		{
			if (ri)
			{
				ri->ri_mactime = *(u_int64_t *) (tmpbuf + 0x5C - 48);
				ri->ri_channel = *(unsigned int *) (tmpbuf + 0x5C - 36);
				ri->ri_power = *(unsigned int *) (tmpbuf + 0x5C);
				ri->ri_noise = *(unsigned int *) (tmpbuf + 0x5C + 12);
				ri->ri_rate = (*(unsigned int *) (tmpbuf + 0x5C + 24)) * 500000;

				got_channel = 1;
				got_signal = 1;
				got_noise = 1;
			}

			n = *(int *) (tmpbuf + 4);
		}

		if (n < 8 || n >= caplen) return (0);
	}

	if (dev->arptype_in == ARPHRD_IEEE80211_FULL)
	{
		struct ieee80211_radiotap_iterator iterator;
		struct ieee80211_radiotap_header * rthdr;

		rthdr = (struct ieee80211_radiotap_header *) tmpbuf;

		if (ieee80211_radiotap_iterator_init(&iterator, rthdr, caplen, NULL)
			< 0)
			return (0);

		/* go through the radiotap arguments we have been given
		 * by the driver
		 */

		while (ri && (ieee80211_radiotap_iterator_next(&iterator) >= 0))
		{

			switch (iterator.this_arg_index)
			{

				case IEEE80211_RADIOTAP_TSFT:
					ri->ri_mactime
						= le64_to_cpu(*((uint64_t *) iterator.this_arg));
					break;

				case IEEE80211_RADIOTAP_DBM_ANTSIGNAL:
					if (!got_signal)
					{
						if (*iterator.this_arg < 127)
							ri->ri_power = *iterator.this_arg;
						else
							ri->ri_power = *iterator.this_arg - 255;

						got_signal = 1;
					}
					break;

				case IEEE80211_RADIOTAP_DB_ANTSIGNAL:
					if (!got_signal)
					{
						if (*iterator.this_arg < 127)
							ri->ri_power = *iterator.this_arg;
						else
							ri->ri_power = *iterator.this_arg - 255;

						got_signal = 1;
					}
					break;

				case IEEE80211_RADIOTAP_DBM_ANTNOISE:
					if (!got_noise)
					{
						if (*iterator.this_arg < 127)
							ri->ri_noise = *iterator.this_arg;
						else
							ri->ri_noise = *iterator.this_arg - 255;

						got_noise = 1;
					}
					break;

				case IEEE80211_RADIOTAP_DB_ANTNOISE:
					if (!got_noise)
					{
						if (*iterator.this_arg < 127)
							ri->ri_noise = *iterator.this_arg;
						else
							ri->ri_noise = *iterator.this_arg - 255;

						got_noise = 1;
					}
					break;

				case IEEE80211_RADIOTAP_ANTENNA:
					ri->ri_antenna = *iterator.this_arg;
					break;

				case IEEE80211_RADIOTAP_CHANNEL:
					ri->ri_channel = getChannelFromFrequency(
						le16toh(*(uint16_t *) iterator.this_arg));
					got_channel = 1;
					break;

				case IEEE80211_RADIOTAP_RATE:
					ri->ri_rate = (*iterator.this_arg) * 500000;
					break;

				case IEEE80211_RADIOTAP_FLAGS:
					/* is the CRC visible at the end?
				 * remove
				 */
					if (*iterator.this_arg & IEEE80211_RADIOTAP_F_FCS)
					{
						fcs_removed = 1;
						caplen -= 4;
					}

					if (*iterator.this_arg & IEEE80211_RADIOTAP_F_BADFCS)
						return (0);

					break;
			}
		}

		n = le16_to_cpu(rthdr->it_len);

		if (n <= 0 || n >= caplen) return (0);
	}

	caplen -= n;

	// detect fcs at the end, even if the flag wasn't set and remove it
	if (fcs_removed == 0 && check_crc_buf_osdep(tmpbuf + n, caplen - 4) == 1)
	{
		caplen -= 4;
	}

	memcpy(buf, tmpbuf + n, caplen);

	if (ri && !got_channel) ri->ri_channel = linux_get_channel(filter);

	return (caplen);
}

/*************************************************
 Function:		openraw
 Descroption:	 
 Input: 
	1.priv_linux * dev
	2.* iface
	3.fd
	4.* arptype
	5.char * mac
 Output: 
 Return: 	
 Other:  
*************************************************/
static int openraw(struct netfilter * filter, char * iface, int fd, int * arptype, unsigned char * mac)
{
	struct ifreq ifr;
	struct ifreq ifr2;
	struct iwreq wrq;
	struct iwreq wrq2;
	struct packet_mreq mr;
	struct sockaddr_ll sll;
	struct sockaddr_ll sll2;
    struct priv_linux * dev = netfilter_get_priv(filter);

	if (iface == NULL || strlen(iface) >= sizeof(ifr.ifr_name))
	{
		LOGE(_LOG_DRIVER_,"Interface name too long: %s\n", iface);
		return (1);
	}

	/* find the interface index */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);

	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFINDEX) failed");
		return (1);
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;

	switch (dev->drivertype)
	{
		default:
			sll.sll_protocol = htons(ETH_P_ALL);
			break;
	}

	/* lookup the hardware type */

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFHWADDR) failed");
		return (1);
	}

	/* lookup iw mode */
	memset(&wrq, 0, sizeof(struct iwreq));
	strncpy(wrq.ifr_name, iface, IFNAMSIZ);
	wrq.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(fd, SIOCGIWMODE, &wrq) < 0)
	{
		/* most probably not supported (ie for rtap ipw interface) *
		 * so just assume its correctly set...                     */
		wrq.u.mode = IW_MODE_MONITOR;
	}

//	if ((ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211
//		 && ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_PRISM
//		 && ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_FULL)
//		|| (wrq.u.mode != IW_MODE_MONITOR))
//	{

//        ifr.ifr_flags &= ~(IFF_UP | IFF_BROADCAST | IFF_RUNNING);
//		if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
//		{
//			LOGE(_LOG_DRIVER_,"ioctl(SIOCSIFFLAGS) failed");
//			return (1);
//		}
//    
//		if (nl80211_set_monitor(filter))
//		{
//            LOGE(_LOG_DRIVER_,"Error setting monitor mode on %s\n", iface);
//			return (1);
//		}
//	}

	/* Is interface st to up, broadcast & running ? */
	if ((ifr.ifr_flags | IFF_UP | IFF_BROADCAST | IFF_RUNNING) != ifr.ifr_flags)
	{
		/* Bring interface up*/
		ifr.ifr_flags |= IFF_UP | IFF_BROADCAST | IFF_RUNNING;

		if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
		{
			LOGE(_LOG_DRIVER_,"ioctl(SIOCSIFFLAGS) failed");
			return (1);
		}
	}
	/* bind the raw socket to the interface */

	if (bind(fd, (struct sockaddr *) &sll, sizeof(sll)) < 0)
	{
		LOGE(_LOG_DRIVER_,"bind(ETH_P_ALL) failed");
		return (1);
	}

	/* lookup the hardware type */

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		LOGE(_LOG_DRIVER_,"ioctl(SIOCGIFHWADDR) failed");
		return (1);
	}

	memcpy(mac, (unsigned char *) ifr.ifr_hwaddr.sa_data, 6);

	*arptype = ifr.ifr_hwaddr.sa_family;

    LOGD(_LOG_DRIVER_,"%s ================ [%d]",__func__,*arptype);
    

	if (ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211
		&& ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_PRISM
		&& ifr.ifr_hwaddr.sa_family != ARPHRD_IEEE80211_FULL)
	{
		if (ifr.ifr_hwaddr.sa_family == ARPHRD_ETHERNET)
		{
		    LOGD(_LOG_DRIVER_,"ARP linktype is set to 1 (Ethernet) ");
        }
		else
		{
		    LOGD(_LOG_DRIVER_,"Unsupported hardware link type %4d ", ifr.ifr_hwaddr.sa_family);
        }
		return (1);
	}

	/* enable promiscuous mode */

	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = sll.sll_ifindex;
	mr.mr_type = PACKET_MR_PROMISC;

	if (setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0)
	{
		LOGE(_LOG_DRIVER_,"setsockopt(PACKET_MR_PROMISC) failed");
		return (1);
	}

	return (0);
}

/*************************************************
 Function:		do_linux_open
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int do_linux_open(struct netfilter * filter)
{
    struct priv_linux *dev = (struct priv_linux *)filter->priv;   

    if(NULL == filter)
    {
        return -1;
    }

    dev = (struct priv_linux *)filter->priv;   

    if(NULL == dev)
    {
        return -1;
    }
    
    /* open raw socks */
	if ((dev->fd_in = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		LOGE(_LOG_DRIVER_,"socket(PF_PACKET) failed");
		return -1;
	}

	nl80211_set_monitor(filter);    
    openraw(filter, netfilter_get_monname(filter), dev->fd_in, &dev->arptype_in, dev->mac);
    
    return 0;
}

/*************************************************
 Function:		do_linux_close
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static int do_linux_close(struct netfilter * filter)
{
    struct priv_linux *dev = (struct priv_linux *)filter->priv;   

    if(NULL == filter)
    {
        return -1;
    }

    dev = (struct priv_linux *)filter->priv;   

    if(NULL == dev)
    {
        return 0;
    }

    if(dev->fd_in)
    {
        close(dev->fd_in);
        dev->fd_in = 0;
    }

    free(dev);
    dev = NULL;
    
    return 0;
}

/*************************************************
 Function:		linux_close_nl80211
 Descroption:	 
 Input: 
	1.netfilter * filter
 Output: 
 Return: 	
 Other:  
*************************************************/
static void linux_close_nl80211(struct netfilter * filter)
{
    struct nl80211_state * nl80211 = (struct nl80211_state *)netfilter_get_nl80211(filter);
	nl80211_cleanup(nl80211);
    free(nl80211);
    nl80211 = NULL;
    do_linux_close(filter);
}

void imrotate(char* src, int width, int height)
{
	int i,j;
	int maxindex = width*height;
	int index = 0;
	char *buf = malloc((sizeof(char)*maxindex));
	if(buf)
	{
		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				buf[(j+1)*height-i-1] = src[j+i*width];
			}
		}
	}
	memcpy(src,buf,maxindex);
}

/*************************************************
 Function:		linux_open
 Descroption:	 
 Input: 
	1.iface
 Output: 
 Return: 	
 Other:  
*************************************************/
static struct netfilter * linux_open(char * iface)
{
    struct nl80211_state* nl80211 = NULL;
    struct netfilter * mnetfilter = NULL;
    struct priv_linux* mPrvLinux = NULL;

	if (iface == NULL)
	{
		return NULL;
	}

    nl80211 = (struct nl80211_state*)malloc(sizeof(struct nl80211_state));
    if(NULL == nl80211)
    {
        return NULL;
    }

    mPrvLinux = (struct priv_linux*)malloc(sizeof(struct priv_linux));
    if(NULL == mPrvLinux)
    {
        free(nl80211);
        nl80211 = NULL;    
        return NULL;
    }
    
	mnetfilter = (struct netfilter *)malloc(sizeof(struct netfilter));
	if (!mnetfilter) 
    {
        free(mPrvLinux);
        mPrvLinux = NULL;

        free(nl80211);
        nl80211 = NULL;
        return NULL;
    }
    memset(mnetfilter, 0, (sizeof(struct netfilter)));
    mnetfilter->n80211 = (void*)nl80211;
    mnetfilter->priv = (void*)mPrvLinux;
    mnetfilter->debug = 1;
    sprintf(mnetfilter->interface,"%s",iface);

    #ifdef CONFIG_LIBNL
	nl80211_init(nl80211);
    #endif 
    
    mnetfilter->wi_close = linux_close_nl80211;
    mnetfilter->wi_set_monitor = nl80211_set_monitor;
    mnetfilter->wi_get_monitor = linux_get_monitor;
    mnetfilter->wi_set_freq = linux_set_freq;
    mnetfilter->wi_get_freq = linux_get_freq;
    mnetfilter->wi_get_mac = linux_get_mac;    
    mnetfilter->wi_set_mac = linux_set_mac;    
    mnetfilter->wi_set_rate = linux_set_rate;    
    mnetfilter->wi_get_rate = linux_get_rate;    
    mnetfilter->wi_set_txpower = linux_set_txpower;      
    mnetfilter->wi_get_txpower = linux_get_txpower;     
    mnetfilter->wi_set_channel = linux_set_channel;      
    mnetfilter->wi_get_channel = linux_get_channel;   
    mnetfilter->wi_read = linux_read;

    
    if(0 != do_linux_open(mnetfilter))
    {
        linux_close_nl80211(mnetfilter);
        do_linux_close(mnetfilter);
        return NULL;
    }

    int freq = linux_get_freq(mnetfilter);
    LOGD(_LOG_DRIVER_,"linux_get_freq ============== [%d]",freq);
    int ret = linux_get_mac(mnetfilter, NULL);
    LOGD(_LOG_DRIVER_,"linux_get_mac ==11============ [%x:%x:%x:%x:%x:%x]",mPrvLinux->mac[0],mPrvLinux->mac[1],mPrvLinux->mac[2]
        ,mPrvLinux->mac[3],mPrvLinux->mac[4],mPrvLinux->mac[5]);
    ret = linux_get_monitor(mnetfilter);
    LOGD(_LOG_DRIVER_,"linux_get_monitor ============== [%d]",ret);

    int txpower = linux_get_txpower(mnetfilter);
    LOGD(_LOG_DRIVER_,"linux_get_txpower ============== [%d]",txpower);

    linux_set_channel(mnetfilter,1);
        
    txpower = linux_get_channel(mnetfilter);
    LOGD(_LOG_DRIVER_,"linux_get_channel ============== [%d]",txpower);
    
    int caplen = 0;
    struct timeval tv0;
	unsigned char buffer[4096];
	unsigned char * h80211;    
    struct rx_info ri;    
    int fdh = 0;
    #if 0    
    while(1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(linux_fd(mnetfilter), &rfds);
        fdh = linux_fd(mnetfilter);
        tv0.tv_sec = 0;
        tv0.tv_usec = 0;
        
        if (select((fdh + 1), &rfds, NULL, NULL, &tv0) < 0)
        {
            continue;
        }
        memset(buffer, 0, sizeof(buffer));
		h80211 = buffer;
		caplen = linux_read(mnetfilter, h80211, sizeof(buffer), &ri);
		dump_add_packet(h80211,caplen, &ri);        
    }
    #endif
    int i;
    char  buf[15] = {0};
    for(i = 0; i < 15; i++)
    {
        buf[i] = i+1;
        LOGD(_LOG_DRIVER_,"%d",buf[i]);
        if((i+1)%5 == 0)
        {
            LOGD(_LOG_DRIVER_,"\n");    
        }
    }
    
    imrotate(buf, 5, 3);
     for(i = 0; i < 15; i++)
      {
        LOGD(_LOG_DRIVER_,"%d",buf[i]);
        if((i+1)%3 == 0)
        {
            LOGD(_LOG_DRIVER_,"\n");    
        }
    }
      
    
    
    
    return mnetfilter;
}

/*************************************************
 Function:		netfilter_open
 Descroption:	 
 Input: 
	1.* iface
 Output: 
 Return: 	
 Other:  
*************************************************/
struct netfilter * netfilter_open(char * iface) 
{     
    return linux_open(iface);
}

