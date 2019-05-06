/*********************************************************
 Copyright (C),2015-2021,Electronic Technology Co.,Ltd.
 File name: 		filter_id.h
 Author: 			Txl
 Version: 			1.0
 Date: 				2018-8-29
 Description: 		
 History: 			
 					
   1.Date:	 		2018-8-29
 	 Author:	 	Txl
 	 Modification:  Created file
 	 
*********************************************************/
#ifndef _FILTER_ID_H_
#define _FILTER_ID_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum __MSFilterId{
	MS_FILTER_NOT_SET_ID,
} MSFilterId;

#define MS_FILTER_METHOD_ID(_id_,_cnt_,_argsize_) \
	(  (((unsigned long)(_id_)) & 0xFFFF)<<16 | (_cnt_<<8) | (_argsize_ & 0xFF ))

#define MS_FILTER_METHOD(_id_,_count_,_argtype_) \
	MS_FILTER_METHOD_ID(_id_,_count_,sizeof(_argtype_))

#define MS_FILTER_METHOD_NO_ARG(_id_,_count_) \
	MS_FILTER_METHOD_ID(_id_,_count_,0)

#define MS_FILTER_EVENT(_id_,_count_,_argtype_) \
	MS_FILTER_METHOD_ID(_id_,_count_,sizeof(_argtype_))

#define MS_FILTER_EVENT_NO_ARG(_id_,_count_)\
	MS_FILTER_METHOD_ID(_id_,_count_,0)

#define MS_FILTER_METHOD_GET_FID(id)	(((id)>>16) & 0xFFFF)
#define MS_FILTER_METHOD_GET_INDEX(id) ( ((id)>>8) & 0XFF) 

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* ifndef _FILTER_ID_H_ Edit By Txl 2018-8-29 */

