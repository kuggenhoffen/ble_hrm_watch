/*
 * fs.c
 *
 *  Created on: Feb 3, 2015
 *      Author: jaska
 */

#include <stdint.h>
#include "flash.h"
#include "fs.h"

static fs_file_handle_s file_handle;
static uint8_t fs_work_buf[FS_PAGE_SIZE];
static fs_config_s fs_config;

void fs_filehandle_init();
fs_result_e fs_get_empty_file_descriptor(uint32_t *p_addr);
fs_result_e fs_flush();
void fs_write_handle(const uint16_t len, const uint8_t *buf);
fs_result_e fs_create(const uint8_t *p_filename);
fs_result_e fs_close();
fs_result_e fs_write(const uint8_t *buf, uint16_t length);

void fs_init(fs_init_config_s *init_config)
{
	fs_config.start_address = init_config->start_address;
	fs_config.file_size = init_config->file_size;
	fs_config.total_size = init_config->total_size;
	fs_config.page_size = init_config->page_size;
	fs_config.block_size = init_config->block_size;
	fs_filehandle_init();
}

void fs_filehandle_init()
{
	file_handle.data_address = 0;
	file_handle.data_length = 0;
	file_handle.data_start = 0;
	file_handle.fd_address = 0;
	file_handle.file_status = FS_FILE_NONE;
	file_handle.work_buf_ptr = fs_work_buf;
}

fs_result_e fs_get_empty_file_descriptor(uint32_t *p_addr)
{
	if (p_addr == 0)
		return FS_FAIL;

	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	uint32_t fd_address;
	uint32_t page_mask = 0xFFFFFFFF;
	for (fd_address = fs_config.start_address; fd_address < fs_config.total_size; fd_address += fs_config.file_size) {
		flash_read(fd_address, 4, (uint8_t*)&page_mask);
		if (page_mask == 0xFFFFFFFF) {
			*p_addr = fd_address;
			return FS_SUCCESS;
		}
	}
	return FS_FAIL;
}

fs_result_e fs_get_file_count(uint8_t *p_count)
{
	if (p_count == 0)
		return FS_FAIL;

	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	uint32_t fd_address;
	uint32_t page_mask = 0xFFFFFFFF;
	uint8_t count = 0;
	for (fd_address = fs_config.start_address; fd_address < fs_config.total_size; fd_address += fs_config.file_size) {
		flash_read(fd_address, 4, (uint8_t*)&page_mask);
		// Count non empty files
		if (page_mask != 0xFFFFFFFF) {
			count++;
		}
	}
	*p_count = count;
	return FS_SUCCESS;
}

fs_result_e fs_get_next_file_id(uint32_t *new_id)
{
	if (new_id == 0)
		return FS_FAIL;

	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	uint32_t fd_address;
	uint32_t temp_id = 0;
	uint32_t cur_id = 0;
	uint8_t count = 0;
	for (fd_address = fs_config.start_address; fd_address < fs_config.total_size; fd_address += fs_config.file_size) {
		flash_read(fd_address, 4, (uint8_t*)&temp_id);
		if ((temp_id != 0xFFFFFFFF) && (temp_id > cur_id)) {
			cur_id = temp_id;
		}
	}

	cur_id++;
	*new_id = cur_id;

	return FS_SUCCESS;
}

fs_result_e fs_get_file_headers(fs_file_descriptor_header_s *p_header_list, uint8_t count)
{
	if (p_header_list == 0 || count == 0)
		return FS_FAIL;

	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	uint8_t index = 0;
	fs_file_descriptor_header_s *phdr = p_header_list;
	uint32_t fd_address;
	uint32_t page_mask = 0xFFFFFFFF;
	for (fd_address = fs_config.start_address; fd_address < fs_config.total_size; fd_address += fs_config.file_size) {
		if (index == count)
			return FS_SUCCESS;
		flash_read(fd_address, 4, (uint8_t*)&page_mask);
		// Get non empty files
		if (page_mask != 0xFFFFFFFF) {
			// Read the whole header
			flash_read(fd_address, sizeof(fs_file_descriptor_header_s), (uint8_t*)phdr);
			// Increment destination pointer
			phdr++;
			// Increment index
			index++;
		}
	}
	return FS_SUCCESS;
}

fs_result_e fs_flush()
{
	if (file_handle.file_status != FS_FILE_OPEN_WRITE)
		return FS_FAIL;

	// Check if file has space left
	if (file_handle.data_address >= file_handle.fd_address + fs_config.file_size) {
		// No space left, fail
		return FS_FAIL;
	}

	// Get number of bytes in work buffer
	uint16_t length = file_handle.work_buf_ptr - fs_work_buf;

	if (length == 0)
		return FS_SUCCESS;

	// Write only as many bytes as can fit into the file
	if (file_handle.data_address + length > file_handle.fd_address + fs_config.file_size) {
		// Not enough space for whole buffer, write what fits
		length = file_handle.fd_address + fs_config.file_size - file_handle.data_address;
	}

	// Check if data address is at the page boundary
	if (file_handle.data_address % fs_config.page_size == 0) {
		// data address is at page boundary, so just write bytes and update file handle
		fs_write_handle(length, fs_work_buf);
	}
	else {
		// Get space left in page
		uint16_t page_left = fs_config.page_size - file_handle.data_address % fs_config.page_size;
		// Check if buffer fits in this page
		if (page_left < length) {
			// Buffer fits in this page, so write it and update file handle
			fs_write_handle(length, fs_work_buf);
		}
		else {
			// Not enough space for whole buffer, so rite as much as fits in this page
			fs_write_handle(page_left, fs_work_buf);
			// Check if there is space left for rest of buffer
			if (file_handle.data_address >= file_handle.fd_address + fs_config.file_size) {
				// No space left, fail
				return FS_FAIL;
			}
			else {
				fs_write_handle(length - page_left, fs_work_buf + page_left);
			}
		}
	}

	return FS_SUCCESS;
}

void fs_write_handle(const uint16_t len, const uint8_t *buf)
{
	if (file_handle.file_status != FS_FILE_OPEN_WRITE)
		return FS_FAIL;

	flash_write(file_handle.data_address, len, buf);
	file_handle.data_address += len;
	file_handle.data_length += len;
	file_handle.work_buf_ptr = fs_work_buf;
}

/*
 * File creation:
 * When creating a file, the memory is first scanned to find empty file descriptor at a 4 block boundary. The empty file
 * is denoted by the first 4 bytes of the 4 block file. If the byte is 0xFFFFFFFF the file is empty, otherwise it is used.
 * First empty file descriptor is chosen. The address is stored in the file handle in fd_address field. data_address field
 * will be initialized to fd_address + FS_PAGE_SIZE as the data space start address. data_length field
 * will be initialized to zero.
 * Lastly the filename will be written to the file descriptor page and the first 4 bytes is set to id.
 */
fs_result_e fs_create(const uint8_t *p_filename)
{
	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	// First find an empty file descriptor
	if (fs_get_empty_file_descriptor(&file_handle.fd_address) != FS_SUCCESS) {
		return FS_FAIL;
	}

	// Get the next file id
	uint32_t id = 0;
	if (fs_get_next_file_id(&id) != FS_SUCCESS) {
		return FS_FAIL;
	}

	// Initialize file handle struct
	file_handle.file_status = FS_FILE_OPEN_WRITE;
	file_handle.data_address = file_handle.fd_address + fs_config.page_size;
	file_handle.data_start = file_handle.data_address;
	file_handle.data_length = 0;
	file_handle.work_buf_ptr = fs_work_buf;

	// Initialize file descriptor header struct
	fs_file_descriptor_header_s fd_header;

	fd_header.id = id;
	fd_header.timestamp = datetime_get_time();
	fd_header.length = 0xFFFFFFFF;
	memset(fd_header.filename, 0x00, FS_FILENAME_SIZE);
	// Copy file name to fd header
	const uint8_t *src = p_filename;
	uint8_t *dst = fd_header.filename;
	while (*src != 0 && (src - p_filename) < FS_FILENAME_SIZE) {
		*dst++ = *src++;
	}

	// Write file descriptor header to flash
	flash_write(file_handle.fd_address, sizeof(fd_header), (uint8_t*)&fd_header);

	return FS_SUCCESS;
}

/*
 * File close:
 * Bytes left in write buffer are written to flash, and the data_length field is written to the file descriptor page.
 */
fs_result_e fs_close()
{
	if (file_handle.file_status == FS_FILE_OPEN_WRITE) {
		// Write any bytes left in work buffer
		fs_flush();

		// Get file descriptor header
		fs_file_descriptor_header_s fd_header;
		flash_read(file_handle.fd_address, sizeof(fs_file_descriptor_header_s), (uint8_t*)&fd_header);

		// Update length field
		fd_header.length = file_handle.data_length;

		// Write updated header
		flash_write(file_handle.fd_address, sizeof(fs_file_descriptor_header_s), (uint8_t*)&fd_header);
	}

	// Initialize file handle
	fs_filehandle_init();

	return FS_SUCCESS;
}

fs_result_e fs_write(const uint8_t *buf, uint16_t length)
{
	if (file_handle.file_status != FS_FILE_OPEN_WRITE)
		return FS_FAIL;

	uint16_t count = 0;
	// Copy from buf to work buffer until no more bytes left or work buffer is filled
	while (count < length && (file_handle.work_buf_ptr < (fs_work_buf + fs_config.page_size))) {
		*(file_handle.work_buf_ptr)++ = *buf++;
		count++;
	}

		// Check if work buffer is full
	if ((file_handle.work_buf_ptr == (fs_work_buf + fs_config.page_size))) {
		// Work buffer is full, flush it
		if (fs_flush() == FS_FAIL)
			return FS_FAIL;
	}

	// Check if buf still has left over bytes
	if (count < length) {
		// Copy left over bytes to work buffer
		while (count < length && (file_handle.work_buf_ptr < (fs_work_buf + fs_config.page_size))) {
			*(file_handle.work_buf_ptr)++ = *buf++;
			count++;
		}
	}

	return FS_SUCCESS;
}

fs_result_e fs_open(uint32_t file_id)
{
	if (file_handle.file_status != FS_FILE_NONE)
		return FS_FAIL;

	if (file_id == 0xFFFFFFFF)
		return FS_FAIL;

	fs_file_descriptor_header_s file_dh;

	// Search for the file_id
	uint32_t search_id = 0xFFFFFFFF;
	uint32_t fd_address;
	for (fd_address = fs_config.start_address; fd_address < fs_config.total_size; fd_address += fs_config.file_size) {
		flash_read(fd_address, 4, (uint8_t*)&search_id);
		if (search_id == file_id) {
			break;
		}
	}

	if (search_id != file_id) {
		return FS_FAIL;
	}

	// Found id match, so read rest of the descriptor
	flash_read(fd_address, sizeof(fs_file_descriptor_header_s), &file_dh);

	// Initialize file handle struct
	file_handle.data_address = fd_address + fs_config.page_size;
	file_handle.data_length = file_dh.length;
	file_handle.data_start = file_handle.data_address;
	file_handle.fd_address = fd_address;
	file_handle.file_status = FS_FILE_OPEN_READ;
	file_handle.work_buf_ptr = fs_work_buf;

	return FS_SUCCESS;
}

uint16_t fs_read(uint8_t *buf, uint16_t amount)
{
	if (buf == 0 || amount == 0)
		return 0;

	if (file_handle.file_status != FS_FILE_OPEN_READ)
		return 0;

	if (file_handle.data_address >= file_handle.data_start + file_handle.data_length)
		return 0;

	// Read amount or until the end, which ever is less
	if (file_handle.data_address + amount > file_handle.data_start + file_handle.data_length) {
		amount = file_handle.data_start + file_handle.data_length - file_handle.data_address;
	}

	flash_read(file_handle.data_address, amount, buf);

	file_handle.data_address += amount;

	return amount;
}

