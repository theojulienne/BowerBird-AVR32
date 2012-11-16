#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <intc.h>
#include <sysclk.h>
#include <tc.h>

#include "ADC.h"
#include "conf_usb.h"
#include "conf_board.h"

#define SAMPLE_SIZE (sizeof(int16_t))
#define SAMPLES_PER_PACKET (EP_SIZE_IN / SAMPLE_SIZE)


// Flag to update the timer value
volatile static bool update_timer = true;
// Variable to contain the time ticks occurred
volatile static uint32_t tc_tick = 0;




volatile static COMPILER_WORD_ALIGNED int16_t samples[SAMPLES_PER_PACKET];
volatile static int sampleIndex = 0;


/**
 * \brief TC interrupt.
 *
 * The ISR handles RC compare interrupt and sets the update_timer flag to
 * update the timer value.
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = EXAMPLE_TC_IRQ_GROUP, 1
__interrupt
#endif
static void tc_irq(void)
{
	// Increment the ms seconds counter
	tc_tick++;
	
	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

	// specify that an interrupt has been raised
	//update_timer = true;
	
	/*
	if ( tc_tick % 1000 == 0 ) {
		printf( "Rawr %d\r\n", sysclk_get_pba_hz() );
		tc_tick = 0;
	}
	*/
	
	if ( tc_tick == IN_FORMAT_LSBYTE_SAMPLE_FREQ ) {
		//printf( "weee %u %u\r\n", tc_tick, udi_audio_is_active() );
		tc_tick = 0;
	}
	
	if ( udi_audio_is_active() ) {
		int16_t a = tc_tick; //ADC_ReadSampleAndSetNextAddr( 4 );
		
		/*
		samples[sampleIndex] = LE16( ADC_ReadSampleAndSetNextAddr( 2 ) );
		sampleIndex++;
		*/
	
		for ( int j = 0; j < IN_FORMAT_NB_CHANNELS; j++ ) {
			samples[sampleIndex] = LE16( ADC_ReadSampleAndSetNextAddr( (j+1) % IN_FORMAT_NB_CHANNELS ) );
			sampleIndex++;
			
			if ( sampleIndex >= SAMPLES_PER_PACKET ) {
				udi_audio_send_samples( samples, (SAMPLE_SIZE * SAMPLES_PER_PACKET) );
				sampleIndex = 0;
			}
		}
	}
}

/**
 * \brief TC Initialization
 *
 * Initializes and start the TC module with the following:
 * - Counter in Up mode with automatic reset on RC compare match.
 * - fPBA/8 is used as clock source for TC
 * - Enables RC compare match interrupt
 * \param tc Base address of the TC module
 */
void tc_init(volatile avr32_tc_t *tc)
{
	// Options for waveform generation.
	static const tc_waveform_opt_t waveform_opt = {
		// Channel selection.
		.channel  = EXAMPLE_TC_CHANNEL,
		// Software trigger effect on TIOB.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,
		// RB compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,
		// Software trigger effect on TIOA.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,
		/*
		 * RA compare effect on TIOA.
		 * (other possibilities are none, set and clear).
		 */
		.acpa     = TC_EVT_EFFECT_NOOP,
		/*
		 * Waveform selection: Up mode with automatic trigger(reset)
		 * on RC compare.
		 */
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		// External event trigger enable.
		.enetrg   = false,
		// External event selection.
		.eevt     = 0,
		// External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		// Counter disable when RC compare.
		.cpcdis   = false,
		// Counter clock stopped with RC compare.
		.cpcstop  = false,
		// Burst signal selection.
		.burst    = false,
		// Clock inversion.
		.clki     = false,
		// Internal source clock 2, connected to fPBA / 2.
		.tcclks   = TC_CLOCK_SOURCE_TC2
	};

	// Options for enabling TC interrupts
	static const tc_interrupt_t tc_interrupt = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1, // Enable interrupt on RC compare alone
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0
	};
	
	INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, EXAMPLE_TC_IRQ_PRIORITY);
	
	// Initialize the timer/counter.
	tc_init_waveform(tc, &waveform_opt);

	/*
	 * Set the compare triggers.
	 * We configure it to count every 1 milliseconds.
	 * We want: (1 / (fPBA / 8)) * RC = 1 ms, hence RC = (fPBA / 8) / 1000
	 * to get an interrupt every 10 ms.
	 */
	
	tc_write_rc(tc, EXAMPLE_TC_CHANNEL, (sysclk_get_pba_hz() / 2 / IN_FORMAT_LSBYTE_SAMPLE_FREQ));
	// configure the timer interrupt
	tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &tc_interrupt);
	// Start the timer/counter.
	tc_start(tc, EXAMPLE_TC_CHANNEL);
}

