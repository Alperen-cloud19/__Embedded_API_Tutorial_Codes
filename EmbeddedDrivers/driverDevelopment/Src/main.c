#include "stm32f407xx.h"

static void GPIO_LedConfig();
static void GPIO_ButtonInterruptConfig();
static void SPI_Config();
static void SPI_GPIO_Config();

SPI_HandleTypeDef_t SPI_Handle;
USART_HandleTypeDef_t USART_Handle;


void delay(volatile unsigned long counter)
{
	while(counter > 0)
	{
		counter--;
	}
}

void EXTI0_IRQHandler()
{
	static char msgToSend[] = "Hello are you there!\n";

	if(EXTI->PR & 0x1)
	{
		EXTI->PR |= (0x1U << 0U);

		SPI_ReceiveData_IT(&SPI_Handle, (uint8_t*)msgToSend, strlen(msgToSend));

		/*if(GPIOA->IDR & 0x01)
		{
			GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_Pin_Set);
		}
		else
		{
			GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_Pin_Reset);
		}*/
	}
}

void SPI1_IRQHandler()
{
	SPI_InterruptHandler(&SPI_Handle);
}

int main(void)
{
	GPIO_LedConfig();

	GPIO_ButtonInterruptConfig();

	SPI_GPIO_Config();

	SPI_Config();

	for(;;)
	{
	}
}

static void GPIO_LedConfig()
{
	GPIO_InitTypeDef_t GPIO_InitStruct = { 0 };

	RCC_GPIOD_CLK_ENABLE();				//GPIOD CLOCK IS ACTIVE
	RCC_GPIOC_CLK_ENABLE();				//GPIOC CLOCK IS ACTIVE
	RCC_GPIOA_CLK_ENABLE();				//GPIOA CLOCK IS ACTIVE

	GPIO_InitStruct.pinNumber = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OType = GPIO_OTYPE_PP;
	GPIO_InitStruct.PuPd = GPIO_PUPD_NOPULL;
	GPIO_InitStruct.Speed = GPIO_OSPEED_LOW;

	GPIO_Init(GPIOD, &GPIO_InitStruct);	//CONFIG

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));			//This func. clear our GPIO_LedStruct.

	GPIO_InitStruct.pinNumber =GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.PuPd = GPIO_PUPD_PULLDOWN;

	GPIO_Init(GPIOA, &GPIO_InitStruct);

}

static void GPIO_ButtonInterruptConfig()
{
	EXTI_InitTypeDef_t EXTI_InitStruct = { 0 };

	RCC_SYSCFG_CLK_ENABLE();
	EXTI_LineConfig(EXTI_PortSource_GPIOA, EXTI_LineSource_0);

	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_LineNumber = EXTI_LineSource_0;
	EXTI_InitStruct.EXTI_Mode = EXTI_MODE_Interrupt;
	EXTI_InitStruct.TriggerSelection = EXTI_TriggerMode_Rising;

	EXTI_Init(&EXTI_InitStruct);

	NVIC_EnableInterrupt(EXTI0_IRQNumber);

}

static void SPI_Config()
{

	RCC_SPI1_CLK_ENABLE();			  //SPI1 CLOCK IS ACTIVE

	SPI_Handle.Instance = SPI1;
	SPI_Handle.Init.BaudRate = SPI_BAUDRATE_DIV8;		//16MHz / 8 = 2MHz
	SPI_Handle.Init.BusConfig = SPI_BUS_FULLDUBLEX;
	SPI_Handle.Init.CPHA = SPI_CPHA_FIRST;
	SPI_Handle.Init.CPOL = SPI_CPOL_LOW;
	SPI_Handle.Init.DFF_Format = SPI_DFF_8BITS;
	SPI_Handle.Init.FrameFormat = SPI_FRAMEFORMAT_MSB;
	SPI_Handle.Init.Mode = SPI_MODE_MASTER;
	SPI_Handle.Init.SSM_Cmd = SPI_SSM_ENABLE;

	SPI_Init(&SPI_Handle);

	NVIC_EnableInterrupt(SPI1_IRQNumber);

	SPI_PeriphCmd(&SPI_Handle, ENABLE);
}

static void SPI_GPIO_Config()
{
	GPIO_InitTypeDef_t GPIO_InitStruct = { 0 };

	GPIO_InitStruct.pinNumber = GPIO_PIN_5 | GPIO_PIN_7;		//PA5 --> SCK, PA7 --> MOSI
	GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.OType = GPIO_OTYPE_PP;
	GPIO_InitStruct.PuPd = GPIO_PUPD_NOPULL;
	GPIO_InitStruct.Speed = GPIO_OSPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5;

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}
