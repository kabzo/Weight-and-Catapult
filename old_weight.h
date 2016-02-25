/*
 * old_weight.h
 *
 *  Created on: 11.06.2015
 *      Author: MT-LAB
 */

#ifndef OLD_WEIGHT_H_
#define OLD_WEIGHT_H_


uint32_t start_position(int pos)
{
	delay_ms(500);
	PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);

	int step = (int) pos / 4;
	int i = 0;
	for (i = 0; i < pos; i += step)
	{
		PWM_MatchUpdate(LPC_PWM1, pwm_channel, i, PWM_MATCH_UPDATE_NOW);
		delay_ms(100);

	}
	return pos;
}


float differenze = 0;
float get_ref_reading(int reading)
{
	wait_for_temperatur(40);

//	set_direction(1);

	set_duty(0);
	int step_size = 1;
	int i = 0;

	printf("***************get_ref_reading***********\n");

	uint32_t duty = (start_duty);

//	uint32_t duty_in_sum = 0, duty_out_sum = 0;
	uint32_t duty_in = 0, duty_out = 0;
	differenze = 0;
	for (i = 0; i < reading; i++)
	{
		toogled_wall = OUT_DOWN;
		while (toogled_wall == OUT_DOWN)
		{

			duty += step_size;
			set_duty(duty);

			duty_in = duty;
		}
//		duty_in_sum += duty_in;

//		while (toogled_wall == IN)
//		{
//			duty += step_size;
//			set_duty(duty);
//
//			duty_out = duty;
//		}
//		duty_out_sum += duty_out;

//		if (differenze == 0)
		differenze = 4;
//		duty -= duty_out - duty_in + 2;
		printf("*********: duty_out: %d, duty_in:%d , differenze: %f, toogled_wall:%d\n", duty_out, duty_in, differenze, toogled_wall);

//		duty-= differenz/2;
//		PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty, PWM_MATCH_UPDATE_NOW);
//		delay_ms(250);
//		duty-= differenz/2;
//		PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty, PWM_MATCH_UPDATE_NOW);
//		delay_ms(250);
//
		if (i != reading - 1)
		{
			temp[index++] = duty;
//			delay_ms(100);
//			int a;
//			for (a = 0; a <= (differenze); ++a)
//			{
//				set_duty(duty--);
//			}
//			delay_ms(100);
			duty = start_duty;
			set_duty(start_duty);
		}
	}

	PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);

	float ref_read = 0; //(float) (duty_out_sum / reading - duty_in_sum / reading) / 2 + duty_in_sum / reading;
	printf("Final red: %f, final Temp:%g \n", ref_read, compute_temperatur());

	printf("***************get_ref_reading***********\n");

	return ref_read;
}

float get_ref_one_gram()
{
	referenc_reading = get_ref_reading(10);

	int a;
	for (a = 0; a < COUNT; a++)
	{
		if (roundNo_f(temp[a], 3) != 0)
			printf("%f\n", temp[a]);

	}

	while (true)
		;

	printf("add 1 gram of weight \n");
//	delay_ms(10000);
	printf("calculating \n");

	float duty_gram; // = get_ref_reading(3);

//	printf("duty weight one gram: %f \n", duty_gram-referenc_reading);

	return duty_gram - referenc_reading;
}


float measure_weight(float referenz, float one_gram_step) // jede schritt ist fast 0.14 g , jede gram ist 7 schritte
{
	int weight_duty = 0;
	uint32_t duty_cycle = start_position(start_duty);
	int step_size = 1;
	printf("'***************measure_weight***********\n");
	while (toogled_wall == false)
	{
		duty_cycle += step_size;
		if (duty_cycle == period)
		{
			return false;
		}
		PWM_MatchUpdate(LPC_PWM1, pwm_channel, duty_cycle, PWM_MATCH_UPDATE_NOW);
		printf("Duty: %d, Temp:%g \n", duty_cycle, compute_temperatur());
		delay_ms(step_delay);
	}
	PWM_MatchUpdate(LPC_PWM1, pwm_channel, 0, PWM_MATCH_UPDATE_NOW);
	weight_duty = duty_cycle - referenz;
	float weight = (float) weight_duty / one_gram_step;

	printf("Weight: %f, final Temp:%g \n", weight, compute_temperatur());

	return weight;

}
#endif /* OLD_WEIGHT_H_ */
