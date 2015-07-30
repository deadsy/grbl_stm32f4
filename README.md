# grbl_stm32f4

## Purpose

This a port of the grbl 0.8c g-code interpreter to the STM32F4 SoC

The STM32F4 SoC has attractive features for a realtime G-Code interpreter and machine controller.

* Lot's of IO - it's possible to run more stepper motors, more limit switches, etc.
* A Floating Point Unit - good for doing the linear/circular interpolation math.
* Fast (168MHz 32-bit versus 16MHz 8-bit for the atmega328p).

## Target Platform

STM32F4Discovery Board
* http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419
* http://www.digikey.com/short/504hf

## Build tools:

GCC Toolchain
* https://launchpad.net/gcc-arm-embedded
* [gcc-arm-none-eabi-4_9-2015q2] (https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q2-update/+download/gcc-arm-none-eabi-4_9-2015q2-20150609-linux.tar.bz2)

STLink Programming Tool
* https://github.com/texane/stlink

## Target CNC Machine
This code is a straight port of grbl and is adaptable to many stepper drivers and CNC machines.

My test and development has used:

Gecko Drives G540 4-Axis Digital Step Drive
* http://www.geckodrive.com/geckodrive-step-motor-drives/g540.html

KRMx01 CNC Router 
* http://www.kronosrobotics.com/krmx01/

## Additions

* Support for XYZY axes operation has been added. ie- Two stepper motors, controlled in the same way, are used to drive the Y-axis.
* All input signals (homing switches, e-stop, etc.) are debounced in software.

## Status

* Non-volatile EEPROM storage of configuration parameters is not currently supported. The EEPROM facility is being faked with RAM based storage. The desired machine parameters should be compiled into the firmware.
* The CDC USB serial device implemented on the board is not as reliable as it needs to be. Pending bug fixes the option of a normal serial port using the USART hardware has been implemented. This can be enabled in the makefile by setting "USB_SERIAL = 0".
