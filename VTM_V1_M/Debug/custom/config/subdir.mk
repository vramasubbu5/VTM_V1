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
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__CUSTOMER_CODE__ -I"C:\Program Files (x86)\CodeSourcery\Sourcery_CodeBench_Lite_for_ARM_EABI\arm-none-eabi\include" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\include" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\ril\inc" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\custom\config" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\custom\fota\inc" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\custom\lstr25\INC" -I"D:\Work_space\Eclipse\vtm_soft\VTM_V1_M\custom\lstr25\SRC" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


