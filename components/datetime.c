#include <stdlib.h>
#include <stdint.h>
#include "hrm_app.h"
#include "time.h"
#include "nrf.h"
#include "datetime.h"
#include "flash.h"

static time_t unix_seconds;
static struct tm* ptr_datetime;
static struct tm* ptr_timer;
static e_datetime_timer_state timer_state = DATETIME_TIMER_DISABLED;

uint32_t datetime_get_time()
{
	return unix_seconds;
}

void datetime_tm_update(struct tm* ptr)
{
	ptr->tm_sec++;
	if (ptr->tm_sec != 60)
		return;

	ptr->tm_sec = 0;
	ptr->tm_min++;
	if (ptr->tm_min != 60)
		return;

	ptr->tm_min = 0;
	ptr->tm_hour++;
	if (ptr->tm_hour != 24) {
		return;
	}

	ptr->tm_hour = 0;
	ptr->tm_mday++;
}

void datetime_tick()
{
	unix_seconds++;
	datetime_tm_update(ptr_datetime);
	if (timer_state == DATETIME_TIMER_RUNNING) {
		datetime_tm_update(ptr_timer);
	}
}

/**
 * Methods for setting values in clock tm struct
 */
void datetime_update_clock(time_t newtime)
{
	unix_seconds = newtime;
	ptr_datetime = localtime(&unix_seconds);
}

void datetime_set_year(uint16_t year)
{
	ptr_datetime->tm_year = year;
	unix_seconds = mktime(ptr_datetime);
}

void datetime_set_month(uint16_t month)
{
	ptr_datetime->tm_mon = month;
	unix_seconds = mktime(ptr_datetime);
}

void datetime_set_day(uint16_t day)
{
	ptr_datetime->tm_mday = day;
	unix_seconds = mktime(ptr_datetime);
}

void datetime_set_hour(uint16_t hour)
{
	ptr_datetime->tm_hour = hour;
	unix_seconds = mktime(ptr_datetime);
}

void datetime_set_minute(uint16_t minute)
{
	ptr_datetime->tm_min = minute;
	unix_seconds = mktime(ptr_datetime);
}

/**
 * Methods for getting values from clock tm struct
 */
uint16_t datetime_get_year()
{
	return ptr_datetime->tm_year;
}

uint16_t datetime_get_month()
{
	return ptr_datetime->tm_mon;
}

uint16_t datetime_get_day()
{
	return ptr_datetime->tm_mday;
}

uint16_t datetime_get_hour()
{
	return ptr_datetime->tm_hour;
}

uint16_t datetime_get_minute()
{
	return ptr_datetime->tm_min;
}

uint8_t datetime_get_clock_string(uint8_t* buf)
{
	if (!buf)
		return 0;

	uint8_t digit = ptr_datetime->tm_hour % 10;
	*buf++ = '0' + (ptr_datetime->tm_hour - digit) / 10;
	*buf++ = '0' + digit;
	*buf++ = ':';
	digit = ptr_datetime->tm_min % 10;
	*buf++ = '0' + (ptr_datetime->tm_min - digit) / 10;
	*buf++ = '0' + digit;
	*buf++ = 0;
	return 6;
}

uint8_t datetime_get_timer_string(uint8_t* buf)
{
	if (!buf)
		return 0;

	uint8_t digit = 0;
	uint8_t len = 0;

	if (ptr_timer->tm_hour > 0) {
		digit = ptr_timer->tm_hour % 10;
		*buf++ = '0' + (ptr_timer->tm_hour - digit) / 10;
		*buf++ = '0' + digit;
		*buf++ = ':';
		len = 3;
	}
	digit = ptr_timer->tm_min % 10;
	*buf++ = '0' + (ptr_timer->tm_min - digit) / 10;
	*buf++ = '0' + digit;
	*buf++ = ':';

	digit = ptr_timer->tm_sec % 10;
	*buf++ = '0' + (ptr_timer->tm_sec - digit) / 10;
	*buf++ = '0' + digit;

	*buf++ = 0;

	return len + 6;
}

void datetime_timer_start()
{
	timer_state = DATETIME_TIMER_RUNNING;
}

void datetime_timer_pause()
{
	timer_state = DATETIME_TIMER_PAUSED;
}

void datetime_timer_stop()
{
	timer_state = DATETIME_TIMER_DISABLED;
	memset(ptr_timer, 0, sizeof(struct tm));
}

e_datetime_timer_state datetime_get_timer_state()
{
	return timer_state;
}

void datetime_init()
{
	unix_seconds = 0;
	ptr_timer = malloc(sizeof(struct tm));
	ptr_datetime = localtime(&unix_seconds);
	datetime_timer_stop();
}
