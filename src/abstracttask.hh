#ifndef ABSTRACTTASK_HH
#define ABSTRACTTASK_HH

#include "sna.hh"
#include <QObject>
#include <QThread>


class AbstractTask : public QThread
{
  Q_OBJECT

public:
  explicit AbstractTask(SNA &sna, QObject *parent = 0);
  virtual ~AbstractTask();

signals:
  void progress(double value);

protected:
  SNA &_sna;
};

#endif // ABSTRACTTASK_HH
