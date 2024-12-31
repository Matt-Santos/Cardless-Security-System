// ECG Biometric Processor Functions
// Written by Matthew Santos

//Includes
#include "xc.h"             //Varriable Refrences
#include "dsp.h"            //DSP Library
#include "Varriables.h"     //Program Varriables
#include "libpic30.h"       //Compiler Helper Library
#include "LCD_Functions.h"  //LCD Function Definitions
#include "ff.h"             //SD Card Functionality

//Function Definitions
//-----------------------

//Delay, causes CPU to pause for specified milliseconds [1,100000] limits
void Delay(int msec){
    int i;
    for (i=0;i<msec;i++){
        __delay32(ClockSpeed);
    }
}
//WriteECGDataToSD, Transfers the Contents of ECGData to SDCard
void WriteECGDataToSD(const char *filename){
    FATFS FatFs;
    FIL fil;
    FRESULT st;
    char report[4]={0};
    LCD_Clear();LCD_L1();
    unsigned int Nwriten=0;
    LCD_SendString("Mount SD:");
    st = f_mount(&FatFs,"",1);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Open File:");
    st = f_open(&fil,filename,FA_READ | FA_WRITE);
    itoa(report,st,16);
    LCD_SendString(report);
    Delay(1000);
    LCD_Clear();LCD_L1();
    LCD_SendString("Write:");
    st = f_write(&fil,&ECGData,2*ECGLength,&Nwriten);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Close File:");
    st = f_close(&fil);
    itoa(report,st,16);
    LCD_SendString(report);
    Delay(1000);
    LCD_Clear();LCD_L1();
    LCD_SendString("Umount:");
    st = f_mount(0,"",0);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Bytes Written:");
    itoa(report,Nwriten,10);
    LCD_SendString(report);
    Delay(1000);
}
//PeakFind, algorithm used to search ECG Data for specific peaks
void PeakFind(fractional x,fractional y,fractional MinHeight, int MinDistance){
    //fractional PeakLocations[10];
    //Apply Derivative Filter
    /*
    FIR(ECGLength,WorkingBuffer,ECGData,&DFilter);
    int i;
    for (i=0;i<ECGLength-1;i++){
        if((WorkingBuffer[i]&0x8000) != (WorkingBuffer[i+1]&0x8000)){ //if signs differ
            if(ECGData[i] >= MinHeight){
                //R Peak Detected Add to array
                PeakLocations[] = i;
            }
        }
    }
    */
}
//startup, perform startup configuration on EVERYTHING!
void startup(){
  //Establish PLL Settings
  PLLFBD = 0x1E;            //M=32
  CLKDIVbits.PLLPRE = 0;    //N1=2
  CLKDIVbits.PLLPOST = 0;   //N2=2
  //Use HS External Clock Source
    __builtin_write_OSCCONH(0x3);    //Assign HS with PLL
    __builtin_write_OSCCONL(0x3);    //Maintain SOSC and Switch
    while(OSCCONbits.COSC != 3);     //Wait for HS Mode
    while(OSCCONbits.LOCK != 1);     //Wait for PLL Lock
  //Use FRC Clock with Pre-scaler
  /*
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    while (OSCCONbits.COSC != 0b001);
    while(OSCCONbits.LOCK != 1);
  */
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
  TRISBbits.TRISB12 = 1;    //Pin 23 => (Input)
  TRISBbits.TRISB13 = 0;    //Pin 24 => (Output)
  TRISBbits.TRISB14 = 0;    //Pin 25 => (Output)
  TRISBbits.TRISB15 = 0;    //Pin 26 => (Output)
  //SD-Card Pin-outs (SPI-1)
  RPINR20bits.SDI1R = 7;    //RP7    => SD SPI-1 Data In
  RPOR4bits.RP8R = 8;       //RP8    => SD SPI-1 Clock
  RPOR4bits.RP9R = 7;       //RP9    => SD SPI-1 Data Out
  RPOR5bits.RP10R = 9;      //RP10   => SD SPI-1 Enable
  //LCD Pin-outs
  RPOR6bits.RP12R = 0;      //RP12   => LCD SPI-2 Data In
  RPOR6bits.RP13R = 0;      //RP13   => LCD SPI-2 Enable
  RPOR7bits.RP14R = 0;      //RP14   => LCD SPI-2 Data Out
  RPOR7bits.RP15R = 0;      //RP15   => LCD SPI-2 Clock
  //External Interrupts (Inputs)
  RPINR0bits.INT1R = 3;     //RP3    => INT-1 (START)
  //Digital Outputs (Outputs)
  RPOR0bits.RP0R = 0;       //RP0    => RP0   (AUX2)
  RPOR0bits.RP1R = 0;       //RP1    => RP1   (AUX3)
  RPOR1bits.RP2R = 0;       //RP2    => RP2   (AUX4)
  RPOR5bits.RP11R = 0;      //RP11   => RP11  (AUX5)
  LATBbits.LATB2 = 1;       //Stop the Motor Right Away
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
  //Interrupt Priority Levels
  INTCON1bits.NSTDIS = 1;   //Interrupt Nesting Disabled
  CORCONbits.IPL3 = 0;      //CPU Priority Level < 7
  SRbits.IPL = 1;           //CPU Priority Level = 1
  IPC5bits.INT1IP = 2;      //INT-1 Priority Level = 2
  //Enable Start Button Interrupt (INT-1)
  IFS1bits.INT1IF = 0;      //Clear INT-1 Flag
  IEC1bits.INT1IE = 1;      //Enable INT-1
  //SPI-1 Configuration (SD-Card)
  SPI1STATbits.SPIEN = 0;	//Disable SPI-1
  SPI1STATbits.SPISIDL = 1;	//Disable Operation in Idle Mode
  SPI1CON1bits.MODE16 = 0;  //Byte Communication
  SPI1CON1bits.MSTEN = 1;   //MasterMode ON
  SPI1CON1bits.DISSDO = 0;  //SDO Pin Peripheral Controlled
  SPI1CON1bits.SMP = 1;     //Data Sampled at end of Output
  SPI1CON1bits.CKE = 1;     //Serial Transmission on Transition from Idle to Active
  SPI1CON1bits.CKP = 0;     //Clock Polarity is Active High
  SPI1CON1bits.PPRE = 3;	//Set N1=1
  SPI1CON1bits.SPRE = 3;	//Set N2=5 for 8MHz Operation
  SPI1CON2bits.FRMEN = 0;   //Framed Mode Disabled
  SPI1CON1bits.SSEN = 0;    //SS1 Pin Controlled by Port Function
  IFS0bits.SPI1IF = 0;      //Clear Interrupt Flag
  IEC0bits.SPI1IE = 1;      //Enable Interrupt //IEC0bits.SPI1IE = 0;      //Disable Interrupt
  /*
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
  */
  //Timer 1 Setup (1KHz)
  T1CONbits.TON = 0;        //Disable Timer
  T1CONbits.TCS = 0;        //Select internal instruction cycle clock
  T1CONbits.TGATE = 0;      //Disable Gated Timer mode
  T1CONbits.TCKPS = 2;      //Select 1:64 Prescaler
  TMR1 = 0x00;              //Clear timer register
  PR1 = 625;                //Load the period value
  IPC0bits.T1IP = 0x01;     //Set Timer1 Interrupt Priority Level
  IFS0bits.T1IF = 0;        //Clear Timer1 Interrupt Flag
  IEC0bits.T1IE = 1;        //Enable Timer1 interrupt
  T1CONbits.TON = 1;        //Start Timer
}

//CollectData, starts ADC sampling and delivers chunks to preprocessing
void CollectData(){
    LCD_Clear();                        //Clear Display
    LCD_L1();                           //Goto Line 1
    LCD_SendString("      Get Ready");  //Update User Status
    int i;
    for (i=0;i<16;i++){                 //Every 500ms
        LCD_Bar(i);                     //Update the Progress Bar
        Delay(500);
    }
    AD1CON1bits.ADON = 1;   //Turn ON ADC1 (rest is interrupt driven)
    LCD_Clear();                        //Clear Display
    LCD_L1();                           //Goto Line 1
    LCD_SendString("Measuring ECG");    //Output Status Indicator
    LCD_Bar(0);                         //Output Progress
    while(!Status.sample){              //Wait for Measurement to Finish
        LCD_Bar(16*ECGBlocksReady/(ECGLength/ADC1_BufferSize));//Update Progress
        if (ECGBlocksReady == ECGLength/ADC1_BufferSize){
            Status.sample = 1;          //Measurement Finished
            LCD_Bar(16);                //Update User Status
        }
    }
}
//Preprocessing, performs the initial filter operations on the incoming data
void Preprocessing(){
    int ActiveBlock =0;                 //Initialize Active Block
    IIRCanonicInit(&PPFilter);          //Reset & Initialize PPFilter
    LCD_Clear();                        //Clear Display
    LCD_L1();                           //Goto Line 1
    LCD_SendString("Pre-Processing");   //Output Status Indicator
    LCD_Bar(0);                         //Output Progress
    while(!Status.preprocess){          //Wait for Preprocessing to Finish
        if (ActiveBlock < ECGBlocksReady){    //If Next Block is Ready
            //Apply the Preprocessing Filter to the Block
            IIRCanonic(ADC1_BufferSize,WorkingBuffer,&ECGData[ADC1_BufferSize*ActiveBlock],&PPFilter);
            if (SRbits.SAB == 1){       //Check for Register Saturation
                Status.Error = 1;       //Error if Event Occurred
            }
            int i;
            for (i=0;i<ADC1_BufferSize-2;i++){
                ECGData[ADC1_BufferSize*ActiveBlock+i] = WorkingBuffer[i];
            }
            //Perform Wavelet Decomposition (can be done per block)
            
            ActiveBlock++;      //Increment ActiveBlock
        }
        LCD_Bar(16*ActiveBlock/(ECGLength/ADC1_BufferSize));    //Update Progress
        if (ActiveBlock == ECGLength/ADC1_BufferSize){
            Status.preprocess = 1;  //Preprocessing Finished
            LCD_Bar(16);
        }
    }
}
//Sectioning, condense the ECG into a single average waveform
void Sectioning(){
    LCD_Clear();LCD_L1();               //Reset the LCD
    LCD_SendString("Normalizing Data"); //User Status Update
    //Perform ECG Normalization
    //int temp;
    //fractional Max = VectorMax(ECGLength,ECGData,&temp);
    //fractional Min = VectorMin(ECGLength,ECGData,&temp);
    //VectorScale(ECGLength,WorkingBuffer,ECGData,);
    

    
    //R-peak Detection
    //---------------------------------------------------
        //FIR UFWT Filter
        //Remove anything greater than zero (Or ignore)
        //Take the absolute value of the signal
        //Execute find peaks routine with predefined settings
    
    //ECGData = avg(Sections);
    
}
//
void FeatureExtraction(){
    LCD_Clear();LCD_L1();               //Reset the LCD
    LCD_SendString("Feature Extraction"); //User Status Update
    Delay(700);
    //Find the Peak Locations
    int P,Q,R,S,T;  
    //R = RPeakDetector();
    //P = PPeakDetector();
    //Q = QPeakDetector();
    //S = SPeakDetector();
    //T = TPeakDetector();
    //Perform Feature the Calculations
    F.HR = 0;
    F.HRV = 0;
    F.QR_T = Q-R;
    F.RS_T = R-S;
    F.QT_T = Q-T;
    F.PS_T = P-S;
    F.PQ_T = P-Q;
    F.ST_T = S-T;
    F.RT_T = R-T;
    F.PR_T = P-R;
    F.PT_T = P-T;
    F.QR_S = (ECGData[Q]-ECGData[R])/(F.QR_T);  //DIVF
    F.RS_S = (ECGData[R]-ECGData[S])/(F.RS_T);
    F.PT_S = (ECGData[P]-ECGData[T])/(F.PT_T);
    F.PR_S = (ECGData[P]-ECGData[R])/(F.PR_T);
    F.RT_S = (ECGData[R]-ECGData[T])/(F.RT_T);
    F.PQ_S = (ECGData[P]-ECGData[Q])/(F.PQ_T);
    F.ST_S = (ECGData[S]-ECGData[T])/(F.ST_T);
    F.QT_S = (ECGData[Q]-ECGData[T])/(F.QT_T);
    F.PS_S = (ECGData[P]-ECGData[S])/(F.PS_T);
    F.RP_A = (ECGData[R]-ECGData[P]);
    F.PT_A = (ECGData[P]-ECGData[T]);
    F.PQ_A = (ECGData[P]-ECGData[Q]);
    F.TS_A = (ECGData[T]-ECGData[S]);
    F.RQ_A = (ECGData[R]-ECGData[Q]);
    F.RS_A = (ECGData[R]-ECGData[S]);
    F.QS_A = (ECGData[Q]-ECGData[S]);
}
//WIP
void FeatureNormalization(){
    LCD_Clear();LCD_L1();               //Reset the LCD
    LCD_SendString("Feature Normalize"); //User Status Update
    Delay(2000);
    //Read Normalization Factors
    FATFS FatFs;
    FIL fil;
    FRESULT st;
    char report[4]={0};
    LCD_Clear();LCD_L1();
    unsigned int Nwriten=0;
    LCD_SendString("Mount SD:");
    st = 0;//f_mount(&FatFs,"",1);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Open File:");
    st = 0;//f_open(&fil,"feature_norms",FA_READ | FA_WRITE);
    itoa(report,st,16);
    LCD_SendString(report);
    Delay(1000);
    LCD_Clear();LCD_L1();
    LCD_SendString("Read:");
    st = 0;//f_write(&fil,&ECGData,2*ECGLength,&Nwriten);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Close File:");
    st = 0;//f_close(&fil);
    itoa(report,st,16);
    LCD_SendString(report);
    Delay(1000);
    LCD_Clear();LCD_L1();
    LCD_SendString("Umount:");
    st = 0;//f_mount(0,"",0);
    itoa(report,st,16);
    LCD_SendString(report);
    LCD_L2();
    LCD_SendString("Bytes Written:");
    itoa(report,Nwriten,10);
    LCD_SendString(report);
    Delay(1000);
 
}
//WIP
void TestClassifiers(){
    LCD_Clear();LCD_L1();               //Reset the LCD
    LCD_SendString("Test Classifiers"); //User Status Update
    //Obtain Classifiers
    
    
	Nop();
}
//WIP
void authorizeCheck(){
    //Allow entry for now
    if(ECGData[1]>Q15(0.3)){
        Status.Open =1;
    }
}
//LockReply, if the user passes then start unlocking sequence or deny entry
void LockReply(){
    if (Status.Open){
        LCD_Clear();                            //Clear Display
        LCD_L1();                               //Goto Line 1
        LCD_SendString("   Access Granted");    //Access Message
        LATBbits.LATB2 = 0;                     //Open Door
        Delay(1000);                            //Motor Delay
        LATBbits.LATB2 = 1;                     //Stop Motor
        Delay(10000);                           //Delay for User to get inside
        LATBbits.LATB2 = 0;                     //Close Door
        Delay(1000);                            //Motor Delay
        LATBbits.LATB2 = 1;                     //Stop Motor
        Status.Open = 0;                        //Reset the Lock Status
    }
    else{
        LCD_Clear();                            //Clear Display
        LCD_L1();                               //Goto Line 1
        LCD_SendString("   Access Denied");     //Access Message
        Delay(5000);                            //Delay for User to read message
    }
}