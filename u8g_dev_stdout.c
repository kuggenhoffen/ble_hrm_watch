/*

  u8g_dev_stdout.c
  
*/

#include <stdio.h>
#include "u8g.h"
#include "app_uart.h"
#include "nrf_delay.h"


#define WIDTH 128
#define HEIGHT 128
#define PAGE_HEIGHT 2

uint8_t lineBuf[WIDTH+1];

uint8_t u8g_dev_stdout_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg);

U8G_PB_DEV(u8g_dev_stdout, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_stdout_fn, NULL);

uint8_t u8g_dev_stdout_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{

	switch(msg)
	{
		case U8G_DEV_MSG_PAGE_NEXT:
		{
			u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
			uint8_t i, j;
			uint8_t page_height;
			uint8_t byte = 0;
			uint8_t *bufPtr;
			page_height = pb->p.page_y1;
			page_height -= pb->p.page_y0;
			page_height++;
			for( j = 0; j < page_height; j++ )
			{
				byte = 0;
				bufPtr = lineBuf;
				for( i = 0; i < WIDTH; i++ )
				{
					byte = byte >> 1;
					if ( (((uint8_t *)(pb->buf))[i] & (1<<j)) != 0 ) {
#ifdef BINARY_DISPLAY
						byte |= (1 << 7);
#else
						*bufPtr++ = 'X';
#endif
					}
					else {
#ifndef BINARY_DISPLAY
						*bufPtr++ = ' ';
#endif
					}
#ifdef BINARY_DISPLAY
					if ((i % 8 == 0 && i != 0) || i == WIDTH) {
						// First xor with 0xAA to get rid of most 0's, since most of the
						// picture will probably be empty
						byte ^= 0xAA;
						// Escape null bytes and escape bytes
						if (byte == 0 || byte == 0x7D) {
							// Write escape byte
							*bufPtr++ = 0x7d;
							*bufPtr++ = byte ^ 0xBB;
						}
						*bufPtr++ = byte;
						byte = 0;
					}
#endif
				}
				*bufPtr++ = '\0';
				printf(">%s\r\n", lineBuf);
				//printf("%s\r\n", lineBuf);
			}
		}
		break;    /* continue to base fn */
	}
	return u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
}

