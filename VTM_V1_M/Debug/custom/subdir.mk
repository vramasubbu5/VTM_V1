################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/main.c 

OBJS += \
./custom/main.o 

C_DEPS += \
./custom/main.d 


# Each subdirectory must supply rules for building sources it contributes
custom/%.o: ../custom/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__CUSTOMER_CODE__ -I"C:\Program Files\CodeSourcery\Sourcery_CodeBench_Lite_for_ARM_EABI\arm-none-eabi\include" -I"E:\D-drive\soft\Quectel\M66 Open CPU\M66_OpenCPU_GS3_SDK_V1.6_Eclipse\include" -I"E:\D-drive\soft\Quectel\M66 Open CPU\M66_OpenCPU_GS3_SDK_V1.6_Eclipse\ril\inc" -I"E:\D-drive\soft\Quectel\M66 Open CPU\M66_OpenCPU_GS3_SDK_V1.6_Eclipse\custom\config" -I"E:\D-drive\soft\Quectel\M66 Open CPU\M66_OpenCPU_GS3_SDK_V1.6_Eclipse\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


