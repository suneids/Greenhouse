#include "activities.h"


uint8_t WaitFlagSet(volatile uint32_t *reg, uint32_t mask, uint32_t timeout)
{
    while(((*reg & mask) == 0u) && timeout--) {
    }

    return ((*reg & mask) != 0u);
}


uint8_t SHT40_Read(){
	I2C_WriteByteArray(I2C1, SHT40_ADDR, SHT40_CMD_MEASURE_HIGH, 0, 0);

	SysTick_Delay(10);

	I2C_Read_Burst(I2C1, SHT40_ADDR, I2C_NO_REG, data, 6);

	raw_t = ((uint16_t)data[0] << 8) | data[1];
	raw_rh = ((uint16_t)data[3] << 8) | data[4];
	temp =  -45.f + 175.f * ((float)raw_t / 65535.f);
	rh =  -6.f + 125.f * ((float)raw_rh / 65535.f);
	if(rh < 0.f)   rh = 0.f;
	if(rh > 100.f) rh = 100.f;
	return 1;
}


void STANDBY_Enter(){
	GREENHOUSE_Off();
	RCC->APBENR1 |= RCC_APBENR1_PWREN;

	PWR->SCR = PWR_SCR_CWUF | PWR_SCR_CSBF;

	PWR->CR1 &= ~PWR_CR1_LPMS;
	PWR->CR1 |= PWR_CR1_LPMS_STANDBY;

	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	__DSB();
	__WFI();
}


uint8_t RTC_Init_LSI_Once(void)
{
	RCC->APBENR1 |= RCC_APBENR1_PWREN | RCC_APBENR1_RTCAPBEN;;
    (void)RCC->APBENR1;

	PWR->CR1 |= PWR_CR1_DBP;
	if(!WaitFlagSet(&PWR->CR1, PWR_CR1_DBP, 1000000u)) {
		return 0;
	}

	if((RCC->BDCR & RCC_BDCR_RTCEN) && ((RCC->BDCR & RCC_BDCR_RTCSEL) == RCC_BDCR_RTCSEL_1)){
		return 1;
	}

	RCC->CSR |= RCC_CSR_LSION;
	if(!WaitFlagSet(&RCC->CSR, RCC_CSR_LSIRDY, 1000000u)) {
		return 0;
	}

	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;

	RCC->BDCR &= ~RCC_BDCR_RTCSEL;
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;

	RCC->BDCR |= RCC_BDCR_RTCEN;

	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	RTC->ICSR |= RTC_ICSR_INIT;

	if(!WaitFlagSet(&RTC->ICSR, RTC_ICSR_INITF, 1000000u)) {
		RTC->WPR = 0xFF;
		return 0;
	}

	RTC->PRER = (127u << RTC_PRER_PREDIV_A_Pos) |
	            (249u << RTC_PRER_PREDIV_S_Pos);

	RTC->ICSR &= ~RTC_ICSR_INIT;
	RTC->WPR = 0xFF;

	return 1;
}


uint8_t RTC_WakeupSeconds(uint32_t seconds)
{
	if(seconds == 0) seconds = 1;
	else if(seconds > 65536u) seconds = 65536u;

	RCC->APBENR1 |= RCC_APBENR1_PWREN;
	PWR->CR1 |= PWR_CR1_DBP;

	if(!WaitFlagSet(&PWR->CR1, PWR_CR1_DBP, 1000000u)) {
		return 0;
	}

	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	RTC->CR &= ~RTC_CR_WUTE;

	if(!WaitFlagSet(&RTC->ICSR, RTC_ICSR_WUTWF, 1000000u)) {
		RTC->WPR = 0xFF;
		return 0;
	}

	RTC->SCR = RTC_SCR_CWUTF;
	PWR->SCR = PWR_SCR_CWUF | PWR_SCR_CSBF;

	RTC->WUTR = seconds - 1u;

	RTC->CR &= ~RTC_CR_WUCKSEL;
	RTC->CR |= RTC_CR_WUCKSEL_2;

	RTC->CR |= RTC_CR_WUTIE;
	RTC->CR |= RTC_CR_WUTE;

	RTC->WPR = 0xFF;

	return 1;
}

void PUMP_Off(void)
{
    PWM_Write(pump, 0);
}


void PUMP_Run(uint16_t duty)
{
    if(duty < PWM_20_PERCENT) duty = PWM_20_PERCENT;

    PWM_Write(pump, PWM_100_PERCENT);
    SysTick_Delay(300);

    PWM_Write(pump, duty);
}


void COOLER_Off(void)
{
    PWM_Write(cooler, 0);
}


void COOLER_Run(uint16_t duty)
{
    if(duty < PWM_20_PERCENT) duty = PWM_20_PERCENT;

    PWM_Write(cooler, PWM_100_PERCENT);
    SysTick_Delay(300);
    PWM_Write(cooler, duty);
}


void GREENHOUSE_Init(){
	SysTick_Init();
	SysTick_Delay(2000);
	GPIO_PinMode(power);
	GPIO_DigitalWrite(power, 1);

	TIM_Init(pwm_tim, pwm_psc, PWM_MAX, 0);
	PWM_Init(pump);
	PWM_Init(cooler);

	adc_pins[0] = soil0;
	adc_pins[1] = soil1;
	adc_pins[2] = soil2;

	I2C_Init(I2C1, SDA, SCL);
	ADC_InitMulti(adc_pins, ADC_COUNT, need_ADC_DMA);
	DMA_Init(DMA1_Channel1,
			 (uint32_t)&ADC1->DR,
			 (uint32_t)soil_raw,
			 ADC_COUNT);
	ADC_Start();

	TIM3->SMCR = 0;
	TIM3->CR1 |= TIM_CR1_CEN;

	USART_Init(USART1, 9600, 0, 1);
	GPIO_Pin_t tx = {.port = GPIOA, .number = 9, .moder = GPIO_MODE_AF, .otype = GPIO_OTYPE_OD,
			         .pull = GPIO_NOPULL, .speed = GPIO_SPEED_LOW, .af = 1};
	GPIO_PinMode(tx);
	Radio_Init();
}


void GREENHOUSE_On(){
	GPIO_DigitalWrite(power, 1);
}


void GREENHOUSE_Off(){
	GPIO_DigitalWrite(power, 0);
	COOLER_Off();
	PUMP_Off();
	GREENHOUSE_PeripheryOff();
}


void GREENHOUSE_ReadState(){
	GREENHOUSE_PeripheryOn();
	SysTick_Delay(50);
	SHT40_Read();
	avg_soil_h = (soil_raw[0] + soil_raw[1] + soil_raw[2]) / 3;
}


void GREENHOUSE_Watering(void)
{
    const uint32_t watering_time_ms = 20000u,
				   soak_wait_ms     = 20000u;
    const uint8_t  max_attempts     = 3u;

    for(uint8_t attempt = 0; attempt < max_attempts; attempt++) {

        GREENHOUSE_ReadState();

        if(avg_soil_h < soil_dry_threshold) break;

        PUMP_Run(PWM_100_PERCENT);
        SysTick_Delay(watering_time_ms);
        PUMP_Off();

        if((attempt + 1u) < max_attempts)  SysTick_Delay(soak_wait_ms);
    }

    PUMP_Off();
}


void GREENHOUSE_Cooling(void)
{
	//Параметры для клубники Елизавета II
    const float temp_threshold = 26.0f;
    const float rh_threshold   = 70.0f;
    const uint32_t max_cooling_ms = 30000u;
    const uint32_t check_period_ms = 5000u;

    uint32_t start_ms = millis();

    GREENHOUSE_ReadState();


    if (temp < 25) {
    	if(rh > 75)      COOLER_Run(PWM_100_PERCENT);
		else if(rh < 65) COOLER_Off();
    }
    else if (temp < 28) {
    	if(rh > 70)      COOLER_Run(PWM_100_PERCENT);
		else if(rh < 60) COOLER_Off();
    }
    else {
    	if(rh > 65)      COOLER_Run(PWM_100_PERCENT);
    	else if(rh < 55) COOLER_Off();
    }

    while((millis() - start_ms) < max_cooling_ms) {
        SysTick_Delay(check_period_ms);

        GREENHOUSE_ReadState();

        if((temp <= temp_threshold) && (rh <= rh_threshold)) break;

    }

    COOLER_Off();
}


void GREENHOUSE_PeripheryOn(void){

}


void GREENHOUSE_PeripheryOff(void){

}


void GREENHOUSE_Task(void)
{
   if(pump_off_time_ms != 0 && millis() >= pump_off_time_ms) {
	   PUMP_Off();
	   pump_off_time_ms = 0;
   }

   if(autowatering)
	   if(avg_soil_h > soil_dry_threshold && water_available)
		   GREENHOUSE_Watering();


   if(autocooling) GREENHOUSE_Cooling();
}


void Radio_HandlePacket(const RadioPacket_t *pkt){
	if(pkt->id != MY_ID && pkt->id != DEV_BROADCAST) {
		return;
	}

	switch(pkt->cmd) {
		case CMD_STATUS_REQUEST:
			if(pkt->id == DEV_GREENHOUSE){
				GREENHOUSE_SendStatus();
			}
			break;
		case CMD_GREENHOUSE_FAN_SET:
			if(pkt->len >= 1) {
				if(pkt->data[0]) {
					COOLER_Run(PWM_100_PERCENT);
				} else {
					COOLER_Off();
				}
			}
			break;

		case CMD_GREENHOUSE_PUMP_SET:
			if(pkt->len >= 1) {
				uint8_t seconds = pkt->data[0];

				if(seconds == 0) {
					PUMP_Off();
					pump_off_time_ms = 0;
				} else {
					PUMP_Run(PWM_100_PERCENT);
					pump_off_time_ms = millis() + ((uint32_t)seconds * 1000u);
				}
			}
			break;

		case CMD_GREENHOUSE_AUTOWATER_SET:
			if(pkt->len >= 1) {
				autowatering = pkt->data[0] ? 1 : 0;
			}
			break;

		case CMD_GREENHOUSE_AUTOVENT_SET:
			if(pkt->len >= 1) {
				autocooling = pkt->data[0] ? 1 : 0;
			}
			break;

		case CMD_GREENHOUSE_SOIL_LIMIT_SET:
			if(pkt->len >= 2)  soil_dry_threshold = pkt->data[0] |((uint16_t)pkt->data[1] << 8);

			break;

		default:
			break;
	}
}


void Radio_Task(USART_TypeDef *USARTx)
{
    RadioPacket_t pkt;

    while(USART_Available(USARTx) > 0) {
        uint8_t byte = (uint8_t)USART_ReadByte(USARTx);

        if(RadioParser_FeedByte(&radio_parser, byte, &pkt)) {
            Radio_HandlePacket(&pkt);
        }
    }
}

