#ifndef __SIMPLE_SENSOR_
#define __SIMPLE_SENSOR_

#define SENSOR_DATA_TYPE_ID       0x14

extern unsigned char sensor_busy;

void init_sensor();

void read_sensor(unsigned char volatile *);

#endif