#ifndef __DIGITAL_ACCEL_MOO_INTERFACE_
#define __DIGITAL_ACCEL_MOO_INTERFACE_

#include "digital_accel_sensor.h"

#define SENSOR_DATA_TYPE_ID       0x12

extern unsigned char sensor_busy;

void init_sensor();

void read_sensor(unsigned char volatile *);

#endif
