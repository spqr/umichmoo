/* See license.txt for license information. */

/* Moo power information. */

#ifndef __DIGITAL_ACCEL_SENSOR_
#define __DIGITAL_ACCEL_SENSOR_
#include "board.h"
#include "inttypes.h"

/*
 * ADXL 362 SPI commands, should be sent as first byte of SPI command
 */
#define DIGITAL_ACCEL_WRITE       ((uint8_t) 0x0A)
#define DIGITAL_ACCEL_READ        ((uint8_t) 0x0B)
#define DIGITAL_ACCEL_READ_FIFO   ((uint8_t) 0x0D) // @TODO: Support this!
/* MSP430 requires data to be written to receive data, so we use dummy byte */
#define DIGITAL_ACCEL_DUMMY       ((uint8_t) 0xFF)

/*
 * ADXL 362 addresses, sent as second byte of SPI command
 */
#define  DIGITAL_ACCEL_REG_DEVID_AD        ((uint8_t)  0x00)
#define  DIGITAL_ACCEL_REG_DEVID_MST       ((uint8_t)  0x01)
#define  DIGITAL_ACCEL_REG_PARTID          ((uint8_t)  0x02)
#define  DIGITAL_ACCEL_REG_REVID           ((uint8_t)  0x03)
#define  DIGITAL_ACCEL_REG_XDATA           ((uint8_t)  0x08)
#define  DIGITAL_ACCEL_REG_YDATA           ((uint8_t)  0x09)
#define  DIGITAL_ACCEL_REG_ZDATA           ((uint8_t)  0x0A)
#define  DIGITAL_ACCEL_REG_STATUS          ((uint8_t)  0x0B)
#define  DIGITAL_ACCEL_REG_FIFO_ENTRIES_L  ((uint8_t)  0x0C)
#define  DIGITAL_ACCEL_REG_FIFO_ENTRIES_H  ((uint8_t)  0x0D)
#define  DIGITAL_ACCEL_REG_XDATA_L         ((uint8_t)  0x0E)
#define  DIGITAL_ACCEL_REG_XDATA_H         ((uint8_t)  0x0F)
#define  DIGITAL_ACCEL_REG_YDATA_L         ((uint8_t)  0x10)
#define  DIGITAL_ACCEL_REG_YDATA_H         ((uint8_t)  0x11)
#define  DIGITAL_ACCEL_REG_ZDATA_L         ((uint8_t)  0x12)
#define  DIGITAL_ACCEL_REG_ZDATA_H         ((uint8_t)  0x13)
#define  DIGITAL_ACCEL_REG_TEMP_L          ((uint8_t)  0x14)
#define  DIGITAL_ACCEL_REG_TEMP_H          ((uint8_t)  0x14)
#define  DIGITAL_ACCEL_REG_SOFT_RESET      ((uint8_t)  0x1F)
#define  DIGITAL_ACCEL_REG_THRESH_ACT_L    ((uint8_t)  0x20)
#define  DIGITAL_ACCEL_REG_THRESH_ACT_H    ((uint8_t)  0x21)
#define  DIGITAL_ACCEL_REG_TIME_ACT        ((uint8_t)  0x22)
#define  DIGITAL_ACCEL_REG_THRESH_INACT_L  ((uint8_t)  0x23)
#define  DIGITAL_ACCEL_REG_THRESH_INACT_H  ((uint8_t)  0x24)
#define  DIGITAL_ACCEL_REG_TIME_INACT_L    ((uint8_t)  0x25)
#define  DIGITAL_ACCEL_REG_TIME_INACT_H    ((uint8_t)  0x26)
#define  DIGITAL_ACCEL_REG_ACT_INACT_CTL   ((uint8_t)  0x27)
#define  DIGITAL_ACCEL_REG_FIFO_CONTROL    ((uint8_t)  0x28)
#define  DIGITAL_ACCEL_REG_FIFO_SAMPLES    ((uint8_t)  0x29)
#define  DIGITAL_ACCEL_REG_INTMAP1         ((uint8_t)  0x2A)
#define  DIGITAL_ACCEL_REG_INTMAP2         ((uint8_t)  0x2B)
#define  DIGITAL_ACCEL_REG_FILTER_CTL      ((uint8_t)  0x2C)
#define  DIGITAL_ACCEL_REG_POWER_CTL       ((uint8_t)  0x2D)
#define  DIGITAL_ACCEL_REG_SELF_TEST       ((uint8_t)  0x2E)
	 
#define  DIGITAL_ACCEL_DATA_READY_MASK     0x01

/*
 * Data for configuring the ADXL 362 filter register
 * DIGITAL_ACCEL_REG_FILTER_CTL
 */
enum EDigitalAccelRange {
  /*
   * Sets the range for the Digital Accelerometer. The range is set between
   * +-2g's, +-4g, or +-8g
   */
  EDigitalAccelRange_2g = 0x0,
  EDigitalAccelRange_4g = 0x1,
  EDigitalAccelRange_8g = 0x2,
};

enum EDigitalAccelOdr {
  /*
   * Sets the frequency that we update the acclerometer information.
   */
  EDigitalAccelOdr_12Hz = 0x0,
  EDigitalAccelOdr_25Hz = 0x1,
  EDigitalAccelOdr_50Hz = 0x2,
  EDigitalAccelOdr_100Hz = 0x3,
  EDigitalAccelOdr_200Hz = 0x4,
  EDigtal_AccelOdr_400Hz = 0x4,
};

#define DIGITAL_ACCEL_FILT_HALF_BW 16
#define DIGITAL_ACCEL_FILT_EXT_SAMPLE 8

/* END DIGITAL_ACCEL_REG_FILTER_CTL */

/*
 * Data for controlling the ADXL 362 power register
 */
enum EDigitalAccelMode {
  /*
   * Standby mode is the default mode. It's low power and the accelerometer
   * neither measures or send measurements.
   * Measurement mode is used to actually measure accelerometer data.
   */
  EDigitalAccelMode_Standby      =  0x0,
  EDigitalAccelMode_Measurement  =  0x2
};

enum EDigitalAccelLowNoise {
  /*
   * How much noise do you want to remove from the signal? This setting, using
   * more power, removes noise from the reading
   */
  EDigitalAccelLowNoise_normal    =  0x0,
  EDigitalAccelLowNoise_low       =  0x1,
  EDigitalAccelLowNoise_ultralow  =  0x2,
};

enum EDigitalAccelInt {
	/*
	 * Select between interrupt 1 and interrupt 2
	 */
	EDigitalAccelInt_Int1        = 0x1,
	EDigitalAccelInt_Int2        = 0x2,
};

#define  DIGITAL_ACCEL_INT_SOURCE_NONE             (0x0)
#define  DIGITAL_ACCEL_INT_SOURCE_DATA_READY       (0x1 << 0)
#define  DIGITAL_ACCEL_INT_SOURCE_FIFO_READY       (0x1 << 1)
#define  DIGITAL_ACCEL_INT_SOURCE_FIFO_WATERMARK   (0x1 << 2)
#define  DIGITAL_ACCEL_INT_SOURCE_FIFO_OVERRUN     (0x1 << 3)
#define  DIGITAL_ACCEL_INT_SOURCE_ACT              (0x1 << 4)
#define  DIGITAL_ACCEL_INT_SOURCE_INACT            (0x1 << 5)
#define  DIGITAL_ACCEL_INT_SOURCE_AWAKE            (0x1 << 6)
#define  DIGITAL_ACCEL_INT_SOURCE_INT_LOW          (0x1 << 7)

#define  DIGITAL_ACCEL_POWER_AUTOSLEEP  4
#define  DIGITAL_ACCEL_POWER_WAKEUP     8
#define  DIGITAL_ACCEL_POWER_EXT_CLK   64

/*
 * Recommended USCI Initialization and Reset (from page 438 of MSP430x2xx Family
 * Guide)
 *   1. Set UCSWRST (BIS.B #UCSWRST,&UCxCTL1) (Done on PUC)
 *   2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1) (Done on PUC)
 *   3. Configure ports (Can really start here if just initializing, not resetting)
 *   4. Clear UCSWRST via software (BIC.B, #UCSWRST, &UCxCTL1)
 *   5. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
 */

/*
 * Initialization/power functions.
 */
/*
 * Turns on the accelerometer.
 *
 * Expects pin 1.4 to be setup properly already (GPIO mode selected, direction
 * set to out). If it is not properly configured, then call
 * digital_accel_setup_pins() first.
 */
void digital_accel_power_on();

/*
 * Turns off the accelerometer.
 */
void digital_accel_power_off();

/*
 * Initializes the SPI interface for the accelerometer.
 */
void digital_accel_init();

/*
 * Selects pin modes for pins of the accelerometer, in case they were configured
 * as GPIO pins.
 *
 * This function by itself will not setup the SPI interface.
 */
void digital_accel_setup_pins();


/*
 * Set the power register to the values that are appropriate
 */
void digital_accel_set_power(enum EDigitalAccelMode, enum EDigitalAccelLowNoise, uint8_t flags);

/*
 * Set the filter register to values that are appropriate
 */
void digital_accel_set_filter(enum EDigitalAccelRange range, enum EDigitalAccelOdr odr, uint8_t flags); 
void digital_accel_write_address(uint8_t address, uint8_t byte); 
uint8_t digital_accel_read_address(uint8_t address); 
void digital_accel_read_burst(uint8_t start_address, uint8_t * data, uint8_t len); 
void digital_accel_write_burst(uint8_t start_address, uint8_t * data, uint8_t len); 

/* For SPI interrupt based transactions */
void digital_accel_spi_start(uint8_t op, uint8_t address, uint8_t * data, uint16_t len);
uint8_t digital_accel_spi_in_use();
uint8_t digital_accel_spi_complete();
void digital_accel_spi_buf_mark_read();

#endif
