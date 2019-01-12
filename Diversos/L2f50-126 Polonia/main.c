#include <avr\io.h>
#include <avr\delay.h>
#include <inttypes.h>
#include <math.h>
#include <avr\pgmspace.h>
#include <avr\interrupt.h>

#include "disp.h"
#include "disp.k"


#define DE0_RX  PORTD &= ~(1 << 5)
#define DE0_TX  PORTD |= (1 << 5)
#define DE1_RX  PORTD &= ~(1 << 4)
#define DE1_TX  PORTD |= (1 << 4)


static void Putpixel(uint8_t x, uint8_t y, uint16_t Color);
static void Pisz_8x14_transparent(char *text, uint8_t Start_x, uint8_t Start_y, uint16_t Colot_Text);
static void USART0_Transmit(uint8_t data);
static void USART1_Transmit(uint8_t data);
static uint8_t crc8_calc(uint8_t *data, uint8_t size);
static void M25P16_Read(uint8_t *data, uint16_t size, uint32_t addr);
static void M25P16_Write(uint8_t *data, uint16_t size, uint32_t addr);
static uint8_t SPI_write_Read(uint8_t data);


	
volatile uint8_t RX0_Buffer[160];
volatile uint8_t RX1_Buffer[160];

volatile uint8_t RX0_Buf_Pos;
volatile uint8_t RX1_Buf_Pos;

volatile uint8_t TX_Buffer[32];


volatile uint16_t Font_16x16[8][16];


volatile uint32_t addr;

volatile uint8_t Table[256];



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
  //uint8_t i;


  PORTA = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);
  PORTB = (1 << 4);
  PORTC = 0x00;
  PORTD = (1 << 4) | (1 << 5);

  DDRA = 0x00;
  DDRB = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);
  DDRC = 0x00;
  DDRD = (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);  


  UCSR0A = 0x00;
  UCSR0B = (1 << 3) | (1 << 4) | (1 << RXCIE0);
  UCSR0C = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);  
  UBRR0H = 0;
  UBRR0L = 51;                       //19200baud

  UCSR1A = 0x00;
  UCSR1B = (1 << 3) | (1 << 4) | (1 << RXCIE1);
  UCSR1C = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);  
  UBRR1H = 0;
  UBRR1L = 51;                       //19200baud




  
  uint8_t i;
  uint32_t j;
  uint32_t l;
  char nr[32];
  
  port_init();

  lcd_init_c();
  backcolor=0xFFFF;
  textcolor=0x0000;
  fill_screen(0x0008);


  DE0_RX;
  DE1_RX;


  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0x06);

  PORTB |= (1 << 4);
  
  sei();
    
  
  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0xC7);

  PORTB |= (1 << 4);
  
  sei();
  

  cli();
  
  PORTB &= ~(1 << 4);

  SPI_write_Read(0x05);

  while((SPI_write_Read(0xFF) & 0x01) == 1);

  PORTB |= (1 << 4);
  
  sei();
  
  
  
                /*PORTB &= ~(1 << 4);
                SPI_write_Read(0x06);
                PORTB |= (1 << 4);
				
                _delay_us(100);
				
				Table[0] = 0x33;
				Table[1] = 0x33;
				Table[2] = 0x33;
				Table[3] = 0x33;
				
                M25P16_Write((uint8_t*)(Table), 256, 32768);*/
				
  
  
  Pisz_8x14_transparent("Skasowane\0", 0, 0, 0xFFFF);


  while (1)
    {
	sei();
	
	ubi2dec(addr,nr,1);
	Pisz_8x14_transparent(nr, 100, 0, 0xFFFF);


/*  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0x05);
  Table[0] = SPI_write_Read(0xFF);
  Table[1] = SPI_write_Read(0xFF);
  Table[2] = SPI_write_Read(0xFF);

  PORTB |= (1 << 4);
  
  sei();
    
	
	ubi2dec(Table[0],nr,1);
	Pisz_8x14_transparent(nr, 100, 0, 0xFFFF);
	
	ubi2dec(Table[1],nr,1);
	Pisz_8x14_transparent(nr, 100, 14, 0xFFFF);
	
	ubi2dec(Table[2],nr,1);
	Pisz_8x14_transparent(nr, 100, 28, 0xFFFF);
	

  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0x9F);
  Table[0] = SPI_write_Read(0xFF);
  Table[1] = SPI_write_Read(0xFF);
  Table[2] = SPI_write_Read(0xFF);

  PORTB |= (1 << 4);
  
  sei();
    
	
	ubi2dec(Table[0],nr,1);
	Pisz_8x14_transparent(nr, 100, 58, 0xFFFF);
	
	ubi2dec(Table[1],nr,1);
	Pisz_8x14_transparent(nr, 100, 72, 0xFFFF);
	
	ubi2dec(Table[2],nr,1);
	Pisz_8x14_transparent(nr, 100, 86, 0xFFFF);*/
	}
  
  return 0;
}
//-------------------------------------------------------------------------
static void Putpixel(uint8_t x, uint8_t y, uint16_t Color){
  cli();
  
  PORTB &= ~_BV(LCD_CS);  // select display
  
  lcd_cmd(SD_CSET);
  lcd_dat0(0x08+y);  // start is 8, not 0
  lcd_dat0(0x01);
  lcd_dat0(0x08+y);  // end is 00x8B = 0x83+8
  lcd_dat0(0x01);
  
  lcd_cmd(SD_PSET);
  lcd_dat0(175-x);
  lcd_dat0(175-x);

  lcd_cmd(RAMWR);
  lcd_dat16(Color);

  PORTB |= _BV(LCD_CS);  // deselect display

  sei();   
}
//-------------------------------------------------------------------------
static void Pisz_8x14_transparent(char *text, uint8_t Start_x, uint8_t Start_y, uint16_t Colot_Text)
{
uint8_t h;
uint8_t p;
uint8_t mask;
uint8_t ch;

/*while (*text != 0)
    {
    for (h=0; h<14; h++)
        {        
        ch=pgm_read_byte(&ascii_tab[*text-32][h]);
		
        mask=0x80;
        for (p=0; p<8; p++)
            {
            if (ch&mask)
                {
                Putpixel(Start_x+p, Start_y+h, Colot_Text);
                }
            else
                {
                Putpixel(Start_x+p, Start_y+h, 0x0000);
                }
             mask=mask >> 1;
            }
        }
		
    text++;
	Start_x = Start_x + 8;
	}*/
	
	
while (*text != 0)
    {
    for (h=0; h<14; h++)
        {        
        ch=pgm_read_byte(&ascii_tab[*text-32][h]);
		
        mask=0x80;
        for (p=0; p<8; p++)
            {
            if (ch&mask)
				Font_16x16[p][h] = Colot_Text;
            else
                Font_16x16[p][h] = 0x0000;
 
             mask=mask >> 1;
            }
        }
		
	
	/*//cli();
	
    PORTB &= ~_BV(LCD_CS);  // select display
	
    lcd_cmd(SD_CSET);
    lcd_dat0(0x08+Start_y);  // start is 8, not 0
    lcd_dat0(0x01);
    lcd_dat0(0x08+Start_y+13);  // end is 00x8B = 0x83+8
    lcd_dat0(0x01);
  
    lcd_cmd(SD_PSET);
    lcd_dat0(175-(Start_x+8));
    lcd_dat0(175-Start_x);
	

    lcd_cmd(RAMWR);
	for (p=7; p<200; p--)
		for (h=0; h<14; h++)
			lcd_dat16(Font_16x16[p][h]);

	PORTB |= _BV(LCD_CS);  // deselect display
	
	//sei();*/
	
	

	
	
	for (p=7; p<200; p--)
	    {
		cli();
		
		PORTB &= ~_BV(LCD_CS);  // select display
		
		lcd_cmd(SD_CSET);
        lcd_dat0(0x08+Start_y);  // start is 8, not 0
        lcd_dat0(0x01);
        lcd_dat0(0x08+Start_y+13);  // end is 00x8B = 0x83+8
        lcd_dat0(0x01);
	
		lcd_cmd(SD_PSET);
        lcd_dat0(175-(Start_x+p));
        lcd_dat0(175-Start_x);
		
		PORTB |= _BV(LCD_CS);  // deselect display
		
		sei();
		cli();
		
		PORTB &= ~_BV(LCD_CS);  // select display
		
		lcd_cmd(RAMWR);
		for (h=0; h<14; h++)
			{
			lcd_dat16(Font_16x16[p][h]);
			//_delay_us(50);
			}
     
	    PORTB |= _BV(LCD_CS);  // deselect display
		
		sei();
		}
	
	
    text++;
	Start_x = Start_x + 8;
	}
}
//-------------------------------------------------------------------------
static void USART0_Transmit(uint8_t data)
{
    while (!(UCSR0A & (1<<UDRE0)));

    UDR0 = data;
}
//-------------------------------------------------------------------------
static void USART1_Transmit(uint8_t data)
{
    while (!(UCSR1A & (1<<UDRE1)));

    UDR1 = data;
}
//-------------------------------------------------------------------------
static uint8_t crc8_calc(uint8_t *data, uint8_t size)
{
/* Calculate CRC-8 value; uses The CCITT-8 polynomial,
expressed as X^8 + X^5 + X^4 + 1 */

    /*uint8_t crc = 0xFF;
    uint8_t index;
    uint8_t b;

    for(index=0; index<size; index++)
      {
      crc ^= data[index];
      for(b=0; b<8; ++b)
        {
        if(crc & 0x80)
          crc = (crc << 1) ^ 0x31;
        else
          crc = (crc << 1);
        }
      }

    return crc;*/
	
	
	uint8_t crc = 0x00;
    uint8_t index;
	
	for(index=0; index<size; index++)
	    crc = crc + data[index];
	
	return crc;
}
//-------------------------------------------------------------------------
static void M25P16_Read(uint8_t *data, uint16_t size, uint32_t addr)
{
  uint16_t i;
  
  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0x03);
  SPI_write_Read((addr >> 16) & 0xFF);
  SPI_write_Read((addr >> 8) & 0xFF);
  SPI_write_Read((addr >> 0) & 0xFF);
  
  for (i=0; i<size; i++)
    {
	Table[i] = SPI_write_Read(0xFF);
	}

  PORTB |= (1 << 4);
  
  sei();
}
//-------------------------------------------------------------------------
static void M25P16_Write(uint8_t *data, uint16_t size, uint32_t addr)
{
  uint16_t i;
  
  cli();
  
  PORTB &= ~(1 << 4);
  
  SPI_write_Read(0x02);
  //SPI_write_Read((addr >> 16) & 0xFF);
  //SPI_write_Read((addr >> 8) & 0xFF);
  //SPI_write_Read((addr >> 0) & 0xFF);
  SPI_write_Read(RX1_Buffer[3]);
  SPI_write_Read(RX1_Buffer[2]);
  SPI_write_Read(RX1_Buffer[1]);
  
  
  for (i=0; i<size; i++)
    {
	SPI_write_Read(Table[i]);
	}

  PORTB |= (1 << 4);
  
  sei();
}
//-------------------------------------------------------------------------
static uint8_t SPI_write_Read(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  return SPDR;
}
//-------------------------------------------------------------------------
ISR(SIG_USART_RECV)
{
    #define Liczba_bajtow_0 22
	
    uint8_t i;
	
	
	RX0_Buffer[RX0_Buf_Pos] = UDR0;
	RX0_Buf_Pos++;
	
	if (RX0_Buf_Pos >= Liczba_bajtow_0)
	    {
		if (RX0_Buffer[0] == 55)
		    {
			if (crc8_calc((uint8_t*)(RX0_Buffer), Liczba_bajtow_0-1) == RX0_Buffer[Liczba_bajtow_0-1])
			    {
				//for(i=0;i<16;i++)
				//    Table[i] = RX1_Buffer[i+1];
				
				
				RX0_Buf_Pos = 0;
				}
			else
			    {
				for(i=0;i<(Liczba_bajtow_0-1);i++)
			        RX0_Buffer[i] = RX0_Buffer[i+1];
				
			    RX0_Buf_Pos = Liczba_bajtow_0-1;
				}
			}
		else
			{
			for(i=0;i<(Liczba_bajtow_0-1);i++)
			    RX0_Buffer[i] = RX0_Buffer[i+1];
				
			RX0_Buf_Pos = Liczba_bajtow_0-1;
			}
		}
}
//-------------------------------------------------------------------------
ISR(SIG_USART1_RECV)
{
    #define Liczba_bajtow_1 134
	
    uint8_t i;
	
	
	RX1_Buffer[RX1_Buf_Pos] = UDR1;
	RX1_Buf_Pos++;
	
	if (RX1_Buf_Pos >= Liczba_bajtow_1)
	    {
		if (RX1_Buffer[0] == 55)
		    {
			if (crc8_calc((uint8_t*)(RX1_Buffer), Liczba_bajtow_1-1) == RX1_Buffer[Liczba_bajtow_1-1])
			    {
				addr = (RX1_Buffer[1]<<0) + (RX1_Buffer[2]<<8) + (RX1_Buffer[3]<<16);
				
				for(i=0;i<128;i++)
				    Table[i] = RX1_Buffer[i+5];
				
				
                PORTB &= ~(1 << 4);
                SPI_write_Read(0x06);
                PORTB |= (1 << 4);
				
                _delay_us(100);
				
                M25P16_Write((uint8_t*)(Table), 128, addr);
                _delay_ms(5);
				
				
				RX1_Buf_Pos = 0;
				}
			else
			    {
				for(i=0;i<(Liczba_bajtow_1-1);i++)
			        RX1_Buffer[i] = RX1_Buffer[i+1];
				
			    RX1_Buf_Pos = Liczba_bajtow_1-1;
				}
			}
		else
			{
			for(i=0;i<(Liczba_bajtow_1-1);i++)
			    RX1_Buffer[i] = RX1_Buffer[i+1];
				
			RX1_Buf_Pos = Liczba_bajtow_1-1;
			}
		}
}
//-------------------------------------------------------------------------
