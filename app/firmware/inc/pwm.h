#ifndef PWM_H
#define PWM_H
#include "mcu_config.h"
#include "tim.h"
extern const uint32_t CCMR_OCxM[4];
extern const uint32_t CCMR_OCxPos[4];
extern const uint32_t CCMR_OCxPE[4];
extern const uint32_t CCER_CCxE[4];

void pwmInit(TIM_PORT_NAME, uint8_t);
void pwmWrite(TIM_PORT_NAME, uint8_t, uint16_t);


#endif
