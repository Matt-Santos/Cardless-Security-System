// ECG Biometric Processor Interrupt Functions
// Written by Matthew Santos

//Includes
#include "xc.h"         //Varriable Refrences
#include "Varriables.h" //Program Varriables
#include "dsp.h"        //DSP Library


//Start Button Procedure
void __attribute__((__interrupt__, auto_psv)) _INT1Interrupt(void) {
    Status.start = 1; //Set the Start Flag
    IFS1bits.INT1IF = 0; //Clear the Interrupt Flag
}

//DMA0 Interrupt (ADC-1)
void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void) {
    static unsigned char DMA0ActiveBuffer = 0; //Active Buffer
    register int DSP_ACCA asm("A"); //Define DSP accumulator A
    if (DMA0ActiveBuffer == 0) { //For Active Buffer
        //Append Active Buffer to ECGData
        VectorCopy(ADC1_BufferSize, &ECGData[ADC1_BufferSize * ECGBlocksReady], &DMA0_BufferA[0][0]);
        //Take one Point of Temperature Data and Add it to Rolling Average
        DSP_ACCA = __builtin_mpy(DMA0_BufferA[1][0], Q15((float) ADC1_BufferSize / (float) ECGLength), NULL, NULL, 0, NULL, NULL, 0);
        AvgTemp = AvgTemp + __builtin_sac(DSP_ACCA, 0);
    } else {
        //Append Active Buffer to ECGData
        VectorCopy(ADC1_BufferSize, &ECGData[ADC1_BufferSize * ECGBlocksReady], &DMA0_BufferB[0][0]);
        //Take one Point of Temperature Data and Add it to Rolling Average
        DSP_ACCA = __builtin_mpy(DMA0_BufferB[1][0], Q15((float) ADC1_BufferSize / (float) ECGLength), NULL, NULL, 0, NULL, NULL, 0);
        AvgTemp = AvgTemp + __builtin_sac(DSP_ACCA, 0);
    }
    ECGBlocksReady++; //Update Number of ECG Blocks Ready
    if (ECGBlocksReady == ECGLength / ADC1_BufferSize) { //When Enough Blocks
        AD1CON1bits.ADON = 0; //Turn OFF ADC1
    }
    DMA0ActiveBuffer ^= 1; //Toggle Active Buffer
    IFS0bits.DMA0IF = 0; //Clear the DMA0 Interrupt Flag
}
//DMA1 Interrupt (SD-Card Input)
void __attribute__((__interrupt__, no_auto_psv)) _DMA1Interrupt(void) {

    IFS0bits.DMA1IF = 0; //Clear the DMA1 Interrupt Flag
}
//DMA2 Interrupt (SD-Card Output)
void __attribute__((__interrupt__, no_auto_psv)) _DMA2Interrupt(void) {

    IFS1bits.DMA2IF = 0; //Clear the DMA2 Interrupt Flag
}
//DMA3 Interrupt (LCD Input)
void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void) {
    
    //Transmission Complete Set CS High
    
    
    IFS2bits.DMA3IF = 0; //Clear the DMA3 Interrupt Flag
}
//DMA4 Interrupt (LCD Output)
void __attribute__((__interrupt__, no_auto_psv)) _DMA4Interrupt(void) {
    //Shouldn't need to read the LCD Output
    IFS2bits.DMA4IF = 0; //Clear the DMA4 Interrupt Flag
}
//SPI-1 SD-Card Procedure (Do I need this?)
void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void) {
    IFS0bits.SPI1IF = 0; //Clear the Interrupt Flag
}
//SPI-2 LCD Procedure (Do I need this?)
void __attribute__((__interrupt__, auto_psv)) _SPI2Interrupt(void) {
    IFS2bits.SPI2IF = 0; //Clear the Interrupt Flag
}