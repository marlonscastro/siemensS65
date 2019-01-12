/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control for L2F50 Display with Epson Controller
**
*********************************************************************** */
/*********************************************
* Chip type           : ATMEGA128
* Clock frequency     : clock 16 MHz
*********************************************/
#include <avr/io.h>
#include <inttypes.h>

#include "disp.h"

const uint16_t pot10[4] ={10,100,1000,10000};

void ubi2dec( uint16_t val,  char *strg, uint8_t nzero);
void ubi2dec( uint16_t val,  char *strg, uint8_t nzero)
/*
  convert unsigned int16 value to a character string
  stringlength has to be 6 character 65536 + the final 0
  
  if nzero==0, no leading zeros
  if nzero==1, with leading zeros  
*/
{
  uint8_t dec,idx=4,odx=0;
  
  do
  {
    idx--;
    for (dec=0; val>=pot10[idx]; val-=pot10[idx])
    {
      dec++;
    }
    nzero+=dec;
    if (nzero) strg[odx++]='0'+dec;
  } while(idx);
  strg[odx++]='0'+val;
  strg[odx]=0;
}


void print_str(uint8_t x, uint8_t y, char *txt);
void print_str(uint8_t x, uint8_t y, char *txt)
{
  uint8_t i=0;
  while (txt[i]!=0)
  {
    put_char(x+i*CHAR_W,y,txt[i],1);
    i++;
  }

}

int main(void)
{
  uint8_t i;
  uint16_t cnt;
  char nr[7];
  
  char txt[]=". Zeile L2F\0";

  port_init();

  lcd_init_c();
  backcolor=0xFFFF;
  textcolor=0x0000;
  fill_screen(0x0780);

  cnt=0;
  while (1)   
  {
    ubi2dec(cnt,nr,1);
    print_str(0       ,(cnt*CHAR_H) % (12*14),nr);
    print_str(5*CHAR_W,(cnt*CHAR_H) % (12*14),txt);

    textcolor=cnt;
    
    cnt++;
   _delay_ms(100);
  }
  return(0);	  
}

