#include "moo.h"
#include "rfid.h"
#include "mymoo.h"
#include "compiler.h"

LOCK_REG(4, volatile unsigned char* dest);
LOCK_REG(5, unsigned short bits);

//
//
// experimental M4 code
//
//

/******************************************************************************
*   Pin Set up
*   P1.1 - communication output
*******************************************************************************/
void sendToReader(volatile unsigned char *data, unsigned char numOfBits)
{
  SEND_CLOCK;

  TACTL &= ~TAIE;
  TAR = 0;
  // assign data address to dest
  dest = data;
  // Setup timer
  P1SEL |= TX_PIN; //  select TIMER_A0
  TACTL |= TACLR;   //reset timer A
  TACTL = TASSEL1 + MC0;     // up mode

  TACCR0 = 5;  // this is 1 us period( 3 is 430x12x1)

  TAR = 0;
  TACCTL0 = OUTMOD2; // RESET MODE

  BCSCTL2 |= DIVM_1;

/*******************************************************************************
*   The starting of the transmitting code. Transmitting code must send 4 or 16
*   of M/LF, then send 010111 preamble before sending data package. TRext
*   determines how many M/LFs are sent.
*
*   Used Register
*   R4 = CMD address, R5 = bits, R6 = counting 16 bits, R7 = 1 Word data, R9 =
*   temp value for loop R10 = temp value for the loop, R13 = 16 bits compare,
*   R14 = timer_value for 11, R15 = timer_value for 5
*******************************************************************************/


  //<-------- The below code will initiate some set up ---------------------->//
    //asm("MOV #05h, R14");
    //asm("MOV #02h, R15");
    bits = TRext;           // 5 cycles
    asm("NOP");             // 1 cycles
    asm("CMP #0001h, R5");  // 1 cycles
    asm("JEQ TRextIs_1");   // 2 cycles
    asm("MOV #0004h, R9");  // 1 cycles
    asm("JMP otherSetup");  // 2 cycles

    // initialize loop for 16 M/LF
    asm("TRextIs_1:");
    asm("MOV #000fh, R9");    // 2 cycles    *** this will chagne to right value
    asm("NOP");

    //
    asm("otherSetup:");
    bits = numOfBits;         // (2 cycles).  This value will be adjusted. if
                              // numOfBit is constant, it takes 1 cycles
    asm("NOP");               // (1 cycles), zhangh 0316

    asm("MOV #0bh, R14");     // (2 cycles) R14 is used as timer value 11, it
                              // will be 2 us in 3 MHz
    asm("MOV #05h, R15");     // (2 cycles) R15 is used as tiemr value 5, it
                              // will be 1 us in 3 MHz
    asm("MOV @R4+, R7");      // (2 cycles) Assign data to R7
    asm("MOV #0010h, R13");   // (2 cycles) Assign decimal 16 to R13, so it will
                              // reduce the 1 cycle from below code
    asm("MOV R13, R6");       // (1 cycle)
    asm("SWPB R7");           // (1 cycle)    Swap Hi-byte and Low byte
    asm("NOP");
    asm("NOP");
    // new timing needs 11 cycles
    asm("NOP");
    //asm("NOP");       // up to here, it make 1 to 0 transition.
    //<----------------2 us --------------------------------
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    //asm("NOP");   // 7
    //asm("NOP");   // 8
    //asm("NOP");   // 9
    // <---------- End of 1 us ------------------------------
    // The below code will create the number of M/LF.  According to the spec,
    // if the TRext is 0, there are 4 M/LF.  If the TRext is 1, there are 16
    // M/LF
    // The upper code executed 1 M/LF, so the count(R9) should be number of M/LF
    // - 1
    //asm("MOV #000fh, R9");    // 2 cycles  *** this will chagne to right value
    asm("MOV #0001h, R10");   // 1 cycles, zhangh?
    // The below code will create the number base encoding waveform., so the
    // number of count(R9) should be times of M
    // For example, if M = 2 and TRext are 1(16, the number of count should be
    // 32.
    asm("M_LF_Count:");
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    asm("NOP");   // 7
    asm("NOP");   // 8
    asm("NOP");   // 9
    asm("NOP");   // 10
    asm("NOP");   // 11
    asm("NOP");   // 12
    asm("NOP");   // 13
    asm("NOP");   // 14
    asm("NOP");   // 15
    asm("NOP");   // 16
    // asm("NOP");   // 17

    asm("CMP R10, R9");       // 1 cycle
    asm("JEQ M_LF_Count_End"); // 2 cycles
    asm("INC R10");           // 1 cycle
    asm("NOP");   // 22
    asm("JMP M_LF_Count");      // 2 cycles

    asm("M_LF_Count_End:");
    // this code is preamble for 010111 , but for the loop, it will only send
    // 01011
    asm("MOV #5c00h, R9");      // 2 cycles
    asm("MOV #0006h, R10");     // 2 cycles

    asm("NOP");                   // 1 cycle zhangh 0316, 2

    // this should be counted as 0. Therefore, Assembly DEC line should be 1
    // after executing
    asm("Preamble_Loop:");
    asm("DEC R10");               // 1 cycle
    asm("JZ last_preamble_set");          // 2 cycle
    asm("RLC R9");                // 1 cycle
    asm("JNC preamble_Zero");     // 2 cycle      .. up to 6
    // this is 1 case for preamble
    asm("NOP");
    asm("NOP");                   // 1 cycle
#if USE_2618
    asm("MOV R14, &0x0172");       // 3 cycle      .. 10
#else
    asm("MOV R14, &0x0172");       // 3 cycle      .. 10
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");                   // 1 cycle
#if USE_2618
    asm("MOV R15, &0x0172");       // 3 cycle      .. 18
#else
    asm("MOV R15, &0x0172");       // 3 cycle      .. 18
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");                   // .. 22
    asm("JMP Preamble_Loop");     // 2 cycles   .. 24

    // this is 0 case for preamble
    asm("preamble_Zero:");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");


    asm("JMP Preamble_Loop");     // 2 cycles .. 24

    asm("last_preamble_set:");
    asm("NOP");			// 4
    asm("NOP");
    asm("NOP");    // TURN ON
    asm("NOP");
    asm("NOP");             // 1 cycle
#if USE_2618
    asm("MOV.B R14, &0x0172");// 3 cycles
#else
    asm("MOV.B R14, &0x0172");// 3 cycles
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
#if USE_2618
    asm("MOV.B R15, &0x0172");
#else
    asm("MOV.B R15, &0x0172");
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    // Here are another 4 cycles. But 3~5 cycles might also work, need to try.
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
//    asm("NOP");


    //<------------- end of initial set up

/***********************************************************************
*   The main loop code for transmitting data in 3 MHz.  This will transmit data
*   in real time.
*   R5(bits) and R6(word count) must be 1 bigger than desired value.
*   Ex) if you want to send 16 bits, you have to store 17 to R5.
************************************************************************/

    // this is starting of loop
    asm("LOOPAGAIN:");
    asm("DEC R5");                              // 1 cycle
    asm("JEQ Three_Cycle_Loop_End");            // 2 cycle
    //<--------------loop condition ------------
//    asm("NOP");                                 // 1 cycle
    asm("RLC R7");                              // 1 cycle
    asm("JNC bit_is_zero");	                // 2 cycles  ..6

    // bit is 1
    asm("bit_is_one:");
//    asm("NOP");                               // 1 cycle
#if USE_2618
    asm("MOV R14, &0x0172");                   // 3 cycles   ..9
#else
    asm("MOV R14, &0x0172");                   // 3 cycles   ..9
#endif                // 4 cycles   ..11

    asm("DEC R6");                              // 1 cycle  ..10
    asm("JNZ bit_Count_Is_Not_16");              // 2 cycle    .. 12
    // This code will assign new data from reply and then swap bytes.  After
    // that, update R6 with 16 bits
    //asm("MOV @R4+, R7");
#if USE_2618
    asm("MOV R15, &0x0172");                   // 3 cycles   .. 15
#else
    asm("MOV R15, &0x0172");                   // 3 cycles   .. 15
#endif

    asm("MOV R13, R6");                         // 1 cycle   .. 16
    asm("MOV @R4+, R7");                        // 2 cycles  .. 18

    asm("SWPB R7");                             // 1 cycle    .. 19
    //asm("MOV R13, R6");                         // 1 cycle
    // End of assigning data byte
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("JMP LOOPAGAIN");                       // 2 cycle    .. 24

    asm("seq_zero:");
#if USE_2618
    asm("MOV R15, &0x0172");         // 3 cycles       ..3
#else
    asm("MOV R15, &0x0172");         // 3 cycles       ..3
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");                     // 1 cycles .. 6


    // bit is 0, so it will check that next bit is 0 or not
    asm("bit_is_zero:");				// up to 6 cycles
    asm("DEC R6");                      // 1 cycle   .. 7
    asm("JNE bit_Count_Is_Not_16_From0");           // 2 cycles  .. 9
    // bit count is 16
    asm("DEC R5");                      // 1 cycle   .. 10
    asm("JEQ Thirteen_Cycle_Loop_End");     // 2 cycle   .. 12
    // This code will assign new data from reply and then swap bytes.  After
    // that, update R6 with 16 bits
    asm("MOV @R4+,R7");                 // 2 cycles     14
    asm("SWPB R7");                     // 1 cycle      15
    asm("MOV R13, R6");                 // 1 cycles     16
    // End of assigning new data byte
    asm("RLC R7");		        // 1 cycles     17
    asm("JC nextBitIs1");	        // 2 cycles  .. 19
    // bit is 0
#if USE_2618
    asm("MOV R14, &0x0172");             // 3 cycles  .. 22
#else
    asm("MOV R14, &0x0172");             // 3 cycles  .. 22
#endif
    // Next bit is 0 , it is 00 case
    asm("JMP seq_zero");                // 2 cycles .. 24

// <---------this code is 00 case with no 16 bits.
    asm("bit_Count_Is_Not_16_From0:");                  // up to 9 cycles
    asm("DEC R5");                          // 1 cycle      10
    asm("JEQ Thirteen_Cycle_Loop_End");     // 2 cycle    ..12
    asm("NOP");         	            // 1 cycles    ..13
    asm("NOP");                             // 1 cycles    ..14
    asm("NOP");                             // 1 cycles    ..15
    asm("RLC R7");	                    // 1 cycle     .. 16
    asm("JC nextBitIs1");	            // 2 cycles    ..18

#if USE_2618
    asm("MOV R14, &0x0172");               // 3 cycles   .. 21
#else
    asm("MOV R14, &0x0172");               // 3 cycles   .. 21
#endif
    asm("NOP");                         // 1 cycle   .. 22
    asm("JMP seq_zero");        // 2 cycles    .. 24

// whenever current bit is 0, then next bit is 1
    asm("nextBitIs1:");     // 18
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");       // 24

    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("JMP bit_is_one");  // end of bit 0 .. 6

    asm("bit_Count_Is_Not_16:");       // up to here 14
#if USE_2618
    asm("MOV R15, &0x0172");             // 3 cycles   .. 15
#else
    asm("MOV R15, &0x0172");             // 3 cycles   .. 15
#endif

    asm("NOP");                               // 1 cycle .. 16
    asm("NOP");                               // 1 cycle .. 17
    asm("NOP");                               // 1 cycle .. 18
    asm("NOP");                               // 1 cycle .. 19
    asm("NOP");                               // 1 cycle .. 20
    asm("NOP");                               // 1 cycle .. 21
    asm("NOP");                               // 1 cycle .. 22
    asm("JMP LOOPAGAIN");     // 2 cycle          .. 24

    // below code is the end of loop code
    asm("Three_Cycle_Loop_End:");
    asm("JMP lastBit");     // 2 cycles   .. 5

    asm("Thirteen_Cycle_Loop_End:");  // up to 12
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    asm("NOP");   // 7
    asm("NOP");   // 8
    asm("NOP");   // 9
    asm("NOP");   // 10
    asm("NOP");   // 11
    asm("NOP");   // 12 ..24
    asm("NOP");   // 13
    asm("NOP");   // 14
    asm("JMP lastBit"); // 16
/***********************************************************************
*   End of main loop
************************************************************************/
// this is last data 1 bit which is dummy data
    asm("lastBit:");  // up to 4
    asm("NOP");       // 5
    asm("NOP");
#if USE_2618
    asm("MOV.B R14, &0x0172");// 3 cycles
#else
    asm("MOV.B R14, &0x0172");// 3 cycles
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
#if USE_2618
    asm("MOV.B R15, &0x0172");
#else
    asm("MOV.B R15, &0x0172");
#endif
    asm("NOP");
    asm("NOP");
    // experiment

    asm("NOP");

    //TACCR0 = 0;

    TACCTL0 = 0;  // DON'T NEED THIS NOP
    RECEIVE_CLOCK;

}
