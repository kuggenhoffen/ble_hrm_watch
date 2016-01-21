/*
 * hrm_app.h
 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#ifndef HRM_APP_H_
#define HRM_APP_H_

#define HRM_MAX_DEVICE_COUNT	5

typedef enum {
	HRM_STATE_STANDBY,
	HRM_STATE_ACTIVE
} e_app_state;

typedef enum {
	HRM_BLE_DISCONNECTED,
	HRM_BLE_CONNECTED
} e_app_ble_status;

typedef enum {
	HRM_BUTTON_BOTTOM_RIGHT,
	HRM_BUTTON_BOTTOM_LEFT,
	HRM_BUTTON_TOP_RIGHT,
	HRM_BUTTON_TOP_LEFT,
	HRM_BUTTON_NONE = 0xff
} e_app_button;

typedef struct {
	uint8_t hrm_address[6];
} hrm_device_s;

void app_init();
void app_uart_recv(uint8_t byte);
void app_sec_tick();
void app_update_heartrate(uint16_t hr);
void app_update_hrm_battery(uint8_t batt);
void app_update_rssi(int8_t rssi);
void app_loop();
e_app_button app_get_button();
uint8_t app_get_heartrate();
uint8_t app_get_hrm_battery();
uint8_t app_get_signal();
uint8_t app_get_battery();
void app_logger_start();
void app_logger_end();
void app_ble_connected();
void app_ble_disconnected();
void app_button_callback(e_app_button button);
e_app_state app_get_state();
void app_add_ble_scan_result(uint8_t *addr);
uint8_t logger_get_frequency();
void logger_set_frequency(uint8_t new);
uint8_t app_get_scan_state();
void app_set_scan_state(uint8_t new);
void scan_start(void);
void scan_stop(void);

#endif /* HRM_APP_H_ */
