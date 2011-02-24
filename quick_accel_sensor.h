

/*
Copyright (c) 2010, UMass, Amherst, SPQR Lab
All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
conditions are met:
 
    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// these bit definitions are specific to Moo 1.0

#define SENSOR_DATA_TYPE_ID       0x0B

#define ACCEL_ENABLE_BIT          BIT4   // 1.4
#define SET_ACCEL_ENABLE_DIR      P1DIR |= ACCEL_ENABLE_BIT
#define CLEAR_ACCEL_ENABLE_DIR    P1DIR &= ~ACCEL_ENABLE_BIT
#define TURN_ON_ACCEL_ENABLE      P1OUT |= ACCEL_ENABLE_BIT
#define TURN_OFF_ACCEL_ENABLE     P1OUT &= ~ACCEL_ENABLE_BIT

#define DATA_LENGTH_IN_WORDS      3
#define DATA_LENGTH_IN_BYTES      (DATA_LENGTH_IN_WORDS*2)

unsigned char sensor_busy = 0;

void init_sensor()
{
  return;
}

void read_sensor(unsigned char volatile *target) 
{
  
  // turn off comparator
  P1OUT &= ~RX_EN_PIN;
  
  // slow down clock
  BCSCTL1 = XT2OFF + RSEL1; // select internal resistor (still has effect when DCOR=1)
  DCOCTL = DCO1+DCO0; // set DCO step. 
  
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
  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  ackReply[4] = (ADC12MEM0 & 0xff);
  ackReply[3] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

  // GRAB DATA
  ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
  ADC12CTL0 = ADC12ON + SHT0_1;
  ADC12CTL1 = SHP;                                  // Use sampling timer
  ADC12MCTL0 = INCH_ACCEL_Y + SREF_0;
  ADC12CTL0 |= ENC;
  ADC12CTL0 |= ADC12SC;
  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  ackReply[6] = (ADC12MEM0 & 0xff);
  ackReply[5] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

  // GRAB DATA
  ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
  ADC12CTL0 = ADC12ON + SHT0_1;
  ADC12CTL1 = SHP;
  ADC12MCTL0 = INCH_ACCEL_Z + SREF_0;
  ADC12CTL0 |= ENC;
  ADC12CTL0 |= ADC12SC;
  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  ackReply[8] = (ADC12MEM0 & 0xff);
  ackReply[7] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it
  
  // Power off sensor and adc
  P1DIR &= ~ACCEL_POWER;
  P1OUT &= ~ACCEL_POWER;
  ADC12CTL0 &= ~ENC;
  ADC12CTL1 = 0;       // turn adc off
  ADC12CTL0 = 0;       // turn adc off

  // Store sensor read count
  sensor_counter++;
  ackReply[10] = (sensor_counter & 0x00ff);
  ackReply[9]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it

  // turn on comparator
  P1OUT |= RX_EN_PIN;
}
