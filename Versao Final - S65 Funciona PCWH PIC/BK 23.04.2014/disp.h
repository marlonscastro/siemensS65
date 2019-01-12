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

#include "lcd.h"

#define DISP_W 132
#define DISP_H 176
#define CHAR_H 14
#define CHAR_W 8
#define TEXT_COL 16
#define TEXT_ROW 12


extern int16 textcolor;  // color of border around text
extern int16 backcolor;    // color of background



/////////////////////////////// function prototypes ////////////////////////


void lcd_wrcmd16(uint16_t);
void lcd_wrdat16(uint16_t);
void fill_screen(int16 color);
void put_char(int8 x, int8 y, char c, int8 rot);
//void lcd_init(void);         // LCD initialise

#endif
// end of file disp.h
