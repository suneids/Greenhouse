#ifndef ACTIVITIES_H_
#define ACTIVITIES_H_
#include "config.h"

uint8_t SHT40_Read();
void STANDBY_Enter();
uint8_t RTC_Init_LSI_Once();
uint8_t RTC_WakeupSeconds(uint32_t seconds);

void PUMP_Off(void);
void PUMP_Run(uint16_t duty);
void COOLER_Off(void);
void COOLER_Run(uint16_t duty);

void GREENHOUSE_Init();
void GREENHOUSE_On();
void GREENHOUSE_Off();
void GREENHOUSE_ReadState();
void GREENHOUSE_Watering(void);
void GREENHOUSE_Cooling(void);
void GREENHOUSE_PeripheryOn(void);
void GREENHOUSE_PeripheryOff(void);


#endif /* ACTIVITIES_H_ */
