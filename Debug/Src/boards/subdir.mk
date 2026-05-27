################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/boards/front_board.c \
../Src/boards/left_board.c \
../Src/boards/right_board.c 

OBJS += \
./Src/boards/front_board.o \
./Src/boards/left_board.o \
./Src/boards/right_board.o 

C_DEPS += \
./Src/boards/front_board.d \
./Src/boards/left_board.d \
./Src/boards/right_board.d 


# Each subdirectory must supply rules for building sources it contributes
Src/boards/%.o Src/boards/%.su Src/boards/%.cyclo: ../Src/boards/%.c Src/boards/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-boards

clean-Src-2f-boards:
	-$(RM) ./Src/boards/front_board.cyclo ./Src/boards/front_board.d ./Src/boards/front_board.o ./Src/boards/front_board.su ./Src/boards/left_board.cyclo ./Src/boards/left_board.d ./Src/boards/left_board.o ./Src/boards/left_board.su ./Src/boards/right_board.cyclo ./Src/boards/right_board.d ./Src/boards/right_board.o ./Src/boards/right_board.su

.PHONY: clean-Src-2f-boards

