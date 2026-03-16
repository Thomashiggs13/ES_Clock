#include "button.h"

// Button state tracking
volatile unsigned int btnTime[2]   = {0, 0};
volatile unsigned char btnPressed[2] = {0, 0};

// Lap time storage
#ifndef MAX_LAPS
#define MAX_LAPS 20
#endif
volatile unsigned int lapTimes[MAX_LAPS] = {0};
volatile unsigned int lapCount = 0;

// Polling-based edge detection for the simple button test
static uint8_t sw1_last = 1; // 1=released (pull-up), 0=pressed
static uint8_t sw2_last = 1;

static void debounce_delay(void)
{
    // ~10-20ms depending on clock; good enough for a simple press test.
    __delay_cycles(16000);
}

void Button_Init(void)
{
    // Configure START/STOP button (P1.2)
    P1DIR &= ~START_STOP_BUTTON;
    P1REN |= START_STOP_BUTTON;
    P1OUT |= START_STOP_BUTTON;
    
    // Configure RESET button (P2.6)
    P2DIR &= ~RESET_BUTTON;
    P2REN |= RESET_BUTTON;
    P2OUT |= RESET_BUTTON;

    sw1_last = 1;
    sw2_last = 1;
}

uint8_t Button_WasPressed_SW1(void)
{
    uint8_t sw1_now = (P1IN & START_STOP_BUTTON) ? 1U : 0U;

    if ((sw1_now == 0U) && (sw1_last == 1U))
    {
        debounce_delay();
        if ((P1IN & START_STOP_BUTTON) == 0U)
        {
            sw1_last = 0U;
            return 1U;
        }
    }

    if (sw1_now != 0U)
    {
        sw1_last = 1U;
    }

    return 0U;
}

uint8_t Button_WasPressed_SW2(void)
{
    uint8_t sw2_now = (P2IN & RESET_BUTTON) ? 1U : 0U;

    if ((sw2_now == 0U) && (sw2_last == 1U))
    {
        debounce_delay();
        if ((P2IN & RESET_BUTTON) == 0U)
        {
            sw2_last = 0U;
            return 1U;
        }
    }

    if (sw2_now != 0U)
    {
        sw2_last = 1U;
    }

    return 0U;
}

void Button_Handler(unsigned char curr, unsigned char *last, int index, void (*action)(void))   //Action is a function pointer to the action to execute on button press
{
    if(curr && !(*last)) {      
        btnTime[index] = 0;
        btnPressed[index] = 1;
    }
    if(!curr && *last) {
        btnPressed[index] = 0;
        if(btnTime[index] >= 50)
            action();
        btnTime[index] = 0;
    }
    *last = curr;
}

void Button_StoreLapTime(unsigned int totalSeconds)
{
    if(lapCount < MAX_LAPS) {
        lapTimes[lapCount++] = totalSeconds;
    }
}
