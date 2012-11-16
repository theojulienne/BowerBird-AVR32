#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_

#define ADC_SPI                 (&AVR32_SPI1)
#define ADC_SPI_NPCS            0
#define ADC_SPI_SCK_PIN         AVR32_SPI1_SCK_0_2_PIN
#define ADC_SPI_SCK_FUNCTION    AVR32_SPI1_SCK_0_2_FUNCTION
#define ADC_SPI_MISO_PIN        AVR32_SPI1_MISO_0_2_PIN
#define ADC_SPI_MISO_FUNCTION   AVR32_SPI1_MISO_0_2_FUNCTION
#define ADC_SPI_MOSI_PIN        AVR32_SPI1_MOSI_0_2_PIN
#define ADC_SPI_MOSI_FUNCTION   AVR32_SPI1_MOSI_0_2_FUNCTION
#define ADC_SPI_NPCS0_PIN       AVR32_SPI1_NPCS_0_1_PIN
#define ADC_SPI_NPCS0_FUNCTION  AVR32_SPI1_NPCS_0_1_FUNCTION


//! \note TC0 module is used in this example.
#define EXAMPLE_TC                 (&AVR32_TC0)
//! \note TC Channel 0 is used.
#define EXAMPLE_TC_CHANNEL         0
//! \note IRQ0 line of TC0 module channel 0 is used.
#define EXAMPLE_TC_IRQ             AVR32_TC0_IRQ0
//! \note IRQ Group of TC0 module
#define EXAMPLE_TC_IRQ_GROUP       AVR32_TC0_IRQ_GROUP
//! \note Interrupt priority 0 is used for TC in this example.
#define EXAMPLE_TC_IRQ_PRIORITY    AVR32_INTC_INT0


#endif /* CONF_BOARD_H_ */
