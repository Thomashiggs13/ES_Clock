#include "time.h"
#include "lcd.h"

// Time storage pointers pointing to backup memory
volatile unsigned char *Seconds = &BAKMEM0_L;
volatile unsigned char *Minutes = &BAKMEM0_H;
volatile unsigned char *Hours   = &BAKMEM1_L;

volatile int running = 0;
volatile unsigned char freezeDisplay = 0;
volatile unsigned char lapActive = 0;
volatile unsigned char lapHoldSeconds = 0;

void Time_Init(void)
{
    *Seconds = *Minutes = *Hours = 0;
}

static void Time_DisplayAll(void)
{
    // Full redraw of HH:MM:SS from current time values
    LCDMEM[LCD_POS1] = LCD_GetDigit((*Hours / 10U) % 10U);
    LCDMEM[LCD_POS2] = LCD_GetDigit((*Hours) % 10U);
    LCDMEM[LCD_POS3] = LCD_GetDigit((*Minutes / 10U) % 10U);
    LCDMEM[LCD_POS4] = LCD_GetDigit((*Minutes) % 10U);
    LCDMEM[LCD_POS5] = LCD_GetDigit((*Seconds / 10U) % 10U);
    LCDMEM[LCD_POS6] = LCD_GetDigit((*Seconds) % 10U);

    LCDMEM[7]  = 0x04;
    LCDMEM[11] = 0x04;

    LCD_Update();
}

void Time_Increment(void)
{
    unsigned char prevSec = *Seconds;
    unsigned char prevMin = *Minutes;
    unsigned char prevHour = *Hours;

    (*Seconds)++;
    if(*Seconds >= 60) { *Seconds = 0; (*Minutes)++; }
    if(*Minutes >= 60) { *Minutes = 0; (*Hours)++; }
    if(*Hours >= 24) *Hours = 0;

    // If lap display is active, count down and do not touch LCD digits
    if (lapActive) {
        if (lapHoldSeconds > 0) {
            lapHoldSeconds--;
        }
        if (lapHoldSeconds == 0) {
            lapActive = 0;
            freezeDisplay = 0;
            Time_DisplayAll(); // force a correct time redraw after being frozen
        }
        return;
    }

    // Only update digits that changed
    if(!freezeDisplay) {
        if(prevHour != *Hours) {
            if((*Hours)/10 != prevHour/10) LCDMEM[LCD_POS1] = LCD_GetDigit((*Hours)/10);
            if((*Hours)%10 != prevHour%10) LCDMEM[LCD_POS2] = LCD_GetDigit((*Hours)%10);
        }
        if(prevMin != *Minutes) {
            if((*Minutes)/10 != prevMin/10) LCDMEM[LCD_POS3] = LCD_GetDigit((*Minutes)/10);
            if((*Minutes)%10 != prevMin%10) LCDMEM[LCD_POS4] = LCD_GetDigit((*Minutes)%10);
        }
        if(prevSec != *Seconds) {
            if((*Seconds)/10 != prevSec/10) LCDMEM[LCD_POS5] = LCD_GetDigit((*Seconds)/10);
            if((*Seconds)%10 != prevSec%10) LCDMEM[LCD_POS6] = LCD_GetDigit((*Seconds)%10);
        }
    }
}

void Time_Reset(void)
{
    *Seconds = 0;
    *Minutes = 0;
    *Hours = 0;
    running = 0;
    LCD_Update();
}

void Lap_Display(unsigned int h, unsigned int m, unsigned int s)
{
    (void)h; // hours replaced by count

    static unsigned int lapPressCount = 0;
    lapPressCount++;

    // Activate lap display for 5 seconds (based on Time_Increment() tick)
    lapActive = 1;
    lapHoldSeconds = 5;
    freezeDisplay = 1;

    // Show count in place of HH (00-99)
    unsigned int cnt = (unsigned int)(lapPressCount % 100U);
    LCDMEM[LCD_POS1] = LCD_GetDigit((cnt / 10U) % 10U);
    LCDMEM[LCD_POS2] = LCD_GetDigit(cnt % 10U);

    // Show MM:SS snapshot passed in
    LCDMEM[LCD_POS3] = LCD_GetDigit((m / 10U) % 10U);
    LCDMEM[LCD_POS4] = LCD_GetDigit(m % 10U);
    LCDMEM[LCD_POS5] = LCD_GetDigit((s / 10U) % 10U);
    LCDMEM[LCD_POS6] = LCD_GetDigit(s % 10U);

    LCDMEM[7]  = 0x04;
    LCDMEM[11] = 0x04;
     __delay_cycles(5000000UL); // ~5s at 1 MHz MCLK
     
    LCD_Update(); 
}