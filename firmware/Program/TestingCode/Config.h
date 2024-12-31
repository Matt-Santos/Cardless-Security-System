// ECG Biometric Processor Configuration Settings
// Written by Matthew Santos

#ifndef CONFIG_HEADER
#define CONFIG_HEADER


//Configuration Bits
#pragma config BWRP = WRPROTECT_OFF //Boot Segment Writeable
#pragma config BSS = NO_BOOT_CODE   //No Boot Section Specified
#pragma config RBS = NO_BOOT_RAM    //No Boot RAM Specified
#pragma config SWRP = WRPROTECT_OFF //Secure Segment Writeable
#pragma config SSS = NO_SEC_CODE    //No Secure Segment Specified
#pragma config RSS = NO_SEC_RAM     //No Secure RAM Specified
#pragma config GWRP = OFF           //Program Memory Writeable
#pragma config GSS = GSS_OFF        //Program Memory Not Protected
    //#pragma config FNOSC = SOSC         //Oscilator is Primary External with PLL
    //#pragma config IESO = ON            //Startup with Internal CLK then Switch
    //#pragma config POSCMD = HS          //Primary Oscilator in HS Crystal Mode
    #pragma config FNOSC = FRC           //Oscilator is Primary External with PLL
    #pragma config IESO = OFF            //Startup with Internal CLK then Switch
    #pragma config POSCMD = NONE         //Primary Oscilator in HS Crystal Mode
#pragma config OSCIOFNC = ON        //Dont Care we are using HS Mode
#pragma config IOL1WAY = ON         //Pin Settings Only Configured Once
#pragma config FCKSM = CSECME       //Clock Switching & Monitoring Enabled
#pragma config WDTPOST = PS1        //Watchdog PostScaler
#pragma config WINDIS = OFF         //Watchdog NonWindowed Mode
#pragma config FWDTEN = OFF         //Watchdog Timer Can be Enabled/Disabled
#pragma config FPWRT = PWR128       //Power-on Reset After 128ms
#pragma config ALTI2C = OFF         //IC2 Set to SDA1/SCL1 pins
#pragma config ICS = PGD3           //Comms on PGD3/PGC3 pins
#pragma config JTAGEN = OFF         //Disable JTAG
#pragma config UID0 = 0x0000004C    //Device ID0 = "L"
#pragma config UID1 = 0x0000004F    //Device ID1 = "O"
#pragma config UID2 = 0x00000043    //Device ID2 = "C"
#pragma config UID3 = 0x0000004B    //Device ID3 = "K"

#endif
