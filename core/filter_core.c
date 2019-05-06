/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		filter_core.c
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-8-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-8-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#include "filter_core.h"

static MSList *desc_list=NULL;


/*************************************************
  Function:		ms_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_free(void * data)
{
	if (data)
	{
		free(data);
		data = NULL;
	}
}

/*************************************************
  Function:		ms_list_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_new(void *data)
{
	MSList *new_elem = (MSList *)ms_new(MSList,1);
	new_elem->prev = new_elem->next = NULL;
	new_elem->data = data;
	return new_elem;
}

/*************************************************
  Function:		ms_list_append
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_append(MSList *elem, void * data)
{
	MSList *new_elem = ms_list_new(data);
	MSList *it = elem;
	if (elem == NULL) 
	{
		return new_elem;
	}
	while (it->next!=NULL) 
	{
		it = ms_list_next(it);
	}
	it->next = new_elem;
	new_elem->prev = it;
	return elem;
}

/*************************************************
  Function:		ms_list_prepend
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_prepend(MSList *elem, void *data)
{
	MSList *new_elem = ms_list_new(data);
	if (elem != NULL) 
	{
		new_elem->next = elem;
		elem->prev = new_elem;
	}
	return new_elem;
}

/*************************************************
  Function:		ms_list_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_free(MSList *list){
	MSList *elem = list;
	MSList *tmp;
	if (list == NULL) 
	{
		return NULL;
	}
	while(elem->next != NULL) 
	{
		tmp = elem;
		elem = elem->next;
		ms_free(tmp);
	}
	ms_free(elem);
	return NULL;
}

/*************************************************
  Function:		ms_list_remove_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_remove_link(MSList *list, MSList *elem)
{
	MSList *ret;
	if (elem==list)
	{
		ret = elem->next;
		elem->prev = NULL;
		elem->next = NULL;
		if (ret != NULL) 
		{
			ret->prev = NULL;
		}
		ms_free(elem);
		return ret;
	}
	elem->prev->next = elem->next;
	if (elem->next != NULL)
	{
		elem->next->prev = elem->prev;
	}
	elem->next = NULL;
	elem->prev = NULL;
	ms_free(elem);
	return list;
}

/*************************************************
  Function:		ms_list_find
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_find(MSList *list, void *data)
{
	for(;list != NULL; list = list->next)
	{
		if (list->data == data) return list;
	}
	return NULL;
}

/*************************************************
  Function:		ms_list_remove
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_remove(MSList *first, void *data)
{
	MSList *it;
	it = ms_list_find(first,data);
	if (it) 
	{
		return ms_list_remove_link(first,it);
	}
	else 
	{
		LOGD(_LOG_CORE_,"ms_list_remove: no element with %p data was in the list\n", data);
		return first;
	}
}

/*************************************************
  Function:			ms_queue_init
  Description:		初始化链表
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_init(MSFilterDesc *f)
{
	ms_return_val_if_fail(f, -1);
	
	int i;
	ms_queue_lock(f);
    LIST_INIT(&f->queue);
	ms_queue_unlock(f);
	return 0;
}

/*************************************************
  Function:			ms_queue_uninit
  Description:		初始化链表
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_uninit(MSFilterDesc *f)
{
    struct listnode *codenode,*pnode; 
    mblk_t *blk;
	ms_return_val_if_fail(f, -1);

	ms_queue_lock(f);

    list_for_each_safe(codenode, pnode, &f->queue)
    {            
        blk = node_to_item(codenode, mblk_t,_list);    
        LIST_DEL(&blk->_list);
        if(blk)
        {
            free(blk);
            blk = NULL;
        }
    }    
	ms_queue_unlock(f);
	return 0;
}

/*************************************************
  Function:			ms_queue_put
  Description:		把数据放入链表
  Input: 			
  	1.address	
  	2.len		
  	3.marker		
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_put(MSFilterDesc *f, mblk_t * arg)
{	
    mblk_t *blk = NULL;
	ms_return_val_if_fail(f, -1);
	ms_return_val_if_fail(arg, -1);
	
	ms_queue_lock(f);

    blk = ms_new(mblk_t,1);
    if(blk)
    {
        memcpy(blk,arg,sizeof(mblk_t));
        LIST_INIT(&blk->_list);
        list_add_tail(&f->queue,&blk->_list);  
    }
   
	ms_queue_unlock(f);
	return 0;
}

/*************************************************
  Function:			ms_queue_get
  Description:		从链表取出数据
  Input: 			
  	1.**address		视频数据指针的指针
  	2.*len			数据长度
  	3.*tps			时钟戳
  Output:			无
  Return:			true / false
  Others:
*************************************************/
static int ms_queue_get(MSFilterDesc *f, mblk_t **arg)
{
    struct listnode *codenode,*pnode; 
    mblk_t *blk = NULL;
	ms_return_val_if_fail(f, -1);
	ms_queue_lock(f);
    list_for_each_safe(codenode, pnode, &f->queue)
    {            
        blk = node_to_item(codenode, mblk_t,_list);    
        LIST_DEL(&blk->_list);
        if(blk)
        {
            memcpy(*arg,blk,sizeof(mblk_t));
            free(blk);
            blk = NULL;
        }
        
        break;
    }    
	ms_queue_unlock(f);
	return 0;
}

/*************************************************
  Function:			set_video_pop_state
  Description:		从链表取出数据
  Input: 			
  	1.**address		视频数据指针的指针
  	2.*len			数据长度
  	3.*tps			时钟戳
  Output:			无
  Return:			true / false
  Others:
*************************************************/
static int ms_queue_pop(MSFilterDesc *f)
{
	ms_return_val_if_fail(f, -1);
	ms_queue_lock(f);

	ms_queue_unlock(f);
	return 0;
}

/*************************************************
  Function:		ms_process_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void *ms_process_thread(void *ptr)
{
    char Cmd[100];
	MSFilterDesc *f = (MSFilterDesc *)ptr;
	mblk_t arg;
	mblk_t *arg1;

	while(f->threadprocess.thread_run)
	{
		arg1 = &arg;
		if(0 == ms_queue_get(f, (&arg1)))	
		{
			if(f->process)
			{
				f->process(f,&arg);
			}
            ms_queue_pop(f);
		}
		else
		{
			usleep(20);
		}
	}

	f->threadprocess.thread	= -1;
	pthread_detach(pthread_self());
    pthread_exit(NULL);
	return NULL;
}

/*************************************************
  Function:		ms_filter_get_desc_form_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSFilterDesc* ms_filter_get_desc_form_id(MSFilterId id)
{
	MSList *elem;
	for (elem = desc_list; elem != NULL; elem = ms_list_next(elem))
	{
		MSFilterDesc *desc = (MSFilterDesc*)elem->data;
		
		if (desc->id == id)
		{
			return desc;
		}
	}

	return NULL;
}

/*************************************************
  Function:		ms_filter_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSFilterDesc *ms_filter_new(MSFilterId id)
{
	MSFilterDesc* desc = ms_filter_get_desc_form_id(id);
	return desc;
}

/*************************************************
  Function:		ms_filter_destory
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_destory(MSFilterDesc **desc)
{
	ms_return_val_if_fail(desc, -1);
	ms_return_val_if_fail((*desc), -1);
	
	(*desc) = NULL;
	return 0;
}

/*************************************************
  Function:		ms_filter_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_link(MSFilterDesc *f1, MSFilterDesc *f2)
{
	int i, ret = -1;
	int connect = 0;
	MSFilterDesc *f3 = NULL;
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	ms_process_lock(f1);  
	for (i = 0; i < f1->noutputs; i++)
	{    
		if (f1->outputs[i].outputs != NULL)
		{
			f3 = (MSFilterDesc *)f1->outputs[i].outputs;
			if(f3->id == f2->id)
			{
				f1->outputs[i].noutputs++;
				connect = 1;
				ret = i;
				break;
			}
		}
	}
	if(connect == 0)
	{
		for (i = 0; i < f1->noutputs; i++)
		{
			if (f1->outputs[i].outputs == NULL)
			{
				f1->outputs[i].outputs = f2;
				f1->outputs[i].noutputs = 1;
				ret = i;
				break;
			}
		}
	}
	
	ms_process_unlock(f1);
	if(-1 != ret)
	{
		if(connect == 0)
		{
			LOGD(_LOG_CORE_,"ms_filter_link f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
		else
		{
			LOGD(_LOG_CORE_,"ms_filter_link have connect f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
	}
	else
	{
		LOGD(_LOG_CORE_,"ms_filter_link ERR f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
	}
	return ret;
}

/*************************************************
  Function:		ms_filter_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_unlink(MSFilterDesc *f1, MSFilterDesc *f2)
{
	int i, ret = -1;
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	ms_process_lock(f1);
	for (i = 0; i < f1->noutputs; i++)
	{
		if (f1->outputs[i].outputs == f2)
		{
			if(f1->outputs[i].noutputs > 0)
			{
				f1->outputs[i].noutputs--;
			}
			if(f1->outputs[i].noutputs == 0)
			{
				f1->outputs[i].outputs = NULL;
				ret = 0;
			}
			
			break;
		}
	}
	ms_process_unlock(f1);
	if(-1 != ret)
	{
		LOGD(_LOG_CORE_,"ms_filter_unlink f1 [%s] -> f2 [%s]",f1->name,f2->name);
	}
	return ret;
}

/*************************************************
  Function:		ms_filter_blk_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_blk_init(mblk_t * arg)
{
	ms_return_val_if_fail(arg, -1);

	arg->id = MS_FILTER_NOT_SET_ID;
	return 0;
}

/*************************************************
  Function:		ms_filter_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_process(MSFilterDesc *f1, mblk_t * arg)
{
	int i;
	MSFilterDesc* f; 
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(arg, -1);
	
	ms_process_lock(f1);  
	if(f1->noutputs > 0)
	{
		for (i = 0; i < f1->noutputs; i++)
		{
			f = f1->outputs[i].outputs;	
			if(NULL != f && (arg->id == MS_FILTER_NOT_SET_ID || arg->id == f->id))
			{
				if(PROCESS_WORK_BLOCK == f->mProcessMode)
				{
					if(f->process)
					{
						f->process(f,arg);
					}
				}
				else if(PROCESS_WORK_UNBLOCK == f->mProcessMode)
				{
					ms_queue_put(f, arg);
				}
			}  
		}
	}	
	ms_process_unlock(f1);
	return 0;
}

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method(MSFilterDesc *f, unsigned int id, void *arg){

	ms_return_val_if_fail(f, -1);

	MSFilterMethod *methods=f->methods;
	int i;
	unsigned int magic=MS_FILTER_METHOD_GET_FID(id);

	if (magic != f->id) 
	{
		LOGD(_LOG_CORE_,"Method type checking failed when calling %u on filter %s Index %d",id,f->name,MS_FILTER_METHOD_GET_INDEX(id));
		return -1;
	}
	for(i=0;methods!=NULL && methods[i].method!=NULL; i++){
		unsigned int mm=MS_FILTER_METHOD_GET_FID(methods[i].id);
		if (mm!=f->id) {
			LOGD(_LOG_CORE_,"Bad method definition on filter %s. fid=%u , mm=%u",f->name,f->id,mm);
			return -1;
		}
		if (methods[i].id==id)
		{
			return methods[i].method(f,arg);
		}
	}

	return -1;
}

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method_noarg(MSFilterDesc *f, unsigned int id)
{
	return ms_filter_call_method(f,id,NULL);
}

/*************************************************
  Function:		ms_filter_set_notify_callback
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_set_notify_callback(MSFilterDesc *f, MSFilterNotifyFunc fn, void *ud)
{
	ms_return_val_if_fail(f, -1);
	
	f->notify = fn;
	f->notify_ud = ud;
	return 0;
}

/*************************************************
  Function:		ms_filter_notify
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify(MSFilterDesc *f, unsigned int id, void *arg)
{
	ms_return_val_if_fail(f, -1);
	if (f->notify != NULL)
	{
		f->notify(f->notify_ud, f, id, arg);
	}

	return 0;
}

/*************************************************
  Function:		ms_filter_notify_no_arg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify_no_arg(MSFilterDesc *f, unsigned int id)
{
	return ms_filter_notify(f,id,NULL);
}

/*************************************************
  Function:		ms_filter_preprocess
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_filter_preprocess(struct _MSFilterDesc *f)
{
	if(f->mProcessMode == PROCESS_WORK_UNBLOCK)
	{
		ms_queue_uninit(f);
		ms_thread_init(&f->threadprocess,1000);
		ms_thread_create(&f->threadprocess, ms_process_thread, f);
		LOGD(_LOG_CORE_,"ms_media_preprocess");
	}
	return 0;
}

/*************************************************
  Function:		ms_filter_postprocess
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_filter_postprocess(struct _MSFilterDesc *f)
{
	if(f->mProcessMode == PROCESS_WORK_UNBLOCK)
	{
		ms_thread_quit(&f->threadprocess);
		ms_queue_uninit(f);
		LOGD(_LOG_CORE_,"ms_media_postprocess");
	}
	return 0;
}

/*************************************************
  Function:		__ms_thread_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_init(MSThread* thread, int exittimer)
{
	ms_return_val_if_fail(thread, -1);
	thread->thread_exittimer = exittimer;
	return 0;
}

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_quit(MSThread* thread)
{
	unsigned int Timer = 0;
  
	ms_return_val_if_fail(thread, -1);

	if (thread->thread == -1)
	{	
		return 0;
	}
	thread->thread_run = 0;
	while(thread->thread != -1)
	{
		if (Timer++ > thread->thread_exittimer)
		{
			break;
		}
		usleep(20);
	}
	if (Timer >= thread->thread_exittimer)
	{
		LOGD(_LOG_CORE_,"ms_thread_quit time out");
	}
	return 0;
}

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void *arg)
{
	ms_return_val_if_fail(thread, -1);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	thread->thread_run = 1;

	if (0 != pthread_create(&thread->thread, &attr, (void*)routine, (void*)arg))
	{
		thread->thread_run = 0;
		pthread_attr_destroy(&attr);
		return -1;
	}
	
	pthread_attr_destroy(&attr);
	return 0;
}

/*************************************************
  Function:		ms_module_register
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_register(MSFilterDesc *desc)
{
	int i;
	if (desc->id == MS_FILTER_NOT_SET_ID)
	{
		LOGD(_LOG_CORE_,"MSFilterId for %s not set !",desc->name);
	}
	desc_list = ms_list_prepend(desc_list,desc);
	if (desc->init)
	{
		desc->init(desc);
	}

	if (desc->noutputs > 0)	
	{
		desc->outputs = (void*)ms_new(MSFilterOut,desc->noutputs);
		for (i = 0; i < desc->noutputs; i++)
		{
			desc->outputs[i].noutputs = 0;
			desc->outputs[i].outputs = NULL;		
		}
	}
	ms_mutex_init(&desc->lock, NULL);
	ms_mutex_init(&desc->lockprocess, NULL);
	ms_mutex_init(&desc->lockqueue, NULL);

	if(desc->mProcessMode == PROCESS_WORK_UNBLOCK)
	{
		desc->preprocess = ms_filter_preprocess;
		desc->postprocess = ms_filter_postprocess;
		ms_queue_init(desc);

        ms_filter_preprocess(desc);
	}
}

/*************************************************
  Function:		ms_module_unregister_all
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_unregister_all(void)
{
	if (desc_list != NULL) 
	{
		ms_list_free(desc_list);
	}
}

/*************************************************
  Function:		ms_module_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_init(void)
{
//	int i;
//	for (i = 0; ms_module_descs[i] != NULL; i++)
//	{
//		ms_module_register(ms_module_descs[i]);
//	}
}

/*************************************************
  Function:		ms_module_exit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_module_exit(void)
{
	ms_module_unregister_all();
}


