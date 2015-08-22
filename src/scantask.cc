#include "scantask.hh"
#include "logger.hh"
#include <cmath>
#include <QFile>
#include <QTextStream>


ScanTask::ScanTask(SNA &sna, QObject *parent)
  : AbstractTask(sna, parent), _F(), _dBm(), _baseline(), _pauseTimer(), _delayTimer()
{
  _pauseTimer.setInterval(10);
  _pauseTimer.setSingleShot(true);

  _delayTimer.setInterval(0);
  _delayTimer.setSingleShot(true);

  QObject::connect(&_pauseTimer, SIGNAL(timeout()), this, SLOT(_onSetFrequency()), Qt::QueuedConnection);
  QObject::connect(&_delayTimer, SIGNAL(timeout()), this, SLOT(_onReceiveValue()), Qt::QueuedConnection);
}

ScanTask::~ScanTask() {
  // pass...
}

unsigned int
ScanTask::delay() const {
  return _delayTimer.interval();
}

void
ScanTask::setDelay(unsigned int delay) {
  _delayTimer.setInterval(delay);
}

void
ScanTask::setRange(double Fmin, double Fmax, size_t Nstep) {
  LogMessage msg(LOG_DEBUG);
  msg << "Reset scan range: " << Fmin << ":" << Fmax << " @ " << Nstep;
  Logger::get().log(msg);
  if (2 > Nstep) { _F.clear(); _dBm.clear(); return; }
  _F.resize(Nstep); _dBm.resize(Nstep); _baseline.resize(Nstep);
  double F = std::min(Fmin, Fmax);
  double dF = (std::max(Fmax, Fmin) - std::min(Fmin, Fmax))/(steps()-1);
  for (size_t i=0; i<steps(); i++, F+=dF) {
    _F[i] = F; _dBm[i] = 0; _baseline[i] = 0;
  }
}

void
ScanTask::scan() {
  QObject::connect(&_sna, SIGNAL(frequencySet()), this, SLOT(_onFrequencySet()), Qt::QueuedConnection);
  QObject::connect(&_sna, SIGNAL(valueReceived(double)), this, SLOT(_onValueReceived(double)), Qt::QueuedConnection);
  QObject::connect(&_sna, SIGNAL(error()), this, SLOT(_onError()), Qt::QueuedConnection);
  _mode = SCAN; _currentIndex = 0;
  _pauseTimer.start();
}

void
ScanTask::calibrate() {
  QObject::connect(&_sna, SIGNAL(frequencySet()), this, SLOT(_onFrequencySet()), Qt::QueuedConnection);
  QObject::connect(&_sna, SIGNAL(valueReceived(double)), this, SLOT(_onValueReceived(double)), Qt::QueuedConnection);
  QObject::connect(&_sna, SIGNAL(error()), this, SLOT(_onError()), Qt::QueuedConnection);
  _mode = CALIBRATE; _currentIndex = 0;
  _pauseTimer.start();
}

ScanTask::Mode
ScanTask::mode() const {
  return _mode;
}

void
ScanTask::reset() {
  for (size_t i=0; i<steps(); i++) {
    _dBm[i] = 0; _baseline[i] = 0;
  }
}

void
ScanTask::save(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadWrite)) {
    LogMessage msg(LOG_ERROR);
    msg << "Can not save scan to '" << filename.toStdString() << "'.";
    Logger::get().log(msg);
    return;
  }

  {
    QTextStream stream(&file);
    for (size_t i=0; i<_F.size(); i++) {
      stream << _F[i] << '\t' << _dBm[i] << "\t" << _baseline[i] << "\n";
    }
    stream.flush();
  }
}


void
ScanTask::_onSetFrequency() {
  if (_F.size() == _currentIndex) { return; };
  _sna.sendSetFrequency(_F[_currentIndex]);
}

void
ScanTask::_onFrequencySet() {
  _delayTimer.start();
}

void
ScanTask::_onReceiveValue() {
  _sna.sendGetValue();
}

void
ScanTask::_onValueReceived(double val) {
  if (SCAN == _mode) {
    _dBm[_currentIndex] = val - _baseline[_currentIndex];
  } else if (CALIBRATE == _mode) {
    _baseline[_currentIndex] = val;
  }
  _currentIndex++;
  emit progress(double(_currentIndex)/_F.size());
  if (_currentIndex == _F.size()) {
    QObject::disconnect(&_sna, SIGNAL(frequencySet()), this, SLOT(_onFrequencySet()));
    QObject::disconnect(&_sna, SIGNAL(valueReceived(double)), this, SLOT(_onValueReceived(double)));
    QObject::disconnect(&_sna, SIGNAL(error()), this, SLOT(_onError()));
    _mode = IDLE;
    emit finished();
  } else {
    _pauseTimer.start();
  }
}

void
ScanTask::_onError() {
  _pauseTimer.start();
}
