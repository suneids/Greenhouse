#include "config.h"

GPIO_Pin_t SDA    = {.port   = GPIOB, .number = 6},
		   SCL    = {.port   = GPIOB, .number = 7},

		   soil0  = {.port   = GPIOA, .number = 0},
		   soil1  = {.port   = GPIOA, .number = 1},
		   soil2  = {.port   = GPIOA, .number = 4},
		   pump_f = {.port   = GPIOB, .number = 0},

		   pump   = {.port   = GPIOA, .number = 6},
		   cooler = {.port   = GPIOA, .number = 7},
		   power  = {.port   = GPIOB, .number = 8, .moder = GPIO_MODE_OUTPUT, .otype = GPIO_OTYPE_PP, .speed = GPIO_SPEED_HIGH, .pull = GPIO_PULLDOWN, .af = 0};

GPIO_Pin_t adc_pins[ADC_COUNT];
TIM_TypeDef *pwm_tim = TIM3;

uint8_t data[6];
volatile uint16_t soil_raw[ADC_COUNT] = {0, 0, 0};
uint32_t pwm_psc = 63, avg_soil_h = 0;
uint16_t raw_t = 0, raw_rh = 0;
float temp = 0, rh = 0;
uint8_t need_ADC_DMA = 1;


