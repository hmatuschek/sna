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


class ScanPanel : public QWidget
{
  Q_OBJECT

public:
  explicit ScanPanel(SNA &sna, QWidget *parent = 0);

signals:
  void started();
  void stopped();

protected slots:
  void onStart(bool enabled);
  void onCalibrate(bool enabled);
  void onTaskFinished();
  void onProgress(double prog);
  void onSave();
  void onReset();

protected:
  SNA &_sna;
  ScanTask _scan;

  Plot *_plot;

  QDoubleSpinBox *_Fmin;
  QDoubleSpinBox *_Fmax;
  QSpinBox *_steps;
  QSpinBox *_delay;

  QProgressBar *_progress;

  QPushButton *_calibrate;
  QPushButton *_reset;
  QPushButton *_save;
  QPushButton *_start;
};

#endif
