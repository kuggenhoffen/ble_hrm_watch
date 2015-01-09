/*
 * app_timer.h
 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#include <time.h>

#ifndef APP_TIMER_H_
#define APP_TIMER_H_

#define MAX_TIMERS 4



typedef enum {
    APP_TIMER_MODE_SINGLE_SHOT,                 /**< The timer will expire only once. */
    APP_TIMER_MODE_REPEATED                     /**< The timer will restart each time it expires. */
} app_timer_mode_t;

typedef int app_timer_id_t;
typedef void (*app_timer_timeout_handler_t)(void * p_context);

typedef struct {
	app_timer_id_t p_id;
	app_timer_mode_t mode;
	app_timer_timeout_handler_t handler;
	clock_t	end_time;

} s_timer_instance;

#define APP_TIMER_TICKS(MS, PRESCALER)\
            ((uint32_t)ROUNDED_DIV((MS) * (uint64_t)APP_TIMER_CLOCK_FREQ, ((PRESCALER) + 1) * 1000))
#define APP_TIMER_CREATE(id, mode, handler)	app_timer_create(id, mode, handler);
#define APP_TIMER_START(id, timeout, ctx) app_timer_start(id, timeout, ctx);

uint32_t app_timer_create(app_timer_id_t *            p_timer_id,
                          app_timer_mode_t            mode,
                          app_timer_timeout_handler_t timeout_handler);
uint32_t app_timer_start(app_timer_id_t timer_id, uint32_t timeout_ticks, void * p_context);

#endif /* APP_TIMER_H_ */
