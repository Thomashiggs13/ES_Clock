#include <msp430.h>

// TI HAL modules (src/MSP430_HAL/HAL_IO_FR413x.c and HAL_Timer_FR413x.c)
// declare these as extern and will fail to link if your application doesn't
// provide them.
//
// Provide minimal default implementations so the project links and you can
// debug, even when using a different main application.

// Mark as weak defaults so real application implementations can override
// without causing duplicate symbol errors.
#if defined(__TI_COMPILER_VERSION__)
#pragma WEAK(ButtonCallback_SW1)
#pragma WEAK(ButtonCallback_SW2)
#pragma WEAK(TimerCallback)
#endif

#if defined(__GNUC__)
#define WEAK_ATTR __attribute__((weak))
#else
#define WEAK_ATTR
#endif

WEAK_ATTR void ButtonCallback_SW1(void)
{
}

WEAK_ATTR void ButtonCallback_SW2(void)
{
}

WEAK_ATTR void TimerCallback(void)
{
}

#undef WEAK_ATTR
