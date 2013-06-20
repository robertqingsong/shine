#ifndef __PROT_H__
#define __PROT_H__

#include "typedefs.h"

#include "shine.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define MSG_ID_REQUEST_PEER  (0x01)
#define MSG_ID_ACK_PEER  (0x02)

#define ALIGN_PACKED __attribute__ ( (packed) )

#pragma pack(1)

typedef struct CCommonHeader_t
{
	int32_t iMsgId;//msg id.
	
	int32_t iMsgSize;//msg len.
}ALIGN_PACKED CCommonHeader;

typedef struct CTellPeer_t
{
	CCommonHeader header;//common header.
	
	int8_t pShineId[MAX_SHINE_ID_SIZE];//shine id of mine.
}ALIGN_PACKED CTellPeer;

typedef struct CAckTellPeer_t
{
	CCommonHeader header;//common header.

	int8_t pPeerIP[MAX_IP_SIZE];
	int16u_t iPeerPort;

}ALIGN_PACKED AckTellPeer;

void init_message( void *pMsgHeader, int32_t iMsgId, int32_t iMsgSize );

#pragma pack ()

#if defined(__cplusplus)
}
#endif


#endif/* __PROT_H__ */

