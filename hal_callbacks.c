#include <msp430.h>

// TI HAL modules (src/MSP430_HAL/HAL_IO_FR413x.c and HAL_Timer_FR413x.c)
// declare these as extern and will fail to link if your application doesn't
// provide them.
//
// Provide minimal default implementations so the project links and you can
// debug, even when using a different main application.

void ButtonCallback_SW1(void)
{
}

void ButtonCallback_SW2(void)
{
}

void TimerCallback(void)
{
}
