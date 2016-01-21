/*
 * uart_protocol.h
 *
 *  Created on: Jan 28, 2015
 *      Author: jaska
 */

#ifndef UART_PROTOCOL_H_
#define UART_PROTOCOL_H_

typedef enum {
	UART_PROTOCOL_STATE_IDLE,
	UART_PROTOCOL_STATE_FILE
} uart_protocol_state_e;

void uart_protocol_handle();
void uart_protocol_send(const uint8_t *buf, const uint16_t len);

#endif /* UART_PROTOCOL_H_ */
