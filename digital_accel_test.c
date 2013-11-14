#include "mymoo.h"
#include "sensor.h"
#include "rfid.h"

int main(void)
{
	uint8_t buf[8];
	init_sensor();
	
	while (1) {
		read_sensor(buf);
	}
	
	return 0;
}
