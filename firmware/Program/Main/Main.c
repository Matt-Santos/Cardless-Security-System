// ECG Biometric Processor Code
// Written by Matthew Santos

/* Important Notes
  - No Watchdog
  - No Write Protection
  - Verify Overheat Protection (BrownOut thing)
  - Can potentially save pins by using SPI in Master/Slave Mode?
  - De-bounce the Start Button
*/

//Includes
#include "xc.h"         //Varriable Refrences
#include "Config.h"     //Configuration Settings
#include "Varriables.h" //Program Varriables
#include "Interrupts.h" //Interrupt Definitions
#include "Functions.h"  //Function Definitions

//Program Start

int main(void) {
    startup(); //Execute Startup Procedures
    while (1) {
        //Code Testing Area
        //------------------------------------

        
        //------------------------------------
        
        //Remove the Forced Status and Uncomment Sleep
        Status.start = 1;
        //Sleep();                  //Wait For Interrupt
        if(Status.start){           //Check if Start Was Pushed
            CollectData();          //Start Sampling
			Preprocessing();        //Start Preprocessing
            Sectioning();           //Start Sectioning
            FeatureExtraction();    //Start Feature Extraction
            FeatureNormalization(); //Perform Feature Normalization
            TestClassifiers();      //Use Support Vector Machine Sequentially
            authorizeCheck();       //Analyze Results
            LockReply();            //Perform Locking Action Based on Analysis
            Status.start = 0;       //clear start
        }
        else{
            Status.Error = 1;       //Error Since nothing else should wake the CPU
        }
    }
}