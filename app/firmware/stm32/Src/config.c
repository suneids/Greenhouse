#include "config.h"
#include "protocol.h"
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
uint32_t pwm_psc = 63, avg_soil_h = 0, pump_off_time_ms = 0;
uint16_t raw_t = 0, raw_rh = 0, soil_dry_threshold = 1500;
float temp = 0, rh = 0;
uint8_t need_ADC_DMA = 1;

uint8_t autowatering = 0, autocooling = 0, fan_on = 0, water_available = 0;

void GREENHOUSE_SendStatus(void)
{
    int16_t temp_x10 = temp*10;      // 24.6 °C
    uint8_t air_hum = rh;        // 61 %
    uint16_t soil_raw = avg_soil_h;
    uint8_t soil_percent = avg_soil_h * 100 / 4095;

    uint8_t data[7];

    data[0] = temp_x10 & 0xFF;
    data[1] = (temp_x10 >> 8) & 0xFF;

    data[2] = air_hum;

    data[3] = soil_raw & 0xFF;
    data[4] = (soil_raw >> 8) & 0xFF;

    data[5] = soil_percent;
    data[6] = water_available;
    SysTick_Delay(150);
    Radio_SendPacket(
        DEV_GREENHOUSE,
        CMD_STATUS_RESPONSE,
        data,
        7
    );
}

