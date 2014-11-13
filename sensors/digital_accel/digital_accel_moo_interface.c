#include "mymoo.h"
#include "sensor.h"

#include "digital_accel_moo_interface.h"
#include "rfid.h" /* sensor_counter */
#include "moo.h"

#if MOO_VERSION != MOO1_2 && MOO_VERSION != MOO1_2_1
#error "Unsupported moo version. Must be version 1.2"
#endif


#define BUF_SIZE 6

static uint8_t buf[BUF_SIZE];

static int init_sensor();
static void read_accel_sensor(unsigned char volatile *target, unsigned long len);

static const struct sensor da_sensor = {
	.sensor_id = 0x12,
	.name      = "Digital_Accel",
	.init      = init_sensor,
	.read      = read_accel_sensor
};

sensor_init(da_sensor);

static int init_sensor() {
	digital_accel_setup_pins();
	digital_accel_init();
	digital_accel_power_on();
	/* Need to wait 5ms to power on. */
	__delay_cycles(5*3000);
	digital_accel_set_filter(EDigitalAccelRange_4g, EDigitalAccelOdr_50Hz, 0);
	digital_accel_set_power(EDigitalAccelMode_Measurement,
                            EDigitalAccelLowNoise_low,
                            0);
	digital_accel_set_interrupt(EDigitalAccelInt_Int1, DIGITAL_ACCEL_INT_SOURCE_DATA_READY);
	return 0;
}

static void read_accel_sensor(unsigned char volatile *target, unsigned long len) {
	int i;
	
	/* We need to reverse the endianess of our data. X/Y/Z data is all two bytes,
	 * stored big endian form Accel, but we need it little endian
	 */
	if (digital_accel_spi_complete()) {
		digital_accel_spi_buf_mark_read();
		sensor_counter++;
  	}
	for (i = 0; i < BUF_SIZE / 2; i++) {
		target[i*2] = buf[i*2 + 1];
		target[i*2 + 1] = buf[i*2];
	}
	target[7] = (sensor_counter & 0x00ff);
  	target[6]  = (sensor_counter & 0xff00) >> 8; // grab msb bits and store it
	
	if (!digital_accel_spi_in_use() && digital_accel_has_data()) {
		digital_accel_spi_start(DIGITAL_ACCEL_READ, DIGITAL_ACCEL_REG_XDATA_L, buf, BUF_SIZE);
	}
}
