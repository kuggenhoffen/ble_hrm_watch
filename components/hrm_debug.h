/*
 * hrm_debug.c
 *
 *  Created on: Feb 5, 2015
 *      Author: jaska
 */

#ifndef HRM_DEBUG_H_
#define HRM_DEBUG_H_

extern volatile uint8_t hrm_debug_enabled;

inline uint8_t hrm_is_debug_enabled()
{
	return hrm_debug_enabled;
}

inline void hrm_debug_enable() {
	hrm_debug_enabled = 1;
}

inline void hrm_debug_disable() {
	hrm_debug_enabled = 0;
}

#define DEBUG_LOG(format, ...) \
do { \
	if (hrm_is_debug_enabled()) { \
		printf("DEBUG: "format, ## __VA_ARGS__); \
	} \
} \
while (0)


#endif /* HRM_DEBUG_H_ */
