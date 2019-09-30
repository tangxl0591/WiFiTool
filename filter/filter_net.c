/*********************************************************
 Copyright (C),2016-2022,Electronic Technology Co.,Ltd.
 File name: 		filter_net.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2019-9-17
 Description: 		
 History: 			
 					
   1.Date:	 		2019-9-17
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "filter_net.h"
#include "net_core.h"
#include "airodump-ng.h"


typedef struct FilterNetParam{
    struct netcore* mNetCore;
}FilterNetParam;

/*************************************************
 Function:		netcore_thread_proc
 Descroption:	 
 Input: 
	1.*ptr
 Output: 
 Return: 	
 Other:  
*************************************************/
static void *netcore_thread_proc(void *ptr)
{
	MSFilterDesc *f = (MSFilterDesc *)ptr;
	FilterNetParam *mnetfilter = (FilterNetParam*)f->private;
    struct netcore* mNetCore = mnetfilter->mNetCore;
    struct timeval tv0;
	unsigned char buffer[4096];
	unsigned char * h80211;    
    struct rx_info ri;    
    int fdh = 0;
    int caplen = 0;

    while (f->thread.thread_run)
    {
        fd_set rfds;
        fdh = mNetCore->wi_get_fd(mNetCore);
        FD_ZERO(&rfds);
        FD_SET(fdh, &rfds); 
        tv0.tv_sec = 0;
        tv0.tv_usec = 0;
        
        if (select((fdh + 1), &rfds, NULL, NULL, &tv0) < 0)
        {
            continue;
        }
        memset(buffer, 0, sizeof(buffer));
		h80211 = buffer;
		caplen = mNetCore->wi_read(mNetCore, h80211, sizeof(buffer), &ri);
		dump_add_packet(h80211,caplen, &ri);       
    }    

    pthread_detach(pthread_self());
	pthread_exit(NULL);    
    return NULL;
}

/*************************************************
 Function:		netcore_open
 Descroption:	 
 Input: 
	1._MSFilterDesc* f
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netcore_open(struct _MSFilterDesc *f, void *arg)
{   
    int found = 0;
    char name[50];
    FilterNetParam *param = NULL;
    
    if(NULL != f->private)
    {
        param = (FilterNetParam *)f->private;
        memset(name,0,50);
        found = find_wifi_classname("/sys/class/net",name);
        LOGD(_LOG_CORE_,"find_wifi_classname [%d]",found);
        if(found == 0)
        {
            param->mNetCore =  (struct netcore *)linux_open(name);
            ms_thread_init(&f->thread,1000);
    	    ms_thread_create(&f->thread, netcore_thread_proc, f);
        }
    }
    return 0;
}

/*************************************************
 Function:		netcore_close
 Descroption:	 
 Input: 
	1._MSFilterDesc *f
	2.*arg
 Output: 
 Return: 	
 Other:  
*************************************************/
static int netcore_close(struct _MSFilterDesc *f, void *arg)
{
    FilterNetParam *param = NULL;
    
    if(NULL != f->private)
    {
        param = (FilterNetParam *)f->private;
        ms_thread_quit(&f->thread);
        param->mNetCore->wi_close(param->mNetCore);
    }
    
    return 0;
}

/*************************************************
 Function:		ms_netcore_init
 Descroption:	 
 Input: 
	1._MSFilterDesc *f
 Output: 
 Return: 	
 Other:  
*************************************************/
static int ms_netcore_init(struct _MSFilterDesc *f)
{
	if(NULL == f->private)
    {
		FilterNetParam *data = (FilterNetParam*)malloc(sizeof(FilterNetParam));
		memset(data,0,sizeof(FilterNetParam));    
		f->private = data;
		f->mcount = 0;
	}

	return 0;
}

/*************************************************
 Function:		ms_netcore_uninit
 Descroption:	 
 Input: 
	1._MSFilterDesc *f
 Output: 
 Return: 	
 Other:  
*************************************************/
static int ms_netcore_uninit(struct _MSFilterDesc *f)
{
	if(f->mcount == 0)
	{
	    ms_thread_quit(&f->thread);
		ms_free(f->private);
		return 0;
	}

	return -1;
}

static MSFilterMethod methods[]={
    {MS_NET_CORE_OPEN,		netcore_open},
    {MS_NET_CORE_CLOSE,		netcore_close},        
	{0,						NULL			}
};

MSFilterDesc ms_netcore_desc={
	.id = MS_NET_CORE_ID,
	.name = "MsNetFilter",
	.text = "Net Core Library",
	.noutputs = 2,
	.outputs = NULL,
	.init = ms_netcore_init,
	.uninit = ms_netcore_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.mProcessMode = PROCESS_WORK_NONE,
	.private = NULL, 
};

