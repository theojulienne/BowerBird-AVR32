#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_audio.h"
#include <string.h>
#include <stdio.h>

bool udi_audio_control_enable(void);
void udi_audio_control_disable(void);
bool udi_audio_control_setup(void);
uint8_t udi_audio_control_getsetting(void);

bool udi_audio_streaming_enable(void);
void udi_audio_streaming_disable(void);
bool udi_audio_streaming_setup(void);
uint8_t udi_audio_streaming_getsetting(void);
//void udi_audio_streaming_sof_notify(void);

UDC_DESC_STORAGE udi_api_t udi_api_audio_control = {
	.enable = udi_audio_control_enable,
	.disable = udi_audio_control_disable,
	.setup = udi_audio_control_setup,
	.getsetting = udi_audio_control_getsetting,
	.sof_notify = NULL,
};
UDC_DESC_STORAGE udi_api_t udi_api_audio_streaming = {
	.enable = udi_audio_streaming_enable,
	.disable = udi_audio_streaming_disable,
	.setup = udi_audio_streaming_setup,
	.getsetting = udi_audio_streaming_getsetting,
	.sof_notify = NULL,
	//.sof_notify = udi_audio_streaming_sof_notify,
};

static volatile int setting = 0;

bool udi_audio_control_enable(void)
{
	//printf( "AC enable\r\n" );
	return true;
}

void udi_audio_control_disable(void)
{
	//printf( "AC disable\r\n" );
}

bool udi_audio_control_setup(void)
{
	//printf( "AC setup\r\n" );
	return false;
}

uint8_t udi_audio_control_getsetting(void)
{
	return 0;
}

bool udi_audio_streaming_enable(void)
{
	setting = udc_get_interface_desc()->bAlternateSetting;
	printf( "Changed to alt %d\r\n", setting );
	//printf( "AS enable\r\n" );
	return true;
}

void udi_audio_streaming_disable(void)
{
	printf( "AS disable\r\n" );
}

bool udi_audio_streaming_setup(void)
{
	//printf( "AS setup\r\n" );
	return false;
}

uint8_t udi_audio_streaming_getsetting(void)
{
	return setting;
}

// application interface

static void transfer_success(udd_ep_status_t status, iram_size_t nb_transfered)
{
	//printf( "Transfer success!\r\n" );
}


volatile bool udi_audio_is_active( void )
{
	return ( setting == 1 );
}

static COMPILER_WORD_ALIGNED uint8_t gBuffer[EP_SIZE_IN];

void udi_audio_send_samples( void *buffer, int bufferSize )
{
	memcpy( gBuffer, buffer, bufferSize );
	if ( !udd_ep_run( EP_AUDIO_IN, false, gBuffer, bufferSize, transfer_success ) ) {
		//printf( "Failed to send samples!\r\n" );
	}
}
