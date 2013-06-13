/* See license.txt for license information. */

#include "moo.h"
#include "rfid.h"
#include "sensor.h"

unsigned char sensor_busy = 0;
unsigned int SENSOR_DATA_TYPE_ID = 0;
unsigned int DATA_LENGTH_IN_WORDS = 0;
unsigned int DATA_LENGTH_IN_BYTES = 0;

void init_sensor(unsigned char sensor_data_type_id) {
  switch(sensor_data_type_id) {  
    // Temperature.    
    case 0x0E:
      SENSOR_DATA_TYPE_ID = 0x0E;
      DATA_LENGTH_IN_WORDS = 1;
      DATA_LENGTH_IN_BYTES = (DATA_LENGTH_IN_WORDS*2);      
    // Accelerometer.
    case 0x0B:
      // Fall through to default.
    default:
      SENSOR_DATA_TYPE_ID = 0x0B;
      DATA_LENGTH_IN_WORDS = 3;
      DATA_LENGTH_IN_BYTES = (DATA_LENGTH_IN_WORDS*2);
  }
  return;
}

// TODO: unsigned char volatile *target
void read_sensor() {
  unsigned int i;
  // We will assume that init is always called prior to read.
  switch(SENSOR_DATA_TYPE_ID) {
    // Temperature.
    case 0x0E:  
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
      
    // Accelerometer.
    case 0x0B:
      // Fall through to default.
    default:
      // turn off comparator
      P1OUT &= ~RX_EN_PIN;

      // slow down clock
      BCSCTL1 = XT2OFF + RSEL1; // select internal resistor (still has effect when
                                // DCOR=1)
      DCOCTL = DCO1+DCO0;       // set DCO step.

      if(!is_power_good())
        sleep();

      // Clear out any lingering voltage on the accelerometer outputs
      P6SEL = 0;
      P6OUT &= ~(ACCEL_X | ACCEL_Y | ACCEL_Z);
      P6DIR |=   ACCEL_X | ACCEL_Y | ACCEL_Z;
      P6DIR &= ~(ACCEL_X | ACCEL_Y | ACCEL_Z);

      P1DIR |= ACCEL_POWER;
      P1OUT |= ACCEL_POWER;
      P6SEL |= ACCEL_X | ACCEL_Y | ACCEL_Z;

      // a little time for regulator to stabilize active mode current AND
      // filter caps to settle.
      for(int i = 0; i < 225; i++);
      RECEIVE_CLOCK;

      // GRAB DATA
      ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
      ADC12CTL0 = ADC12ON + SHT0_1;                     // Turn on and set up ADC12
      ADC12CTL1 = SHP;                                  // Use sampling timer
      ADC12MCTL0 = INCH_ACCEL_X + SREF_0;               // Vr+=AVcc=Vreg=1.8V
      // ADC12CTL1 =  + ADC12SSEL_0 + SHS_0 + CONSEQ_0;
      ADC12CTL0 |= ENC;
      ADC12CTL0 |= ADC12SC;
      while (ADC12CTL1 & ADC12BUSY);           // wait while ADC finished work
      ackReply[4] = (ADC12MEM0 & 0xff);
      ackReply[3] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

      // GRAB DATA
      ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
      ADC12CTL0 = ADC12ON + SHT0_1;
      ADC12CTL1 = SHP;                                  // Use sampling timer
      ADC12MCTL0 = INCH_ACCEL_Y + SREF_0;
      ADC12CTL0 |= ENC;
      ADC12CTL0 |= ADC12SC;
      while (ADC12CTL1 & ADC12BUSY);           // wait while ADC finished work
      ackReply[6] = (ADC12MEM0 & 0xff);
      ackReply[5] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it
    
      // GRAB DATA
      ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
      ADC12CTL0 = ADC12ON + SHT0_1;
      ADC12CTL1 = SHP;
      ADC12MCTL0 = INCH_ACCEL_Z + SREF_0;
      ADC12CTL0 |= ENC;
      ADC12CTL0 |= ADC12SC;
      while (ADC12CTL1 & ADC12BUSY);           // wait while ADC finished work
      ackReply[8] = (ADC12MEM0 & 0xff);
      ackReply[7] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it
    
      // Power off sensor and adc
      P1DIR &= ~ACCEL_POWER;
      P1OUT &= ~ACCEL_POWER;
      ADC12CTL0 &= ~ENC;
      ADC12CTL1 = 0;       // turn adc off
      ADC12CTL0 = 0;       // turn adc off

      // turn on comparator
      P1OUT |= RX_EN_PIN;
  }
  
  // Store sensor read count.
  sensor_counter++;
  ackReply[10] = (sensor_counter & 0x00ff);
  ackReply[9]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it
}
