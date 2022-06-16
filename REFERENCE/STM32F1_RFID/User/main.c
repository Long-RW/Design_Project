/** 
  ******************************************************************************
	RFID - Connected with STM32F103
	* VCC 			- 	3.3V
	* RST 			-	 	3.3V
	* GND 			- 	GND
	*	IRQ 			- 	NC
	*	MISO 			-		PB12
	*	MOSI			-		PB13			
	*	SCK (CLK) - 	PB15
	*	SDA (CS)	- 	PB14
	
	Keypad - Connected with STM32F103
	* A0 : A7 

	USART1
	* TX - A9 (RX of STM32F1xx)
	* RX - A10 (TX of STM32F1xx)
	
	Output_Pin to control your's device
	* PC13 - Connected to device 
	
	Note: "STM32F10X_M, USE_STDPERIPH_DRIVER"
	******************************************************************************
	**/ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f1_rc522.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f1_delay.h"
#include "usart.h"
//#include "keypad.h"
#include "stdio.h"
#include "string.h"



//////////////////////////////////////
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/////////////////////////////////////

PUTCHAR_PROTOTYPE
{
/* Place your implementation of fputc here */
/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);
/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;
}

uchar i,tmp;
uchar status;
uchar str[MAX_LEN]; // Max_LEN = 16

uchar serNum[5];
char password[16]="123456"; //Max lenght of password is 16 charaters
char keypass[16];
int cnt=0;
uint8_t key;
uchar check,check2;

uchar Key_Card[5]  = {0x16, 0xe7, 0x23, 0xa3, 0x71};
uchar Key_Card2[5] = {0xc4, 0x24, 0x99, 0x22, 0x5b};
											 

/* Private function prototypes -----------------------------------------------*/
void GPIO_Config(void);
void EXT_Config(void);
void Output_Config(void);
void CheckCard(uchar data[5]); 
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t CheckPass(char *Str1, char *Str2);
void EnterPassWord(void);
													 
															 
/* Private functions ---------------------------------------------------------*/
int main(void)
{    
	u8 i;	
	Delay_Init();
	UART1_Config();		
  MFRC522_Init();	
	//KeypadInit();
	Output_Config();
	GPIOC->BSRR = GPIO_Pin_13;
	Delay_ms(1000);
	GPIOC->BRR = GPIO_Pin_13;
	printf("Welcome to RFID...\r\n");
	Delay_ms(500);
	
	while(1)
	{
		printf("Waiting your Card! \r\n");
				
		status = MFRC522_Request(PICC_REQIDL, str);	
		if (status == MI_OK)
		{
			printf("Find out a card: %x, %x\r\n",str[0],str[1]);
		}

		//Chong va cham the, tra ve 5 byte ma the
		status = MFRC522_Anticoll(str);
		memcpy(serNum, str, 5);
		if (status == MI_OK)
		{
			printf("Your card's number are: %x, %x, %x, %x, %x \r\n",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
			for(i=0;i<5;i++)
			{
				if(Key_Card[i]!=serNum[i]) check = 0;
				else check = 1;
				if(Key_Card2[i]!=serNum[i]) check2 = 0;
				else check2 = 1;
			}
		}
		
//		key = KeypadGetKey();
//		if(key != KEYPAD_NO_PRESSED)
//		{
//			printf("Key: %c \r\n",key);
//			Delay_ms(25);
//			if(key == '#')
//			{
//				EnterPassWord();
//			}
//			while(key!= KEYPAD_NO_PRESSED) {key = KeypadGetKey();}
//		}		
	
		if(check == 1||check2 ==1)
		{
			check = 0;
			check2 = 0;
			GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
			printf("The Card's number is Ok!\r\n");
			Delay_ms(1000);
		}
		Delay_ms(50);	
	}
}


void Output_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOB Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PB1 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  GPIOx->ODR ^= GPIO_Pin;
}

uint8_t CheckPass(char *Str1, char *Str2)
{
	if(strlen(Str1)==strlen(Str2))
	{
		if(strstr(Str1,Str2)) return 1;
		else return 0;
	}
	else return 0;
}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
