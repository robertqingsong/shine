/*************************************************************************************************
* file name: shine.h
* writer: robert jerry.gzl.
* date time: 2013-6-20 17:11
* description: shine api head file.
*
**************************************************************************************************/
#ifndef __SHINE_H__
#define __SHINE_H__

/*************************************************************************************************
* head files include.
**************************************************************************************************/
#include "typedefs.h"

#include "net_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*************************************************************************************************
* macro define.
**************************************************************************************************/
#define MAX_SHINE_ID_SIZE  (64)

/*************************************************************************************************
* type define.
**************************************************************************************************/

//shine operation error code.
typedef enum C_SHINE_ERROR_CODE_T
{
	SHINE_ERROR_CODE_RECV_BUF_SMALL
}C_SHINE_ERROR_CODE;

//shine event define.
typedef enum C_SHINE_EVENT_T
{
	SHINE_EVENT_ACK_OPEN_TUNNEL //peer ack open tunnel request.
}C_SHINE_EVENT;

//shine param type define.
typedef struct CShineParam_t
{	
	int8_t pShineId[MAX_SHINE_ID_SIZE];//shine id.
	
	int8_t pServerIP[32];//server ip or domain name.
	int16u_t iServerPort;//shine server port.
	int8_t pLocalIP[MAX_IP_SIZE];
	int16u_t iLocalPort;//local port.
	
	int8_t pUserName[64];//user name.
	int8_t pUserPassword[64];//user password.
	
	int32_t iIsReliable;//if it's Reliable data transportation.
}CShineParam;

/*************************************************************************************************
* shine api declaration.
**************************************************************************************************/

/*************************************************************************************************
* function name: init_shine
* param: void
* return value type: int32_t
* description: init shine.
**************************************************************************************************/
int32_t init_shine( void );

/*************************************************************************************************
* function name: release_shine
* param: void
* return value type: int32_t
* description: release shine.
**************************************************************************************************/
void release_shine( void );

/*************************************************************************************************
* function name: open_shine
* param: const CShineParam *pShineParam// open param.
* return value type: void * //shine handle.
* description: open shine so that it can send or receive data.
**************************************************************************************************/
void *open_shine( const CShineParam *pShineParam );

/*************************************************************************************************
* function name: close_shine
* param: void *pHandle
* return value type: void
* description: close shine to release resource.
**************************************************************************************************/
void close_shine( void *pHandle );

/*************************************************************************************************
* function name: open_shine_tunnel
* param: void *pHandle
* return value type: int32_t //tunnel id.
* description: open shine tunnel for data transpartation.
**************************************************************************************************/
int32_t open_shine_tunnel( void *pHandle, int8_t *pPeerShineId );

/*************************************************************************************************
* function name: close_shine_tunnel
* param: void *pHandle, int32_t iTunnelId
* return value type: int32_t //tunnel id.
* description: close shine tunnel.close data transfer.
**************************************************************************************************/
int32_t close_shine_tunnel( void *pHandle, int32_t iTunnelId );

/*************************************************************************************************
* function name: start_shine_tunnel
* param: void *pHandle
* return value type: int32_t //tunnel id.
* description: start tunnel id service for data processing.
**************************************************************************************************/
int32_t start_shine_tunnel( void *pHandle );

/*************************************************************************************************
* function name: stop_shine_tunnel
* param: void *pHandle, const int32_t iTunnelNo
* return value type: 0 ok, -1 failed.
* description: stop shine tunnel.
**************************************************************************************************/
int32_t stop_shine_tunnel( void *pHandle, const int32_t iTunnelId );

/*************************************************************************************************
* function name: read_shine_data
* param: const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize
* return value type: int32_t //data that have been read.
* description: read shine data.
**************************************************************************************************/
int32_t read_shine_data( const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize );

//
/*************************************************************************************************
* function name: write_shine_data
* param: const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize 
* return value type: int32_t //data that have been written.
* description: write shine data.
**************************************************************************************************/
int32_t write_shine_data( const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize );

#if defined(__cplusplus)
}
#endif

#endif/* __SHINE_H__ */
