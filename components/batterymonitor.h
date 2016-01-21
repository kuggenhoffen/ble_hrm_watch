/*
 * batterymonitor.h
 *
 *  Created on: Jan 19, 2015
 *      Author: jaska
 */

#ifndef BATTERYMONITOR_H_
#define BATTERYMONITOR_H_

void battery_start_adc();
void battery_adc_init();
uint8_t battery_get_level();

#endif /* BATTERYMONITOR_H_ */
