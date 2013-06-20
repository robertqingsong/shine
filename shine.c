#include "shine.h"

#include "data.h"

#include "data_pool.h"

#include "oal_api.h"

#include "mem_api.h"

#include "session.h"

#include "lock.h"

#include "block.h"

#include "prot.h"

#include "log.h"

/*************************************************************************************************
* macro define.
**************************************************************************************************/
#define MAX_SHINE_TUNNEL_SIZE  (32)

/*************************************************************************************************
* type define.
**************************************************************************************************/
typedef enum C_SHINE_ROLE_T
{
	SHINE_ROLE_PRIVIDE_SERVICE, 
	SHINE_ROLE_SHARE_SERVICE
}C_SHINE_ROLE;

#define SHINE_STATE_OK  (0)
#define SHINE_STATE_FAILED  (-1)

//shine tunnel define.
typedef struct CTunnel_t
{
	CSession *pSession;//tunnel session.
}CTunnel;

//shine type define.
typedef struct CShine_t
{
	CShineParam stShineParam;//shine param.
	
	CTunnel *ppTunnelArray[MAX_SHINE_TUNNEL_SIZE];//shine tunnel point array.
	int32_t iCurrentTunnelIndex;//current tunnel index.
	
	CSession *pServerSession;//server session.
	
	void *pBlock;
	
	int32_t iState;
	
	int8_t pPeerIP[MAX_IP_SIZE];
	int16u_t iPeerPort;

	CMutex Locker;//shine locker.
}CShine;

typedef struct CShineManager_t
{
	int32_t iInitFlag;
	
	int16u_t iLocalPortIndex;
	
	
}CShineManager;

static CShineManager fg_ShineManager = {
	0, 
	65530	
};

/*************************************************************************************************
* file function declaration.,
**************************************************************************************************/
//create tunnel .
static CTunnel *create_tunnel( CShine *pThis, const int8_t *pPeerShineId, C_SHINE_ROLE eRole );
//close tunnel.
static void destroy_tunnel( CTunnel *pTunnel );

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
				 
static int32_t tunnel_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData );

static int32_t tunnel_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData );

/*************************************************************************************************
* function name: init_shine
* param: void
* return value type: int32_t
* description: init shine.
**************************************************************************************************/
int32_t init_shine( void )
{
	int32_t iRetCode = -1;
	
	if ( !fg_ShineManager.iInitFlag )
	{
		fg_ShineManager.iLocalPortIndex = 51930;
		if ( init_session(  ) >= 0 )
		{
			fg_ShineManager.iInitFlag = 1;
			iRetCode = init_block(  );
		}
	}
	else 
		iRetCode = 0;
	
	return iRetCode;	
}

/*************************************************************************************************
* function name: release_shine
* param: void
* return value type: int32_t
* description: release shine.
**************************************************************************************************/
void release_shine( void )
{
	if ( fg_ShineManager.iInitFlag )
	{
		release_block(  );
	
		release_session(  );
		
		fg_ShineManager.iInitFlag = 0;
	}
}

static int32_t is_shine_manager_ready( void )
{
	int32_t iRetCode = -1;

	if ( fg_ShineManager.iInitFlag )
		iRetCode = 0;	
	
	return iRetCode;	
}

/*************************************************************************************************
* function name: open_shine
* param: const CShineParam *pShineParam// open param.
* return value type: void * //shine handle.
* description: open shine so that it can send or receive data.
**************************************************************************************************/
void *open_shine( const CShineParam *pShineParam )
{
	void *pRetCode = NULL;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return pRetCode;
	
	if ( pShineParam )
	{
		CShine *pNewShine = NULL;
		
		pNewShine = mem_malloc( sizeof(*pNewShine) );
		if ( pNewShine )
		{
			memset( pNewShine, 0x00, sizeof(*pNewShine) );
			
			if ( init_mutex( &( pNewShine->Locker ) ) >= 0 )
			{
				CSessionParam stSessionParam;
				
				pNewShine->stShineParam = *pShineParam;
				
				memset( &stSessionParam, 0x00, sizeof(stSessionParam) );
				
				memcpy( stSessionParam.pIP, pShineParam->pServerIP, strlen(pShineParam->pServerIP) + 1 );
				stSessionParam.iPort = pShineParam->iServerPort;
				
				memcpy( stSessionParam.pLocalIP, pShineParam->pLocalIP, strlen(pShineParam->pLocalIP) + 1 );
				stSessionParam.iLocalPort = pShineParam->iLocalPort;
				
				pNewShine->pServerSession = create_session( SESSION_TYPE_DGRAM_CLIENT, &stSessionParam );
				
				if ( pNewShine->pServerSession )
				{
					if ( set_session_business( pNewShine->pServerSession, shine_session_business, pNewShine ) >= 0 )
					{
						if ( set_session_event( pNewShine->pServerSession, shine_session_event ) >= 0 )
						{
							pRetCode = pNewShine;
						}
						else 
							log_print( "%s %s %d: set session event failed.", __FILE__, __FUNCTION__, __LINE__  );
					}
					else 
						log_print( "%s %s %d: set session business failed.", __FILE__, __FUNCTION__, __LINE__  );
					
				}
				else 
					log_print( "%s %s %d: create session of shine server failed.", __FILE__, __FUNCTION__, __LINE__  );
			}
			else 
				log_print( "%s %s %d: open shine init mutex failed.", __FILE__, __FUNCTION__, __LINE__  );
			
			if ( !pRetCode )
			{
				mem_free( pNewShine );
				pNewShine = NULL;
			}
		}
		else 
			log_print( "%s %s %d: open shine malloc memory failed.", __FILE__, __FUNCTION__, __LINE__  );
	}
	else 
		log_print( "%s %s %d: open shine param is NULL.", __FILE__, __FUNCTION__, __LINE__ );
	
	return pRetCode;	
}

/*************************************************************************************************
* function name: close_shine
* param: void *pHandle
* return value type: void
* description: close shine to release resource.
**************************************************************************************************/
void close_shine( void *pHandle )
{
	if ( is_shine_manager_ready(  ) < 0 )
		return ;
		
	if ( pHandle )
	{
		CShine *pShine = (CShine *)pHandle;
		int32_t i = 0;
		CTunnel *pTunnel = NULL;
		
		lock( &( pShine->Locker ) );
		
		for ( i = 0; i < pShine->iCurrentTunnelIndex; i++ )
		{
			pTunnel = pShine->ppTunnelArray[i];
			
			destroy_tunnel( pTunnel );
		}
		
		destroy_session( pShine->pServerSession );
		pShine->pServerSession = NULL;
		
		unlock( &( pShine->Locker ) );
	}
}

/*************************************************************************************************
* function name: open_shine_tunnel
* param: void *pHandle
* return value type: int32_t //tunnel id.
* description: open shine tunnel for data transpartation.
**************************************************************************************************/
int32_t open_shine_tunnel( void *pHandle, int8_t *pPeerShineId )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	if ( pHandle && pPeerShineId )
	{
		CShine *pShine = (CShine *)pHandle;
		
		lock( &( pShine->Locker ) );
		
		if ( pShine->iCurrentTunnelIndex < MAX_SHINE_TUNNEL_SIZE )
		{
			pShine->ppTunnelArray[pShine->iCurrentTunnelIndex] = create_tunnel( pShine, pPeerShineId, SHINE_ROLE_SHARE_SERVICE );
			
			if ( pShine->ppTunnelArray[pShine->iCurrentTunnelIndex] )
			{
				(pShine->iCurrentTunnelIndex)++;
				
				log_print( "create tunnel ok!!!!!!!!!!!!!!!!!!" );
				
				iRetCode = 0;
			}
			else 
				log_print( "create tunnel failed?????????????????" );
		}
		
		unlock( &( pShine->Locker ) );
	}
	
	return iRetCode;		
}

/*************************************************************************************************
* function name: close_shine_tunnel
* param: void *pHandle, int32_t iTunnelId
* return value type: int32_t //tunnel id.
* description: close shine tunnel.close data transfer.
**************************************************************************************************/
int32_t close_shine_tunnel( void *pHandle, int32_t iTunnelId )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	return iRetCode;		
}

/*************************************************************************************************
* function name: start_shine_tunnel
* param: void *pHandle
* return value type: int32_t //tunnel id.
* description: start tunnel id service for data processing.
**************************************************************************************************/
int32_t start_shine_tunnel( void *pHandle )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	return iRetCode;		
}

/*************************************************************************************************
* function name: stop_shine_tunnel
* param: void *pHandle, const int32_t iTunnelNo
* return value type: 0 ok, -1 failed.
* description: stop shine tunnel.
**************************************************************************************************/
int32_t stop_shine_tunnel( void *pHandle, const int32_t iTunnelId )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	return iRetCode;		
}

/*************************************************************************************************
* function name: read_shine_data
* param: const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize
* return value type: int32_t //data that have been read.
* description: read shine data.
**************************************************************************************************/
int32_t read_shine_data( const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	return iRetCode;		
}

//
/*************************************************************************************************
* function name: write_shine_data
* param: const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize 
* return value type: int32_t //data that have been written.
* description: write shine data.
**************************************************************************************************/
int32_t write_shine_data( const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize )
{
	int32_t iRetCode = -1;
	
	if ( is_shine_manager_ready(  ) < 0 )
		return iRetCode;
	
	return iRetCode;	
}

/*************************************************************************************************
* function name: create_tunnel
* param: 
* return value type: CTunnel *
* description: create tunnel .
**************************************************************************************************/
static CTunnel *create_tunnel( CShine *pThis, const int8_t *pPeerShineId, C_SHINE_ROLE eRole )
{
	CTunnel *pNewTunnel = NULL;
	CTellPeer stTellPeerReqMsg;
	int32_t iLen = -1;
		
	if ( !pThis || !pPeerShineId )
		return pNewTunnel;
	
	iLen = strlen(pPeerShineId);
	if ( iLen < MAX_SHINE_ID_SIZE )
	{	
		memset( &stTellPeerReqMsg, 0x00, sizeof(stTellPeerReqMsg) );
	
		init_message( &stTellPeerReqMsg, MSG_ID_REQUEST_PEER, sizeof( stTellPeerReqMsg ) );
		memcpy( stTellPeerReqMsg.pShineId, pPeerShineId, iLen + 1 );
		
		pThis->pBlock = block_create( 5 );
		if ( pThis->pBlock )
		{
			pThis->iState = SHINE_STATE_FAILED;
			if ( sizeof( stTellPeerReqMsg ) == send_session_data( pThis->pServerSession, &stTellPeerReqMsg, sizeof(stTellPeerReqMsg), NULL ) )
			{
				block_wait( pThis->pBlock );
				
				if ( SHINE_STATE_OK == pThis->iState )
				{
					pNewTunnel = mem_malloc( sizeof(*pNewTunnel) );
					if ( pNewTunnel )
					{
						CSessionParam stTunnelSessionParam;
						int32_t iOpRet = -1;
						
						memset( &stTunnelSessionParam, 0x00, sizeof(stTunnelSessionParam) );
						
						memset( pNewTunnel, 0x00, sizeof(*pNewTunnel) );
						
						switch ( eRole )
						{
						case SHINE_ROLE_SHARE_SERVICE:
						{
							int8_t pLocalIP[MAX_IP_SIZE] = { 0x00, };
							
							if ( net_get_local_ip( pLocalIP, sizeof(pLocalIP) ) >= 0 )
							{
								memcpy( stTunnelSessionParam.pIP, pThis->pPeerIP, strlen(pThis->pPeerIP) + 1 );
								stTunnelSessionParam.iPort = pThis->iPeerPort;
				
								memcpy( stTunnelSessionParam.pLocalIP, pLocalIP, strlen(pLocalIP) + 1 );
								stTunnelSessionParam.iLocalPort = fg_ShineManager.iLocalPortIndex;
								pNewTunnel->pSession = create_session( SESSION_TYPE_DGRAM_CLIENT, &stTunnelSessionParam );
								if ( pNewTunnel->pSession )
								{
									log_print( "open tunnel ok........................." );
									iOpRet = 0;
								}
								else 	
									log_print( "open tunnel failed???????????????????" );
							}						
						}break ;
						default:
						{
							
						}break ;	
						}
						
						if ( iOpRet < 0 )
						{
							mem_free( pNewTunnel );
							pNewTunnel = NULL;
						}
						else 
							log_print( "iOpRet-->%d", iOpRet );
					}
				}
			}
			
			block_destroy( pThis->pBlock );
			pThis->pBlock = NULL;	
		}
	}
	
	return pNewTunnel;	
}

/*************************************************************************************************
* function name: destroy_tunnel
* param: CTunnel *pTunnel
* return value type: void
* description: close tunnel.
**************************************************************************************************/
static void destroy_tunnel( CTunnel *pTunnel )
{
	
}

static int32_t shine_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData )
{
	int32_t iRetCode = -1;
	
	CShine *pShine = (CShine *)pUserData;
	CCommonHeader *pMsgHeader = NULL;
	
	log_print( "shine session business calling.................." );	
	
	if ( !pShine || !pInData || iInDataLen <= 0 )
	{
		log_print( "param is invalid???????????????????????????" );
		
		return iRetCode;
	}
		
	if ( pShine->pBlock )
	{
		block_exiting( pShine->pBlock );
		
		pMsgHeader = (CCommonHeader *)pInData;
		
		switch ( pMsgHeader->iMsgId )
		{
		case MSG_ID_ACK_PEER:
		{
			AckTellPeer *pAckTellPeerMsg = (AckTellPeer *)pInData;
			
			memcpy( pShine->pPeerIP, pAckTellPeerMsg->pPeerIP, strlen(pAckTellPeerMsg->pPeerIP) );
			pShine->iPeerPort = pAckTellPeerMsg->iPeerPort;
			
			pShine->iState = SHINE_STATE_OK;
			
			log_print( "peer ip:%s, port:%d", pShine->pPeerIP, pShine->iPeerPort );
			
		}break ;
		default:
		{
			log_print( "unknown msg id?????????????????????" );
		}break ;
		}
		
		block_post( pShine->pBlock );
	}
	else 
		log_print( "shine block is NULL???????????????????" );
	
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

static int32_t tunnel_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData )
{
	int32_t iRetCode = -1;
	
	return iRetCode;	
}

static int32_t tunnel_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData )
{
	int32_t iRetCode = -1;
	
	return iRetCode;	
}

