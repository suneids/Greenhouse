#ifndef CONFIG_H
#define CONFIG_H
#include "../inc/myhal/inc/gpio.h"
#include "../inc/myhal/inc/i2c.h"
#include "../inc/myhal/inc/tim.h"
#include "../inc/myhal/inc/pwm.h"
#include "../inc/myhal/inc/adc.h"
#include "../inc/myhal/inc/dma.h"

#define PWR_CR1_LPMS_STANDBY (0x3u << PWR_CR1_LPMS_Pos)
#define SHT40_ADDR 0x44
#define SHT40_CMD_MEASURE_HIGH 0xFD
#define ADC_COUNT 4
#define PWM_MAX 999u
#define PWM_20_PERCENT 200u
#define PWM_50_PERCENT 500u
#define PWM_100_PERCENT 999u

extern GPIO_Pin_t SDA, SCL,
				  soil0, soil1, soil2,
				  pump, pump_f, cooler,
				  power;

extern GPIO_Pin_t adc_pins[ADC_COUNT];
extern TIM_TypeDef *pwm_tim;

extern uint8_t data[6];
extern volatile uint16_t soil_raw[ADC_COUNT];
extern uint32_t pwm_psc, avg_soil_h;
extern uint16_t raw_t, raw_rh;
extern float temp, rh;
extern uint8_t need_ADC_DMA;

#endif /* CONFIG_H_ */
