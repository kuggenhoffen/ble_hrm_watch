/*
 * uart_protocol.c
 *
 *  Created on: Jan 28, 2015
 *      Author: jaska
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_error.h"
#include "app_uart.h"
#include "uart_protocol.h"
#include "afproto.h"
#include "hrm_debug.h"
#include "fs.h"

#define UART_PROTOCOL_RX_SIZE			32
#define UART_PROTOCOL_TX_SIZE			128
#define UART_PROTOCOL_TXRX_SIZE			128

static uint8_t protocol_rx_msg_buf[UART_PROTOCOL_RX_SIZE];
static uint8_t protocol_tx_msg_buf[UART_PROTOCOL_TX_SIZE];
static uint8_t protocol_txrx_buf[UART_PROTOCOL_TXRX_SIZE];

static volatile uint8_t *protocol_tx_buf_ptr;
static volatile uint8_t protocol_tx_buf_len = 0;

static uint8_t *p_rx_ptr;
uint8_t* p_msg_buffer = 0;
uint8_t msg_type = 0;
uint16_t uart_count = 0;

static uart_protocol_state_e uart_protocol_state = UART_PROTOCOL_STATE_IDLE;

void uart_protocol_init()
{
	p_rx_ptr = protocol_txrx_buf;
	uart_protocol_state = UART_PROTOCOL_STATE_IDLE;
}

void uart_protocol_handle()
{
	uint8_t rx_byte;
	uint8_t packet_length = 0;
	int8_t result = 0;
	uint8_t rx_count = 0;

	// Read uart fifo into work buffer
	while (app_uart_get(&rx_byte) == NRF_SUCCESS) {
		*p_rx_ptr++ = rx_byte;
		rx_count++;
		// Check if next byte overflows work buffer
		if (p_rx_ptr == protocol_txrx_buf + UART_PROTOCOL_TXRX_SIZE) {
			break;
		}
	}

	// Check how many received, if none we don't need to do anything
	if (rx_count != 0) {
		result = afproto_get_data(protocol_txrx_buf, p_rx_ptr - protocol_txrx_buf - 1, protocol_rx_msg_buf, &packet_length);
		if (result >= 0) {
			// Valid message found, handle message
			protocol_rx_msg_buf[packet_length + 1] = 0;
			uart_protocol_packet(packet_length);
			// Discard message bytes from buffer
			uint16_t new_len = p_rx_ptr - protocol_txrx_buf - result;
			memcpy(protocol_txrx_buf, protocol_txrx_buf + result + 1, new_len);
			p_rx_ptr = protocol_txrx_buf + new_len;
		}
		else {
			// No valid message, discard packet_length amount of bytes from start
			uint16_t new_len = p_rx_ptr - protocol_txrx_buf - packet_length;
			memcpy(protocol_txrx_buf, protocol_txrx_buf + packet_length, new_len);
			p_rx_ptr = protocol_txrx_buf + new_len;
		}
	}
}

uint8_t str_to_uint32(uint8_t *buf, uint8_t len, uint32_t *dst)
{
	if (dst == 0)
		return 0;

	uint32_t val = 0;

	while (*buf != 0) {
		if (*buf >= '0' && *buf <= '9') {
			val = val * 10 + (*buf - '0');
			buf++;
			if (len-- == 0)
				break;
		}
		else {
			break;
		}
	}
	*dst = val;
	return 1;
}

uint8_t uart_protocol_send_file_packet()
{
	uint8_t *p_buf = malloc(5 + 64);
	if (p_buf == 0)
		return 0;

	p_buf[0] = 'D';
	p_buf[1] = 'A';
	p_buf[2] = 'T';
	p_buf[3] = 'A';
	p_buf[4] = ':';

	// Read data
	uint8_t len = fs_read(p_buf + 5, 64);

	// Check if read successful
	if (len > 0) {
		uart_protocol_send(p_buf, len + 5);
	}

	free(p_buf);

	return len;
}

void uart_protocol_packet(uint8_t len)
{
	if (strncmp("FILES", protocol_rx_msg_buf, 5) == 0) {
		uint8_t fbuf[FS_FILENAME_SIZE + 16];
		uint8_t count = 0;
		fs_get_file_count(&count);

		if (count > 0) {
			fs_file_descriptor_header_s *file_headers = malloc(sizeof(fs_file_descriptor_header_s) * count);
			if (fs_get_file_headers(file_headers, count) == FS_SUCCESS) {
				int16_t len = 0;
				while (count--) {
					len = sprintf(fbuf, "FILE:%s:%d:%d", file_headers[count].filename,file_headers[count].length, file_headers[count].id);
					if ((len > 0) && (len < FS_FILENAME_SIZE + 16)) {
						uart_protocol_send(fbuf, len);
					}
				}
			}
			free(file_headers);
		}
	}
	// GETFILE message
	// Format: GETFILE:#
	// Where # is the file id to download
	// If # is not valid integer number, the first file will be sent if one exists
	else if (strncmp("GETFILE", protocol_rx_msg_buf, 7) == 0) {
		if (len >= 9) {
			uint32_t id = 0;

			// Close any existing open file
			if (uart_protocol_state == UART_PROTOCOL_STATE_FILE) {
				fs_close();
			}

			// Decode integer from string
			if (str_to_uint32(protocol_rx_msg_buf + 8, len - 8, &id) == 1) {
				// Open file
				if (fs_open(id) == FS_SUCCESS) {
					uart_protocol_state = UART_PROTOCOL_STATE_FILE;
					uart_protocol_send_file_packet();
				}
			}
		}
	}
	else if (strncmp("ACK", protocol_rx_msg_buf, 3) == 0) {
		if (uart_protocol_state == UART_PROTOCOL_STATE_FILE) {
			if (uart_protocol_send_file_packet() == 0) {
				uart_protocol_state = UART_PROTOCOL_STATE_IDLE;
				// Close file
				fs_close();
			}
		}
	}
	else if (strncmp("DBGON", protocol_rx_msg_buf, 5) == 0) {
		hrm_debug_enable();
	}
	else if (strncmp("DBGOFF", protocol_rx_msg_buf, 6) == 0) {
		hrm_debug_disable();
	}
}

void uart_protocol_send(const uint8_t *buf, const uint16_t len)
{
	uint16_t dst_len;

	uint8_t *p = buf;
	afproto_frame_data(p, len, protocol_tx_msg_buf, &dst_len);

	// Initialize tx buffer variables
	protocol_tx_buf_ptr = protocol_tx_msg_buf;
	protocol_tx_msg_buf[dst_len + 1] = 0;
	printf("%s\r\n", protocol_tx_msg_buf);
}

