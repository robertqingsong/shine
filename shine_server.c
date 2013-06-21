#include "log.h"

#include "prot.h"

#include "session.h"

#include "future.h"

#include "mem_api.h"

typedef struct CShineFuture_t
{
	CFuture stFuture;//base future.
	
	int8u_t *pMsgBuf;//point to msg buffer.
	int32_t iMsgBufSize;//max msg buffer size.
	
	CSocket *pSocket;//udp server socket.
	
	CNetAddr stPeerNetAddr;
	
}CShineFuture;

static int32_t fg_iIsTaskRunning = 0;
static void *pTaskHandle = NULL;

static int32_t shine_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData );
			
static int32_t shine_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData );
				 
static void *shine_task_callback( void *pParam );

int main( int argc, char **argv )
{
	CSession *pServiceSession = NULL;
	CSessionParam stSessionParam;
	const int8u_t *pLocalIP = "192.168.1.100";
	

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_session(  ) < 0 )
		return -1;
		
	//if ( net_get_local_ip( pLocalIP, sizeof(pLocalIP) ) < 0 )
		//return -1;
	
	fg_iIsTaskRunning = 1;
	pTaskHandle = create_task( shine_task_callback, 5, 1024 * 1024 );
	if ( !pTaskHandle )
		return -1;
		
	log_print( pLocalIP );
	
	memset( &stSessionParam, 0x00, sizeof(stSessionParam) );
	memcpy( stSessionParam.pLocalIP, pLocalIP, strlen(pLocalIP) + 1 );
	stSessionParam.iLocalPort = 9000;
	
	pServiceSession = create_session( SESSION_TYPE_DGRAM_SERVER, &stSessionParam );
	if ( pServiceSession )
	{
		if ( set_session_business( pServiceSession, shine_session_business, NULL ) >= 0 )
		{
			if ( set_session_event( pServiceSession, shine_session_event ) >= 0 )
			{
				log_print( "shine server is running..............................." );	
			}
		}
		
	}
	
	while ( 1 )
		os_sleep( 1000 );
		
	release_session(  );	
	
	return 0;	
}

//future callback.
int32_t shine_future_callback( struct CFuture_t *pFuture )
{
	int32_t iRetCode = -1;
	CShineFuture *pShineFuture = (CShineFuture *)pFuture;
	CCommonHeader *pMsgHeader = NULL;
	
	log_print( "shine future callback...................." );	
	
	if ( !pShineFuture )
		return iRetCode;
	
	pMsgHeader = (CCommonHeader *)pShineFuture->pMsgBuf;
	
	if ( pMsgHeader )
	{
		switch ( pMsgHeader->iMsgId )
		{
		case MSG_ID_REQUEST_PEER:
		{
			CTellPeer *pTellPeerMsg = (CTellPeer *)pMsgHeader;
			AckTellPeer stAckTellPeerMsg;
			const int8_t *pPeerIP = "192.168.1.100";
			const int16u_t iPeerPort = 8000;
			
			memset( &stAckTellPeerMsg, 0x00, sizeof(stAckTellPeerMsg) );
		
			init_message( &stAckTellPeerMsg, MSG_ID_ACK_PEER, sizeof(stAckTellPeerMsg) );	
			
			memcpy( stAckTellPeerMsg.pPeerIP, pPeerIP, strlen(pPeerIP) + 1 );
			stAckTellPeerMsg.iPeerPort = iPeerPort;
			
			iRetCode = net_sendto( pShineFuture->pSocket, &stAckTellPeerMsg, sizeof(stAckTellPeerMsg), &(pShineFuture->stPeerNetAddr) );
			log_print( "send %d bytes", iRetCode );
		}break ;
		default:
		{
			
		}break ;	
		}	
	}
	
	return iRetCode;
}

static void *shine_task_callback( void *pParam )
{
	CQueue *pTaskQ = (CQueue *)pParam;
	CQueueNode *pTaskQNode = NULL;
	CShineFuture *pShineFuture = NULL;
	
	if ( !pTaskQ )
		return NULL;
	
	while ( fg_iIsTaskRunning )
	{
		pTaskQNode = de_queue( pTaskQ );
		if ( pTaskQNode )
		{
			log_print( "task new msg received................." );
			
			pShineFuture = CONTAINER_OF_QUEUE( pTaskQNode, CMessage );
			
			if ( pShineFuture->stFuture.future_callback )
			{
				pShineFuture->stFuture.future_callback( (CFuture *)pShineFuture );
			}

			if ( pShineFuture->pMsgBuf )
			{
				mem_free( pShineFuture->pMsgBuf );
				pShineFuture->pMsgBuf = NULL;	
			}			
			
			mem_free( pShineFuture );
			pShineFuture = NULL;
		}
		else 
			os_sleep( 10 );
	}
	
	return NULL;
}

static int32_t shine_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData )
{
	int32_t iRetCode = -1;
	
	log_print( "shine_sesson_business:---------------------->" );
	
	if ( pInData && iInDataLen > 0 )
	{
		CShineFuture *pNewFuture = NULL;
		
		log_print( "pInData-->%u, iInDataLen-->%d", pInData, iInDataLen );
		
		pNewFuture = mem_malloc( sizeof( *pNewFuture ) );
		if ( pNewFuture )
		{
			//
			log_print( "sero future, pNewFuture->%u..............", pNewFuture );
			memset( pNewFuture, 0x00, sizeof( *pNewFuture ) );
			
			//
			log_print( "start to call init_future.................." );
			init_future( pNewFuture, shine_future_callback );
			
			pNewFuture->pMsgBuf = mem_malloc( iInDataLen );
			if ( pNewFuture->pMsgBuf )
			{
				log_print( "start to copy data............" );
				memcpy( pNewFuture->pMsgBuf, pInData, iInDataLen );
				pNewFuture->iMsgBufSize = iInDataLen;
				
				pNewFuture->pSocket = pSocket;
				if ( pNetAddr )
					pNewFuture->stPeerNetAddr = *pNetAddr;
				
				//post future msg.
				log_print( "post task msg................" );
				iRetCode = post_task_msg( pTaskHandle, pNewFuture );
			}
			else 
				log_print( "alloc msg buffer failed??????????????????????????" );
			
			if ( iRetCode < 0 )
			{
				log_print( "post task msg failed??????????????????????????" );
				
				if ( pNewFuture->pMsgBuf )
				{
					mem_free( pNewFuture->pMsgBuf );
					pNewFuture->pMsgBuf = NULL;					
				}
				
				mem_free( pNewFuture );
				pNewFuture = NULL;
			}
			
		}
		else 
			log_print( "new future malloc failed?????????????????????" );
	}
	
	log_print( "shine_business_callback<---------------------" );
	
	return iRetCode;
}

static int32_t shine_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData )
{
	int32_t iRetCode = -1;
	
	return iRetCode;
}

