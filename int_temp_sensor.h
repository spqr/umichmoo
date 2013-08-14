/* See license.txt for license information. */

#ifndef INT_TEMP_SENSOR_H
#define INT_TEMP_SENSOR_H

#define SENSOR_DATA_TYPE_ID       0x0F

void init_sensor();
void read_sensor(unsigned char volatile *target);
unsigned char is_sensor_sampling();

#endif // INT_TEMP_SENSOR_H
