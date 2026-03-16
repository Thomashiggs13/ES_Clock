#ifndef RTC_H_
#define RTC_H_

#include <msp430.h>

// Function prototype
void RTC_Init(void);

// Call from your application's RTC ISR if the application owns RTC_VECTOR.
void RTC_HandleInterrupt(void);

#endif /* RTC_H_ */
