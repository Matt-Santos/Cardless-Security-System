// ECG Biometric Variables Declarations
// Written by Matthew Santos

#ifndef VARRIABLES_HEADER
#define	VARRIABLES_HEADER

//Includes
#include "dsp.h"            //DSP Library
#include "fatfs/diskio.h"   //FatFs lower layer API

//Note anything that could be called simultaniously or changed in an interupt service routine should be volitile

//Global Defines
#define sampleRate 500             //ADC1 Sample Rate limits are (10Hz to 625kHz) 
#define ECGLength 3056             //Number of ECG Data points (must be multiple of DMA0_BufferSize)
#define ADC1_BufferSize 16         //Size of ADC Data Chunks (128,64,32,16,8,4,2,1)
#define SDCard_BufferSize 512      //Size of SD-Card Page Chunks
#define LCD_BufferSize 10          //Size of LCD Buffer Chunk

//Global Variables
extern fractional ECGData[ECGLength] __attribute__ ((address(0x800)));   //ECG Data Storage
extern fractional AvgTemp;              //Store the Average Temperature Value
extern unsigned int ECGBlocksReady;     //Stores the number of ECG Blocks Ready for Pre-processing
extern volatile DSTATUS SD_Status;      //Stores the current Status of the SD-Card
extern unsigned int SD_CardType;        //Stores a numeric representation of SD-Card type
extern struct StatusFlags {             //The Global Status Register (restructure as needed)
    int start :1;                       //Start Button Flag Indicates if Start Procedure is Active
    int preprocess :1;
    int features :1;
    int AUX1 :1;
    int AUX5 :1;
    int AUX6 :1;
    int Open :1;                        //Indicates if the Conditions to Open the Door Have been Satisfied
    int Error :1;                       //Error Flag Used to Indicate Something Unexpected Happened
} Status;                               //Declare and Externalize the Status Register

//Buffers
extern fractional DMA0_BufferA[2][ADC1_BufferSize] __attribute__((space(dma)));
extern fractional DMA0_BufferB[2][ADC1_BufferSize] __attribute__((space(dma)));
extern fractional DMA1_Buffer[SDCard_BufferSize]   __attribute__((space(dma)));
extern fractional DMA3_Buffer[LCD_BufferSize]      __attribute__((space(dma)));
extern fractional DMA4_Buffer[LCD_BufferSize]      __attribute__((space(dma)));
extern fractional WorkingBuffer[ADC1_BufferSize]   __attribute__ ((space(ymemory)));

//Preprocessing Filter
#define PPnumCoefs 100                  //Number of Coeficients in Preprocessing Filter
#define PPcoeffsPage 0xFF00             //Address Space of Preprocessing Filter Coefficients
extern FIRStruct PPFilter __attribute__ ((space(ymemory)));
extern fractional PPdelay[PPnumCoefs]  __attribute__ ((space(ymemory)));
extern fractional PPcoeffs[PPnumCoefs] __attribute__ ((space(xmemory)));



#endif

