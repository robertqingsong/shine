#include "prot.h"

void init_message( void *pMsgHeader, int32_t iMsgId, int32_t iMsgSize )
{
	if ( pMsgHeader )
	{
		CCommonHeader *pHeader = (CCommonHeader *)pMsgHeader;
		
		pHeader->iMsgId = iMsgId;
		pHeader->iMsgSize = iMsgSize;
	}
}
