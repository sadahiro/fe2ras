################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/fe2ras.cpp \
../src/feOption.cpp \
../src/feParse.cpp \
../src/feZone.cpp 

OBJS += \
./src/fe2ras.o \
./src/feOption.o \
./src/feParse.o \
./src/feZone.o 

CPP_DEPS += \
./src/fe2ras.d \
./src/feOption.d \
./src/feParse.d \
./src/feZone.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


