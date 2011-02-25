// Copyright (c) 2009, Intel Corporation

// Copyright 2011 UMass, Amherst, PRISMS Lab.

// Exercise Moo1.0 Sensors

// Voltage sensor is trying to measure how fast we can get a good sample

// Accel is showing the exponential chargeup of the filter = V*(1-exp(-t/rc))

// Temp is not done yet

// Cap is not done yet

#include "msp430x26x.h"

#include "pinDefMoo1.0.h"

// sensors
#define SENSOR_VOLTAGE  1
#define SENSOR_ACCEL_X  2
#define SENSOR_ACCEL_Y  3
#define SENSOR_ACCEL_Z  4
#define SENSOR_TEMP_INT 5
#define SENSOR_TEMP_EXT 6

// set yours here:
const unsigned short sensor = SENSOR_ACCEL_X;

// variables
volatile unsigned short adc_sample;
unsigned short max = 0, maxID = 0;
unsigned short min = 4096, minID = 0;
unsigned short diff = 8000;
unsigned short diffID = 8000;
unsigned short diffadc_sample = 0;

#define MAX_HISTORY 50 // keep in mind we only have 512 bytes ram.
unsigned short history_idx = 0;
unsigned short history[MAX_HISTORY + 1];
unsigned short maxhistory[MAX_HISTORY + 1];
unsigned short minhistory[MAX_HISTORY + 1];
unsigned short delhistory[MAX_HISTORY + 1];

unsigned short ldiff = 0;
unsigned short ldiffID = 8000;


volatile unsigned int best_delta = 4096;
volatile unsigned int delta_idx;



inline void sensor_power(unsigned short sensorOn)
{
  
  switch(sensor)
  {
  case SENSOR_VOLTAGE:
      P4DIR |= VSENSE_POWER;
      if(sensorOn) P4OUT |= VSENSE_POWER;
      else         P4OUT &= ~VSENSE_POWER;
    break;
  case SENSOR_ACCEL_X:
  case SENSOR_ACCEL_Y: // fall through, no break
  case SENSOR_ACCEL_Z: // fall through, no break
      P1DIR |= ACCEL_POWER;
      if(sensorOn) P1OUT |= ACCEL_POWER;
      else         P1OUT &= ~ACCEL_POWER;
    break;
  case SENSOR_TEMP_EXT:
      P1DIR |= TEMP_POWER;
      if(sensorOn) P1OUT |= TEMP_POWER;
      else         P1OUT &= ~TEMP_POWER;
    break;
    
  default:
    while(1); // lock up - not supported.
  } 
  
}

inline void setup_adc()
{
  switch(sensor)
  {
  case SENSOR_VOLTAGE:
    // setup adc for external voltage
    // The reference voltage is set to 2.5V from REF generator, which is not 
    // available without programmer (3.3V)
    ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
    P6SEL |= INCH_VSENSE_IN;                          // Enable A/D channel A0
    ADC12CTL0 = ADC12ON + SHT0_2 + REFON + REF2_5V;   // Turn on and set up ADC12
    ADC12CTL1 = SHP;                                  // Use sampling timer
    ADC12MCTL0 = INCH_VSENSE_IN + SREF_1;             // Vr+=Vref+
    break;
  case SENSOR_ACCEL_X:
    // setup adc for accelerameter X
    ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
    P6SEL |= INCH_ACCEL_X;                            // Enable A/D channel A1
    ADC12CTL0 = ADC12ON + SHT0_1;                     // Turn on and set up ADC12
    ADC12CTL1 = SHP;                                  // Use sampling timer
    ADC12MCTL0 = INCH_ACCEL_X + SREF_0;               // Vr+=AVcc=Vreg=1.8V
    break;
  case SENSOR_ACCEL_Y:
    // setup adc for accelerameter Y
    ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
    P6SEL |= INCH_ACCEL_Y;                            // Enable A/D channel A2
    ADC12CTL0 = ADC12ON + SHT0_1;                     // Turn on and set up ADC12
    ADC12CTL1 = SHP;                                  // Use sampling timer
    ADC12MCTL0 = INCH_ACCEL_Y + SREF_0;               // Vr+=AVcc=Vreg=1.8V
    break;
  case SENSOR_ACCEL_Z:
    // setup adc for accelerameter Z
    ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
    P6SEL |= INCH_ACCEL_Z;                            // Enable A/D channel A3
    ADC12CTL0 = ADC12ON + SHT0_1;                     // Turn on and set up ADC12
    ADC12CTL1 = SHP;                                  // Use sampling timer
    ADC12MCTL0 = INCH_ACCEL_Z + SREF_0;               // Vr+=AVcc=Vreg=1.8V
    break;
  case SENSOR_TEMP_EXT:
    // setup adc for external temperature sensor
    ADC12CTL0 &= ~ENC; // make sure this is off otherwise settings are locked.
    P6SEL |= INCH_TEMP_EXT_IN;                        // Enable A/D channel A4
    ADC12CTL0 = ADC12ON + SHT0_2 + REFON + REF2_5V;   // Turn on and set up ADC12
    ADC12CTL1 = SHP;                                  // Use sampling timer
    ADC12MCTL0 = INCH_TEMP_EXT_IN + SREF_1;           // Vr+=Vref+
    break;
  default:
    while(1); // lock up - not supported.
  }
    
}


inline void sample_adc()
{ 
  // start conversion
  ADC12CTL0 |= ENC;      // Enable conversion
  ADC12CTL0 |= ADC12SC;  // Start conversion

  while (ADC12CTL1 & ADC12BUSY);    // wait while ADC finished work
  adc_sample = ADC12MEM0;  // store sample
}


inline void process_sample(unsigned short sample_idx)
{ 
  switch(sensor)
  {
  case SENSOR_VOLTAGE:
  case SENSOR_TEMP_EXT:
    
    // find max/min
    if (adc_sample < min)
    {
      min = adc_sample;
      minID = sample_idx;
    }
    else if (adc_sample > max)
    {
      max = adc_sample;
      maxID = sample_idx;
    }
    break;
  case SENSOR_ACCEL_X: // fall through, no break
  case SENSOR_ACCEL_Y: // fall through, no break
  case SENSOR_ACCEL_Z:
    
    // store sample
    history[sample_idx] = adc_sample;
    
    // find max/min
    if (adc_sample < minhistory[sample_idx])
      minhistory[sample_idx] = adc_sample;
    if (adc_sample > maxhistory[sample_idx])
      maxhistory[sample_idx] = adc_sample;
    
    break;
  default:
    while(1); // lock up - not supported.
  }
  
}



void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  /*
  // Setup 1mhz DCO from cal register
  if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1);                               // If calibration constants erased
                                            // do not load, trap CPU!!
  }   
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  */
  
  
  // setup timer A to free-run so we can measure time easily
  TACTL = TASSEL_2 + MC_2;                  // SMCLK, Continuous mode
  
//  P4DIR |= 0x04;                        // Set P4.2 to output direction for LED

  setup_adc();
//  for (int i = 0; i < 0x3600; i++);                // Delay for reference start-up
  
  // Clear out max/min
  // max already at 0
  // set min to 4096
  for(int j = 0; j < MAX_HISTORY; j++) minhistory[j] = 4096;
  
  while (1)
  {
    // reset stats
    max = 0;
    min = 4096;
    diffID = 8000;
    diff = 8000;
    
    // Clear out any lingering voltage on the accelerometer outputs
    P6OUT &= ~(ACCEL_X | ACCEL_Y | ACCEL_Z);
    P6DIR |=   ACCEL_X | ACCEL_Y | ACCEL_Z;
    P6DIR &= ~(ACCEL_X | ACCEL_Y | ACCEL_Z);
    
    sensor_power(1); // on
    
    for (int j = 0; j < MAX_HISTORY; j++)
    {
      TAR = 0;
      while(TAR < 5000); // 1 count = 1 us
    
      sample_adc();
      
      process_sample(j);    
    }
    
    best_delta = 4096;
    // find best max/min
    for (int j = 0; j < MAX_HISTORY; j++)
    {
      delhistory[j] = maxhistory[j] - minhistory[j];
      best_delta = delhistory[j];
      delta_idx = j;
    }
    
    
    sensor_power(0); // off
    
    TAR = 0;
    while(TAR < 10000); // 1 count = 1 us
    TAR = 0;
    while(TAR < 10000);
    TAR = 0;
    while(TAR < 10000);
    TAR = 0;
    while(TAR < 10000);
    TAR = 0;
    while(TAR < 10000);

//    P4OUT ^= 0x04;                      // Toggle P4.2 LEDCtrl using exclusive-OR    
    _NOP();
  }
}

