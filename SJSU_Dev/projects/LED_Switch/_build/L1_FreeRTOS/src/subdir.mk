################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L1_FreeRTOS/src/croutine.c \
../L1_FreeRTOS/src/event_groups.c \
../L1_FreeRTOS/src/list.c \
../L1_FreeRTOS/src/queue.c \
../L1_FreeRTOS/src/tasks.c \
../L1_FreeRTOS/src/timers.c 

OBJS += \
./L1_FreeRTOS/src/croutine.o \
./L1_FreeRTOS/src/event_groups.o \
./L1_FreeRTOS/src/list.o \
./L1_FreeRTOS/src/queue.o \
./L1_FreeRTOS/src/tasks.o \
./L1_FreeRTOS/src/timers.o 

C_DEPS += \
./L1_FreeRTOS/src/croutine.d \
./L1_FreeRTOS/src/event_groups.d \
./L1_FreeRTOS/src/list.d \
./L1_FreeRTOS/src/queue.d \
./L1_FreeRTOS/src/tasks.d \
./L1_FreeRTOS/src/timers.d 


# Each subdirectory must supply rules for building sources it contributes
L1_FreeRTOS/src/%.o: ../L1_FreeRTOS/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\newlib" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L0_LowLevel" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L1_FreeRTOS" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L1_FreeRTOS\include" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L1_FreeRTOS\portable\no_mpu" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L2_Drivers" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L2_Drivers\base" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L3_Utils" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L3_Utils\tlm" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L4_IO" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L4_IO\fat" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L4_IO\wireless" -I"C:\CMPE146\CMPE-146\SJSU_Dev\projects\LED_Switch\L5_Application" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


