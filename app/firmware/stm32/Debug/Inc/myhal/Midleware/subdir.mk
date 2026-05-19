################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/myhal/Midleware/button.c 

OBJS += \
./Inc/myhal/Midleware/button.o 

C_DEPS += \
./Inc/myhal/Midleware/button.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/myhal/Midleware/%.o Inc/myhal/Midleware/%.su Inc/myhal/Midleware/%.cyclo: ../Inc/myhal/Midleware/%.c Inc/myhal/Midleware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32G0 -DSTM32G0B1CBTx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Inc-2f-myhal-2f-Midleware

clean-Inc-2f-myhal-2f-Midleware:
	-$(RM) ./Inc/myhal/Midleware/button.cyclo ./Inc/myhal/Midleware/button.d ./Inc/myhal/Midleware/button.o ./Inc/myhal/Midleware/button.su

.PHONY: clean-Inc-2f-myhal-2f-Midleware

