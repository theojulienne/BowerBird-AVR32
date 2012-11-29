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
#include "conf_usb.h"
#include "conf_board.h"
#include "ADC.h"

#include "ring_buffer.h"

#define NUM_ADC_CHANNELS 8
#define RING_BUFFER_SIZE 0xff

/*
typedef uint8_t adc_buffer_t[RING_BUFFER_SIZE];

adc_buffer_t adcBuffers[NUM_ADC_CHANNELS];
struct ring_buffer adcRingBuffers[NUM_ADC_CHANNELS];
*/

#define SPI_ADC_BAUDRATE    480000 // 0
struct spi_device SPI_DEVICE_ADC = {
	//! Board specific select id
	.id = ADC_SPI_NPCS
};

volatile uint32_t spiHandlerCalled = 0;
volatile uint32_t spiWrites = 0;

volatile int writeFlush = 0;
volatile int isLow = 0;
volatile uint16_t currReadBuf;
volatile uint16_t currWriteBuf = ADC_CR_SHIFT(ADC_CR_ADDR(0));

volatile int16_t latestSamples1[NUM_ADC_CHANNELS] = {-1,-1,-1,-1,-1,-1,-1,-1};
volatile int16_t latestSamples2[NUM_ADC_CHANNELS] = {-1,-1,-1,-1,-1,-1,-1,-1};
volatile uint8_t latestSelector[NUM_ADC_CHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile int latestChannel = 0;

#if CONFIG_SPI_MASTER_BITS_PER_TRANSFER != 16
#error Need 16 bit SPI
#endif

__attribute__((__interrupt__))
static void spi_int_handler(void)
{
	volatile avr32_spi_t *spi = ADC_SPI;
	
	if ( spi->SR.rdrf == 1 ) {
#if CONFIG_SPI_MASTER_BITS_PER_TRANSFER == 16
		currReadBuf = spi_get( spi );
		spiHandlerCalled++;
		
		uint8_t ch = (currReadBuf >> 13) & 0x7;
		int16_t sample = (currReadBuf & 0x1FFE) << 3;
		if ( latestSelector[ch] ) {
			latestSamples1[ch] = sample;
		} else {
			latestSamples2[ch] = sample;
		}
		latestSelector[ch] = !latestSelector[ch];
		//latestChannel = ch;
		
		/*if ( spiHandlerCalled % 10000 == 1000 ) {
			printf( "written: %04x\r\n", currWriteBuf );
			printf( "raw: %04x (ch=%d, d=%d, s=%d, s2=%d)\r\n", currReadBuf, (currReadBuf >> 13), (currReadBuf & 0x7FF), (int16_t)(currReadBuf << 4) , ((int16_t)currReadBuf) << 4 );
		}*/
		
		ch = (ch + 1) & 0x1; //0x7;
		currWriteBuf = ADC_CR_SHIFT(ADC_CR_ADDR(ch));
		
		//while (!spi_is_tx_ready(spi));
		
		if ( spi_is_tx_ready( spi ) ) {
			spi_deselect_device( ADC_SPI, &SPI_DEVICE_ADC );
			spi_select_device( ADC_SPI, &SPI_DEVICE_ADC );
			
			spiWrites++;
			spi_put( spi, currWriteBuf );
		}
#else
		uint8_t inByte;// = spi_get(spi) & 0xFF;
		spi_read_single( spi, &inByte );
		/*
		if ( writeFlush < 3 ) {
			writeFlush++;
			spi->tdr = 0xff; // send 3 more bytes of 0xff (ie, 0xffff + 0xffff, startup sequence). first 0xff sent in ADC_Init
			isLow = 1;
			return;
		}
		*/
		spiHandlerCalled++;
		
		if ( spiHandlerCalled % 10000 == 1 ) {
			printf( "raw: in %d (l=%d); %04x (ch=%d, d=%d, s=%d, s2=%d)\r\n", inByte, isLow, currReadBuf, (currReadBuf >> 13), (currReadBuf & 0x7FF), (int16_t)(currReadBuf << 4) , ((int16_t)currReadBuf) << 4 );
		}
		
		while (!spi_is_tx_ready(spi));
		
		if ( isLow ) {
			// we want to rx the low byte
			
			// low byte comes second, so or in
			currReadBuf |= inByte;
			
			// now that we've received the second byte, process it
			// XX: do stuff with currReadBuf
			uint8_t ch = (currReadBuf >> 13) & 0x7;
			int16_t sample = (currReadBuf & 0x1FFE) << 3;
			latestSamples[ch] = sample;
			latestChannel = ch;
			
			// now prepare the next write
			ch = (ch + 1) % NUM_ADC_CHANNELS;
			currWriteBuf = ADC_CR_SHIFT(ADC_CR_ADDR(0));
			
			// and queue writing of the next high byte 
			spi_write_single( spi, (currWriteBuf >> 8) );
			
			isLow = 0;
		} else {
			// we want to tx the high byte
			
			// high byte comes first, so clear
			currReadBuf = (inByte << 8);
			
			// now queue writing of the low high byte
			spi_write_single( spi, (currWriteBuf & 0xff) );
			
			isLow = 1;
		}
#endif
	}
#if 0
 	else if ( spi->SR.tdre == 1 ) {
		//spi->tdr = 0xff;
		/*
		if ( writeLow ) {
			spi->tdr = writeData & 0xff;
			writeLow = 0;
		} else {
			spi->tdr = writeData >> 8;
			writeLow = 1;
		}*/
		
		spi->IER.tdre = 0;
	}
#endif
}

static status_code_t spi_rxtx_safe( volatile avr32_spi_t *spi, uint8_t toSend, uint8_t *received ) {
	unsigned int timeout;
	
	// wait until we can transmit
	timeout = SPI_TIMEOUT;
	while (!spi_is_tx_ready(spi)) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	
	// write our single byte
	spi_write_single( spi, toSend );
	
	// wait until we can receive
	timeout = SPI_TIMEOUT;
	while (!spi_is_rx_ready(spi)) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	spi_read_single( spi, received );
	
	return STATUS_OK;
}



static uint16_t adc_sendWord( uint16_t word ) {
	uint16_t rxData = 0;
	uint8_t b;
	int result;
	
	spi_select_device( ADC_SPI, &SPI_DEVICE_ADC );
	
	result = spi_rxtx_safe( ADC_SPI, word >> 8, &b );
	if ( result == STATUS_OK ) {
		rxData = b << 8;
	} else {
		printf( "Failure writing/reading high word. " );
		if ( result == ERR_TIMEOUT ) {
			printf( "(timeout)" );
		}
		printf( "\r\n" );
	}
	
	result = spi_rxtx_safe( ADC_SPI, word & 0xff, &b );
	if ( result == STATUS_OK ) {
		rxData |= b;
	} else {
		printf( "Failure writing/reading low word. " );
		if ( result == ERR_TIMEOUT ) {
			printf( "(timeout)" );
		}
		printf( "\r\n" );
	}
	
	spi_deselect_device( ADC_SPI, &SPI_DEVICE_ADC );
	
	return rxData;
}

int16_t ADC_ReadSampleAndSetNextAddr( uint8_t addr ) {
	/* Drop the leading 0 and the three address bits, retaining the 12-bit sample. */
	return 0;
	uint16_t tmp = adc_sendWord( ADC_CR_SHIFT(ADC_CR_ADDR(addr)) );
	printf( "raw: %04x (ch=%d, d=%d, s=%d, s2=%d)\r\n", tmp, (tmp >> 13), (tmp & 0x7FF), (int16_t)(tmp << 4) , ((int16_t)tmp) << 4 );
	//printf( "raw: %04x\r\n", tmp );
	//return ((int16_t)tmp) << 4;
	return (tmp & 0x1FFE) << 3;
}

#define SPI_MODE_2 2
#define SPI_MODE_3 3

int ADC_Init( ) {
	// enable the GPIO peripherals for the SPI bus to the ADC
	const gpio_map_t ADC_SPI_GPIO_MAP = {
		{ADC_SPI_SCK_PIN,   ADC_SPI_SCK_FUNCTION },  // SPI Clock.
		{ADC_SPI_MISO_PIN,  ADC_SPI_MISO_FUNCTION},  // MISO.
		{ADC_SPI_MOSI_PIN,  ADC_SPI_MOSI_FUNCTION},  // MOSI.
		{ADC_SPI_NPCS0_PIN, ADC_SPI_NPCS0_FUNCTION},  // Chip Select NPCS.
	};
	
	gpio_enable_module( ADC_SPI_GPIO_MAP, sizeof (ADC_SPI_GPIO_MAP) / sizeof (ADC_SPI_GPIO_MAP[0]));
	
	spi_master_init( ADC_SPI );
	spi_master_setup_device( ADC_SPI, &SPI_DEVICE_ADC, SPI_MODE_2, SPI_ADC_BAUDRATE, 0 ); // was originally 'SPI_MODE_2'
	ADC_SPI->CSR0.csnaat = 1;
	
	// Enable SPI interrupt mode
	INTC_register_interrupt( &spi_int_handler, ADC_SPI_IRQ, AVR32_INTC_INT0 ); 
	
	// Initialise the ring buffers for each channel
	/*for ( int ch = 0; ch < NUM_ADC_CHANNELS; ch++ ) {
		adcRingBuffers[ch] = ring_buffer_init( adcBuffers[ch], RING_BUFFER_SIZE );
	}*/
	
	//ADC_SPI->IER.rdrf = 1; // Receive Data Register Full
	//ADC_SPI->IER.tdre = 1; // Transmit Data Register Empty
	
#if CONFIG_SPI_MASTER_BITS_PER_TRANSFER == 8
	spi_enable( ADC_SPI );
	
	/* Reset the ADC (done twice at boot time). p23, Fig 24 of the datasheet. */
	adc_sendWord( 0xffff );
	adc_sendWord( 0xffff );
	
	spi_disable( ADC_SPI );
#endif

	ADC_SPI->IER.rdrf = 1; // Receive Data Register Full
	//ADC_SPI->IER.tdre = 1; // Transmit Data Register Empty
	
	spi_enable( ADC_SPI );
	
	spi_select_device( ADC_SPI, &SPI_DEVICE_ADC );
	
#if CONFIG_SPI_MASTER_BITS_PER_TRANSFER == 16
	ADC_SPI->IER.rdrf = 0;
	
	while (!spi_is_tx_ready(ADC_SPI));
	spi_put( ADC_SPI, 0xffff );
	while (!spi_is_rx_ready(ADC_SPI));
	spi_get( ADC_SPI );
	
	while (!spi_is_tx_ready(ADC_SPI));
	spi_put( ADC_SPI, 0xffff );
	while (!spi_is_rx_ready(ADC_SPI));
	spi_get( ADC_SPI );
	
	ADC_SPI->IER.rdrf = 1;
#endif

	spi_deselect_device( ADC_SPI, &SPI_DEVICE_ADC );
	/*
	spi_disable( ADC_SPI );
	spi_enable( ADC_SPI );
	*/
	spi_select_device( ADC_SPI, &SPI_DEVICE_ADC );

	while (!spi_is_tx_ready(ADC_SPI));
#if CONFIG_SPI_MASTER_BITS_PER_TRANSFER == 16
	spi_put( ADC_SPI, currWriteBuf );
#else
	isLow = 0;
	spi_put( ADC_SPI, currWriteBuf >> 8 ); // write first high byte
#endif
	
	return 0;
}