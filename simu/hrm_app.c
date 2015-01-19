/*
 * hrm_app.c

 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#include <stdint.h>
#include "u8g.h"
#include "hrm_app.h"
//#include "app_timer.h"
#include "bluetooth_icon.h"
#include "battery_empty_icon.h"
#include "cog_icon.h"
#include "power_standby_icon.h"
#include "heart_icon.h"
#include "signal_icon.h"
#include "timer_icon.h"
#include "wrench_icon.h"

#define DRAW_ICON(X, Y, NAME) \
	u8g_DrawXBM(&u8g, X, Y, NAME##_width, NAME##_height, NAME##_bits)

u8g_t u8g;

// Application variables
static uint8_t heartrate = 50;
static uint16_t timer = 3590;
static e_app_ble_status conn_status = APP_BLE_DISCONNECTED;
static e_app_state app_state = APP_MAIN;

// Display string variables
uint8_t hrstr[4];
uint8_t timerstr[9];
uint8_t timerbig = 0;

inline uint8_t getHours(uint16_t seconds)
{
	if (seconds < 3600)
		return 0;

	return seconds / 3600;
}

inline uint8_t getMinutes(uint16_t seconds)
{
	if (seconds < 60)
		return 0;

	return (seconds % 3600) / 60;
}

inline uint8_t getSeconds(uint16_t seconds)
{
	return seconds % 60;
}

void update_values()
{
	sprintf(hrstr, "%3u", heartrate);
	if (getHours(timer) > 0) {
		sprintf(timerstr, "%02u:%02u:%02u", getHours(timer), getMinutes(timer), getSeconds(timer));
		timerbig = 1;
	}
	else {
		sprintf(timerstr, "%02u:%02u", getMinutes(timer), getSeconds(timer));
		timerbig = 0;
	}

}

void second_timer(void *p_ctx)
{
	display_update_handler();
	timer++;
}

void draw_main_screen()
{
	u8g_SetFont(&u8g, u8g_font_gdb20n);
	u8g_DrawStr(&u8g,  50, 76, hrstr);
	DRAW_ICON(30, 58, heart_icon);

	u8g_SetFont(&u8g, u8g_font_gdb12n);
	u8g_DrawStr(&u8g,  50, 92, hrstr);
	DRAW_ICON(30, 78, battery_empty_icon);
	u8g_DrawStr(&u8g,  50, 112, hrstr);
	DRAW_ICON(30, 98, signal_icon);

	//u8g_DrawFrame(&u8g, 24, 54, 80, 62);
	// Horizontals
	u8g_DrawLine(&u8g, 0, 54, 128, 54);
	u8g_DrawLine(&u8g, 0, 91, 24, 91);
	u8g_DrawLine(&u8g, 104, 91, 128, 91);
	u8g_DrawLine(&u8g, 24, 54, 24, 128);
	u8g_DrawLine(&u8g, 104, 54, 104, 128);

	if (timerbig)
		u8g_DrawStr(&u8g, 30, 52, timerstr);
	else
		u8g_DrawStr(&u8g, 40, 52, timerstr);

	DRAW_ICON(111, 1, bluetooth_icon);
	DRAW_ICON(4, 1, battery_empty_icon);
	DRAW_ICON(108, 64, wrench_icon);
	DRAW_ICON(4, 64, power_standby_icon);
	DRAW_ICON(4, 101, timer_icon);
}

void display_update_handler()
{
	update_values();
	u8g_FirstPage(&u8g);
	do
	{

	} while( u8g_NextPage(&u8g) );
}

void hr_handler(uint8_t hr)
{
	heartrate = hr;
}

void hrm_app_init()
{
	u8g_Init(&u8g, &u8g_dev_sdl_1bit);
    //app_timer_create(&gfxUpdateTimer, APP_TIMER_MODE_REPEATED, &displayUpdateHandler);
    //app_timer_start(gfxUpdateTimer, APP_TIMER_TICKS(500, APP_TIMER_PRESCALER), NULL);
}

void hrm_app_loop()
{
}
