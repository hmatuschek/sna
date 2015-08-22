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

  if (! _port.setBaudRate(QSerialPort::Baud9600)) {
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
  if (! _port.setFlowControl(QSerialPort::NoFlowControl)) {
    LogMessage msg(LOG_ERROR);
    msg << "IO: Can not set stop bits.";
    Logger::get().log(msg);
    return;
  }

  QObject::connect(&_port, SIGNAL(bytesWritten(qint64)), this, SLOT(_onBytesWritten(qint64)));
  QObject::connect(&_port, SIGNAL(readyRead()), this, SLOT(_onReadyRead()));
}


double
SNA::Fosc() const {
  return _Fosc;
}

void
SNA::sendGetValue() {
  uint8_t tx[1] = {0x01};
  _mode = GET_VALUE;
  _port.write((char *)tx, 1);
}

void
SNA::sendSetFrequency(double f) {
  uint32_t n = (0xffffffff * f * (1+_ppm/1e6) / _Fosc);
  uint8_t tx[5] = { 0x02, uint8_t(n>>24), uint8_t(n>>16), uint8_t(n>>8), uint8_t(n) };
  _mode = SET_FREQUENCY;
  _port.write((char *)tx, 5);
}

void
SNA::sendShutdown() {
  uint8_t tx[1] = { 0x04 };
  _mode = SHUTDOWN;
  _port.write((char *)tx, 1);
}

QSettings &
SNA::settings() {
  return _settings;
}

void
SNA::_onBytesWritten(qint64 n) {
  // pass...
}

void
SNA::_onReadyRead() {
  _buffer.append(_port.readAll());
  if (0 == _buffer.size()) { return; }

  if (IDLE == _mode) {
    LogMessage msg(LOG_WARNING);
    msg << "Unexpected data received from the device! (n=" << _buffer.size() << ")";
    Logger::get().log(msg);
    _buffer.clear();
    return;
  }

  if (GET_VALUE == _mode) {
    if (0x00 != _buffer[0]) {
      LogMessage msg(LOG_WARNING);
      msg << "Get value: Device returned error: " << std::hex << uint16_t(((uint8_t *)_buffer.data())[0]);
      Logger::get().log(msg);
      _mode = IDLE; _buffer.clear();
      emit error();
      return;
    }
    // If value is complete
    if (3 > _buffer.size()) { return; }
    // Compute dBm from value
    double val = (uint16_t( ((uint8_t *)_buffer.data())[1] )<<8) +
        uint16_t( ((uint8_t *)_buffer.data())[2] );
    val = ((2.2*val/(1<<16))/25e-3) - 84.0;
    // Remove response from buffer
    _buffer.remove(0, 3);
    _mode = IDLE;
    emit valueReceived(val);
    return;
  }

  if (SET_FREQUENCY == _mode) {
    if (0x00 != _buffer[0]) {
      LogMessage msg(LOG_WARNING);
      msg << "Set frequency: Device returned error: " << std::hex << uint16_t(((uint8_t *)_buffer.data())[0]);
      Logger::get().log(msg);
      _mode = IDLE; _buffer.clear();
      emit error();
      return;
    }
    // consume status byte:
    _buffer.remove(0, 1);
    _mode = IDLE;
    // signal success
    emit frequencySet();
    return;
  }

  if (SHUTDOWN == _mode) {
    if (0x00 != _buffer.at(0)) {
      LogMessage msg(LOG_WARNING);
      msg << "Shutdown: Device returned error: " << std::hex << uint16_t(((uint8_t *)_buffer.data())[0]);
      Logger::get().log(msg);
      _mode = IDLE; _buffer.clear();
      emit error();
      return;
    }
    // consume status byte:
    _buffer.remove(0, 1);
    _mode = IDLE;
    emit deviceShutdown();
    return;
  }
}
