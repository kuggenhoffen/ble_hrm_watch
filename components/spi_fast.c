/*
 * spi_master_fast.c
 *
 *  Created on: Jan 21, 2015
 *      Author: jaska
 */

#include "nrf51.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "spi_fast.h"

volatile uint32_t* tx_reg = 0;
volatile uint32_t* rx_reg = 0;

static volatile uint8_t spi_fast_initialized = 0;
static volatile IRQn_Type irqtype;
static volatile NRF_SPI_Type* p_spi_instance;

static volatile spi_fast_init_params_t spi_fast_params;

void spi_fast_init(const spi_fast_init_params_t* p_spi_params)
{
	if (!p_spi_params || spi_fast_initialized)
		return;

	if (p_spi_params->spi_instance == 0) {
		p_spi_instance = NRF_SPI0;
		irqtype = SPI0_TWI0_IRQn;
	}
	else {
		p_spi_instance = NRF_SPI1;
		irqtype = SPI1_TWI1_IRQn;
	}

    //A Slave select must be set as high before setting it as output,
    //because during connect it to the pin it causes glitches.
    nrf_gpio_pin_set(p_spi_params->pin_cs);
    nrf_gpio_cfg_output(p_spi_params->pin_cs);
    nrf_gpio_pin_set(p_spi_params->pin_cs);

    //Configure GPIO
    nrf_gpio_cfg_output(p_spi_params->pin_sck);
    nrf_gpio_cfg_output(p_spi_params->pin_mosi);
    nrf_gpio_cfg_input(p_spi_params->pin_miso, NRF_GPIO_PIN_NOPULL);

    // Configure SPI hardware
    p_spi_instance->PSELSCK = p_spi_params->pin_sck;
    p_spi_instance->PSELMISO = p_spi_params->pin_miso;
    p_spi_instance->PSELMOSI = p_spi_params->pin_mosi;

    p_spi_instance->CONFIG = (((p_spi_params->byte_order << SPI_CONFIG_ORDER_POS) & SPI_CONFIG_ORDER_MASK) |
						((p_spi_params->mode << SPI_CONFIG_MODE_POS) & SPI_CONFIG_MODE_MASK));

    p_spi_instance->FREQUENCY = p_spi_params->frequency;

	// Get pointers to tx and rx registers
	tx_reg = &(p_spi_instance->TXD);
	rx_reg = &(p_spi_instance->RXD);

    // Clear events
	p_spi_instance->EVENTS_READY = 0;

	// Disable interrupt
	p_spi_instance->INTENCLR = 0;
	APP_ERROR_CHECK(sd_nvic_DisableIRQ(irqtype));

	// Enable SPI peripheral
	p_spi_instance->ENABLE = 1;

	spi_fast_params = *p_spi_params;

	spi_fast_initialized = 1;
}

void spi_fast_close()
{
	if (!spi_fast_initialized)
		return;

    /* Disable interrupt */
    APP_ERROR_CHECK(sd_nvic_ClearPendingIRQ(irqtype));
    APP_ERROR_CHECK(sd_nvic_DisableIRQ(irqtype));

    p_spi_instance->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);

    /* Set Slave Select pin */
    nrf_gpio_pin_set(spi_fast_params.pin_cs);

    /* Disconnect pins from SPI hardware */
    p_spi_instance->PSELSCK  = (uint32_t)SPI_PIN_DISCONNECTED;
    p_spi_instance->PSELMOSI = (uint32_t)SPI_PIN_DISCONNECTED;
    p_spi_instance->PSELMISO = (uint32_t)SPI_PIN_DISCONNECTED;

    spi_fast_initialized = 0;
}


void spi_fast_write(const uint8_t* buf, uint8_t len)
{
	if (!buf || !len || !spi_fast_initialized)
		return;


	uint8_t count = 0;
	uint8_t rxdata = 0;

	while (len) {
		*tx_reg = *buf++;
		len--;
		nrf_delay_us(2);
	}
}

