/**
  ******************************************************************************
  * @file    STM32_MFRC522.h
  * @brief   This file contains the headers of MFRC522 modules
  ******************************************************************************
  * @attention
  * 
  *
 ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_RFID_H
#define STM32_RFID_H

#define MAX_LENGTH 16
#define u_char unsigned char
#define uint   unsigned int
#include "main.h"
#include "stm32f1xx_hal.h"
//Private Command Define for MFRC522
#define PCD_IDLE            0x00    //No action
#define PCD_MEM             0x01    //Stores 25bytes into the internal buffer
#define GENERATE_RANDOM_ID  0x02    //Generates a 10-bytes random ID number
#define PCD_CALCCRC         0x03    //Caculate CRC number
#define PCD_TRANSMIT        0x04    //Transmit Data
#define PCD_NO_CMD_CHANGE   0x07    //Can be used to modify the CommandReg Register bits without affecting the command, for example, the PowerDown bit
#define PCD_RECEIVE         0x08    //Receive Data
#define PCD_AUTHENT         0x0E    //Performs the MIFARE standard authentication as a reader
#define PCD_TRANCEIVE       0x0C    //Transmit and Receive Data
#define PCD_SOFTRESET       0x0F    //Reset

//Define Register Command for Midare_One
#define PICC_REQIDL         0x26    // find the antenna area does not enter hibernation
#define PICC_REQALL         0x52    //All type of clard
#define PICC_ANTICOLL       0x93    //Anti coll cards
#define PICC_SELECTTAG      0x93    //Optional Card
#define PICC_AUTHENT1A      0x60    //Authent right
#define PICC_AUTHENT1B      0x61    //Authent B button
#define PICC_READ           0x30    //Read Block
#define PICC_WRITE          0xA0    //Write Block
#define PICC_DECREMENT      0xC0    
#define PICC_INCREMENT      0xC1    
#define PICC_RESTORE        0xC2    //Moce data block into counter
#define PICC_TRANSFER       0xB0    //Restored in Data counter
#define PICC_HALT           0x50    //Into Sleep Mode

//Comunication Error and Reponse Code Tab
#define MI_OK             0
#define MI_NOTAGERR       1
#define MI_ERR            2
//Command and status
#define Reserved00          0x00    //reserved for future use
#define CommandReg          0x01    //starts and stops command excution
#define ComIEnReg           0x02    //enable and dsiable interrupt request control bits
#define DivIENReg           0x03    //enable and disable interrupt request control bits
#define ComIrqReg           0x04    //interrupt request bits
#define DivIrqReg           0x05    //interrupt request bits
#define ErrorReg            0x06    //error bits showing the error status of the last command excuted
#define Status1Reg          0x07    //comunication status bits
#define Status2Reg          0x08    //receiver and transmitter status bits
#define FIFODataReg         0x09    //input and output of 64 byte FIFO buffer
#define FIFOLevelReg        0x0A    //number of bytes stored in the FIFO buffer
#define WaterLevelReg       0x0B    //level for FIFO underflow and overflow warning
#define ControlReg          0x0C    //miscellaneous control registers
#define BitFramingReg       0x0D    //adjustments for bit-oriented frames
#define CollReg             0x0E    //bit position of the first bit-collision detected on the RF interface
#define Reserved01          0x0F    //reserved for future use

//Command
#define Reserved10          0x10    //reserved for future use
#define ModeReg             0x11    //defines general modes for transmitting and receiving
#define TxModeReg           0x12    //defines transmission data rate and framing
#define RxModeReg           0x13    //defines reception data rate and framing
#define TxControlReg        0x14    //controls the logical behavior of the antenna driver pins TX1 and TX2
#define TxASKReg            0x15    //controls the setting of the transmission modulation
#define TxSelReg            0x16    //selects the internal sources for the antenna driver
#define RxSelReg            0x17    //selects internal receiver settings
#define RxThresholdReg      0x18    //selects thresholds for the bit decoder
#define DemodReg            0x19    //defines demodulator settings
#define Reserved11          0x1A    //reserved for future use
#define Reserved12          0x1B    //reserved for future use
#define MfTxReg             0x1C    //controls some MIFARE communication transmit parameters
#define MfRxReg             0x1D    //controls some MIFARE communication receive paramaters
#define Reserved13          0x1E    //reserved for future use
#define SerialSpeedReg      0x1F    //selects the speed of the serial UART interface    

//Configuration
#define Reserved20          0x20    //reserved for future use
#define CRCResultRegM       0x21    //show the MSB values of CRC caculation
#define CRCResultRegL       0x22    //show the LSB values of CRC caculation
#define Reserved21          0x23    //reserved for future use
#define ModWidthReg         0x24    //controls the ModWidth setting
#define Reversed22          0x25    //reversed for future use
#define RFCfgReg            0x26    //configures the receiver gain
#define GsNReg              0x27    //selects the conductance of the antenna dricer pins TX1 and TX2 for modulation
#define CWGsPReg            0x28    //defines the conductance of the p-driver output during periods of no modulation
#define ModGsPReg           0x29    //defines the conductance of the p-driver output during periods of modulation
#define TModeReg            0x2A    //define the setting for the internal timer
#define TPrescalerReg       0x2B    //define the setting for the internal timer
#define TReloadRegH         0x2C    //define the 16bit timer reload value
#define TreloadRegL         0x2D    //define the 16bit timer reload value
#define TCounterValRegH     0x2E    //shows the 16bit timer value
#define TCounverValRegL     0x2F    //shows the 16bit timer value

//Test register
#define Reverved30          0x30    //reserved for future use
#define TestSel1Reg         0x31    //general test signal configuration
#define TestSel2Reg         0x32    //general test signal configuration and PRBS control
#define TestPinEnReg        0x33    //enables pin output driver on pins D1 to D7
#define TestPinValueReg     0x34    //defines the values for D1 to D7 when it is used as an I/O bus
#define TestBusReg          0x35    //shows the status of the internal test bus
#define AutoTestReg         0x36    //controls the digital self test
#define VersionReg          0x37    //shows the software version
#define AnalogTestReg       0x38    //comtrols the pins AUX1 and AUX2
#define TestDAC1Reg         0x39    //defines the test value for TestDAC1
#define TestDAC2Reg         0x3A    //defines the test value for TestDAC2
#define TestADCReg          0x3B    //shows the value of ADC I and Q channels
#define Reserved31          0x3C    //reserved for future use
#define Reserved32          0x3D    //reserved for future use

//Function
void MFRC522_write(u_char reg, u_char data);
u_char MFRC522_read(u_char reg);
void SetBitMask(u_char reg, u_char mask);
void ClearBitMask(u_char reg, u_char mask);
void MFRC522_AntennaOn(void);
void AntennaOff(void);
void MFRC522_Reset(void);
void MFRC522_Init(void);
u_char MFRC522_ToCard(u_char command, u_char *sendData, u_char sendLength, u_char *backData, uint *backLength);
u_char MFRC522_Request(u_char reqMode, u_char* TagType);
u_char MFRC522_Anticoll(u_char *serNum);
void MFRC522_CalculateCRC(u_char *pIndata, u_char length, u_char *pOutData);
u_char MFRC522_read_block(u_char blockAdd, u_char *recvData);
u_char MFRC522_write_block(u_char blockAddr, u_char *writeData);
u_char MFRC522_Auth(u_char authMode, u_char BlockAddr, u_char *Sectorkey, u_char *serNum);
u_char MFRC522_SelectTag(u_char *serNum);
void MFRC522_Halt (void);
void MFRC522_StopCrypto1(void);


#endif /* __STM32_MFRC522_H */
