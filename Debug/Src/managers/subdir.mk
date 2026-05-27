################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/managers/adc_manager.c \
../Src/managers/can_manager.c \
../Src/managers/config_manager.c \
../Src/managers/interrupt_manager.c \
../Src/managers/pwm_manager.c \
../Src/managers/sensor_manager.c 

OBJS += \
./Src/managers/adc_manager.o \
./Src/managers/can_manager.o \
./Src/managers/config_manager.o \
./Src/managers/interrupt_manager.o \
./Src/managers/pwm_manager.o \
./Src/managers/sensor_manager.o 

C_DEPS += \
./Src/managers/adc_manager.d \
./Src/managers/can_manager.d \
./Src/managers/config_manager.d \
./Src/managers/interrupt_manager.d \
./Src/managers/pwm_manager.d \
./Src/managers/sensor_manager.d 


# Each subdirectory must supply rules for building sources it contributes
Src/managers/%.o Src/managers/%.su Src/managers/%.cyclo: ../Src/managers/%.c Src/managers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-managers

clean-Src-2f-managers:
	-$(RM) ./Src/managers/adc_manager.cyclo ./Src/managers/adc_manager.d ./Src/managers/adc_manager.o ./Src/managers/adc_manager.su ./Src/managers/can_manager.cyclo ./Src/managers/can_manager.d ./Src/managers/can_manager.o ./Src/managers/can_manager.su ./Src/managers/config_manager.cyclo ./Src/managers/config_manager.d ./Src/managers/config_manager.o ./Src/managers/config_manager.su ./Src/managers/interrupt_manager.cyclo ./Src/managers/interrupt_manager.d ./Src/managers/interrupt_manager.o ./Src/managers/interrupt_manager.su ./Src/managers/pwm_manager.cyclo ./Src/managers/pwm_manager.d ./Src/managers/pwm_manager.o ./Src/managers/pwm_manager.su ./Src/managers/sensor_manager.cyclo ./Src/managers/sensor_manager.d ./Src/managers/sensor_manager.o ./Src/managers/sensor_manager.su

.PHONY: clean-Src-2f-managers

