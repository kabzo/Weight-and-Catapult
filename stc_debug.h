/*
 * debug.h
 *
 *  Created on: 03.12.2013
 *      Author: Andi
 */

#ifndef DEBUG_H_
#define DEBUG_H_


#define LED_ONBOARD_PORT 0
#define LED_ONBOARD_PIN 22


// --- Funktionsdeklerationen ---
void delay_ms(uint32_t i);
void delay_us(uint32_t i);
void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
void blink(uint8_t times);

#endif /* DEBUG_H_ */
