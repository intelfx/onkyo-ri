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
# This file is a simple CMake toolchain file for the AVR platform (avr-gcc).
#

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_SYSROOT /usr/avr)
set(CMAKE_C_COMPILER /usr/bin/avr-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/avr-g++)
set(CMAKE_AR /usr/bin/avr-gcc-ar)
set(CMAKE_RANLIB /usr/bin/avr-gcc-ranlib)
set(CMAKE_NM /usr/bin/avr-gcc-nm)

set(MCU "attiny85" CACHE STRING "AVR core/part")
set(F_CPU "16500000" CACHE STRING "AVR core frequency in Hz")

set(CMAKE_ASM_FLAGS "" CACHE STRING "Assembly flags")
set(CMAKE_C_FLAGS "" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "" CACHE STRING "CXX flags")
set(CMAKE_ASM_FLAGS_DEBUG "-Os -DDEBUG" CACHE STRING "Assembly flags - Debug")
set(CMAKE_C_FLAGS_DEBUG "-Os -DDEBUG" CACHE STRING "C flags - Debug")
set(CMAKE_CXX_FLAGS_DEBUG "-Os -DDEBUG" CACHE STRING "C++ flags - Debug")
set(CMAKE_ASM_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "Assembly flags - Release")
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "C flags - Release")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "C++ flags - Release")
set(CMAKE_MODULE_LINKER_FLAGS "" CACHE STRING "Module link flags")
set(CMAKE_STATIC_LINKER_FLAGS "" CACHE STRING "Static library link flags")
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "Shared library link flags")
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "Executable link flags")
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "Shared library link flags")

#
# Language features and optimizations
#
set(AVR_COM_FLAGS "${AVR_COM_FLAGS} -fpack-struct -fshort-enums -funsigned-bitfields -funsigned-char")
set(AVR_CXX_FLAGS "${AVR_CXX_FLAGS} -fno-rtti -fno-exceptions")
set(AVR_CXX_FLAGS "${AVR_CXX_FLAGS} -fdevirtualize -fdevirtualize-speculatively -fdevirtualize-at-ltrans")
set(AVR_LINK_FLAGS "${AVR_LINK_FLAGS} -Wl,-O1,--relax")

#
# MCU
#
set(AVR_COM_FLAGS "${AVR_COM_FLAGS} -mmcu=${MCU} -DF_CPU=${F_CPU}")

#
# Function-level linking
#
set(AVR_COM_FLAGS "${AVR_COM_FLAGS} -ffunction-sections -fdata-sections")
set(AVR_LINK_FLAGS "${AVR_LINK_FLAGS} -Wl,--gc-sections")

#
# LTO
#
set(AVR_COM_FLAGS "${AVR_COM_FLAGS} -flto -fuse-linker-plugin")
set(AVR_LINK_FLAGS "${AVR_LINK_FLAGS} -flto -fuse-linker-plugin")

#
# Finalize
#
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${AVR_COM_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${AVR_COM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AVR_COM_FLAGS} ${AVR_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_LINK_FLAGS}")

#
# find_*()
#
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
