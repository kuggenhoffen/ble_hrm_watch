/*
 * hrm_app.h
 *
 *  Created on: Jan 6, 2015
 *      Author: jaska
 */

#ifndef HRM_APP_H_
#define HRM_APP_H_

typedef enum {
	APP_STANDBY,
	APP_MAIN,
	APP_SETTINGS
} e_app_state;

typedef enum {
	APP_TIMER_DISABLED,
	APP_TIMER_ENABLED
} e_app_timer_state;

typedef enum {
	APP_BLE_DISCONNECTED,
	APP_BLE_CONNECTED
} e_app_ble_status;

void hrm_app_loop();
void hr_handler(uint8_t hr);
void displayUpdateHandler(void *p_ctx);
void hrm_app_init();
void second_timer(void *p_ctx);

#endif /* HRM_APP_H_ */
