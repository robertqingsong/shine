#ifndef __SESSION_H__
#define __SESSION_H__

#include "config.h"
#include "net_api.h"

#include "lock.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum CSessionType_t
{
	SESSION_TYPE_DGRAM_CLIENT, 
	SESSION_TYPE_STREAM_CLIENT,
	SESSION_TYPE_MULTICAST_LISTENER, 
	SESSION_TYPE_DGRAM_SERVER, 
	SESSION_TYPE_STREAM_SERVER, 
	
	SESSION_TYPE_HTTP_CLIENT, 
	
	SESSION_TYPE_END
}C_SESSION_TYPE;

typedef enum CSessionEvent_t
{
	EVENT_READ_ERROR, 
	EVENT_WRITE_ERROR, 
	EVENT_ACCEPT_ERROR, 
	EVENT_ACCEPT_NEW_STREAM_CLIENT_NOTIFY
}C_SESSION_EVENT;

//http method define.
typedef enum C_HTTP_ACTION_T
{
	SESSION_HTTP_GET, 
	SESSION_HTTP_POST
}C_HTTP_ACTION;

typedef struct CEventParam_t
{
	CSocket *pSocket;
	
	CNetAddr stNetAddr;
	
}CEventParam;

typedef struct CSessionParam_t
{
	int8_t pIP[32];
	int16u_t iPort;

	int8_t pLocalIP[32];
	int16u_t iLocalPort;
}CSessionParam;

//http client type define.
typedef struct CHttpData_t
{
	void *pSetting;
	
	void *pParser;	
	
	int8_t *pURL;
	
	int8_t *pHost;
	
	int8_t *pHttpData;
	int32_t iHttpDataBufLen;
	int32_t iCurrentHttpDataLen;
	
	C_HTTP_ACTION eHttpAction;
	
	int32_t iEndFlag;
}CHttpData;

typedef int32_t (*session_business_t)( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData );

typedef int32_t (*session_event_t)( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData );

typedef struct CSession_t
{
	//init.
	int32_t (*init)( struct CSession_t *pThis, const CSessionParam *pParam );
	//release.
	void (*release)( struct CSession_t *pThis );

	//input data.
	int32_t (*handle_input)( const struct CSession_t *pThis, 
				 const CSocket *pSocket );

	//output data.
	int32_t (*handle_output)( const struct CSession_t *pThis, 
				  const int8u_t *pOutDatabuf, 
				  const int32_t iOutDataLen, 
				  const CNetAddr *pNetAddr );

	//business.
	int32_t (*handle_business)( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr, 
				    void *pUserData );

	//event.
	int32_t (*handle_event)( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize, 
				 void *pUserData );
				 
	//session param.
	CSessionParam stSessionParam;				 
				 
	//session data define.
	CReactor *pOwnerReactor;
	CSocket *pSocket;
	
	CNetAddr stPeerAddr;
	
	int32_t iBlockId;//block id.	
	
	void *pResultCode;//point to result.
	
	void *pUserData;
	
	CMutex Locker;
}CSession;

//init session.
int32_t init_session( void );

//release session.
void release_session( void );

//create session.
CSession *create_session( C_SESSION_TYPE eSessionType, const CSessionParam *pSessionParam );

//destroy session.
void destroy_session( CSession *pThis );

//set session business.
int32_t set_session_business( CSession *pThis, session_business_t business, void *pUserData );

//set session event.
int32_t set_session_event( CSession *pThis, session_event_t event );

//send data.
int32_t send_session_data( const CSession *pThis, const int8u_t *pData, const int32_t iDataLen, const CNetAddr *pNetAddr );

//add session socket.
int32_t add_session_socket( CSession *pThis, CSocket *pSocket );

//remove session socket.
int32_t remove_session_socket( CSession *pThis, const CSocket *pSocket );

#if defined(__cplusplus)
}
#endif

#endif/* __SESSION_H__ */

