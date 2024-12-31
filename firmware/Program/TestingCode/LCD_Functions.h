// ECG Biometric Peripheral Functions
// Written by Matthew Santos

#ifndef LCD_FUNCTIONS_HEADER
#define	LCD_FUNCTIONS_HEADER

//LCD Commands
#define LCD_CMD_CLEAR    0x01  //Long delay ~5ms and also runs HOME
#define LCD_CMD_LINE1    0x02  //Return to 1st Line Home Position
#define LCD_CMD_LINE2    0xc0  //Go to the 2nd Line Home Position
#define LCD_CMD_MODEA    0x06  //Normal Mode (Write from left to right)
#define LCD_CMD_MODEB    0x05  //Scope Writing (Write to Current Pos, shift remainder right)
#define LCD_CMD_OFF      0x08  //Hide the Display
#define LCD_CMD_ON       0x0c  //Show the Display
#define LCD_CMD_PWRON    0x17  //Power On the Display
#define LCD_CMD_PWROFF   0x13  //Power Off the Display
#define LCD_CMD_BLINK    0x0f  //Turn on Cursor Blink
#define LCD_FUNC_SET     0x38  //8 bit interface, 2 lines, 5x10 font

//LCD Pin Shortcuts
#define LCD_DO      LATBbits.LATB14
#define LCD_CLK     LATBbits.LATB15
#define LCD_CS      LATBbits.LATB13

//LCD Functions
void LCD_Initialize();
void LCD_SendString(const char *str);
void LCD_Send(unsigned char data,unsigned char RS);
void LCD_Clear();   //Reset Display
void LCD_L1();      //Goto Line 1
void LCD_L2();      //Goto Line 2
void LCD_Bar(int progress);

#endif