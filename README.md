# Scalar Network Analyzer

A [scalar network analyzer](https://en.wikipedia.org/wiki/Network_analyzer_%28electrical%29) (SNA) measures the amplitude of an output signal of a circuit as a function of the input frequency.

This project provides a circuit, firmware and software to implement a very simple and cheap SNA. It uses a [AD9850 DDS module](http://www.minikits.com.au/electronic-kits/dds-synthesizer/basic-dds/AD9850-DDS-01) as the programmable signal generator and a AD8307 logarithmic amplifier in conjecture with the ADC of a ATMEGA 168 to record the output amplitude. The SNA hardware is interfaced via the common RS-232 using a MAX232 level converter. The firmware for the ATMEGA 168 can be found in the firmware directory.

The client software then provides a simple means of performing the measurements and displays the results.

## License
SNA - A simple scalar network analyzer. 

(c) 2015 Hannes Matuschek <hmatuschek at gmail dot com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.</p>

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.</p>

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
