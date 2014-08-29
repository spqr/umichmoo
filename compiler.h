#ifndef __MOO_COMPILIERS_H__
#define __MOO_COMPILIERS_H__

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
	#define __LOCK_REG_QUOTE(str) #str
	#define __LOCK_REG_EXPAND_AND_QUOTE(str) __LOCK_REG_QUOTE(str)
	#define LOCK_REG(reg_num, var_decl) register var_decl asm(__LOCK_REG_EXPAND_AND_QUOTE(R ## reg_num))
#elif __IAR_SYSTEMS_ICC__
	#define LOCK_REG(reg_num, var_decl) __regvar __no_init var_decl @ ## num ##
#endif
/* End Lock Register */


#endif /*__MOO_COMPILIERS_H__ */
