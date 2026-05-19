#include "../../../inc/i2c.h"
#if defined(STM32F103)

void I2C_Init(I2C_TypeDef *I2Cx, Pin_t SDA, Pin_t SCL){
	GPIO_PinMode(SCL, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_PUSH_PULL, 0);

	// Генерируем 9 тактов, чтобы датчик "выплюнул" застрявший бит
	// TODO сделать универсальнее на основе переданных SDA SCL
	for(int i = 0; i < 9; i++) {
		GPIOB->BRR = GPIO_BRR_BR6;   // SCL в 0
		for(volatile int d = 0; d < 500; d++);
		GPIOB->BSRR = GPIO_BSRR_BS6;  // SCL в 1
		for(volatile int d = 0; d < 500; d++);
	}

	GPIO_PinMode(SDA, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_OPEN_DRAIN_ALT, 0);
	GPIO_PinMode(SCL, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_OPEN_DRAIN_ALT, 0);

	I2Cx->CR1 |= I2C_CR1_SWRST;
	for(volatile int i = 0; i < 1000; i++);
	I2Cx->CR1 &= ~I2C_CR1_SWRST;
	I2Cx->CR2 |= 8;
	I2Cx->CCR = 40;
	I2Cx->TRISE = 9;
	I2Cx->CR1 |= I2C_CR1_PE;
	for(volatile int i = 0; i < 10000; i++);
}


void I2C_Start(I2C_TypeDef *I2Cx){
	I2Cx->CR1 |= I2C_CR1_START;
	while(!(I2Cx->SR1 & I2C_SR1_SB));
}


static void I2C_WriteByte(I2C_TypeDef *I2Cx, uint8_t data){
	uint32_t timeout = 10000;
	I2Cx->DR = data;
	while(!(I2Cx->SR1 & I2C_SR1_TXE) && timeout--);
}


static void I2C_Send_Address_And_Reg(I2C_TypeDef *I2Cx, uint8_t devAddr, uint8_t regAddr){
	I2C_Start(I2Cx);
	I2C_WriteByte(I2Cx, devAddr << 1);

	while(!(I2Cx->SR1 & I2C_SR1_ADDR));
	(void)I2Cx->SR2;

	I2C_WriteByte(I2Cx, regAddr);
	while(!(I2Cx->SR1 & I2C_SR1_BTF));
}


void I2C_WriteReg(I2C_TypeDef *I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t data){
	I2C_Send_Address_And_Reg(I2Cx, devAddr, regAddr);
	I2C_WriteByte(I2Cx, data);
	I2Cx->CR1 |= I2C_CR1_STOP;
}



void I2C_WriteByteArray(I2C_TypeDef *I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t *pData, uint16_t len){
	I2C_Send_Address_And_Reg(I2Cx, devAddr, regAddr);
	for(uint16_t i = 0; i < len; i++){
		I2C_WriteByte(I2Cx, pData[i]);
	}

	I2Cx->CR1 |= I2C_CR1_STOP;
}



uint8_t I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t devAddr, uint8_t regAddr){
	uint8_t data = 0;
	I2C_Read_Burst(I2Cx, devAddr, regAddr, &data, 1);
	return data;
}


void I2C_Read_Burst(I2C_TypeDef *I2Cx, uint8_t devAddr, uint8_t regAddr, uint8_t *pBuffer, uint16_t size){
	while(I2Cx->SR2 & I2C_SR2_BUSY);

	I2C_Send_Address_And_Reg(I2Cx, devAddr, regAddr);

	I2C_Start(I2Cx);
	I2Cx->DR = (devAddr << 1) | 0x01;
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));

	if(size == 1){
		I2Cx->CR1 &= ~I2C_CR1_ACK;
		(void)I2Cx->SR2;
		I2Cx->CR1 |= I2C_CR1_STOP;
	}
	else{
		I2Cx->CR1 |= I2C_CR1_ACK;
		(void)I2Cx->SR2;
	}

	for(uint16_t i = 0; i < size; i++){
		if(i == size - 1){
			I2Cx->CR1 &= ~I2C_CR1_ACK;
			I2Cx->CR1 |= I2C_CR1_STOP;
		}

		while(!(I2Cx->SR1 & I2C_SR1_RXNE));
		pBuffer[i] = I2Cx->DR;
	}

}
#endif
