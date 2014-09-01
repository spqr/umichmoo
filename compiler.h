#ifndef __MOO_COMPILIERS_H__
#define __MOO_COMPILIERS_H__

/* Helper functions */
#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

#ifdef __GNUC__
/* GNU C compiler supported */
#elif __IAR_SYSTEMS_ICC__
/* IAR compiler supported */
#else
	#error Unsupported compiler
#endif

/* Interrupts */
#ifdef __GNUC__
	#define ISR(num, func_name) __attribute__((interrupt(num))) void func_name(void)
#elif __IAR_SYSTEMS_ICC__
	#define ISR(num, func_name) #pragma vector=num \
                                    __interrupt void func_name(void)
#endif

/* Lock Register */
#ifdef __GNUC__
	#define LOCK_REG(reg_num, var_decl) register var_decl asm(EXPAND_AND_QUOTE(R ## reg_num))
#elif __IAR_SYSTEMS_ICC__
	#define LOCK_REG(reg_num, var_decl) __regvar __no_init var_decl @ ## num ##
#endif
/* End Lock Register */

/* Delay cycles */
#ifdef __GNUC__
	static inline void __delay_cycles(unsigned int n) {
		__asm__ __volatile__ (
				"1: \n"
				" dec %[n] \n"
				" jne 1b \n"
				: [n] "+r"(n));
	}
#elif __IAR_SYSTEMS_ICC__
	/* Defined by the compilier */
#endif
/* End Delay cycles */

/* Pragma Message */
#ifdef __GNUC__
	#define DO_PRAGMA(x) _Pragma (#x)
	#define MESSAGE(msg) DO_PRAGMA(message (msg))
#elif __IAR_SYSTEMS_ICC__
	#define MESSAGE(msg) #pragma message (msg)
#endif
/* End Pragma Message */

#endif /*__MOO_COMPILIERS_H__ */
