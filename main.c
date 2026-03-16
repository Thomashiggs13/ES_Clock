#include "main.h"

/*-----------------------------------------------------------------------------
 * File: main.c
 *
 * Description:
 * This application runs on the MSP430FR4133 and displays a 24-hour clock on
 * the integrated segmented LCD.
 *
 * Display format:
 *   HH:MM DD
 *
 * Where:
 *   - HH is the hour (00 to 23)
 *   - MM is the minute (00 to 59)
 *   - DD is a two-character weekday abbreviation:
 *       Mo, Tu, We, Th, Fr, Sa, Su
 *
 * The application uses:
 *   - XT1 (32.768 kHz crystal) as the RTC clock source
 *   - RTC modulo mode to generate a 1 second interrupt
 *   - LCD_E peripheral to drive the on-board segmented LCD
 *   - backup memory to retain time and weekday variables
 *
 * Three push buttons are configured with interrupts for future use:
 *   - P1.2
 *   - P1.7
 *   - P2.6
 *
 * At present, these button interrupts are implemented but intentionally do
 * nothing beyond clearing the interrupt flag.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Seven-segment digit patterns for numeric display
 *---------------------------------------------------------------------------*/
static const uint8_t g_digit_map[10] =
{
    0xFC,   /* 0 */
    0x60,   /* 1 */
    0xDB,   /* 2 */
    0xF3,   /* 3 */
    0x67,   /* 4 */
    0xB7,   /* 5 */
    0xBF,   /* 6 */
    0xE4,   /* 7 */
    0xFF,   /* 8 */
    0xF7    /* 9 */
};

/*-----------------------------------------------------------------------------
 * Weekday glyph definitions
 *---------------------------------------------------------------------------*/
static const glyph_t GLYPH_M = { SEG_B | SEG_C | SEG_E | SEG_F, SEG_H | SEG_K };
static const glyph_t GLYPH_T = { SEG_A, SEG_J | SEG_P };
static const glyph_t GLYPH_W = { SEG_B | SEG_C | SEG_E | SEG_F, SEG_Q | SEG_N };
static const glyph_t GLYPH_F = { SEG_A | SEG_F | SEG_E | SEG_G, 0 };
static const glyph_t GLYPH_S = { SEG_A | SEG_C | SEG_D | SEG_F | SEG_G | SEG_M, 0 };

static const glyph_t GLYPH_o = { SEG_C | SEG_D | SEG_E | SEG_G | SEG_M, 0 };
static const glyph_t GLYPH_u = { SEG_C | SEG_D | SEG_E, 0 };
static const glyph_t GLYPH_e = { SEG_A | SEG_D | SEG_E | SEG_F | SEG_G | SEG_M, 0 };
static const glyph_t GLYPH_h = { SEG_C | SEG_E | SEG_F | SEG_G | SEG_M, 0 };
static const glyph_t GLYPH_r = { SEG_E | SEG_G, 0 };
static const glyph_t GLYPH_a = { SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_G | SEG_M, 0 };

/*-----------------------------------------------------------------------------
 * Backup memory variables
 *---------------------------------------------------------------------------*/
volatile unsigned char *Day     = &BAKMEM1_H;

/*-----------------------------------------------------------------------------
 * main
 *---------------------------------------------------------------------------*/
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    Init_GPIO();
    Init_Clock();

    /* Release GPIO pins from power-on high-impedance state. */
    PM5CTL0 &= ~LOCKLPM5;

    /* Set initial time and day. */
    *Seconds = 0;
    *Minutes = 0;
    *Hours   = 12;
    *Day     = 0;      /* Monday */

    Init_RTC();
    Init_LCD();
    Init_Buttons();

    Display_All();

    while (1)
    {
        __bis_SR_register(LPM3_bits | GIE);
        __no_operation();
    }
}

/*-----------------------------------------------------------------------------
 * Init_GPIO
 *
 * Configures all GPIO pins as outputs driven low by default to minimise
 * unwanted current draw. Pins required for buttons and peripherals are
 * reconfigured later in their dedicated initialisation functions.
 *---------------------------------------------------------------------------*/
void Init_GPIO(void)
{
    P1OUT = 0x00;
    P2OUT = 0x00;
    P3OUT = 0x00;
    P4OUT = 0x00;
    P5OUT = 0x00;
    P6OUT = 0x00;
    P7OUT = 0x00;
    P8OUT = 0x00;

    P1DIR = 0xFF;
    P2DIR = 0xFF;
    P3DIR = 0xFF;
    P4DIR = 0xFF;
    P5DIR = 0xFF;
    P6DIR = 0xFF;
    P7DIR = 0xFF;
    P8DIR = 0xFF;
}

/*-----------------------------------------------------------------------------
 * Init_Clock
 *
 * Configures the external 32.768 kHz crystal oscillator on XT1 and waits until
 * oscillator fault flags have cleared. XT1 is used as the RTC clock source.
 *---------------------------------------------------------------------------*/
void Init_Clock(void)
{
    P4SEL0 |= BIT1 | BIT2;

    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);
        SFRIFG1 &= ~OFIFG;
    }
    while (SFRIFG1 & OFIFG);

    CSCTL6 = (CSCTL6 & ~XT1DRIVE_3) | XT1DRIVE_2;
}

/*-----------------------------------------------------------------------------
 * Init_RTC
 *
 * Configures the RTC to use XT1 as its source clock and to generate an
 * interrupt once every second.
 *---------------------------------------------------------------------------*/
void Init_RTC(void)
{
    RTCCTL = RTCSS__XT1CLK | RTCIE;
    RTCMOD = 32768 - 1;
}

/*-----------------------------------------------------------------------------
 * Init_LCD
 *
 * Configures the LCD_E peripheral and enables the required LCD pins for the
 * segmented display. Only the colon between hours and minutes is enabled.
 *---------------------------------------------------------------------------*/
void Init_LCD(void)
{
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

    LCDMEM[7]  = 0x04;
    LCDMEM[11] = 0x00;
}

/*-----------------------------------------------------------------------------
 * Init_Buttons
 *
 * Configures buttons on P1.2, P1.7 and P2.6 as inputs with pull-up resistors.
 * Interrupts are enabled on the falling edge, assuming active-low buttons.
 *---------------------------------------------------------------------------*/
void Init_Buttons(void)
{
    /* Configure Port 1 buttons */
    P1DIR &= ~(BUTTON_P1_2 | BUTTON_P1_7);
    P1REN |=  (BUTTON_P1_2 | BUTTON_P1_7);
    P1OUT |=  (BUTTON_P1_2 | BUTTON_P1_7);
    P1IES |=  (BUTTON_P1_2 | BUTTON_P1_7);
    P1IFG &= ~(BUTTON_P1_2 | BUTTON_P1_7);
    P1IE  |=  (BUTTON_P1_2 | BUTTON_P1_7);

    /* Configure Port 2 button */
    P2DIR &= ~BUTTON_P2_6;
    P2REN |=  BUTTON_P2_6;
    P2OUT |=  BUTTON_P2_6;
    P2IES |=  BUTTON_P2_6;
    P2IFG &= ~BUTTON_P2_6;
    P2IE  |=  BUTTON_P2_6;
}

/*-----------------------------------------------------------------------------
 * LCD_WriteGlyph
 *
 * Writes a custom two-byte glyph to the specified LCD memory position.
 *---------------------------------------------------------------------------*/
void LCD_WriteGlyph(uint8_t position, glyph_t glyph)
{
    LCDMEM[position]     = glyph.lo;
    LCDMEM[position + 1] = glyph.hi;
}

/*-----------------------------------------------------------------------------
 * Display_Time
 *
 * Updates the LCD with the current hour and minute values in HH:MM format.
 *---------------------------------------------------------------------------*/
void Display_Time(void)
{
    LCDMEM[LCD_POS_HOUR_TENS] = g_digit_map[*Hours / 10];
    LCDMEM[LCD_POS_HOUR_ONES] = g_digit_map[*Hours % 10];
    LCDMEM[LCD_POS_MIN_TENS]  = g_digit_map[*Minutes / 10];
    LCDMEM[LCD_POS_MIN_ONES]  = g_digit_map[*Minutes % 10];
}

/*-----------------------------------------------------------------------------
 * Display_Day
 *
 * Updates the two weekday character positions on the LCD.
 *---------------------------------------------------------------------------*/
void Display_Day(void)
{
    switch (*Day)
    {
        case 0:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_M);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_o);
            break;

        case 1:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_T);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_u);
            break;

        case 2:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_W);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_e);
            break;

        case 3:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_T);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_h);
            break;

        case 4:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_F);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_r);
            break;

        case 5:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_S);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_a);
            break;

        default:
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_1, GLYPH_S);
            LCD_WriteGlyph(LCD_POS_DAY_CHAR_2, GLYPH_u);
            break;
    }
}

/*-----------------------------------------------------------------------------
 * Display_All
 *
 * Refreshes both the time and weekday sections of the LCD.
 *---------------------------------------------------------------------------*/
void Display_All(void)
{
    Display_Time();
    Display_Day();
}

/*-----------------------------------------------------------------------------
 * Increment_Day
 *
 * Increments the weekday value and wraps from Sunday back to Monday.
 *---------------------------------------------------------------------------*/
void Increment_Day(void)
{
    (*Day)++;

    if (*Day >= 7)
    {
        *Day = 0;
    }

    Display_Day();
}

/*-----------------------------------------------------------------------------
 * Increment_Time
 *
 * Increments the internal clock by one second. Minutes, hours and weekday are
 * advanced as rollover occurs. The LCD is updated when the displayed values
 * change.
 *---------------------------------------------------------------------------*/
void Increment_Time(void)
{
    (*Seconds)++;

    if (*Seconds >= 60)
    {
        *Seconds = 0;
        (*Minutes)++;

        if (*Minutes >= 60)
        {
            *Minutes = 0;
            (*Hours)++;

            if (*Hours >= 24)
            {
                *Hours = 0;
                Increment_Day();
            }
        }

        Display_Time();
    }
}

/*-----------------------------------------------------------------------------
 * RTC ISR
 *---------------------------------------------------------------------------*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(RTC_VECTOR))) RTC_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(RTCIV, RTCIV_RTCIF))
    {
        case RTCIV_NONE:
            break;

        case RTCIV_RTCIF:
            Increment_Time();
            __bic_SR_register_on_exit(LPM3_bits);
            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------------------------
 * Port 1 ISR
 *
 * Handles button interrupts on P1.2 and P1.7.
 * Currently these are placeholders and perform no action.
 *---------------------------------------------------------------------------*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(PORT1_VECTOR))) PORT1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P1IFG & BUTTON_P1_2)
    {
        P1IFG &= ~BUTTON_P1_2;
    }

    if (P1IFG & BUTTON_P1_7)
    {
        P1IFG &= ~BUTTON_P1_7;
    }

    __bic_SR_register_on_exit(LPM3_bits);
}

/*-----------------------------------------------------------------------------
 * Port 2 ISR
 *
 * Handles button interrupts on P2.6.
 * Currently this is a placeholder and performs no action.
 *---------------------------------------------------------------------------*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(PORT2_VECTOR))) PORT2_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P2IFG & BUTTON_P2_6)
    {
        P2IFG &= ~BUTTON_P2_6;
    }

    __bic_SR_register_on_exit(LPM3_bits);
}