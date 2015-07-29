#ifndef LIVEPANEL_HH
#define LIVEPANEL_HH

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDoubleSpinBox>

#include "sna.hh"


class PollPanel : public QWidget
{
  Q_OBJECT

public:
  explicit PollPanel(SNA &sna, QWidget *parent = 0);

signals:
  void started();
  void stopped();

protected slots:
  void onStartToggled(bool enabled);
  void onUpdate();
  void onPeriodChanged(int value);
  void onFrequencyChanged(double value);

protected:
  SNA &_sna;
  QLabel *_dBm;
  QLabel *_F;
  QDoubleSpinBox *_freq;
  QSpinBox *_period;
  QPushButton *_start;
  QTimer _timer;
};

#endif // LIVEPANEL_HH
