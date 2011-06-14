//******************************************************************************
//  UMass Moo Demo - Flash In-System Programming, Copy SegC to SegD
//
//  Description: This program first erases flash seg C, then it increments all
//  values in seg C, then it erases seg D, then copies seg C to seg D. Starting
//  addresses of segments defined in this code: Seg C-0x1100, Seg D-0x8000.
//  ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//  //* Set Breakpoint on NOP in the Mainloop to avoid Stressing Flash *//
//
//            MSP430F261x/241x
//            -----------------
//        /|\|              XIN|-
//         | |                 |
//         --|RST          XOUT|-
//           |                 |
//
//  H. Zhang
//  UMass, Amherst, SPQR Lab
//  February 2011
//  Built with IAR Embeded Workbench kickstart for MSP430 Version: 4.21
//
//  (adapted from MSP430x261x_flashwrite_01.c from TI's MSP430F261x code
//   examples)
//******************************************************************************
#include "msp430x26x.h"

// Function prototypes
void copy_RAM2FLASH(void);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1);                               // If calibration constants erased
                                            // do not load, trap CPU!!
  }  
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ; 
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

  while(1)                                  // Repeat forever
  {
    copy_RAM2FLASH();                       // Copy segment C to D
    __no_operation();                       // SET BREAKPOINT HERE
  }
}

void copy_RAM2FLASH(void)
{
  char *Flash_ptrC;                         // Segment C pointer
  char *Flash_ptrD;                         // Segment D pointer
  unsigned int i;

  Flash_ptrC = (char *)0x1100;              // Initialize Flash segment C ptr
  Flash_ptrD = (char *)0x8000;              // Initialize Flash segment D ptr
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptrD = 0;                          // Dummy write to erase Flash seg D
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i = 0; i < 64; i++)
  {
    *Flash_ptrD++ = *Flash_ptrC++;          // copy value segment C to seg D
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
