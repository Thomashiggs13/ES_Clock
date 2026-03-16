#include "rtc.h"
#include "time.h"

#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    extern volatile int running;
    
    switch(__even_in_range(RTCIV, RTCIV_RTCIF))
    {
        case RTCIV_NONE:
            break;
        case RTCIV_RTCIF:
            if(running)
                Time_Increment();
            break;
        default:
            break;
    }
}

void RTC_Init(void)
{
    // Configure XT1 oscillator
    P4SEL0 |= BIT1 | BIT2;
    do {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);
    CSCTL6 = (CSCTL6 & ~(XT1DRIVE_3)) | XT1DRIVE_2;

    PM5CTL0 &= ~LOCKLPM5;

    // Configure RTC
    RTCCTL |= RTCSS__XT1CLK | RTCIE;
    RTCMOD = 32768;
    
    // Timer_A0: 1ms for button debounce
    TA0CCTL0 = CCIE;
    TA0CCR0 = 1000 - 1;
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;
}
