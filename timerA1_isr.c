#include "moo.h"
#include "rfid.h"
#include "mymoo.h"
#include "compiler.h"

LOCK_REG(5, volatile unsigned short bits);
LOCK_REG(4, volatile unsigned char* dest);
extern unsigned short TRcal;

//*************************************************************************
//************************ Timer INTERRUPT *******************************

// Pin Setup :  P1.2
// Description :

ISR(TIMERA1_VECTOR, TimerA1_ISR)   // (6 cycles) to enter interrupt
{
    static unsigned int pivot_count;
    static unsigned int pivot;
    static unsigned int num_bits;
    unsigned int count = TACCR1;
    TAR       = 0;          // reset timer (4 cycles)
    TACCTL1   &= ~CCIFG;    // must manually clear interrupt flag (4 cycles)
    if (bits >= 3 || (bits == 2 && count < pivot_count)) {
        asm __volatile__ ("ADD    %[pivot], %[count]\n" //count = pivot + count
            "ADDC.b @R4+,     -1(R4)\n"   // (*dest) = (*dest - 1) + 
            : [count] "+r"  (count)
            : [pivot] "r" (pivot));
        num_bits++;
        bits++;
        if (num_bits < 8) {
            dest--;
        }
        num_bits &= ~(0x8); // Clear the 8th bit (make sure we don't go over 8)
    }
    else if (bits == 2) {
        TRcal = count;
        bits = 3;
        num_bits=0;
    }
    else if (bits == 1) {
        pivot_count = count;
        pivot = 0xFFFFu - pivot_count / 2;
        bits = 2;
    }
    else {
    // <-------------------- this is bit == 0 case --------------------->
        num_bits = 0;
        bits = 1;
    }
}
