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

#define SPI_ADC_BAUDRATE    12000000 // 0
struct spi_device SPI_DEVICE_ADC = {
	//! Board specific select id
	.id = ADC_SPI_NPCS
};

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
	uint16_t rxData;
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
	uint16_t tmp = adc_sendWord( ADC_CR_SHIFT(ADC_CR_ADDR(addr)) );
	//printf( "raw: %04x (ch=%d, d=%d, s=%d, s2=%d)\r\n", tmp, (tmp >> 13), (tmp & 0x7FF), (int16_t)(tmp << 4) , ((int16_t)tmp) << 4 );
	//printf( "raw: %04x\r\n", tmp );
	//return ((int16_t)tmp) << 4;
	return (tmp & 0x1FFE) << 3;
}

#define SPI_MODE_2 2
#define SPI_MODE_3 3

void ADC_Init( ) {
	// enable the GPIO peripherals for the SPI bus to the ADC
	const gpio_map_t ADC_SPI_GPIO_MAP = {
		{ADC_SPI_SCK_PIN,   ADC_SPI_SCK_FUNCTION },  // SPI Clock.
		{ADC_SPI_MISO_PIN,  ADC_SPI_MISO_FUNCTION},  // MISO.
		{ADC_SPI_MOSI_PIN,  ADC_SPI_MOSI_FUNCTION},  // MOSI.
		{ADC_SPI_NPCS0_PIN, ADC_SPI_NPCS0_FUNCTION},  // Chip Select NPCS.
	};
	
	gpio_enable_module( ADC_SPI_GPIO_MAP, sizeof (ADC_SPI_GPIO_MAP) / sizeof (ADC_SPI_GPIO_MAP[0]));
	
	spi_master_init( ADC_SPI );
	spi_master_setup_device( ADC_SPI, &SPI_DEVICE_ADC, SPI_MODE_2, SPI_ADC_BAUDRATE, 0 );
	spi_enable( ADC_SPI );
	
	/* Reset the ADC (done twice at boot time). p23, Fig 24 of the datasheet. */
	adc_sendWord( 0xffff );
	adc_sendWord( 0xffff );
}