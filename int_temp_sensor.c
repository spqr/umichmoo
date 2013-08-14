/* See license.txt for license information. */
#include "mymoo.h"
#if (ACTIVE_SENSOR == SENSOR_INTERNAL_TEMP)

#include "moo.h"
#include "rfid.h"
#include "int_temp_sensor.h"

// transfer function from user's guide says these are vals at 30C, 85C
// these are bounds for calibration
#define TF_TEMP_15_30 2982
#define TF_TEMP_15_85 3515
#define TLVStruct(x)  *(&(((int*)TLV_ADC12_1_TAG_)[x+1]))

unsigned char sensor_busy = 0;

void init_sensor() {
  return;
}

unsigned int get_calibrated_adc () {
  unsigned int tmptemp;
  int ADCoffset = 0;
  sensor_busy = 1;
  int TLV_TEMP_15_30;
  unsigned char busy;

  TLV_TEMP_15_30 = TLVStruct(CAL_ADC_15T30); // Retrieve from TLV structure
  ADC12CTL0 = SHT0_8 + REFON + ADC12ON;      // Internal ref = 1.5V
  ADC12CTL1 = SHP;                           // enable sample timer
  ADC12MCTL0 = SREF_1 + INCH_10;             // ADC i/p ch A10 = temp sense i/p
  ADC12CTL0 |= ADC12SC + ENC;                // start sampling
  while (ADC12CTL1 & ADC12BUSY);
  tmptemp = ADC12MEM0;
  if(TLV_TEMP_15_30 > TF_TEMP_15_30)
  {
    ADCoffset = TLV_TEMP_15_30 - TF_TEMP_15_30;
    tmptemp -= ADCoffset;
  }
  else
  {
    ADCoffset = TF_TEMP_15_30 - TLV_TEMP_15_30;
    tmptemp += ADCoffset;
  }

  ADC12CTL0 &= ~ENC;
  ADC12CTL1 = 0;       // turn adc off
  ADC12CTL0 = 0;       // turn adc off

  sensor_busy = 0;
  return tmptemp;
}

void read_sensor(unsigned char volatile *target) {
  unsigned int temp = get_calibrated_adc();

  *(target + 1) = (temp   & 0xff);
  *target       = (temp   & 0x0f00) >> 8;
  return;
}

unsigned char is_sensor_sampling() {
  return sensor_busy ? 1 : 0;
}

#endif // (ACTIVE_SENSOR == SENSOR_INTERNAL_TEMP)
