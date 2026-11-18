#include "stm32f407xx.h"

USART_HandleTypeDef_t USART_Handle;
I2C_HandleTypeDef_t I2C_Handle;

static void GPIO_Config(void);
static void USART_Config(void);

void USART2_IRQHandler()
{
	USART_InterruptHandler(&USART_Handle);
}

int main(void)
{
	char msgToSend[] = "Hey bro How are you ?\n";
	char receiveData[30];


	GPIO_Config();
	USART_Config();

	//USART_ReceiveData(&USART_Handle, (uint8_t*)msgToSend, 20);
	//USART_TransmitData(&USART_Handle, (uint8_t*)msgToSend, strlen(msgToSend));	// Stringlerin veya char dizilerinin boyutunu
																					// strlen -> fonksiyonu ile buluruz.
	USART_TransmitData_IT(&USART_Handle, (uint8_t* )msgToSend, strlen(msgToSend));
	USART_ReceiveData_IT(&USART_Handle, (uint8_t*)receiveData, 20);

	I2C_PeriphCmd(I2C_Handle.Instance, ENABLE);		// I2C bu sekilde ENABLE olur.

	while(1);
}

static void USART_Config(void)
{
	RCC_USART2_CLK_ENABLE();

	USART_Handle.Instance = USART2;
	USART_Handle.Init.BaudRate = 115200;
	USART_Handle.Init.HardwareFlowControl = USART_HFC_DISABLE;
	USART_Handle.Init.Mode = USART_MODE_TX_RX;
	USART_Handle.Init.OverSampling = USART_OVERSAMPLE_BY16;
	USART_Handle.Init.Parity = USART_PARITY_DISABLE;
	USART_Handle.Init.StopBits = USART_STOP_BIT_1;
	USART_Handle.Init.WordLength = USART_WORDLENGTH_8BITS;

	USART_Init(&USART_Handle);
	NVIC_EnableInterrupt(USART2_IRQNumber);
	USART_PeriphCmd(&USART_Handle, ENABLE);
}

static void GPIO_Config(void)
{
	GPIO_InitTypeDef_t GPIO_InitStruct = { 0 };		// Local bir degisken oldugu icin 0 atanir.
	RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.pinNumber = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.OType = GPIO_OTYPE_PP;
	GPIO_InitStruct.PuPd = GPIO_PUPD_NOPULL;
	GPIO_InitStruct.Speed = GPIO_OSPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7;

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}
