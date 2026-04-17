#include "gpio.h"

void enableGPIOClock(GPIO_PORT_NAME gpio_port){
	switch(gpio_port){
		case GPIO_PORT_A:{
			RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
			break;
		}
		case GPIO_PORT_B:{
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
			break;
		}
		case GPIO_PORT_C:{
			RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
			break;
		}

	}
}

GPIO_TypeDef* portToGPIO(GPIO_PORT_NAME gpio_port){
	switch(gpio_port){
		case GPIO_PORT_A:{
			return GPIOA;
		}
		case GPIO_PORT_B:{
			return GPIOB;
		}
		case GPIO_PORT_C:{
			return GPIOC;
		}
	}
	return 0;
}

uint8_t gpioPortIndex(GPIO_PORT_NAME port){ // @suppress("No return")
	switch(port){
		case GPIO_PORT_A:{
			return 0;
		}
		case GPIO_PORT_B:{
			return 1;
		}
		case GPIO_PORT_C:{
			return 2;
		}
	}
	return 0;
}


void pinMode(GPIO_PORT_NAME gpio_port, uint8_t pin, uint8_t mode, uint8_t cnf, uint8_t pull){
	GPIO_TypeDef* GPIOx = portToGPIO(gpio_port);
	uint8_t shift = 4*(pin < 8? pin: pin - 8);
	if(pin < 8){
		GPIOx->CRL &= ~( 0b1111 << shift);
		GPIOx->CRL |= ((cnf << 2) | mode) << shift;
	}
	else{
		GPIOx->CRH &= ~( 0b1111 << shift);
		GPIOx->CRH |= ((cnf << 2) | mode) << shift;
	}
	if(mode == GPIO_MODE_INPUT && cnf == GPIO_CNF_INPUT_PU_PD){
		if(pull){
			GPIOx->ODR |= 1 << pin;
		}
		else{
			GPIOx->ODR &= ~(1 << pin);
		}
	}
}

void digitalWrite(GPIO_PORT_NAME gpio_port, uint8_t pin, uint8_t value){
	GPIO_TypeDef* GPIOx = portToGPIO(gpio_port);
	if(value == 1){
		GPIOx->BSRR = 1 << pin;
	}
	else{
		GPIOx->BRR = 1 << pin;
	}
}


uint8_t digitalRead(GPIO_PORT_NAME gpio_port, uint8_t pin){
	GPIO_TypeDef* GPIOx = portToGPIO(gpio_port);
	return (GPIOx->IDR & (1 << pin))? 1 : 0;
}
