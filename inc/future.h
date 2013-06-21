/********************************************************
 * file name: future.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: future model for framework kpf
 * *****************************************************/

#ifndef __FUTURE_H__
#define __FUTURE_H__

#include "task.h"

#if defined(__cplusplus)
extern "C"
{
#endif

struct CFuture_t;

//future callback.
typedef int32_t (*future_callback_t)( struct CFuture_t *pFuture );

//future type define.
typedef struct CFuture_t
{
	CMessage stMesg;
	
	future_callback_t future_callback;
}CFuture;

//init future.
void init_future( void *pFuture, const future_callback_t callback );

#if defined(__cplusplus)
}
#endif

#endif/* __FUTURE_H__ */