/*
 * temp.h
 *
 *  Created on: 02.06.2015
 *      Author: MT-LAB
 */

#ifndef TEMP_H_
#define TEMP_H_

float P = 1;
float K = 1;
float X_e = 35.5, X_p = 0, Z_e = 0;
float var_m = 2*0.169545122*0.169545122;
float var_p = 0.4*0.4;//1e-9;


float kalman_filter(float reading)
{
	float Pc = P + var_p;
	K = Pc / (Pc + var_m);
	P = (1 - K) * Pc;
//	X_p = X_e;
//	Z_e = X_p;
	X_e = K * reading+(1-K)*X_e;
	return X_e;
}


float last_val_temp = 25;
float get_temperatur()
{
	int ADC_resolution = 4096;
	int ADC_ref_voltage = 3.3;
	uint32_t adc_value;

	adc_value = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);

	float voltage = (float) adc_value * ADC_ref_voltage / ADC_resolution;
	float temperatureC = (voltage - 0.6) * 100;

	if (abs(last_val_temp - temperatureC) > 120)
	{
		temperatureC = last_val_temp;
	}
	last_val_temp = temperatureC;
	return temperatureC;
}

float compute_temperatur()
{
//	float temperatureC =get_temperatur();

	double t_sum = 0;
	int i;
	int count = 5000;
	for(i = 0; i< count;i++){
		t_sum +=((float) get_temperatur());
	}
	float temperatureC = t_sum/count;

	return roundNo_f(temperatureC,2);
}



#endif /* TEMP_H_ */
