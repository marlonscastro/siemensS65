#include <16F876.h>
#FUSES NOWDT, HS, NOPUT, NOBROWNOUT, NOLVP, NOCPD, NOWRT, NODEBUG, NOPROTECT
#use delay(clock=16000000)
#include "S65-L2F50.c"

#define LED_STATUS   pin_a1

#define SPI_MODE_0  (SPI_L_TO_H | SPI_XMIT_L_TO_H)
#define SPI_MODE_1  (SPI_L_TO_H)
#define SPI_MODE_2  (SPI_H_TO_L)
#define SPI_MODE_3  (SPI_H_TO_L | SPI_XMIT_L_TO_H)

int tempo;

#int_TIMER0
void  TIMER0_isr(void){
   tempo++;
   if(tempo == 70){
      tempo=0;
      output_toggle(LED_STATUS);
   }
}
void main(void){
  int i;
  char txt[30];
  setup_spi(SPI_MASTER | SPI_MODE_0 | SPI_CLK_DIV_4);   
  //setup_spi(SPI_SS_DISABLED);
  setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256|RTCC_8_bit);
  
  // Config para PWM 62,5 Khz para o BackLight
  setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);      //16.3 ms overflow
  setup_timer_2(T2_DIV_BY_1,63,1);      //16.0 us overflow, 16.0 us interrupt
  setup_ccp1(CCP_PWM);
  set_pwm1_duty(47);   // 47% ?
  //-----------------
  
  
  setup_adc_ports(NO_ANALOGS);
  setup_adc(ADC_OFF);
  enable_interrupts(INT_TIMER0);
  enable_interrupts(GLOBAL);  
  
  delay_ms(500);
  lcd_init();
  fill_screen(BLACK);
  delay_ms(500);  
  fill_screen(RED);
  delay_ms(500);
  fill_screen(GREEN);
  delay_ms(500);
  fill_screen(BLUE);
  delay_ms(500);
  fill_screen(BLACK);  
  drawBox(90, 10, 20, 20, GREEN, RETRATO);
  drawBox(30, 40, 20, 20, BLUE, PAISAGEM);  
  drawBox(50, 80, 50, 50, RED, PAISAGEM);    
  for(i=0;i<100;i++){
      Putpixel(i, 10, YELLOW, RETRATO);
      Putpixel(i, 10, YELLOW, PAISAGEM);      
  }
  sprintf(txt, "Rbj.net\0");
  putstr(0, 4, txt, BLUE, WHITE, FONT812, RETRATO);
  putstr(0, 20, txt, RED, WHITE, FONT88, RETRATO);
  putstr(0, 30, txt, BLACK, WHITE, FONT58, RETRATO);  
  
  sprintf(txt, "Marlon S65\0");
  putstr(20, 20, txt, BLUE, WHITE, FONT812, PAISAGEM);
  putstr(20, 40, txt, RED, WHITE, FONT88, PAISAGEM);
  sprintf(txt, "ABCDEFGHIJKLMNOPQRSTUVWXYZ012\0");
  putstr(0, 60, txt, BLACK, WHITE, FONT58, PAISAGEM);
  sprintf(txt, "abcdefghijklmnopqrstuvwxyz345\0");
  putstr(0, 70, txt, BLACK, WHITE, FONT58, PAISAGEM);
  while(true){}
}

