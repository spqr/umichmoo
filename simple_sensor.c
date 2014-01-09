#include "mymoo.h"
#if (ACTIVE_SENSOR == SENSOR_SIMPLE)

#include "simple_sensor.h"
#include "rfid.h" /* sensor_counter */
#include "moo.h"


static unsigned int count = 0;

void init_sensor() {
	count = 0;
}

void read_sensor(unsigned char volatile * target) {
	target[0] = count++ & 0xFF;
}

#endif