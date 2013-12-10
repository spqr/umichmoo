/* See license.txt for license information. */

#include "moo.h"
#include "mymoo.h"
#include "rfid.h"
#include "temp_sensor.h"

#if (ACTIVE_SENSOR == SENSOR_EXTERNAL_TEMP)

unsigned char sensor_busy = 0;

void init_sensor() {
  return;
}

void read_sensor(unsigned char volatile *target) {
  unsigned int i;

  // setup ADC to read external analog temperature sensor
  ADC12CTL0 &= ~ENC;                              // make sure this is off otherwise settings are locked.
  P6SEL |= TEMP_EXT_IN;                           // Enable A/D channel A4
  ADC12CTL0 = ADC12ON + SHT0_2 + REFON + REF2_5V; // Turn on and set up ADC12
  ADC12CTL1 = SHP;                                // Use sampling timer
  ADC12MCTL0 = INCH_TEMP_EXT_IN + SREF_1;         // Vr+=Vref+

  // turn on temperature sensor and allow it at least 0.7-0.8 ms to stabilize
  P1DIR |= TEMP_POWER;
  P1OUT |= TEMP_POWER;
  for (i = 0; i != 100; ++i);

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
  ackReply[10] = (sensor_counter & 0x00ff);      
  ackReply[9]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it
}

#endif