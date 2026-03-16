#include "lcd.h"

// Digit patterns for 7-segment display
const char digit[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE4, 0xFF, 0xF7};

unsigned char LCD_GetDigit(unsigned char digit_idx)
{
    if(digit_idx < 10)
        return digit[digit_idx];
    return 0x00;
}

void LCD_Init(void)
{
    // LCD setup
    SYSCFG2 |= LCDPCTL;
    LCDPCTL0 = 0xFFFF;
    LCDPCTL1 = 0x07FF;
    LCDPCTL2 = 0x00F0;
    
    LCDCTL0 = LCDSSEL_0 | LCDDIV_7;
    LCDVCTL = LCDCPEN | LCDREFEN | VLCD_6 |
              (LCDCPFSEL0 | LCDCPFSEL1 | LCDCPFSEL2 | LCDCPFSEL3);
    LCDMEMCTL |= LCDCLRM;
    
    LCDCSSEL0 = 0x000F;
    LCDCSSEL1 = 0x0000;
    LCDCSSEL2 = 0x0000;
    
    LCDM0 = 0x21;
    LCDM1 = 0x84;
    
    LCDCTL0 |= LCD4MUX | LCDON;
}

void LCD_Update(void)
{
    extern volatile unsigned char *Seconds;
    extern volatile unsigned char *Minutes;
    extern volatile unsigned char *Hours;
    
    LCDMEM[LCD_POS1] = digit[(*Hours)/10];
    LCDMEM[LCD_POS2] = digit[(*Hours)%10];
    LCDMEM[LCD_POS3] = digit[(*Minutes)/10];
    LCDMEM[LCD_POS4] = digit[(*Minutes)%10];
    LCDMEM[LCD_POS5] = digit[(*Seconds)/10];
    LCDMEM[LCD_POS6] = digit[(*Seconds)%10];
    LCDMEM[7]  = 0x04;
    LCDMEM[11] = 0x04;
}
