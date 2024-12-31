// Interrupt Subroutines
// Written by Matthew Santos

#ifndef INTERRUPTS_HEADER
#define	INTERRUPTS_HEADER

//Start Button Procedure
void __attribute__((__interrupt__)) _INT1Interrupt(void);

//SD Card Procedure
void __attribute__((__interrupt__)) _SPI1Interrupt(void);

//DMA0 Interrupt (ADC-1)
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);

//DMA1 Interrupt (SD-Card Input)
void __attribute__((__interrupt__, no_auto_psv)) _DMA1Interrupt(void);

//DMA2 Interrupt (SD-Card Output)
void __attribute__((__interrupt__, no_auto_psv)) _DMA2Interrupt(void);

//Timer1 Interrupt Procedure
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void);

//Error Traps
void __attribute__((__interrupt__)) _OscillatorFail(void);
void __attribute__((__interrupt__)) _AddressError(void);
void __attribute__((__interrupt__)) _StackError(void);
void __attribute__((__interrupt__)) _MathError(void);
void __attribute__((__interrupt__)) _DMACError(void);
void __attribute__((__interrupt__)) _AltOscillatorFail(void);
void __attribute__((__interrupt__)) _AltAddressError(void);
void __attribute__((__interrupt__)) _AltStackError(void);
void __attribute__((__interrupt__)) _AltMathError(void);
void __attribute__((__interrupt__)) _AltDMACError(void);

#endif