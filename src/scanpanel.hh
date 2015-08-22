#ifndef SCANPANEL_HH
#define SCANPANEL_HH

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QProgressBar>

#include "sna.hh"
#include "scantask.hh"
#include "plot.hh"


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
  /** Setset button callback. */
  void onReset();
  /** Gets called if frequency range or step size is changed. */
  void onCalibrationNeeded();

protected:
  /** Reference of the SNA object. */
  SNA &_sna;
  /** The scan task object. */
  ScanTask _scan;
  /** The plot window. */
  Plot *_plot;
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
