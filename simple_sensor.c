#include "mymoo.h"
#include "simple_sensor.h"
#include "rfid.h" /* sensor_counter */
#include "moo.h"
#include "sensor.h"

static unsigned int count = 0;

static int init_sensor();
static void read_simple_sensor(unsigned char volatile * target, unsigned long len);

static const struct sensor simple_sensor = {
	.sensor_id = 0x14,
	.name      = "Simple",
	.init      = init_sensor,
	.read      = read_simple_sensor
};

sensor_init(simple_sensor);

static int init_sensor() {
	count = 0;
	return 0;
}

static void read_simple_sensor(unsigned char volatile * target, unsigned long len) {
	target[0] = count++ & 0xFF;
}