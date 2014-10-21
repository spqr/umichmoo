#include "sensor_api.h"

void read_sensor(unsigned char volatile * target, unsigned long read, uint8_t * id) {
	static const struct sensor ** cur_sensor_iter = 0;
	if (cur_sensor_iter == 0) {
		cur_sensor_iter = getSensorIter();
	}
	
	(*cur_sensor_iter)->read(target, read);
	if (id) {
		(*id) = (*cur_sensor_iter)->sensor_id;
	}
	cur_sensor_iter = getSensorNext(cur_sensor_iter);
}
