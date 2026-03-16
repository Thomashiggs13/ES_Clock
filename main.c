#include <msp430.h>
#include "gpio.h"
#include "lcd.h"
#include "rtc.h"
#include "button.h"
#include "time.h"

// Button action functions
void toggleRunning(void)
{
    extern volatile int running;
    running ^= 1;
}

void resetTimer(void)
{
    //Time_Reset();
    //Lap_Display(); // remove: wrong signature
    __delay_cycles(2000000UL); // ~2s at 1 MHz MCLK; adjust if clock differs
}

void recordLap(void)
{
    unsigned int totalSeconds = ((unsigned int)(*Hours) * 3600U) +
                                ((unsigned int)(*Minutes) * 60U) +
                                (unsigned int)(*Seconds);
    Button_StoreLapTime(totalSeconds);

    unsigned int lapH = totalSeconds / 3600U;
    unsigned int lapM = (totalSeconds % 3600U) / 60U;
    unsigned int lapS = totalSeconds % 60U;

    freezeDisplay = 1;
    Lap_Display(lapH, lapM, lapS);   // shows and delays ~2s
    freezeDisplay = 0;
    LCD_Update();                    // restore live time
}

// Main entry point - Complete Clock System
int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    
    // Initialize all subsystems
    GPIO_Init();
    Button_Init();
    LCD_Init();
    Time_Init();
    RTC_Init();
    
    // Unlock GPIO pins from Low Power Mode 5
    PM5CTL0 &= ~LOCKLPM5;
    
    // Update display with initial time
    extern volatile unsigned char *Seconds;
    extern volatile unsigned char *Minutes;
    extern volatile unsigned char *Hours;
    
    LCDMEM[LCD_POS1] = LCD_GetDigit((*Hours)/10);
    LCDMEM[LCD_POS2] = LCD_GetDigit((*Hours)%10);
    LCDMEM[LCD_POS3] = LCD_GetDigit((*Minutes)/10);
    LCDMEM[LCD_POS4] = LCD_GetDigit((*Minutes)%10);
    LCDMEM[LCD_POS5] = LCD_GetDigit((*Seconds)/10);
    LCDMEM[LCD_POS6] = LCD_GetDigit((*Seconds)%10);
    LCDMEM[7]  = 0x04;  // Colon separator
    LCDMEM[11] = 0x04;  // Colon separator
    
    // Enable global interrupts
    __enable_interrupt();
    
    // Start the timer
    extern volatile int running;
    running = 0;
    
    // Main loop - handle buttons and wait for interrupts
    unsigned char btnLast[2] = {0, 0};
    
    while(1)
    {
        Button_Handler(!(P1IN & START_STOP_BUTTON), &btnLast[0], 0, toggleRunning);
        // P2.6 temporarily used for lap
        Button_Handler(!(P2IN & RESET_BUTTON),      &btnLast[1], 1, recordLap);
        __no_operation();
    }
}

// Timer_A0 ISR for button debounce timing
extern volatile unsigned int btnTime[2];
extern volatile unsigned char btnPressed[2];

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void)
{
    if(btnPressed[0]) btnTime[0]++;
    if(btnPressed[1]) btnTime[1]++;
}
