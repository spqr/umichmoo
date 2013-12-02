#include "digital_accel_sensor.h"
#include "inttypes.h"

/* Private helper functions for this file, all declared static */
void _digital_accel_blocking_tx(uint8_t data); 
void _digital_accel_blocking_rx(); 
void _digital_accel_spi_select();
void _digital_accel_spi_deselect(); 

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

void digital_accel_power_off() {
	P1OUT &= ~(DIGITAL_ACCEL_POWER);
}

void digital_accel_init() {
	/*
	 * Setup for:
	 *   - Synchronous Mode
	 *   - 3-pin SPI Mode (Using non-standard select pin)
	 *   - Master Mode
	 *   - 8-bit character length
	 *   - MSB first
	 *   - CPOL = 0/CKPL = 0
	 *   - CPHA = 0/CKPH = 1
	 * See http://www.ti.com/lit/ug/slau144j/slau144j.pdf page 445 for details
	 */
	UCA0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;
	/*
	 * Setup for:
	 *   - Clock: SMCLK
   *   - Reset: Leave in last reset state
	 */
	UCA0CTL1 |= UCSSEL_2;

  /*
   * Clock divider = /1
   * Clock must be between 1MHz and 5MHz, per the ADXL362 spec
   */
  UCA0BR0 = 0x01;                           // /1
  UCA0BR1 = 0;                              //

  /*
   * Take SPI driver on MSP430 out of reset
   */
  UCA0CTL1 &= ~(UCSWRST);
}

void digital_accel_set_power(enum EDigitalAccelMode mode, enum EDigitalAccelLowNoise noise, uint8_t flags) {
  uint8_t data;
  data = (uint8_t) mode + (uint8_t) (noise << 4) + flags;
  digital_accel_write_address(DIGITAL_ACCEL_REG_POWER_CTL, data);
}

void digital_accel_set_filter(enum EDigitalAccelRange range, enum EDigitalAccelOdr odr, uint8_t flags) {
  uint8_t data;
  data = (uint8_t) (range << 6) + (uint8_t) odr + flags;
  digital_accel_write_address(DIGITAL_ACCEL_REG_FILTER_CTL, data);
}

void digital_accel_write_address(uint8_t address, uint8_t byte) {
  digital_accel_write_burst(address, &byte, 1);
}

uint8_t digital_accel_read_address(uint8_t address) {
  uint8_t buf[1];
  digital_accel_read_burst(address, buf, 1);
  return buf[0];
}

void digital_accel_read_burst(uint8_t start_address, uint8_t * data, uint8_t len) {
  uint8_t i;
  _digital_accel_spi_select();
  _digital_accel_blocking_tx(DIGITAL_ACCEL_READ);
  _digital_accel_blocking_tx(start_address);
  for(i = 0; i < len; i++) {
    _digital_accel_blocking_rx();
    data[i] = UCA0RXBUF;
  }
  _digital_accel_spi_deselect();
}

void digital_accel_write_burst(uint8_t start_address, uint8_t * data, uint8_t len) {
  uint8_t i;
  _digital_accel_spi_select();
  _digital_accel_blocking_tx(DIGITAL_ACCEL_WRITE);
  _digital_accel_blocking_tx(start_address);
  for(i = 0; i < len; i++) {
    _digital_accel_blocking_tx(data[i]);
  }
  _digital_accel_spi_deselect();
}

static void _digital_accel_blocking_tx(uint8_t data) {
  while (UCA0STAT & UCBUSY) ; // Wait for bus to clear
  while (!(IFG2 & UCA0TXIFG)) ; // Wait for previous transaction
  UCA0TXBUF = data;
}

static void _digital_accel_blocking_rx() {
  IFG2 &= ~(UCA0RXIFG); // Clear read interrupt so we can wait for it to be set
  _digital_accel_blocking_tx(DIGITAL_ACCEL_DUMMY);
  while (!(IFG2 & UCA0RXIFG)); // Wait for data to be ready
}

static void _digital_accel_spi_select() {
  /* CS is active low */
  P3OUT &= ~(DIGITAL_ACCEL_SEL);

}

static void _digital_accel_spi_deselect() {
  /* CS is active low */

	P3OUT |= DIGITAL_ACCEL_SEL;
}
