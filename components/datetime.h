/*
 * datetime.h
 *
 *  Created on: Jan 14, 2015
 *      Author: jaska
 */

#ifndef DATETIME_H_
#define DATETIME_H_

#define SECONDS_IN_YEAR 31536000
#define SECONDS_IN_DAY	86400
#define SECONDS_IN_HOUR	3600

typedef enum {
	DATETIME_TIMER_DISABLED,
	DATETIME_TIMER_PAUSED,
	DATETIME_TIMER_RUNNING
} e_datetime_timer_state;

void datetime_tick();
uint8_t datetime_get_clock_string(uint8_t* buf);
uint8_t datetime_get_timer_string(uint8_t* buf);
void datetime_init();
void datetime_timer_start();
void datetime_timer_pause();
void datetime_timer_stop();
e_datetime_timer_state datetime_get_timer_state();
uint32_t datetime_get_time();

/**
 * Methods for setting values in clock tm struct
 */
void datetime_set_year(uint16_t year);
void datetime_set_month(uint16_t month);
void datetime_set_day(uint16_t day);
void datetime_set_hour(uint16_t hour);
void datetime_set_minute(uint16_t minute);

/**
 * Methods for getting values from clock tm struct
 */
uint16_t datetime_get_year();
uint16_t datetime_get_month();
uint16_t datetime_get_day();
uint16_t datetime_get_hour();
uint16_t datetime_get_minute();

#endif /* DATETIME_H_ */
