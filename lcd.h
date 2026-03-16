#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>

// LCD position definitions
#define LCD_POS1 4   // Hours tens
#define LCD_POS2 6   // Hours ones
#define LCD_POS3 8   // Minutes tens
#define LCD_POS4 10  // Minutes ones
#define LCD_POS5 2   // Seconds tens
#define LCD_POS6 18  // Seconds ones

// Function prototypes
void LCD_Init(void);
void LCD_Update(void);
unsigned char LCD_GetDigit(unsigned char digit);

#endif /* LCD_H_ */
