/* See license.txt for license information. */
#include "mymoo.h"
#include "sensor.h"
#include "moo.h"
#include "rfid.h"
#include "quick_accel_sensor.h"

#if MOO_VERSION != MOO1_1
#error "Unsupported moo version. Must be version 1.1"
#endif

static int init_sensor();
static void read_accel_sensor(unsigned char volatile *target, unsigned long len);

static const struct sensor accel_sensor = {
	.sensor_id = 0x0B,
	.name      = "Accel",
	.init      = init_sensor,
	.read      = read_accel_sensor
};

sensor_init(accel_sensor);

static int init_sensor()
{
  return 0;
}

static void read_accel_sensor(unsigned char volatile *target, unsigned long len)
{

  // turn off comparator
  P1OUT &= ~RX_EN_PIN;

  // slow down clock to around .13MHz
  // (0.13MHz from graph on page 168 of Newnes MSP430 Microcontroller Basics)
  BCSCTL1 = XT2OFF + RSEL1; // select internal resistor (still has effect when
                            // DCOR=1)
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
  // About 10.4ms, by oscilliscope measurement.
  // 3 instructions per cycle * i_MAX * 0.0077037 to calculate loop time (in ms)
  for(int i = 0; i < 360; i++);
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
  target[1] = (ADC12MEM0 & 0xff);
  target[0] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

  // GRAB DATA
  ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
  ADC12CTL0 = ADC12ON + SHT0_1;
  ADC12CTL1 = SHP;                                  // Use sampling timer
  ADC12MCTL0 = INCH_ACCEL_Y + SREF_0;
  ADC12CTL0 |= ENC;
  ADC12CTL0 |= ADC12SC;
  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  target[3] = (ADC12MEM0 & 0xff);
  target[2] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

  // GRAB DATA
  ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
  ADC12CTL0 = ADC12ON + SHT0_1;
  ADC12CTL1 = SHP;
  ADC12MCTL0 = INCH_ACCEL_Z + SREF_0;
  ADC12CTL0 |= ENC;
  ADC12CTL0 |= ADC12SC;
  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  target[5] = (ADC12MEM0 & 0xff);
  target[4] = (ADC12MEM0 & 0x0f00) >> 8; // grab msb bits and store it

  // Power off sensor and adc
  P1DIR &= ~ACCEL_POWER;
  P1OUT &= ~ACCEL_POWER;
  ADC12CTL0 &= ~ENC;
  ADC12CTL1 = 0;       // turn adc off
  ADC12CTL0 = 0;       // turn adc off

  // Store sensor read count
  sensor_counter++;
  target[7] = (sensor_counter & 0x00ff);
  target[6]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it

  // turn on comparator
  P1OUT |= RX_EN_PIN;
}