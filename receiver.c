#include "shine.h"

#include "log.h"

//shine event callback type define.
int32_t shine_event_callback( void *pHandle, C_SHINE_EVENT eEvent, void *pUserData )
{
	int32_t iRetCode = -1;
	
	log_print( "shine event callback..........." );
	
	
	return iRetCode;	
}

int main( int argc, char **argv )
{
	void *pShineHandle = NULL;
	CShineParam stShineParam;
	const int8_t *pShineServerIP = "192.168.1.101";
	const int16u_t iShineServerPort = 9000;
	const int8_t *pShineId = "dsflseewfreeeeeeeeee";

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	
	if ( init_shine(  ) < 0 )
		return -1;
		
	//
//	void *open_shine( const CShineParam *pShineParam );
	
	memset( &stShineParam, 0x00, sizeof(stShineParam) );
	memcpy( stShineParam.pServerIP, pShineServerIP, strlen(pShineServerIP) + 1 );
	stShineParam.iServerPort = iShineServerPort;
	
	memcpy( stShineParam.pLocalIP, pShineServerIP, strlen(pShineServerIP) + 1 );
	stShineParam.iLocalPort = 8001;
	
	memcpy( stShineParam.pShineId, pShineId, strlen(pShineId) + 1 );
	
	pShineHandle = open_shine( &stShineParam );
	if ( pShineHandle )
	{
		//int32_t open_shine_tunnel( void *pHandle, int8_t *pPeerShineId );
		int32_t iTunnelId = -1;

		log_print( "open shine ok................" );
			
		iTunnelId = open_shine_tunnel( pShineHandle, "134354325" );
		if ( iTunnelId >= 0 )
		{
			log_print( "open shine tunnel ok.............." );
		}
		else 	
			log_print( "open shine tunnel failed." );
	}
	
	while ( 1 )
		os_sleep( 1000 );
		
	release_shine(  );	
	
	return 0;	
}
