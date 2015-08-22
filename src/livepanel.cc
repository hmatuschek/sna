#include "livepanel.hh"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>
#include "logger.hh"


PollPanel::PollPanel(SNA &sna, QWidget *parent) :
  QWidget(parent), _sna(sna), _Fupdate(true), _oscEnabled(true), _pwrEnabled(true), _timer()
{
  double F = _sna.settings().value("poll/F", 1e6).toDouble();
  unsigned int period = _sna.settings().value("poll/period", 500).toUInt();

  QFont font("sans");
  font.setFixedPitch(true);
  font.setPointSize(24);

  _dBm = new QLabel(tr("--- dBm"));
  _dBm->setFont(font);
  _dBm->setAlignment(Qt::AlignCenter);

  QString label;
  if (F < 1e3) {
    label = tr("%1 Hz").arg(F);
  } else if(F < 1e6) {
    label = tr("%1 kHz").arg(F/1e3);
  } else {
    label = tr("%1 MHz").arg(F/1e6);
  }
  _F = new QLabel(label);
  _F->setFont(font);
  _F->setAlignment(Qt::AlignCenter);

  _mode = new QComboBox();
  _mode->addItem(tr("Normal"));
  _mode->addItem(tr("Power only"));
  _mode->addItem(tr("Oscillator only"));

  _freq = new QDoubleSpinBox();
  _freq->setRange(0, _sna.Fosc()/2e3);
  _freq->setSingleStep(1);
  _freq->setValue(F/1e3);

  _period = new QSpinBox();
  _period->setRange(100, 2000);
  _period->setSingleStep(100);
  _period->setValue(period);

  _start = new QPushButton(tr("Start"));
  _start->setCheckable(true);
  _start->setChecked(false);

  _timer.setInterval(_period->value());
  _timer.setSingleShot(true);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_dBm, 1);
  layout->addWidget(_F, 1);
  layout->addWidget(new QWidget(), 1);
  QHBoxLayout *form = new QHBoxLayout();
  QLabel *flabel = new QLabel(tr("Mode"));
  flabel->setBuddy(_mode); form->addWidget(flabel); form->addWidget(_mode);
  flabel = new QLabel(tr("Frequency (kHz)"));
  flabel->setBuddy(_freq); form->addWidget(flabel); form->addWidget(_freq);
  flabel = new QLabel(tr("Period (ms)"));
  flabel->setBuddy(_period); form->addWidget(flabel); form->addWidget(_period);
  layout->addLayout(form, 0);
  layout->addWidget(_start, 0);

  setLayout(layout);

  QObject::connect(_mode, SIGNAL(currentIndexChanged(int)), SLOT(_onModeSelected(int)));
  QObject::connect(_start, SIGNAL(toggled(bool)), this, SLOT(_onStartToggled(bool)));
  QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(_onUpdate()));
  QObject::connect(_period, SIGNAL(valueChanged(int)), this, SLOT(_onPeriodChanged(int)));
  QObject::connect(_freq, SIGNAL(valueChanged(double)), this, SLOT(_onFrequencyChanged(double)));
}


void
PollPanel::_onModeSelected(int idx) {
  if (0 == idx) {
    _oscEnabled = true;
    _pwrEnabled = true;
    _Fupdate    = true;
  } else if (1 == idx) {
    _oscEnabled = false;
    _pwrEnabled = true;
    // disable oscillator immediately
    _sna.sendShutdown();
  } else if (2 == idx) {
    _oscEnabled = true;
    _pwrEnabled = false;
    _Fupdate    = true;
  }
}

void
PollPanel::_onStartToggled(bool enabled) {
  if (enabled) {
    QObject::connect(&_sna, SIGNAL(frequencySet()), this, SLOT(_onFrequencySet()));
    QObject::connect(&_sna, SIGNAL(valueReceived(double)), this, SLOT(_onValueReceived(double)));
    QObject::connect(&_sna, SIGNAL(error()), this, SLOT(_onError()));
    emit started();
    _timer.start();
    _start->setText(tr("stop"));
  } else {
    emit stopped();
    QObject::disconnect(&_sna, SIGNAL(frequencySet()), this, SLOT(_onFrequencySet()));
    QObject::disconnect(&_sna, SIGNAL(valueReceived(double)), this, SLOT(_onValueReceived(double)));
    QObject::disconnect(&_sna, SIGNAL(error()), this, SLOT(_onError()));
    _timer.stop();
    _start->setText(tr("start"));
  }
}

void
PollPanel::_onUpdate() {
  if (_Fupdate && _oscEnabled) {
    _Fupdate = false;
    _sna.sendSetFrequency(_freq->value()*1e3);
  } else if (_pwrEnabled) {
    _sna.sendGetValue();
  }
}

void
PollPanel::_onPeriodChanged(int value) {
  _sna.settings().setValue("poll/period", value);
  _timer.setInterval(value);
}

void
PollPanel::_onFrequencyChanged(double value) {
  value *= 1e3;
  _sna.settings().setValue("poll/F", value);
  if (value < 1e3) {
    _F->setText(tr("%1 Hz").arg(value));
  } else if (value < 1e6) {
    _F->setText(tr("%1 kHz").arg(value/1e3));
  } else {
    _F->setText(tr("%1 MHz").arg(value/1e6));
  }
  _Fupdate = true;
}

void
PollPanel::_onFrequencySet() {
  if (_pwrEnabled) {
    _sna.sendGetValue();
  } else if (_start->isChecked()) {
    _timer.start();
  }

}

void
PollPanel::_onValueReceived(double value) {
  _dBm->setText(tr("%1 dBm").arg(QString::number(value, 'f', 1)));
  if (_start->isChecked()) {
    _timer.start();
  }
}

void
PollPanel::_onError() {
  if (_start->isChecked()) {
    _timer.start();
  }
}
