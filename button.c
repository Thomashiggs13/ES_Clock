#include "button.h"

static uint8_t sw1_last = 1; // 1 = released (pull-up), 0 = pressed
static uint8_t sw2_last = 1;

static void debounce_delay(void)
{
    // ~10-20ms - simple test.
    __delay_cycles(16000);
}

void Button_Init(void)
{
    // SW1: P1.2 input with pull-up
    P1DIR &= ~START_STOP_BUTTON;
    P1REN |= START_STOP_BUTTON;
    P1OUT |= START_STOP_BUTTON;

    // SW2: P2.6 input with pull-up
    P2DIR &= ~RESET_BUTTON;
    P2REN |= RESET_BUTTON;
    P2OUT |= RESET_BUTTON;

    sw1_last = 1;
    sw2_last = 1;
}

uint8_t Button_WasPressed_SW1(void)
{
    uint8_t sw1_now = (P1IN & START_STOP_BUTTON) ? 1 : 0;

    if ((sw1_now == 0) && (sw1_last == 1))
    {
        debounce_delay();
        if ((P1IN & START_STOP_BUTTON) == 0)
        {
            sw1_last = 0;
            return 1;
        }
    }

    if (sw1_now != 0)
    {
        sw1_last = 1;
    }

    return 0;
}

uint8_t Button_WasPressed_SW2(void)
{
    uint8_t sw2_now = (P2IN & RESET_BUTTON) ? 1 : 0;

    if ((sw2_now == 0) && (sw2_last == 1))
    {
        debounce_delay();
        if ((P2IN & RESET_BUTTON) == 0)
        {
            sw2_last = 0;
            return 1;
        }
    }

    if (sw2_now != 0)
    {
        sw2_last = 1;
    }

    return 0;
}
