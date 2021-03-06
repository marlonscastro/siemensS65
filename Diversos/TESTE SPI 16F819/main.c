#include <16F819.h>
#FUSES NOWDT, HS, NOPUT, MCLR, NOPROTECT, NOCPD, NOBROWNOUT
#use delay(clock=12000000)

#define SPI_MODE_0  (SPI_L_TO_H | SPI_XMIT_L_TO_H)
#define SPI_MODE_1  (SPI_L_TO_H)
#define SPI_MODE_2  (SPI_H_TO_L)
#define SPI_MODE_3  (SPI_H_TO_L | SPI_XMIT_L_TO_H) 

#include "driver3310.c"
long x;

#int_timer0
void isr(){
   x++;
   if (x == 1000){
      x = 0;
      output_toggle(pin_a4);
   }
}

void main(){
   setup_spi(SPI_MASTER | SPI_L_TO_H | SPI_XMIT_L_TO_H | SPI_CLK_DIV_16); // Configura��o SPI
   nokia_init();                    // Inicializa o LCD 3310 da Nokia  
   setup_adc_ports(NO_ANALOGS);     // Quais portas analogicas ser�o utilizadas
   setup_adc(ADC_OFF);              // Clock interno dividido por 64 
   setup_timer_1(T1_DISABLED);      // Timer 1 desativado
   set_adc_channel(0);      
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4|RTCC_8_bit);      //1.0 ms overflow 
   enable_interrupts(INT_TIMER0);

   enable_interrupts(GLOBAL);      
   while(true){
      delay_ms(100);
      nokia_gotoxy(0,0);      
      printf(nokia_printchar, "Teste Nokia3310");       
   }
}
