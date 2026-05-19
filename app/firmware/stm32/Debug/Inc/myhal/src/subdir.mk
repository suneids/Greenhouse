################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/myhal/src/modbus.c \
../Inc/myhal/src/nvstore.c \
../Inc/myhal/src/soft_i2c.c \
../Inc/myhal/src/soft_uart.c \
../Inc/myhal/src/utilities.c 

OBJS += \
./Inc/myhal/src/modbus.o \
./Inc/myhal/src/nvstore.o \
./Inc/myhal/src/soft_i2c.o \
./Inc/myhal/src/soft_uart.o \
./Inc/myhal/src/utilities.o 

C_DEPS += \
./Inc/myhal/src/modbus.d \
./Inc/myhal/src/nvstore.d \
./Inc/myhal/src/soft_i2c.d \
./Inc/myhal/src/soft_uart.d \
./Inc/myhal/src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/myhal/src/%.o Inc/myhal/src/%.su Inc/myhal/src/%.cyclo: ../Inc/myhal/src/%.c Inc/myhal/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32G0 -DSTM32G0B1CBTx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Inc-2f-myhal-2f-src

clean-Inc-2f-myhal-2f-src:
	-$(RM) ./Inc/myhal/src/modbus.cyclo ./Inc/myhal/src/modbus.d ./Inc/myhal/src/modbus.o ./Inc/myhal/src/modbus.su ./Inc/myhal/src/nvstore.cyclo ./Inc/myhal/src/nvstore.d ./Inc/myhal/src/nvstore.o ./Inc/myhal/src/nvstore.su ./Inc/myhal/src/soft_i2c.cyclo ./Inc/myhal/src/soft_i2c.d ./Inc/myhal/src/soft_i2c.o ./Inc/myhal/src/soft_i2c.su ./Inc/myhal/src/soft_uart.cyclo ./Inc/myhal/src/soft_uart.d ./Inc/myhal/src/soft_uart.o ./Inc/myhal/src/soft_uart.su ./Inc/myhal/src/utilities.cyclo ./Inc/myhal/src/utilities.d ./Inc/myhal/src/utilities.o ./Inc/myhal/src/utilities.su

.PHONY: clean-Inc-2f-myhal-2f-src

