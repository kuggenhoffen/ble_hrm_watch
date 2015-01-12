/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef BOARD_CUSTOM_H
#define BOARD_CUSTOM_H

// LEDs definitions for BLE_WATCH
#define LEDS_NUMBER    0

#define BUTTONS_NUMBER 4

#define BUTTON_1       9
#define BUTTON_2       0
#define BUTTON_3       10
#define BUTTON_4       23
#define BUTTON_PULL    NRF_GPIO_PIN_PULLDOWN

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3
#define BSP_BUTTON_3   BUTTON_4

#define BSP_BUTTON_0_MASK (1<<BSP_BUTTON_0)
#define BSP_BUTTON_1_MASK (1<<BSP_BUTTON_1)
#define BSP_BUTTON_2_MASK (1<<BSP_BUTTON_2)
#define BSP_BUTTON_3_MASK (1<<BSP_BUTTON_3)

#define BUTTONS_MASK   (BSP_BUTTON_0_MASK | BSP_BUTTON_1_MASK | BSP_BUTTON_2_MASK | BSP_BUTTON_3_MASK)

#define RX_PIN_NUMBER  15
#define TX_PIN_NUMBER  13
#define CTS_PIN_NUMBER 14
#define RTS_PIN_NUMBER 12/*
#define RX_PIN_NUMBER  11
#define TX_PIN_NUMBER  9
#define CTS_PIN_NUMBER 10
#define RTS_PIN_NUMBER 8*/
#define HWFC           true


// Shared SPI signals
#define SPI_SCK_PIN       3     /**< SPI clock GPIO pin number. */
#define SPI_MOSI_PIN      2     /**< SPI Master Out Slave In GPIO pin number. */
#define SPI_MISO_PIN      25     /**< SPI Master In Slave Out GPIO pin number. */
// Display signals
#define SPI_CS_DISPLAY	6             /**< SPI Slave Select GPIO pin number. */
#define DISPLAY_RESET	4
#define	DISPLAY_DC		5
#define DISPLAY_EN		7
// Flash memory signals
#define SPI_CS_MEMORY	29
#define	MEM_HOLD		28
#define MEM_WP			24

#define VBAT_SEN

#endif // BOARD_CUSTOM_H
