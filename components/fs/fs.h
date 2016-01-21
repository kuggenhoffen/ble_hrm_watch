/*
 * fs.h
 *
 *  Created on: Feb 3, 2015
 *      Author: jaska
 */

#ifndef FS_H_
#define FS_H_


#define FS_BLOCK_SIZE		65536
#define FS_PAGE_SIZE		256
#define FS_MAX_FILE_SIZE	4*FS_BLOCK_SIZE
#define FS_FILENAME_SIZE	16


typedef enum {
	FS_SUCCESS,
	FS_FAIL
} fs_result_e;

typedef enum {
	FS_FILE_OPEN_WRITE,
	FS_FILE_OPEN_READ,
	FS_FILE_NONE
} fs_file_status_e;

typedef struct {
	uint32_t fd_address;
	uint32_t data_address;
	uint32_t data_length;
	fs_file_status_e file_status;
	uint8_t *work_buf_ptr;
	uint32_t data_start;
} fs_file_handle_s;

typedef struct {
	uint32_t id;
	uint8_t filename[FS_FILENAME_SIZE];
	uint32_t timestamp;
	uint32_t length;
} fs_file_descriptor_header_s;

typedef struct {
	uint32_t start_address;
	uint32_t total_size;
	uint32_t file_size;
	uint32_t page_size;
	uint32_t block_size;
} fs_config_s;

typedef struct {
	uint32_t start_address;
	uint32_t total_size;
	uint32_t file_size;
	uint32_t page_size;
	uint32_t block_size;
} fs_init_config_s;

void fs_init(fs_init_config_s *init_config);
void fs_filehandle_init();
fs_result_e fs_get_file_count(uint8_t *p_count);
fs_result_e fs_get_file_headers(fs_file_descriptor_header_s *p_header_list, uint8_t count);
fs_result_e fs_create(const uint8_t *p_filename);
fs_result_e fs_close();
fs_result_e fs_write(const uint8_t *buf, uint16_t length);

#endif /* FS_H_ */

/*
Simple file system:
Each file uses 4 blocks (4*65536 bytes). First page of the 4 block file contains the file descriptor.
File descriptor contains file name (uint8_t[16]), uint16_t (timestamp), uint32_t (length)
Data starts from the second page of the 4 block file.

File creation:
When creating a file, the memory is first scanned to find empty file descriptor at a 4 block boundary. The empty file
is denoted by the first byte of the 4 block file. If the byte is 0xFF the file is empty, otherwise it is used.
First empty file descriptor is chosen. The address is stored in the file handle in fd_address field. data_address field
will be initialized to fd_address + FS_PAGE_SIZE as the data space start address. data_length field
will be initialized to zero.
Lastly the filename will be written to the file descriptor page and the first byte is set to 0x00.

File write:
Write buffer is appended to work buffer. If appending would overflow the buffer, the buffer is filled, written to flash
at the data_address address in file handle, and the overflowed bytes are copied to the start of the work buffer. The
data_length field is also increased by FS_PAGE_SIZE bytes.

File close:
Bytes left in write buffer are written to flash, and the data_length field is written to the file descriptor page.
 */
