#include "abstracttask.hh"

AbstractTask::AbstractTask(SNA &sna, QObject *parent)
  : QObject(parent), _sna(sna)
{
  // pass...
}

AbstractTask::~AbstractTask() {
  // pass...
}

void
AbstractTask::start() {
  // pass...
}
