'=================================================================================================
'=                 Siemens S65 display routines for LS020 controller                             =
'= Special thanks to Dirk Milewski www.comwebnet.de who's routines i have used to get me going.  =
'=                Copyright, not for private use. by Evert Dekker 2009.                          =
'=                   Written and tested with Bascom version 1.11.9.3                             =
'=================================================================================================


$hwstack = 100
$swstack = 100
$framesize = 100

$regfile = "m128def.Dat"
$crystal = 16000000
$baud = 19200






'=== Config the hardware ===
Rs Alias Porta.1                                            'S65 Data/command pin
Res Alias Porta.5                                           'S65 Reset pin
Cs Alias Porta.3                                            'S65 Chip Select pin
Clk Alias Portb.1                                           'S65 Clock pin
Dat Alias Portb.2                                           'S65 Data pin

Config Rs = Output
Config Dat = Output
Config Cs = Output
Config Clk = Output
Config Res = Output

Config Spi = Hard , Interrupt = Off , Data Order = Msb , Master = Yes , Polarity = Low , Phase = 0 , Clockrate = 128

'=== Define the constante ===
Const Blue = &B00000011                                     '8bit color  RRRGGGBB
Const Yellow = &B11111100
Const Red = &B11100000
Const Green = &B00011100
Const Black = &B00000000
Const White = &B11111111
Const Brightgreen = &B00111110
Const Darkgreen = &B00010100
Const Darkred = &B10100000
Const Darkblue = &B00000010
Const Brightblue = &B00011111
Const Orange = &B11111000

Const Displaysize_x = 176
Const Displaysize_y = 132
Const Total_pixels = Displaysize_x * Displaysize_y
Const Landscape = 0                                         'Change to 0 if you want to use the display in portrait


Declare Sub S65_init()
Declare Sub S65_pset(byval Lx As Byte , Byval Ly As Byte , Byval Color As Byte)
Declare Sub S65_line(byval Lx1 As Byte , Byval Ly1 As Byte , Byval Lx2 As Byte , Byval Ly2 As Byte , Byval Color As Byte)
Declare Sub S65_cls(byval Backcolor As Byte)
Declare Sub Lcdtext(byval S As String , Byval Xoffset As Byte , Byval Yoffset As Byte , Byval Fontset As Byte , Byval Forecolor As Byte , Byval Backcolor As Byte )
Declare Sub S65_cmdw(byval Cmd As Word)
Declare Sub S65_showpicture(byval Xoffset As Byte , Byval Yoffset As Byte)


'=== Main prog ===
S65_init                                                    'First thing to do, init the display
S65_cls White                                               'Clear the display with white background

Do
Lcdtext "Hello world" , 10 , 1 , 1 , Red , White
Lcdtext "SIEMENS" , 10 , 12 , 1 , White , Green
Lcdtext "S-65" , 10 , 23 , 2 , Blue , White 
Lcdtext "By Evert Dekker 2009" , 20 , 50 , 4 , Black , White
Lcdtext "Landscape & portrait mode" , 5 , 60 , 4 , Black , White

'Wait 4
Restore Plaatje                                             'FIRST restore you image before you can show it
S65_showpicture 1 , 70                                      'Show your image
Wait 2
S65_cls White
Wait 2
Loop
End


'=== Subroutines ===
Sub S65_init()                                              'Init the Siemens S65 display
Local Tempw As Word , Tempb As Byte
Set Res
Waitms 10
Reset Res                                                   'HW Reset
Set Cs
Set Clk
Set Dat
Set Rs
Waitms 1
Set Res                                                     'Reset End
Waitms 10
Set Cs
Waitms 1
Set Rs
Restore Init1
For Tempb = 1 To 12
Read Tempw
S65_cmdw Tempw
Next
Waitms 7
Restore Init2
For Tempb = 1 To 25
Read Tempw
S65_cmdw Tempw
Next
End Sub

Sub S65_cmdw(cmd As Word)                                   'Send 16bits to the display
Reset Cs
Shiftout Dat , Clk , Cmd
Set Cs
End Sub


Sub S65_cls(byval Backcolor As Byte)                        'Clear the lcd
Local Pixel As Word
Set Rs
S65_cmdw &HEF90
S65_cmdw &H0500
S65_cmdw &H0600
S65_cmdw &H0700
Reset Rs
Reset Cs
For Pixel = 0 To Total_pixels
 Shiftout Dat , Clk , Backcolor
Next Pixel
Set Cs
End Sub



Sub S65_pset(lx As Byte , Ly As Byte , Color As Byte)       'Set a pixel
Set Rs
#if Landscape = 1
   Ly = Displaysize_y - Ly
   S65_cmdw &H0600 + Ly
   S65_cmdw &H0700 + Lx
#else
   S65_cmdw &H0600 + Lx
   S65_cmdw &H0700 + Ly
#endif
Reset Rs
Reset Cs
Shiftout Dat , Clk , Color
Set Cs
End Sub



Sub S65_line(byval Lx1 As Byte , Byval Ly1 As Byte , Byval Lx2 As Byte , Byval Ly2 As Byte , Byval Color As Byte)       'Draw a line on the lcd
Local M As Byte , M1 As Single , M2 As Single
Local Yg As Byte , Cv As Single , Bn As Integer
Local R As Byte , B As Byte
M1 = Ly2 - Ly1
M2 = Lx2 - Lx1
Cv = M1 / M2
Bn = Int(cv)
M = Low(bn)
B = M * Lx1
B = -1 * B
B = B + Ly1
For R = Lx1 To Lx2
Yg = M * R
Yg = Yg + B
S65_pset R , Yg , Color
Next R
End Sub



Sub S65_showpicture(xoffset As Byte , Yoffset As Byte)      'Draw an Bascom BGF image on the lcd
' Important that you first RESTORE the image before calling this sub. Examp:  [ Restore Plaatje ]
Local Ysize As Byte , Xsize As Byte , Xpos As Byte , Ypos As Byte       'Dim some local variable
Local Tempx As Byte , Tempy As Byte
Local Imagesize As Word , Imagesize_l As Byte , Imagesize_h As Byte
Local L As Word , Pixel As Byte , Prevpixel As Byte , Repeat As Byte
Read Ysize                                                  'Read the first 4 bytes from the BGF, this contains info about the image
Read Xsize
Read Imagesize_l
Read Imagesize_h
Imagesize = Makeint(imagesize_l , Imagesize_h)              'Byte 3&4 contains the image size, add them together
Xpos = 1 : Ypos = 1                                         'Set the pointers to 1
Do
Read Pixel                                                  'Read the value for the pixel
   If Pixel = &HAA Then                                     'BGF file is lre coded &HAA represents this, the byte after &HAA is the number of times that the byte before &HAA wil be repeated
     Read Repeat                                            'Byte after &HAA contains the number of times to repeat
     Repeat = Repeat - 1
      For L = 1 To Repeat                                   'Loop the repeat
          Tempx = Xpos + Xoffset                            'Pixel position is the image postion + the offset
          Tempy = Ypos + Yoffset
          S65_pset Tempx , Tempy , Prevpixel                'Set the pixel, for repeating it's the byte before &HAA (prevpixel)
          Decr Imagesize                                    'Decrease the bytes still to be processed
          Incr Xpos                                         'Incr the xpos
            If Xpos > Xsize Then                            'If the xpos is larger then xsize of the image start at a new y line
               Xpos = 1                                     'and set the xpos pos back to the first position on the new line
               Incr Ypos                                    'Start a new y line
            End If
      Next L                                                'Next repeat loop
   Else                                                     'It's a regular byte, not LRE encoded
      Tempx = Xpos + Xoffset                                'Pixel position is the image postion + the offset
      Tempy = Ypos + Yoffset
      S65_pset Tempx , Tempy , Pixel                        'Set the pixel
      Prevpixel = Pixel                                     'Store the pixel in a temp byte, maybe we need it if the next byte is &HAA
      Decr Imagesize                                        'Decrease the bytes still to be processed
      Incr Xpos
         If Xpos > Xsize Then                               'If the xpos is larger then xsize of the image start at a new y line
          Xpos = 1                                          'and set the xpos pos back to the first position on the new line
          Incr Ypos                                         'Start a new y line
         End If
End If
Loop Until Imagesize = 0                                    'Do until all bytes are processed
End Sub


Sub Lcdtext(byval S As String , Xoffset As Byte , Yoffset As Byte , Fontset As Byte , Forecolor As Byte , Backcolor As Byte )       'Print text on the display
Local Tempstring As String * 1 , Temp As Byte               'Dim local the variables
Local A As Byte , Pixels As Byte , Count As Byte , Carcount As Byte , Lus As Byte
Local Row As Byte , Byteseach As Byte , Blocksize As Byte , Dummy As Byte
Local Colums As Byte , Columcount As Byte , Rowcount As Byte , Stringsize As Byte
Local Xpos As Byte , Ypos As Byte , Pixel As Byte , Pixelcount As Byte
Stringsize = Len(s) - 1                                     'Size of the text string -1 because we must start with 0
For Carcount = 0 To Stringsize                              'Loop for the numbers of caracters that must be displayed

 If Fontset = 1 Then Restore Font8x8                        'Add or remove here fontset's that you need or not,
 If Fontset = 2 Then Restore Font16x16                      'this is the name that you gave to the font, NOT the filename
 If Fontset = 3 Then Restore Font6x8                        'If you dont know the name, open the font file in wordpad, and there it is,
 If Fontset = 4 Then Restore Font5x5                        'right on top.

 Temp = Carcount + 1                                        'Cut the text string in seperate caracters
Tempstring = Mid(s , Temp , 1)
Read Row : Read Byteseach : Read Blocksize : Read Dummy     'Read the first 4 bytes from the font file
Temp = Asc(tempstring) - 32                                 'Font files start with caracter 32
For Lus = 1 To Temp                                         'Do dummie read to point to the correct line in the fontfile
   For Count = 1 To Blocksize
    Read Pixels
   Next Count
Next Lus
Colums = Blocksize / Row                                    'Calculate the numbers of colums
Row = Row * 8                                               'Row is always 8 pixels high = 1 byte, so working with row in steps of 8.
Row = Row - 1                                               'Want to start with row=0 instead of 1
Colums = Colums - 1                                         'Same for the colums
   For Rowcount = 0 To Row Step 8                           'Loop for numbers of rows
     A = Rowcount + Yoffset
       For Columcount = 0 To Colums                         'Loop for numbers of Colums
         Read Pixels
         Xpos = Columcount                                  'Do some calculation to get the caracter on the correct Xposition
         Temp = Carcount * Byteseach
         Xpos = Xpos + Temp
         Xpos = Xpos + Xoffset
            For Pixelcount = 0 To 7                         'Loop for 8 pixels to be set or not
               Ypos = A + Pixelcount                        'Each pixel on his own spot
               Pixel = Pixels.0                             'Set the pixel (or not)
                 If Pixel = 1 Then
                   Pixel = Forecolor
                 Else
                   Pixel = Backcolor
                 End If
                     S65_pset Xpos , Ypos , Pixel           'Finaly we can set the pixel
                     Shift Pixels , Right                   'Shift the byte 1 bit to the right so the next pixel comes availible
            Next Pixelcount
       Next Columcount
   Next Rowcount
Next Carcount
End Sub                                                     'End of this amazing subroutine


'=== Includes ===
$include "Font8x8.font"                                     'Includes here your font files
$include "Font16x16.font"                                   'If you don't need the files in your program, don't include them,
$include "Font6x8.font"                                     'these are flash memory eaters.
$include "Font5x5.font"


Plaatje:
$bgf "mcse.bgc"


Init1:
Data &HFDFD% , &HFDFD%,
Data &HEF00%,
Data &HEE04% , &H1B04%,
Data &HFEFE% , &HFEFE%,
Data &HEF90% , &H4A04% , &H7F3F% , &HEE04% , &H4306%

Init2:
Data &HEF90% , &H0983% , &H0800% , &H0BAF% , &H0A00% , &H0500% , &H0600% , &H0700%,
Data &HEF00%,
Data &HEE0C%,
Data &HEF90% , &H0080%,
Data &HEFB0% , &H4902%,
Data &HEF00%,
Data &H7F01% , &HE181%,
Data &HE202%,
Data &HE276%,
Data &HE183%,
Data &H8001%,
Data &HEF90%,
Data &H0000%
Data &HEF90% , &HE801%                                      'Switch to 8-bit mode