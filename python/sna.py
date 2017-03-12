import serial
import struct


class SNA:
  """ Implements a simple interface to my Scalar Network Analyzer for python.

      The interface is quiet simple: call setFrequency(F) to set the output frequency to the
      specified frequency in Hz. Then, call getValue() to get an power reading from the input
      in dBm (dB milli Watts). """

  def __init__(self, port, vRef=2.5, Fosc=125e6, ppm=0):
    """ Constructs a new instance to interface the SNA connected to the specified serial port.
    The optional vRef argument specifies the reference voltage for the ADC (2.5V by defaut).
    The optional Fosc argument specifies the reference oscillator frequency of the DDS (125MHz by
    default).
    The optional ppm argument specifies the frequency offset of the DDS oscillator in parts per
    million. """
    self._port = port;
    self._vRef = vRef;
    self._Fosc = Fosc;
    self._ppm = ppm;

  def __del__(self):
    """ Closes the port upon destruction. """
    self._port.close();

  def setFrequency(self, double F):
    """ Sets the DDS output frequency (in Hz). """
    n = int((0xffffffff * F * (1.0+self._ppm/1.0e6) / self._Fosc));
    buffer = struct.pack(">BI", 0x02, n);
    if (4 != self._port.write(buffer)):
      pass
    (ret,) = struct.unpack(">B", self._port.read(1));
    return (0 == ret)

  def getValue(self):
    """ Requests and returns a power reading in dBm. """
    buffer = struct.pack(">B", 0x01);
    if (1 != self._port.write(buffer)):
      pass
    (ret, value) = struct.unpack(">BS", self._port.read(3));
    return ( (self._vRef * value)/0xffff/25e-3 - 84. );
