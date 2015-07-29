#include "sna.hh"
#include "logger.hh"
#include <limits>

SNA::SNA(const QString &portname, double Fosc, double ppm, QObject *parent)
  : QObject(parent), _settings("com.github.hmatuschek", "sna"), _port(portname), _Fosc(Fosc), _ppm(ppm)
{
  // Open port
  _port.open(QIODevice::ReadWrite);
  if (! _port.isOpen()) {
    LogMessage msg(LOG_ERROR);
    msg << "IO Error: Can not open device " << portname.toStdString();
    Logger::get().log(msg);
    return;
  }

  if (! _port.setBaudRate(QSerialPort::Baud38400)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set baudrate.";
    Logger::get().log(msg);
    return;
  }
  if (! _port.setDataBits(QSerialPort::Data8)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set data bits.";
    Logger::get().log(msg);
    return;
  }
  if (! _port.setParity(QSerialPort::NoParity)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set parity.";
    Logger::get().log(msg);
    return;
  }
  if (! _port.setStopBits(QSerialPort::OneStop)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set stop bits.";
    Logger::get().log(msg);
    return;
  }
  if (! _port.setFlowControl(QSerialPort::HardwareControl)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set stop bits.";
    Logger::get().log(msg);
    return;
  }
}

bool
SNA::_send(const uint8_t *tx, size_t txlen, uint8_t *rx, size_t rxlen) {
  // send data
  while (txlen) {
    int n = _port.write((const char *)tx, txlen);
    if (n<0) {
      LogMessage msg(LOG_ERROR);
      msg << "IO: Can not send command.";
      Logger::get().log(msg);
      return false;
    }
    tx += n; txlen -= n;
  }
  if (! _port.waitForReadyRead(1000)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Timeout.";
    Logger::get().log(msg);
    return false;
  }
  uint8_t res;
  if (1 != _port.read((char *)&res, 1)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not read response.";
    Logger::get().log(msg);
    return false;
  }
  if (0x00 != res) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Device returned error 0x" << std::hex << int(uint16_t(res)) <<".";
    Logger::get().log(msg);
    return false;
  }
  // read data
  while (rxlen) {
    if (! _port.waitForReadyRead(1000)) {
      LogMessage msg(LOG_ERROR);
      msg << "IO: Timeout.";
      Logger::get().log(msg);
      return false;
    }
    int n = _port.read((char *)rx, rxlen);
    if (n<0) {
      LogMessage msg(LOG_ERROR);
      msg << "IO: Can not read response.";
      Logger::get().log(msg);
      return false;
    }
    rx += n; rxlen -= n;
  }
  return true;
}

double
SNA::Fosc() const {
  return _Fosc;
}

double
SNA::value() {
  uint8_t tx[1] = {0x01}, rx[2];
  // send command
  if (! _send(tx, 1, rx, 2)) {
    return std::numeric_limits<double>::signaling_NaN();
  }
  // convert to voltage
  double v = 2.5*double((uint16_t(rx[0])<<8) + rx[1])/0xffff;
  // Compute dBm
  return 16. - (2.5-v)/25e-3;
}

bool
SNA::setFrequency(double f) {
  uint32_t n = (0xffffffff * f * (1+_ppm/1e6) / _Fosc);
  uint8_t tx[5] = { 0x02, uint8_t(n>>24), uint8_t(n>>16), uint8_t(n>>8), uint8_t(n) };
  return _send(tx, 5, 0, 0);
}

double
SNA::valueAt(double f) {
  if (! setFrequency(f)) {
    return std::numeric_limits<double>::signaling_NaN();
  }
  return value();
}

QSettings &
SNA::settings() {
  return _settings;
}
