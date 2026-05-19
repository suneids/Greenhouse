#include "../../../inc/gpio.h"
#if defined(STM32F103)


static void GPIO_ClockEnable(Pin_t pin){
	if(pin.port == GPIOA){
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}
	else if(pin.port == GPIOB){
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	}
	else if(pin.port == GPIOC){
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	(void)pin.port->CRL;
}


uint8_t GPIO_GetPortIndex(GPIO_TypeDef *port){
	if(port == GPIOA){
		return 0;
	}
	else if(port == GPIOB){
		return 1;
	}
	else if(port == GPIOC){
		return 2;
	}
	return 0;
}


void GPIO_PinMode(GPIO_Pin_t pin, uint8_t mode, uint8_t cnf, uint8_t pull){
	GPIO_ClockEnable(pin);
	GPIO_TypeDef* GPIOx = pin.port;
	uint8_t pin_number = pin.number;
	uint8_t shift = 4*(pin_number < 8? pin_number: pin_number - 8);
	if(pin_number < 8){
		GPIOx->CRL &= ~( 0xFUL << shift);
		GPIOx->CRL |= ((cnf << 2) | mode) << shift;
	}
	else{
		GPIOx->CRH &= ~( 0xFUL << shift);
		GPIOx->CRH |= ((cnf << 2) | mode) << shift;
	}
	if(mode == GPIO_MODE_INPUT && cnf == GPIO_CNF_INPUT_PU_PD){
		if(pull){
			GPIOx->ODR |= 1 << pin_number;
		}
		else{
			GPIOx->ODR &= ~(1 << pin_number);
		}
	}
}



void GPIO_PinModeMulti(Pin_t *pins, size_t pins_number, uint8_t mode, uint8_t cnf, uint8_t pull){
	for(uint8_t i = 0; i < pins_number; i++)
		GPIO_PinMode(pins[i], mode, cnf, pull);
}


void GPIO_PinToggle(Pin_t pin){
	GPIO_TypeDef *GPIOx = pin.port;
	uint8_t pin_number = pin.number;
	if(GPIOx->ODR & (1U << pin_number)){
		GPIOx->BSRR = (1U << (pin_number + 16));
	}
	else{
		GPIOx->BSRR = 1U << pin_number;
	}
}


void GPIO_DigitalWrite(Pin_t pin, uint8_t value){
	GPIO_TypeDef* GPIOx = pin.port;
	uint8_t pin_number = pin.number;
	if(value == 1){
		GPIOx->BSRR = 1 << pin_number;
	}
	else{
		GPIOx->BSRR = 1 << (pin_number+16);
	}
}


uint8_t GPIO_DigitalRead(Pin_t pin){
	GPIO_TypeDef* GPIOx = pin.port;
	uint8_t pin_number = pin.number;
	return (GPIOx->IDR & (1 << pin_number))? 1 : 0;
}

#endif
