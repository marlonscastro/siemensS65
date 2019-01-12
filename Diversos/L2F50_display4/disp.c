#define DISP_W 132
#define DISP_H 176
#define CHAR_H 14
#define CHAR_W 8
#define TEXT_COL 16
#define TEXT_ROW 12

#define LCD_RS     pin_c0
#define LCD_RESET  pin_c1
#define LCD_CS     pin_a5   // SS SPI
#define LCD_SCK    pin_c3   // CKL SPI
#define LCD_DATA   pin_c5   // SDO SPI
//#define LCD_MISO   pin_b4

#define _DATCTL 0xBC         // Data Control (data handling in RAM)
#define _DISCTL 0xCA         // Display Control
#define _GCP64 0xCB          // pulse set for 64 gray scale
#define _GCP16 0xCC          // pulse set for 16 gray scale
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

int16 textcolor;    // color of text
int16 backcolor;    // color of background

/////////////////////////////// function prototypes ////////////////////////

void port_init(void);

void lcd_write(int dat);
void lcd_cmd(int dat);
void lcd_dat0(int dat);
void lcd_dat16(int16 dat);

void lcd_cspulse(void);

void lcd_init_c(void);
void fill_screen(int16 color);
void put_char(int x, int y, char c, int rot);



void port_init(){
  int tmp;
  
  output_low(LCD_RESET);  
  output_high(LCD_DATA);  
  output_low(LCD_CS);  
  output_low(LCD_SCK);  
  //output_high(LCD_MISO);  
  output_high(LCD_RS);     // não usado no display LPH
  
  // setup SPI Interface
  //SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR0);
  //SPSR = 1;  // double speed bit
  /* Aqui tem que configurar SPI */
  //tmp=SPSR;
}


void lcd_write(int dat){
   // Comunicação SPI sem Biblioteca 
  spi_write(dat);
  /*int i;
  for (i=8;i>0;i--) {
    output_low(LCD_SCK);
    delay_us(5);
    if ((dat&0x80)==0){
      output_low(LCD_DATA);  // Era LCD_DATA
    }
    else {
    output_high(LCD_DATA);
    }
    output_high(LCD_SCK);
    dat=dat<<1;
  }*/
  /*SPDR= dat;
  while (!(SPSR & _BV(SPIF)));*/
}


void lcd_cmd(int dat){
  output_low(LCD_RS);  
  lcd_write(dat);
  lcd_write(0x00);
  output_high(LCD_RS);    
}

void lcd_dat0(int dat){
  lcd_write(dat);
  lcd_write(0x00);  
}

void lcd_dat16(int16 dat){
  lcd_write(dat>>8);
  lcd_write(dat);
}


void lcd_cspulse(void){
  output_high(LCD_CS);    
  //asm volatile("nop");
  delay_cycles(3);
  output_low(LCD_CS);    
}

void lcd_init_c(void)
/*
  Init the Siemens S65 Display with EPSON controller

  (c) Christian Kranz, 10/2005
*/
{

  int i;
  BYTE const disctl[9] = {0x4C, 0x01, 0x53, 0x00, 0x02, 0xB4, 0xB0, 0x02, 0x00};
  BYTE const gcp64_0[29] =
                      {0x11,0x27,0x3C,0x4C,0x5D,0x6C,0x78,0x84,0x90,0x99,0xA2,0xAA,0xB2,0xBA,
                       0xC0,0xC7,0xCC,0xD2,0xD7,0xDC,0xE0,0xE4,0xE8,0xED,0xF0,0xF4,0xF7,0xFB,
                       0xFE};
  BYTE const gcp64_1[34] =
                     {0x01,0x03,0x06,0x09,0x0B,0x0E,0x10,0x13,0x15,0x17,0x19,0x1C,0x1E,0x20,
                      0x22,0x24,0x26,0x28,0x2A,0x2C,0x2D,0x2F,0x31,0x33,0x35,0x37,0x39,0x3B,
                      0x3D,0x3F,0x42,0x44,0x47,0x5E};
  BYTE const gcp16[15] =
                      {0x13,0x23,0x2D,0x33,0x38,0x3C,0x40,0x43,0x46,0x48,0x4A,0x4C,0x4E,0x50,0x64};

  // generate clean display reset
  output_low(LCD_RESET);   // CS is high during reset release 
  output_high(LCD_CS);  
  output_high(LCD_RS);  // RS is set to high


  output_high(LCD_RESET);  // release reset


  output_low(LCD_CS);  // select display
  
  lcd_cmd(_DATCTL);
  lcd_dat0(0x2A);  // 0x2A=565 mode, 0x0A=666mode, 0x3A=444mode

  lcd_cspulse(); // Pulse on CS
  
  lcd_cmd(_DISCTL);
  for (i=0; i<9; i++){
    lcd_dat0(disctl[i]);
  }
  
  lcd_cmd(_GCP64);
  for (i=0; i<29; i++){
    lcd_dat0(gcp64_0[i]);
    lcd_dat0(0x00);
  }
  for (i=0; i<34; i++){
    lcd_dat0(gcp64_1[i]);
    lcd_dat0(0x01);
  }

  lcd_cmd(_GCP16);
  for (i=0; i<15; i++){
    lcd_dat0(gcp16[i]);
  }

  lcd_cmd(GSSET);
  lcd_dat0(0x00);

  lcd_cmd(OSSEL);
  lcd_dat0(0x00);

  lcd_cmd(SLPOUT);
  
  //delay_ms(7);

  lcd_cmd(SD_CSET);
  lcd_dat0(0x08);
  lcd_dat0(0x01);
  lcd_dat0(0x8B);
  lcd_dat0(0x01);

  lcd_cmd(SD_PSET);
  lcd_dat0(0x00);
  lcd_dat0(0x8F);

  lcd_cmd(ASCSET);
  lcd_dat0(0x00);
  lcd_dat0(0xAF);
  lcd_dat0(0xAF);
  lcd_dat0(0x03);
  
  lcd_cmd(SCSTART);
  lcd_dat0(0x00);

  output_low(LCD_RS);    
  lcd_dat0(DISON);
  
  output_high(LCD_CS);    // deselect display
}  




void fill_screen(int16 color){
  int16 i;
  output_low(LCD_CS);    
  lcd_cmd(SD_CSET);
  lcd_dat0(0x08);  // start is 8, not 0
  lcd_dat0(0x01);
  lcd_dat0(0x8B);  // is 00x8B = 0x83+8
  lcd_dat0(0x01);

  lcd_cmd(SD_PSET);
  lcd_dat0(0x00);
  lcd_dat0(0xAF);
  
  lcd_cmd(RAMWR);

  // start data transmission  
  for (i=0; i<DISP_W*DISP_H; i++){
    lcd_dat16(color);
  }
    
  output_high(LCD_CS);    
}

/*
void put_char(int x, int y, char c, int rot)
{
  int h,ch,p,mask,he;

  //PORTB &= ~_BV(LCD_CS);  // select display
  output_low(LCD_CS);  
  
  lcd_cmd(SD_CSET);
  lcd_dat0(0x08+x);  // start is 8, not 0
  lcd_dat0(0x01);
  lcd_dat0(0x08+x+CHAR_W-1);  // end is 00x8B = 0x83+8
  lcd_dat0(0x01);
  
  lcd_cmd(SD_PSET);
  lcd_dat0(y);
  if (y < DISP_H-CHAR_H)
  {
    he=CHAR_H;
    lcd_dat0(y+CHAR_H-1);
  }
  else
  {
    he=DISP_H-y;
    lcd_dat0(DISP_H-1);
  }

  lcd_cmd(RAMWR);
  
  for (h=0; h<he; h++) // every column of the character
  {        
        ch=pgm_read_byte(&ascii_tab[ c-32 ][h]);

        mask=0x80;
        for (p=0; p<CHAR_W; p++)  // write the pixels
        {
          if (ch&mask)
          {
            lcd_dat16(textcolor);                
          }
          else
          {
            lcd_dat16(backcolor);
          }
          mask=mask/2;
        }  // for p
  }
  PORTB |= _BV(LCD_CS);  // deselect display    

}*/



// end of file
