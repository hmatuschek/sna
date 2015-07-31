#ifndef LIVEPANEL_HH
#define LIVEPANEL_HH

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDoubleSpinBox>

#include "sna.hh"


/** Control panel for the polling. This allows to set the frequency of the
 * oscillator and read the incomming amplitude periodically. */
class PollPanel : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit PollPanel(SNA &sna, QWidget *parent = 0);

signals:
  /** Gets emitted on start of the "analysis". */
  void started();
  /** Gets emitted at the end of the "analysis". */
  void stopped();

protected slots:
  /** Callback for the start/stop button. */
  void onStartToggled(bool enabled);
  /** Callback for the update timer. */
  void onUpdate();
  /** Callback for the update periode selection. */
  void onPeriodChanged(int value);
  /** Callback for the frequency selection. */
  void onFrequencyChanged(double value);

protected:
  /** A weak reference to the SNA instance. */
  SNA &_sna;
  /** Label displaying the input amplitude. */
  QLabel *_dBm;
  /** Label displaying the output frequency. */
  QLabel *_F;
  /** Frequency selection. */
  QDoubleSpinBox *_freq;
  /** Update-period selection. */
  QSpinBox *_period;
  /** Start/stop button. */
  QPushButton *_start;
  /** Update timer. */
  QTimer _timer;
};

#endif // LIVEPANEL_HH
