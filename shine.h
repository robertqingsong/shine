#ifndef __SHINE_H__
#define __SHINE_H__

#include "typedefs.h"

#include "net_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//shine type define.
typedef enum C_SHINE_TYPE_T
{
	SHINE_TYPE_SENDER, //send shine.
	SHINE_TYPE_RECEIVER//receive shine.
}C_SHINE_TYPE;

//shine operation error code.
typedef enum C_SHINE_ERROR_CODE_T
{
	SHINE_ERROR_CODE_RECV_BUF_SMALL
}C_SHINE_ERROR_CODE;

//shine param type define.
typedef struct CShineParam_t
{
	C_SHINE_TYPE eShineType;	
	
	CSocket *pSocket;//socket to read or write data.
	
	int32_t iIsReliable;//if it's Reliable data transportation.
}CShineParam;

//open shine so that it can send or receive data.
void *open_shine( const CShineParam *pShineParam );

//close shine to release resource.
void close_shine( void *pHandle );

//read shine data.
int32_t read_shine_data( const void *pHandle, int8u_t *pRecvBuf, const int32_t iRecvBufSize );

//write shine data.
int32_t write_shine_data( const void *pHandle, const int8u_t *pSendBuf, const int32_t iSendBufSize );

#if defined(__cplusplus)
}
#endif

#endif/* __SHINE_H__ */
