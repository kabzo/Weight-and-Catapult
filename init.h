/*
 * init.h
 *
 *  Created on: 02.06.2015
 *      Author: MT-LAB
 */

#ifndef INIT_H_
#define INIT_H_

void init_timer()
{
	TIM_TIMERCFG_Type timecfg;
	timecfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	timecfg.PrescaleValue = 1;

//	timecfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
//	timecfg.PrescaleValue = 1;
//	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &timecfg);

	TIM_MATCHCFG_Type timmatchcfg;

	timmatchcfg.MatchChannel = 0;
	timmatchcfg.IntOnMatch = DISABLE;
	timmatchcfg.StopOnMatch = DISABLE;
	timmatchcfg.ResetOnMatch = DISABLE;
	timmatchcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timecfg);
	TIM_ConfigMatch(LPC_TIM0, &timmatchcfg);
	TIM_Cmd(LPC_TIM0, ENABLE);

//	TIM_UpdateMatchValue(LPC_TIM0, 0, 1);
//	NVIC_EnableIRQ(TIMER0_IRQn);

}

void init_interupt()
{
	LPC_PINCON->PINSEL4 = 0x00500000; //0x00500000;
	LPC_GPIOINT->IO2IntEnF = (1 << 10) | (1 << 11); //0x200; /* Port2.10 is falling edge. */
	LPC_SC->EXTMODE |= (EINT0_EDGE | EINT1_EDGE); /* INT0 edge trigger */

	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);

//LPC_GPIOINT ->IO2IntEnF |= (1 << 10); //0x200; /* Port2.11 is falling edge. */
//	LPC_SC ->EXTMODE |= EINT1_EDGE;
//
//	LPC_PINCON ->PINSEL4 |= (1 << 22);
//	LPC_GPIOINT ->IO2IntEnF = (1 << 10); /* Port2.13 is falling edge. */
//	LPC_SC ->EXTMODE |= EINT1_EDGE; /* INT0 edge trigger */
//
//	NVIC_EnableIRQ(EINT1_IRQn);

	//Enable interrupt
	LPC_GPIOINT->IO2IntEnR |= (1 << 6);	// Rising
	LPC_GPIOINT->IO2IntEnF |= (1 << 6);	// Falling
	LPC_GPIOINT->IO2IntEnR |= (1 << 7);	// Rising
	LPC_GPIOINT->IO2IntEnF |= (1 << 7);	// Falling
	NVIC_EnableIRQ(EINT3_IRQn);

}

uint8_t pwm_channel = 1;
uint32_t period = 800;
uint8_t pwm_pin_num = 0;

int init_pwm(int pwm_pin_num, int pwm_channel, uint32_t period, uint32_t duty_cycle)
{

	PWM_TIMERCFG_Type PWMCfgDat;
	PWM_MATCHCFG_Type PWMMatchCfgDat;
	PINSEL_CFG_Type PinCfg;

	/* PWM block section -------------------------------------------- */
	/* Initialize PWM peripheral, timer mode
	 * PWM prescale value = 1 (absolute value - tick value) */
	PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_USVAL;
	PWMCfgDat.PrescaleValue = 1;
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &PWMCfgDat);

	/*
	 * Initialize PWM pin connect
	 */
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = pwm_pin_num;
	PINSEL_ConfigPin(&PinCfg);

	/* Set match value for PWM match channel 0 = 300, update immediately */
	PWM_MatchUpdate(LPC_PWM1, 0, period, PWM_MATCH_UPDATE_NOW); //controll match in us -- period

	/* PWM Timer/Counter will be reset when channel 0 matching
	 * no interrupt when match
	 * no stop when match */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);

	/* Configure each PWM channel: --------------------------------------------- */
	/* - Single edge
	 * - PWM Duty on each PWM channel determined by
	 * the match on channel 0 to the match of that match channel.
	 * Example: PWM Duty on PWM channel 1 determined by
	 * the match on channel 0 to the match of match channel 1.
	 */

	/* Configure PWM channel edge option
	 * Note: PWM Channel 1 is in single mode as default state and
	 * can not be changed to double edge mode */

	/* Configure match value for each match channel */

	/* Set up match value */
	PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty_cycle, PWM_MATCH_UPDATE_NOW);
	/* Configure match option */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = pwm_channel;
	PWMMatchCfgDat.ResetOnMatch = DISABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
	/* Enable PWM Channel Output */
	PWM_ChannelCmd(LPC_PWM1, pwm_channel, ENABLE);

	/* Reset and Start counter */
	PWM_ResetCounter(LPC_PWM1);
	PWM_CounterCmd(LPC_PWM1, ENABLE);

	/* Start PWM now */
	PWM_Cmd(LPC_PWM1, ENABLE);

	return 1;
}

void init_coil()
{
	/**
	 * H-bridge is connected on port P1.18 and P1.19
	 */
	GPIO_SetDir(1, (1 << 18), 1);
	GPIO_SetDir(1, (1 << 19), 1);

	GPIO_SetValue(1, (1 << 18));
	GPIO_ClearValue(1, (1 << 19));
}

/**
 * Change direction of coil movement
 */
void set_direction(int dir)
{
	if (dir == 1)
	{
		GPIO_SetValue(1, (1 << 18));
		GPIO_ClearValue(1, (1 << 19));
	}
	if (dir == -1)
	{
		GPIO_SetValue(1, (1 << 19));
		GPIO_ClearValue(1, (1 << 18));
	}
}

int init_ADC(void)
{
	PINSEL_CFG_Type PinCfg;

	/*
	 * Init ADC pin connect
	 * AD0.2 on P0.25
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/* Configuration for ADC:
	 *  select: ADC channel 2 (if using MCB1700 board)
	 *  		ADC channel 5 (if using IAR-LPC1768 board)
	 *  ADC conversion rate = 200KHz
	 */
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC, _ADC_CHANNEL, ENABLE);

	//Start burst conversion
	ADC_BurstCmd(LPC_ADC, ENABLE);

	return (0);
}

int roundNo(float num)
{

	return num < 0 ? num - 0.5 : num + 0.5;
}

float roundNo_f(float num, int pos)
{
	return (float) roundNo(num * pow(10, pos)) / pow(10, pos);
}

#endif /* INIT_H_ */
