/*
 * spi_master_fast.h
 *
 *  Created on: Jan 21, 2015
 *      Author: jaska
 */

#ifndef SPI_MASTER_FAST_H_
#define SPI_MASTER_FAST_H_

#define SPI_CONFIG_ORDER_POS	0
#define SPI_CONFIG_MODE_POS		1

#define SPI_CONFIG_ORDER_MASK	1
#define SPI_CONFIG_MODE_MASK	3<<1

#define SPI_PIN_DISCONNECTED 0xFFFFFFFF

typedef enum {
	SPI_FAST_FREQ_125K = 0x02000000,
	SPI_FAST_FREQ_250K = 0x04000000,
	SPI_FAST_FREQ_500K = 0x08000000,
	SPI_FAST_FREQ_1M = 0x10000000,
	SPI_FAST_FREQ_2M = 0x20000000,
	SPI_FAST_FREQ_4M = 0x40000000,
	SPI_FAST_FREQ_8M = 0x80000000
} spi_fast_speed_t;

typedef enum {
	SPI_FAST_ORDER_MSB,
	SPI_FAST_ORDER_LSB
} spi_fast_order_t;

typedef enum {
	SPI_FAST_MODE_0,
	SPI_FAST_MODE_1,
	SPI_FAST_MODE_2,
	SPI_FAST_MODE_3,
} spi_fast_mode_t;

typedef struct {
	uint32_t pin_sck;
	uint32_t pin_miso;
	uint32_t pin_mosi;
	uint32_t pin_cs;
	spi_fast_speed_t frequency;
	spi_fast_order_t byte_order;
	spi_fast_mode_t mode;
	uint8_t spi_instance;
} spi_fast_init_params_t;

void spi_fast_init(const spi_fast_init_params_t* spi_params);
void spi_fast_write(const uint8_t* buf, uint8_t len);
void spi_fast_close();

#endif /* SPI_MASTER_FAST_H_ */
