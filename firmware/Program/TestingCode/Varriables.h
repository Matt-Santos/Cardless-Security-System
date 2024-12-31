// ECG Biometric Variables Declarations
// Written by Matthew Santos

#ifndef VARRIABLES_HEADER
#define	VARRIABLES_HEADER

//Includes
#include "dsp.h"            //DSP Library

//Note anything that could be called simultaniously or changed in an interrupt service routine should be volatile

//Global Defines
#define ClockSpeed 40000           //Acurate Clock Speed for Precise Timing (kHz)
#define sampleRate 500             //ADC1 Sample Rate limits are (10Hz to 625kHz) 
#define ECGLength 3056             //Number of ECG Data points (must be multiple of ADC1_BufferSize)
#define ADC1_BufferSize 16         //Size of ADC Data Chunks (128,64,32,16,8,4,2,1)
//#define SDCard_BufferSize 512      //Size of SD-Card Page Chunks

//Global Variables
extern fractional ECGData[ECGLength] __attribute__ ((address(0x800)));   //ECG Data Storage
extern fractional AvgTemp;              //Store the Average Temperature Value
extern unsigned int ECGBlocksReady;     //Stores the number of ECG Blocks Ready for Pre-processing
extern struct StatusFlags {             //The Global Status Register (restructure as needed)
    int start :1;                       //Start Button Flag Indicates if Start Procedure is Active
    int sample :1;
    int preprocess :1;
    int features :1;
    int AUX5 :1;
    int AUX6 :1;
    int Open :1;                        //Indicates if the Conditions to Open the Door Have been Satisfied
    int Error :1;                       //Error Flag Used to Indicate Something Unexpected Happened
} Status;                               //Declare and Externalize the Status Register

//Buffers
extern fractional DMA0_BufferA[2][ADC1_BufferSize] __attribute__((space(dma)));
extern fractional DMA0_BufferB[2][ADC1_BufferSize] __attribute__((space(dma)));
//extern fractional DMA1_Buffer[SDCard_BufferSize]   __attribute__((space(dma)));
extern fractional WorkingBuffer[ECGLength]         __attribute__ ((space(ymemory)));

//Preprocessing Filter
#define PPnumCoefs 145                  //Number of Coeficients in Preprocessing Filter
#define PPcoeffsPage 0xFF00             //Address Space of Preprocessing Filter Coefficients
extern IIRCanonicStruct PPFilter;
extern fractional PPdelay[PPnumCoefs]  __attribute__ ((space(ymemory)));
extern fractional PPcoeffs[PPnumCoefs] __attribute__ ((space(xmemory)));

//Derivative Filter
#define DnumCoefs 4
#define DcoeffsPage 
extern FIRStruct DFilter;
extern fractional Ddelay[DnumCoefs]    __attribute__ ((space(ymemory)));
extern fractional Dcoeffs[DnumCoefs]   __attribute__ ((space(xmemory)));

//Features Array
extern struct Features{
    fractional HR;
    fractional HRV;
    fractional QR_T;
    fractional RS_T;
    fractional QT_T;
    fractional PS_T;
    fractional PQ_T;
    fractional ST_T;
    fractional RT_T;
    fractional PR_T;
    fractional PT_T;
    fractional QR_S;
    fractional RS_S;
    fractional PT_S;
    fractional PR_S;
    fractional RT_S;
    fractional PQ_S;
    fractional ST_S;
    fractional QT_S;
    fractional PS_S;
    fractional RP_A;
    fractional PT_A;
    fractional PQ_A;
    fractional TS_A;
    fractional RQ_A;
    fractional RS_A;
    fractional QS_A;
}F;


#endif

