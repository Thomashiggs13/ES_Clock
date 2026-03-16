#ifndef TIME_H_
#define TIME_H_

#include <msp430.h>

// Time storage using backup memory
extern volatile unsigned char *Seconds;
extern volatile unsigned char *Minutes;
extern volatile unsigned char *Hours;

extern volatile int running;
extern volatile unsigned char freezeDisplay;

// Function prototypes
void Time_Init(void);
void Time_Increment(void);
void Time_Reset(void);
void Lap_Display(unsigned int h, unsigned int m, unsigned int s);

#endif /* TIME_H_ */
