/*
 * tests.c
 *
 *  Created on: Jan 23, 2015
 *      Author: jaska
 */
#include <stdint.h>
#include <stdio.h>
#include "flash.h"
#include "display.h"
#include "u8g.h"
#include "u8g_arm.h"
#include "bsp.h"
#include "tests.h"

void flash_test();
void serial_test();
void display_test();

void test_event_handler(bsp_event_t bsp_event)
{
	switch (bsp_event) {
	case BSP_EVENT_KEY_0:
		printf("Button0\r\n");
		break;
	case BSP_EVENT_KEY_1:
		printf("Button1\r\n");
		break;
	case BSP_EVENT_KEY_2:
		printf("Button2\r\n");
		break;
	case BSP_EVENT_KEY_3:
		printf("Button3\r\n");
		break;
	default:
		break;
	}
}

void run_tests()
{
	serial_test();
	display_test();
	flash_test();
}

void flash_test()
{
	uint8_t *buf = "Flash OK";
	uint8_t rb[9];
	flash_spi_chip_erase();
	flash_spi_wait_busy();
	flash_write(0, 8, buf);
	flash_read(0, 8, rb);
	rb[8] = 0;
	printf("%s\r\n", rb);
}

void serial_test()
{
	printf("Serial OK\r\n");
}

void display_test()
{
	display_init();
	u8g_spi_init();
	u8g_FirstPage(&u8g);
	do
	{
		u8g_SetColorIndex(&u8g, 0xff);
		u8g_SetFont(&u8g, u8g_font_10x20);
		u8g_DrawStr(&u8g,  5, 30, "Display OK");
	} while( u8g_NextPage(&u8g) );
	spi_fast_close();
}
