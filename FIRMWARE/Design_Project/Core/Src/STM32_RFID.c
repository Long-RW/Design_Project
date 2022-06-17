/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_RFID.c
  * @brief   RFID_RC522 Connection Routines.
  ******************************************************************************
  * @attention
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
#include "STM32_RFID.h"
#include "main.h"

/******************************************************************************/
/*                   RFID_RC522 Communication and Control Function            */
/******************************************************************************/
extern SPI_HandleTypeDef hspi1;

void RFID_write(uint8_t reg, uint8_t data)
{
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, (reg<<1) & 0x7E, 1, 100);
    HAL_SPI_Transmit(&hspi1, data, 1, 100);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

uint8_t RFID_read(uint8_t reg)
{
    uint8_t data;
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, ((reg << 1) & 0x7E) | 0x80, 1, 100);
    data = HAL_SPI_Transmit(&hspi1, data, 1, 100);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    return data;

}

void SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = 0x00;
    temp = RFID_read(reg);
    RFID_write(reg, temp | mask);
}

void ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = 0x00;
    temp = RFID_read(reg);
    RFID_write(reg, temp & ~mask);
}
void RFID_Init(void)
{
    
}