/* See license.txt for license information. */

#ifndef FLASH_H
#define FLASH_H

// pin setup for external flash wired to SPI bus
#define EXTFLASH_PORT_OUT P5OUT
#define EXTFLASH_PORT_DIR P5DIR
#define EXTFLASH_PORT_SEL P5SEL
#define EXTFLASH_CHIP_ENABLE_PIN          0x01 // pin 0 wired to flash's CE# pin
#define EXTFLASH_SPI_DATA_OUT_PIN         0x02 // pin 1 wired to SPI data out
#define EXTFLASH_SPI_DATA_IN_PIN          0x04 // pin 2 wired to SPI data in
#define EXTFLASH_SPI_SERIAL_CLOCK_OUT_PIN 0x08 // pin 3 wired to SPI clock

// flash commands; see page 11 of serial flash datasheet
#define EXTFLASH_BYTE_PROGRAM_COMMAND 0x02
#define EXTFLASH_BYTE_READ_COMMAND 0x03
#define EXTFLASH_WRITE_ENABLE_COMMAND 0x06
#define EXTFLASH_ERASE_SEGMENT_COMMAND 0x20

extern unsigned char spi_iodata;

unsigned char extflash_read_uchar (unsigned long);
unsigned char extflash_write_uchar (unsigned long, unsigned char);
void extflash_enable_chip (void);
void extflash_disable_chip (void);
void extflash_send_byte (unsigned char);
void extflash_erase_segment (unsigned long);

#endif // FLASH_H
