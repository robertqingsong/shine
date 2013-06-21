/********************************************************
 * file name: task.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: task for framework kpf
 * *****************************************************/

#ifndef __TASK_H__
#define __TASK_H__

#include "config.h"
#include "typedefs.h"

#include "queue.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct CMessage_t
{
	
	CQueueNode QNode;
}CMessage;

typedef void *(*task_callback_t)( void *pParam );

//create task.
void *create_task( const task_callback_t task, const int32_t iMaxThreadCount, int32_t iMaxWaterLevel );

//destroy task.
void destroy_task( void *pTaskHandle );

//post task message.
int32_t post_task_msg( void *pTaskHandle, const CMessage *pNewMsg );

#if defined(__cplusplus)
}
#endif

#endif/* __TASK_H__ */
