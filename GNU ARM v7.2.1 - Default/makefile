################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include protocol/bluetooth/lib/EFR32BG13P/GCC/subdir.mk
-include platform/service/sleeptimer/src/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/subdir.mk
-include hardware/kit/common/drivers/subdir.mk
-include hardware/kit/common/bsp/subdir.mk
-include app/bluetooth/common/util/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: soc-empty.axf

# Tool invocations
soc-empty.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\efr32bg13p732f512gm48.ld" -Xlinker --gc-sections -Xlinker -Map="soc-empty.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o soc-empty.axf -Wl,--start-group "./accel.o" "./app.o" "./application_properties.o" "./ble.o" "./gatt_db.o" "./gecko_ble_errors.o" "./gpio.o" "./i2c.o" "./init_app.o" "./init_board_efr32xg1.o" "./init_mcu_efr32xg1.o" "./irq.o" "./log.o" "./main.o" "./oscillators.o" "./scheduler.o" "./timer.o" "./app/bluetooth/common/util/infrastructure.o" "./hardware/kit/common/bsp/bsp_stk.o" "./hardware/kit/common/drivers/i2cspm.o" "./hardware/kit/common/drivers/retargetio.o" "./hardware/kit/common/drivers/retargetserial.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/startup_efr32bg13p.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/system_efr32bg13p.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emlib/src/em_assert.o" "./platform/emlib/src/em_burtc.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_eusart.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_letimer.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_se.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "./platform/service/sleeptimer/src/sl_sleeptimer.o" "./platform/service/sleeptimer/src/sl_sleeptimer_hal_prortc.o" "./platform/service/sleeptimer/src/sl_sleeptimer_hal_rtcc.o" "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\protocol\bluetooth\lib\EFR32BG13P\GCC\libpsstore.a" "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\protocol\bluetooth\lib\EFR32BG13P\GCC\binapploader.o" "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\protocol\bluetooth\lib\EFR32BG13P\GCC\libmbedtls.a" "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\protocol\bluetooth\lib\EFR32BG13P\GCC\libbluetooth.a" "C:\Users\bryan\SimplicityStudio\v4_workspace\soc-empty\platform\radio\rail_lib\autogen\librail_release\librail_efr32xg13_gcc_release.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: soc-empty.hex'
	arm-none-eabi-objcopy -O ihex "soc-empty.axf" "soc-empty.hex"
	@echo ' '

	@echo 'Building bin file: soc-empty.bin'
	arm-none-eabi-objcopy -O binary "soc-empty.axf" "soc-empty.bin"
	@echo ' '

	@echo 'Building s37 file: soc-empty.s37'
	arm-none-eabi-objcopy -O srec "soc-empty.axf" "soc-empty.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "soc-empty.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) soc-empty.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
