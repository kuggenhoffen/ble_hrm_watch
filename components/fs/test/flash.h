/*
 * flash.h
 *
 *  Created on: Feb 3, 2015
 *      Author: jaska
 */

#ifndef FLASH_H_
#define FLASH_H_

void flash_read(const uint32_t addr, const uint32_t size, uint8_t *dst);
void flash_write(const uint32_t addr, const uint32_t size, const uint8_t *src);
void flash_erase(const uint32_t addr, const uint32_t size);

#endif /* FLASH_H_ */
