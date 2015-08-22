#include "scanpanel.hh"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include "logger.hh"
#include <QFileDialog>


ScanPanel::ScanPanel(SNA &sna, QWidget *parent)
  : QWidget(parent), _sna(sna), _scan(sna), _calibrationNeeded(true)
{
  double Fmin = _sna.settings().value("scan/Fmin", 1e3).toDouble();
  double Fmax = _sna.settings().value("scan/Fmax", 2e3).toDouble();
  unsigned int Nstep = _sna.settings().value("scan/steps", 100).toUInt();
  unsigned int delay = _sna.settings().value("scan/delay", 100).toUInt();

  _scan.setRange(Fmin*1e3, Fmax*1e3, Nstep);
  _scan.setDelay(delay);

  QLabel *label = new QLabel(tr("Settings"));
  QFont labelFont("sans"); labelFont.setPointSize(18);
  label->setFont(labelFont);

  _Fmin = new QDoubleSpinBox();
  _Fmin->setRange(0, _sna.Fosc()/2000);
  _Fmin->setSingleStep(100);
  _Fmin->setValue(Fmin);

  _Fmax = new QDoubleSpinBox();
  _Fmax->setRange(0, _sna.Fosc()/2000);
  _Fmax->setSingleStep(100);
  _Fmax->setValue(Fmax);

  _steps = new QSpinBox();
  _steps->setRange(10, 10000);
  _steps->setSingleStep(10);
  _steps->setValue(Nstep);

  _delay = new QSpinBox();
  _delay->setRange(0, 1000);
  _delay->setSingleStep(10);
  _delay->setValue(delay);

  _progress = new QProgressBar();
  _progress->setRange(0, 100);
  _progress->setValue(100);

  _calibrate = new QPushButton(tr("calibrate"));
  _calibrate->setCheckable(true);
  _calibrate->setChecked(false);

  _start = new QPushButton(tr("start"));
  _start->setCheckable(true);
  _start->setChecked(false);

  _reset = new QPushButton(tr("reset"));
  _save = new QPushButton(tr("save"));
  _save->setEnabled(false);

  _plot = new Plot();

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(3);
  layout->addWidget(_plot, 1);

  QVBoxLayout *left = new QVBoxLayout();
  left->setContentsMargins(3,3,3,3);
  left->setSpacing(5);
  left->addWidget(label, 0);

  QFormLayout *form = new QFormLayout();
  form->setContentsMargins(5,5,5,5);
  form->addRow(tr("min. freq. (kHz)"), _Fmin);
  form->addRow(tr("max. freq. (kHz)"), _Fmax);
  form->addRow(tr("steps"), _steps);
  form->addRow(tr("delay (ms)"), _delay);
  left->addLayout(form, 1);
  left->addWidget(_progress, 0);

  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->setContentsMargins(0,0,0,0);
  bbox->setSpacing(5);
  bbox->addWidget(_calibrate, 1);
  bbox->addWidget(_reset, 0);
  left->addLayout(bbox, 0);

  bbox = new QHBoxLayout();
  bbox->setContentsMargins(0,0,0,0);
  bbox->setSpacing(5);
  bbox->addWidget(_start, 1);
  bbox->addWidget(_save, 0);
  left->addLayout(bbox, 0);

  layout->addLayout(left, 0);
  setLayout(layout);

  QObject::connect(_calibrate, SIGNAL(toggled(bool)), this, SLOT(onCalibrate(bool)));
  QObject::connect(_reset, SIGNAL(clicked()), this, SLOT(onReset()));
  QObject::connect(_start, SIGNAL(toggled(bool)), this, SLOT(onStart(bool)));
  QObject::connect(_save, SIGNAL(clicked()), this, SLOT(onSave()));
  QObject::connect(&_scan, SIGNAL(finished()), this, SLOT(onTaskFinished()));
  QObject::connect(&_scan, SIGNAL(progress(double)), this, SLOT(onProgress(double)));

  QObject::connect(_Fmin, SIGNAL(valueChanged(double)), this, SLOT(onCalibrationNeeded()));
  QObject::connect(_Fmax, SIGNAL(valueChanged(double)), this, SLOT(onCalibrationNeeded()));
  QObject::connect(_steps, SIGNAL(valueChanged(int)), this, SLOT(onCalibrationNeeded()));
}


void
ScanPanel::onStart(bool enabled) {
  if (! enabled) { return; }

  // Save config
  _sna.settings().setValue("scan/Fmin", _Fmin->value());
  _sna.settings().setValue("scan/Fmax", _Fmax->value());
  _sna.settings().setValue("scan/steps", _steps->value());
  _sna.settings().setValue("scan/delay", _delay->value());

  // Signal start
  emit started();

  // disable start button
  _start->setEnabled(false);
  _calibrate->setEnabled(false);
  // reset progress bar
  _progress->setValue(0);

  // Update scan task
  if (_calibrationNeeded) {
    _scan.setRange(_Fmin->value()*1e3, _Fmax->value()*1e3, _steps->value());
    _scan.setDelay(_delay->value());
  }

  // go
  _scan.scan();
}

void
ScanPanel::onCalibrate(bool enabled) {
  if (! enabled) { return; }

  // Save config
  _sna.settings().setValue("scan/Fmin", _Fmin->value());
  _sna.settings().setValue("scan/Fmax", _Fmax->value());
  _sna.settings().setValue("scan/steps", _steps->value());
  _sna.settings().setValue("scan/delay", _delay->value());

  // Signal start
  emit started();

  // disable start button
  _start->setEnabled(false);
  _calibrate->setEnabled(false);
  // reset progress bar
  _progress->setValue(0);

  // Update scan task
  _scan.setRange(_Fmin->value()*1e3, _Fmax->value()*1e3, _steps->value());
  _scan.setDelay(_delay->value());
  _calibrationNeeded = false;

  // go
  _scan.calibrate();
}


void
ScanPanel::onTaskFinished() {
  emit stopped();
  if (_start->isChecked()) {
    _plot->plot(_scan.F(), _scan.dBm());
    _save->setEnabled(true);
  }
  _start->setEnabled(true);
  _start->setChecked(false);
  _calibrate->setEnabled(true);
  _calibrate->setChecked(false);
  _progress->setValue(100);
  // disable oscillator.
  _sna.sendShutdown();
}

void
ScanPanel::onProgress(double prog) {
  _progress->setValue(prog*100);
}

void
ScanPanel::onSave() {
  QString filename =
      QFileDialog::getSaveFileName(0, tr("Save scan ..."), "", "CSV (*.csv)");
  if (0 == filename.size()) { return; }
  _scan.save(filename);
}

void
ScanPanel::onReset() {
  _scan.reset();
  _calibrationNeeded = true;
  _save->setEnabled(false);
  _plot->clear();
}

void
ScanPanel::onCalibrationNeeded() {
  _calibrationNeeded = true;
}
