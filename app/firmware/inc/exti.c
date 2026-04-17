#include "exti.h"
void extiInit(GPIO_PORT_NAME port, uint8_t pin, ExtiEdge edge){
	pinMode(port, pin, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PU_PD, 1);
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	uint8_t index = pin/4;
	uint8_t offset = (pin%4)*4;
	AFIO->EXTICR[index] &= ~(0xF << offset);
	AFIO->EXTICR[index] |= (gpioPortIndex(port) << offset);

	EXTI->IMR |= (1 << pin);
	if(edge == EXTI_RISING_EDGE){
		EXTI->RTSR |= (1 << pin);
	}
	else if(edge == EXTI_FALLING_EDGE){
		EXTI->FTSR |= (1 << pin);
	}
	else if(edge == EXTI_BOTH_EDGES){
		EXTI->RTSR |= (1 << pin);
		EXTI->FTSR |= (1 << pin);
	}

	if(pin <= 4){
		NVIC_EnableIRQ(EXTI0_IRQn+pin);
	}
	else if(pin <= 9){
		NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
	else{
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}


void extiClearFlag(uint8_t pin){
	EXTI->PR |= (1 << pin);
}
