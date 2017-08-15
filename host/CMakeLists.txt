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
# This file is the main build script of the LIRC-compatible transceiver drivers.
#

project(OnkyoRI_LIRC C)
cmake_minimum_required(VERSION 3.2)

set(UART_BAUD "9600" CACHE STRING "Baud rate for the tty device (must match firmware)")

#
# Find the lirc driver API library.
# We try to be good cmake-i-zen and leverage as much infrastructure as possible.
# However...
#

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIRC REQUIRED IMPORTED_TARGET lirc-driver)
# fixup the target -- lirc buildsystem engineers are morons
# 1. -DIN_DRIVER
set_property(TARGET PkgConfig::LIRC APPEND
	PROPERTY INTERFACE_COMPILE_DEFINITIONS IN_DRIVER
)
# 2. -I<path/to/config.h>
find_file(LIRC_CONFIG_H lirc/config.h
	HINTS ${LIRC_INCLUDEDIR} ${LIRC_INCLUDE_DIRS}
	NO_DEFAULT_PATH
)
get_filename_component(
	LIRC_CONFIG_H ${LIRC_CONFIG_H} DIRECTORY
)
set_property(TARGET PkgConfig::LIRC APPEND
	PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LIRC_CONFIG_H}
)
# 3. -llirc
find_library(LIRC_LIB lirc
	HINTS ${LIRC_LIBDIR} ${LIRC_LIBRARY_DIRS}
	NO_DEFAULT_PATH
)
set_property(TARGET PkgConfig::LIRC APPEND
	PROPERTY INTERFACE_LINK_LIBRARIES ${LIRC_LIB}
)

#
# Build the driver.
#

add_library(lirccode_uart SHARED
	main.c
	vec.c
	timespec.c
)
target_compile_definitions(lirccode_uart PRIVATE
	-DUART_BAUD=${UART_BAUD}
)
target_link_libraries(lirccode_uart PRIVATE
	PkgConfig::LIRC
)

#
# Install the driver to the designated directory.
#

pkg_get_variable(LIRC_PLUGIN_DIR lirc-driver plugindir)
install(TARGETS lirccode_uart
	DESTINATION ${LIRC_PLUGIN_DIR}
)
