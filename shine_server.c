#include "log.h"

#include "prot.h"

#include "session.h"

static int32_t shine_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData )
{
	int32_t iRetCode = -1;
	
	CCommonHeader *pMsgHeader = (CCommonHeader *)pInData;
	
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
			
			iRetCode = net_sendto( pSocket, &stAckTellPeerMsg, sizeof(stAckTellPeerMsg), pNetAddr );
			log_print( "send %d bytes", iRetCode );
		}break ;
		default:
		{
			
		}break ;	
		}	
	}
	
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

int main( int argc, char **argv )
{
	CSession *pServiceSession = NULL;
	CSessionParam stSessionParam;
	const int8u_t *pLocalIP = "192.168.1.101";

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_session(  ) < 0 )
		return -1;
		
	//if ( net_get_local_ip( pLocalIP, sizeof(pLocalIP) ) < 0 )
		//return -1;
		
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
