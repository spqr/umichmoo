#ifndef SENSOR_CONF_H
#define SENSOR_CONF_H

#include "mymoo.h"

#if ENABLE_SESSIONS
#include "sessions.h"
#endif

#include <inttypes.h>

struct sensor {
	uint8_t sensor_id;
	const char * name;
	int (*init)(void);
	void (*read)(unsigned char volatile * target, unsigned long size);
};

#pragma segment="SENSOR_INIT_I" __data16
#pragma segment="SENSOR_SPACE" __data16

#define __init_call     _Pragma("location=\"SENSOR_INIT_I\"")
#define __sensor_space  _Pragma("diag_suppress=Be033") _Pragma("location=\"SENSOR_SPACE\"")
#define __initcall(x) \
__init_call __root static const struct sensor * const __initstruct_##x  = &(x); \
__sensor_space __root static const struct sensor * __space_struct_##x;
#define sensor_init(x)  __initcall(x);

void init_sensors();
void read_sensor(unsigned char volatile * target, unsigned long read, uint8_t *sensor_id);
const struct sensor * getSensorByName(const char *,int);
const struct sensor * getSensorById(uint8_t,int);
const struct sensor ** getSensorIter();
const struct sensor **getSensorNext(const struct sensor **);

#define ALL_SENSORS 0
#define ACTIVE_SENSORS 1


#if SIMPLE_QUERY_ACK
#define ENABLE_READS                  0
#define READ_SENSOR                   0
#pragma message ("compiling simple query-ack application")
#endif
#if SENSOR_DATA_IN_ID
#define ENABLE_READS                  0
#define READ_SENSOR                   1
#pragma message ("compiling sensor data in id application")
#endif
#if SIMPLE_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   0
#pragma message ("compiling simple read command application")
#endif
#if SENSOR_DATA_IN_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   1
#pragma message ("compiling sensor data in read command application")
#endif

#endif //SENSOR_CONF_H
