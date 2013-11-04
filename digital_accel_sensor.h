/* See license.txt for license information. */

// these bit definitions are specific to Moo 1.2

#ifndef __DIGITAL_ACCEL_SENSOR_
#define __DIGITAL_ACCEL_SENSOR_

#define SENSOR_DATA_TYPE_ID       0x0D

#define DIGITAL_ACCEL_POWER       BIT4   // Pin 1.4

/*
 * Clock should be between 1 MHz and 5 MHz, according to the spec
 */
#define DIGITAL_ACCEL_CLK         BIT0   // Pin 3.0 - UCA0CLK
#define DIGITAL_ACCEL_MOSI        BIT4   // Pin 3.4 - UCA0SIMO
#define DIGITAL_ACCEL_MISO        BIT5   // Pin 3.5 - UCA0SOMI
#define DIGITAL_ACCEL_SEL         BIT6   // Pin 3.6 - GPIO 3.6
#define DIGITAL_ACCEL_INT1        BIT1   // Pin 6.1
#define DIGITAL_ACCEL_INT2        BIT0   // Pin 6.0

/*
 * Recommended USCI Initialization and Reset (from page 438 of MSP430x2xx Family
 * Guide)
 *   1. Set UCSWRST (BIS.B #UCSWRST,&UCxCTL1) (Done on PUC)
 *   2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1) (Done on PUC)
 *   3. Configure ports (Can really start here if just initializing, not resetting)
 *   4. Clear UCSWRST via software (BIC.B, #UCSWRST, &UCxCTL1)
 *   5. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
 */

void digital_accel_power_on();
void digital_aceel_power_off();
void digital_accel_init();
void digital_accel_setup_pins();

#endif
