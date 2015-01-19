/*
  
  u8g_arm.h
  
  Header file for u8g LPC11xx utility procedures 

  Universal 8bit Graphics Library
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/


#ifndef _U8G_ARM_H
#define _U8G_ARM_H

#include "u8g.h"
#include "spi_master.h"

#define CMD_SET_COLUMN_ADDRESS			0x15
#define CMD_SET_ROW_ADDRESS				0x75
#define	CMD_WRITE_RAM_COMMAND			0x5C
#define	CMD_READ_RAM_COMMAND			0x5D
#define	CMD_SET_REMAP_COM_LINE			0xA0
#define	CMD_SET_DISPLAY_START_LINE		0xA1
#define CMD_SET_DISPLAY_OFFSET			0xA2
#define CMD_SET_DISPLAY_MODE_OFF		0xA4
#define CMD_SET_DISPLAY_MODE_ON			0xA5
#define	CMD_SET_DISPLAY_MODE_NORMAL		0xA6
#define	CMD_SET_DISPLAY_MODE_INVERSE	0xA7
#define	CMD_SET_FUNCTION_SELECTION		0xAB
#define	CMD_SET_SLEEP_MODE_ON			0xAE
#define	CMD_SET_SLEEP_MODE_OFF			0xAF
#define	CMD_SET_PHASE_LENGTH			0xB1
#define	CMD_SET_FRONT_CLK_DIVIDER		0xB3
#define CMD_SET_VSL						0xB4
#define CMD_SET_GPIO					0xB5
#define	CMD_SET_SECOND_PRECHARGE_PERIOD	0xB6
#define	CMD_LUT_FOR_GRAYSCALE			0xB8
#define	CMD_USE_BUILT_IN_LUT			0xB9
#define	CMD_SET_PRECHARGE_VOLTAGE		0xBB
#define	CMD_SET_VCOMH_VOLTAGE			0xBE
#define CMD_SET_CONTRAST_ABC			0xC1
#define	CMD_SET_CONTRAST_MASTER			0xC7
#define	CMD_SET_MULTIPLEX_RATIO			0xCA
#define	CMD_COMMAND_LOCK				0xFD

static void spi_init(spi_master_event_handler_t spi_master_event_handler);

extern uint16_t u8g_pin_a0;
extern uint16_t u8g_pin_cs;
extern uint16_t u8g_pin_rst;
uint8_t u8g_com_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr);
void display_test();
void update_values(uint8_t newhr, uint16_t newtimer);
// Graphics driver instance
u8g_t u8g;

#endif


