#ifndef FLASH_H
#define FLASH_H

unsigned char read_extflash_uchar (unsigned long);

// Return 1 on successful write; 0 otherwise.
unsigned char write_extflash_uchar (unsigned long, unsigned char);

#endif // FLASH_H
