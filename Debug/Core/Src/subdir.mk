################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ELTEC_EmulatedEEPROM.c \
../Core/Src/ModbusMap.c \
../Core/Src/ModbusRTU.c \
../Core/Src/bluetooth.c \
../Core/Src/bootloader.c \
../Core/Src/commonMainAsm.c \
../Core/Src/common_subs.c \
../Core/Src/comunicacion.c \
../Core/Src/display.c \
../Core/Src/inicio.c \
../Core/Src/logger.c \
../Core/Src/main.c \
../Core/Src/muestreo.c \
../Core/Src/noctar.c \
../Core/Src/refrigera.c \
../Core/Src/stm32u0xx_hal_msp.c \
../Core/Src/stm32u0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u0xx.c \
../Core/Src/temper.c \
../Core/Src/tiempo.c \
../Core/Src/tx_buffer_prep.c \
../Core/Src/tx_control.c \
../Core/Src/voltmetro.c \
../Core/Src/watch.c 

OBJS += \
./Core/Src/ELTEC_EmulatedEEPROM.o \
./Core/Src/ModbusMap.o \
./Core/Src/ModbusRTU.o \
./Core/Src/bluetooth.o \
./Core/Src/bootloader.o \
./Core/Src/commonMainAsm.o \
./Core/Src/common_subs.o \
./Core/Src/comunicacion.o \
./Core/Src/display.o \
./Core/Src/inicio.o \
./Core/Src/logger.o \
./Core/Src/main.o \
./Core/Src/muestreo.o \
./Core/Src/noctar.o \
./Core/Src/refrigera.o \
./Core/Src/stm32u0xx_hal_msp.o \
./Core/Src/stm32u0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u0xx.o \
./Core/Src/temper.o \
./Core/Src/tiempo.o \
./Core/Src/tx_buffer_prep.o \
./Core/Src/tx_control.o \
./Core/Src/voltmetro.o \
./Core/Src/watch.o 

C_DEPS += \
./Core/Src/ELTEC_EmulatedEEPROM.d \
./Core/Src/ModbusMap.d \
./Core/Src/ModbusRTU.d \
./Core/Src/bluetooth.d \
./Core/Src/bootloader.d \
./Core/Src/commonMainAsm.d \
./Core/Src/common_subs.d \
./Core/Src/comunicacion.d \
./Core/Src/display.d \
./Core/Src/inicio.d \
./Core/Src/logger.d \
./Core/Src/main.d \
./Core/Src/muestreo.d \
./Core/Src/noctar.d \
./Core/Src/refrigera.d \
./Core/Src/stm32u0xx_hal_msp.d \
./Core/Src/stm32u0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u0xx.d \
./Core/Src/temper.d \
./Core/Src/tiempo.d \
./Core/Src/tx_buffer_prep.d \
./Core/Src/tx_control.d \
./Core/Src/voltmetro.d \
./Core/Src/watch.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ELTEC_EmulatedEEPROM.cyclo ./Core/Src/ELTEC_EmulatedEEPROM.d ./Core/Src/ELTEC_EmulatedEEPROM.o ./Core/Src/ELTEC_EmulatedEEPROM.su ./Core/Src/ModbusMap.cyclo ./Core/Src/ModbusMap.d ./Core/Src/ModbusMap.o ./Core/Src/ModbusMap.su ./Core/Src/ModbusRTU.cyclo ./Core/Src/ModbusRTU.d ./Core/Src/ModbusRTU.o ./Core/Src/ModbusRTU.su ./Core/Src/bluetooth.cyclo ./Core/Src/bluetooth.d ./Core/Src/bluetooth.o ./Core/Src/bluetooth.su ./Core/Src/bootloader.cyclo ./Core/Src/bootloader.d ./Core/Src/bootloader.o ./Core/Src/bootloader.su ./Core/Src/commonMainAsm.cyclo ./Core/Src/commonMainAsm.d ./Core/Src/commonMainAsm.o ./Core/Src/commonMainAsm.su ./Core/Src/common_subs.cyclo ./Core/Src/common_subs.d ./Core/Src/common_subs.o ./Core/Src/common_subs.su ./Core/Src/comunicacion.cyclo ./Core/Src/comunicacion.d ./Core/Src/comunicacion.o ./Core/Src/comunicacion.su ./Core/Src/display.cyclo ./Core/Src/display.d ./Core/Src/display.o ./Core/Src/display.su ./Core/Src/inicio.cyclo ./Core/Src/inicio.d ./Core/Src/inicio.o ./Core/Src/inicio.su ./Core/Src/logger.cyclo ./Core/Src/logger.d ./Core/Src/logger.o ./Core/Src/logger.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/muestreo.cyclo ./Core/Src/muestreo.d ./Core/Src/muestreo.o ./Core/Src/muestreo.su ./Core/Src/noctar.cyclo ./Core/Src/noctar.d ./Core/Src/noctar.o ./Core/Src/noctar.su ./Core/Src/refrigera.cyclo ./Core/Src/refrigera.d ./Core/Src/refrigera.o ./Core/Src/refrigera.su ./Core/Src/stm32u0xx_hal_msp.cyclo ./Core/Src/stm32u0xx_hal_msp.d ./Core/Src/stm32u0xx_hal_msp.o ./Core/Src/stm32u0xx_hal_msp.su ./Core/Src/stm32u0xx_it.cyclo ./Core/Src/stm32u0xx_it.d ./Core/Src/stm32u0xx_it.o ./Core/Src/stm32u0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u0xx.cyclo ./Core/Src/system_stm32u0xx.d ./Core/Src/system_stm32u0xx.o ./Core/Src/system_stm32u0xx.su ./Core/Src/temper.cyclo ./Core/Src/temper.d ./Core/Src/temper.o ./Core/Src/temper.su ./Core/Src/tiempo.cyclo ./Core/Src/tiempo.d ./Core/Src/tiempo.o ./Core/Src/tiempo.su ./Core/Src/tx_buffer_prep.cyclo ./Core/Src/tx_buffer_prep.d ./Core/Src/tx_buffer_prep.o ./Core/Src/tx_buffer_prep.su ./Core/Src/tx_control.cyclo ./Core/Src/tx_control.d ./Core/Src/tx_control.o ./Core/Src/tx_control.su ./Core/Src/voltmetro.cyclo ./Core/Src/voltmetro.d ./Core/Src/voltmetro.o ./Core/Src/voltmetro.su ./Core/Src/watch.cyclo ./Core/Src/watch.d ./Core/Src/watch.o ./Core/Src/watch.su

.PHONY: clean-Core-2f-Src

