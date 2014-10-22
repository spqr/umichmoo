#include "moo.h"
#include "rfid.h"
#include "mymoo.h"
#include "compiler.h"

// This is needed to make the inline assembly compile properly w/ these symbols
void port1_isr_decls()
{
  P1IE;
  TACCTL1;
  P1SEL;
  P1IES;
}

//*************************************************************************
//************************ PORT 1 INTERRUPT *******************************

// warning   :  Whenever the clock frequency changes, the value of TAR should be
//              changed in aesterick lines
// Pin Setup :  P1.2
// Description : Port 1 interrupt is used as finding delimeter.

ISR(PORT1_VECTOR, Port1_ISR)   // (5-6 cycles) to enter interrupt
{
  asm("MOV 0x0170, R7");  // move TAR to R7(count) register (3 CYCLES)
  P1IFG = 0x00;       // 4 cycles
  TAR = 0;            // 4 cycles
  LPM4_EXIT;

  asm("CMP #0000h, R5\n");          // if (bits == 0) (1 cycle)
  asm("JEQ bit_Is_Zero_In_Port_Int\n");                // 2 cycles
  // bits != 0:
  asm("MOV #0000h, R5\n");          // bits = 0  (1 cycles)

  asm("CMP #0010h, R7\n");          // finding delimeter (12.5us, 2 cycles)
                                    // 2d -> 14
  asm("JNC delimiter_Value_Is_wrong\n");            //(2 cycles)
  asm("CMP #0040h, R7");            // finding delimeter (12.5us, 2 cycles)
                                    // 43H
  asm("JC  delimiter_Value_Is_wrong\n");
  asm("CLR 0x0025");
  asm("BIS #8010h, 0x0164\n");     // (5 cycles)   TACCTL1 |= CM1 + CCIE
  asm("MOV #0004h, 0x0026\n");       // enable TimerA1    (4 cycles)
  asm("RETI\n");

  asm("delimiter_Value_Is_wrong:\n");
  asm("BIC #0004h, 0x0024\n");
  asm("MOV #0000h, R5\n");          // bits = 0  (1 cycles)
  delimiterNotFound = 1;
  asm("RETI");

  asm("bit_Is_Zero_In_Port_Int:\n");                 // bits == 0
  asm("MOV #0000h, 0x0170\n");     // reset timer (4 cycles)
  asm("BIS #0004h, 0x0024\n");   // 4 cycles  change port interrupt edge to neg
  asm("INC R5\n");            // 1 cycle
  asm("RETI\n");
}
