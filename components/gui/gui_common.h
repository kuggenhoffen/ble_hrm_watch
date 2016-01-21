/*
 * gui_elements.h
 *
 *  Created on: Jan 20, 2015
 *      Author: jaska
 */

#ifndef GUI_ELEMENTS_H_
#define GUI_ELEMENTS_H_

typedef enum {
	GUI_STATE_STANDBY,
	GUI_STATE_DISPLAYOFF,
	GUI_STATE_MAIN,
	GUI_STATE_SETTINGS,
} gui_state_e;

#define DRAW_ICON(X, Y, NAME) \
	u8g_DrawXBM(&u8g, X, Y, NAME##_width, NAME##_height, NAME##_bits)

void gui_set_state(gui_state_e new_state);
gui_state_e gui_get_state();
void gui_context_update();
void gui_draw();
void gui_draw_signal_indicator(uint8_t x, uint8_t y, uint8_t signal_level);
void gui_draw_battery_indicator(uint8_t x, uint8_t y, uint8_t battery_level);
void gui_draw_frame();

void set_gui_color_active();
void set_gui_color_inactive();
void set_gui_color_alert();
void set_gui_color_background();

#endif /* GUI_ELEMENTS_H_ */
