#ifndef EXTI_C
#define EXTI_H
#include "gpio.h"

typedef enum{
	EXTI_RISING_EDGE,
	EXTI_FALLING_EDGE,
	EXTI_BOTH_EDGES
} ExtiEdge;

void extiInit(GPIO_PORT_NAME, uint8_t, ExtiEdge);
void extiClearFlag(uint8_t pin);

#endif
