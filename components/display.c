/*
 * display.c
 *
 *  Created on: Jan 19, 2015
 *      Author: jaska
 */

#include <string.h>
#include "nordic_common.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "spi_master.h"
#include "app_error.h"
#include "app_util.h"
#include "app_util_platform.h"
#include "hrm_app.h"
#include "datetime.h"
#include "u8g.h"
#include "u8g_arm.h"
#include "custom_board.h"
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

static volatile uint8_t contrast = 7;
static volatile uint8_t do_display_update = 0;

void display_enable_vdd()
{
	nrf_gpio_pin_set(DISPLAY_EN);
	nrf_delay_ms(150);
}

void display_disable_vdd()
{
	nrf_gpio_pin_clear(DISPLAY_EN);
	nrf_delay_ms(150);
}

uint16_t display_get_contrast()
{
	return contrast;
}

void display_set_contrast(uint16_t newc)
{
	contrast = newc;
}

void display_set_update()
{
	do_display_update = 1;
}

void display_update()
{
	if (do_display_update == 0)
		return;

	do_display_update = 0;

	// Update gui values
	gui_context_update();

	u8g_spi_init();

	/**
	 * Contrast setting can have values 1-7
	 * SSD1351 driver uses the 4 msb's.
	 * With this scaling, we will get useful values of contrast
	 * 3,5,7,9,11,13,15
	 */
	u8g_SetContrast(&u8g, (1 + (contrast * 2)) << 4);

	u8g_FirstPage(&u8g);
	do
	{
		gui_draw();
	} while( u8g_NextPage(&u8g) );

	spi_fast_close();
}

void display_init()
{
	// Initialize U8GLib
	u8g_InitComFn(&u8g, &u8g_dev_ssd1351_128x128_65k_332_hw_spi, u8g_com_hw_spi_fn);

	// Enable display 12V vdd
	display_enable_vdd();

	spi_fast_close();
}

