/*
 * gui_main.c
 *
 *  Created on: Jan 31, 2015
 *      Author: jaska
 */

#include <stdint.h>
#include "hrm_app.h"
#include "u8g.h"
#include "u8g_arm.h"
#include "custom_board.h"
#include "datetime.h"
#include "display.h"
#include "gui_common.h"
#include "bluetooth_icon.h"
#include "battery_empty_icon.h"
#include "cog_icon.h"
#include "power_standby_icon.h"
#include "heart_icon.h"
#include "timer_icon.h"
#include "wrench_icon.h"
#include "media_play_icon.h"
#include "media_stop_icon.h"
#include "media_pause_icon.h"
#include "lightbulb_icon.h"
#include "signal_icon.h"

// Variables to hold display values
uint8_t display_heartrate = 0;
uint8_t display_hrm_battery = 0;
uint16_t display_timer = 0;
int8_t display_signal = 0;
uint8_t frame_visible = 1;
uint8_t timer_visible = 1;

// Display variables to hold text strings
uint8_t hrstr[4];
uint8_t timerstr[9];

// Display variables to indicate states and formatting
uint8_t timerbig = 0;

void gui_main_update_timer()
{
	if (datetime_get_timer_string(timerstr) == 6)
		timerbig = 0;
	else
		timerbig = 1;
}

void gui_main_prepare_update()
{
	gui_main_update_timer();

	display_heartrate = app_get_heartrate();
	sprintf(hrstr, "%3u", display_heartrate);

	display_hrm_battery = app_get_hrm_battery();

	display_signal = app_get_signal();
}

void gui_main_button_handler(e_app_button btn)
{
	if (gui_get_state() == GUI_STATE_MAIN) {
		switch (btn) {
			case HRM_BUTTON_TOP_LEFT:
				printf("button topleft\r\n");
				switch (datetime_get_timer_state()) {
				case DATETIME_TIMER_DISABLED:
					printf("timer enabled\r\n");
					datetime_timer_stop();
					datetime_timer_pause();
					app_logger_start();
					break;
				case DATETIME_TIMER_RUNNING:
					datetime_timer_pause();
					break;
				case DATETIME_TIMER_PAUSED:
					datetime_timer_start();

					break;
				}
				break;
			case HRM_BUTTON_TOP_RIGHT:
				switch (datetime_get_timer_state()) {
				case DATETIME_TIMER_DISABLED:
					gui_set_state(GUI_STATE_SETTINGS);
					break;
				case DATETIME_TIMER_RUNNING:
				case DATETIME_TIMER_PAUSED:
					datetime_timer_stop();
					app_logger_end();
					break;
				}
				break;
			case HRM_BUTTON_BOTTOM_RIGHT:
				gui_set_state(GUI_STATE_DISPLAYOFF);
				display_disable_vdd();
				break;
			case HRM_BUTTON_BOTTOM_LEFT:
				datetime_timer_stop();
				app_logger_end();
				gui_set_state(GUI_STATE_DISPLAYOFF);
				display_disable_vdd();
				app_enable_standby();
				break;
		}
	}
	else {
		switch (btn) {
			case HRM_BUTTON_TOP_LEFT:
			case HRM_BUTTON_TOP_RIGHT:
			case HRM_BUTTON_BOTTOM_RIGHT:
			case HRM_BUTTON_BOTTOM_LEFT:
				display_enable_vdd();
				gui_set_state(GUI_STATE_MAIN);
				break;
			default:
				break;
		}
	}
}

void gui_main_draw()
{
	// Draw heartrate, and hrm icons
	if (app_get_ble_status() == HRM_BLE_CONNECTED) {
		set_gui_color_active();
		u8g_SetFont(&u8g, u8g_font_gdb20n);
		u8g_DrawStr(&u8g,  50, 76, hrstr);
		DRAW_ICON(30, 58, heart_icon);
		gui_draw_battery_indicator(44, 78, display_hrm_battery);
		gui_draw_signal_indicator(66, 78, display_signal);
	}
	else {
		set_gui_color_inactive();
		u8g_SetFont(&u8g, u8g_font_gdb20n);
		u8g_DrawStr(&u8g,  50, 76, "--");
		DRAW_ICON(30, 58, heart_icon);
		DRAW_ICON(44, 78, battery_empty_icon);
		DRAW_ICON(66, 78, signal_icon);
	}

	set_gui_color_active();
	switch (datetime_get_timer_state()) {
	case DATETIME_TIMER_DISABLED:
		DRAW_ICON(0, 64, timer_icon);
		DRAW_ICON(112, 64, wrench_icon);
		break;
	case DATETIME_TIMER_PAUSED:
		u8g_SetFont(&u8g, u8g_font_fur14n);
		if (timerbig)
			u8g_DrawStr(&u8g, 27, 52, timerstr);
		else
			u8g_DrawStr(&u8g, 40, 52, timerstr);
		DRAW_ICON(0, 64, media_play_icon);
		DRAW_ICON(112, 64, media_stop_icon);
		break;
	case DATETIME_TIMER_RUNNING:
		u8g_SetFont(&u8g, u8g_font_fur14n);
		if (timerbig)
			u8g_DrawStr(&u8g, 27, 52, timerstr);
		else
			u8g_DrawStr(&u8g, 40, 52, timerstr);
		DRAW_ICON(0, 64, media_pause_icon);
		DRAW_ICON(112, 64, media_stop_icon);
		break;
	}
	DRAW_ICON(112, 101, lightbulb_icon);
	DRAW_ICON(0, 101, power_standby_icon);
}
