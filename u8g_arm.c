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
#include "u8g_arm.h"
#include "nrf_delay.h"
#include "custom_board.h"
#include "spi_master.h"
#include "app_error.h"
#include "app_util.h"
#include "nordic_common.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "app_trace.h"

#define TX_RX_MSG_LENGTH	10

static uint8_t tx_data_spi[TX_RX_MSG_LENGTH];
static uint8_t rx_data_spi[TX_RX_MSG_LENGTH];

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

void spi_event_handler(spi_master_evt_t spi_master_evt)
{
    uint32_t err_code = NRF_SUCCESS;
    bool result = false;

    switch (spi_master_evt.evt_type)
    {
        case SPI_MASTER_EVT_TRANSFER_COMPLETED:
            // Check if received data is correct.
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void spi_init(spi_master_event_handler_t spi_master_event_handler)
{
    uint32_t err_code = NRF_SUCCESS;

    // Configure SPI master.
    spi_master_config_t spi_config = {                                                                           \
							(0x80000000UL), /**< Serial clock frequency 1 Mbps. */      \
							0xFFFFFFFF,       /**< SCK pin DISCONNECTED. */               \
							0xFFFFFFFF,       /**< MISO pin DISCONNECTED. */              \
							0xFFFFFFFF,       /**< MOSI pin DISCONNECTED. */              \
							0xFFFFFFFF,       /**< Slave select pin DISCONNECTED. */      \
							APP_IRQ_PRIORITY_LOW,       /**< Interrupt priority LOW. */             \
							(1UL),  /**< Bits order LSB. */                     \
							(1UL), /**< Serial clock polarity ACTIVEHIGH. */   \
							(1UL),    /**< Serial clock phase LEADING. */         \
							0                           /**< Don't disable all IRQs. */             \
						};

	spi_config.SPI_Pin_SCK  = SPI_SCK_PIN;
	spi_config.SPI_Pin_MISO = SPI_MISO_PIN;
	spi_config.SPI_Pin_MOSI = SPI_MOSI_PIN;
	spi_config.SPI_Pin_SS   = SPI_CS_DISPLAY;

    spi_config.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_LsbFirst;

    err_code = spi_master_open(SPI_MASTER_0, &spi_config);
    APP_ERROR_CHECK(err_code);

    // Register event handler for SPI master.
    spi_master_evt_handler_reg(SPI_MASTER_0, spi_event_handler);
}

static void init_buf(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     const uint16_t  len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        p_tx_buf[i] = i;
        p_rx_buf[i] = 0;
    }
}

void spi_out(uint8_t byte)
{
	tx_data_spi[0] = byte;
	// Start transfer.
	uint32_t err_code =
		spi_master_send_recv(SPI_MASTER_0, tx_data_spi, 1, rx_data_spi, 0);
	APP_ERROR_CHECK(err_code);
}

void send_cmd(uint8_t byte)
{
	// Pull DC low
	nrf_gpio_pin_clear(DISPLAY_DC);
	spi_out(byte);

	nrf_delay_us(1);
}

void send_data(uint8_t byte)
{
	// Pull DC high
	nrf_gpio_pin_set(DISPLAY_DC);
	spi_out(byte);
	nrf_delay_us(1);
}

void enable_display_vdd()
{
	nrf_gpio_pin_clear(DISPLAY_EN);
	nrf_delay_ms(100);
}

void disable_display_vdd()
{
	nrf_gpio_pin_set(DISPLAY_EN);
	nrf_delay_ms(100);
}

void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t fillcolor)
{

	// Bounds check
	if ((x >= 128) || (y >= 128))
		return;

	// Y bounds check
	if (y+h > 128)
	{
		h = 128 - y - 1;
	}

	// X bounds check
	if (x+w > 128)
	{
		w = 128 - x - 1;
	}

	/*
	Serial.print(x); Serial.print(", ");
	Serial.print(y); Serial.print(", ");
	Serial.print(w); Serial.print(", ");
	Serial.print(h); Serial.println(", ");
	*/

	// set location
	send_cmd(CMD_SET_COLUMN_ADDRESS);
	send_data(x);
	send_data(x+w-1);
	send_cmd(CMD_SET_ROW_ADDRESS);
	send_data(y);
	send_data(y+h-1);
	// fill!
	send_cmd(CMD_WRITE_RAM_COMMAND);

	for (uint16_t i=0; i < w*h; i++) {
		send_data(fillcolor >> 8);
		send_data(fillcolor);
	}
}

void display_test()
{
	// Reset
	printf("Display reset");
	nrf_gpio_pin_set(DISPLAY_RESET);
	nrf_delay_us(10);
	nrf_gpio_pin_clear(DISPLAY_RESET);
	nrf_delay_us(10);
	nrf_gpio_pin_set(DISPLAY_RESET);
	// Initialize SPI
	spi_init(spi_event_handler);

	send_cmd(CMD_COMMAND_LOCK);
	send_data(0x12);
	send_cmd(CMD_COMMAND_LOCK);
	send_data(0xB1);

	send_cmd(CMD_SET_SLEEP_MODE_ON);

	send_cmd(CMD_SET_FRONT_CLK_DIVIDER);
	send_data(0xF1);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)

	send_cmd(CMD_SET_MULTIPLEX_RATIO);
	send_data(127);

	send_cmd(CMD_SET_REMAP_COM_LINE);
	send_data(0x74);

	send_cmd(CMD_SET_COLUMN_ADDRESS);
	send_data(0x00);
	send_data(0x7F);
	send_cmd(CMD_SET_ROW_ADDRESS);
	send_data(0x00);
	send_data(0x7F);

	send_cmd(CMD_SET_DISPLAY_START_LINE);
	send_data(0);


	send_cmd(CMD_SET_DISPLAY_OFFSET);
	send_data(0x0);

	send_cmd(CMD_SET_GPIO);
	send_data(0x00);

	send_cmd(CMD_SET_FUNCTION_SELECTION);
	send_data(0x01);

	send_cmd(CMD_SET_PHASE_LENGTH);
	send_cmd(0x32);

	send_cmd(CMD_SET_VCOMH_VOLTAGE);
	send_cmd(0x05);

	send_cmd(CMD_SET_DISPLAY_MODE_NORMAL);

	send_cmd(CMD_SET_CONTRAST_ABC);
	send_data(0xC8);
	send_data(0x80);
	send_data(0xC8);

	send_cmd(CMD_SET_CONTRAST_MASTER);
	send_data(0x0F);

	send_cmd(CMD_SET_VSL);
	send_data(0xA0);
	send_data(0xB5);
	send_data(0x55);

	send_cmd(CMD_SET_SECOND_PRECHARGE_PERIOD);
	send_data(0x01);

	enable_display_vdd();

	send_cmd(CMD_SET_SLEEP_MODE_OFF);

	fillRect(10, 10, 50, 50, 0x8104);
	nrf_delay_ms(5000);


	// Set sleep mode on
	send_cmd(CMD_SET_SLEEP_MODE_ON);
	// Disable VDD
	disable_display_vdd();
}


/*========================================================================*/
/* u8glib com procedure */

/* gps board */
/*
uint16_t u8g_pin_a0 = PIN(0,11);
uint16_t u8g_pin_cs = PIN(0,6);
uint16_t u8g_pin_rst = PIN(0,5);
*/

/* new gps board */
/*
uint16_t u8g_pin_a0 = PIN(1,0);
uint16_t u8g_pin_cs = PIN(0,8);
uint16_t u8g_pin_rst = PIN(0,6);
*/

/* eval board */
uint16_t u8g_pin_a0 = DISPLAY_DC;
uint16_t u8g_pin_cs = SPI_CS_DISPLAY;
uint16_t u8g_pin_rst = DISPLAY_RESET;

uint8_t u8g_com_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	uint8_t *ptr = arg_ptr;
  switch(msg)
  {
    case U8G_COM_MSG_STOP:
      break;
    
    case U8G_COM_MSG_INIT:
    	printf("U8G init %d\r\n", arg_val);
    	spi_init(spi_event_handler);
    	u8g_MicroDelay();
      break;
    
    case U8G_COM_MSG_ADDRESS:
    	printf("U8G address\r\n");
    	if (arg_val)
    		nrf_gpio_pin_set(DISPLAY_DC);
    	else
    		nrf_gpio_pin_clear(DISPLAY_DC);
     break;

    case U8G_COM_MSG_CHIP_SELECT:
    	printf("U8G cs\r\n");
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
    	printf("U8G reset\r\n");
    	if (arg_val)
    		nrf_gpio_pin_clear(DISPLAY_RESET);
    	else
    		nrf_gpio_pin_set(DISPLAY_RESET);
    	u8g_MicroDelay();
      break;
      
    case U8G_COM_MSG_WRITE_BYTE:
    	spi_out(arg_val);
      break;
    
    case U8G_COM_MSG_WRITE_SEQ:
    case U8G_COM_MSG_WRITE_SEQ_P:
        while( arg_val > 0 )
        {
          spi_out(*ptr++);
          arg_val--;
        }
      break;
  }
  return 1;
}

