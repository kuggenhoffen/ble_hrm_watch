/*
 * gui_settings.h
 *
 *  Created on: Feb 1, 2015
 *      Author: jaska
 */

#ifndef GUI_SETTINGS_H_
#define GUI_SETTINGS_H_

typedef enum {
	MENU_ITEM_NUMERICAL,
	MENU_ITEM_LIST,
	MENU_ITEM_BOOLEAN
} menu_item_type_e;

typedef struct {
	uint8_t* item_caption;
	menu_item_type_e item_type;
	void* item_data;
} menu_item_s;

typedef struct {
	uint16_t min_value;
	uint16_t max_value;
	uint16_t current_value;
	uint16_t (*get_value)();
	void (*set_value)(uint16_t);
} numerical_menu_item_s;

typedef struct {
	uint8_t boolean_state;
	uint8_t (*get_value)();
	void (*set_value)(uint8_t);
} boolean_menu_item_s;

typedef struct {
	uint8_t *caption;
	uint8_t index;
} list_menu_item_s;

typedef struct {
	list_menu_item_s *items;
	uint8_t length;
} list_menu_s;

void gui_settings_prepare_update();
void gui_settings_button_handler(e_app_button btn);
void gui_settings_draw();

#endif /* GUI_SETTINGS_H_ */
