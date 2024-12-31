// Interrupt Subroutines
// Written by Matthew Santos

#ifndef INTERRUPTS_HEADER
#define	INTERRUPTS_HEADER

//Start Button Procedure
void __attribute__((__interrupt__)) _INT1Interrupt(void);

//SD Card Procedure
void __attribute__((__interrupt__)) _SPI1Interrupt(void);

//LCD Procedure
void __attribute__((__interrupt__, auto_psv)) _SPI2Interrupt(void);

//DMA0 Interrupt (ADC-1)
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);

#endif