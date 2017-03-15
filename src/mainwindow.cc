#include "mainwindow.hh"
#include <QVBoxLayout>
#include "aboutwidget.hh"

StatusLogHandler::StatusLogHandler(QStatusBar *status)
  : QObject(0), _status(status)
{
  connect(_status, SIGNAL(destroyed(QObject*)), this, SLOT(_onStatusDeleted()));
}

void
StatusLogHandler::handle(const LogMessage &msg) {
  if (0 == _status)
    return;

  QString buffer;
  QTextStream stream(&buffer);
  switch (msg.level()) {
    case LOG_DEBUG: stream << "DEBUG: "; break;
    case LOG_INFO: stream << "INFO: "; break;
    case LOG_WARNING: stream << "WARN: "; break;
    case LOG_ERROR: stream << "ERROR: "; break;
  }
  stream << QString::fromStdString(msg.str());

  _status->showMessage(buffer);
}

void
StatusLogHandler::_onStatusDeleted() {
  _status = 0;
}



MainWindow::MainWindow(SNA &sna, QWidget *parent) :
  QMainWindow(parent), _sna(sna), _status(0)
{
  setWindowTitle(tr("Scalar Network Analyzer"));

  _taskSelect = new QComboBox();
  _taskSelect->addItem(tr("Scan"));
  _taskSelect->addItem(tr("Poll"));
  _taskSelect->addItem(tr("About SNA"));

  _view = new QStackedWidget();
  _scan = new ScanPanel(_sna);
  _poll = new PollPanel(_sna);

  _view->addWidget(_scan);
  _view->addWidget(_poll);
  _view->addWidget(new AboutWidget());

  _status = this->statusBar();
  Logger::get().addHandler(new StatusLogHandler(_status));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_taskSelect, 0);
  layout->addWidget(_view, 1);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(5);

  QWidget *panel = new QWidget();
  panel->setLayout(layout);
  setCentralWidget(panel);

  QObject::connect(_taskSelect, SIGNAL(currentIndexChanged(int)), _view, SLOT(setCurrentIndex(int)));
  QObject::connect(_scan, SIGNAL(started()), this, SLOT(onTaskStarted()));
  QObject::connect(_scan, SIGNAL(stopped()), this, SLOT(onTaskStopped()));
  QObject::connect(_poll, SIGNAL(started()), this, SLOT(onTaskStarted()));
  QObject::connect(_poll, SIGNAL(stopped()), this, SLOT(onTaskStopped()));
}


void
MainWindow::onTaskStarted() {
  _taskSelect->setEnabled(false);
}

void
MainWindow::onTaskStopped() {
  _taskSelect->setEnabled(true);
}

