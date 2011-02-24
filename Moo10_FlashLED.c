//******************************************************************************
//  MSP430x2xx Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x2xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  A. Dannenberg
//  Texas Instruments, Inc
//  January 2006
//  Built with CCE for MSP430 Version: 3.0
//******************************************************************************

#include "msp430x26x.h"

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer
  P4DIR |= 0x04;                        // Set P4.2 to output direction

  for (;;)
  {
    volatile unsigned int i;            // volatile to prevent optimization

    P4OUT ^= 0x04;                      // Toggle P4.2 using exclusive-OR

    i = 50000;                          // SW Delay
    do i--;
    while (i != 0);
  }
}
