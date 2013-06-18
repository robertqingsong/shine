#ifndef __DATA_H__
#define __DATA_H__

#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//data header type define.
typedef struct CDataHeader_t
{
	int16u_t iDataId;//primary data id.the data id.

	int8u_t iDataSubId;//secondary data id.current data slice id.
	
	int8u_t iDataSubIdRange;//data sub id range.
	
}CDataHeader;

//data slice type define.
typedef struct CDataSlice_t
{
	CDataHeader stDataHeader;
	
	int16u_t iTotalDataSize;//total data length.
	int32u_t iCurrentSliceSize;//current slice data size.
}CDataSlice;

//data item type define.
typedef struct CData_t
{
	CDataSlice *pDataSliceArray;//data slice array.
	int16u_t iDataSliceArraySize;//data slice array size.
	
	int8u_t *pDataBuf;//point to the data buffer.
	int16u_t iDataBufSize;//the data buffer length.
}CData;

//create data.
CData *create_data( const CDataHeader *pDataHeader );

//destroy data.
void destroy_data( CData *pData );

//add data slice.
int32_t add_data_slice( CData *pData, const CDataSlice *pDataSlice );

#if defined(__cplusplus)
}
#endif

#endif/* __DATA_H__ */
