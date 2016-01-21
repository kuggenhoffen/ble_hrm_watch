/*
 * display.h
 *
 *  Created on: Jan 19, 2015
 *      Author: jaska
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

extern uint8_t frame_visible;

void display_enable_vdd();
void display_disable_vdd();
void display_update();
void display_init();
void spi_init();
uint16_t display_get_contrast();
void display_set_contrast(uint16_t newc);
void display_set_update();
#endif /* DISPLAY_H_ */
