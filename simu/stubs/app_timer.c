/*
 * app_timer.c
 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#include <stdint.h>
#include "app_timer.h"
#include <time.h>


s_timer_instance *timers[MAX_TIMERS];
int timer_count = 0;
/*
uint32_t app_timer_create(app_timer_id_t *            p_timer_id,
                          app_timer_mode_t            mode,
                          app_timer_timeout_handler_t timeout_handler)
{
	if (timer_count >= MAX_TIMERS)
		return 0;

	if (p_timer_id == -1)
		return 0;

	p_timer_id = timer_count;
	timers[timer_count]->handler = timeout_handler;
	timers[timer_count]->mode = mode;
	timer_count++;
	return 1;
}

uint32_t app_timer_start(app_timer_id_t timer_id, uint32_t timeout_ticks, void * p_context);
{
	for ()
	timers[timer_count]->end_time = clock() + ;
}

void timer_init() {
	for (int i = 0; i < MAX_TIMERS-1; i++) {
		timers[i]->p_id = -1;
		timers[i]->next = -1;
		timers[i]->prev = -1;
	}
}

void timer_handler() {

}
*/
