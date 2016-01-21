/*
  
  u8g_arm.c
  

  u8g utility procedures for LPC11xx

  Universal 8bit Graphics Library
  
  Copyright (c) 2013, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

 
  The following delay procedures must be implemented for u8glib. This is done in this file:

  void u8g_Delay(uint16_t val)		Delay by "val" milliseconds
  void u8g_MicroDelay(void)		Delay be one microsecond
  void u8g_10MicroDelay(void)	Delay by 10 microseconds
  
  Additional requirements:
  
      SysTick must be enabled, but SysTick IRQ is not required. Any LOAD values are fine,
      it is prefered to have at least 1ms
      Example:
        SysTick->LOAD = (SystemCoreClock/1000UL*(unsigned long)SYS_TICK_PERIOD_IN_MS) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = 7;   // enable, generate interrupt (SysTick_Handler), do not divide by 2
*/

#include <stdint.h>
#include <stdio.h>
#include "u8g.h"
#include "u8g_arm.h"
#include "nrf_delay.h"
#include "custom_board.h"
#include "app_error.h"
#include "app_util.h"
#include "nordic_common.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "spi_master.h"
#include "app_trace.h"
//#include "display.h"
#include "spi_fast.h"

#define TX_RX_MSG_LENGTH	10

#define SPI_STATE_IDLE		0
#define SPI_STATE_WAITING	1

static uint8_t tx_data_spi[TX_RX_MSG_LENGTH];
static uint8_t rx_data_spi[TX_RX_MSG_LENGTH];

static spi_fast_init_params_t spi_params = {
		.pin_cs = SPI_SCK_PIN,
		.pin_miso = SPI_MISO_PIN,
		.pin_mosi = SPI_MOSI_PIN,
		.pin_sck = SPI_SCK_PIN,
		.spi_instance = 0,
		.byte_order = SPI_FAST_ORDER_MSB,
		.frequency = SPI_FAST_FREQ_8M,
		.mode = 3
};

/*========================================================================*/
/*
  The following delay procedures must be implemented for u8glib

  void u8g_Delay(uint16_t val)		Delay by "val" milliseconds
  void u8g_MicroDelay(void)		Delay be one microsecond
  void u8g_10MicroDelay(void)	Delay by 10 microseconds

*/

void u8g_Delay(uint16_t val)
{
	nrf_delay_ms((uint32_t)val);
}

void u8g_MicroDelay(void)
{
	nrf_delay_us(1);
}

void u8g_10MicroDelay(void)
{
	nrf_delay_us(10);
}

void u8g_spi_init()
{
	spi_fast_init(&spi_params);
}

uint8_t u8g_com_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	uint8_t *ptr = arg_ptr;

  switch(msg)
  {
    case U8G_COM_MSG_STOP:
      break;
    
    case U8G_COM_MSG_INIT:
    	u8g_spi_init();
    	u8g_SetDefaultBackgroundColor(u8g);
    	u8g_MicroDelay();
      break;

    case U8G_COM_MSG_ADDRESS:
    	if (arg_val)
    		nrf_gpio_pin_set(DISPLAY_DC);
    	else
    		nrf_gpio_pin_clear(DISPLAY_DC);
     break;

    case U8G_COM_MSG_CHIP_SELECT:
    	if (arg_val) {
    		// Enable
    		nrf_gpio_pin_clear(SPI_CS_DISPLAY);
    	}
    	else {
    		// Disable
    		nrf_gpio_pin_set(SPI_CS_DISPLAY);
    	}
    	u8g_MicroDelay();
      break;

    case U8G_COM_MSG_RESET:
    	if (arg_val)
    		nrf_gpio_pin_set(DISPLAY_RESET);
    	else
    		nrf_gpio_pin_clear(DISPLAY_RESET);
    	u8g_MicroDelay();
      break;

    case U8G_COM_MSG_WRITE_BYTE:
    	tx_data_spi[0] = arg_val;
    	spi_fast_write(tx_data_spi, 1);
      break;

    case U8G_COM_MSG_WRITE_SEQ:
    case U8G_COM_MSG_WRITE_SEQ_P:
    	spi_fast_write(ptr, arg_val);
      break;
  }
  return 1;
}
