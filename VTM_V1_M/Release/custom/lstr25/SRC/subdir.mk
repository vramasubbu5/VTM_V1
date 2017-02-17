################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/lstr25/SRC/copy_27.c \
../custom/lstr25/SRC/data.c \
../custom/lstr25/SRC/flash.c \
../custom/lstr25/SRC/flashing.c \
../custom/lstr25/SRC/global.c \
../custom/lstr25/SRC/gpio.c \
../custom/lstr25/SRC/gpiop.c \
../custom/lstr25/SRC/main.c \
../custom/lstr25/SRC/multi_event.c \
../custom/lstr25/SRC/multi_time.c \
../custom/lstr25/SRC/proj_main.c \
../custom/lstr25/SRC/rtc.c \
../custom/lstr25/SRC/sms.c \
../custom/lstr25/SRC/subtask1_gpio.c \
../custom/lstr25/SRC/task1.c \
../custom/lstr25/SRC/task2.c \
../custom/lstr25/SRC/task3.c \
../custom/lstr25/SRC/task4.c \
../custom/lstr25/SRC/taskmi.c \
../custom/lstr25/SRC/test_multi_2.c \
../custom/lstr25/SRC/utils.c 

OBJS += \
./custom/lstr25/SRC/copy_27.o \
./custom/lstr25/SRC/data.o \
./custom/lstr25/SRC/flash.o \
./custom/lstr25/SRC/flashing.o \
./custom/lstr25/SRC/global.o \
./custom/lstr25/SRC/gpio.o \
./custom/lstr25/SRC/gpiop.o \
./custom/lstr25/SRC/main.o \
./custom/lstr25/SRC/multi_event.o \
./custom/lstr25/SRC/multi_time.o \
./custom/lstr25/SRC/proj_main.o \
./custom/lstr25/SRC/rtc.o \
./custom/lstr25/SRC/sms.o \
./custom/lstr25/SRC/subtask1_gpio.o \
./custom/lstr25/SRC/task1.o \
./custom/lstr25/SRC/task2.o \
./custom/lstr25/SRC/task3.o \
./custom/lstr25/SRC/task4.o \
./custom/lstr25/SRC/taskmi.o \
./custom/lstr25/SRC/test_multi_2.o \
./custom/lstr25/SRC/utils.o 

C_DEPS += \
./custom/lstr25/SRC/copy_27.d \
./custom/lstr25/SRC/data.d \
./custom/lstr25/SRC/flash.d \
./custom/lstr25/SRC/flashing.d \
./custom/lstr25/SRC/global.d \
./custom/lstr25/SRC/gpio.d \
./custom/lstr25/SRC/gpiop.d \
./custom/lstr25/SRC/main.d \
./custom/lstr25/SRC/multi_event.d \
./custom/lstr25/SRC/multi_time.d \
./custom/lstr25/SRC/proj_main.d \
./custom/lstr25/SRC/rtc.d \
./custom/lstr25/SRC/sms.d \
./custom/lstr25/SRC/subtask1_gpio.d \
./custom/lstr25/SRC/task1.d \
./custom/lstr25/SRC/task2.d \
./custom/lstr25/SRC/task3.d \
./custom/lstr25/SRC/task4.d \
./custom/lstr25/SRC/taskmi.d \
./custom/lstr25/SRC/test_multi_2.d \
./custom/lstr25/SRC/utils.d 


# Each subdirectory must supply rules for building sources it contributes
custom/lstr25/SRC/%.o: ../custom/lstr25/SRC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__CUSTOMER_CODE__ -D__OCPU_COMPILER_GCC__ -I"${GCC_PATH}\arm-none-eabi\include" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\include" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\ril\inc" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\config" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\lstr25\INC" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\lstr25\SRC" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


