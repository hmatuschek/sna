# Specify programmer
set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER stk500v2)
set(AVR_UPLOADTOOL_PORT /dev/tty.usbmodem1411)

# Specify CPU
set(AVR_MCU atmega328p)

# Specify fuse magic
set(AVR_L_FUSE 0xf7)
set(AVR_H_FUSE 0xdf)

# Specify CPU frequency
set(MCU_SPEED "16000000UL")

# Include avg-gcc toolchain
include(generic-gcc-avr)

# Typical options
add_definitions("-DF_CPU=${MCU_SPEED}")
#add_definitions("-fpack-struct")
add_definitions("-fshort-enums")
add_definitions("-Wall")
add_definitions("-Werror")
add_definitions("-pedantic")
add_definitions("-pedantic-errors")
#add_definitions("-funsigned-char")
#add_definitions("-funsigned-bitfields")
#add_definitions("-ffunction-sections")
add_definitions("-c")
add_definitions("-std=gnu99")
add_definitions("-Os")

##########################################################################
# Firmware source
##########################################################################
set(firmware_SOURCES main.c uart.c adc.c ad9850.c)

# compile & link
add_avr_executable(firmware ${firmware_SOURCES})
avr_target_link_libraries(firmware ${AVR_C_LIB})
