/*
 * flash.c
 *
 *  Created on: Jan 25, 2015
 *      Author: jaska
 */

#include "nordic_common.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#include "custom_board.h"
#include "spi_master.h"
#include "flash.h"
#include "hrm_debug.h"

// SPI flash defines
#define FLASH_SPI_BUF_LEN	16
#define FLASH_BLOCK_SIZE	65536

// SPI flash static variables
static volatile bool flash_spi_transfer_complete = true;
static uint8_t flash_spi_tx_data[FLASH_SPI_BUF_LEN];
static uint8_t flash_spi_rx_data[FLASH_SPI_BUF_LEN];
static volatile bool spiffs_mounted = false;

// Logger buffer and variables


void flash_spi_event_handler(spi_master_evt_t spi_master_evt)
{
    uint32_t err_code = NRF_SUCCESS;
    bool result = false;

    switch (spi_master_evt.evt_type)
    {
        case SPI_MASTER_EVT_TRANSFER_COMPLETED:
            flash_spi_transfer_complete = true;
            break;

        default:
            break;
    }
}

uint8_t flash_spi_read_status_register(uint8_t reg)
{
	flash_spi_init();

	flash_spi_tx_data[0] = reg;
	flash_spi_tx_data[1] = 0xFF;
	flash_spi_transfer_complete = false;
	uint32_t err_code =	spi_master_send_recv(SPI_MASTER_0, flash_spi_tx_data, 2, flash_spi_rx_data, 2);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	spi_master_close(SPI_MASTER_0);

	return flash_spi_rx_data[1];
}

void flash_spi_write_enable()
{
	flash_spi_init();

	flash_spi_tx_data[0] = FLASH_CMD_WRITE_ENABLE;
	flash_spi_transfer_complete = false;
	uint32_t err_code =	spi_master_send_recv(SPI_MASTER_0, flash_spi_tx_data, 1, flash_spi_rx_data, 0);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	spi_master_close(SPI_MASTER_0);
}

void flash_spi_chip_erase()
{
	flash_spi_write_enable();

	flash_spi_init();

	flash_spi_tx_data[0] = FLASH_CMD_CHIP_ERASE;
	flash_spi_transfer_complete = false;
	uint32_t err_code =	spi_master_send_recv(SPI_MASTER_0, flash_spi_tx_data, 1, flash_spi_rx_data, 0);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	spi_master_close(SPI_MASTER_0);
}

void flash_spi_block_erase(const uint32_t address, const uint32_t size)
{
	uint16_t count = size / FLASH_BLOCK_SIZE;
	while (count--) {
		flash_spi_write_enable();

		flash_spi_init();

		// Copy command to tx buffer
		flash_spi_tx_data[0] = FLASH_CMD_BLOCK_ERASE;
		// Flash address
		flash_spi_tx_data[1] = (address & 0x00FF0000) >> 16;
		flash_spi_tx_data[2] = (address & 0x0000FF00) >> 8;
		flash_spi_tx_data[3] = (address & 0x000000FF);

		flash_spi_transfer_complete = false;
		uint32_t err_code =	spi_master_send_recv(SPI_MASTER_0, flash_spi_tx_data, 4, flash_spi_rx_data, 0);
		APP_ERROR_CHECK(err_code);

		while (!flash_spi_transfer_complete) { ; }

		spi_master_close(SPI_MASTER_0);

		flash_spi_wait_busy();
	}
}

void flash_spi_wait_busy()
{
	while (flash_spi_read_status_register(FLASH_CMD_READ_STATUS_REG1) & 0x01) { ; }
}

int8_t flash_spi_page_write(const uint8_t* buf, const uint32_t len, const uint32_t address)
{
	if (!buf || !len)
		return 0;

	flash_spi_write_enable();

	flash_spi_init();

	// Copy command to tx buffer
	flash_spi_tx_data[0] = FLASH_CMD_PAGE_PROGRAM;
	// Flash address
	flash_spi_tx_data[1] = (address & 0x00FF0000) >> 16;
	flash_spi_tx_data[2] = (address & 0x0000FF00) >> 8;
	flash_spi_tx_data[3] = (address & 0x000000FF);

	flash_spi_transfer_complete = false;
	uint32_t err_code =	spi_master_send_recv_multi(SPI_MASTER_0, flash_spi_tx_data, 4, flash_spi_rx_data, 0);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	flash_spi_transfer_complete = false;
	err_code =	spi_master_send_recv(SPI_MASTER_0, buf, len, flash_spi_rx_data, 0);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	spi_master_close(SPI_MASTER_0);

	return 1;
}

int8_t flash_spi_read(const uint8_t* buf, const uint32_t len, const uint32_t address)
{
	if (!buf || !len)
		return 0;

	flash_spi_init();

	// Copy command to tx buffer
	flash_spi_tx_data[0] = FLASH_CMD_READ_DATA;
	// Flash address
	flash_spi_tx_data[1] = (address & 0x00FF0000) >> 16;
	flash_spi_tx_data[2] = (address & 0x0000FF00) >> 8;
	flash_spi_tx_data[3] = (address & 0x000000FF);

	flash_spi_transfer_complete = false;
	uint32_t err_code =	spi_master_send_recv_multi(SPI_MASTER_0, flash_spi_tx_data, 4, flash_spi_rx_data, 0);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	flash_spi_transfer_complete = false;
	err_code = spi_master_send_recv(SPI_MASTER_0, flash_spi_tx_data, 0, buf, len);
	APP_ERROR_CHECK(err_code);

	while (!flash_spi_transfer_complete) { ; }

	spi_master_close(SPI_MASTER_0);

	return len;
}

uint8_t flash_spi_transfer_finished()
{
	return flash_spi_transfer_complete;
}

void flash_spi_init()
{
    uint32_t err_code = NRF_SUCCESS;

    // Memory hold pin
	nrf_gpio_cfg_output(MEM_HOLD);
	nrf_gpio_pin_set(MEM_HOLD);

	// Memory write protect pin
	nrf_gpio_cfg_output(MEM_WP);
	nrf_gpio_pin_set(MEM_WP);

    // Configure SPI master.
    spi_master_config_t spi_config = SPI_MASTER_INIT_DEFAULT;

	spi_config.SPI_Pin_SCK  = SPI_SCK_PIN;
	spi_config.SPI_Pin_MISO = SPI_MISO_PIN;
	spi_config.SPI_Pin_MOSI = SPI_MOSI_PIN;
	spi_config.SPI_Pin_SS   = SPI_CS_MEMORY;

    spi_config.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_MsbFirst;
    spi_config.SPI_CONFIG_CPHA = SPI_CONFIG_CPOL_ActiveHigh;
    spi_config.SPI_CONFIG_CPOL = SPI_CONFIG_CPOL_ActiveHigh;

    spi_config.SPI_Freq = SPI_FREQUENCY_FREQUENCY_M4;

    err_code = spi_master_open(SPI_MASTER_0, &spi_config);
    APP_ERROR_CHECK(err_code);

    // Register event handler for SPI master.
    spi_master_evt_handler_reg(SPI_MASTER_0, flash_spi_event_handler);
}

void flash_read(const uint32_t addr, const uint32_t size, uint8_t *dst)
{
	//DEBUG_LOG("read addr=%d, size=%d\r\n", addr, size);
	while (!flash_spi_transfer_complete) { ; }
	flash_spi_wait_busy();
	flash_spi_read(dst, size, addr);
}

void flash_write(const uint32_t addr, const uint32_t size, const uint8_t *src) {
	//DEBUG_LOG("write addr=%d, size=%d\r\n", addr, size);
	while (!flash_spi_transfer_complete) { ; }
	flash_spi_wait_busy();
	flash_spi_page_write(src, size, addr);
}

void flash_erase(const uint32_t addr, const uint32_t size) {
	//DEBUG_LOG("erase\r\n");
	while (!flash_spi_transfer_complete) { ; }
	flash_spi_wait_busy();
	flash_spi_block_erase(addr, size);
}
