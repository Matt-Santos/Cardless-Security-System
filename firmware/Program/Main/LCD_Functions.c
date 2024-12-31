// ECG Biometric Peripheral Functions
// Written by Matthew Santos


//Includes
#include "Varriables.h"      //Program Varriables
#include "Functions.h"       //Function Definitions
#include "LCD_Functions.h"   //Function Definitions


void LCD_Initialize(){
    Delay(10);                  //Wait until Ready
    LCD_PutCmd(LCD_FUNC_SET);   //Set the LCD Function
    LCD_PutCmd(LCD_CMD_ON);     //Turn Display ON
    LCD_PutCmd(LCD_CMD_CLEAR);  //Clear Display
    LCD_PutCmd(LCD_CMD_MODEB);  //Set Entry Mode
}
void LCD_PutChar(char ch){
    //Send RS=1
    //Send RW=0
    //Send Data
}
void LCD_PutCmd(char ch){
    //Send RS=0
    //Send RW=0
    //Send CMD
}
void LCD_SendString(const char *str){
    int i=0;
    while(str[i]){
        LCD_PutChar(str[i++]);
    }
}
void LCD_Goto(char pos, char ln){
    
}