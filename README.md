# Scalar Network Analyzer

A [scalar network analyzer](https://en.wikipedia.org/wiki/Network_analyzer_%28electrical%29) (SNA) measures the amplitude of an output signal of a circuit as a function of the input frequency.

This project provides the circuit, firmware and software to implement a very simple and cheap SNA. It uses a [AD9850 DDS module](http://www.minikits.com.au/electronic-kits/dds-synthesizer/basic-dds/AD9850-DDS-01) as the programmable signal generator and a AD8307 logarithmic amplifier in conjecture with the ADC of an ATMEGA 168 (or the pin compatible ATMEGA 8/48/88/328) to record the input amplitude. The SNA hardware is interfaced via the common RS-232 using a MAX232 level converter. The firmware for the ATMEGA 168 can be found in the firmware directory.

[![screenshot](http://i57.tinypic.com/15fobcp.png)](http://i57.tinypic.com/15fobcp.png)

The client software then provides a simple means of performing the measurements and displays the results. It provides two operating modes. The <emph>poll</emph> mode outputs a fixed frequency and measures the input amplitude while the <emph>scan</emph> mode records the input amplitude as a function of the output frequency. The result is then plotted (see above). 


## Hardware
As mentioned above, the [circuit](https://github.com/hmatuschek/sna/raw/master/doc/compact_scm.pdf) consists of an AD9850 DDS module (visible below) an ATMEGA 168 (partially covered in the picture) to program the DDS and to measure the output voltage of the logarithmic amplifier (AD8307, visible in the top-left corner). The logarithmic amplifier converts the input power over a 51 Ohms resistor into a voltage (0-2.5V) which is a linear function of the input power in dBm. 

[![the prototype](http://i60.tinypic.com/16k17go.jpg)](http://i60.tinypic.com/16k17go.jpg)

The AD8307 can handle up to 17dBm (50mW). The output power of the DDS, however, is only about 0.1mW (-10dBm). To enhence the dynamic range of the SNA, a buffer amplifier (two 2N3904, visible at the bottom-right corner) is used to increase the output to about 1-3mW (0-5dBm).  

The SNA is interfaced through the UART (RS-232) of the ATMEGA using a MAX232 level converter (visible in the bottom-left corner). In fact, the hardware is ARDUINO UNO compatible. By flashing the ARDUINO bootloader onto the ATMEGA, the SNA can be programmed easily via the RS-232 interface. 

The [board](https://github.com/hmatuschek/sna/raw/master/doc/compact_brd.pdf) is routed such that it is possible to assemble the circuit on a perfboard without bridges. The board dimensions are kept small. The complete circuit fits into a relatively small TEKO 2/A chassis (57 x 72 x 28 mm). For a better performance, the distance between the AD8307 logarithmic amplifier and the rest of the circuit should be increased and the AD8307 should also be shielded. Due to the lack of shielding, my prototype has a relatively large noise level of about -40dBm.


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
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
