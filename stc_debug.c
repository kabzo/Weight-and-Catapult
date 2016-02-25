/*
 * debugging.c
 *
 *  Created on: 03.12.2013
 *      Author: Andi
 */
#include "main.h"

void delay_ms(uint32_t i)
{
	uint32_t j = 0;
	i *= SystemCoreClock / 12500;	// Grob empirisch ermittelter Wert für ca 1 ms

	for (j = 0; j < i; j++)
		;
}

void delay_us(uint32_t i)
{
	uint32_t j = 0;
	i *= SystemCoreClock / 12500000;

	for (j = 0; j < i; j++)
		;
}

void led_init(void)	// LED als Ausgang setzen
{
	PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum = LED_ONBOARD_PORT;
	PinCfg.Pinnum = LED_ONBOARD_PIN;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(LED_ONBOARD_PORT, 1 << LED_ONBOARD_PIN, 1);
	GPIO_ClearValue(LED_ONBOARD_PORT, 1 << LED_ONBOARD_PIN);
}

void led_on(void)	// Onboard LED
{
	GPIO_SetValue(LED_ONBOARD_PORT, (1 << LED_ONBOARD_PIN));
}

void led_off(void)	// Onboard LED
{
	GPIO_ClearValue(LED_ONBOARD_PORT, (1 << LED_ONBOARD_PIN));
}

void led_toggle(void)	// Toggles the LED
{
	static uint8_t toggle = 0;

	if(toggle)
	{
		led_off();
		toggle = 0;
	}
	else
	{
		led_on();
		toggle = 1;
	}
}

void blink(uint8_t times)
{
	for (; times > 0; times--)
	{
		led_on();
		delay_ms(100);
		led_off();
		delay_ms(100);
	}

}


