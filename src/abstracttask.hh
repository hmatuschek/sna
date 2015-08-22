#ifndef ABSTRACTTASK_HH
#define ABSTRACTTASK_HH

#include "sna.hh"
#include <QObject>


/** Base class of tasks being performed in a separate thread. */
class AbstractTask: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractTask(SNA &sna, QObject *parent = 0);

public:
  /** Destructor. */
  virtual ~AbstractTask();

public slots:
  void start();

signals:
  /** Gets emitted on progress. */
  void progress(double value);
  void finished();

protected:
  /** A weak reference to the SNA instance. */
  SNA &_sna;
};

#endif // ABSTRACTTASK_HH
