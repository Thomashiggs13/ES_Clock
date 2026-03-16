#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdint.h>

// Button pin definitions
#define START_STOP_BUTTON 0x04  // P1.2
#define RESET_BUTTON      0x40  // P2.6

// Simple button API for polling with debounce.
void Button_Init(void);
uint8_t Button_WasPressed_SW1(void);
uint8_t Button_WasPressed_SW2(void);

#endif /* BUTTON_H_ */
