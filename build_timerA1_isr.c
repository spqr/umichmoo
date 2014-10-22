#include "moo.h"
#include "rfid.h"
#include "mymoo.h"
#include "compiler.h"

LOCK_REG(5, volatile unsigned short bits);
extern unsigned short TRcal;

// This is needed to make the inline assembly compile properly w/ this symbol
void timera1_isr_decls()
{
  TACCR1;
}

//*************************************************************************
//************************ Timer INTERRUPT *******************************

// Pin Setup :  P1.2
// Description :

ISR(TIMERA1_VECTOR, TimerA1_ISR)   // (6 cycles) to enter interrupt
{

    asm("MOV 0x0174, R7");  // move TACCR1 to R7(count) register (3 CYCLES)
    TAR = 0;               // reset timer (4 cycles)
    TACCTL1 &= ~CCIFG;      // must manually clear interrupt flag (4 cycles)

    //<------up to here 26 cycles + 6 cyles of Interrupt == 32 cycles -------->
    asm("CMP #0003h, R5\n");      // if (bits >= 3).  it will do store bits
    asm("JGE bit_Is_Over_Three\n");
    // bit is not 3
    asm("CMP #0002h, R5\n");   // if ( bits == 2)
    asm("JEQ bit_Is_Two\n");         // if (bits == 2).

    // <----------------- bit is not 2 ------------------------------->
    asm("CMP #0001h, R5\n");      // if (bits == 1) -- measure RTcal value.
    asm("JEQ bit_Is_One\n");          // bits == 1

    // <-------------------- this is bit == 0 case --------------------->
    asm("bit_Is_Zero_In_Timer_Int:");
    asm("CLR R6\n");
    asm("INC R5\n");        // bits++
    asm("RETI");
    // <------------------- end of bit 0  --------------------------->

    // <-------------------- this is bit == 1 case --------------------->
    asm("bit_Is_One:\n");         // bits == 1.  calculate RTcal value
    asm("MOV R7, R9\n");       // 1 cycle
    asm("RRA R7\n");    // R7(count) is divided by 2.   1 cycle
    asm("MOV #0FFFFh, R8\n");   // R8(pivot) is set to max value    1 cycle
    asm("SUB R7, R8\n");        // R8(pivot) = R8(pivot) -R7(count/2) make new
                                // R8(pivot) value     1 cycle
    asm("INC R5\n");        // bits++
    asm("CLR R6\n");
    asm("RETI\n");
    // <------------------ end of bit 1 ------------------------------>

    // <-------------------- this is bit == 2 case --------------------->
    asm("bit_Is_Two:\n");
    asm("CMP R9, R7\n");    // if (count > (R9)(180)) this is hardcoded number,
                            // so have  to change to proper value
    asm("JGE this_Is_TRcal\n");
    // this is data
    asm("this_Is_Data_Bit:\n");
    asm("ADD R8, R7\n");   // count = count + pivot
    // store bit by shifting carry flag into cmd[bits]=(dest*) and increment
    // dest*  (5 cycles)
    asm("ADDC.b @R4+,-1(R4)\n"); // roll left (emulated by adding to itself ==
                                 // multiply by 2 + carry)
    // R6 lets us know when we have 8 bits, at which point we INC dest* (1
    // cycle)
    asm("INC R6\n");
    asm("CMP #0008,R6\n\n");   // undo increment of dest* (R4) until we have 8
                               // bits
    asm("JGE out_p\n");
    asm("DEC R4\n");
    asm("out_p:\n");           // decrement R4 if we haven't gotten 16 bits yet
                               // (3 or 4 cycles)
    asm("BIC #0008h,R6\n");   // when R6=8, this will set R6=0   (1 cycle)
    asm("INC R5\n");
    asm("RETI");
    // <------------------ end of bit 2 ------------------------------>

    asm("this_Is_TRcal:\n");
    asm("MOV R7, R5\n");    // bits = count. use bits(R5) to assign new value of
                            // TRcal
    TRcal = bits;       // assign new value     (4 cycles)
    asm("MOV #0003h, R5\n");      // bits = 3..assign 3 to bits, so it will keep
                                  // track of current bits    (2 cycles)
    asm("CLR R6\n"); // (1 cycle)
    asm("RETI");

   // <------------- this is bits >= 3 case ----------------------->
    asm("bit_Is_Over_Three:\n");     // bits >= 3 , so store bits
    asm("ADD R8, R7\n");    // R7(count) = R8(pivot) + R7(count),
    // store bit by shifting carry flag into cmd[bits]=(dest*) and increment
    // dest* (5 cycles)
    asm("ADDC.b @R4+,-1(R4)\n"); // roll left (emulated by adding to itself ==
                                 // multiply by 2 + carry)
    // R6 lets us know when we have 8 bits, at which point we INC dest* (1
    // cycle)
    asm("INC R6\n");
    asm("CMP #0008,R6\n");   // undo increment of dest* (R4) until we have 8
                             // bits
    asm("JGE out_p1\n");
    asm("DEC R4\n");
    asm("out_p1:\n");           // decrement R4 if we haven't gotten 16 bits yet
                                // (3 or 4 cycles)
    asm("BIC #0008h,R6\n");   // when R6=8, this will set R6=0   (1 cycle)
    asm("INC R5\n");              // bits++
    asm("RETI\n");
    // <------------------ end of bit is over 3 ------------------------------>
}
