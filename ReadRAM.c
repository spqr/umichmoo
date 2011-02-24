//******************************************************************************
//   MSP430x26x Demo - DMA0, Repeated Burst to-from RAM, Software Trigger
//
//   Description: A 16 word block from 1400-141fh is transfered to 1420h-143fh
//   using DMA0 in a burst block using software DMAREQ trigger.
//   After each transfer, source, destination and DMA size are
//   reset to inital software setting because DMA transfer mode 5 is used.
//   P1.0 is toggled during DMA transfer only for demonstration purposes.
//   ** RAM location 0x1400 - 0x143f used - make sure no compiler conflict **
//   ACLK = 32kHz, MCLK = SMCLK = default DCO 1.045MHz
//
//                MSP430F261x
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//   B. Nisarga
//   Texas Instruments Inc.
//   September 2007
//   Built with CCE Version: 3.2.0
//******************************************************************************
#include "msp430x26x.h"

void main(void)
{
  volatile unsigned int i;
  
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

//  FLL_CTL0 |= XCAP14PF;                     // Configure xtal load capacitance

  // LFXT1 startup delay
  do
  {
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);           // Time for flag to set
  }
  while (IFG1 & OFIFG);                     // OSCFault flag still set?
  __disable_interrupt();                    // Disable interrupts
  

  // Setup Timer_B for DMA to FLASH
  TBCTL = TBSSEL_2;                         // Use SMCLK as Timer_B source
  TBR = 0;
  TBCCR0 = 100;                             // Initialize TBCCR0
  TBCCR1 = 50;                              // Initialize TBCCR0
  TBCCTL1 = OUTMOD_7;                       // Reset OUT1 on EQU1, set on EQU0

  // set up DMA
   __data16_write_addr((unsigned short)&DMA0SA, 0x1100);
                                            // Start block address
   __data16_write_addr((unsigned short)&DMA0DA, 0x8000); // 0x10000ul
                                            // Destination block address
  DMA0SZ = 0x0020;                          // Block size
  DMA0CTL = DMADT_0 + DMASRCINCR_3 + DMADSTINCR_3 + DMAEN + DMAIE; 
                                            // single xfer, inc s & d, 
                                            // enable DMA & interrupt
  DMACTL0 = DMA0TSEL_8;                     // TBCCR0 triggers DMA0

  // Setup and erase Flash memory
  // (Rem.: This time is also used to wait for
  //        the voltages getting stabilized)
  FCTL2 = FWKEY + FSSEL1 + FN1;        // SMCLK/3 = ~333kHz
  FCTL3 = FWKEY;                       // Unlock Flash memory for write
  
  FCTL1 = FWKEY + ERASE;
  __data20_write_char(0x8000, 0x00); // Dummy write activates segment erase
   
  FCTL1 = FWKEY + WRT;                  // Enable Flash write for copying
   
  TBCTL |= MC0;                           // Start Timer_B in UP mode
                                          // (counts up to TBCL0)

  // Activate LPM during DMA transfer, wake-up when finished
  __bis_SR_register(LPM0_bits + GIE);     // Enable interrupts, enter LPM0
  __disable_interrupt();                  // Disable interrupts

  // Deactivate Flash memory write access
  FCTL1 = FWKEY;                       // Disable Flash write
  FCTL3 = FWKEY + LOCK;                // Lock Flash memory

  TBCTL = 0;                           // Disable Timer_B
}

//------------------------------------------------------------------------------
// DMA interrupt handler
//------------------------------------------------------------------------------
#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void)
{
  DMA0CTL &= ~DMAIFG;                       // Clear DMA0 interrupt flag
  __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0 on reti
}
//------------------------------------------------------------------------------


