/*
 * gui_elements.c
 *
 *  Created on: Jan 20, 2015
 *      Author: jaska
 */

#include <stdint.h>
#include "hrm_app.h"
#include "u8g.h"
#include "u8g_arm.h"
#include "gui_common.h"
#include "gui_main.h"
#include "gui_settings.h"
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

static volatile gui_state_e gui_state = GUI_STATE_MAIN;

uint8_t batstr[5];
uint8_t clockstr[6];
uint8_t display_device_battery = 0;

void set_gui_color_active()
{
	u8g_SetColorIndex(&u8g, 0xff);
}

void set_gui_color_inactive()
{
	u8g_SetColorIndex(&u8g, 0x49);
}

void set_gui_color_alert()
{
	u8g_SetColorIndex(&u8g, 0xe0);
}

void set_gui_color_background()
{
	u8g_SetColorIndex(&u8g, 0x00);
}

gui_state_e gui_get_state()
{
	return gui_state;
}

void gui_set_state(gui_state_e new_state)
{
	gui_state = new_state;
}

void gui_common_prepare_update()
{
	datetime_get_clock_string(clockstr);
	display_device_battery = app_get_battery();
	sprintf(batstr, "%3d%%", display_device_battery);
}

void gui_common_draw()
{
	// Draw top icons and time
	// Draw battery indicator and percentage string
	gui_draw_battery_indicator(28, 1, display_device_battery);
	set_gui_color_active();
	u8g_SetFont(&u8g, u8g_font_6x12r);
	u8g_DrawStr(&u8g,  1, 12, batstr);

	// Draw clock string
	u8g_SetFont(&u8g, u8g_font_fur11n);
	u8g_DrawStr(&u8g, 76, 15, clockstr);
}

void gui_draw_signal_indicator(uint8_t x, uint8_t y, uint8_t signal_level)
{
	// Set color to red if signal level less than 20%, otherwise white
	if (signal_level < 20)
		set_gui_color_alert();
	else
		set_gui_color_active();

	u8g_DrawBox(&u8g,  x+1, y+9, 2,  7);

	// If signal level less than 30%, grey out the three remaining boxes
	if (signal_level < 30)
		set_gui_color_inactive();
	u8g_DrawBox(&u8g,  x+5, y+6, 2, 10);

	// If signal level less than 60%, grey out the two remaining boxes
	if (signal_level < 60)
		set_gui_color_inactive();
	u8g_DrawBox(&u8g,  x+9, y+3, 2, 13);

	// If signal level less than 90%, grey out the last box
	if (signal_level < 90)
		set_gui_color_inactive();
	u8g_DrawBox(&u8g, x+13,   y, 2, 16);
}

void gui_draw_battery_indicator(uint8_t x, uint8_t y, uint8_t battery_level)
{
	if (battery_level < 30)
		set_gui_color_alert();
	else
		set_gui_color_active();
	// Draw battery icon
	DRAW_ICON(x, y, battery_empty_icon);
	uint8_t xpos = (battery_level + 9) / 10;

	u8g_DrawBox(&u8g, x+2, y+4, xpos, 8);
}

void gui_draw_frame()
{
	set_gui_color_active();
	u8g_DrawLine(&u8g, 0, 54, 24, 54);
	u8g_DrawLine(&u8g, 104, 54, 128, 54);
	u8g_DrawLine(&u8g, 0, 91, 24, 91);
	u8g_DrawLine(&u8g, 104, 91, 128, 91);
	u8g_DrawLine(&u8g, 24, 54, 24, 128);
	u8g_DrawLine(&u8g, 104, 54, 104, 128);
}

void gui_context_update()
{
	e_app_button btn = app_get_button();
	gui_common_prepare_update();
	switch (gui_state) {
	case GUI_STATE_MAIN:
	case GUI_STATE_DISPLAYOFF:
		gui_main_button_handler(btn);
		gui_main_prepare_update();
		break;
	case GUI_STATE_SETTINGS:
		gui_settings_button_handler(btn);
		gui_settings_prepare_update();
		break;
	}
}

void gui_draw()
{
	switch (gui_state) {
	case GUI_STATE_MAIN:
		gui_common_draw();
		gui_main_draw();
		break;
	case GUI_STATE_SETTINGS:
		gui_common_draw();
		gui_settings_draw();
		break;
	default:
		break;
	}
}
