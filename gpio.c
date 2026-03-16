#include "gpio.h"

void GPIO_Init(void)
{
    // LED1: P1.0
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;

    // LED2: P4.0
    P4OUT &= ~BIT0;
    P4DIR |= BIT0;

    // SW1: P1.2 (input with pull-up)
    P1DIR &= ~BIT2;
    P1REN |= BIT2;
    P1OUT |= BIT2;

    // SW2: P2.6 (input with pull-up)
    P2DIR &= ~BIT6;
    P2REN |= BIT6;
    P2OUT |= BIT6;

    // Make sure port interrupts are disabled for this simple polling test.
    P1IE = 0;
    P2IE = 0;
    P1IFG = 0;
    P2IFG = 0;
}
