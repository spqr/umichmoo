/* See license.txt for license information. */

// SENSOR_DATA_TYPE_ID, DATA_LENGTH_IN_WORDS, and DATA_LENGTH_IN_BYTES
// will now be coded as globals for runtime configurability.
extern unsigned int SENSOR_DATA_TYPE_ID;
extern unsigned int DATA_LENGTH_IN_WORDS;
extern unsigned int DATA_LENGTH_IN_BYTES;

extern unsigned char sensor_busy;

// init_sensor has been expanded to one parameter. This parameter will
// adjust the above globals to the specification provided by sensor_data_type_id.
// Allowable ids: 0x0B for accel, 0x0E for external temp.
void init_sensor(unsigned int sensor_data_type_id);

void read_sensor(unsigned char volatile *);