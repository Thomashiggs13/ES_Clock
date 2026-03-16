#include "time.h"
#include "lcd.h"

// Time storage pointers pointing to backup memory
volatile unsigned char *Seconds = &BAKMEM0_L;
volatile unsigned char *Minutes = &BAKMEM0_H;
volatile unsigned char *Hours   = &BAKMEM1_L;

volatile int running = 0;

void Time_Init(void)
{
    *Seconds = *Minutes = *Hours = 10;
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

    // Only update digits that changed
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

void Time_Reset(void)
{
    *Seconds = 0;
    *Minutes = 0;
    *Hours = 0;
    running = 0;
    LCD_Update();
}
