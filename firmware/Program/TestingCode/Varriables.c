// ECG Biometric Variables Initializations
// Written by Matthew Santos

//Includes
#include "Varriables.h" //Program Varriables

//Global Variables
fractional ECGData[ECGLength] __attribute__ ((address(0x800)));
fractional AvgTemp=0;
struct StatusFlags Status ={0,0,0,0,0,0,0,0};
unsigned int ECGBlocksReady=0;

//Buffers
fractional DMA0_BufferA[2][ADC1_BufferSize] __attribute__((space(dma)));
fractional DMA0_BufferB[2][ADC1_BufferSize] __attribute__((space(dma)));
//fractional DMA1_Buffer[SDCard_BufferSize]   __attribute__((space(dma)));
fractional WorkingBuffer[ECGLength]         __attribute__((space(ymemory)));

//Preprocessing Filter
fractional PPdelay[PPnumCoefs]  __attribute__ ((space(ymemory)));
fractional PPcoeffs[PPnumCoefs] __attribute__ ((space(xmemory))) = {
    //a2            a1              b2              b1              b0
    Q15(0.97501),	Q15(-1.43972),	Q15(0.00008),	Q15(0.00016),	Q15(0.00008),
    Q15(0.97501),	Q15(-1.43972),	Q15(1),         Q15(1.9256),	Q15(1),
    Q15(0),         Q15(-0.995),	Q15(1),         Q15(1.83398),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(1.70828),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(1.55105),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(1.41421),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(1.36555),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(1.15578),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0.9266),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0.68413),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0.43693),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0.2006),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0.0175),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(0),         Q15(1),
    Q15(0),         Q15(0),         Q15(0.62134),	Q15(-0.6649),	Q15(1),
    Q15(0),         Q15(0),         Q15(0.57324),	Q15(-0.99662),	Q15(1),
    Q15(0),         Q15(0),         Q15(1.60941),	Q15(-1.0701),	Q15(1),
    Q15(0),         Q15(0),         Q15(0.54996),	Q15(-1.24864),	Q15(1),
    Q15(0),         Q15(0),         Q15(0.53751),	Q15(-1.40683),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(-1.41421),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(-1.45794),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(-1.45794),	Q15(1),
    Q15(0),         Q15(0),         Q15(1.74447),	Q15(-1.73857),	Q15(1),
    Q15(0),         Q15(0),         Q15(1.81832),	Q15(-2.27044),	Q15(1),
    Q15(0),         Q15(0),         Q15(1.86041),	Q15(-2.6173),	Q15(1),
    Q15(0),         Q15(0),         Q15(1),         Q15(2.0),       Q15(1),
    Q15(0),         Q15(0),         Q15(0.17719),	Q15(-0.97298),	Q15(1),
    Q15(0),         Q15(0),         Q15(1.36917),	Q15(-2.36917),	Q15(1),
    Q15(0),         Q15(0),         Q15(0),         Q15(-4.12187),	Q15(1)
};
IIRCanonicStruct PPFilter       __attribute__ ((space(ymemory))) = {
    .numSectionsLess1 = 28,     //Number of Canonic Filters minus one
    .coeffsBase = PPcoeffs,     //Pointer to Filter Coefficients
    .coeffsPage = PPcoeffsPage, //Address Space of the Filter Coefficients
    .delayBase = PPdelay,       //Pointer to the Delay Space
    .initialGain = Q15(1),      //Initial Gain
    .finalShift = 0             //Delay Shift
};

//Derivative Filter
/*
fractional Ddelay[DnumCoefs]    __attribute__ ((space(ymemory)));
fractional Dcoeffs[DnumCoefs]   __attribute__ ((space(xmemory))) = {
    //b3               b2              b1              b0
    Q15(-0.083333),	Q15(0.666667),	Q15(-0.666667),	Q15(0.083333)
};
*/
struct Features F;