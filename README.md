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


Motivation
----------

The overall motivation to use Onkyo RI instead of directly pointing the IR remote
at the HTPC is to leverage the (potentially better) existing IR receiver instead
of buying another one (which is probably worse in terms of sensitivity).
For example, my Onkyo AV receiver is capable of successfully decoding IR signals
reflected by the walls or furniture or sent from another room (pointing the
remote at an open door).

Thus, the easiest way to send/receive Onkyo RI commands with a PC is to use a
GPIO line and wire it up to the AV receiver, then point LIRC at that GPIO line
(assuming Linux kernel knows how to operate the GPIO). However, typical x86
systems do not have ready-to-be-accessed GPIOs, so we need a USB-GPIO controller —
with the easiest example being any of the FTDI chips, like the FT232RL. Actually,
LIRC does even have a driver for them.

There is just a single problem with FTDI chips: they lack interrupts. They
require the CPU to poll them for samples (while the limited internal sample buffer
only helps so much). Yes, I know that USB does not have real interrupts, but I'd
rather poll with the host-controller and not with the CPU. Essentially, what I
wanted is a USB-GPIO controller with edge-triggered interrupts, and here we go.


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
