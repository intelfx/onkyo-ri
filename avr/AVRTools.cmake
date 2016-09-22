#
# This file is part of onkyo-ri.
#
# Copyright 2016 Ivan Shapovalov <intelfx@intelfx.name>
#
# onkyo-ri is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 3
# as published by the Free Software Foundation.
#
# onkyo-ri is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with onkyo-ri.  If not, see <http://www.gnu.org/licenses/>.
#

#
# This file contains certain ancillary CMake functions useful in AVR projects
# (defining custom targets to flash the firmware and fuse bits).
#

find_program(AVRDUDE avrdude DOC "Path to avrdude tool")
if(NOT AVRDUDE)
	message(FATAL_ERROR "avrdude not found")
endif()

set(AVRDUDE_PROGRAMMER "usbtiny" CACHE STRING "Programmer name for the avrdude tool")
set(AVRDUDE_MCU "${MCU}" CACHE STRING "AVR part name for the avrdude tool")
set(AVRDUDE_ARGS "" CACHE STRING "Extra arguments for the avrdude tool")

set(AVRDUDE_CMDLINE "${AVRDUDE}" -c "${AVRDUDE_PROGRAMMER}" -p "${AVRDUDE_MCU}" ${AVRDUDE_ARGS})

set(AVR_LFUSE "" CACHE STRING "AVR low fuse byte, blank to ignore")
set(AVR_HFUSE "" CACHE STRING "AVR high fuse byte, blank to ignore")
set(AVR_EFUSE "" CACHE STRING "AVR extended fuse byte, blank to ignore")

function(avr_set_fuses)
	add_custom_target(getfuse
	                  ${CMAKE_CURRENT_LIST_DIR}/parse_fuses.sh ${AVRDUDE_CMDLINE}
	                  VERBATIM)

	set(avrdude_fuse_cmdline)

	if(AVR_LFUSE)
		list(APPEND avrdude_fuse_cmdline -U lfuse:w:${AVR_LFUSE}:m)
	endif()

	if(AVR_HFUSE)
		list(APPEND avrdude_fuse_cmdline -U hfuse:w:${AVR_HFUSE}:m)
	endif()

	if(AVR_EFUSE)
		list(APPEND avrdude_fuse_cmdline -U efuse:w:${AVR_EFUSE}:m)
	endif()

	add_custom_target(fuse
	                  ${AVRDUDE_CMDLINE} ${avrdude_fuse_cmdline}
	                  VERBATIM USES_TERMINAL)
endfunction()

function(avr_flash target)
	add_custom_command(OUTPUT ${target}.bin
	                   COMMAND ${CMAKE_OBJCOPY} -j .text -j .data
	                                            -O binary
	                                            $<TARGET_FILE:${target}>
	                                            ${target}.bin
	                   DEPENDS ${target}
	                   VERBATIM)
	add_custom_command(OUTPUT ${target}.eep.bin
	                   COMMAND ${CMAKE_OBJCOPY} -j .eeprom
	                                            --change-section-lma .eeprom=0
	                                            -O binary
	                                            $<TARGET_FILE:${target}>
	                                            ${target}.eep.bin
	                   DEPENDS ${target}
	                   VERBATIM)
	add_custom_target(flash
	                  COMMAND ${AVRDUDE_CMDLINE} -e
	                                             -U flash:w:$<TARGET_FILE:${target}>.bin:r
	                                             -U eeprom:w:$<TARGET_FILE:${target}>.eep.bin:r
	                  DEPENDS ${target}.bin ${target}.eep.bin
	                  VERBATIM USES_TERMINAL)
endfunction()
