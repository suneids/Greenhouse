#include "activities.h"


int main(void)
{

	GREENHOUSE_Init();

	if(!RTC_Init_LSI_Once()) {
		GREENHOUSE_Off();
		while(1); // ошибка RTC init, не спим
	}

	GREENHOUSE_ReadState();
//	GREENHOUSE_Watering();
//	GREENHOUSE_Cooling();

//	GREENHOUSE_Off();
//
//	if(RTC_WakeupSeconds(60)) {
//		STANDBY_Enter();
//	}

	while(1){

		GREENHOUSE_ReadState();
		GREENHOUSE_Task();
		Radio_Task(USART1);
	}
}
