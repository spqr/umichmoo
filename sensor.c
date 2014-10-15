#include "sensor.h"
#include "string.h"

static const struct sensor ** sg_sensor_space_end = 0;

void init_sensors() {
	int ret;
	const struct sensor ** s = (const struct sensor **) __segment_begin("SENSOR_INIT_I");
	const struct sensor ** end = (const struct sensor **) __segment_end("SENSOR_INIT_I");
	const struct sensor ** space_s = (const struct sensor **) __segment_begin("SENSOR_SPACE");
	const struct sensor ** space_end = (const struct sensor **) __segment_end("SENSOR_SPACE");
	const struct sensor ** space_ptr = space_s;
	while (space_ptr != space_end) {
		/* Make sure there are no valid sensors before calling init */
		*space_ptr++ = 0;
	}
	space_ptr = space_s;
	while (s != end) {
		ret = (*s)->init();
		if (ret == 0) {
			*space_ptr++ = *s;
		}
		sg_sensor_space_end = space_ptr;
		s++;
	}
	
}

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

const struct sensor * getSensorByName(const char *name, int active) {
	const struct sensor ** space_s;
	const struct sensor ** space_end;
	if (active) {
		space_s = (const struct sensor **) __segment_begin("SENSOR_SPACE");
		space_end = sg_sensor_space_end;
	}
	else {
		space_s = (const struct sensor **) __segment_begin("SENSOR_INIT_I");
		space_end = (const struct sensor **) __segment_end("SENSOR_INIT_I");
	}
		
		
	while (space_s != space_end) {
		if (strcmp((*space_s)->name, name) == 0) {
			return *space_s;
		}
		space_s++;
	}
	return 0;
}

const struct sensor * getSensorById(uint8_t id, int active) {
	const struct sensor ** space_s;
	const struct sensor ** space_end;
	if (active) {
		space_s = (const struct sensor **) __segment_begin("SENSOR_SPACE");
		space_end = sg_sensor_space_end;
	}
	else {
		space_s = (const struct sensor **) __segment_begin("SENSOR_INIT_I");
		space_end = (const struct sensor **) __segment_end("SENSOR_INIT_I");
	}
	
	while (space_s != space_end) {
		if ((*space_s)->sensor_id == id) {
			return *space_s;
		}
		space_s++;
	}
	return 0;
}

const struct sensor ** getSensorIter() {
	return (const struct sensor **) __segment_begin("SENSOR_SPACE");
}

const struct sensor **getSensorNext(const struct sensor ** sensorIter) {
	const struct sensor ** space_end = sg_sensor_space_end;
	const struct sensor ** space_s = (const struct sensor **) __segment_begin("SENSOR_SPACE");
	sensorIter++;
	if (sensorIter < space_s || sensorIter >= space_end) {
		return 0;
	}
	else {
		return sensorIter;
	}
}