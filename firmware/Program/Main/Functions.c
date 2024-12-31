// ECG Biometric Processor Functions
// Written by Matthew Santos

//Includes
#include "xc.h"         //Varriable Refrences
#include "dsp.h"        //DSP Library
#include "Varriables.h" //Program Varriables
#include "math.h"       //Math Functions
#include <libpic30.h>   //Compiler Helper Library
#include <string.h>     //String Functions
#include "fatfs/ff.h"   //SD-Card Filesystem Functions

//Function Definitions
//-----------------------

//Delay, causes CPU to pause for specified milliseconds [1,100000] limits
void Delay(int msec){
    __delay32(msec*40000);
}
int SQRT(int value){
    //w0 to w5
    //w4 = 0x4000
    //
    
    
    return value;
}
//startup, perform startup configuration on EVERYTHING!
void startup(){
  //Establish PLL Settings
  PLLFBD = 0x1E;            //M=32
  CLKDIVbits.PLLPRE = 0;    //N1=2
  CLKDIVbits.PLLPOST = 0;   //N2=2
  //Switch to HS External Clock Source
  __builtin_write_OSCCONH(0x3);    //Assign HS with PLL
  __builtin_write_OSCCONL(0x3);    //Maintain SOSC and Switch
  while(OSCCONbits.COSC != 3);     //Wait for HS Mode
  while(OSCCONbits.LOCK != 1);     //Wait for PLL Lock
  //Perform Global Management Tasks
  RCONbits.SWDTEN = 0;      //Disable Watch Dog Timer
  CORCONbits.US = 0;        //DSP Multiplies are Signed
  CORCONbits.IF = 0;        //Fractional Mode for DSP Engine
  CORCONbits.RND = 0;       //Unbiased Rounding Mode
  //Analog Pin Operating Mode (Not Listed => Digital)
  AD1PCFGLbits.PCFG0  = 0;  //Pin 2  => (Analog)
  AD1PCFGLbits.PCFG1  = 0;  //Pin 3  => (Analog)
  AD1PCFGLbits.PCFG2  = 1;  //Pin 4  => (Digital)
  AD1PCFGLbits.PCFG3  = 1;  //Pin 5  => (Digital)
  AD1PCFGLbits.PCFG4  = 1;  //Pin 6  => (Digital)
  AD1PCFGLbits.PCFG5  = 1;  //Pin 7  => (Digital)
  AD1PCFGLbits.PCFG12 = 1;  //Pin 23 => (Digital)
  AD1PCFGLbits.PCFG11 = 1;  //Pin 24 => (Digital)
  AD1PCFGLbits.PCFG10 = 1;  //Pin 25 => (Digital)
  AD1PCFGLbits.PCFG9  = 1;  //Pin 26 => (Digital)
  //Pin Directions (Not Used => Input)
  TRISAbits.TRISA0 = 1;     //Pin 2  => (Input)
  TRISAbits.TRISA1 = 1;     //Pin 3  => (Input)
  TRISBbits.TRISB0 = 1;     //Pin 4  => (Input)
  TRISBbits.TRISB1 = 0;     //Pin 5  => (Output)
  TRISBbits.TRISB2 = 0;     //Pin 6  => (Output)
  TRISBbits.TRISB3 = 1;     //Pin 7  => (Input)
  TRISAbits.TRISA2 = 1;     //Pin 9  => (Input)
  TRISAbits.TRISA3 = 1;     //Pin 10 => (Input)
  TRISBbits.TRISB4 = 1;     //Pin 11 => (Input)
  TRISAbits.TRISA4 = 1;     //Pin 12 => (Input)
  TRISBbits.TRISB5 = 1;     //Pin 14 => (Input)
  TRISBbits.TRISB6 = 1;     //Pin 15 => (Input)
  TRISBbits.TRISB7 = 1;     //Pin 16 => (Input)
  TRISBbits.TRISB8 = 0;     //Pin 17 => (Output)
  TRISBbits.TRISB9 = 0;     //Pin 18 => (Output)
  TRISBbits.TRISB10 = 0;    //Pin 21 => (Output)
  TRISBbits.TRISB11 = 0;    //Pin 22 => (Output)
  TRISBbits.TRISB12 = 0;    //Pin 23 => (Output)
  TRISBbits.TRISB13 = 0;    //Pin 24 => (Output)
  TRISBbits.TRISB14 = 1;    //Pin 25 => (Input)
  TRISBbits.TRISB15 = 0;    //Pin 26 => (Output)  
  //SD-Card Pin-outs (SPI-1)
  RPINR20bits.SDI1R = 7;    //RP7    => SD SPI-1 Data In
  RPOR4bits.RP8R = 8;       //RP8    => SD SPI-1 Clock
  RPOR4bits.RP9R = 7;       //RP9    => SD SPI-1 Data Out
  RPOR5bits.RP10R = 9;      //RP10   => SD SPI-1 Enable
  //LCD Pin-outs (SPI-2)
  RPOR6bits.RP12R = 11;     //RP12   => LCD SPI-2 Clock
  RPOR6bits.RP13R = 10;     //RP13   => LCD SPI-2 Data Out
  RPINR22bits.SDI2R = 14;   //RP14   => LCD SPI-2 Data In
  RPOR7bits.RP15R = 12;     //RP15   => LCD SPI-2 Enable
  //External Interrupts (Inputs)
  RPINR0bits.INT1R = 3;     //RP3    => INT-1 (START)
  //Digital Outputs (Outputs)
  RPOR0bits.RP0R = 0;       //RP0    => RP0   (AUX2)
  RPOR0bits.RP1R = 0;       //RP1    => RP1   (AUX3)
  RPOR1bits.RP2R = 0;       //RP2    => RP2   (AUX4)
  RPOR5bits.RP11R = 0;      //RP11   => RP11  (AUX5)
  //ADC-1 Configuration with DMA0 (CH0=AN1,CH1=AN0)
  AD1CON1bits.ADON = 0;     //Turn OFF ADC-1
  AD1CON1bits.ADSIDL = 1;   //Disable in Idle Mode
  AD1CON1bits.ADDMABM = 0;  //DMA Buffer in Gather Mode
  AD1CON1bits.AD12B = 0;    //10bit Operation
  AD1CON1bits.FORM = 3;     //Signed Fractional Operation
  AD1CON1bits.SSRC = 2;     //Conversion Rate Tied to Timer3
  AD1CON1bits.SIMSAM = 1;   //Simultaneous Channel Sampling
  AD1CON1bits.ASAM = 1;     //ADC Auto Sampling
  AD1CON1bits.SAMP = 1;     //Set ADC Pins to Sampling
  AD1CON2bits.VCFG = 0;     //Set Analog References to AVdd and AVss
  AD1CON2bits.CSCNA = 0;    //Disable Channel Scanning
  AD1CON2bits.CHPS = 1;     //Convert CH0 and CH1
  AD1CON2bits.SMPI = 1;     //Increment DMA Address after every 2nd sample
  AD1CON2bits.BUFM = 0;     //16-word Buffer Conversions
  AD1CON2bits.ALTS = 0;     //MUXA Input Selection
  AD1CON3bits.ADRC = 0;     //ADC Conversion Clock set to Program Clock
  AD1CON3bits.SAMC = 31;    //Sampling Time is ADC Program Clock x31
  AD1CON3bits.ADCS = 30;    //Conversion Time is Program Clock x31
  AD1CON4bits.DMABL = log(ADC1_BufferSize)/log(2); //Words of Buffer Per Input Per DMA Request
  AD1CHS123bits.CH123SA = 0;//CH1 = AN0
  AD1CHS123bits.CH123NA = 0;//Use AVss as Negative Reference
  AD1CHS0bits.CH0SA = 1;    //AN1 is CH0 Positive Input
  AD1CHS0bits.CH0NA = 0;    //Use AVss as Negative Reference
  IFS0bits.AD1IF = 0;		//Clear the ADC1 interrupt flag bit
  IEC0bits.AD1IE = 0;		//Do Not Enable ADC1 interrupt 
  DMA0CONbits.CHEN = 0;     //DMA CH0 Disabled
  DMA0CONbits.SIZE = 0;     //Set to Word Transfer
  DMA0CONbits.DIR = 0;      //Read from Peripheral to SRAM
  DMA0CONbits.HALF = 0;     //Initiate DMA Interrupt when all data written
  DMA0CONbits.NULLW = 0;    //Normal Operation, no write back
  DMA0CONbits.AMODE = 2;    //Use Peripheral Indirect Addressing
  DMA0CONbits.MODE = 2;     //Continuous Ping-Pong Mode
  DMA0REQbits.IRQSEL = 13;  //Use ADC-1 Peripheral
  DMA0CNT = 2*ADC1_BufferSize;  //Number of samples in DMA request
  DMA0STA = __builtin_dmaoffset(DMA0_BufferA); //BufferA Address
  DMA0STB = __builtin_dmaoffset(DMA0_BufferB); //BufferB Address
  DMA0PAD = (volatile unsigned int)&ADC1BUF0;  //Grab Data from ADC1BUF0
  IFS0bits.DMA0IF = 0;      //Clear DMA Interrupt Flag bit
  IEC0bits.DMA0IE = 1;      //Enable DMA CH0 Interrupts
  DMA0CONbits.CHEN = 1;     //Enable DMA CH0
  T3CONbits.TON = 0;        //Stop Timer 3 operation
  T3CONbits.TGATE = 0;      //Disable Gated Mode
  T3CONbits.TCKPS = 2;      //Pre-scaler 1/64
  TMR3 = 0x0000;            //Reset Timer 3
  PR3 = 40E6/64/sampleRate; //Define ADC Trigger Frequency
  T3CONbits.TSIDL = 1;      //Disable Timer 3 in idle mode
  IFS0bits.T3IF = 0;        //Clear Timer 3 Interrupt flag
  IEC0bits.T3IE = 0;        //Disable Timer 3 Interrupt
  T3CONbits.TON = 1;        //Start Timer 3 operation
  //SPI-1 Configuration (SD-Card)
  SPI1STATbits.SPIEN = 0;	//Disable SPI-1
  SPI1STATbits.SPISIDL = 1;	//Disable Operation in Idle Mode
  SPI1CON1bits.MODE16 = 0;  //Byte Communication
  SPI1CON1bits.MSTEN = 1;   //MasterMode ON
  SPI1CON1bits.DISSDO = 0;  //SDO Pin Peripheral Controlled
  SPI1CON1bits.SMP = 1;     //Data Sampled at end of Output
  SPI1CON1bits.CKE = 0;     //Serial Transmission on Transition from Idle to Active
  SPI1CON1bits.CKP = 0;     //Clock Polarity is Active High
  SPI1CON1bits.PPRE = 3;	//Set N1=1
  SPI1CON1bits.SPRE = 3;	//Set N2=5 for 8MHz Operation
  SPI1CON2bits.FRMEN = 0;   //Framed Mode Disabled
  SPI1CON1bits.SSEN = 0;    //SS1 Pin Controlled by Port Function
  IFS0bits.SPI1IF = 0;      //Clear Interrupt Flag
  IEC0bits.SPI1IE = 0;      //Disable Interrupt
  DMA1CONbits.CHEN = 0;     //DMA CH1 Disabled
  DMA2CONbits.CHEN = 0;     //DMA CH2 Disabled
  DMA1CONbits.SIZE = 1;     //Set to Byte Transfer
  DMA2CONbits.SIZE = 1;     //Set to Byte Transfer
  DMA1CONbits.DIR = 1;      //Read from SRAM to Peripheral
  DMA2CONbits.DIR = 0;      //Read from Peripheral to SRAM
  DMA1CONbits.HALF = 0;     //Initiate DMA Interrupt when all data written
  DMA2CONbits.HALF = 0;     //Initiate DMA Interrupt when all data written
  DMA1CONbits.NULLW = 0;    //Normal Operation, no write back
  DMA2CONbits.NULLW = 0;    //Normal Operation, no write back
  DMA1CONbits.AMODE = 0;    //Register Indirect with Post Increment
  DMA2CONbits.AMODE = 0;    //Register Indirect with Post Increment
  DMA1CONbits.MODE = 1;     //One Shot Mode without Ping-Pong
  DMA2CONbits.MODE = 0;     //Continuous Mode without Ping-Pong
  DMA1REQbits.IRQSEL = 10;  //Use SPI-1 Peripheral
  DMA2REQbits.IRQSEL = 10;  //Use SPI-1 Peripheral
  DMA1CNT = SDCard_BufferSize;                //Number of samples in DMA request
  DMA2CNT = SDCard_BufferSize;                //Number of samples in DMA request
  DMA1STA = __builtin_dmaoffset(DMA1_Buffer); //BufferA Address
  DMA2STA = __builtin_dmaoffset(DMA1_Buffer); //BufferA Address
  DMA1STB = __builtin_dmaoffset(DMA1_Buffer); //BufferB Address
  DMA2STB = __builtin_dmaoffset(DMA1_Buffer); //BufferB Address
  DMA1PAD = (volatile unsigned int)&SPI1BUF;  //Grab Data from SPI1BUF
  DMA2PAD = (volatile unsigned int)&SPI1BUF;  //Grab Data from SPI1BUF
  IFS0bits.DMA1IF = 0;      //Clear DMA Interrupt Flag bit
  IFS1bits.DMA2IF = 0;      //Clear DMA Interrupt Flag bit
  IEC0bits.DMA1IE = 1;      //Enable DMA CH1 Interrupts
  IEC1bits.DMA2IE = 1;      //Enable DMA CH2 Interrupts
  DMA1CONbits.CHEN = 1;     //Enable DMA CH1
  DMA2CONbits.CHEN = 1;     //Enable DMA CH2
        //SPI1STATbits.SPIEN = 1;   //Enable SPI-1
        //Use DMA1REQbits.FORCE = 1; to start a SPI Transfer Procedure
  //SPI-2 Configuration (LCD)
  SPI2STATbits.SPIEN = 0;	//Disable SPI-1
  SPI2STATbits.SPISIDL = 1;	//Disable Operation in Idle Mode
  SPI2CON1bits.MODE16 = 0;  //Byte Communication
  SPI2CON1bits.MSTEN = 1;   //MasterMode ON
  SPI2CON1bits.DISSDO = 0;  //SDO Pin Peripheral Controlled
  SPI2CON1bits.SMP = 1;     //Data Sampled at end of Output
  SPI2CON1bits.CKE = 0;     //Serial Transmission on Transition from Idle to Active
  SPI2CON1bits.CKP = 1;     //Clock Polarity is Active Low
  SPI2CON1bits.PPRE = 1;	//Set N1=1
  SPI2CON1bits.SPRE = 7;	//Set N2=2 for 2.5MHz Operation
  SPI2CON2bits.FRMEN = 0;   //Framed Mode Disabled
  SPI2CON1bits.SSEN = 0;    //SS1 Pin Controlled by Port Function
  IFS2bits.SPI2IF = 0;      //Clear Interrupt Flag
  IEC2bits.SPI2IE = 0;      //Disable Interrupt
  DMA3CONbits.CHEN = 0;     //DMA CH3 Disabled
  DMA4CONbits.CHEN = 0;     //DMA CH4 Disabled
  DMA3CONbits.SIZE = 1;     //Set to Byte Transfer
  DMA4CONbits.SIZE = 1;     //Set to Byte Transfer
  DMA3CONbits.DIR = 1;      //Read from SRAM to Peripheral
  DMA4CONbits.DIR = 0;      //Read from Peripheral to SRAM
  DMA3CONbits.HALF = 0;     //Initiate DMA Interrupt when all data written
  DMA4CONbits.HALF = 0;     //Initiate DMA Interrupt when all data written
  DMA3CONbits.NULLW = 0;    //Normal Operation, no write back
  DMA4CONbits.NULLW = 0;    //Normal Operation, no write back
  DMA3CONbits.AMODE = 0;    //Register Indirect with Post Increment
  DMA4CONbits.AMODE = 0;    //Register Indirect with Post Increment
  DMA3CONbits.MODE = 1;     //One Shot Mode without Ping-Pong
  DMA4CONbits.MODE = 0;     //Continuous Mode without Ping-Pong
  DMA3REQbits.IRQSEL = 33;  //Use SPI-2 Peripheral
  DMA4REQbits.IRQSEL = 33;  //Use SPI-2 Peripheral
  DMA3CNT = LCD_BufferSize; //Number of samples in DMA request
  DMA4CNT = LCD_BufferSize; //Number of samples in DMA request
  DMA3STA = __builtin_dmaoffset(DMA3_Buffer); //BufferA Address
  DMA4STA = __builtin_dmaoffset(DMA4_Buffer); //BufferA Address
  DMA3STB = __builtin_dmaoffset(DMA3_Buffer); //BufferB Address
  DMA4STB = __builtin_dmaoffset(DMA4_Buffer); //BufferB Address
  DMA3PAD = (volatile unsigned int)&SPI2BUF;  //Grab Data from SPI2BUF
  DMA4PAD = (volatile unsigned int)&SPI2BUF;  //Grab Data from SPI2BUF
  IFS2bits.DMA3IF = 0;      //Clear DMA Interrupt Flag bit
  IFS2bits.DMA4IF = 0;      //Clear DMA Interrupt Flag bit
  IEC2bits.DMA3IE = 1;      //Enable DMA CH3 Interrupts
  IEC2bits.DMA4IE = 1;      //Enable DMA CH4 Interrupts
  DMA3CONbits.CHEN = 1;     //Enable DMA CH3
  DMA4CONbits.CHEN = 1;     //Enable DMA CH4
  SPI2STATbits.SPIEN = 1;   //Enable SPI-2
        //Use DMA1REQbits.FORCE = 1; to start a SPI Transfer Procedure
  //Interrupt Priority Levels
  INTCON1bits.NSTDIS = 1; //Interrupt Nesting Disabled
  CORCONbits.IPL3 = 0;    //CPU Priority Level < 7
  SRbits.IPL = 1;         //CPU Priority Level = 1
  IPC5bits.INT1IP = 2;    //INT-1 Priority Level = 2
  //Enable Start Button Interrupt (INT-1)
  IFS1bits.INT1IF = 0;      //Clear INT-1 Flag
  IEC1bits.INT1IE = 1;      //Enable INT-1
}
//CollectData, starts ADC sampling and delivers chunks to preprocessing
void CollectData(){
    char i;
    LCD_SendString(1,"Measurement Starts In");
    for (i=3;i<0;i--){
        LCD_SendString(2,"    "i);
        //Delay(1000);
    }
    AD1CON1bits.ADON = 1;   //Turn ON ADC1 (rest is interrupt driven)
}
//Preprocessing, 
void Preprocessing(){
    int ActiveBlock =0;             //Initialize Active Block
    FIRStructInit(&PPFilter,PPnumCoefs,&PPcoeffs[0],PPcoeffsPage,&PPdelay[0]);//Setup PPFilter
    FIRDelayInit(&PPFilter);        //Reset Delay Values in FIR Structure
    while(!Status.preprocess){      //Wait for Preprocessing to Finish
        if (ActiveBlock < ECGBlocksReady){    //If Next Block is Ready
            //Apply the Preprocessing Filter to the Block
            FIR(ADC1_BufferSize,&ECGData[ADC1_BufferSize*ActiveBlock],&ECGData[ADC1_BufferSize*ActiveBlock],&PPFilter);
            if (SRbits.SAB == 1){   //Check for Register Saturation
                Status.Error = 1;
            }
            LCD_SendString(1,"Checking Block " ActiveBlock);

            //Prepare for R-peak Detection (perform if possible)
            //---------------------------------------------------
            
            //Perform Wavelet Decomposition (can be done per block)
            
            

            //Cut the top off (remove anything greater than zero)
            int i;
            for (i=0;i<ADC1_BufferSize;i++){
                if (WorkingBuffer[i]>0){    //check bit instead its quicker
                    WorkingBuffer[i] = 0;   //is there a clear operation thats faster?
                }
                else{
                    //negate the value
                }
            }            

            
            //RPeakLocs = peakFind();
            
            
            
            
            
            ActiveBlock++;      //Increment ActiveBlock
        }
        
        
        //FREETIME HERE???
                
        if (ActiveBlock == ECGLength/ADC1_BufferSize){
            Status.preprocess = 1;  //Preprocessing Finished
        }
    }
}


//WIP
void Sectioning(){
    Nop();
}
//WIP
void FeatureExtraction(){
	Nop();
}
//WIP
void FeatureNormalization(){
    //Get FeaturesNorms from SD Card
    FIL file;
    f_open(&file, "FeatureNorms", FA_READ);
    
    
    f_close(&file);
    
    
}
//WIP
void TestClassifiers(){
	Nop();
}
//WIP
void authorizeCheck(){
	Nop();
}
//LockReply, if the user passes then start unlocking sequence otherwise deny entry
void LockReply(){
    if (Status.Open == 1){
        LATBbits.LATB2 = 1; //Open Door
        LCD_SendString(1,"Access Granted");
        Delay(10000);       //10 second Delay for User to get inside
        LATBbits.LATB2 = 0; //Close Door
        Status.Open = 0;   //Reset the Lock Status
    }
    else{
        LCD_SendString(1,"Access Denied");
        Delay(5000);      //5 second Delay for User to read message
    }
}

