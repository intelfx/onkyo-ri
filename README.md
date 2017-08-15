Onkyo RI transceiver
====================

[Onkyo RI][1] is a simple vendor-specific wired remote control protocol,
implemented in many home entertainment devices by Onkyo (e. g. AV receivers, DVD
players etc). It is mainly used to pass user remote control commands to the
devices that cannot be reached directly by IR link (e. g. located in a different
room).

[1]: http://fredboboss.free.fr/articles/onkyo_ri.php

![Timing diagram of the RI protocol](http://fredboboss.free.fr/images/ri_pulses.png)

This project implements an AVR-based Onkyo RI transceiver firmware and a simple
host-side driver in form of a LIRC plugin, allowing to communicate with
RI-enabled devices from a PC. This includes:
* controlling auxiliary devices, e. g. turning on the receiver from a HTPC box;
* listening to the user's remote control without duplicating the IR sensor and
  demodulator.


Roadmap
-------

This project aims to implement two kinds of transceiver firmware and corresponding
drivers:

- [x] LIRCCODE-style transceiver, where all de-/encoding happens on the device
  and the driver only receives/sends RI button codes;
- [ ] Fully generic pulse transceiver, aka "USB-GPIO with edge-triggered interrupts",
  where the device only registers pulses and exports the timing information
  and the rest is done by LIRC.

Orthogonal to that, the project aims to support communicating with the transceiver
device via two different interfaces:

- [x] UART (via UART-USB), as found on the
  [Arduino Nano](http://arduino.ru/Hardware/ArduinoBoardNano);
- [ ] native USB (via V-USB running on the device) using e. g. the
  [Digispark](http://digistump.com/products/1) board.

The end goal is to upstream the two non-LIRCCODE drivers and persuade LIRC
developers to link to this project under the "Supported Hardware / Home-brew"
section.


License
-------

This project is distributed under the terms of a [GNU General Public License
(GPL), version 3][4].

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 3
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

[4]: https://www.gnu.org/licenses/gpl-3.0.en.html
