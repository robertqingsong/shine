#include "shine.h"

#include "data.h"

#include "data_pool.h"

#include "oal_api.h"

#include "mem_api.h"

//shine type define.
typedef struct CShine_t
{
	CShineParam stShineParam;//shine param.

	int32_t iIsRunning;	
	CThread *pThread;//thread of shine.

}CShine;

static void *common_shine_task( void *pParam );

//open shine so that it can send or receive data.
void *open_shine( const CShineParam *pShineParam )
{
	void *pRetCode = NULL;
	CShine *pNewShine = NULL;
	
	if ( !pShineParam )
		return pRetCode;
	
	pNewShine = mem_malloc( sizeof(*pNewShine) );
	if ( pNewShine )
	{
		memset( pNewShine, 0x00, sizeof(*pNewShine) );
		
		pNewShine->stShineParam = *pShineParam;
		
		switch ( pShineParam->eShineType )
		{
		case SHINE_TYPE_SENDER:
		{
			pRetCode = pNewShine;
		}break ;
		case SHINE_TYPE_RECEIVER:
		{
			pNewShine->iIsRunning = 1;
			pNewShine->pThread = os_thread_create( common_shine_task, pNewShine, OS_THREAD_PRIORITY_NORMAL, 1024 * 10 );
			if ( pNewShine->pThread )
			{
				pRetCode = pNewShine;
			}
			else 
				pNewShine->iIsRunning = 0;	
		}break ;
		default:
		{
			
		}break ;			
		}
			
		if ( !pRetCode )
		{
			mem_free( pNewShine );
			pNewShine = NULL;
		}
	}
	
	return pRetCode;	
}

//close shine to release resource.
void close_shine( void *pHandle )
{
	CShine *pShine = (CShine *)pHandle;
	
	if ( pShine )
	{
		CShineParam *pShineParam = &( pShine->stShineParam );
		
		switch ( pShineParam->eShineType )
		{
		case SHINE_TYPE_SENDER:
		{
			
		}break ;
		case SHINE_TYPE_RECEIVER:
		{
			pShine->iIsRunning = 0;
		
			os_thread_wait( pShine->pThread );
			pShine->pThread = NULL;	
		}break ;
		default:
		{
			
		}break ;	
		}
		
		mem_free( pShine );
		pShine = NULL;
	}
}

//open tunnel.
int32_t open_tunnel( void *pHandle )
{
	int32_t iRetCode = -1;
	
	return iRetCode;	
}

//close tunnel.
int32_t close_tunnel( void *pHandle, int32_t iTunnelId )
{
	int32_t iRetCode = -1;
	
	return iRetCode;		
}

//read shine data.
int32_t read_shine_data( const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize )
{
	int32_t iRetCode = -1;
	
	return iRetCode;
}

//write shine data.
int32_t write_shine_data( const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize )
{
	int32_t iRetCode = -1;
	
	return iRetCode;
}

static void *common_shine_task( void *pParam )
{
	void *pRetCode = NULL;
	
	CShine *pShine = (CShine *)pParam;
	
	if ( !pShine )
		return pRetCode;
		
	while ( pShine->iIsRunning )
	{
		
	}
	
	return pRetCode;
}

