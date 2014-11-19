#include "mymoo.h"
#include "sensor.h"

#include "spi_sensor.h"
#include "rfid.h" /* sensor_counter */
#include "moo.h"
#include <inttypes.h>

#define SLAVE_READY_PIN BIT5
#define SLAVE_SELECT_PIN BIT7

#define SET(var, bit) (var |= bit)
#define CLR(var, bit) (var &= ~bit)
#define BIT(var, bit) (var & bit)

static void spi_sensor_ready()     {CLR(P1OUT, SLAVE_READY_PIN);}
static void spi_sensor_not_ready() {SET(P1OUT, SLAVE_READY_PIN);}
static uint8_t spi_sensor_ss()     {return !(BIT(P1IN, SLAVE_SELECT_PIN));}
static uint8_t spi_sensor_data_ready() {return BIT(IFG2,UCB0RXIFG);}
static uint8_t spi_sensor_data()   {return UCB0RXBUF;}

static void spi_sensor_setup_pins();
static void spi_sensor_init();
static void spi_sensor_spi_enable();


static unsigned int count = 0;

static int init_sensor();
static void read_spi_sensor(unsigned char volatile * target, unsigned long read);

static const struct sensor spi_sensor = {
	.sensor_id = 0x16,
	.name      = "SPI",
	.init      = init_sensor,
	.read      = read_spi_sensor
};

sensor_init(spi_sensor);

static int init_sensor() {
	spi_sensor_setup_pins();
	spi_sensor_init();
	spi_sensor_spi_enable();
	spi_sensor_not_ready();
	count = 0;
	return 0;
}

#define MAX_DATA_READY 10

static void read_spi_sensor(unsigned char volatile * target, unsigned long len) {
	uint8_t old_IE2;
	target[0] = count++ & 0xFF;
	static int loop_count; /* Static because something was trashing stack*/
	
	old_IE2 = IE2;
	loop_count = 0;
	while (spi_sensor_ss() && loop_count < MAX_DATA_READY) { /* If slave select */
		CLR(IE2, UCB0RXIE);
		CLR(IE2, UCB0TXIE);
		spi_sensor_ready();
		if (spi_sensor_data_ready()) {
			target[0] = count++ & 0xFF;
			target[1] = spi_sensor_data();
			break;
		}
		loop_count++;
	}
	spi_sensor_not_ready();
	IE2 |= (UCB0RXIE & old_IE2);
	IE2 |= (UCB0TXIE & old_IE2);
}


/*
 * Selects pin modes for pins of the accelerometer, in case they were configured
 * as GPIO pins.
 *
 * This function by itself will not setup the SPI interface.
 */
static void spi_sensor_setup_pins() {
	/* Configure UCB0* Pins */
	P3SEL |= (BIT1 | BIT2 | BIT3); /* UCB0SIMO */ /* UCB0SOMI */ /* UCB0CLK  */
	/* End configure UCB0 Pins */
	
	/* Configure Slave select/slave ready pin */
	P1OUT |= SLAVE_READY_PIN;    /* And we are currently outputing 0 */
	P1DIR |= SLAVE_READY_PIN;    /* Make sure this is an output */
	P1SEL &= ~(SLAVE_READY_PIN); /* SPI slave ready */
	
	P1DIR &= ~(SLAVE_SELECT_PIN); /* SPI Slave select input */
	P1SEL &= ~(SLAVE_SELECT_PIN); /* SPI Slave select */
	/* End configure slave select/slave ready pin */
}

static void spi_sensor_init() {
	UCB0CTL1 |= UCSWRST; /* Reset hardwarese block */
	                     /* No need to select clock, slave device */
	
	UCB0CTL0 = 0;
	UCB0CTL0 |= (UCSYNC | UCMSB); /* SPI mode */ /* MSB first */
                                  /* 3 pin SPI (non standard chip select) */
	                              /* Slave mode */
	                              /* 8 bit data */

}

static void spi_sensor_spi_enable() {
	UCB0CTL1 &= ~(UCSWRST);
}