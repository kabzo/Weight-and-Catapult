/*
 * stc_io.c
 *
 *  Created on: 21.06.2013
 *      Author: Andi
 */

#include "main.h"

bool init_io(void)
{
	PINSEL_CFG_Type PinCfg;
	// --- Outputs ---

	PinCfg.Portnum = OUTPUT_PORT;
	PinCfg.Pinnum = OUTPUT_PIN;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(OUTPUT_PORT, 1 << OUTPUT_PIN, 1);
	GPIO_ClearValue(OUTPUT_PORT, 1 << OUTPUT_PIN);

	// --- Inputs ---

	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfg.Pinnum = INPUT_PIN;
	PinCfg.Portnum = INPUT_PORT;
	PinCfg.Funcnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(INPUT_PORT, 1 << INPUT_PIN, 0); // Set as input


	return true;
}

bool pin_check(void)
{
	return (GPIO_ReadValue(INPUT_PORT) & (1 << INPUT_PIN));
}



