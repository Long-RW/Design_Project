
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_MFRC522.c
  * @brief   MFRC522_RC522 Connection Routines.
  ******************************************************************************
  * @Connection
  * Connectivity: SPI1
  * SDA/CS  : GPIOB, PIN_0
  * SCK     : GPIOA, PIN_5
  * MOSI    : GPIOA, PIN_6
  * MISO    : GPIOA, PIN_7
  * VCC     : 3.3V
  * GND     : GND
  * IRQ     : Float                       
  * RESET   : 3.3V
  ******************************************************************************
  */
/* USER CODE END Header */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "STM32_RFID.h"
#include "main.h"
/******************************************************************************/
/*                   MFRC522_RC522 Communication and Control Function            */
/******************************************************************************/

extern SPI_HandleTypeDef hspi1;
void MFRC522_write(u_char reg, u_char data)
{		
		u_char addr_bits = (((reg<<1) & 0x7E));
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &addr_bits, 1, 500);
    HAL_SPI_Transmit(&hspi1, &data, 1, 500);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

/**
  * @brief  MFRC522_read
  * @note   This function READ a byte into MFRC522 register 
  * @param  reg is MFRC522 regsister
  * @retval return byte
  *
  */
u_char MFRC522_read(u_char reg)
{
  u_char rx_bits;
  u_char addr_bits = (((reg << 1) & 0x7E) | 0x80);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &addr_bits, 1, 500);
  HAL_SPI_Receive(&hspi1, &rx_bits, 1, 500);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  return (u_char) rx_bits;
}
/**
  * @brief  SetBitMask
  * @note   This function Set bit in a MFRC522 register 
  * @param  reg is MFRC522 regsister, mask is the mask of bit need to be set
  * @retval None
  *
  */
void SetBitMask(u_char reg, u_char mask)
{
    u_char temp = 0x00;
    temp = MFRC522_read(reg);
    MFRC522_write(reg, temp | mask);
}

/**
  * @brief  ClearBitMask
  * @note   This function Clear  bit in a MFRC522 register 
  * @param  reg is MFRC522 regsister, mask is the mask of bit need to be cleared
  * @retval None
  *
  */
void ClearBitMask(u_char reg, u_char mask)
{
  u_char temp = 0x00;
  temp = MFRC522_read(reg);
  MFRC522_write(reg, temp & (~mask));
}

/**
  * @brief  MFRC522_AntennaOn
  * @note   This function TurnOn Antenna of MFRC522  
  * @param  None
  * @retval None
  *
  */
void MFRC522_AntennaOn(void)
{
  SetBitMask(TxControlReg, 0x03);
}

/**
  * @brief  MFRC522_AntennaOff
  * @note   This function TurnOff Antenna of MFRC522
  * @param  None
  * @retval None
  *
  */
void AntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03); 
}

 /**
  * @brief  MFRC522_Reset
  * @note   This function Reset MFRC522
  * @param  None
  * @retval None
  *
  */
void MFRC522_Reset(void)
{
  MFRC522_write(CommandReg, PCD_SOFTRESET);
}

/**
  * @brief  MFRC522_Init
  * @note   This function is initialization of MFRC522
  * @param  None
  * @retval None
  *
  */
void MFRC522_Init(void)
{
	u_char checkReg;
  MFRC522_Reset();
  
  //Setup Delay Timer
  
  // Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
  MFRC522_write(TModeReg, 0x80);         // Tauto=1; f(Timer) = 6.78MHz/TPreScaler
  MFRC522_write(TPrescalerReg, 0xA9);    // TModeReg[3..0] + TPrescalerReg
  MFRC522_write(TreloadRegL, 0x03);        //TReloadVal = 30
  MFRC522_write(TReloadRegH, 0xE8);         //Td = 6ms
  MFRC522_write(TxASKReg, 0x40);         //Forces a 100% ASK modulation independent of the ModGsPReg register setting
  MFRC522_write(ModeReg, 0x3D);          //CRC gia tri ban dau 0x6363
  MFRC522_AntennaOn();                   //Antenna On    
    
}

/**
  * @brief  MFRC522_ToCard
  * @note   RC522 and ISO14443 card communication
  * @param  command - MF522 command word,
  *			    sendData--RC522 sent to the card by the data
  *			    sendLen--Length of data sent
  *			    backData--Received the card returns data,
  *			    backLen--Return data bit length
  *         *
  * @retval communication status
  *
*/
u_char MFRC522_ToCard(u_char command, u_char *sendData, u_char sendLength, 
                                    u_char *backData, uint *backLength)
{
  u_char status = MI_ERR;
  u_char irqEn = 0x00;
  u_char waitIRq = 0x00;
  u_char lastBits;
  u_char n;
  uint i = 0;

  switch (command)
  {
  case PCD_AUTHENT:         //Performs the MIFARE standard authentication as a reader
    irqEn   = 0x12;
    waitIRq = 0x10;
    break;
  case PCD_TRANCEIVE:      //Transmit Data to FIFO buffer
    irqEn = 0x77;
    waitIRq = 0x30;
    break;  
  default:
    break;
  }

  MFRC522_write(ComIEnReg, irqEn|0x80);    //Interrupt Request
  ClearBitMask(ComIEnReg, 0x80);        //Clear all of Interrupt Request Bits
  SetBitMask(FIFOLevelReg, 0x80);       //FlushBuffer = 1, Initial FIFO Buffer

  MFRC522_write(CommandReg, PCD_IDLE);     //No action; cancel the current command

  //Write Data into FIFO Data Register
  for(i = 0; i < sendLength; i++)
  {
    MFRC522_write(FIFODataReg, sendData[i]);
  }

  //Running ...
  MFRC522_write(CommandReg, command);
  if(command == PCD_TRANCEIVE)
  {
    SetBitMask(BitFramingReg, 0x80);    //StatSend=1, transmission of data starts
  }

  //Waiting for Transmision completed
	i = 2000;
	while((i!=0) && (!(n&0x01)) && (!(n&waitIRq)))
  {
    n = MFRC522_read(ComIEnReg);
    i--;
  }
	HAL_Delay(4);  //wait until i = 0 and ComIEnReg = 0x31 = 0011 0001 => RxIEn, IdleIEn and TimerIEn bits set
  ClearBitMask(BitFramingReg, 0x80);      //StartSend = 0
  
  if(i != 0)
  {
    if(!(MFRC522_read(ErrorReg) & 0x1B))    //Bufferovfl Collerr CRCErr ProtocolErr
    {
      status = MI_OK;

      if(n & irqEn & 0x01)
      {
        status = MI_NOTAGERR;
      }
      if(command == PCD_TRANCEIVE)
      {
        n = MFRC522_read(FIFOLevelReg);
        lastBits = MFRC522_read(ControlReg) & 0x07;

        if(lastBits)
        {
          *backLength = (n - 1) * 8 + lastBits;
        }
        else
        {
          *backLength = n * 8;
        }
        
        if(n == 0)
        {
          n = 1;
        }

        if(n > MAX_LENGTH)
        {
          n = MAX_LENGTH;
        }
        //Doc FIFO trong cac du lieu nhan duoc
        for(i = 0; i < n; i++)
        {
          backData[i] = MFRC522_read(FIFODataReg);
        }
      }
      else
      {
        status = MI_ERR;
      }
    }
  }
  
  return status;
}

/**
  * @brief  MFRC522_Request
  * @note   Find cards, read the card type number
  * @param  reqMode - find cards way
 *          TagType - Return Card Type
 *          0x4400 = Mifare_UltraLight
 *          0x0400 = Mifare_One(S50)
 *          0x0200 = Mifare_One(S70)
 *          0x0800 = Mifare_Pro(X)
 *          0x4403 = Mifare_DESFire
 * Return value: the successful return MI_OK
  *         *
  * @retval the successful return MI_OK
  */
u_char MFRC522_Request(u_char reqMode, u_char* TagType)
{
  u_char status;
  uint backBits;                 //Data bits Received

  MFRC522_write(BitFramingReg, 0x07);   //TxLastBits = BitFramingReg[2..0]
  
  TagType[0] = reqMode;
  status = MFRC522_ToCard(PCD_TRANCEIVE, TagType, 1, TagType, &backBits);

  if((status != MI_OK) || (backBits != 0x10))
  {
    status = MI_ERR;
  }

  return status;
}

/**
  * @brief  MFRC522_Request
  * @note   Anti-collision detection, reading selected card serial number card
  * @param  serNum - returns 4 bytes card serial number, the first 5 bytes for the checksum byte
  *         *
  * @retval the successful return MI_OK
  *
  */
 u_char MFRC522_Anticoll(u_char *serNum)
 {
    u_char status;
    u_char i;
    u_char serNumCheck = 0 ;
    uint unLen;

    MFRC522_write(BitFramingReg, 0x00);    //TxLastBits = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANCEIVE, serNum, 2, serNum, &unLen);
    if(status == MI_OK)
    {
      //Check Serial Number of Card
      for(i = 0; i < 4; i++)
      {
        serNumCheck ^= serNum[i];
      }
      if(serNumCheck != serNum[i])
      {
        status = MI_ERR;
      }
    }

    return status;
 }

/**
  * @brief  MFRC522_CalculateCRC
  * @note   CRC calculation with MF522
  * @param  pIndata - To read the CRC data, len - the data length, pOutData - CRC calculation results
  *         *
  * @retval 
  *
  */
void MFRC522_CalculateCRC(u_char *pIndata, u_char length, u_char *pOutData)
{
  u_char i, n;
  ClearBitMask(DivIrqReg, 0x04);        //CRC = 0;
  SetBitMask(FIFOLevelReg, 0x80);       //FIFO pointer

  //Write Data into FIFO
  for(i = 0; i < length; i++)
  {
    MFRC522_write(FIFODataReg, *(pIndata+i));
  }
  MFRC522_write(CommandReg, PCD_CALCCRC);

  //Wait CRC calculation is complte
  i = 0xFF;
  do
  {
    n = MFRC522_read(DivIrqReg);
    i--;
  } while ((i != 1) && !(n&0x04));        //CRCIrq = 1
  
  //Read CRC calculation result
  pOutData[0] = MFRC522_read(CRCResultRegL);
  pOutData[1] = MFRC522_read(CRCResultRegM);
}

/**
  * @brief  MFRC522_read_block
  * @note   Read block data
  * @param  blockAddr - block address; recvData - read block data
  *         *
  * @retval the successful return MI_OK
  *
  */
 u_char MFRC522_read_block(u_char blockAdd, u_char *recvData)
 {
    u_char status;
    uint unLength;
    recvData[0] = PICC_READ;
    recvData[1] = blockAdd;
    MFRC522_CalculateCRC(recvData, 2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANCEIVE, recvData, 4,recvData, &unLength);

    if((status != MI_OK) || (unLength != 0x90))
    {
      status = MI_ERR;
    }

    return status;
 }

 /**
  * @brief  MFRC522_read_block
  * @note   Read block data
  * @param  blockAddr - block address; recvData - read block data
  *         *
  * @retval the successful return MI_OK
  *
  */
u_char MFRC522_write_block(u_char blockAddr, u_char *writeData)
{
  u_char status;
  uint recvBits;
  u_char i;
  u_char buff[18];

  MFRC522_CalculateCRC(buff, 2, &buff[2]);
  status = MFRC522_ToCard(PCD_TRANCEIVE, buff, 4, buff, &recvBits);

  if((status != MI_OK))
  {
    status = MI_ERR;
  }

  if(status == MI_OK)
  {
    for(i = 0; i < 16; i++)
    {
      buff[i] = *(writeData+i);
    }
    MFRC522_CalculateCRC(buff, 16, &buff[16]);
    status = MFRC522_ToCard(PCD_TRANCEIVE, buff, 18, buff, &recvBits);

    if((status != MI_OK))
    {
      status = MI_ERR;
    }
  }

  return status;
}

 /**
  * @brief  MFRC522_Auth
  * @note   Verify card password
  * @param authMode - Password Authentication Mode
          0x60 = A key authentication
          0x61 = Authentication Key B
          BlockAddr--Block address
          Sectorkey--Sector password
          serNum--Card serial number, 4-byte
  * @retval the successful return MI_OK
  *
  */
u_char MFRC522_Auth(u_char authMode, u_char BlockAddr, u_char *Sectorkey, u_char *serNum)
{
  u_char status;
  uint recvBits;
  u_char i;
  u_char buff[12];

  //Verify the command block address + sector + password + card serial number
  buff[0] = authMode;
  buff[1] = BlockAddr;

  for(i = 0; i < 6; i++)
  {
    buff[i+2] = *(Sectorkey+i);
  }

  for(i = 0; i < 4; i++)
  {
    buff[i+8] = *(serNum+i);
  }

  status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

  if((status != MI_OK) || (!(MFRC522_read(Status2Reg) & 0x08)))
  {
    status = MI_ERR;
  }

  return status;
}

/**
  * @brief  MFRC522_SelectTag
  * @note   election card, read the card memory capacity
  * @param  serNum - Incoming card serial number
  * @retval the successful return MI_OK
  *
  */
u_char MFRC522_SelectTag(u_char *serNum)
{
  u_char i;
  u_char status;
  u_char size;
  uint recvBits;
  u_char buffer[9];

  buffer[0] = PICC_SELECTTAG;
  buffer[1] = 0x70;

  for(i = 0; i < 5; i++)
  {
    buffer[i+2] = *(serNum+i);
  }

  MFRC522_CalculateCRC(buffer, 7, &buffer[7]);
  status = MFRC522_ToCard(PCD_TRANCEIVE, buffer, 9, buffer, &recvBits);

  if((status == MI_OK) && (recvBits == 0x18))
  {
    size = buffer[0];
  }
  else
  {
    size = 0;
  }

  return size;
}

/**
  * @brief  MFRC522_Halt
  * @note   Command card into hibernation
  * @param  None
  * @retval None
  *
  */
void MFRC522_Halt (void)
{
  u_char status;
  uint unLength;
  u_char buff[4];

  buff[0] = PICC_HALT;
  buff[1] = 0;
  MFRC522_CalculateCRC(buff, 2, &buff[2]);

  status = MFRC522_ToCard(PCD_TRANCEIVE, buff, 4, buff, &unLength); 
	
}

void MFRC522_StopCrypto1(void)
{
  ClearBitMask(Status2Reg, 0x08);
}
