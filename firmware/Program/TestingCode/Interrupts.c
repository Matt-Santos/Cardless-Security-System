// ECG Biometric Processor Interrupt Functions
// Written by Matthew Santos

//Includes
#include "xc.h"             //Varriable Refrences
#include "Varriables.h"     //Program Varriables
#include "dsp.h"            //DSP Library
#include "LCD_Functions.h"  //LCD Function Definitions
#include "diskio.h"         //SD Card I/O

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
//SPI-1 SD-Card Procedure
void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void) {
    IFS0bits.SPI1IF = 0; //Clear the Interrupt Flag
}

//Timer1 Interrupt Procedure
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    disk_timerproc();
    IFS0bits.T1IF = 0; //Clear the Interrupt Flag
}

//Error Traps
//----------------------------------------------------------------
void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AddressError(void)
{
        INTCON1bits.ADDRERR = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _StackError(void)
{
        INTCON1bits.STKERR = 0;         //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _MathError(void)
{
        INTCON1bits.MATHERR = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _DMACError(void)
{
        INTCON1bits.DMACERR = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AltOscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AltAddressError(void)
{
        INTCON1bits.ADDRERR = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AltStackError(void)
{
        INTCON1bits.STKERR = 0;         //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AltMathError(void)
{
        INTCON1bits.MATHERR = 0;        //Clear the trap flag
        Status.Error = 1;
}
void __attribute__((interrupt, no_auto_psv)) _AltDMACError(void)
{
        INTCON1bits.DMACERR = 0;        //Clear the trap flag
        Status.Error = 1;
}