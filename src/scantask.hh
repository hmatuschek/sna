#ifndef SCANTASK_HH
#define SCANTASK_HH

#include "sna.hh"
#include "abstracttask.hh"


class ScanTask: public AbstractTask
{
  Q_OBJECT

public:
  typedef enum {
    SCAN, CALIBRATE
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
  void reset();

  void scan();
  void calibrate();
  Mode mode() const;

protected:
  void run();

protected:
  unsigned int _delay;
  Mode _mode;
  std::vector<double> _F;
  std::vector<double> _dBm;
  std::vector<double> _baseline;
};


#endif // SCANTASK_HH
