#ifndef SCANPANEL_HH
#define SCANPANEL_HH

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QProgressBar>

#include "sna.hh"
#include "scantask.hh"
#include "qcustomplot.hh"


/** Scan control widget.
 * This class implements a view for the @c ScanTask model. */
class ScanPanel : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ScanPanel(SNA &sna, QWidget *parent = 0);

signals:
  /** Gets emitted on start of the task. */
  void started();
  /** Gets emitted at the end of the task. */
  void stopped();

protected slots:
  /** Start button callback. */
  void onStart(bool enabled);
  /** Calibrate button callback. */
  void onCalibrate(bool enabled);
  /** Gets called at the end of the scan task. */
  void onTaskFinished();
  /** Gets called on task progress. */
  void onProgress(double prog);
  /** Save button callback. */
  void onSave();
  /** Clear button callback. */
  void onClear();
  /** Reset button callback. */
  void onReset();
  /** Gets called if frequency range or step size is changed. */
  void onCalibrationNeeded();
  void onClick(QMouseEvent *evt);
  void reset3dBMeasurement();
  void update3dBMeasurement(QCPGraph *graph);

protected:
  /** Reference of the SNA object. */
  SNA &_sna;
  /** The scan task object. */
  ScanTask _scan;
  /** The plot window. */
  QCustomPlot *_plot;
  /** A tracer to highlight a certain data-point. */
  QCPItemTracer *_tracer;
  QCPItemText *_tracerLabel;
  QCPItemTracer *_bwMax;
  QCPItemText   *_bwMaxText;
  QCPItemTracer *_bwLeft;
  QCPItemText   *_bwLeftText;
  QCPItemTracer *_bwRight;
  QCPItemText   *_bwRightText;
  QCPItemLine   *_bwLine;
  QCPItemText   *_bwText;
  /** F min selection. */
  QDoubleSpinBox *_Fmin;
  /** F max selection. */
  QDoubleSpinBox *_Fmax;
  /** Scan steps selection. */
  QSpinBox *_steps;
  /** Scan step delay selection. */
  QSpinBox *_delay;
  /** The progress bar. */
  QProgressBar *_progress;
  /** Calibrate button. */
  QPushButton *_calibrate;
  /** Clear button. */
  QPushButton *_clear;
  /** Reset button. */
  QPushButton *_reset;
  /** Start button. */
  QPushButton *_start;
  /** Save button. */
  QPushButton *_save;
  /** If true, a recalibration is needed. */
  bool _calibrationNeeded;
};

#endif
