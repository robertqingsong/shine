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
	
	int16u_t iDataSize;//total data length.
	
	int32u_t iCurrentSliceSize;//current slice data size.
}CDataHeader;

//data slice type define.
typedef struct CDataSlice_t
{
	CDataHeader stDataHeader;
	
	
}CDataSlice;

//data item type define.
typedef struct CData_t
{
	
}CData;

#if defined(__cplusplus)
}
#endif

#endif/* __DATA_H__ */
