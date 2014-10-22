#include "sensor.h"
#include "string.h"
#include "compiler.h"

static const struct sensor ** sg_sensor_space_end = 0;

void init_sensors() {
	int ret;
	const struct sensor ** s = __sensor_init_begin;
	const struct sensor ** end = __sensor_init_end;
	const struct sensor ** space_s = __sensor_space_begin;
	const struct sensor ** space_end = __sensor_space_end;
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

const struct sensor * getSensorByName(const char *name, int active) {
	const struct sensor ** space_s;
	const struct sensor ** space_end;
	if (active) {
		space_s = __sensor_space_begin;
		space_end = sg_sensor_space_end;
	}
	else {
		space_s = __sensor_init_end;
		space_end = __sensor_init_end;
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
		space_s = __sensor_space_begin;
		space_end = sg_sensor_space_end;
	}
	else {
		space_s = __sensor_init_end;
		space_end = __sensor_init_end;
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
	return __sensor_space_begin;
}

const struct sensor **getSensorNext(const struct sensor ** sensorIter) {
	const struct sensor ** space_end = sg_sensor_space_end;
	const struct sensor ** space_s = __sensor_space_begin;
	sensorIter++;
	if (sensorIter < space_s || sensorIter >= space_end) {
		return 0;
	}
	else {
		return sensorIter;
	}
}
