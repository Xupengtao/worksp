################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SataIO.cpp \
../src/SpiCtrl.cpp \
../src/main.cpp 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/SataIO.o \
./src/SpiCtrl.o \
./src/main.o 

CPP_DEPS += \
./src/SataIO.d \
./src/SpiCtrl.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo Building file: $<
	@echo Invoking: MicroBlaze g++ compiler
	mb-g++ -Wall -O0 -g3 -c -fmessage-length=0 -Wl,--no-relax -I../../MicroFileSys_bsp/microblaze_0/include -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.40.b -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


