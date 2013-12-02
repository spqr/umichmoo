#include "mymoo.h"
#include "sensor.h"
#include "rfid.h"

int main(void)
{
	uint8_t buf[8], buf2[8];
	volatile uint8_t i = 1;
	volatile uint8_t j = 0xf0;
	init_sensor();
	
	digital_accel_power_on();
  	digital_accel_write_address(DIGITAL_ACCEL_REG_SOFT_RESET, 0x52);
  	digital_accel_set_filter(EDigitalAccelRange_4g, EDigitalAccelOdr_50Hz, 0);
  	digital_accel_set_power(EDigitalAccelMode_Measurement,
                          EDigitalAccelLowNoise_normal,
                          0);
	digital_accel_read_burst(DIGITAL_ACCEL_REG_DEVID_AD, buf, 3);
	
	if (buf[0] != 0xAD) {
		while (1) ;
	}
	
	j = digital_accel_read_address(DIGITAL_ACCEL_REG_DEVID_MST);
	
	if (buf[1] != 0x1D) {
		while (1) ;
	}
	
	if (buf[2] != 0xF2) {
		while (1) ;
	}
	
	j = digital_accel_read_address(DIGITAL_ACCEL_REG_POWER_CTL);
	
	if (j != 0x02) {
		while (1) ;
	}
	
	
	while (1) {
		read_sensor(buf);
	}
	
	return 0;
}
