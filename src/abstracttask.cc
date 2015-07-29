#include "abstracttask.hh"

AbstractTask::AbstractTask(SNA &sna, QObject *parent)
  : QThread(parent), _sna(sna)
{
  // pass...
}

AbstractTask::~AbstractTask() {
  // pass...
}
