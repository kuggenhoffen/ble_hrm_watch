/*
 * hrm_app.c

 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "hrm_app.h"
#include "nrf_error.h"
#include "display.h"
#include "batterymonitor.h"
#include "datetime.h"
#include "fs.h"
#include "uart_protocol.h"
#include "hrm_debug.h"
#include "gui_common.h"

// Application variables
static uint8_t heartrate = 50;
static uint16_t timer = 3590;
static uint8_t hrm_batt = 100;
static uint8_t device_batt = 100;
static volatile uint8_t logger_delta = 2;
static volatile uint8_t logger_timer = 0;
static int8_t signal = 0;
static volatile e_app_ble_status conn_status = HRM_BLE_DISCONNECTED;
static e_app_state app_state = HRM_STATE_ACTIVE;
static volatile uint8_t do_display_update = 0;
static volatile e_app_button button_press = HRM_BUTTON_NONE;
volatile uint8_t hrm_debug_enabled = 1;
static uint8_t scan_enabled = 0;

uint8_t *logger_buf[32];

void app_sec_tick()
{
	datetime_tick();
	if (app_state == HRM_STATE_ACTIVE) {
		display_set_update();
		battery_start_adc();
		logger_timer++;
	}
}

void app_init()
{
	//flash_spi_chip_erase();
	//flash_spi_wait_busy();

	uart_protocol_init();

	fs_init_config_s iniconf;
	iniconf.total_size = 2*1024*1024;
	iniconf.block_size = 65536;
	iniconf.page_size = 256;
	iniconf.start_address = 0;
	iniconf.file_size = 3*iniconf.block_size;
	fs_init(&iniconf);

	uint8_t count = 0;
	fs_get_file_count(&count);
	DEBUG_LOG("FS holds %d out of %d files\r\n", count, iniconf.total_size / iniconf.file_size);

	if (count > 0) {
		fs_file_descriptor_header_s *file_headers = malloc(sizeof(fs_file_descriptor_header_s) * count);
		fs_get_file_headers(file_headers, count);
		fs_file_descriptor_header_s * hdr = file_headers;
		while (count--) {
			DEBUG_LOG("File %d: %s\r\n", hdr->id, &(hdr->filename));
			hdr++;
		}
		free(file_headers);
	}

	datetime_init();
	datetime_update_clock(BUILD_TIME);
	display_init();
	battery_adc_init();
	DEBUG_LOG("init done\r\n");
}

void app_logger_start()
{
	memset(logger_buf, 0x00, 32);
	sprintf(logger_buf, "%d.log", datetime_get_time());
	DEBUG_LOG("Started logging in %s\r\n", logger_buf);
	fs_create(logger_buf);
	DEBUG_LOG("Logger start\r\n");
}

void app_logger_end()
{
	fs_close();
	DEBUG_LOG("Logger end\r\n");
}

void app_update_heartrate(uint16_t hr)
{
	heartrate = hr;
	DEBUG_LOG("HR %d\r\n;", hr);
}

void app_update_hrm_battery(uint8_t batt)
{
	hrm_batt = batt;
	DEBUG_LOG("BAT %d\r\n;", batt);
}

void app_update_rssi(int8_t rssi)
{
	signal = rssi;
	DEBUG_LOG("RSSI %d\r\n;", rssi);
}

void app_button_callback(e_app_button button)
{
	button_press = button;
}

e_app_button app_get_button()
{
	e_app_button btn = button_press;
	button_press = HRM_BUTTON_NONE;
	return btn;
}

uint8_t app_get_heartrate()
{
	return heartrate;
}

uint8_t app_get_hrm_battery()
{
	return hrm_batt;
}

uint8_t app_get_signal()
{
	return signal;
}

uint8_t app_get_battery()
{
	device_batt = battery_get_level();
	return device_batt;
}

void app_ble_connected()
{
	conn_status = HRM_BLE_CONNECTED;
}

void app_ble_disconnected()
{
	conn_status = HRM_BLE_DISCONNECTED;
}

void app_enable_standby()
{
	app_state = HRM_STATE_STANDBY;
}

void app_disable_standby()
{
	app_state = HRM_STATE_ACTIVE;
	display_enable_vdd();
	gui_set_state(GUI_STATE_MAIN);
}

e_app_ble_status app_get_ble_status()
{
	return conn_status;
}

void app_update_display_values()
{
	heartrate += 5;
	if (heartrate > 130)
		heartrate = 50;
	hrm_batt -= 3;
	if (hrm_batt > 100)
		hrm_batt = 100;

}

void app_add_ble_scan_result(uint8_t *addr)
{

}

uint8_t app_get_scan_state()
{
	return scan_enabled;
}

void app_set_scan_state(uint8_t new)
{
	if (new != scan_enabled) {
		if (new) {
			scan_start();
		}
		else {
			scan_end();
		}
	}
	scan_enabled = new;
}

void app_scan_start()
{
	scan_start();
}

uint8_t logger_get_frequency()
{
	return logger_delta;
}

void logger_set_frequency(uint8_t new)
{
	logger_delta = new;
}

// [timestamp][,][hr][,][hrmbat][,][devbat][,][rssi][\r\n]
// 		8	   1  3   1     3    1    3     1    3    2
// 26 bytes max

void app_loop()
{
	if (app_state == HRM_STATE_ACTIVE) {
		if (logger_timer == logger_delta) {
			logger_timer = 0;
			if (datetime_get_timer_state() == DATETIME_TIMER_RUNNING) {
				int16_t count = sprintf(logger_buf, "%d,%d,%d,%d,%d\r\n", datetime_get_time(), heartrate, hrm_batt, device_batt, signal);
				if (count > 0) {
					fs_write(logger_buf, count);
				}
			}
		}

		uart_protocol_handle();
		display_update();
	}
	else {
		if (app_get_button() != HRM_BUTTON_NONE) {
			app_disable_standby();
		}
	}
}
