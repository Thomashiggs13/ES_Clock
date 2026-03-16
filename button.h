#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdint.h>

// Button pin definitions
#define START_STOP_BUTTON 0x04  // P1.2
#define RESET_BUTTON      0x40  // P2.6

// Button state tracking
extern volatile unsigned int btnTime[2];
extern volatile unsigned char btnPressed[2];

// Function prototypes
void Button_Init(void);
void Button_Handler(unsigned char curr, unsigned char *last, int index, void (*action)(void));

// Simple polling API (used by the button/LED test main.c)
uint8_t Button_WasPressed_SW1(void);
uint8_t Button_WasPressed_SW2(void);

#ifndef MAX_LAPS
#define MAX_LAPS 20
#endif

extern volatile unsigned int lapTimes[MAX_LAPS];
extern volatile unsigned int lapCount;

void Button_StoreLapTime(unsigned int totalSeconds);

#endif /* BUTTON_H_ */
