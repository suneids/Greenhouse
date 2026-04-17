#ifndef TIM_H
#define TIM_H
#include "mcu_config.h"
extern volatile uint32_t msTicks;
typedef enum {
	TIM_PORT_1,
	TIM_PORT_2,
	TIM_PORT_3//,
	//TIM_PORT_4
}TIM_PORT_NAME;

void sysTickInit(void);
TIM_TypeDef *portToTIM(TIM_PORT_NAME);
uint32_t millis(void);
void timerInit(TIM_PORT_NAME, uint32_t, uint32_t);
void debounceTimerInit(TIM_PORT_NAME, uint32_t, uint32_t);


#endif
