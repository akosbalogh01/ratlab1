################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/benchmarking.c \
../src/capture.c \
../src/display.c \
../src/imageProcessor.c \
../src/main.c \
../src/vdma.c 

OBJS += \
./src/benchmarking.o \
./src/capture.o \
./src/display.o \
./src/imageProcessor.o \
./src/main.o \
./src/vdma.o 

C_DEPS += \
./src/benchmarking.d \
./src/capture.d \
./src/display.d \
./src/imageProcessor.d \
./src/main.d \
./src/vdma.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O3 -std=c99 -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -g3 -I../../bsp/ps7_cortexa9_0/include -I"D:\BME\Doktori\Oktatas\2019F2\LaborMSC_FPGA\LaborForras\Workspace\Meres6\firmware\application\inc" -c -fmessage-length=0 -MT"$@" -I../../bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


