################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 3
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
timer_toggle_led_delay.elf \

EXECUTABLES_QUOTED += \
"timer_toggle_led_delay.elf" \

EXECUTABLES_OS_FORMAT += \
timer_toggle_led_delay.elf \

SECONDARY_SIZE += \
timer_toggle_led_delay.siz \

SECONDARY_SIZE_QUOTED += \
"timer_toggle_led_delay.siz" \

SECONDARY_SIZE_OS_FORMAT += \
timer_toggle_led_delay.siz \


# All Target
all: warning-messages explicit-dependencies timer_toggle_led_delay.elf secondary-outputs

# Tool invocations
timer_toggle_led_delay.elf: $(OBJS) D:/Documentos/SemestresTec/Semestre_i/Microcontroladores/NXP-S32K144/timer_toggle_led_delay/Project_Settings/Linker_Files/SKEAZ_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #4 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "timer_toggle_led_delay.elf" "@timer_toggle_led_delay.args"  $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

timer_toggle_led_delay.siz: $(EXECUTABLES)
	@echo 'Executing target #5 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*/*.d ./*/*/*.d  ./*/*.o ./*/*/*.o  $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

