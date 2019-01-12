#include <16F876.h>
#FUSES NOWDT, HS, NOPUT, NOBROWNOUT, NOLVP, NOCPD, NOWRT, NODEBUG, NOPROTECT
#use delay(clock=16000000)
#USE SPI (MASTER, SPI1, MODE=0, BITS=8, STREAM=SPI_1, MSB_FIRST, BAUD=1000000)

#include "disp.c"

#define      LED_STATUS         pin_a1

#define SPI_MODE_0  (SPI_L_TO_H | SPI_XMIT_L_TO_H)
#define SPI_MODE_1  (SPI_L_TO_H)
#define SPI_MODE_2  (SPI_H_TO_L)
#define SPI_MODE_3  (SPI_H_TO_L | SPI_XMIT_L_TO_H)

int tempo;

#int_TIMER0
void  TIMER0_isr(void){
   tempo++;
   if(tempo == 46){   //  46 * 21,8ms =~ 1002,8ms ou 1,002 segundos 
      tempo=0;
      output_toggle(LED_STATUS);
   }
}

BYTE const pot10[4] ={10,100,1000,10000};

void ubi2dec( int16 val,  char *strg, int nzero);
void ubi2dec( int16 val,  char *strg, int nzero)
/*
  convert unsigned int16 value to a character string
  stringlength has to be 6 character 65536 + the final 0
  
  if nzero==0, no leading zeros
  if nzero==1, with leading zeros  
*/
{
  int dec,idx=4,odx=0;
  
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
/*

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
*/

void main(void){
  /*uint8_t i;
  uint16_t cnt;
  char nr[7];
  
  char txt[]=". Zeile L2F\0";
*/   
   setup_spi(SPI_MASTER | SPI_MODE_0 | SPI_CLK_DIV_4); 
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256|RTCC_8_bit);      //21.8 ms overflow
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   
   output_bit(LED_STATUS,1);
   port_init();

  lcd_init_c();
  backcolor=0xFFFF;
  textcolor=0x0000;
  fill_screen(0x0000);

  /*  cnt=0;
  while (1)   
  {
    ubi2dec(cnt,nr,1);
    print_str(0       ,(cnt*CHAR_H) % (12*14),nr);
    print_str(5*CHAR_W,(cnt*CHAR_H) % (12*14),txt);

    textcolor=cnt;
    
    cnt++;
   _delay_ms(100);
  }*/
   while(true){}
}

