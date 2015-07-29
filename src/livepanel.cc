#include "livepanel.hh"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>

PollPanel::PollPanel(SNA &sna, QWidget *parent) :
  QWidget(parent), _sna(sna), _timer()
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

  _sna.setFrequency(F);
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

  _timer.setSingleShot(true);
  _timer.setInterval(_period->value());

  QVBoxLayout *layout = new QVBoxLayout();
  QFormLayout *form = new QFormLayout();
  layout->addWidget(_dBm, 1);
  layout->addWidget(_F, 1);
  layout->addWidget(new QWidget(), 1);
  form->addRow(tr("Frequency (kHz)"), _freq);
  form->addRow(tr("Period (ms)"), _period);
  layout->addLayout(form, 0);
  layout->addWidget(_start, 0);

  setLayout(layout);

  QObject::connect(_start, SIGNAL(toggled(bool)), this, SLOT(onStartToggled(bool)));
  QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(onUpdate()));
  QObject::connect(_period, SIGNAL(valueChanged(int)), this, SLOT(onPeriodChanged(int)));
  QObject::connect(_freq, SIGNAL(valueChanged(double)), this, SLOT(onFrequencyChanged(double)));
}


void
PollPanel::onStartToggled(bool enabled) {
  if (enabled) {
    emit started();
    _timer.start();
    _start->setText(tr("stop"));
  } else {
    emit stopped();
    _timer.stop();
    _start->setText(tr("start"));
  }
}

void
PollPanel::onUpdate() {
  double val = _sna.value();
  if (val != val) { _dBm->setText(tr("--- dBm")); }
  else { _dBm->setText(tr("%1 dBm").arg(QString::number(val, 'g', 4))); }
  _timer.start();
}

void
PollPanel::onPeriodChanged(int value) {
  _sna.settings().setValue("poll/period", value);
  _timer.setInterval(value);
}

void
PollPanel::onFrequencyChanged(double value) {
  value *= 1e3;
  _sna.settings().setValue("poll/F", value);
  _sna.setFrequency(value);
  if (value < 1e3) {
    _F->setText(tr("%1 Hz").arg(value));
  } else if (value < 1e6) {
    _F->setText(tr("%1 kHz").arg(value/1e3));
  } else {
    _F->setText(tr("%1 MHz").arg(value/1e6));
  }
}
