// ECG Biometric Processor Code
// Written by Matthew Santos

//Includes
#include "xc.h"             //Varriable Refrences
#include "Config.h"         //Configuration Settings
#include "Varriables.h"     //Program Varriables
#include "Interrupts.h"     //Interrupt Definitions
#include "Functions.h"      //Function Definitions
#include "LCD_Functions.h"  //LCD Function Definitions

//Program Start
int main(void) {
    startup();                              //Execute Startup Procedures
    LCD_Initialize();                       //Initialize the LCD
    LCD_SendString("  Cardless Security");  //Reset Message Line 1
    LCD_L2();                               //Move LCD Cursor to Line 2
    LCD_SendString("       System");        //Reset Message Line 2
    Delay(5000);                            //Message Display Delay
    LCD_Clear();                            //Clear the Display
    LCD_L1();                               //Set LCD Cursor to Line 1
    while (1) {
        //Code Testing Area (Diagnostics go in here)
        //------------------------------------

        //------------------------------------
        LCD_Send(LCD_CMD_OFF,0);        //Hide the Display
        LCD_Send(LCD_CMD_PWROFF,0);     //Power Off the Display
        Sleep();                        //Wait For Interrupt
        LCD_Send(LCD_CMD_ON,0);         //Show the Display
        LCD_Send(LCD_CMD_PWRON,0);      //Power ON the Display
        if(Status.start){               //Check if Start Was Pushed
            CollectData();              //Start Sampling
            WriteECGDataToSD("Raw.txt");//Transfers ECGData to SDCard
            Preprocessing();            //Start Preprocessing
            WriteECGDataToSD("ECG.txt");//Transfers ECGData to SDCard
            Sectioning();               //Start Sectioning
            //WriteECGDataToSD("Sec.txt");//Transfers ECGData to SDCard
            FeatureExtraction();        //Start Feature Extraction
            FeatureNormalization();     //Perform Feature Normalization
            TestClassifiers();          //Use Support Vector Machine Sequentially
            authorizeCheck();           //Analyze Results
            LockReply();                //Perform Locking Action Based on Analysis
            Status.start = 0;           //Clear the start flag
        }
        else{
            Status.Error = 1;  //Error Since nothing else should wake the CPU
        }
        if (Status.Error){                          //If Error Occurred
            LCD_Clear();LCD_L1();                   //Clear LCD
            LCD_SendString("An Error Occurred");    //Notify User
            Delay(1000);                            //Message Delay
        }
    }
}