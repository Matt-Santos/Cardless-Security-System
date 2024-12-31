// ECG Biometric Peripheral Functions
// Written by Matthew Santos


//Includes
#include "xc.h"              //Varriable Refrences
#include "Varriables.h"      //Program Varriables
#include "Functions.h"       //Function Definitions
#include "LCD_Functions.h"   //Function Definitions
#include "libpic30.h"        //Compiler Helper Library
#include "stdlib.h"          //Compiler Utility Functions

void LCD_Initialize(){
    LCD_CS = 1;
    LCD_CLK = 1;
    LCD_Send(LCD_FUNC_SET,0);   //Set the LCD Function
    LCD_Send(LCD_CMD_ON,0);     //Turn Display ON
    LCD_Clear();                //Reset Display
    LCD_L1();                   //Goto Line 1
    LCD_Send(LCD_CMD_PWRON,0);  //Power On the Display
    LCD_Send(LCD_CMD_MODEA,0);  //Set Entry Mode
}
void LCD_SendString(const char *str){
    int i=0;
    while(str[i]){
        LCD_Send(str[i++],1);
    }
}
void LCD_Send(unsigned char data,unsigned char RS){
    unsigned int msg = (unsigned int) data;
    msg ^= RS << 9;
    int i;
    LCD_CS = 0;
    __delay32(ClockSpeed/4000);
    for (i=0;i<10;i++){
        LCD_CLK =0;
        if (msg & 0x0200){   //Send a bit
            LCD_DO = 1;
        }
        else{
            LCD_DO = 0;
        }
        __delay32(ClockSpeed/4000);
        LCD_CLK =1; //Clock Toggle
        msg <<= 1;  //shift the message
        __delay32(ClockSpeed/4000);
    }
    LCD_CS = 1;
    __delay32(ClockSpeed/4000);
}
void LCD_Clear(){
    LCD_Send(LCD_CMD_CLEAR,0);  //Clear Display
    Delay(8);                   //Wait until Ready
}
void LCD_L1(){
    LCD_Send(LCD_CMD_LINE1,0);  //Goto Line 1
}
void LCD_L2(){
    LCD_Send(LCD_CMD_LINE2,0);  //Goto Line 2
}
void LCD_Bar(int progress){
    LCD_L2();
    int i;
    for (i=0;i<16;i++){
        if(i<=progress){
            LCD_Send(0xff,1);
        }
        else{
            LCD_Send(0x20,1);
        }
    }
    char percent[3];
    itoa(percent,(4+6*progress),10);
    LCD_SendString(percent);
    LCD_Send(0x25,1);
}