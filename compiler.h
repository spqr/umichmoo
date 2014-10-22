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

/* Register Sensors */
#ifdef __GNUC__
	#define __initcall(x) \
		static const struct sensor * const __initcall_##x __init_call = &(x); \
		static const struct sensor * const __space_struct_##x __sensor_space;
	#define __init_call     __attribute__ ((unused,__section__ ("SENSOR_INIT_I")))
	#define __sensor_space  __attribute__ ((unused,__section__ ("SENSOR_SPACE")))
#elif __IAR_SYSTEMS_ICC__
	#pragma segment="SENSOR_INIT_I" __data16
	#pragma segment="SENSOR_SPACE" __data16
	#define __init_call     _Pragma("location=\"SENSOR_INIT_I\"")
	#define __sensor_space  _Pragma("diag_suppress=Be033") _Pragma("location=\"SENSOR_SPACE\"")
	#define __initcall(x) \
		__init_call __root static const struct sensor * const __initstruct_##x  = &(x); \
		__sensor_space __root static const struct sensor * __space_struct_##x;
#endif
/* End Register Sensors */
#ifdef __GNUC__
	#define __sensor_init_begin  (const struct sensor **) (&__sensor_init_begin_link)
	#define __sensor_init_end    (const struct sensor **) (&__sensor_init_end_link)
	#define __sensor_space_begin (const struct sensor **) (&__sensor_space_begin_link)
	#define __sensor_space_end   (const struct sensor **) (&__sensor_space_end_link)
#elif __IAR_SYSTEMS_ICC__
	#include <intrinsics.h>
	#define __moo_segment_begin(x) (const struct sensor **) __segment_begin(x)
	#define __moo_segment_end(x) (const struct sensor **) __segment_end(x)
	#define __sensor_init_begin  moo_segment_begin("SENSOR_INIT_I")
	#define __sensor_init_end    moo_segment_end("SENSOR_INIT_I")
	#define __sensor_space_begin  moo_segment_begin("SENSOR_SPACE")
	#define __sensor_space_end  moo_segment_end("SENSOR_SPACE")
#endif
/* Register Sensors Segments */

/* End Register Sensors Segments */

#endif /*__MOO_COMPILIERS_H__ */
