#include "tim.h"
volatile uint32_t msTicks = 0;


void sysTickInit(void){
	SysTick_Config(SystemCoreClock / 1000);
}

TIM_TypeDef *portToTIM(TIM_PORT_NAME port){
	switch(port){
		case TIM_PORT_1:{
			return TIM1;
		}
		case TIM_PORT_2:{
			return TIM2;
		}
		case TIM_PORT_3:{
			return TIM3;
		}
//		case TIM_PORT_4:{
//			return TIM4;
//		}
		default:{
			return 0;
		}
	}
}

uint32_t millis(void){
	return msTicks;
}


void SysTick_Handler(void){
	msTicks++;
}


void timerInit(TIM_PORT_NAME port, uint32_t psc, uint32_t arr){
	TIM_TypeDef *TIMx = portToTIM(port);
	switch(port){
		case TIM_PORT_1:{
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			break;
		}
		case TIM_PORT_2:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			break;
		}
		case TIM_PORT_3:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			break;
		}
//		case TIM_PORT_4:{
//			RCC->APB1ENR |= RCC_APB1ENR_TIM;
//		}
	}

	TIMx->PSC = psc;
	TIMx->ARR = arr;

	TIMx->CNT = 0;
	TIMx->CR1 |= TIM_CR1_ARPE;
	TIMx->EGR |= TIM_EGR_UG;
	TIMx->DIER |= TIM_DIER_UIE;
	TIMx->CR1 |= TIM_CR1_CEN;
}


void debounceTimerInit(TIM_PORT_NAME port, uint32_t psc, uint32_t arr){
	TIM_TypeDef *TIMx = portToTIM(port);
	switch(port){
		case TIM_PORT_1:{
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			NVIC_EnableIRQ(TIM1_UP_IRQn);
			break;
		}
		case TIM_PORT_2:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			NVIC_EnableIRQ(TIM2_IRQn);
			break;
		}
		case TIM_PORT_3:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			NVIC_EnableIRQ(TIM3_IRQn);
			break;
		}
//		case TIM_PORT_4:{
//			RCC->APB1ENR |= RCC_APB1ENR_TIM;
//		}
	}


	TIMx->PSC = psc;
	TIMx->ARR = arr;
	TIMx->CNT = 0;
	TIMx->DIER |= TIM_DIER_UIE;
	TIMx->CR1 &= ~TIM_CR1_CEN;

}
