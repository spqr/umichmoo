/* See license.txt for license information. */
#define SENSOR_DATA_TYPE_ID       0x1B

#define DATA_LENGTH_IN_WORDS      1
#define DATA_LENGTH_IN_BYTES      (DATA_LENGTH_IN_WORDS*2)

extern unsigned char sensor_busy;

void init_sensor();

void read_sensor(unsigned char volatile *);