#include "scantask.hh"
#include "logger.hh"
#include <cmath>
#include <QFile>
#include <QTextStream>


ScanTask::ScanTask(SNA &sna, QObject *parent)
  : AbstractTask(sna, parent), _delay(0), _F(), _dBm(), _baseline()
{
  // pass...
}

ScanTask::~ScanTask() {
  // pass...
}

void
ScanTask::run() {
  // Scan
  for (size_t i=0; i<_F.size(); i++) {
    if (0 < _delay) {
      _sna.setFrequency(_F[i]);
      msleep(_delay);
      if (SCAN == _mode) {
        //_dBm[i] = _sna.value()-_baseline[i];
        _dBm[i] = std::log(_F[i]);
      } else {
        _baseline[i] = _sna.value();
      }
    } else {
      if (SCAN == _mode) {
        //_dBm[i] = _sna.valueAt(_F[i])-_baseline[i];
        _dBm[i] = std::log(_F[i]);
      } else {
        _baseline[i] = _sna.valueAt(_F[i]);
      }
    }
    emit progress(double(i+1)/_F.size());
  }
}

unsigned int
ScanTask::delay() const {
  return _delay;
}

void
ScanTask::setDelay(unsigned int delay) {
  _delay = delay;
}

void
ScanTask::setRange(double Fmin, double Fmax, size_t Nstep) {
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
  _mode = SCAN;
  start();
}

void
ScanTask::calibrate() {
  _mode = CALIBRATE;
  start();
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

