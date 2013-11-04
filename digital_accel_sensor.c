#include "digital_accel_sensor.h"

/*
 * Selects pin modes for pins of the accelerometer, in case they were configured
 * as GPIO pins.
 *
 * This function by itself will not setup the SPI interface.
 */
void digital_accel_setup_pins() {
	/* Accelerometer power Setup */
	P1SEL &= ~(DIGITAL_ACCEL_POWER); // Set P1.4 to GPIO
	P1DIR |= DIGITAL_ACCEL_POWER;    // Set Pin direction to output
	/* End Accelerometer Power Setup */
	
	/* Configure UCA0* Pins */
	P3SEL |= DIGITAL_ACCEL_CLK + DIGITAL_ACCEL_MOSI + DIGITAL_ACCEL_MISO;
	P3SEL &= ~(DIGITAL_ACCEL_SEL); // Non-standard select pin by design
	P3DIR |= DIGITAL_ACCEL_SEL;    // Again, setting up non-standard sel pin
	/* End configure UCA0* Pins */
}

/*
 * Turns on the accelerometer.
 *
 * Expects pin 1.4 to be setup properly already (GPIO mode selected, direction
 * set to out). If it is not properly configured, then call
 * digital_accel_setup_pins() first.
 */
void digital_accel_power_on() {
	P1OUT |= DIGITAL_ACCEL_POWER;
}

void digital_aceel_power_off() {
	P1OUT &= ~(DIGITAL_ACCEL_POWER);
}

void digital_accel_init();

void digital_accel_spi_setup() {
	/*
	 * Setup for:
	 *   - Synchronous Mode
	 *   - 3-pin SPI Mode (Using non-standard select pin)
	 *   - Master Mode
	 *   - 8-bit character length
	 *   - LSB first
	 *   - CPOL = 0
	 *   - CPHA = 0
	 * See http://www.ti.com/lit/ug/slau144j/slau144j.pdf page 445 for details
	 */
	UCA0CTL0 = UCMST + USYNC;
	/*
	 * Setup for:
	 *   - Clock: SMCLK
	 */
	UCA0CTL1 = UCSSEL1 + UCSSEL0 + UCSWRST;
}

void digital_accel_set_clock() {
	BCSCTL1 = XT2OFF + CAL_BC1_1MHZ;
	DCOCTL1 = CALDCO_1MHZ;
}
