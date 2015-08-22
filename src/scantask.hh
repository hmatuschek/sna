#ifndef SCANTASK_HH
#define SCANTASK_HH

#include "sna.hh"
#include "vector"
#include "abstracttask.hh"
#include <QTimer>

class ScanTask: public AbstractTask
{
  Q_OBJECT

public:
  typedef enum {
    IDLE, SCAN, CALIBRATE
  } Mode;

public:
  ScanTask(SNA &sna, QObject *parent=0);
  virtual ~ScanTask();

  inline const std::vector<double> &F() const { return _F; }
  inline double Fmin() const { return _F.front(); }
  inline double Fmax() const { return _F.back(); }
  inline const std::vector<double> &dBm() const { return _dBm; }
  inline const std::vector<double> &baseline() const { return _baseline; }
  inline size_t steps() const { return _F.size(); }

  unsigned int delay() const;
  void setDelay(unsigned int delay);
  void setRange(double Fmin, double Fmax, size_t Nstep);
  void save(const QString &filename);

  void scan();
  void calibrate();
  void reset();
  Mode mode() const;

protected slots:
  void _onSetFrequency();
  void _onFrequencySet();
  void _onReceiveValue();
  void _onValueReceived(double val);
  void _onError();

protected:
  Mode _mode;
  size_t _currentIndex;
  std::vector<double> _F;
  std::vector<double> _dBm;
  std::vector<double> _baseline;
  QTimer _pauseTimer;
  QTimer _delayTimer;
};


#endif // SCANTASK_HH
