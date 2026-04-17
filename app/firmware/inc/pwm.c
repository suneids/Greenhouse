#include "pwm.h"
const uint32_t CCMR_OCxM[4] = { TIM_CCMR1_OC1M, TIM_CCMR1_OC2M,
						 TIM_CCMR2_OC3M, TIM_CCMR2_OC4M };
const uint32_t CCMR_OCxPos[4] = { TIM_CCMR1_OC1M_Pos, TIM_CCMR1_OC2M_Pos,
							TIM_CCMR2_OC3M_Pos, TIM_CCMR2_OC4M_Pos };
const uint32_t CCMR_OCxPE[4] = {TIM_CCMR1_OC1PE, TIM_CCMR1_OC2PE,
						  TIM_CCMR2_OC3PE, TIM_CCMR2_OC4PE};
const uint32_t CCER_CCxE[4] = {TIM_CCER_CC1E, TIM_CCER_CC2E,
						 TIM_CCER_CC3E, TIM_CCER_CC4E };

void pwmInit(TIM_PORT_NAME port, uint8_t channel){
	TIM_TypeDef *TIMx = portToTIM(port);
	if((channel < 1) || (channel > 4)){
		//TODO assert error
	}
	__IO uint32_t *CCMRx = channel <= 2? &TIMx->CCMR1 : &TIMx->CCMR2;
	channel -= 1;
	*CCMRx &= ~(CCMR_OCxM[channel]);
	*CCMRx |= (6 << CCMR_OCxPos[channel]);
	*CCMRx |= (CCMR_OCxPE[channel]);
	TIMx->CCER |= (CCER_CCxE[channel]);
}


void pwmWrite(TIM_PORT_NAME port, uint8_t channel, uint16_t value){
	TIM_TypeDef *TIMx = portToTIM(port);
	__IO uint16_t *CCRs[4] = {&TIMx->CCR1, &TIMx->CCR2, &TIMx->CCR3, &TIMx->CCR4};
	if((channel > 0) && (channel < 5) && (value <= TIMx->ARR)){
		*CCRs[channel - 1] = value;
	}
}
