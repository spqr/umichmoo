/* See license.txt for license information. */

#include "moo.h"
#include "mymoo.h"
#include "rfid.h"
#include "temp_sensor.h"
#include "sensor.h"

static int init_sensor();
static void read_temp_sensor(unsigned char volatile *target, unsigned long len);

static const struct sensor temp_sensor = {
	.sensor_id = 0x0E,
	.name      = "Temp",
	.init      = init_sensor,
	.read      = read_temp_sensor
};

sensor_init(temp_sensor);


static int init_sensor() {
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; // Turn on and set up ADC12
	return 0;
}

static void read_temp_sensor(unsigned char volatile *target, unsigned long len) {
  // setup ADC to read external analog temperature sensor
  ADC12CTL0 &= ~ENC;                              // make sure this is off otherwise settings are locked.
  P6SEL |= TEMP_EXT_IN;                           // Enable A/D channel A4
  //ADC12CTL0 = ADC12ON + SHT0_2;// + REFON; // Turn on and set up ADC12
  ADC12CTL1 = SHP;                                // Use sampling timer
  ADC12MCTL0 = INCH_TEMP_EXT_IN + SREF_1;         // Vr+=Vref+

  P1DIR |= TEMP_POWER;
  P1OUT |= TEMP_POWER;
  // turn on temperature sensor and allow it at least 1 ms to stabilize
  /* Spec says to wait longer than 1 ms, but lab testing shows that the data is
   * fairly accurate at 1 ms, and I don't want to sleep too long.
   */
  __delay_cycles(3000);

  ADC12CTL0 |= ENC | ADC12SC;    // enable and start conversion
  _BIC_SR(GIE);                  // disable interrupts while busy-waiting on ADC
  while (ADC12CTL1 & ADC12BUSY); // busy-wait for ADC to sample 
  ackReply[4] = (ADC12MEM0 & 0xff);        // grab that data.
  ackReply[3] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it
  ADC12CTL0 = ADC12CTL1 = 0;               // turn off ADC
  P1OUT &= ~TEMP_POWER;          // turn off temperature sensor
  
  // Reenable interrupts.
  _BIS_SR(GIE);
  
  // Store sensor read count. I assume this should be after reenabling interrupts.
  sensor_counter++;                              
  ackReply[8] = (sensor_counter & 0x00ff);      
  ackReply[7]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it
}