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
//#include "spi_master.h"
#include "app_error.h"
#include "app_util.h"
#include "nordic_common.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "app_trace.h"
#include "bluetooth_icon.h"
#include "battery_empty_icon.h"
#include "cog_icon.h"
#include "power_standby_icon.h"
#include "heart_icon.h"
#include "signal_icon.h"
#include "timer_icon.h"
#include "wrench_icon.h"

#define DRAW_ICON(X, Y, NAME) \
	u8g_DrawXBM(&u8g, X, Y, NAME##_width, NAME##_height, NAME##_bits)

#define TX_RX_MSG_LENGTH	10

#define SPI_STATE_IDLE		0
#define SPI_STATE_WAITING	1

static uint8_t tx_data_spi[TX_RX_MSG_LENGTH];
static uint8_t rx_data_spi[TX_RX_MSG_LENGTH];

static volatile uint8_t spi_state = SPI_STATE_IDLE;

// Display string variables
uint8_t hrstr[4];
uint8_t timerstr[9];
uint8_t timerbig = 0;

inline uint8_t getHours(uint16_t seconds)
{
	if (seconds < 3600)
		return 0;

	return seconds / 3600;
}

inline uint8_t getMinutes(uint16_t seconds)
{
	if (seconds < 60)
		return 0;

	return (seconds % 3600) / 60;
}

inline uint8_t getSeconds(uint16_t seconds)
{
	return seconds % 60;
}

void update_values(uint8_t newhr, uint16_t newtimer)
{
	sprintf(hrstr, "%3u", newhr);
	if (getHours(newtimer) > 0) {
		sprintf(timerstr, "%02u:%02u:%02u", getHours(newtimer), getMinutes(newtimer), getSeconds(newtimer));
		timerbig = 1;
	}
	else {
		sprintf(timerstr, "%02u:%02u", getMinutes(newtimer), getSeconds(newtimer));
		timerbig = 0;
	}

}
/*
void spi_master_init();
uint8_t spi_master_exchange_byte(uint8_t txbyte);
void spi_master_end_transaction();
void spi_master_start_transaction();
*/
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
/*
void SPI0_TWI0_IRQHandler(void)
{
    if ((NRF_SPI0->EVENTS_READY == 1) && (NRF_SPI0->INTENSET & SPI_INTENSET_READY_Msk))
    {
        NRF_SPI0->EVENTS_READY = 0;
        spi_state = SPI_STATE_IDLE;
    }
}
*/
static void spi_init(spi_master_event_handler_t spi_master_event_handler)
{
    uint32_t err_code = NRF_SUCCESS;

    // Configure SPI master.
    spi_master_config_t spi_config = {                                                                           \
							SPI_FREQUENCY_FREQUENCY_M4, /**< Serial clock frequency 1 Mbps. */      \
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

    spi_config.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_MsbFirst;

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
	spi_master_exchange_byte(byte);
}

void send_data(uint8_t byte)
{
	// Pull DC high
	nrf_gpio_pin_set(DISPLAY_DC);
	spi_master_exchange_byte(byte);
}

void enable_display_vdd()
{
	nrf_gpio_pin_set(DISPLAY_EN);
	nrf_delay_ms(150);
}

void disable_display_vdd()
{
	nrf_gpio_pin_clear(DISPLAY_EN);
	nrf_delay_ms(100);
}
/*
void spi_master_init()
{
	//spi_master_init_hw_instance(NRF_SPI0, SPI0_TWI0_IRQn, p_spi_instance, p_spi_master_config->SPI_DisableAllIRQ);

	//A Slave select must be set as high before setting it as output,
	//because during connect it to the pin it causes glitches.
	nrf_gpio_pin_set(SPI_CS_DISPLAY);
	nrf_gpio_cfg_output(SPI_CS_DISPLAY);
	nrf_gpio_pin_set(SPI_CS_DISPLAY);

	//Configure GPIO
	nrf_gpio_cfg_output(SPI_SCK_PIN);
	nrf_gpio_pin_set(SPI_SCK_PIN);
	nrf_gpio_cfg_output(SPI_MOSI_PIN);
	nrf_gpio_pin_clear(SPI_MOSI_PIN);
	nrf_gpio_cfg_input(SPI_MISO_PIN, NRF_GPIO_PIN_NOPULL);

	// Configure SPI hardware
	NRF_SPI0->PSELSCK  = SPI_SCK_PIN;
	NRF_SPI0->PSELMOSI = SPI_MOSI_PIN;
	NRF_SPI0->PSELMISO = SPI_MISO_PIN;

	NRF_SPI0->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M1;

	NRF_SPI0->CONFIG =
		(uint32_t)(SPI_CPHA << SPI_CONFIG_CPHA_Pos) |
		(SPI_CPOL << SPI_CONFIG_CPOL_Pos) |
		(SPI_ORDER << SPI_CONFIG_ORDER_Pos);

	// Clear waiting interrupts and events
	NRF_SPI0->EVENTS_READY = 0;

	APP_ERROR_CHECK(sd_nvic_ClearPendingIRQ(SPI0_TWI0_IRQn));
	APP_ERROR_CHECK(sd_nvic_SetPriority(SPI0_TWI0_IRQn, APP_IRQ_PRIORITY_LOW));
}

void spi_master_start_transaction()
{
	// Clear pending interrupts and events
	NRF_SPI0->EVENTS_READY = 0;
	APP_ERROR_CHECK(sd_nvic_ClearPendingIRQ(SPI0_TWI0_IRQn));
	APP_ERROR_CHECK(sd_nvic_SetPriority(SPI0_TWI0_IRQn, APP_IRQ_PRIORITY_LOW));

	// Enable interrupt
	NRF_SPI0->INTENSET = (SPI_INTENSET_READY_Set << SPI_INTENCLR_READY_Pos);
	APP_ERROR_CHECK(sd_nvic_EnableIRQ(SPI0_TWI0_IRQn));

	// Enable SPI hardware
	NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);

	// Clear chipselect pin
	nrf_gpio_pin_clear(SPI_CS_DISPLAY);
}

void spi_master_end_transaction()
{
	// Set chipselect pin
	nrf_gpio_pin_set(SPI_CS_DISPLAY);

	APP_ERROR_CHECK(sd_nvic_DisableIRQ(SPI0_TWI0_IRQn));

	// Enable SPI hardware
	NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
}

uint8_t spi_master_exchange_byte(uint8_t txbyte)
{
	uint8_t rxbyte = 0;

	//Disable interrupt SPI.
	APP_ERROR_CHECK(sd_nvic_DisableIRQ(SPI0_TWI0_IRQn));

	// Write byte to transmit register
	NRF_SPI0->TXD = (uint32_t) txbyte;
	spi_state = SPI_STATE_WAITING;

	// Wait until exchange is finished
	while (spi_state == SPI_STATE_WAITING) { ; }

	//NRF_SPI0->EVENTS_READY = 0;

	// Read byte from receive register
	rxbyte = (uint8_t)NRF_SPI0->RXD;

	//Enable SPI interrupt.
	APP_ERROR_CHECK(sd_nvic_EnableIRQ(SPI0_TWI0_IRQn));

	return rxbyte;
}
*/
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

void draw_display()
{
	u8g_SetColorIndex(&u8g, 1);
	u8g_SetFont(&u8g, u8g_font_gdb20n);
	u8g_DrawStr(&u8g,  50, 76, hrstr);
	DRAW_ICON(30, 58, heart_icon);

	u8g_SetFont(&u8g, u8g_font_gdb12n);
	u8g_DrawStr(&u8g,  50, 92, hrstr);
	DRAW_ICON(30, 78, battery_empty_icon);
	u8g_DrawStr(&u8g,  50, 112, hrstr);
	DRAW_ICON(30, 98, signal_icon);

	//u8g_DrawFrame(&u8g, 24, 54, 80, 62);
	// Horizontals
	u8g_DrawLine(&u8g, 0, 54, 128, 54);
	u8g_DrawLine(&u8g, 0, 91, 24, 91);
	u8g_DrawLine(&u8g, 104, 91, 128, 91);
	u8g_DrawLine(&u8g, 24, 54, 24, 128);
	u8g_DrawLine(&u8g, 104, 54, 104, 128);

	if (timerbig)
		u8g_DrawStr(&u8g, 30, 52, timerstr);
	else
		u8g_DrawStr(&u8g, 40, 52, timerstr);

	DRAW_ICON(111, 1, bluetooth_icon);
	DRAW_ICON(4, 1, battery_empty_icon);
	DRAW_ICON(108, 64, wrench_icon);
	DRAW_ICON(4, 64, power_standby_icon);
	DRAW_ICON(4, 101, timer_icon);
}

void display_test()
{
	//enable_display_vdd();
	u8g_FirstPage(&u8g);
	do
	{
		draw_display();
	} while( u8g_NextPage(&u8g) );
/*	uint8_t byte = 0;
	// Reset
	printf("Display reset");
	nrf_gpio_pin_set(DISPLAY_RESET);
	nrf_delay_us(10);
	nrf_gpio_pin_clear(DISPLAY_RESET);
	nrf_delay_us(10);
	nrf_gpio_pin_set(DISPLAY_RESET);
	// Initialize SPI
	spi_master_init();
	spi_master_start_transaction();
	while (1) {
		send_cmd(CMD_WRITE_RAM_COMMAND);
		send_data(byte);
		byte++;
		//nrf_delay_us(10);
		//nrf_delay_ms(500);
	}
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
	disable_display_vdd();*/
}

void display_init()
{
	printf("DISP INIT\r\n");
	nrf_gpio_cfg_output(DISPLAY_RESET);
	nrf_gpio_pin_set(DISPLAY_RESET);
	nrf_gpio_cfg_output(DISPLAY_DC);
	nrf_gpio_cfg_output(DISPLAY_EN);
	//u8g_Begin(&u8g);
	enable_display_vdd();
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
    	printf("ini\r\n");
    	spi_init(spi_event_handler);
    	u8g_SetDefaultBackgroundColor(u8g);
    	u8g_MicroDelay();
      break;
    
    case U8G_COM_MSG_ADDRESS:
    	//printf("ad\r\n");
    	if (arg_val)
    		nrf_gpio_pin_set(DISPLAY_DC);
    	else
    		nrf_gpio_pin_clear(DISPLAY_DC);
     break;

    case U8G_COM_MSG_CHIP_SELECT:
    	//printf("cs\r\n");
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
    	printf("res %d\r\n", arg_val);
    	if (arg_val)
    		nrf_gpio_pin_set(DISPLAY_RESET);
    	else
    		nrf_gpio_pin_clear(DISPLAY_RESET);
    	u8g_MicroDelay();
      break;
      
    case U8G_COM_MSG_WRITE_BYTE:
    	tx_data_spi[0] = arg_val;
    	spi_master_send_recv(SPI_MASTER_0, tx_data_spi, 1, rx_data_spi, 0);
      break;
    
    case U8G_COM_MSG_WRITE_SEQ:
    case U8G_COM_MSG_WRITE_SEQ_P:
    	spi_master_send_recv(SPI_MASTER_0, ptr, arg_val, rx_data_spi, 0);
        /*while( arg_val > 0 )
        {
          spi_out(*ptr++);
          arg_val--;
        }*/
      break;
  }
  return 1;
}

