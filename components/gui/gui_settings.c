/*
 * gui_settings.c
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
#include "gui_settings.h"
#include "battery_empty_icon.h"
#include "arrow_circle_left_icon.h"
#include "check_icon.h"
#include "x_icon.h"
#include "arrow_thick_top_icon.h"
#include "arrow_thick_bottom_icon.h"

#define SETTINGS_MENU_LENGTH			8
#define SETTINGS_MENU_WINDOW_LENGTH		4

static volatile uint8_t menu_selected = 0;
static volatile uint8_t editing = 0;
static volatile uint8_t list_start = 0;

numerical_menu_item_s menu_brightness = { 1, 7, 7,	&display_get_contrast,	&display_set_contrast };
numerical_menu_item_s menu_date_year = { 2015, 2020, 2015,	&datetime_get_year,		&datetime_set_year };
numerical_menu_item_s menu_date_month = { 1, 12, 2,	&datetime_get_month,	&datetime_set_month };
numerical_menu_item_s menu_date_day = { 1, 31, 1,		&datetime_get_day,		&datetime_set_day };
numerical_menu_item_s menu_clock_hour = { 0, 23, 21,	&datetime_get_hour,		&datetime_set_hour };
numerical_menu_item_s menu_clock_minute = { 0, 59, 20,	&datetime_get_minute,	&datetime_set_minute };
numerical_menu_item_s menu_log_frequency = { 1, 10, 1, &logger_get_frequency, &logger_set_frequency };

boolean_menu_item_s menu_ble_scan = { 0, &app_get_scan_state, &app_set_scan_state };

menu_item_s menuitems[SETTINGS_MENU_LENGTH] = {
		{ "Brightness", 	MENU_ITEM_NUMERICAL, 	&menu_brightness },
		{ "Date: Year", 	MENU_ITEM_NUMERICAL, 	&menu_date_year },
		{ "Date: Month", 	MENU_ITEM_NUMERICAL, 	&menu_date_month },
		{ "Date: Day", 		MENU_ITEM_NUMERICAL, 	&menu_date_day },
		{ "Clock: Hour", 	MENU_ITEM_NUMERICAL, 	&menu_clock_hour },
		{ "Clock: Minute", 	MENU_ITEM_NUMERICAL, 	&menu_clock_minute },
		{ "Log freq",	 	MENU_ITEM_NUMERICAL, 	&menu_clock_minute },
		{ "BLE Scan",		MENU_ITEM_BOOLEAN,		&menu_ble_scan },
};

list_menu_s ble_menu;

static volatile uint8_t edit_str[6];

void gui_item_get_value(menu_item_s* item)
{
	numerical_menu_item_s* data = item->item_data;
	data->current_value = data->get_value();
}

void gui_item_set_value(menu_item_s* item)
{
	numerical_menu_item_s* data = item->item_data;
	data->set_value(data->current_value);
}

void gui_settings_prepare_update()
{
	if (editing) {
		numerical_menu_item_s* data = menuitems[menu_selected].item_data;
		sprintf(edit_str, "%d", data->current_value);
	}
}

void gui_settings_button_handler(e_app_button btn)
{
	switch (btn) {
		case HRM_BUTTON_TOP_LEFT:
			if (editing) {
				editing = 0;
				gui_item_set_value(&menuitems[menu_selected]);
			}
			else {
				editing = 1;
				if (menuitems[menu_selected].item_type == MENU_ITEM_NUMERICAL) {
					gui_item_get_value(&menuitems[menu_selected]);
				}
			}
			break;
		case HRM_BUTTON_TOP_RIGHT:
			if (editing) {
				if (menuitems[menu_selected].item_type == MENU_ITEM_NUMERICAL) {
					numerical_menu_item_s* data = menuitems[menu_selected].item_data;
					data->current_value++;
					if (data->current_value > data->max_value || data->current_value < data->min_value)
						data->current_value = data->min_value;
				}
			}
			else {
				if (menu_selected > 0) {
					menu_selected--;
					if (menu_selected < list_start)
						list_start--;
				}
			}
			break;
		case HRM_BUTTON_BOTTOM_RIGHT:
			if (editing) {
				if (menuitems[menu_selected].item_type == MENU_ITEM_NUMERICAL) {
					numerical_menu_item_s* data = menuitems[menu_selected].item_data;
					data->current_value--;
					if (data->current_value > data->max_value || data->current_value < data->min_value)
						data->current_value = data->max_value;
				}
			}
			else {
				if (menu_selected < SETTINGS_MENU_LENGTH - 1) {
					menu_selected++;
					if (menu_selected > list_start + SETTINGS_MENU_WINDOW_LENGTH - 1)
						list_start++;
				}
			}
			break;
		case HRM_BUTTON_BOTTOM_LEFT:
			if (editing) {
				editing = 0;
			}
			else {
				gui_set_state(GUI_STATE_MAIN);
			}
			break;
	}
}

void gui_settings_draw()
{
	uint8_t height, index, xoff, yindex, liststart;
	// Up/Down arrows
	DRAW_ICON(112, 64, arrow_thick_top_icon);
	DRAW_ICON(112, 101, arrow_thick_bottom_icon);
	if (!editing) {
		set_gui_color_active();
		u8g_SetFont(&u8g, u8g_font_fur14r);
		u8g_DrawStr(&u8g,  27, 42, "Settings");
		// Menu
		u8g_SetFont(&u8g, u8g_font_fur11r);
		u8g_SetFontRefHeightText(&u8g);
		u8g_SetFontPosTop(&u8g);
		height = u8g_GetFontAscent(&u8g)-u8g_GetFontDescent(&u8g) + 2;
		for (index = list_start, yindex = 0; index < list_start + SETTINGS_MENU_WINDOW_LENGTH; index++, yindex++) {
			xoff = (128 - u8g_GetStrWidth(&u8g, menuitems[index].item_caption))/2;
			set_gui_color_active();
			if (index == menu_selected) {
				set_gui_color_active();
				u8g_DrawBox(&u8g, 20, 56+yindex*height+1, 88, height);
				set_gui_color_background();
				u8g_DrawStr(&u8g, xoff, 56+yindex*height+2, menuitems[index].item_caption);
			}
			else {
				set_gui_color_background();
				u8g_DrawBox(&u8g, 20, 56+yindex*height+1, 88, height);
				set_gui_color_active();
				u8g_DrawStr(&u8g, xoff, 56+yindex*height+2, menuitems[index].item_caption);
			}
		}
		set_gui_color_active();
		DRAW_ICON(0, 64, check_icon);
		DRAW_ICON(0, 101, arrow_circle_left_icon);
	}
	else {
		set_gui_color_active();
		u8g_SetFont(&u8g, u8g_font_fur14r);
		xoff = (128 - u8g_GetStrWidth(&u8g, menuitems[menu_selected].item_caption))/2;
		u8g_DrawStr(&u8g,  xoff, 42, menuitems[menu_selected].item_caption);

		u8g_SetFontRefHeightText(&u8g);
		u8g_SetFontPosTop(&u8g);

		switch (menuitems[menu_selected].item_type) {
		case MENU_ITEM_NUMERICAL:
			xoff = (128 - u8g_GetStrWidth(&u8g, edit_str))/2;
			u8g_DrawStr(&u8g, xoff, 64, edit_str);
			break;
		case MENU_ITEM_LIST:
			break;
		}

		DRAW_ICON(0, 64, check_icon);
		DRAW_ICON(0, 101, x_icon);
	}
}
