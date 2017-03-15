#include "scanpanel.hh"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include "logger.hh"
#include <QFileDialog>
#include <QDebug>


QList<QColor> defaultColors = QList<QColor>()
    << QColor(0, 0, 125) << QColor(125, 0, 0) << QColor(0, 125, 0) << QColor(125, 125, 0)
    << QColor(0, 125, 125) << QColor(125, 0, 125) << QColor(205, 79, 18) << QColor(255, 185, 24)
    << QColor(243, 250, 146) << QColor(105, 151, 102) << QColor(69, 47, 96)
    << QColor(224, 26, 53) << QColor(204, 15, 19) << QColor(63, 61, 153) << QColor(153, 61, 113)
    << QColor(61, 153, 86) << QColor(61, 90, 153) << QColor(153, 61, 144) << QColor(61, 121, 153)
    << QColor(132, 61, 153) << QColor(153, 78, 61) << QColor(98, 153, 61) << QColor(61, 151, 153)
    << QColor(101, 61, 153) << QColor(153, 61, 75);


inline QString formatFrequency(double F) {
  if (F < 1e3) {
    return QString("%1Hz").arg(F, 0, 'f', 3);
  } else if (F < 1e6) {
    return QString("%1kHz").arg(F/1e3, 0, 'f', 3);
  }
  return QString("%1MHz").arg(F/1e6, 0, 'f', 3);
}


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

  _clear = new QPushButton(tr("clear"));
  _reset = new QPushButton(tr("reset"));
  _save = new QPushButton(tr("save"));
  _save->setEnabled(false);

  _plot = new QCustomPlot();
  _plot->xAxis->setLabel(tr("Frequency [kHz]"));
  _plot->yAxis->setLabel(tr("Power [dBm]"));
  _plot->setInteractions(QCP::iSelectPlottables);

  _tracer = new QCPItemTracer(_plot);
  _tracer->setStyle(QCPItemTracer::tsCrosshair);
  _tracer->setVisible(false);
  _tracerLabel = new QCPItemText(_plot);
  _tracerLabel->setVisible(false);
  _tracerLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
  _tracerLabel->setTextAlignment(Qt::AlignLeft);

  _bwMax = new QCPItemTracer(_plot);
  _bwMax->setStyle(QCPItemTracer::tsCircle);
  _bwMax->setVisible(false);
  _bwMaxText = new QCPItemText(_plot);
  _bwMaxText->setVisible(false);
  _bwMaxText->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  _bwMaxText->setTextAlignment(Qt::AlignLeft);

  _bwLeft = new QCPItemTracer(_plot);
  _bwLeft->setStyle(QCPItemTracer::tsPlus);
  _bwLeft->setVisible(false);
  _bwLeftText = new QCPItemText(_plot);
  _bwLeftText->setVisible(false);
  _bwLeftText->setPositionAlignment(Qt::AlignVCenter | Qt::AlignRight);

  _bwRight = new QCPItemTracer(_plot);
  _bwRight->setStyle(QCPItemTracer::tsPlus);
  _bwRight->setVisible(false);
  _bwRightText = new QCPItemText(_plot);
  _bwRightText->setVisible(false);
  _bwRightText->setPositionAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  _bwLine = new QCPItemLine(_plot);
  _bwLine->setHead(QCPLineEnding::esBar);
  _bwLine->setTail(QCPLineEnding::esBar);
  _bwLine->setVisible(false);
  _bwText = new QCPItemText(_plot);
  _bwText->setVisible(false);
  _bwText->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);

  // 2 Demo graphs:
  _plot->addGraph()->setPen(QPen(defaultColors[0]));
  _plot->addGraph()->setPen(QPen(defaultColors[1]));
  for (int i=0; i<100; i++) {
    double f = 10690e3+2.e2*i;
    double df = (f-10.7e6)/2e3;
    _plot->graph(0)->addData(f, -df*df);
    _plot->graph(1)->addData(f, df*df);
  }
  _plot->rescaleAxes();
  _plot->replot();


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
  bbox->addWidget(_clear, 0);
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
  QObject::connect(_clear, SIGNAL(clicked()), this, SLOT(onClear()));
  QObject::connect(_reset, SIGNAL(clicked()), this, SLOT(onReset()));
  QObject::connect(_start, SIGNAL(toggled(bool)), this, SLOT(onStart(bool)));
  QObject::connect(_save, SIGNAL(clicked()), this, SLOT(onSave()));
  QObject::connect(&_scan, SIGNAL(finished()), this, SLOT(onTaskFinished()));
  QObject::connect(&_scan, SIGNAL(progress(double)), this, SLOT(onProgress(double)));

  QObject::connect(_Fmin, SIGNAL(valueChanged(double)), this, SLOT(onCalibrationNeeded()));
  QObject::connect(_Fmax, SIGNAL(valueChanged(double)), this, SLOT(onCalibrationNeeded()));
  QObject::connect(_steps, SIGNAL(valueChanged(int)), this, SLOT(onCalibrationNeeded()));

  QObject::connect(_plot, SIGNAL(mousePress(QMouseEvent*)),
                   this, SLOT(onClick(QMouseEvent*)));
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
    QCPGraph *graph = _plot->addGraph();
    graph->setPen(QPen( defaultColors[(_plot->graphCount() % defaultColors.size())] ));
    graph->addData(_scan.F(), _scan.dBm());
    _save->setEnabled(true);
    _plot->replot();
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
ScanPanel::onClear() {
  reset3dBMeasurement();
  _tracer->setVisible(false);
  _tracerLabel->setVisible(false);
  _plot->clearGraphs();
  _plot->replot();
}

void
ScanPanel::onReset() {
  onClear();
  _scan.reset();
  _calibrationNeeded = true;
}

void
ScanPanel::onCalibrationNeeded() {
  _scan.reset();
  _calibrationNeeded = true;
}

void
ScanPanel::onClick(QMouseEvent *evt) {
  // Get plotable near click event
  QCPAbstractPlottable *plotable = _plot->plottableAt(evt->localPos(), true);
  if (0 == plotable) {
    _tracer->setVisible(false);
    _tracerLabel->setVisible(false);
    reset3dBMeasurement();
    return;
  }

  // ensure it is a graph
  QCPGraph *graph = qobject_cast<QCPGraph *>(plotable);
  if (0 == graph) {
    _tracer->setVisible(false);
    _tracerLabel->setVisible(false);
    reset3dBMeasurement();
    return;
  }

  // get key value at click event
  double key = _plot->xAxis->pixelToCoord(evt->localPos().x());
  _tracer->setGraph(graph);
  _tracer->setGraphKey(key);
  _tracer->setVisible(true);
  _tracer->updatePosition();
  _tracerLabel->setText(QString("%1\n%2dBm")
                        .arg(formatFrequency(key))
                        .arg(_tracer->position->value()));
  _tracerLabel->position->setPixelPosition(_tracer->position->pixelPosition());
  _tracerLabel->setVisible(true);

  // Update -3dB measurement
  update3dBMeasurement(graph);
}

void
ScanPanel::reset3dBMeasurement() {
  _bwMax->setVisible(false);
  _bwMaxText->setVisible(false);
  _bwLeft->setVisible(false);
  _bwLeftText->setVisible(false);
  _bwRight->setVisible(false);
  _bwRightText->setVisible(false);
  _bwLine->setVisible(false);
  _bwText->setVisible(false);
}

void
ScanPanel::update3dBMeasurement(QCPGraph *graph) {
  if ((0 == graph) || (0 == graph->dataCount()))
    return;

  // Update -3dB bandwidth around maximum ...

  // First, search for maximum
  int maxIdx = 0;
  double maxY = graph->dataMainValue(maxIdx);
  for (int i=1; i<graph->dataCount(); i++) {
    if (maxY < graph->dataMainValue(i)) {
      maxIdx = i; maxY = graph->dataMainValue(i);
    }
  }
  // If maximum is at the edges -> abort
  if ((0 == maxIdx) || ((graph->dataCount()-1) == maxIdx)) {
    reset3dBMeasurement();
    return;
  }

  // Find left & right -3dB points
  int leftIdx = maxIdx;
  while ((leftIdx>=0) && (graph->dataMainValue(leftIdx)>(maxY-3.0)))
    leftIdx--;
  // If left point is at the edge -> abort
  if (0 >= leftIdx) {
    reset3dBMeasurement();
    return;
  }

  int rightIdx = maxIdx;
  while ((rightIdx<graph->dataCount()) && (graph->dataMainValue(rightIdx)>(maxY-3.0)))
    rightIdx++;
  // If left point is at the edge -> abort
  if ((graph->dataCount()-1) <= rightIdx) {
    reset3dBMeasurement();
    return;
  }

  // Update maximum marker
  _bwMax->setGraph(graph);
  double center = graph->dataMainKey(maxIdx);
  _bwMax->setGraphKey(center);
  _bwMax->setVisible(true);
  _bwMaxText->setText(QString("Fc %1\nPc %2dBm")
                      .arg(formatFrequency(center))
                      .arg(maxY, 0, 'f', 2));
  _bwMaxText->position->setCoords(center, maxY);
  _bwMaxText->setVisible(true);

  // update left -3dB marker
  _bwLeft->setGraph(graph);
  double left = graph->dataMainKey(leftIdx);
  _bwLeft->setGraphKey(left);
  _bwLeft->setVisible(true);
  _bwLeftText->setText(formatFrequency(left));
  _bwLeftText->position->setCoords(left, maxY-3);
  _bwLeftText->setVisible(true);

  // update right -3dB marker
  _bwRight->setGraph(graph);
  double right = graph->dataMainKey(rightIdx);
  _bwRight->setGraphKey(right);
  _bwRight->setVisible(true);
  _bwRightText->setText(formatFrequency(right));
  _bwRightText->position->setCoords(right, maxY-3);
  _bwRightText->setVisible(true);

  // update BW line
  _bwLine->start->setCoords(left, maxY-3);
  _bwLine->end->setCoords(right, maxY-3);
  _bwLine->setVisible(true);

  // update and position text
  _bwText->setText(formatFrequency(right-left));
  _bwText->position->setCoords(left+(right-left)/2, maxY-3);
  _bwText->setVisible(true);
}
