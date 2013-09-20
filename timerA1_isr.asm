///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR C/C++ Compiler V5.10.6.50180/W32 for MSP430      03/Aug/2012  15:07:54 /
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
//                     Mayberry\Desktop\moofirmwaredev\build_timera1_isr.c    /
//    Command line  =  "C:\Documents and Settings\Addison                     /
//                     Mayberry\Desktop\moofirmwaredev\build_timera1_isr.c"   /
//                     -lcN "C:\Documents and Settings\Addison                /
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
//                     Mayberry\Desktop\moofirmwaredev\Debug\List\build_timer /
//                     a1_isr.s43                                             /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME timerA1_isr

        RTMODEL "__SystemLibrary", "DLib"
        RTMODEL "__core", "430X"
        RTMODEL "__data_model", "small"
        RTMODEL "__double_size", "32"
        RTMODEL "__pic", "no"
        RTMODEL "__reg_r4", "regvar"
        RTMODEL "__reg_r5", "regvar"
        RTMODEL "__rt_version", "3"

        RSEG CSTACK:DATA:SORT:NOROOT(0)

        PUBWEAK `??TimerA1_ISR??INTVEC 48`
        PUBWEAK TACCR1
        PUBWEAK TACCTL1
        PUBWEAK TAR
        PUBLIC TimerA1_ISR
        FUNCTION TimerA1_ISR,080233H
        ARGFRAME CSTACK, 0, STACK
        LOCFRAME CSTACK, 4, STACK
        PUBLIC timera1_isr_decls
        FUNCTION timera1_isr_decls,0201H
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
        
        
        CFI Common cfiCommon1 Using cfiNames0
        CFI CodeAlign 2
        CFI DataAlign 2
        CFI ReturnAddress PC CODE
        CFI CFA SP+4
        CFI PC or(add(CFA, literal(-4)), lshift(add(CFA, literal(-2)), 4))
        CFI SR Frame(CFA, -4)
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
        CFI R12L SameValue
        CFI R12H 0
        CFI R12 Concat
        CFI R13L SameValue
        CFI R13H 0
        CFI R13 Concat
        CFI R14L SameValue
        CFI R14H 0
        CFI R14 Concat
        CFI R15L SameValue
        CFI R15H 0
        CFI R15 Concat
        CFI EndCommon cfiCommon1
        
TimerA1_ISR         SYMBOL "TimerA1_ISR"
`??TimerA1_ISR??INTVEC 48` SYMBOL "??INTVEC 48", TimerA1_ISR

        EXTERN TRcal
        EXTERN bits


        ASEGN DATA16_AN:DATA:NOROOT,0164H
// unsigned short volatile TACCTL1
TACCTL1:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0170H
// unsigned short volatile TAR
TAR:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0174H
// unsigned short volatile TACCR1
TACCR1:
        DS8 2

// This is needed to make the inline assembly compile properly w/ this symbol
        RSEG CODE:CODE:REORDER:NOROOT(1)
timera1_isr_decls:
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function timera1_isr_decls
        MOV.W   &0x174, R15
        RETA
        CFI EndBlock cfiBlock0
        REQUIRE TACCR1

//*************************************************************************
//************************ Timer INTERRUPT *******************************

// Pin Setup :  P1.2
// Description :
        RSEG ISR_CODE:CODE:REORDER:NOROOT(1)
TimerA1_ISR:
        CFI Block cfiBlock1 Using cfiCommon1
        CFI Function TimerA1_ISR  // (6 cycles) to enter interrupt
        MOV TACCR1, R7            // move TACCR1 to R7(count) register (3 CYCLES)
        MOV.W   #0x0, &0x170      // reset timer (4 cycles)
        BIC.W   #0x1, &0x164      // must manually clear interrupt flag (4 cycles)
        
        //<------up to here 26 cycles + 6 cyles of Interrupt == 32 cycles -------->
        CMP #0003h, R5            // if (bits >= 3).  it will do store bits
        JGE bit_Is_Over_Three
        // bit is not 3
        CMP #0002h, R5            // if ( bits == 2)
        JEQ bit_Is_Two            // if (bits == 2).
        
        // <----------------- bit is not 2 ------------------------------->
        CMP #0001h, R5            // if (bits == 1) -- measure RTcal value.
        JEQ bit_Is_One            // bits == 1
        
        // <-------------------- this is bit == 0 case --------------------->
bit_Is_Zero_In_Timer_Int:
        CLR R6
        INC R5                    // bits++
        RETI
        // <------------------- end of bit 0  --------------------------->
        
        // <-------------------- this is bit == 1 case --------------------->
bit_Is_One:                       // bits == 1.  calculate RTcal value
        MOV R7, R9                // 1 cycle
        RRA R7                    // R7(count) is divided by 2.   1 cycle
        MOV #0FFFFh, R8           // R8(pivot) is set to max value    1 cycle
        SUB R7, R8                // R8(pivot) = R8(pivot) -R7(count/2) make new R8(pivot) value     1 cycle
        INC R5                    // bits++
        CLR R6
        RETI
        // <------------------ end of bit 1 ------------------------------>
        
        // <-------------------- this is bit == 2 case --------------------->
bit_Is_Two:
        CMP R9, R7                // if (count > (R9)(180)) this is hardcoded number, so have  to change to proper value
        JGE this_Is_TRcal
        // this is data
this_Is_Data_Bit:
        ADD R8, R7                // count = count + pivot
        ADDC.b @R4+,-1(R4)        // roll left (emulated by adding to itself == multiply by 2 + carry)
        INC R6
        CMP #0008,R6              // undo increment of dest* (R4) until we have 8 bits
        JGE out_p
        DEC R4
out_p:                            // decrement R4 if we haven't gotten 16 bits yet (3 or 4 cycles)
        BIC #0008h,R6             // when R6=8, this will set R6=0   (1 cycle)
        INC R5
        RETI
        // <------------------ end of bit 2 ------------------------------>
        
this_Is_TRcal:
        MOV R7, R5                // bits = count. use bits(R5) to assign new value ofvTRcal
        MOV.W   R5, &TRcal        // assign new value     (4 cycles)
        MOV #0003h, R5            // bits = 3..assign 3 to bits, so it will keep track of current bits    (2 cycles)
        CLR R6                    // (1 cycle)
        RETI
        
        // <------------- this is bits >= 3 case ----------------------->
bit_Is_Over_Three:                // bits >= 3 , so store bits
        ADD R8, R7                // R7(count) = R8(pivot) + R7(count),
                                  // store bit by shifting carry flag into cmd[bits]=(dest*) and increment
                                  // dest* (5 cycles)
        ADDC.b @R4+,-1(R4)        // roll left (emulated by adding to itself == multiply by 2 + carry)
        INC R6
        CMP #0008,R6              // undo increment of dest* (R4) until we have 8 bits
        JGE out_p1
        DEC R4
out_p1:                           // decrement R4 if we haven't gotten 16 bits yet (3 or 4 cycles)
        BIC #0008h,R6             // when R6=8, this will set R6=0   (1 cycle)
        INC R5                    // bits++
        RETI
        // <------------------ end of bit is over 3 ------------------------------>
        CFI EndBlock cfiBlock1
        REQUIRE TAR
        REQUIRE TACCTL1
        REQUIRE bits

        COMMON INTVEC:CONST:ROOT(1)
        ORG 48
`??TimerA1_ISR??INTVEC 48`:
        DC16    TimerA1_ISR

        END
// 
//   6 bytes in segment CODE
//   6 bytes in segment DATA16_AN
//   2 bytes in segment INTVEC
// 106 bytes in segment ISR_CODE
// 
// 112 bytes of CODE  memory
//   0 bytes of CONST memory (+ 2 bytes shared)
//   0 bytes of DATA  memory (+ 6 bytes shared)
//
//Errors: none
//Warnings: none
