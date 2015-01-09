

#include "u8g.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hrm_app.h"
#include <time.h>

int u8g_sdl_get_key(void);
int key = -1;

uint8_t hr = 110;

uint8_t random_hr() {
	uint8_t newhr = hr + (rand() % 5 - 2);
	if (newhr > 40 && newhr < 140) {
		hr = newhr;
	}
	return hr;
}

int main(void)
{
	srand(time(NULL));

	hrm_app_init();
	clock_t next = clock() + CLOCKS_PER_SEC;

	while (1) {
		// First check if a key was pressed
		key = u8g_sdl_get_key();

		// Timer
		if (next <= clock()) {
			next = clock() + CLOCKS_PER_SEC;
			second_timer(NULL);
			hr_handler(random_hr());
		}

		// Enter app main loop
		hrm_app_loop();
		if (key >= 0)
			break;
	}
}

