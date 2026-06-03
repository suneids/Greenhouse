#ifndef CONFIG_H
#define CONFIG_H
#include "../inc/HAL_STM32F103C6T6/inc/gpio.h"
#include "../inc/HAL_STM32F103C6T6/inc/i2c.h"
#include "../inc/HAL_STM32F103C6T6/inc/tim.h"
#include "../inc/HAL_STM32F103C6T6/inc/pwm.h"
#include "../inc/HAL_STM32F103C6T6/inc/adc.h"
#include "../inc/HAL_STM32F103C6T6/inc/dma.h"
#include "../inc/HAL_STM32F103C6T6/inc/usart.h"
#include "protocol.h"

#define PWR_CR1_LPMS_STANDBY (0x3u << PWR_CR1_LPMS_Pos)
#define SHT40_ADDR 0x44
#define SHT40_CMD_MEASURE_HIGH 0xFD
#define ADC_COUNT 3
#define PWM_MAX 999u
#define PWM_20_PERCENT 200u
#define PWM_50_PERCENT 500u
#define PWM_100_PERCENT 999u

extern uint8_t autowatering, autocooling, fan_on, water_available;
extern GPIO_Pin_t SDA, SCL,
				  soil0, soil1, soil2,
				  pump, pump_f, cooler,
				  power;

extern GPIO_Pin_t adc_pins[ADC_COUNT];
extern TIM_TypeDef *pwm_tim;

extern uint8_t data[6];
extern volatile uint16_t soil_raw[ADC_COUNT];
extern uint32_t pwm_psc, avg_soil_h, pump_off_time_ms;
extern uint16_t raw_t, raw_rh, soil_dry_threshold;
extern float temp, rh;
extern uint8_t need_ADC_DMA;
#define CMD_GREENHOUSE_STATUS 0x30

void GREENHOUSE_SendStatus(void);
#endif /* CONFIG_H_ */
