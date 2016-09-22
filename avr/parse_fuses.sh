#!/bin/bash

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
# This file parses and pretty-prints avrdude(1)'s fuse bits output.
#

readarray -t REPLY < <( \
	"$@" -qq \
	-U lfuse:r:/dev/stdout:h \
	-U lfuse:r:/dev/stdout:b \
	-U hfuse:r:/dev/stdout:h \
	-U hfuse:r:/dev/stdout:b \
	-U efuse:r:/dev/stdout:h \
	-U efuse:r:/dev/stdout:b)

echo "Fuses: L: ${REPLY[0]} (${REPLY[1]})"
echo "       H: ${REPLY[2]} (${REPLY[3]})"
echo "       E: ${REPLY[4]} (${REPLY[5]})"
