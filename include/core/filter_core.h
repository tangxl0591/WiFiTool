/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		filter_core.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-8-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-8-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _FILTER_CORE_H_
#define _FILTER_CORE_H_

#include "platform.h"
#include "filter_id.h"
#include "list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*******************************************************/
/*                        Define                       */ 
/*******************************************************/

#define __ms_malloc	        malloc
#define ms_realloc	        realloc
#define ms_free		        free
#define ms_strdup	        strdup
#define ms_strndup	        strndup
#define ms_new(type,count)	(type*)__ms_malloc(sizeof(type)*(count))

#define ms_return_val_if_fail(_expr_,_ret_)\
	if (!(_expr_)) { LOGE(_LOG_COMMON_,"assert "#_expr_ "failed"); return (_ret_);}

#define ms_return_if_fail(_expr_) \
	if (!(_expr_)){ LOGE(_LOG_COMMON_,"assert "#_expr_ "failed"); return ;}

#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))
/*******************************************************/
/*                        Thread                       */ 
/*******************************************************/

struct _MSThread
{
	pthread_t thread;
	unsigned int thread_run;
	unsigned int thread_exittimer;
};

typedef struct _MSThread MSThread;

int __ms_thread_quit(MSThread* thread);
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void *arg);
int __ms_thread_init(MSThread* thread, int exittimer);

#define ms_thread_create	__ms_thread_create
#define ms_thread_join		pthread_join
#define ms_thread_exit		pthread_exit
#define ms_thread_quit		__ms_thread_quit
#define ms_thread_init		__ms_thread_init
#define ms_mutex_init		pthread_mutex_init
#define ms_mutex_lock		pthread_mutex_lock
#define ms_mutex_unlock		pthread_mutex_unlock
#define ms_mutex_destroy	pthread_mutex_destroy
#define ms_cond_init		pthread_cond_init
#define ms_cond_signal		pthread_cond_signal
#define ms_cond_broadcast	pthread_cond_broadcast
#define ms_cond_wait		pthread_cond_wait
#define ms_cond_destroy		pthread_cond_destroy

#define ms_process_lock(f)	 ms_mutex_lock(&(f)->lockprocess)
#define ms_process_unlock(f) ms_mutex_unlock(&(f)->lockprocess)

#define ms_queue_lock(f)	 ms_mutex_lock(&(f)->lockqueue)
#define ms_queue_unlock(f) 	 ms_mutex_unlock(&(f)->lockqueue)

/*******************************************************/
/*                        List                         */ 
/*******************************************************/
struct _MSList {
	struct _MSList *next;
	struct _MSList *prev;
	void *data;
};

typedef struct _MSList MSList;

#define ms_list_next(elem) ((elem)->next)


/*******************************************************/
/*                        Filter                       */ 
/*******************************************************/
struct _MSFilterDesc;


typedef int (*MSFilterMethodFunc)(struct _MSFilterDesc *f, void *arg);
typedef int (*MSFilterFunc)(struct _MSFilterDesc* f);
typedef void (*MSFilterDataFunc)(struct _MSFilterDesc *f, void *arg);
typedef void (*MSFilterNotifyFunc)(void *userdata, struct _MSFilterDesc *f, unsigned int id, void *arg);

struct _MSFilterMethod{
	unsigned int id;
	MSFilterMethodFunc method;
};

typedef struct _MSFilterMethod MSFilterMethod;

struct _MSFilterOut{
	int noutputs;
	void* outputs;
};

typedef struct _MSFilterOut MSFilterOut;

typedef struct msgb
{
	MSFilterId id;
    struct listnode _list;  
}mblk_t;

typedef enum __MSProcess
{
    PROCESS_WORK_NONE = 0,
	PROCESS_WORK_UNBLOCK,
	PROCESS_WORK_BLOCK,	
} MSProcess;

struct _MSFilterDesc{
	MSFilterId id;
	const char *name; 
	const char *text;
	int noutputs;
	MSFilterOut* outputs;
	MSFilterFunc init;
	MSFilterFunc uninit;
	MSFilterFunc preprocess;
	MSFilterDataFunc process;
	MSFilterFunc postprocess;	
	MSFilterMethod *methods;
	int mcount;
	MSProcess mProcessMode;
	void *private;

	MSFilterNotifyFunc notify;
	void* notify_ud;
    
	struct listnode queue;	
	MSThread thread;
	MSThread threadprocess;
	pthread_mutex_t lock;
	pthread_mutex_t lockprocess;
	pthread_mutex_t lockqueue;
};

typedef struct _MSFilterDesc MSFilterDesc;

/*************************************************
  Function:		ms_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_free(void * data);

/*************************************************
  Function:		ms_list_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_new(void *data);

/*************************************************
  Function:		ms_list_append
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_append(MSList *elem, void * data);

/*************************************************
  Function:		ms_list_prepend
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_prepend(MSList *elem, void *data);

/*************************************************
  Function:		ms_list_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_free(MSList *list);

/*************************************************
  Function:		ms_list_remove_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_remove_link(MSList *list, MSList *elem);

/*************************************************
  Function:		ms_list_find
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_find(MSList *list, void *data);

/*************************************************
  Function:		ms_list_remove
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_remove(MSList *first, void *data);

/*************************************************
  Function:		ms_filter_get_desc_form_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSFilterDesc* ms_filter_get_desc_form_id(MSFilterId id);

/*************************************************
  Function:		ms_filter_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSFilterDesc *ms_filter_new(MSFilterId id);

/*************************************************
  Function:		ms_filter_destory
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_destory(MSFilterDesc **desc);

/*************************************************
  Function:		ms_filter_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_link(MSFilterDesc *f1, MSFilterDesc *f2);

/*************************************************
  Function:		ms_filter_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_unlink(MSFilterDesc *f1, MSFilterDesc *f2);

/*************************************************
  Function:		ms_filter_blk_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_blk_init(mblk_t * arg);

/*************************************************
  Function:		ms_filter_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_process(MSFilterDesc *f1, mblk_t * arg);

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method(MSFilterDesc *f, unsigned int id, void *arg);

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method_noarg(MSFilterDesc *f, unsigned int id);

/*************************************************
  Function:		ms_filter_set_notify_callback
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_set_notify_callback(MSFilterDesc *f, MSFilterNotifyFunc fn, void *ud);

/*************************************************
  Function:		ms_filter_notify
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify(MSFilterDesc *f, unsigned int id, void *arg);

/*************************************************
  Function:		ms_filter_notify_no_arg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify_no_arg(MSFilterDesc *f, unsigned int id);

/*************************************************
  Function:		__ms_thread_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_init(MSThread* thread, int exittimer);

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_quit(MSThread* thread);

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void *arg);

/*************************************************
  Function:		ms_module_register
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_register(MSFilterDesc *desc);

/*************************************************
  Function:		ms_module_unregister_all
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_unregister_all(void);

/*************************************************
  Function:		ms_module_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_init(void);

/*************************************************
  Function:		ms_module_exit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_exit(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _FILTER_CORE_H_ Edit By Txl 2018-8-29 */

