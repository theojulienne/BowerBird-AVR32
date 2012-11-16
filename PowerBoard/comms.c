#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <wdt.h>

#include "comms.h"

// defining this prevents the watchdog from actually resetting the beagle
//#define WATCHDOG_DRY_RUN

// defining this causes the watchdog to be reset by *any* comms from the beagle
//#define WATCHDOG_RESET_ON_ALL_COMMS

#define COMMAND_PREFIX "avr://"

#define WATCHDOG_CMD "watchdog"
#define WATCHDOG_ENABLE "enable"
#define WATCHDOG_DISABLE "disable"
#define WATCHDOG_PULSE "pulse"
#define WATCHDOG_TIMEOUT_S 600
#define WATCHDOG_PRESCALER 1024

#define LCD_CMD "lcd"
#define LCD_CLEAR_CMD "clear"
#define POWER_LCD "lcd"
#define POWER_PIN_LCD 7
#define DEVICE_NAME_LCD "LCD Panel"
// These lines need to be less than 24 characters
#define LCD_STARTUP_LINE1 "Bowerbird Starting..."
#define LCD_STARTUP_LINE2 ""

#define NEW_CABLE

#define BEAGLE_RESET_CMD "REALLY reset the Beagleboard"

/*
#define POWER_PORT PORTC
#define ALL_POWER_PINS 8
#ifdef NEW_CABLE
	#define POWER_PIN_BEAGLE 7
	#define POWER_PIN_MIC 6
#else
	#define POWER_PIN_BEAGLE 0
	#define POWER_PIN_MIC 1
#endif
*/
#define POWER_PORT 0
#define ALL_POWER_PINS 0
#define POWER_PIN_BEAGLE 0
#define POWER_PIN_MIC 0



#define BEAGLE_RESET_DURATION_IN_S 10
#define POWER_CMD "power"
#define POWER_ON "on"
#define POWER_OFF "off"
#define POWER_MIC "usbmic"
#define DEVICE_NAME_MIC "USB Microphones"

#define RESET_CMD "REALLY reset the AVR"

void WriteStringToUSB(char *format, ...);
void WriteStringToLCD(char *format, ...);

/** Buffer to store the last AVR command received on the serial port */
char CommandBuffer[MAX_LINE_LENGTH];
short CommandBufferIndex = 0;
short CommandPrefixIndex = 0;
short InCommand = 0;


#define LCD_LINE_LENGTH 12
#define lcd_set_power_state(val)
#define lcd_get_power_state() 0
#define lcd_enter_low_power_mode()
#define lcd_init()
#define lcd_clear_screen()

// buffer to store the second line so it can be "scrolled" up to the first line
// on the next write
char LCD_Buffer[LCD_LINE_LENGTH + 1];
short LCD_Lines = 0;


void ProcessBeaglePowerDownCommand(char *cmd);
void ProcessPowerCommand(char *cmd);
void ProcessLCDCommand(char *cmd);
void ProcessWatchdogCommand(char *cmd);
void ProcessResetCommand(void);

void StartBeagleWatchdog(void);
void StopBeagleWatchdog(void);
void ResetBeagleWatchdog(void);
void SetDelayedBeaglePowerDown(uint32_t wait_seconds, uint32_t power_down_seconds);

/** Process bytes received on the serial port to see if there's any commands to
 *  the AVR itself. We're looking for strings that start with the #defined
 *  magic string and end with a newline.
 */
void ProcessByte(uint8_t ReceivedByte)
{
#ifdef WATCHDOG_RESET_ON_ALL_COMMS
	// reset the watchdog
	Beagle_Watchdog_Counter = 0;
#endif

	// If we're currently read in a command, then pay attention
	if (InCommand) {
		// check if this is the end of the line
		if (ReceivedByte == '\n' || ReceivedByte == '\r') {
			// null-terminate the string in the buffer
			CommandBuffer[CommandBufferIndex] = '\0';

			// see if we understand the command
			if (strncmp(CommandBuffer, BEAGLE_RESET_CMD, strlen(BEAGLE_RESET_CMD)) == 0) {
				ProcessBeaglePowerDownCommand(CommandBuffer + strlen(BEAGLE_RESET_CMD) + 1);
			}
			else if (strncmp(CommandBuffer, POWER_CMD, strlen(POWER_CMD)) == 0) {
				ProcessPowerCommand(CommandBuffer + strlen(POWER_CMD) + 1);
			}
			else if (strncmp(CommandBuffer, LCD_CMD, strlen(LCD_CMD)) == 0) {
				ProcessLCDCommand(CommandBuffer + strlen(LCD_CMD) + 1);
			}
			else if (strncmp(CommandBuffer, WATCHDOG_CMD, strlen(WATCHDOG_CMD)) == 0) {
				ProcessWatchdogCommand(CommandBuffer + strlen(WATCHDOG_CMD) + 1);
			}
			else if (strncmp(CommandBuffer, RESET_CMD, strlen(RESET_CMD)) == 0) {
				ProcessResetCommand();
			}
			else {
				WriteStringToUSB("\r\nGot unknown AVR command '%s'\r\n", CommandBuffer);
				WriteStringToLCD("Unknown command:");
				WriteStringToLCD(CommandBuffer);
			}

			// clear the buffer
			CommandBufferIndex = 0;
			// clear the flag
			InCommand = 0;
		}
		else if (CommandBufferIndex < MAX_LINE_LENGTH) {
			CommandBuffer[CommandBufferIndex++] = ReceivedByte;
		}
	}
	// see if this is a line for the AVR
	else if (ReceivedByte == COMMAND_PREFIX[CommandPrefixIndex]) {
		CommandPrefixIndex++;
		if (CommandPrefixIndex >= strlen(COMMAND_PREFIX)) {
			CommandPrefixIndex = 0;
			InCommand = 1;
		}
	}
	else {
		CommandPrefixIndex = 0;
	}
}

/** Handle a command to reset the beagleboard. This involves turning off power
 *  to the beagle power pin for a while, then turning it back on.
 */
void ProcessBeaglePowerDownCommand(char *cmd)
{
	uint32_t wait_seconds = 0;
	uint32_t power_down_seconds = 0;
    char *p;

	WriteStringToUSB("ProcessBeaglePowerDownCommand='%s'\r\n", cmd);
    wait_seconds = strtoul(cmd, &p, 10);
    power_down_seconds = strtoul(p, NULL, 10);
//    WriteStringToUSB("wait_seconds=%ld power_down_seconds=%ld cmd='%s' p='%s'\r\n", (long)wait_seconds, (long)power_down_seconds, cmd, p);
	if (!power_down_seconds)
		power_down_seconds = BEAGLE_RESET_DURATION_IN_S;
	WriteStringToUSB("\r\nIn (%ld) seconds powering Beagleboard off for (%ld) seconds\r\n", wait_seconds, power_down_seconds);
	WriteStringToLCD("Beagleboard -> wait");
	SetDelayedBeaglePowerDown(wait_seconds, power_down_seconds);
}


/** Handle a command to turn on or off power to one of the devices.
 *  This is just toggling gpios.
 */
void ProcessPowerCommand(char *cmd)
{
	int new_power_state, is_on_power_port, pin;
	char *device_name;

	// parse if it's an on or an off command
	if (strncmp(cmd, POWER_ON, strlen(POWER_ON)) == 0) {
		new_power_state = 1;
		cmd += strlen(POWER_ON) + 1;
	}
	else if (strncmp(cmd, POWER_OFF, strlen(POWER_OFF)) == 0) {
		new_power_state = 0;
		cmd += strlen(POWER_OFF) + 1;
	}
	else {
		WriteStringToUSB("\r\nGot unrecognised POWER command '%s'\r\n", cmd);
		WriteStringToLCD("Unknown power command:");
		WriteStringToLCD(cmd);
		return;
	}

    int sw;
    sw = 0;
	// now parse what device to turn on or off
	if (cmd[0] >= '1' && cmd[0] <= '9') {
		is_on_power_port = 1;
		pin = cmd[0] - '1';
        sw = pin + 1;
#ifdef NEW_CABLE
		pin = 7 - pin;
#endif
		device_name = "pin";
	}
	else if (strncmp(cmd, POWER_MIC, strlen(POWER_MIC)) == 0) {
		is_on_power_port = 1;
		pin = POWER_PIN_MIC;
		device_name = DEVICE_NAME_MIC;
	}
	else if (strncmp(cmd, POWER_LCD, strlen(POWER_LCD)) == 0) {
		is_on_power_port = 0;
		// remember if the LCD is on so we can ignore writes to it when its off
		lcd_set_power_state( new_power_state );
		// handle LCD power commands specially, because it's a GPIO directly to
		// the LCD power pin
		if (new_power_state) {
			// re-initialise the lcd
			lcd_init( );
		}
		else {
			// shut down all pins to the LCD (including the power enable pin)
			// to stop the LCD panel sourcing power from the control & data lines
			lcd_enter_low_power_mode( );
		}
		device_name = DEVICE_NAME_LCD;
		pin = 0;
	}
	else {
		WriteStringToUSB("\r\nGot request to turn %s unknown device: '%s'\r\n", 
				new_power_state ? "on" : "off", cmd);
		WriteStringToLCD("Unknown power device:");
		WriteStringToLCD(cmd);
		return;
	}

	// carry out command (if it's on the power port)
	if (is_on_power_port) {
		//PowerOn(pin, new_power_state);
	}

	// report to host
	WriteStringToUSB("\r\nAVR Power System: Turned %s %s[%d] sw%d\r\n",
				new_power_state ? "on" : "off", device_name, pin, sw);
	WriteStringToLCD("%s: %s[%d] sw%d", new_power_state ? "ON" : "OFF", device_name, pin, sw);
}


/** Handle a command to put a message on the LCD.
 *  String should be up to two lines of 24 or less characters
 */
void ProcessLCDCommand(char *cmd)
{
	if (strncmp(cmd, LCD_CLEAR_CMD, strlen(LCD_CLEAR_CMD)) == 0) {
		lcd_clear_screen();
		// clear the buffer
		LCD_Buffer[0] = '\0';
		LCD_Lines = 0;
		WriteStringToUSB("\r\nCleared LCD screen\r\n");
	}
	else {
		WriteStringToLCD(cmd);
		WriteStringToUSB("\r\nSent to LCD: '%s'\r\n", cmd);
	}
}


/** Turn the beagle watchdog on or off */
void ProcessWatchdogCommand(char *cmd)
{
	// parse if it's an enable or disable command
	if (strncmp(cmd, WATCHDOG_ENABLE, strlen(WATCHDOG_ENABLE)) == 0) {
		StartBeagleWatchdog();
	}
	else if (strncmp(cmd, WATCHDOG_DISABLE, strlen(WATCHDOG_DISABLE)) == 0) {
		StopBeagleWatchdog();
	}
	else if (strncmp(cmd, WATCHDOG_PULSE, strlen(WATCHDOG_PULSE)) == 0) {
		// Reset the beagle watchdog counter
		ResetBeagleWatchdog();
		WriteStringToUSB("WATCHDOG pulse\n");
		WriteStringToLCD("WATCHDOG pulse\n");
	}
	else {
		WriteStringToUSB("\r\nGot unrecognised WATCHDOG command '%s'\r\n", cmd);
		WriteStringToLCD("Unknown watchdog command:");
		WriteStringToLCD(cmd);
	}
}

wdt_opt_t opt = {
	.us_timeout_period = 500000 // 0.5s
};

void ProcessResetCommand(void) {
	wdt_enable( &opt ); // causes the AVR to reset in 0.5 sec 
	while(1);
}

void SetDelayedBeaglePowerDown(uint32_t wait_seconds, uint32_t power_down_seconds)
{
	
}

/** Start the watchdog timer */
void StartBeagleWatchdog()
{
	
}

/** Start the watchdog timer */
void StopBeagleWatchdog()
{
	
}

/** Reset the watchdog timer */
void ResetBeagleWatchdog() {
	
}


