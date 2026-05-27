################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/sensors/coolant_temp.c \
../Src/sensors/linear_pot.c \
../Src/sensors/pitot_tube.c \
../Src/sensors/steering_angle.c \
../Src/sensors/tach.c \
../Src/sensors/wheel_speed.c 

OBJS += \
./Src/sensors/coolant_temp.o \
./Src/sensors/linear_pot.o \
./Src/sensors/pitot_tube.o \
./Src/sensors/steering_angle.o \
./Src/sensors/tach.o \
./Src/sensors/wheel_speed.o 

C_DEPS += \
./Src/sensors/coolant_temp.d \
./Src/sensors/linear_pot.d \
./Src/sensors/pitot_tube.d \
./Src/sensors/steering_angle.d \
./Src/sensors/tach.d \
./Src/sensors/wheel_speed.d 


# Each subdirectory must supply rules for building sources it contributes
Src/sensors/%.o Src/sensors/%.su Src/sensors/%.cyclo: ../Src/sensors/%.c Src/sensors/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-sensors

clean-Src-2f-sensors:
	-$(RM) ./Src/sensors/coolant_temp.cyclo ./Src/sensors/coolant_temp.d ./Src/sensors/coolant_temp.o ./Src/sensors/coolant_temp.su ./Src/sensors/linear_pot.cyclo ./Src/sensors/linear_pot.d ./Src/sensors/linear_pot.o ./Src/sensors/linear_pot.su ./Src/sensors/pitot_tube.cyclo ./Src/sensors/pitot_tube.d ./Src/sensors/pitot_tube.o ./Src/sensors/pitot_tube.su ./Src/sensors/steering_angle.cyclo ./Src/sensors/steering_angle.d ./Src/sensors/steering_angle.o ./Src/sensors/steering_angle.su ./Src/sensors/tach.cyclo ./Src/sensors/tach.d ./Src/sensors/tach.o ./Src/sensors/tach.su ./Src/sensors/wheel_speed.cyclo ./Src/sensors/wheel_speed.d ./Src/sensors/wheel_speed.o ./Src/sensors/wheel_speed.su

.PHONY: clean-Src-2f-sensors

