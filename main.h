#ifndef __MAIN_H
#define __MAIN_H

// --- Included Headerfiles ------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
// --- LPC17Xxx - headerfiles ---

#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_pwm.h"
#include "lpc17xx_rit.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_qei.h"

// --- Spaceteam - headerfiles ---

#include "stc_debug.h"
#include "stc_io.h"

// Typedefs und Structs
/*typedef struct bsp_s
{
	unsigned int bsp_var :1;
} bsp_t;

*/

// --- Globale Variablen ---
extern int global;


#define EINT0		0x00000001
#define EINT1		0x00000002
#define EINT2		0x00000004
#define EINT3		0x00000008

#define EINT0_EDGE	0x00000001
#define EINT1_EDGE	0x00000002
#define EINT2_EDGE	0x00000004
#define EINT3_EDGE	0x00000008

#define EINT0_RISING	0x00000001
#define EINT1_RISING	0x00000002
#define EINT2_RISING	0x00000004
#define EINT3_RISING	0x00000008

#define _ADC_INT			ADC_ADINTEN2
#define _ADC_CHANNEL		ADC_CHANNEL_2

#define IN 0
#define OUT_UP 1
#define OUT_DOWN -1

#define UP 1
#define DOWN 0


// --- Funktionsdeklerationen ---


#endif


