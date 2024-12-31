// ECG Biometric Peripheral Functions
// Written by Matthew Santos

#ifndef LCD_FUNCTIONS_HEADER
#define	LCD_FUNCTIONS_HEADER

//LCD Commands
#define LCD_CS_PIN RP14
#define LCD_CMD_CLEAR 0x01  //Long delay ~5ms and also runs HOME
#define LCD_CMD_HOME 0x02
#define LCD_CMD_MODEA 0x06  //Normal Mode (Write from left to right)
#define LCD_CMD_MODEB 0x05  //Scope Writing (Write First Bit, shift remainder right)
#define LCD_CMD_OFF 0x08
#define LCD_CMD_ON 0x
#define LCD_CMD_
#define LCD_CMD_NOBLINK
#define LCD_CMD_STATUS

//LCD Functions (these write CMDs to the buffer which does its thing using DMA and automatically checks for busy ect..)
void LCD_INIT();
void LCD_CMD(char data);
void LCD_DATA(char data);

void LCD_SendString(char row,unsigned char *string);


#endif

