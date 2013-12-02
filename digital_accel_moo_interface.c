#include "mymoo.h"
#if (ACTIVE_SENSOR == SENSOR_DIGITAL_ACCEL)

#include "digital_accel_moo_interface.h"
#include "rfid.h" /* sensor_counter */

#define BUF_SIZE 6
void init_sensor() {
  digital_accel_setup_pins();
  digital_accel_init();
  digital_accel_power_on();
  digital_accel_set_filter(EDigitalAccelRange_4g, EDigitalAccelOdr_50Hz, 0);
  digital_accel_set_power(EDigitalAccelMode_Measurement,
                          EDigitalAccelLowNoise_normal,
                          0);
}

void read_sensor(unsigned char volatile *target) {
  int i;
  uint8_t buf[BUF_SIZE];


  digital_accel_set_filter(EDigitalAccelRange_4g, EDigitalAccelOdr_50Hz, 0);
  digital_accel_set_power(EDigitalAccelMode_Measurement,
                          EDigitalAccelLowNoise_normal,
                          0);
  
  if (!digital_accel_read_address(DIGITAL_ACCEL_REG_STATUS) & DIGITAL_ACCEL_DATA_READY_MASK) {
	  return;
  }
  
  digital_accel_read_burst(DIGITAL_ACCEL_REG_XDATA_L, buf, BUF_SIZE);

  /* We need to reverse the endianess of our data. X/Y/Z data is all two bytes,
   * stored big endian form Accel, but we need it little endian
   */
  for (i = 0; i < BUF_SIZE / 2; i++) {
    target[i*2] = buf[i*2 + 1];
    target[i*2 + 1] = buf[i*2];
  }
  /*digital_accel_power_off();*/

  sensor_counter++;
  target[7] = (sensor_counter & 0x00ff);
  target[6]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it
}

#endif
