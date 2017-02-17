################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/config/custom_sys_cfg.c \
../custom/config/sys_config.c 

OBJS += \
./custom/config/custom_sys_cfg.o \
./custom/config/sys_config.o 

C_DEPS += \
./custom/config/custom_sys_cfg.d \
./custom/config/sys_config.d 


# Each subdirectory must supply rules for building sources it contributes
custom/config/%.o: ../custom/config/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__CUSTOMER_CODE__ -D__OCPU_COMPILER_GCC__ -I"${GCC_PATH}\arm-none-eabi\include" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\include" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\ril\inc" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\config" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\lstr25\INC" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\lstr25\SRC" -I"E:\Work_proj\LSTR25_Use\OpenCPU_SDK\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


