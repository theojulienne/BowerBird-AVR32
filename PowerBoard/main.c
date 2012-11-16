#include <board.h>
#include <sysclk.h>
#include <stdio_usb.h>
#include <gpio.h>
#include <usart.h>
#include <string.h>
#include <fifo.h>
#include <stdarg.h>
#include <wdt.h>

#include "comms.h"

#define BEAGLE_USART                 (&AVR32_USART0)
#define BEAGLE_USART_RX_PIN          AVR32_USART0_RXD_0_1_PIN
#define BEAGLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_1_FUNCTION
#define BEAGLE_USART_TX_PIN          AVR32_USART0_TXD_0_1_PIN
#define BEAGLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_1_FUNCTION
#define BEAGLE_USART_CLOCK_MASK      AVR32_USART0_CLK_PBA
#define BEAGLE_USART_IRQ             AVR32_USART0_IRQ
#define BEAGLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#define BEAGLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA

#define BEAGLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

void WriteStringToUSB(char *format, ...);

static void main_loop(void);
int main(void);

#define FIFO_BUFFER_SIZE 64
fifo_desc_t beagleOutFIFO;
char beagleOutBuffer[FIFO_BUFFER_SIZE];

fifo_desc_t beagleInFIFO;
char beagleInBuffer[FIFO_BUFFER_SIZE];

__attribute__((__interrupt__))
static void usart_int_handler(void) {
	int in;
	int ret;
	
	while ( (ret = usart_read_char( BEAGLE_USART, &in )) == USART_SUCCESS ) {
		fifo_push_uint8( &beagleInFIFO, in );
	}

#if 0
	if ( ret == USART_RX_ERROR ) {
		volatile avr32_usart_t *usart = BEAGLE_USART;
		
		usart_write_line( BEAGLE_USART, "RX ERROR:" );
		if ( usart->csr & AVR32_USART_CSR_OVRE_MASK ) {
			usart_write_line( BEAGLE_USART, "overrun" );
		}
		if ( usart->csr & AVR32_USART_CSR_FRAME_MASK ) {
			usart_write_line( BEAGLE_USART, "frame" );
		}
		if ( usart->csr & AVR32_USART_CSR_PARE_MASK ) {
			usart_write_line( BEAGLE_USART, "parity" );
		}
		usart_write_line( BEAGLE_USART, "!\r\n" );
	}
#endif
}

static void main_loop( ) {
	int in;
	
	while ( true ) {
		// if we have data coming in from USB, echo and pipe to beagle output buffer
		if ( udi_cdc_is_rx_ready() ) {
			in = udi_cdc_getc( );
			
			// pipe
			fifo_push_uint8( &beagleOutFIFO, in );
		}
		
		// if we have data in the output buffer and the usart can take it, send it off
		if ( fifo_get_used_size( &beagleOutFIFO ) > 0 && usart_tx_ready( BEAGLE_USART ) ) {
			usart_write_char( BEAGLE_USART, fifo_pull_uint8_nocheck( &beagleOutFIFO ) );
		}
		
		// if we have data in the beagle input buffer, process it
		if ( fifo_get_used_size( &beagleInFIFO ) > 0 ) {
			in = fifo_pull_uint8_nocheck( &beagleInFIFO );
			
			// handle it
			ProcessByte( in );
			
			// pipe if usb is connected and ready
			if ( udi_cdc_is_tx_ready() ) {
				udi_cdc_putc( in );
			}
		}
		
		/*	
		// echo
		if ( udi_cdc_is_tx_ready() ) {
			udi_cdc_putc( in );
		}
		
		// pipe
		usart_write_char( BEAGLE_USART, in );
		*/
		
		/*
		// echo
		usart_write_char( BEAGLE_USART, in );
		
		// pipe
		if ( udi_cdc_is_tx_ready() ) {
			udi_cdc_putc( in );
		}
		*/
	}
}

/* wrappers to make the old code work on the new avr32 platform */
void WriteStringToUSB(char *format, ...)
{
	int i, len;

	char string[MAX_LINE_LENGTH];
	va_list ap;
	va_start(ap, format);
	vsnprintf(string, MAX_LINE_LENGTH, format, ap);
	va_end(ap);

	len = strlen(string);
	for (i = 0; i < len; ++i) {
		while ( !udi_cdc_is_tx_ready() )
			;
		udi_cdc_putc( string[i] );
	}
}

void WriteStringToLCD(char *format, ...)
{
	
}

static void sio_init (void);

int main()
{
	static const gpio_map_t USART_GPIO_MAP =
	{
		{BEAGLE_USART_RX_PIN, BEAGLE_USART_RX_FUNCTION},
		{BEAGLE_USART_TX_PIN, BEAGLE_USART_TX_FUNCTION}
	};
	
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 115200,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};
	
	wdt_disable();
	
	/* Initialize basic board support features.
	 * - Initialize system clock sources according to device-specific
	 *   configuration parameters supplied in a conf_clock.h file.
	 * - Set up GPIO and board-specific features using additional configuration
	 *   parameters, if any, specified in a conf_board.h file.
	 */
	sysclk_init();
	board_init();
	
	// prepare buffers
	fifo_init( &beagleOutFIFO, beagleOutBuffer, FIFO_BUFFER_SIZE );
	fifo_init( &beagleInFIFO, beagleInBuffer, FIFO_BUFFER_SIZE );
	
	// Assign GPIO to USART.
	gpio_enable_module( USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]) );
	
	// Initialize USART in RS232 mode.
	usart_init_rs232( BEAGLE_USART, &USART_OPTIONS, BEAGLE_TARGET_PBACLK_FREQ_HZ );

    // Initialize interrupt vector table support.
    irq_initialize_vectors();

	irq_register_handler( &usart_int_handler, BEAGLE_USART_IRQ, 3 );
	BEAGLE_USART->ier = AVR32_USART_IER_RXRDY_MASK; // enable interrupt

    // Enable interrupts
    cpu_irq_enable();	

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
	
#if 0
 	/* Start and attach USB CDC device interface for devices with
 	 * integrated USB interfaces.
 	 */
	stdio_usb_init(&AVR32_USBB); // not sure why they use this when it's all software ?? --theoj

	// Specify that stdout and stdin should not be buffered.

#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin,  NULL);	
#endif
#endif
}

