/*
 * flash.h
 *
 *  Created on: Jan 25, 2015
 *      Author: jaska
 */

#ifndef FLASH_H_
#define FLASH_H_

#define FLASH_CMD_READ_STATUS_REG1						0x05
#define FLASH_CMD_READ_STATUS_REG2						0x35
#define FLASH_CMD_READ_STATUS_REG3						0x33
#define FLASH_CMD_WRITE_ENABLE							0x06
#define	FLASH_CMD_WRITE_ENABLE_VOLATILE_STATUS_REG		0x50
#define	FLASH_CMD_WRITE_DISABLE							0x04
#define	FLASH_CMD_PAGE_PROGRAM							0x02
#define FLASH_CMD_SECTOR_ERASE							0x20
#define	FLASH_CMD_BLOCK_ERASE							0xD8
#define	FLASH_CMD_CHIP_ERASE							0xc7
#define	FLASH_CMD_READ_DATA								0x03
#define	FLASH_CMD_DEEP_POWERDOWN						0xb9
#define	FLASH_CMD_RELEASE_POWERDOWN						0xab

uint8_t flash_spi_read_status_register(uint8_t reg);
void flash_spi_write_enable();
int8_t flash_spi_page_write(const uint8_t* buf, const uint32_t len, const uint32_t address);
int8_t flash_spi_read(const uint8_t* buf, const uint32_t len, const uint32_t address);
void flash_spi_chip_erase();
void flash_spi_init();
uint8_t flash_spi_transfer_finished();
void flash_test();

void flash_read(uint32_t addr, uint32_t size, uint8_t *dst);
void flash_write(uint32_t addr, uint32_t size, const uint8_t *src);
void flash_erase(uint32_t addr, uint32_t size);

#endif /* FLASH_H_ */
