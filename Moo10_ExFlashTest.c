#include "msp430x26x.h"
#include "ExternalFlashDriver.h"

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer

  P5DIR |= 0x0B;      // Set output for 5.0:CE#, 5.1:SIMO, 5.3:SCK
  P5OUT &= ~CE;
  
  init();
  
  for (;;)
  {
    volatile unsigned int i;            // volatile to prevent optimization  
    WREN();
    Byte_Program(0x123456, 0x79);
    WRDI();
    Read_Status_Register();
    Read(0x123456);

    i = 10;                          // SW Delay
    do i--;
    while (i != 0);
  }
}
