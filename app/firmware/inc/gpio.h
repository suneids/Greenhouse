#ifndef GPIO_H
#define GPIO_H
#include "mcu_config.h"

typedef enum{
	GPIO_PORT_A,
	GPIO_PORT_B,
	GPIO_PORT_C
}GPIO_PORT_NAME;

#define GPIO_MODE_INPUT 0b00
#define GPIO_MODE_OUTPUT_10MHz 0b01
#define GPIO_MODE_OUTPUT_2MHz 0b10
#define GPIO_MODE_OUTPUT_50MHz 0b11

#define GPIO_CNF_ANALOG 0b00
#define GPIO_CNF_FLOATING 0b01
#define GPIO_CNF_INPUT_PU_PD 0b10


#define GPIO_CNF_PUSH_PULL 0b00
#define GPIO_CNF_OPEN_DRAIN 0b01
#define GPIO_CNF_PUSH_PULL_ALT 0b10
#define GPIO_CNF_OPEN_DRAIN_ALT 0b11

void enableGPIOClock(GPIO_PORT_NAME);
GPIO_TypeDef* portToGPIO(GPIO_PORT_NAME);
uint8_t gpioPortIndex(GPIO_PORT_NAME);
void pinMode(GPIO_PORT_NAME gpio_port, uint8_t pin, uint8_t mode, uint8_t cnf, uint8_t pull);
void digitalWrite(GPIO_PORT_NAME, uint8_t, uint8_t);
uint8_t digitalRead(GPIO_PORT_NAME, uint8_t);

#endif
