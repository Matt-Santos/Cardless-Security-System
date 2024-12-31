// ECG Biometric Processor Functions
// Written by Matthew Santos

#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER


//Function Declarations
//-----------------------
void Delay(int msec);                       //Accurate Delay, limits are (1 to 100000)
void WriteECGDataToSD(const char *filename);//Transfers the Contents of ECGData to SDCard

//-----------------------
void startup();                             //Perform Startup Tasks and Configurations
void CollectData();                         //Starts ADC sampling and delivers chunks to preprocessing
void Preprocessing();                       //Performs the initial filter operations on the incoming data
void Sectioning();                          //WIP
void FeatureExtraction();                   //WIP
void FeatureNormalization();                //WIP
void TestClassifiers();                     //WIP
void authorizeCheck();                      //WIP
void LockReply();                           //Start unlocking sequence if the user is Valid

#endif