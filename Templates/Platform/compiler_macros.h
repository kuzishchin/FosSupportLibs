/*
 * compiler_macros.h
 *
 * Abstraction layer of compiler dependent functions
 *
 */

#ifndef APPLICATION_INITIALIZATION_PLATFORM_COPILER_MACROS_H_
#define APPLICATION_INITIALIZATION_PLATFORM_COPILER_MACROS_H_



#define __FPU_PRESENT 1U

/*
// Abstraction layer of compiler dependent functions
*/

#if defined ( __ICCARM__ ) // IAR

	#include <intrinsics.h>

	#define ENTER_CRITICAL(x)     x=__get_interrupt_state(); __disable_interrupt()
	#define LEAVE_CRITICAL(x)     __set_interrupt_state(x)

	#define DISABLE_INTERRUPT()   __disable_interrupt()
	#define ENABLE_INTERRUPT()    __enable_interrupt()

	#define SET_MSP(x)            __set_MSP(x)
	#define GET_MSP(x)            x=__get_MSP()

	#define SET_PSP(x)            __set_PSP(x)
	#define GET_PSP(x)            x=__get_PSP()

	#define SET_CONTROL(x)       __set_CONTROL(x)

	#define NOP()                __no_operation()

	#define SFINLINE             static inline

	#define IAR_COMPILER

#elif defined (__CC_ARM) // KEIL

	#define ENTER_CRITICAL(x)     x=__disable_irq()
	#define LEAVE_CRITICAL(x)     if (!x) __enable_irq()

#elif defined (__GNUC__) //GCC

	#include <stdint.h>
	#include <cmsis_gcc.h>

	#define __weak __attribute__((weak))

	#define ENTER_CRITICAL(x)       x = __get_BASEPRI(); __set_BASEPRI(1 << 4)
    #define LEAVE_CRITICAL(x)       __set_BASEPRI(x)

	#define DISABLE_INTERRUPT()   __set_PRIMASK(1)
	#define ENABLE_INTERRUPT()    __set_PRIMASK(0)

	#define SET_MSP(x)            __set_MSP(x)
	#define GET_MSP(x)            x=__get_MSP()

	#define SET_PSP(x)            __set_PSP(x)
	#define GET_PSP(x)            x=__get_PSP()

	#define SET_CONTROL(x)       __set_CONTROL(x)

	#define NOP()                __NOP()

	#define SFINLINE             __STATIC_FORCEINLINE

	#define GCC_COMPILER

#else
  #error Unknown!
#endif




#endif /* APPLICATION_INITIALIZATION_PLATFORM_COPILER_MACROS_H_ */
