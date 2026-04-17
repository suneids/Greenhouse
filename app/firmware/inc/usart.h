#ifndef USART_H
#define USART_H
#include <stdint.h>
#include "mcu_config.h"
#define USART_BUFFER_SIZE 257
#define USART_BUFFER_MASK (USART_BUFFER_SIZE - 1)

typedef enum {
	USART_1 = 0,
	USART_2,
	USART_COUNT
}USART_PORT_NAME;

extern USART_TypeDef *USARTs[2];
extern volatile uint16_t rx_heads[2], tx_heads[2];
extern volatile uint16_t rx_tails[2], tx_tails[2];
extern volatile char usartRXBuffers[2][USART_BUFFER_SIZE], usartTXBuffers[2][USART_BUFFER_SIZE];

void echo(USART_PORT_NAME);
void usartInit(USART_PORT_NAME, uint32_t);
uint16_t usartAvailable(USART_PORT_NAME);
void usartWriteByte(USART_PORT_NAME, char);
void usartWriteLine(USART_PORT_NAME, const char*);

char usartReadByte(USART_PORT_NAME port_id);
void usartReadBytes(USART_PORT_NAME port_id, char *, uint32_t);

void USART_IRQHandler_Generic(USART_PORT_NAME);
uint32_t usartDiv(uint32_t F_CPU, uint32_t baud_rate);
#endif
