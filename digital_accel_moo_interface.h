#ifndef __DIGITAL_ACCEL_MOO_INTERFACE_
#define __DIGITAL_ACCEL_MOO_INTERFACE_

extern unsigned char sensor_busy;

void init_sensor();

void read_sensor(unsigned char volatile *);

#endif