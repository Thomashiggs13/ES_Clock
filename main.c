#include <msp430.h>
#include "gpio.h"
#include "button.h"

// LED pins
#define LED1 BIT0  // P1.0
#define LED2 BIT0  // P4.0

// These are required by the TI HAL IO module (still part of the CCS project).
// This simplified button test uses polling and does NOT enable port interrupts,
// so these callbacks won't run unless you enable the HAL button interrupts.
void ButtonCallback_SW1(void)
{
    P1OUT ^= LED1;
}

void ButtonCallback_SW2(void)
{
    P4OUT ^= LED2;
}

// Required by the TI HAL timer module (still part of the CCS project build).
// This simplified polling test does not use the HAL timer, so this is a no-op.
void TimerCallback(void)
{
}

// Main entry point - GPIO and Button test
int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Unlock GPIO pins from LPM5
    PM5CTL0 &= ~LOCKLPM5;

    // Initialize GPIO + buttons (polling, no interrupts)
    GPIO_Init();
    Button_Init();

    while(1)
    {
        if (Button_WasPressed_SW1())
        {
            P1OUT ^= LED1;
        }

        if (Button_WasPressed_SW2())
        {
            P4OUT ^= LED2;
        }

        __no_operation();
    }
}

