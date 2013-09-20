///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR C/C++ Compiler V5.10.6.50180/W32 for MSP430      03/Aug/2012  15:07:53 /
// Copyright 1996-2010 IAR Systems AB.                                        /
//                                                                            /
//    __rt_version  =  3                                                      /
//    __double_size =  32                                                     /
//    __reg_r4      =  regvar                                                 /
//    __reg_r5      =  regvar                                                 /
//    __pic         =  no                                                     /
//    __core        =  430X                                                   /
//    __data_model  =  small                                                  /
//    Source file   =  C:\Documents and Settings\Addison                      /
//                     Mayberry\Desktop\moofirmwaredev\build_send_to_reader.c /
//    Command line  =  "C:\Documents and Settings\Addison                     /
//                     Mayberry\Desktop\moofirmwaredev\build_send_to_reader.c /
//                     " -lcN "C:\Documents and Settings\Addison              /
//                     Mayberry\Desktop\moofirmwaredev\Debug\List\" -la       /
//                     "C:\Documents and Settings\Addison                     /
//                     Mayberry\Desktop\moofirmwaredev\Debug\List\" -o        /
//                     "C:\Documents and Settings\Addison                     /
//                     Mayberry\Desktop\moofirmwaredev\Debug\Obj\" --no_cse   /
//                     --no_unroll --no_inline --no_code_motion --no_tbaa     /
//                     --debug -D__MSP430F2618__ -e --double=32               /
//                     --dlib_config "C:\Program Files\IAR Systems\Embedded   /
//                     Workbench 6.0\430\LIB\DLIB\dl430xsfn.h" --regvar_r4    /
//                     --regvar_r5 --core=430X --data_model=small -Ol         /
//                     --multiplier=16s                                       /
//    List file     =  C:\Documents and Settings\Addison                      /
//                     Mayberry\Desktop\moofirmwaredev\Debug\List\build_send_ /
//                     to_reader.s43                                          /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

// Needed for definition of MILLER_4_ENCODING
#include "mymoo.h"

        NAME send_to_reader

        RTMODEL "__SystemLibrary", "DLib"
        RTMODEL "__core", "430X"
        RTMODEL "__data_model", "small"
        RTMODEL "__double_size", "32"
        RTMODEL "__pic", "no"
        RTMODEL "__reg_r4", "regvar"
        RTMODEL "__reg_r5", "regvar"
        RTMODEL "__rt_version", "3"

        RSEG CSTACK:DATA:SORT:NOROOT(0)

        PUBWEAK BCSCTL1
        PUBWEAK BCSCTL2
        PUBWEAK DCOCTL
        PUBWEAK P1SEL
        PUBWEAK TACCR0
        PUBWEAK TACCTL0
        PUBWEAK TACTL
        PUBWEAK TAR
        PUBLIC sendToReader
        FUNCTION sendToReader,080203H
        ARGFRAME CSTACK, 0, STACK
        LOCFRAME CSTACK, 4, STACK
        
        CFI Names cfiNames0
        CFI StackFrame CFA SP DATA
        CFI Resource PC:20, SP:20, SR:16, R4L:16, R4H:4, R4:20, R5L:16, R5H:4
        CFI Resource R5:20, R6L:16, R6H:4, R6:20, R7L:16, R7H:4, R7:20, R8L:16
        CFI Resource R8H:4, R8:20, R9L:16, R9H:4, R9:20, R10L:16, R10H:4
        CFI Resource R10:20, R11L:16, R11H:4, R11:20, R12L:16, R12H:4, R12:20
        CFI Resource R13L:16, R13H:4, R13:20, R14L:16, R14H:4, R14:20, R15L:16
        CFI Resource R15H:4, R15:20
        CFI ResourceParts R4 R4H, R4L
        CFI ResourceParts R5 R5H, R5L
        CFI ResourceParts R6 R6H, R6L
        CFI ResourceParts R7 R7H, R7L
        CFI ResourceParts R8 R8H, R8L
        CFI ResourceParts R9 R9H, R9L
        CFI ResourceParts R10 R10H, R10L
        CFI ResourceParts R11 R11H, R11L
        CFI ResourceParts R12 R12H, R12L
        CFI ResourceParts R13 R13H, R13L
        CFI ResourceParts R14 R14H, R14L
        CFI ResourceParts R15 R15H, R15L
        CFI EndNames cfiNames0
        
        CFI Common cfiCommon0 Using cfiNames0
        CFI CodeAlign 2
        CFI DataAlign 2
        CFI ReturnAddress PC CODE
        CFI CFA SP+4
        CFI PC Frame(CFA, -4)
        CFI SR Undefined
        CFI R4L SameValue
        CFI R4H 0
        CFI R4 Concat
        CFI R5L SameValue
        CFI R5H 0
        CFI R5 Concat
        CFI R6L SameValue
        CFI R6H 0
        CFI R6 Concat
        CFI R7L SameValue
        CFI R7H 0
        CFI R7 Concat
        CFI R8L SameValue
        CFI R8H 0
        CFI R8 Concat
        CFI R9L SameValue
        CFI R9H 0
        CFI R9 Concat
        CFI R10L SameValue
        CFI R10H 0
        CFI R10 Concat
        CFI R11L SameValue
        CFI R11H 0
        CFI R11 Concat
        CFI R12L Undefined
        CFI R12H Undefined
        CFI R12 Undefined
        CFI R13L Undefined
        CFI R13H Undefined
        CFI R13 Undefined
        CFI R14L Undefined
        CFI R14H Undefined
        CFI R14 Undefined
        CFI R15L Undefined
        CFI R15H Undefined
        CFI R15 Undefined
        CFI EndCommon cfiCommon0
        
        EXTERN TRext
        EXTERN bits
        EXTERN dest


        ASEGN DATA16_AN:DATA:NOROOT,056H
// unsigned char volatile DCOCTL
DCOCTL:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,057H
// unsigned char volatile BCSCTL1
BCSCTL1:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,058H
// unsigned char volatile BCSCTL2
BCSCTL2:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,026H
// unsigned char volatile P1SEL
P1SEL:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,0160H
// unsigned short volatile TACTL
TACTL:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0162H
// unsigned short volatile TACCTL0
TACCTL0:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0170H
// unsigned short volatile TAR
TAR:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0172H
// unsigned short volatile TACCR0
TACCR0:
        DS8 2

//
//
// experimental M4 code
//
//

/******************************************************************************
*   Pin Set up
*   P1.1 - communication output
*******************************************************************************/
        RSEG CODE:CODE:REORDER:NOROOT(1)
sendToReader:
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function sendToReader
        MOV.B   #0x89, &0x57
        MOV.B   #0xc0, &0x56
        BIC.W   #0x2, &0x160
        MOV.W   #0x0, &0x170
        MOV.W   R12, R4               // assign data address to dest
        // Setup timer
        BIS.B   #0x2, &0x26           //  select TIMER_A0
        BIS.W   #0x4, &0x160          //  reset timer A
        MOV.W   #0x210, &0x160        //  up mode
        MOV.W   #0x5, &0x172          //  this is 1 us period( 3 is 430x12x1)
        MOV.W   #0x0, &0x170
        MOV.W   #0x80, &0x162         //  RESET MODE
#if MILLER_4_ENCODING
        BIS.B   #0x10, &0x58
#endif
        
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
        //MOV #05h, R14
        //MOV #02h, R15
        MOV.B   &TRext, R15       // }
        MOV.B   R15, R15          // }  5 cycles
        MOV.W   R15, R5           // }
        NOP                       // 1 cycle
        CMP #0001h, R5            // 1 cycle
        JEQ TRextIs_1             // 2 cycles
        MOV #0004h, R9            // 1 cycle
        JMP otherSetup            // 2 cycles
        // initialize loop for 16 M/LF
TRextIs_1:
        MOV #000fh, R9          // 2 cycles *** this will change to right value
        NOP
otherSetup:
        MOV.B   R13, R13          // } (2 cycles).  This value will be adjusted. if
        MOV.W   R13, R5           // } numOfBit is constant, it takes 1 cycles
        NOP                       // (1 cycles), zhangh 0316
        MOV #0bh, R14             // (2 cycles) R14 is used as timer value 11, it will be 2 us in 3 MHz
        MOV #05h, R15             // (2 cycles) R15 is used as timer value 5, it will be 1 us in 3 MHz
        MOV @R4+, R7              // (2 cycles) Assign data to R7
        MOV #0010h, R13           // (2 cycles) Assign decimal 16 to R13, so it will reduce the 1 cycle from below code
        MOV R13, R6               // (1 cycle)
        SWPB R7                   // (1 cycle)    Swap Hi-byte and Low byte
        NOP
        NOP
        // new timing needs 11 cycles
        NOP
        //NOP                     // up to here, it make 1 to 0 transition.
        //<----------------2 us --------------------------------
        NOP                       // 1
        NOP                       // 2
        NOP                       // 3
        NOP                       // 4
        NOP                       // 5
        NOP                       // 6
        //NOP                       // 7
        //NOP                       // 8
        //NOP                       // 9
        // <---------- End of 1 us ------------------------------
        // The below code will create the number of M/LF.  According to the spec,
        // if the TRext is 0, there are 4 M/LF.  If the TRext is 1, there are 16
        // M/LF
        // The upper code executed 1 M/LF, so the count(R9) should be number of M/LF
        // - 1
        //MOV #000fh, R9              // 2 cycles  *** this will change to right value
        MOV #0001h, R10               // 1 cycles, zhangh?
        // The below code will create the number base encoding waveform., so the
        // number of count(R9) should be times of M
        // For example, if M = 2 and TRext are 1(16, the number of count should be
        // 32.
M_LF_Count:
        NOP                       // 1
        NOP                       // 2
        NOP                       // 3
        NOP                       // 4
        NOP                       // 5
        NOP                       // 6
        NOP                       // 7
        NOP                       // 8
        NOP                       // 9
        NOP                       // 10
        NOP                       // 11
        NOP                       // 12
        NOP                       // 13
        NOP                       // 14
        NOP                       // 15
        NOP                       // 16
        //NOP                       // 17
        CMP R10, R9               // 1 cycle
        JEQ M_LF_Count_End        // 2 cycles
        INC R10                   // 1 cycle
        NOP                       // 22
        JMP M_LF_Count            // 2 cycles
        // this code is preamble for 010111 , but for the loop, it will only send 01011
M_LF_Count_End:
        MOV #5c00h, R9            // 2 cycles
        MOV #0006h, R10           // 2 cycles
        NOP                       // 1 cycle zhangh 0316, 2
       // this should be counted as 0. Therefore, Assembly DEC line should be 1 after executing
Preamble_Loop:
        DEC R10                   // 1 cycle
        JZ last_preamble_set      // 2 cycle
        RLC R9                    // 1 cycle
        JNC preamble_Zero         // 2 cycle      .. up to 6
        NOP
        NOP                       // 1 cycle
        MOV R14, TACCR0           // 3 cycle      .. 10
        NOP
        NOP
        NOP
        NOP                       // 1 cycle
        MOV R15, TACCR0           // 3 cycle      .. 18
        NOP
        NOP
        NOP
        NOP                       // .. 22
        JMP Preamble_Loop         // 2 cycles   .. 24
        // this is 0 case for preamble
preamble_Zero:
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        JMP Preamble_Loop         // 2 cycles .. 24
last_preamble_set:
        NOP                       // 4
        NOP
        NOP                       // TURN ON
        NOP
        NOP                       // 1 cycle
        MOV.B R14, TACCR0         // 3 cycles
        NOP
        NOP
        NOP
        NOP
        MOV.B R15, TACCR0
        NOP
        NOP
        NOP
        NOP
        NOP
        // Here are another 4 cycles. But 3~5 cycles might also work, need to try.// Here are another 4 cycles. But 3~5 cycles might also work, need to try.
        NOP
        NOP
        NOP
        NOP
        //NOP
        
        /***********************************************************************
        *   The main loop code for transmitting data in 3 MHz.  This will transmit data
        *   in real time.
        *   R5(bits) and R6(word count) must be 1 bigger than desired value.
        *   Ex) if you want to send 16 bits, you have to store 17 to R5.
        ************************************************************************/

        // this is starting of loop
LOOPAGAIN:
        DEC R5                    // 1 cycle
        JEQ Three_Cycle_Loop_End  // 2 cycle
        //<--------------loop condition ------------
        //NOP                       // 1 cycle
        RLC R7                    // 1 cycle
        JNC bit_is_zero           // 2 cycles  ..6
        // bit is 1
bit_is_one:
        //NOP                       // 1 cycle
        MOV R14, TACCR0           // 3 cycles   ..9
                                  // 4 cycles   ..11 (? - not sure what this is referring to - mayberry)
        DEC R6                    // 1 cycle  ..10
        JNZ bit_Count_Is_Not_16   // 2 cycle    .. 12
        MOV R15, TACCR0           // 3 cycles   .. 15
        MOV R13, R6               // 1 cycle   .. 16
        MOV @R4+, R7              // 2 cycles  .. 18
        SWPB R7                   // 1 cycle    .. 19
        //MOV R13, R6               // 1 cycle
        // End of assigning data byte
        NOP
        NOP
        NOP
        JMP LOOPAGAIN             // 2 cycle    .. 24
seq_zero:
        MOV R15, TACCR0           // 3 cycles       ..3
        NOP
        NOP
        NOP                       // 1 cycles .. 6
        
        // bit is 0, so it will check that next bit is 0 or not
bit_is_zero:                      // up to 6 cycles
        DEC R6                    // 1 cycle   .. 7
        JNE bit_Count_Is_Not_16_From0   // 2 cycles  .. 9
        // bit count is 16
        DEC R5                    // 1 cycle   .. 10
        JEQ Thirteen_Cycle_Loop_End   // 2 cycle   .. 12
        // This code will assign new data from reply and then swap bytes.  After
        // that, update R6 with 16 bits
        MOV @R4+,R7               // 2 cycles     14
        SWPB R7                   // 1 cycle      15
        MOV R13, R6               // 1 cycles     16
        // End of assigning new data byte
        RLC R7                    // 1 cycles     17
        JC nextBitIs1             // 2 cycles  .. 19
        // bit is 0
        MOV R14, TACCR0           // 3 cycles  .. 22
        // Next bit is 0 , it is 00 case
        JMP seq_zero              // 2 cycles .. 24
        
        // <---------this code is 00 case with no 16 bits.
bit_Count_Is_Not_16_From0:        // up to 9 cycles
        DEC R5                    // 1 cycle      10
        JEQ Thirteen_Cycle_Loop_End   // 2 cycle    ..12
        NOP                       // 1 cycles    ..13
        NOP                       // 1 cycles    ..14
        NOP                       // 1 cycles    ..15
        RLC R7                    // 1 cycle     .. 16
        JC nextBitIs1             // 2 cycles    ..18
        MOV R14, TACCR0           // 3 cycles   .. 21
        NOP                       // 1 cycle   .. 22
        JMP seq_zero              // 2 cycles    .. 24
        
        // whenever current bit is 0, then next bit is 1
nextBitIs1:                       // 18
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP                       // 24
        NOP
        NOP
        NOP
        JMP bit_is_one            // end of bit 0 .. 6
bit_Count_Is_Not_16:              // up to here 14
        MOV R15, TACCR0           // 3 cycles   .. 15
        NOP                       // 1 cycle .. 16
        NOP                       // 1 cycle .. 17
        NOP                       // 1 cycle .. 18
        NOP                       // 1 cycle .. 19
        NOP                       // 1 cycle .. 20
        NOP                       // 1 cycle .. 21
        NOP                       // 1 cycle .. 22
        JMP LOOPAGAIN             // 2 cycle          .. 24
        
        // below code is the end of loop code
Three_Cycle_Loop_End:
        JMP lastBit               // 2 cycles   .. 5
        
Thirteen_Cycle_Loop_End:          // up to 12
        NOP                       // 1
        NOP                       // 2
        NOP                       // 3
        NOP                       // 4
        NOP                       // 5
        NOP                       // 6
        NOP                       // 7
        NOP                       // 8
        NOP                       // 9
        NOP                       // 10
        NOP                       // 11
        NOP                       // 12 ..24
        NOP                       // 13
        NOP                       // 14
        JMP lastBit               // 16
        /***********************************************************************
        *   End of main loop
        ************************************************************************/
        
        // this is last data 1 bit which is dummy data
lastBit:                          // up to 4
        NOP                       // 5
        NOP
        MOV.B R14, TACCR0         // 3 cycles
        NOP
        NOP
        NOP
        MOV.B R15, TACCR0
        NOP
        NOP
        // experiment
        NOP
        MOV.W   #0x0, &0x162      // DON'T NEED THIS NOP
        MOV.B   #0x8b, &0x57
        MOV.B   #0x0, &0x56
        MOV.B   #0x0, &0x58
        RETA
        CFI EndBlock cfiBlock0
        REQUIRE BCSCTL1
        REQUIRE DCOCTL
        REQUIRE TACTL
        REQUIRE TAR
        REQUIRE dest
        REQUIRE P1SEL
        REQUIRE TACCR0
        REQUIRE TACCTL0
        REQUIRE BCSCTL2
        REQUIRE bits

        END
// 
// 502 bytes in segment CODE
//  12 bytes in segment DATA16_AN
// 
// 502 bytes of CODE memory
//   0 bytes of DATA memory (+ 12 bytes shared)
//
//Errors: none
//Warnings: none
