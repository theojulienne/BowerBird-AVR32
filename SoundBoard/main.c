#include <board.h>
#include <sysclk.h>
#include <stdio_usb.h>
#include <gpio.h>
#include <usart.h>
#include <string.h>
#include <fifo.h>
#include <stdarg.h>
#include <wdt.h>
#include <spi_master.h>
#include <math.h>
#include "conf_board.h"
#include "conf_usb.h"
#include "conf_board.h"
#include "ADC.h"
#include "PreAmp.h"

void tc_init(volatile avr32_tc_t *tc);

static void main_loop(void);
int main(void);

/*
#define SAMPLE_SIZE (sizeof(int16_t))
#define SAMPLES_PER_PACKET (EP_SIZE_IN / SAMPLE_SIZE)
*/

static void main_loop( ) {
	int in;
	
	printf( "Initialising I2C...\r\n" );
	
	int ret = PreAmp_Init( );
	
	if ( ret == STATUS_OK ) {
		// display test result to user
		printf( "TARGET SLAVE FIND:\tPASS\r\n");
	} else {
		// display test result to user
		printf("TARGET CHIP FIND:\tFAILED\r\n");
	}
	
	for ( int i = 0; i < 8; i++ ) {
		PreAmp_write( i, 32 );
	}
	int result = PreAmp_read( 0 );
	
	printf( "result = %d\r\n", result );
	
	printf( "Initialising ADC...\r\n" );
	ADC_Init( );
	printf( "Done!\r\n" );
	
	while ( true ) {
		if ( udi_cdc_is_rx_ready() ) {
			in = udi_cdc_getc( );
			
			udi_cdc_putc( in );
			
			if ( in == '?' ) {
				printf( "sysclk_get_pba_hz() = %d\r\n", sysclk_get_pba_hz() );
			}
			
			if ( in == 'q' ) {
				printf( "Initialising I2C...\r\n" );
				
				int ret = PreAmp_Init( );
				
				if ( ret == STATUS_OK ) {
					// display test result to user
					printf( "TARGET SLAVE FIND:\tPASS\r\n");
				} else {
					// display test result to user
					printf("TARGET CHIP FIND:\tFAILED\r\n");
				}
				
				PreAmp_write( 0, 182 );
				int result = PreAmp_read( 0 );
				
				printf( "result = %d\r\n", result );
			}
			
			if ( in == 'a' ) {
//				printf( "Testing SPI...\r\n" );
				
				printf( "%x\r\n", ADC_ReadSampleAndSetNextAddr( 4 ) );
				/*
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 0 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 1 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 2 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 3 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 4 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 5 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 6 ) );
				printf( "%d\r\n", ADC_ReadSampleAndSetNextAddr( 7 ) );
				*/
				
//				printf( "Done!\r\n" );
			}
		}
	}
}


static void sio_init (void);

int main()
{
	wdt_disable();
	
	/* Initialize basic board support features.
	 * - Initialize system clock sources according to device-specific
	 *   configuration parameters supplied in a conf_clock.h file.
	 * - Set up GPIO and board-specific features using additional configuration
	 *   parameters, if any, specified in a conf_board.h file.
	 */
	sysclk_init();
	board_init();
	
	//ADC_Init( );
	
    // Initialize interrupt vector table support.
    irq_initialize_vectors();

    // Enable interrupts
    cpu_irq_enable();

	tc_init(EXAMPLE_TC);

	/* Call a local utility routine to initialize C-Library Standard I/O over
	 * a USB CDC protocol.  Tunable parameters in a conf_usb.h file must be
	 * supplied to configure the protocol correctly.
     */
    sio_init();

	// application login
	main_loop( );
}

/**
 * \brief Initialize USB CDC support for C-Library Standard I/O functions
 */
static void sio_init (void)
{
	udc_start ();

	if (! udc_include_vbus_monitoring ()) {
		stdio_usb_vbus_event (true);
	}
	
 	/* Start and attach USB CDC device interface for devices with
 	 * integrated USB interfaces.
 	 */
	stdio_usb_init(&AVR32_USBB); // not sure why they use this when it's all software ?? --theoj

	// Specify that stdout and stdin should not be buffered.

#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin,  NULL);	
#endif
}


/* usb functions */
void main_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void main_suspend_action(void)
{
	//ui_powerdown();
}

void main_resume_action(void)
{
	//ui_wakeup();
}

void main_sof_action(void)
{
	/*
	static int16_t samples[SAMPLES_PER_PACKET];
	static int setup = 0;
	
	for ( int i = 0; i < SAMPLES_PER_PACKET; i+=IN_FORMAT_NB_CHANNELS ) {
		for ( int j = 0; j < IN_FORMAT_NB_CHANNELS; j++ ) {
			samples[i+j] = ADC_ReadSampleAndSetNextAddr( 4 ); // (j+1) % IN_FORMAT_NB_CHANNELS );
		}
	}
	
	udi_audio_send_samples( samples, (SAMPLE_SIZE * SAMPLES_PER_PACKET) );
	*/
	/*
	static int sofCount = 0;
	
	sofCount++;
	
	if ( sofCount == 1000 ) {
		sofCount = 0;
		
		printf( "sof = %d\r\n", udd_get_frame_number() );
	}
	*/
	
	/*
	if ( sofCount == 400 ) {
		sofCount = 0;
		
		if ( udi_audio_is_active() ) {
			samples[sampleIndex] = sampleIndex; //ADC_ReadSampleAndSetNextAddr( 0 );
			sampleIndex++;
		
			if ( sampleIndex == SAMPLES_PER_PACKET ) {
				udi_audio_send_samples( samples, (SAMPLE_SIZE * SAMPLES_PER_PACKET) );
				sampleIndex = 0;
			}
		}
	}
	*/
	
	/*if ((!main_b_msc_enable) ||
		(!main_b_cdc_enable))
		return;
	ui_process(udd_get_frame_number());*/
}

