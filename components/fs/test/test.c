/*
 * test.c
 *
 *  Created on: Feb 3, 2015
 *      Author: jaska
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "flash.h"
#include "fs.h"

#define TEST_SIZE	256*2*4

uint8_t *flash_data;

uint32_t datetime_get_time()
{
	return time(0);
}

void check_address_bounds(uint32_t address)
{
	if ((flash_data + address < flash_data) || (flash_data + address > flash_data + TEST_SIZE)) {
		printf("ASSERT: Address out-of-bounds");
		exit(0);
	}
}

void flash_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
	printf("read addr=%d(%d), size=%d\r\n", addr, (uintptr_t)flash_data+addr, size);
	check_address_bounds(addr);
	//memcpy(dst, flash_data + addr, size);
	while (size > 0) {
		*dst++ = flash_data[addr];
		addr++;
		size--;
	}
}

void flash_write(uint32_t addr, uint32_t size, const uint8_t *src)
{
	printf("write addr=%d(%d), size=%d\r\n", addr, (uintptr_t)flash_data+addr, size);
	check_address_bounds(addr);
	//memcpy(flash_data + addr, src, size);
	while (size > 0) {
		flash_data[addr] = *src++;
		addr++;
		size--;
	}
}

void flash_erase(uint32_t addr, uint32_t size)
{
	memset(flash_data + addr, 0xFF, size);
}

uint8_t test_buf[256];

void main()
{
	fs_init_config_s iniconf;
	iniconf.total_size = TEST_SIZE;
	iniconf.block_size = 256;
	iniconf.page_size = 256;
	iniconf.start_address = 0;
	iniconf.file_size = 3*iniconf.block_size;

	// Reserve memory for test data
	flash_data = malloc(TEST_SIZE);
	if (!flash_data) {
		printf("Unable to allocate enough memory\r\n");
		return;
	}
	printf("mem start=%d, end=%d\r\n", flash_data, flash_data + TEST_SIZE);

	memset(flash_data, 0xFF, TEST_SIZE);

	fs_init(&iniconf);

	printf("Test create 1\r\n");
	fs_file_handle_s fh;
	fs_create("TestiFile.txt");

	int i;
	for (i = 0; i < 128; i++) {
		test_buf[i] = i;
		test_buf[128+i] = i;
	}

	printf("Test write 1\r\n");
	fs_write(test_buf, 256);

	memset(test_buf, 0xBE, 256);
	printf("Test write 2\r\n");
	fs_write(test_buf, 256);

	fs_close();

	printf("Test create 2\r\n");
	fs_create("tokafiles.txt");

	for (i = 0; i < 128; i++) {
		test_buf[i] = 128-i;
	}

	printf("Test write 3\r\n");
	fs_write(test_buf, 128);

	memset(test_buf, 0xAD, 256);
	printf("Test write 4\r\n");
	fs_write(test_buf, 160);

	fs_close();

	i = 0;
	int a = 0;
	for (i = 0; i < 6; i++) {
		for (a = 0; a < 256; a++) {
			if (a % 16 == 0)
				printf("%x: ", &(flash_data[i*256 + a]));
			printf("%02x ", flash_data[i*256 + a]);
			if ((a + 1) % 16 == 0)
				printf("\r\n");
		}
		printf("\r\n");
	}

	free(flash_data);

}
