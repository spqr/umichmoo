//******************************************************************************
//  UMass Moo Demo - Flash LED by toggling MSP430F2618/2619 P4.2
//
//  Description; Toggle P4.2 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x2xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P4.2|-->LED
//
//  H. Zhang
//  UMass, Amherst, SPQR Lab
//  October 2010
//  Built with IAR Embeded Workbench kickstart for MSP430 Version: 4.21
//
//  (adapted from MSP430x261x_1.c from TI's MSP430F261x code examples)
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
