/*
 * batterymonitor.c
 *
 *  Created on: Jan 19, 2015
 *      Author: jaska
 */
#include "nrf_gpio.h"

static volatile uint32_t adc_result;
static uint8_t adc_done = 0;

// Interrupt handler for ADC data ready event
void ADC_IRQHandler(void)
{
	/* Clear dataready event */
	NRF_ADC->EVENTS_END = 0;
	adc_result = NRF_ADC->RESULT;
	adc_done = 1;
}

void battery_start_adc()
{
	NRF_ADC->TASKS_START = 1;
}

/*
 * Returns the percentage of battery left. Based on (false) assumption of linear
 * correlation between voltage and capacity left for now.
 */
uint8_t battery_get_level()
{
	int32_t pct = ((int32_t)adc_result) - 522;
	if (pct < 0)
		pct = 0;
	pct = pct * 100 / 124;
	return pct;
}

void battery_adc_init(void)
{
	// Enable interrupt on ADC sample ready event
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NVIC_EnableIRQ(ADC_IRQn);

	// config ADC: disable external reference, analog input 2, use internal bandgap reference,
	// no input prescale, 10bit resolution
	NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos)
					| (ADC_CONFIG_PSEL_AnalogInput2 << ADC_CONFIG_PSEL_Pos)
					| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)
					| (ADC_CONFIG_INPSEL_AnalogInputNoPrescaling << ADC_CONFIG_INPSEL_Pos)
					| (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos);

	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
	battery_start_adc();
}
