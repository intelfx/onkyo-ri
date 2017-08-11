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
# This file is the main build script of the RI decoder firmware.
#

project(OnkyoRI_AVR C)
cmake_minimum_required(VERSION 3.2)

include(AVRTools.cmake)

set(UART_BAUD "9600" CACHE STRING "Baud rate for the on-chip UART engine")

add_executable(main
	main.c
	timer_counter.c
	uart.c
	util.c
	event.c
)

target_compile_options(main PRIVATE
	-fwrapv
)
target_compile_definitions(main PRIVATE
	-DUART_BAUD=${UART_BAUD}
)
target_link_libraries(main PRIVATE
	-Wl,-u,vfprintf -lprintf_min
)

avr_flash(main)
avr_set_fuses()
