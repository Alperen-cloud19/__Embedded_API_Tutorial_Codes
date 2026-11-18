#include "GPIO.h"

/**
  * @brief 	GPIO_Init, Configures the port and pin
  *
  * @param  GPIOx = GPIO Port Base Address
  *
  * @param 	GPIO_InitTypeDef_t = User Config Structures
  *
  * @retval void
  */

void GPIO_Init(GPIO_TypeDef_t *GPIOx, GPIO_InitTypeDef_t *GPIO_ConfigStruct)
{
	uint32_t position;
	uint32_t fakeValue = 0;
	uint32_t lastValue = 0;

	for(position = 0; position < MAX_GPIO_PINS; position++)
	{
		fakeValue = (0x1 << position);
		lastValue = (uint32_t)(GPIO_ConfigStruct->pinNumber) & fakeValue;

		if(lastValue == fakeValue)
		{
			/* MODE CONFIG */
			uint32_t tempValue = GPIOx->MODER;

			tempValue &= ~(0x3U << (position * 2));
			tempValue |= (GPIO_ConfigStruct->Mode << (position * 2));

			GPIOx->MODER = tempValue;

			if(GPIO_ConfigStruct->Mode == GPIO_MODE_OUTPUT || GPIO_ConfigStruct->Mode == GPIO_MODE_AF)
			{
				/* OUTPUT TYPE CONFIG */
				tempValue = GPIOx->OTYPER;
				tempValue &= ~(0x1U << position);
				tempValue |= (GPIO_ConfigStruct->OType << position);
				GPIOx->OTYPER = tempValue;

				/* OUTPUT SPEED CONFIG */
				tempValue = GPIOx->OSPEEDR;
				tempValue &= ~(0x3U << (position * 2));
				tempValue |= (GPIO_ConfigStruct->Speed << (position * 2));
				GPIOx->OSPEEDR = tempValue;
			}

			/* PUSH/PULL CONFIG */
			tempValue = GPIOx->PUPDR;
			tempValue &= ~(0x3U << (position * 2));
			tempValue |= (GPIO_ConfigStruct->PuPd << (position * 2));
			GPIOx->PUPDR = tempValue;

			if(GPIO_ConfigStruct->Mode == GPIO_MODE_AF)
			{
				tempValue = GPIOx->AFR[position >> 3U];
				tempValue &= ~((0xFU) << ((position & 0x7U) * 4U));
				tempValue |= (GPIO_ConfigStruct->Alternate << ((position & 0x7U) * 4U));
				GPIOx->AFR[position >> 3U] = tempValue;
			}
		}

	}
}



/**
  * @brief 	GPIO_Write_Pin, makes pin High or Low
  *
  * @param  GPIOx = GPIO Port Base Address
  *
  * @param  pinNumber = GPIO Pin Numbers 0 - 15
  *
  * @param 	pinState = GPIO_Pin_Set OR GPIO_Pin_Reset
  *
  * @retval void
  */

void GPIO_WritePin(GPIO_TypeDef_t *GPIOx, uint16_t pinNumber, GPIO_PinState_t pinState)
{
	if(pinState == GPIO_Pin_Set)
	{
		GPIOx->BSRR = pinNumber;
	}
	else
	{
		GPIOx->BSRR = (pinNumber << 16U);
	}
}

/**
  * @brief 	GPIO_ReadPin, reads the pin of GPIOx Port
  *
  * @param  GPIOx = GPIO Port Base Address
  *
  * @param  pinNumber = GPIO Pin Numbers 0 - 15
  *
  * @retval GPIO_PinState_t
  */

GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef_t *GPIOx, uint16_t pinNumber)		//0000 1001 1100 0010
{
	GPIO_PinState_t bitStatus = GPIO_Pin_Reset;								//0000 0000 1000 0000

	if((GPIOx->IDR & pinNumber) != GPIO_Pin_Reset)							//0000 0000 1000 0000
	{
		bitStatus = GPIO_Pin_Set;
	}

	return bitStatus;
}

/**
  * @brief 	GPIO_LockPin, locks the pin of GPIOx Port
  *
  * @param  GPIOx = GPIO Port Base Address
  *
  * @param  pinNumber = GPIO Pin Numbers 0 - 15
  *
  * @retval void
  */

void GPIO_LockPin(GPIO_TypeDef_t *GPIOx, uint16_t pinNumber)
{
	uint32_t tempValue = (0x1U << 16U) | pinNumber;		//1 0000 0000 1101 0000

	GPIOx->LCKR = tempValue;							//LCKR[16] = '1' + LCKR[15:0] = DATA
	GPIOx->LCKR = pinNumber;							//LCKR[16] = '0' + LCKR[15:0] = DATA
	GPIOx->LCKR = tempValue;							//LCKR[16] = '1' + LCKR[15:0] = DATA

	tempValue = GPIOx->LCKR;							//Read Lock Register(GPIO->LCKR)



}

/**
  * @brief 	GPIO_TogglePin, toggles the pin of GPIOx Port
  *
  * @param  GPIOx = GPIO Port Base Address
  *
  * @param  pinNumber = GPIO Pin Numbers 0 - 15
  *
  * @retval void
  */

void GPIO_TogglePin(GPIO_TypeDef_t *GPIOx, uint16_t pinNumber)
{
	uint32_t tempODRRegister = GPIOx->ODR;
																							//0000 0000 0011 0100
	GPIOx->BSRR = ((tempODRRegister & pinNumber) << 16U) | (~(tempODRRegister) & pinNumber);//0000 0000 0101 0000
																							//0000 0000 0001 0000 set->reset

																	//~(0000 0000 0011 0100)->1111 1111 1100 1011
																							//0000 0000 0101 0000
																							//0000 0000 0100 0000 reset->set

}
