// ECG Biometric Variables Initializations
// Written by Matthew Santos

//Includes
#include "Varriables.h" //Program Varriables

//Global Variables
fractional ECGData[ECGLength] __attribute__ ((address(0x800)));
fractional AvgTemp=0;
volatile DSTATUS SD_Status = STA_NOINIT;
unsigned int SD_CardType = 0;
struct StatusFlags Status ={0,0,0,0,0,0,0,0};
unsigned int ECGBlocksReady=0;

//Buffers
fractional DMA0_BufferA[2][ADC1_BufferSize] __attribute__((space(dma)));
fractional DMA0_BufferB[2][ADC1_BufferSize] __attribute__((space(dma)));
fractional DMA1_Buffer[SDCard_BufferSize]   __attribute__((space(dma)));
fractional DMA3_Buffer[LCD_BufferSize]      __attribute__((space(dma)));
fractional DMA4_Buffer[LCD_BufferSize]      __attribute__((space(dma)));
fractional WorkingBuffer[ADC1_BufferSize]   __attribute__((space(ymemory)));

//Preprocessing Filter
FIRStruct PPFilter __attribute__ ((space(ymemory)));
fractional PPdelay[PPnumCoefs]  __attribute__ ((space(ymemory)));
fractional PPcoeffs[PPnumCoefs] __attribute__ ((space(xmemory))) = {
    Q15(0.1),  //b0
    Q15(0.2),  //b1
    Q15(0.3),  //b2
    Q15(0.4),  //b3
    Q15(0.5)   //b4
};
