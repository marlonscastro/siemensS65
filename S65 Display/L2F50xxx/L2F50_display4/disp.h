/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control
**
*********************************************************************** */
// disp.h
#ifndef _DISP_H
#define _DISP_H

#define F_CPU 16000000

#include "avr/io.h"
#include <avr/delay.h>
#include <inttypes.h>

#define DISP_W 132
#define DISP_H 176
#define CHAR_H 14
#define CHAR_W 8
#define TEXT_COL 16
#define TEXT_ROW 12



#define LCD_CS	   PB0
#define LCD_RESET  PB6
#define LCD_RS	   PB7
#define LCD_MOSI   PB2
#define LCD_MISO   PB3
#define LCD_SCK    PB1



#define DATCTL 0xBC         // Data Control (data handling in RAM)
#define DISCTL 0xCA         // Display Control
#define GCP64 0xCB          // pulse set for 64 gray scale
#define GCP16 0xCC          // pulse set for 16 gray scale
#define OSSEL 0xD0          // Oscillator select
#define GSSET 0xCD          // set for gray scales
#define ASCSET 0xAA         // aerea scroll setting
#define SCSTART 0xAB        // scroll start setting
#define DISON 0xAF          // Display ON (no parameter)
#define DISOFF 0xAE         // Display OFF (no parameter)
#define DISINV 0xA7         // Display Invert (no parameter)
#define DISNOR 0xA6         // Display Normal (no parameter)
#define SLPIN  0x95         // Display Sleep (no parameter)
#define SLPOUT 0x94         // Display out of sleep (no parameter)
#define RAMWR 0x5C          // Display Memory write
#define PTLIN 0xA8          // partial screen write

#define SD_CSET 0x15        // column address setting
#define SD_PSET 0x75        // page address setting





extern uint16_t textcolor;    // color of text
extern uint16_t backcolor;    // color of background



/////////////////////////////// function prototypes ////////////////////////

void port_init(void);

void lcd_write(uint8_t);
void lcd_cmd(uint8_t dat);
void lcd_dat0(uint8_t dat);
void lcd_dat16(uint16_t dat);

void lcd_cspulse(void);

void lcd_init_c(void);
void fill_screen(uint16_t color);
void put_char(uint8_t x, uint8_t y, char c, uint8_t rot);

#endif
// end of file disp.h
