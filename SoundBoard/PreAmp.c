#include <twim.h>
#include <gpio.h>
#include <sysclk.h>
#include <stdio.h>
#include <stdint.h>
#include "PreAmp.h"

/* digital pot values used to calculate gains */
#define DIGITAL_POT_RESISTANCE_BASE 75
#define DIGITAL_POT_RESISTANCE_MAX 100000
#define PREAMP_INPUT_RESISTANCE 560
#define PREAMP_MINIMUM_SETPOINT ((uint8_t)3)
//FIXME should be calculated but this calc doesn't work
// #define PREAMP_MINIMUM_SETPOINT ((PREAMP_INPUT_RESISTANCE - DIGITAL_POT_RESISTANCE_BASE) * 0xFF / DIGITAL_POT_RESISTANCE_MAX))
#define PREAMP_MAXIMUM_SETPOINT ((uint8_t)0xFF)

#define RDAC_TWI_ADDR(addr) ((0x58 | (addr & 0x06)) >> 1)
#define RDAC_1_OR_3(addr) ((addr) & 0x1 ? 0x3 : 0x1)

#define TWIM               (&AVR32_TWIM1)  //! TWIM Module Used
//#define TARGET_BASE_ADDRESS     44 // 0b01011 00            //! Target's TWI address
#define TWIM_MASTER_SPEED  100000          //! Speed of TWI

int PreAmp_Init(void) {
	gpio_enable_gpio_pin( AVR32_PIN_PC01 );
	gpio_clr_gpio_pin( AVR32_PIN_PC01 ); // write protect
	
	gpio_enable_gpio_pin( AVR32_PIN_PA14 );
	gpio_enable_gpio_pin( AVR32_PIN_PA15 );
	const gpio_map_t TWIM_GPIO_MAP = {
		{ AVR32_TWIMS1_TWCK_0_PIN,	AVR32_TWIMS1_TWCK_0_FUNCTION },
		{ AVR32_TWIMS1_TWD_0_PIN, 	AVR32_TWIMS1_TWD_0_FUNCTION  },
	};
	
	// Set TWIM options
	const twi_options_t TWIM_OPTIONS = {
		.pba_hz = sysclk_get_pba_hz(),
		.speed = TWIM_MASTER_SPEED,
		.chip = RDAC_TWI_ADDR(0),
		.smbus = false,
	};
	
	// TWIM gpio pins configuration
	gpio_enable_module (TWIM_GPIO_MAP,
			sizeof (TWIM_GPIO_MAP) / sizeof (TWIM_GPIO_MAP[0]));
	
	return twim_master_init (TWIM, &TWIM_OPTIONS);
}

int PreAmp_read( uint8_t channel ) {
	char buf = 0x42;
	const twim_package_t transfer = {
		.chip = RDAC_TWI_ADDR(channel),
		.addr = RDAC_1_OR_3(channel),
		.addr_length = 1,
		
		.buffer = &buf,
		.length = 1,
	};
	
	if ( twim_read_packet( TWIM, &transfer ) == STATUS_OK ) {
		printf( "twim_read_packet: OK!\r\n" );
	} else {
		printf( "twim_read_packet: ERR\r\n" );
	}
	
	return buf;
}

int PreAmp_write( uint8_t channel, uint8_t value ) {
	const twim_package_t transfer = {
		.chip = RDAC_TWI_ADDR(channel),
		.addr = RDAC_1_OR_3(channel),
		.addr_length = 1,
		
		.buffer = &value,
		.length = 1,
	};
	
	gpio_set_gpio_pin( AVR32_PIN_PC01 ); // un-write protect
	
	int ret;
	if ( ( ret = twim_write_packet( TWIM, &transfer ) ) == STATUS_OK ) {
		printf( "twim_write_packet: OK!\r\n" );
	} else {
		printf( "twim_write_packet: ERR\r\n" );
	}
	
	gpio_clr_gpio_pin( AVR32_PIN_PC01 ); // write protect
	
	return ret;
}
