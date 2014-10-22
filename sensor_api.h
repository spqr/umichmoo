#ifndef __SENSOR_API_H_
#define __SENSOR_API_H_

#include <inttypes.h>
#include "compiler.h"

/**
 * Sensor API - Describes how to create a new sensor to be added to the moo.
 *
 * The sensor API integration with the moo code is based heavily on linux kernel
 * modules. The code for a sensor can be defined in one .c file, and no
 * modifications to other files are needed to get the sensor working.
 *
 * ENABLING/DISABLING SENSORS:
 * Enabling/disabling sensors is easy, just include the sensor's .c in the build
 * to enable the sensor, and exclude the sensor's .c file to disable the sensor.
 * In IAR, this is done by right clicking on the file name and selecting
 * "Exclude from build" in the window.
 * Long story short, if the sensor's .c file is linked as part of the final
 * executable, it will be enabled (unless init() fails, see init() below).
 *
 * MAKING YOUR OWN SENSOR:
 * The first thing you will need is to defined a struct of type struct sensor.
 * struct sensor is defined below, and all members of the struct are explained.
 * The struct should be static, and should probably be const, but none of these
 * things are required.
 * Once you've defined the struct, you need to call sensor_init(struct_name).
 * sensor_init() is a macro that will register the struct with the moo's sensor
 * library. If you do not call sensor_init(), then you sensors functions will
 * not be called.
 *
 * READ_SENSOR():
 * The read_sensor() function is defined sensor_read.c. By default, it just
 * round robins through all the sensors, but can be rewritten by the users,
 * using the functions below, to whatever the users wants.
 */

struct sensor {
	/* Sensor ID. This should be unique, however we do not check it's unique,
	 * and nothing will fail if it's not unique. This is not used for anything
	 * internally. */
	uint8_t sensor_id;
	/* Sensor human readable name. */
	const char * name;
	/**
	 * Initialization function. This function will be called once at startup. If
	 * the function returns a non-zero value, this sensor will not be considered
	 * active, as this signals to the library that the sensor failed to init.
	 */
	int (*init)(void);
	/**
	 * Read function. This reads from the sensor, and populates the target
	 * buffer up to size size. By default, this is called once per sensor in a
	 * round robin function, but the user can rewrite the read_sensor() function
	 * to make it call the read() functions for whatever arbitrary sensors it
	 * wants.
	 */
	void (*read)(unsigned char volatile * target, unsigned long size);
};

#define sensor_init(x)  __initcall(x);

#define ALL_SENSORS 0
#define ACTIVE_SENSORS 1
void read_sensor(unsigned char volatile * target, unsigned long read, uint8_t *sensor_id);

/**
 * Returns a pointer to a sensor identified by a name (name). Or, returns NULL
 * if no such sensor can be found. You can limit your search to just the active
 * sensors by setting active=ACTIVE_SENSORS, or all sensors by setting
 * active=ALL_SENSORS.
 */
const struct sensor * getSensorByName(const char * name,int active);

/**
 * Returns a pointer to a sensor identified by a id (id). Or, returns NULL
 * if no such sensor can be found. You can limit your search to just the active
 * sensors by setting active=ACTIVE_SENSORS, or all sensors by setting
 * active=ALL_SENSORS.
 */
const struct sensor * getSensorById(uint8_t id, int active);

/**
 * Returns a pointer to a pointer to a sensor, or null if there are no active
 * sensors. (Again, an active sensor is a sensors for which the "init()"
 * function did not fail.) You can get the next sensor by calling
 * getSensorNext(const struct sensor ** my_iter).
 */
const struct sensor ** getSensorIter();
const struct sensor **getSensorNext(const struct sensor **);

#endif
