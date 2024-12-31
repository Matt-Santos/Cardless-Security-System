// ECG Biometric Processor Functions
// Written by Matthew Santos

#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER


//Function Declarations
//-----------------------
void Delay(int msec);                   //Accurate Delay, limits are (1 to 100000)

//-----------------------
void startup();                         //WIP (a lot of setup work still to go)
void CollectData();                     //Starts ADC sampling and delivers chunks to preprocessing
void Preprocessing();                   //WIP
void Sectioning();                      //WIP
void FeatureExtraction();				//WIP
void FeatureNormalization();			//WIP
void TestClassifiers();					//WIP
void authorizeCheck();   				//WIP
void LockReply();                       //Start unlocking sequence if the user is Valid


#endif
