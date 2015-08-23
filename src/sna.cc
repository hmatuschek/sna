#include "sna.hh"
#include "logger.hh"
#include <limits>

SNA::SNA(const QString &portname, double Fosc, double ppm, QObject *parent)
  : QObject(parent), _settings("com.github.hmatuschek", "sna"), _port(portname),
    _Fosc(Fosc), _ppm(ppm), _timeout()
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

  // Setup timeout timer
  _timeout.setInterval(1000);
  _timeout.setSingleShot(true);

  QObject::connect(&_port, SIGNAL(bytesWritten(qint64)), this, SLOT(_onBytesWritten(qint64)));
  QObject::connect(&_port, SIGNAL(readyRead()), this, SLOT(_onReadyRead()));
  QObject::connect(&_timeout, SIGNAL(timeout()), this, SLOT(_onTimeout()));
}


double
SNA::Fosc() const {
  return _Fosc;
}

bool
SNA::sendGetValue() {
  if (IDLE != _mode) { return false; }
  uint8_t tx[1] = {0x01};
  _mode = GET_VALUE;
  return (1 == _port.write((char *)tx, 1));
}

bool
SNA::sendSetFrequency(double f) {
  if (IDLE != _mode) { return false; }
  uint32_t n = (0xffffffff * f * (1+_ppm/1e6) / _Fosc);
  uint8_t tx[5] = { 0x02, uint8_t(n>>24), uint8_t(n>>16), uint8_t(n>>8), uint8_t(n) };
  _mode = SET_FREQUENCY;
  return (5 == _port.write((char *)tx, 5));
}

bool
SNA::sendShutdown() {
  if (IDLE != _mode) { return false; }
  uint8_t tx[1] = { 0x04 };
  _mode = SHUTDOWN;
  return (1 == _port.write((char *)tx, 1));
}

QSettings &
SNA::settings() {
  return _settings;
}

void
SNA::_onBytesWritten(qint64 n) {
  _timeout.start();
}

void
SNA::_onReadyRead() {
  _buffer.append(_port.readAll());
  if (0 == _buffer.size()) { return; }

  // Stop timer
  _timeout.stop();

  // Dispatch by mode

  if (IDLE == _mode) {
    LogMessage msg(LOG_WARNING);
    msg << "Unexpected data received from the device! (n=" << _buffer.size() << ")";
    Logger::get().log(msg);
    _buffer.clear(); _timeout.stop();
  } else if (GET_VALUE == _mode) {
    if (0x00 != _buffer[0]) {
      LogMessage msg(LOG_WARNING);
      msg << "Get value: Device returned error: " << std::hex << uint16_t(((uint8_t *)_buffer.data())[0]);
      Logger::get().log(msg);
      _mode = IDLE; _buffer.clear();
      emit error();
      return;
    }
    // If value is incomplete
    if (3 > _buffer.size()) {
      // restart timeout timer
      _timeout.start();
      return;
    }
    // Compute dBm from value
    double val = (uint16_t( ((uint8_t *)_buffer.data())[1] )<<8) +
        uint16_t( ((uint8_t *)_buffer.data())[2] );
    val = ((2.2*val/(1<<16))/25e-3) - 84.0;
    // Remove response from buffer
    _buffer.remove(0, 3);
    _mode = IDLE;
    emit valueReceived(val);
  } else if (SET_FREQUENCY == _mode) {
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
  } else if (SHUTDOWN == _mode) {
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
  }
}

void
SNA::_onTimeout() {
  _mode = IDLE; _buffer.clear();
  LogMessage msg(LOG_WARNING);
  msg << "SNA: IO timeout.";
  Logger::get().log(msg);
  emit error();
}
