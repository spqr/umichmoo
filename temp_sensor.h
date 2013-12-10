/* See license.txt for license information. */
#ifndef __TEMP_SENSOR_H_
#define __TEMP_SENSOR_H_
#define SENSOR_DATA_TYPE_ID       0x0E

#define DATA_LENGTH_IN_WORDS      1
#define DATA_LENGTH_IN_BYTES      (DATA_LENGTH_IN_WORDS*2)

extern unsigned char sensor_busy;

void init_sensor();

void read_sensor(unsigned char volatile *);
#endif