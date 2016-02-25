/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
//#include "kalman.h"

#include "main.h"
#include "init.h"
#include "temp.h"

//#define DEBUG_WEIGHT //uncoment to get some Debug messages

/**
 ********************************* WEIGHT PARAMETERS *********************************
 */
float duty_one_gram = 6;
float referenc_reading = 141;
float reference_time = 255326.000000;
float time_one_gram = 15000.000000;
int8_t calibrate = false;
float calibration_temp = 17.3;
float last_meassured_weight = 6;
/**
 ********************************* WEIGHT PARAMETERS *********************************
 */

/**
 ********************************* INTERRUPTS *********************************
 */
volatile int8_t toogled_wall = OUT_DOWN;
volatile int8_t wall_up_down = DOWN;
volatile int8_t down_wall_passed = 0;
volatile int8_t up_wall_passed = 0;
volatile int8_t up_wall_passed_fallend = 0;

volatile uint64_t time_wall_down = 0;
volatile uint64_t time_wall_up = 0;

volatile uint64_t tick = 0;
//******************************** BUTTONS *************************************//
volatile uint8_t button_pressed_1 = false;
volatile uint8_t button_pressed_2 = false;
/**
 ********************************* INTERRUPTS *********************************
 */

/**
 ********************************* STORE TEMPERATUR *********************************
 */
#define MAX_TEMP 19
#define COUNT 2000
float temp[COUNT];
long index = -1;
/**
 ********************************* STORE TEMPERATUR *********************************
 */

/**
 ********************************* HARD CODED DUTY SHOOT *********************************
 */
int hard_coded_table = 0;
uint16_t shoot_duty_table[] =
{ 315, 315, 0, 0, 450, 0, 0, 0, 0, 560, 0, 0, 0, 0, 0, 0, 0, 0, 0, 800, 780 };

/**
 ********************************* HARD CODED DUTY SHOOT *********************************
 */

/**
 * Get duty necesary for specific shootingdistance based on meassured weight
 */
uint16_t get_duty_shoot(float weight)
{
	uint16_t duty_rounded = 0;
	if (hard_coded_table)
	{
		int index = roundNo(weight) - 1;
		duty_rounded = shoot_duty_table[index];
	}
	else
	{
		float duty = 24.9792 * weight + 290.15;
		if (roundNo(weight) == 6)
		{
			duty = 420;
		}
		duty_rounded = roundNo(duty);
	}

	return duty_rounded;

}

/**
 * Lightwall interrupts
 */
void EINT3_IRQHandler(void)
{
	int pin1 = 6;
	int pin2 = 7;
	if (LPC_GPIOINT ->IO2IntStatR & (1 << pin1))  //down lightwall
	{
		if (down_wall_passed == 0)
		{
			time_wall_down = LPC_TIM0 ->TC;
			TIM_Cmd(LPC_TIM0, ENABLE);

			down_wall_passed = 1;
		}

		LPC_GPIOINT ->IO2IntClr = (1 << pin1);

	}

	if (LPC_GPIOINT ->IO2IntStatR & (1 << pin2)) //up lightwall
	{
		if (up_wall_passed == 0)
		{
			time_wall_up = LPC_TIM0 ->TC;
			up_wall_passed = 1;
			TIM_Cmd(LPC_TIM0, DISABLE);

		}
		LPC_GPIOINT ->IO2IntClr = (1 << pin2);

	}

	if (LPC_GPIOINT ->IO2IntStatF & (1 << pin2))
	{
		up_wall_passed_fallend = 1;
		LPC_GPIOINT ->IO2IntClr = (1 << pin2);

	}

	if (LPC_GPIOINT ->IO2IntStatF & (1 << pin1)) //falling interrupt down lightwall
	{
		LPC_GPIOINT ->IO2IntClr = (1 << pin1);
	}

	LPC_GPIOINT ->IO0IntClr = 0xFFFFFFFF;
	LPC_GPIOINT ->IO2IntClr = 0xFFFFFFFF;
//	LPC_SC ->EXTINT = EINT3; /* clear interrupt */

}

/**
 * Button 1 interrupt
 */
void EINT0_IRQHandler(void)
{
	LPC_SC ->EXTINT = EINT0; /* clear interrupt */
#ifdef DEBUG_WEIGHT
	printf("Button1 = %d\n", button_pressed_1);
#endif
	if (button_pressed_1 == false)
	{
		led_toggle();
		button_pressed_1 = true;
	}
}

/**
 * Button 2 interrupt
 */
void EINT1_IRQHandler(void)
{
	LPC_SC ->EXTINT = EINT1; /* clear interrupt */
#ifdef DEBUG_WEIGHT
	printf("Button2 = %d\n", button_pressed_2);
#endif
	if (button_pressed_2 == false)
	{
		led_toggle();
		button_pressed_2 = true;
	}

}

/**
 * Set specific duty on pwm output
 * @param - specific duty
 */
uint8_t set_duty(uint32_t duty)
{
	uint16_t step_delay = 25;
	PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty, PWM_MATCH_UPDATE_NOW);
#ifdef DEBUG_WEIGHT
	temp[index++] = compute_temperatur();
#endif
	delay_ms(step_delay);
	return true;
}

/**
 * Wait for specific temperature.
 * @param - wanted temperatur
 */bool wait_for_temperatur(float temp)
{
	float te = compute_temperatur();

	while (te > (temp - 3))
	{
		te = compute_temperatur();
		set_duty(0);
		printf("%f\n", te);

	}
	do
	{
		te = compute_temperatur();

		set_duty((int) period - 10);

		printf("%f\n", te);

	} while (!((te > temp) && (te < (temp + 1))));

	return true;
}

/**
 * Check if temperature is not too hight.
 * @param temp - Threashold temperatur
 */bool check_temperatur(float temp)
{
	printf("Checking temp: ");
	float te = compute_temperatur();
	int8_t temp_too_heigh = false;
	while (te > temp)
	{
		te = compute_temperatur();
		set_duty(0);
		printf(" Too Hot: %g\n", te);
		delay_ms(1000);
		temp_too_heigh = true;
	}
	printf("TEMP OK, %g \n", te);
	return temp_too_heigh;
}

/**
 * Measure time between two lightbariers. Afterwards computes weight and duty cycle for shooting action
 * @param shoot - 0 if we meassure just reference weight
 * 				  1 if we want also shoot and compute weight
 */
float measure_time_weight(int8_t shoot)
{
	float weight;
	uint32_t shoot_duty = 0;

	if (check_temperatur(MAX_TEMP)) // check if temperatur is not too high
		return 0;

	TIM_Cmd(LPC_TIM0, DISABLE);
	TIM_ResetCounter(LPC_TIM0 );

	PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);
	delay_ms(500);
	if (shoot)
	{
		printf("Waiting for up wall time and shooting, ");
	}
	else
	{
		printf("Waiting for up wall time ");
	}
	uint32_t duty_measure = (uint32_t) roundNo((float) period * 0.61);

	up_wall_passed = 0;
	down_wall_passed = 0;

	PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty_measure, PWM_MATCH_UPDATE_NOW);
	while (up_wall_passed == 0)
		;
	unsigned long measured_tick = time_wall_up - time_wall_down;

	//shooting procedure
	if (shoot == 1)
	{
		weight = (measured_tick - reference_time) / time_one_gram; //compute weight
		shoot_duty = get_duty_shoot(weight);
		PWM_MatchUpdate(LPC_PWM1, pwm_channel, shoot_duty, PWM_MATCH_UPDATE_NOW);
		delay_ms(100);
	}

	PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);

	up_wall_passed = 0;
	down_wall_passed = 0;
	LPC_SC ->EXTINT = EINT0; /* clear interrupt */
	LPC_SC ->EXTINT = EINT1; /* clear interrupt */

	printf(", tick %lu , shoot_duty: %ld ,hard c. table:%d, temp:%g \n", measured_tick, shoot_duty, hard_coded_table, compute_temperatur());

	if (!shoot)
	{
		weight = measured_tick;
		calibration_temp = compute_temperatur();
	}

	return weight;
}

/**
 * Calibration of measurement units (reference_time, time_one_gram ...)
 */
void calibration()
{

	reference_time = measure_time_weight(0);
	if (reference_time == 0)
	{
		calibrate = true;
		return;
	}

	button_pressed_2 = false;
	printf("reference_time  %f \n", reference_time);
	int ref_grams = 5;
	printf("ADD %d GRAM AND PRESS BUTTON 2\n", ref_grams);
	while (button_pressed_2 == false)
		;
	printf("MEASSURING\n");
	uint64_t time_one_gram_sum = measure_time_weight(0);
	if (time_one_gram_sum == 0)
	{
		calibrate = true;
		return;
	}
	printf("time_one_gram + referenc  %d \n", time_one_gram_sum);
	time_one_gram = (time_one_gram_sum - reference_time) / ref_grams;
	printf("time_one_gram  %f \n", time_one_gram);
	calibrate = false;
}

/**
 * Checks if button_2 was pressed. Launch calibration or measurement action
 */
void measure_button()
{
	if (button_pressed_2)
	{
		if (calibrate)
		{
			printf("calibration necesary\n");
			calibration();
		}
		else
		{
			last_meassured_weight = measure_time_weight(1);
			printf("WEIGHT  %g \n", roundNo_f(last_meassured_weight, 2));
			delay_ms(2000);
		}
		button_pressed_2 = false;
	}
}

/**
 * Checks if button_1 was pressed. Start catapult action
 */
void catapult_button()
{
	if (button_pressed_1)
	{
		button_pressed_1 = false;
//		if (check_temperatur(MAX_TEMP))
//			return;
//		printf("shoooooting \n");
//		uint32_t duty = get_duty_shoot(last_meassured_weight);
//		PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty, PWM_MATCH_UPDATE_NOW);
//		delay_ms(300);
//		PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);
		LPC_SC ->EXTINT = EINT0; /* clear interrupt */
		hard_coded_table = !hard_coded_table;
		printf("hardcoded_table used:%d \n", hard_coded_table);

	}
}

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();

	led_init();
	init_interupt();
	init_pwm(pwm_pin_num, pwm_channel, period, 0);
	init_coil();
	init_timer();
	init_ADC();
	init_timer();

#ifdef DEBUG_WEIGHT
	int a;
	for (a = 0; a < COUNT; a++)
	{
		temp[a] = 0;
	}
#endif

	printf("Weight Meassure. clibration: %d \n Press Button 1 to calibrate \n", calibrate);
	delay_ms(5000);
	if (button_pressed_1)
	{
		calibrate = true;
		button_pressed_1 = false;
		printf("Device will be calibrated \n");
	}
	else
	{
		printf("Stock calibration will be used bei Temperatur:%g \n",calibration_temp);
		calibrate = false;
	}

	while (1)
	{
		measure_button();		//Funktion zum Start der Messung
		delay_ms(5);
		catapult_button();

		check_temperatur(MAX_TEMP);
		delay_ms(2000);
//		printf("%g\n",compute_temperatur());
	}
	return 0;
}
