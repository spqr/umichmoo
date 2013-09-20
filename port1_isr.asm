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
//                     Mayberry\Desktop\moofirmwaredev\build_port1_isr.c      /
//    Command line  =  "C:\Documents and Settings\Addison                     /
//                     Mayberry\Desktop\moofirmwaredev\build_port1_isr.c"     /
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
//                     Mayberry\Desktop\moofirmwaredev\Debug\List\build_port1 /
//                     _isr.s43                                               /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME port1_isr

        RTMODEL "__SystemLibrary", "DLib"
        RTMODEL "__core", "430X"
        RTMODEL "__data_model", "small"
        RTMODEL "__double_size", "32"
        RTMODEL "__pic", "no"
        RTMODEL "__reg_r4", "regvar"
        RTMODEL "__reg_r5", "regvar"
        RTMODEL "__rt_version", "3"

        RSEG CSTACK:DATA:SORT:NOROOT(0)

        PUBWEAK `??Port1_ISR??INTVEC 36`
        PUBWEAK P1IE
        PUBWEAK P1IES
        PUBWEAK P1IFG
        PUBWEAK P1SEL
        PUBLIC Port1_ISR
        FUNCTION Port1_ISR,080233H
        ARGFRAME CSTACK, 0, STACK
        LOCFRAME CSTACK, 4, STACK
        PUBWEAK TACCTL1
        PUBWEAK TAR
        PUBLIC port1_isr_decls
        FUNCTION port1_isr_decls,0201H
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
        
Port1_ISR           SYMBOL "Port1_ISR"
`??Port1_ISR??INTVEC 36` SYMBOL "??INTVEC 36", Port1_ISR

        EXTERN delimiterNotFound


        ASEGN DATA16_AN:DATA:NOROOT,023H
// unsigned char volatile P1IFG
P1IFG:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,024H
// unsigned char volatile P1IES
P1IES:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,025H
// unsigned char volatile P1IE
P1IE:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,026H
// unsigned char volatile P1SEL
P1SEL:
        DS8 1

        ASEGN DATA16_AN:DATA:NOROOT,0164H
// unsigned short volatile TACCTL1
TACCTL1:
        DS8 2

        ASEGN DATA16_AN:DATA:NOROOT,0170H
// unsigned short volatile TAR
TAR:
        DS8 2

// This is needed to make the inline assembly compile properly w/ these symbols
        RSEG CODE:CODE:REORDER:NOROOT(1)
port1_isr_decls:
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function port1_isr_decls
        MOV.B   &0x25, R14
        MOV.W   &0x164, R15
        MOV.B   &0x26, R13
        MOV.B   &0x24, R12
        RETA
        CFI EndBlock cfiBlock0
        REQUIRE P1IE
        REQUIRE TACCTL1
        REQUIRE P1SEL
        REQUIRE P1IES

//*************************************************************************
//************************ PORT 1 INTERRUPT *******************************

// warning   :  Whenever the clock frequency changes, the value of TAR should be
//              changed in aesterick lines
// Pin Setup :  P1.2
// Description : Port 1 interrupt is used as finding delimeter.

        RSEG ISR_CODE:CODE:REORDER:NOROOT(1)
Port1_ISR:
        CFI Block cfiBlock1 Using cfiCommon1
        CFI Function Port1_ISR        // (5-6 cycles) to enter interrupt
        MOV TAR, R7                   // move TAR to R7(count) register (3 CYCLES)
        MOV.B   #0x0, &0x23           // 4 cycles, clear P1IFG
        MOV.W   #0x0, &0x170          // 4 cycles, TAR = 0
        BIC.W   #0xf0, 0(SP)          // Change status register so we leave low
                                      // power mode 4 when exit interrupt
        CMP #0000h, R5                // if (bits == 0) (1 cycle)
        JEQ bit_Is_Zero_In_Port_Int   // 2 cycles
        // bits != 0:
        MOV #0000h, R5                // bits = 0  (1 cycles)
        CMP #0010h, R7                // finding delimeter (12.5us, 2 cycles)
                                      // 2d -> 14
        JNC delimiter_Value_Is_wrong  //(2 cycles)
        CMP #0040h, R7                // finding delimeter (12.5us, 2 cycles)
                                      // 43H
        JC  delimiter_Value_Is_wrong
        CLR P1IE
        BIS #8010h, TACCTL1           // (5 cycles)   TACCTL1 |= CM1 + CCIE
        MOV #0004h, P1SEL             // enable TimerA1    (4 cycles)
        RETI
delimiter_Value_Is_wrong:
        BIC #0004h, P1IES
        MOV #0000h, R5                // bits = 0  (1 cycles)
        MOV.B   #0x1, &delimiterNotFound
        RETI
bit_Is_Zero_In_Port_Int:              // bits == 0
        MOV #0000h, TAR               // reset timer (4 cycles)
        BIS #0004h, P1IES         // 4 cycles change port interrupt edge to neg
        INC R5                        // 1 cycle
        RETI
        CFI EndBlock cfiBlock1
        REQUIRE P1IFG
        REQUIRE TAR

        COMMON INTVEC:CONST:ROOT(1)
        ORG 36
`??Port1_ISR??INTVEC 36`:
        DC16    Port1_ISR

        END
// 
// 18 bytes in segment CODE
//  8 bytes in segment DATA16_AN
//  2 bytes in segment INTVEC
// 78 bytes in segment ISR_CODE
// 
// 96 bytes of CODE  memory
//  0 bytes of CONST memory (+ 2 bytes shared)
//  0 bytes of DATA  memory (+ 8 bytes shared)
//
//Errors: none
//Warnings: none
