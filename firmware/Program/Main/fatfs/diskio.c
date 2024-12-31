// DSPIC33 Fatfs SPI Low Level Implementation with DMA
//Originally written by Jonas Meyer but heavily modified by Matthew Santos
//http://www.microchip.com/forums/m689217.aspx

//Includes
#include "xc.h"         //Varriable Refrences
#include "diskio.h"		//FatFs lower layer API

//Command Definitions
#define SD_CMD_IDLE                      (0x40+0)        /* GO_IDLE_STATE */
#define SD_CMD_OP_COND                   (0x40+1)        /* SEND_OP_COND (MMC) */
#define SD_CMD_IF_COND                   (0x40+8)        /* SEND_IF_COND */
#define SD_CMD_CSD                       (0x40+9)        /* SEND_CSD */
#define SD_CMD_CID                       (0x40+10)       /* SEND_CID */
#define SD_CMD_STOP_TRANSMISSION         (0x40+12)       /* STOP_TRANSMISSION */
#define SD_CMD_SET_BLOCKLEN              (0x40+16)       /* SET_BLOCKLEN */
#define SD_CMD_READ_SINGLE_BLOCK         (0x40+17)       /* READ_SINGLE_BLOCK */
#define SD_CMD_READ_MULTIPLE_BLOCK       (0x40+18)       /* READ_MULTIPLE_BLOCK */
#define SD_CMD_SET_BLOCK_COUNT           (0x40+23)       /* SET_BLOCK_COUNT (MMC) */
#define SD_CMD_WRITE_BLOCK               (0x40+24)       /* WRITE_BLOCK */
#define SD_CMD_WRITE_MULTIPLE_BLOCK      (0x40+25)       /* WRITE_MULTIPLE_BLOCK */
#define SD_CMD_APP_CMD                   (0x40+55)       /* APP_CMD */
#define SD_CMD_READ_OCR                  (0x40+58)       /* READ_OCR */
#define SD_ACMD_OP_COND                  (0xC0+41)       /* SEND_OP_COND (SDC) */
#define SD_ACMD_STATUS                   (0xC0+13)       /* SD_STATUS (SDC) */
#define SD_ACMD_SET_WR_BLOCK_ERASE_COUNT (0xC0+23)       /* SET_WR_BLK_ERASE_COUNT (SDC) */


void SetChipSelect(int){
    //todo
}


static BYTE SDSendCmd(BYTE cmd, DWORD arg){

}


static inline __attribute__((always_inline)) BYTE SDWaitRdy(void){
	BYTE res=0;
    WriteSPI1(0xFF);    //one more write than reads in the beginning to keep the rx buffer filled
	while(res != 0xFF){
        Delay(500);     // wait for a timeout of 500ms
        WriteSPI1(0xFF);
        res=getcSPI();
    }
        getcSPI();      //flush the extra rx buffer element
	return res;
}
static inline  __attribute__((always_inline)) BYTE SDTransferByte(BYTE SendByte)
{
    WriteSPI1((unsigned int) SendByte);     //assume there are free elements in the TX Buffer blindly.
    while(SPI1STATbits.SPIRBF);             //wait for the RX BUf to contain data
    return((BYTE)SPI1BUF);
}
//disk_status, returns the status of the drive specified by pdrv
DSTATUS disk_status (void){
	return SD_Status;
}
//disk_initialize, used to initialize the SD-Card
DSTATUS disk_initialize (void){
    if (SD_Status & STA_NODISK) return SD_Status;   //No card detected
    SetChipSelect(0);
    Delay(5);
    SPI1STATbits.SPIEN = 1;                         //Enable SPI
    Delay(5);
    SPI1CON1bits.PPRE = 0;                          //Change SPI Speed to 125kHz
    Delay(5);
    BYTE n;BYTE bData[4];
    for(n=10;n;n--){
        SDTransferByte(0xFF);
    }
    SD_CardType = 0;
    if (SDSendCmd(SD_CMD_IDLE, 0) == 1){                        //Attempt to put the Card in Idle Mode
        if (SDSendCmd(SD_CMD_IF_COND, 0x1AA) == 1){             //If the card is the SDHC Type
            for (n=0;n<4;n++){                                  //Read 4 Replies
                bData[n] = SDTransferByte(0xFF);                //Store Replies
            }
            if ((bData[2] == 0x01) && (bData[3] == 0xAA)){      //If Reply is SDC V2
                while (SDSendCmd(SD_ACMD_OP_COND, 1UL << 30)){
                    Delay(1000);                                //Wait 1000ms between attempts
                }
                if (SDSendCmd(SD_CMD_READ_OCR, 0) == 0){
                    for (n = 0; n < 4; n++){                    //Read 4 Replies
                        bData[n] = SDTransferByte(0xFF);        //Store Replies
                    }
                    SD_CardType = (bData[0] & 0x40) ? 12 : 4;   //Update Card Type
                }
            }           
        }
        else if(SDSendCmd(SD_ACMD_OP_COND, 0) <= 1){    //If the Card is SDC Type
            SD_CardType = 2;                            //Update the Card Type
        }
        else{                                           //The card must then be MMC Type
            SD_CardType = 1;                            //Update the Card Type
        }
    }
    SPI1CON1bits.PPRE = 3;                              //Change SPI Speed back to 8MHz
    if (SD_CardType == 1){
        while (SDSendCmd(SD_CMD_OP_COND, 0)){           //wait for the card to leave idle state
            Delay(1000);                                //Wait 1000ms between attempts
        }
        if (SDSendCmd(SD_CMD_SET_BLOCKLEN, 512) != 0)   //Set Block Length to 512
				SD_CardType = 0;
    }
    else if(SD_CardType == 2){
        while (SDSendCmd(SD_ACMD_OP_COND, 0)){          //wait for the card to leave idle state
            Delay(1000);                                //Wait 1000ms between attempts
        }
        if (SDSendCmd(SD_CMD_SET_BLOCKLEN, 512) != 0)   //Set Block Length to 512
				SD_CardType = 0;
    }
    SetChipSelect(0);               //Deselect the Chip
    SDTransferByte(0xFF);
    if (SD_CardType!=0){            //Success
        SD_Status &= ~STA_NOINIT;   //Update Status
    }
    else{                           //Failure
        SPI1STATbits.SPIEN = 0;     //Shutdown SPI
    }
    SetChipSelect(0);               //Deselect the Chip
    return SD_Status;               //Return Status
}
//disk_read, used to read blocks from the SD-Card
DRESULT disk_read (BYTE *buff,DWORD sector,UINT count){

}
//disk_write, used to write blocks to the SD-Card
DRESULT disk_write (const BYTE *buff,DWORD sector,UINT count){

}

DRESULT disk_ioctl (BYTE cmd,void *buff){

}

